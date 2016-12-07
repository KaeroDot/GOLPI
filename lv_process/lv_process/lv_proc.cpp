//---------------------------------------------------------------------------------------------------------------------
// LV Process DLL
//---------------------------------------------------------------------------------------------------------------------
// Author: Stanislav Maslan
// E-mail: s.maslan@seznam.cz, smaslan@cmi.cz
// www: https://forums.ni.com/t5/Community-Documents/LV-Process-Windows-pipes-LabVIEW/tac-p/3497843/highlight/true
// Revision: V4.1, 2016-12-06
//
//
// LICENSE:
// --------
// Copyright (c) 2014 - 2016, Stanislav Maslan <s.maslan@seznam.cz>
//
// This file is part of the LV Process DLL.
//
// LV Process DLL interface is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LV Process DLL interface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LV Process DLL interface. If not, see <http://www.gnu.org/licenses/>.
//
//
//
// BRIEF DESCRIPTION:
// ------------------
// This library was developed for pipe communication with console application. It is basically 
// just a wrapper for nasty WINAPIs dedicated for pipe handling and process execution. The wrapper
// simplifies the whole operation to just a several functions with simple data types. So it can be 
// easily linked also to LabVIEW (that is why it is called "LV Process").
// 
// Usage it quite straightforward:
// 1) Call "proc_create()" to create instance of desired process with some setup.
//    This function will also create stdin and stdout pipes which are then used for communication with the process.
// 2) Call "proc_write_stdin()" to write string to the process' stdin.
// 3) Call "proc_peek_stdout()" to read process stdout (eventual response to 2), etc.).
// 4) Call "proc_command()" to write and read response at once.
// 5) Repeat from 2) and profit???
// 6) When you are done, terminate the process somehow. For instance send "exit\n" to stdin when you want to
//    to terminate "cmd.exe". Then call "proc_wait_exit()" to wait for process end. If it does not do so,
//    kill it with fire ... i mean "proc_terminate()". Eventually retrieve exit code by "proc_get_exit_code()".
// 
// There are several other functions exported to DLL so follow the header file for details. 
//
// The DLL also enables to create debug console. It is just a read console where you can check the stdin/stdout
// traffic. Some day I will maybe add keyboard input too.
// 
//
// lv_proc.ini:
//
//  If 'lv_proc.ini' config file is found in the folder with the 'lv_proc.dll' it will be used as a config file.
//  Supported options of the *.ini file:
//   [DEBUG]
//   ;enables logging to a "debug.log" file located in DLL's folder
//   debug_enabled = 0
//
//   [PIPES]
//   ;read and write pipe sizes (0: system decides)
//   write_pipe_buffer_size = 0
//   read_pipe_buffer_size = 0
//   
//   [READ]
//   ;read thread priority (0: normal, <-15,15> range possible)
//   thread_priority = +1
//   ;read thread idle time if no data in read pipe (1 to 100 ms)
//   thread_idle_time = 1
//   
//   [CONSOLE]
//   ;always create console (1 - overides proc_create(..., hide) parameter)
//   no_hide = 0
//   
//   ;console buffer sizes (use -1 for default, max 32767)
//   buf_size_x = 100
//   buf_size_y = 8192
//   
//   ;=== console text color scheme ===
//   ;Use standard text attribute flags:
//   ; FOREGROUND_BLUE            Text color contains blue. 
//   ; FOREGROUND_GREEN           Text color contains green. 
//   ; FOREGROUND_RED             Text color contains red. 
//   ; FOREGROUND_INTENSITY       Text color is intensified. 
//   ; BACKGROUND_BLUE            Background color contains blue. 
//   ; BACKGROUND_GREEN           Background color contains green. 
//   ; BACKGROUND_RED             Background color contains red. 
//   ; BACKGROUND_INTENSITY       Background color is intensified. 
//   ; COMMON_LVB_LEADING_BYTE    Leading byte. 
//   ; COMMON_LVB_TRAILING_BYTE   Trailing byte. 
//   ; COMMON_LVB_GRID_HORIZONTAL Top horizontal. 
//   ; COMMON_LVB_GRID_LVERTICAL  Left vertical. 
//   ; COMMON_LVB_GRID_RVERTICAL  Right vertical. 
//   ; COMMON_LVB_REVERSE_VIDEO   Reverse foreground and background attributes. 
//   ; COMMON_LVB_UNDERSCORE      Underscore. 
//   ;standard input text format
//   color_stdin = FOREGROUND_RED|FOREGROUND_INTENSITY
//   ;standard output text format
//   color_stdout = FOREGROUND_GREEN
//
// Debug:
//  If the 'debug_enabled' option is enabled in 'lv_proc.ini' the calls of the particular functions will be
//  logged to the 'debug.log' that will be created in the folder with 'lv_proc.dll'.
//---------------------------------------------------------------------------------------------------------------------


#include <windows.h>
#include <VersionHelpers.h>
#include <stdio.h>
#include <stdarg.h>

#define _LVPDLLEXPORT
#include "lv_proc.h"

// DLL file path
wchar_t dll_path[MAX_PATH];

//---------------------------------------------------------------------------
// debug file initialization
//---------------------------------------------------------------------------
int debug_init(TLVPHndl *proc,wchar_t *path)
{
	proc->dbg_path[0] = '\0';
	if(proc && path)
	{
		FILE *fw;
        if(_wfopen_s(&fw,path,L"wt"))
			return(1);
		
        wcscpy_s(proc->dbg_path,path);
		fclose(fw);
	}
	return(0);
}

//---------------------------------------------------------------------------
// debug file printf
//---------------------------------------------------------------------------
int debug_printf(TLVPHndl *proc,const char *fmt,...)
{
	if(proc && proc->dbg_path[0])
	{
		FILE *fw;
		if(_wfopen_s(&fw,proc->dbg_path,L"a"))
			return(1);

		va_list vpr;
		va_start(vpr,fmt);
		vfprintf(fw,fmt,vpr);
		va_end(vpr);
		fflush(fw);

		fclose(fw);
	}
	return(0);
}


//---------------------------------------------------------------------------------------------------------------------
// combine two paths
//---------------------------------------------------------------------------------------------------------------------
wchar_t *build_path(wchar_t *dest,wchar_t *p1,wchar_t *p2,int maxlen)
{
    // copy first path into the buffer
    if(dest != p1)
        wcscpy_s(dest,maxlen,p1);
    
    // add path separator
    int len = wcsnlen_s(dest,maxlen);
    if(len > 1 && dest[len-1] != L'\\')
        wcscat_s(dest,maxlen,L"\\");

    // add second path
    wcscat_s(dest,maxlen,p2);

    return(dest);
}

//---------------------------------------------------------------------------------------------------------------------
// strips absolute path to path and file name
//---------------------------------------------------------------------------------------------------------------------
void strip_path(wchar_t *path,int size,wchar_t **name)
{
    if(name)
        *name = NULL;
    
    // exctract folder path
    wchar_t *str = wcsrchr(path,L'\\');
    if(str)
    {
        *str = L'\0';
        if(name && (str - path - 1) > size)
            *name = &str[1];
    }
}


//---------------------------------------------------------------------------
// INI: parse text format flags, optionaly builds text string from flags list 
//---------------------------------------------------------------------------
WORD ini_parse_color(wchar_t *str,WORD clr_in,wchar_t *clr_str_out,int size)
{
	// flags list
	struct{
		wchar_t *str;
		WORD flag;
	}clrlst[]={
		{L"FOREGROUND_BLUE",FOREGROUND_BLUE},
		{L"FOREGROUND_GREEN",FOREGROUND_GREEN},
		{L"FOREGROUND_RED",FOREGROUND_RED},
		{L"FOREGROUND_INTENSITY",FOREGROUND_INTENSITY},
		{L"BACKGROUND_BLUE",BACKGROUND_BLUE},
		{L"BACKGROUND_GREEN",BACKGROUND_GREEN},
		{L"BACKGROUND_RED",BACKGROUND_RED},
		{L"BACKGROUND_INTENSITY",BACKGROUND_INTENSITY},
		{L"COMMON_LVB_LEADING_BYTE",COMMON_LVB_LEADING_BYTE},
		{L"COMMON_LVB_TRAILING_BYTE",COMMON_LVB_TRAILING_BYTE},
		{L"COMMON_LVB_GRID_HORIZONTAL",COMMON_LVB_GRID_HORIZONTAL},
		{L"COMMON_LVB_GRID_LVERTICAL",COMMON_LVB_GRID_LVERTICAL},
		{L"COMMON_LVB_GRID_RVERTICAL",COMMON_LVB_GRID_RVERTICAL},
		{L"COMMON_LVB_REVERSE_VIDEO",COMMON_LVB_REVERSE_VIDEO},
		{L"COMMON_LVB_UNDERSCORE",COMMON_LVB_UNDERSCORE},
		{L"",0}
	};

	// parse flags
	WORD flags = 0;
	int i = -1;
	while(str && clrlst[++i].flag)
	{
		if(wcsstr(str,clrlst[i].str))
			flags |= clrlst[i].flag;
	}

	// inverse conversion
	if(clr_str_out && size>1)
	{
		// clear output string
		//memset((void*)clr_str_out,'\0',size);
        clr_str_out[0] = '\0';
		size--;

        // combine flags
		i = -1;
		while(clrlst[++i].flag)
		{
			// contains flag?
			if(clr_in&clrlst[i].flag)
			{
				// yaha, enough space in string?
				if((int)wcslen(clrlst[i].str)<size)
				{
					// yaha, add separator?
					if(clr_str_out[0])
					{
						wcscat_s(clr_str_out,size,L"|");
						size--;
					}
					// add flag string
					wcscat_s(clr_str_out,size,clrlst[i].str);
					size -= wcslen(clrlst[i].str);
				}
			}
		}
	}

	// return all flags
	return(flags);
}
WORD ini_parse_color(wchar_t *str)
{
	return(ini_parse_color(str,0,NULL,0));
}



//---------------------------------------------------------------------------
// INI: load setup from ini (if found)
//---------------------------------------------------------------------------
int ini_read_ini(TCfg *cfg,int *dbg)
{
    wchar_t cstr[1024];

    // no destination buffer
    if(!cfg)
        return(1);

	// defaults
	cfg->no_hide = 0;
	cfg->console_x = -1;
	cfg->console_y = -1;
	cfg->th_priority = 1;
	cfg->th_idle = 1;
	cfg->write_pipe_buf = 0;
 	cfg->read_pipe_buf = 0;
	cfg->console_clr_stdin = FOREGROUND_RED|FOREGROUND_INTENSITY;
	cfg->console_clr_stdout = FOREGROUND_GREEN;
	if(dbg)
	    *dbg = 0;
  
	// build "config.ini"
	wchar_t pini[MAX_PATH];
    build_path(pini,dll_path,LVPROC_INI,MAX_PATH);
          	
    // debug mode?
	if(dbg)
		*dbg = GetPrivateProfileInt(L"DEBUG",L"debug_enabled",0,pini);
    
	// always show console
	cfg->no_hide = GetPrivateProfileInt(L"CONSOLE",L"no_hide",cfg->no_hide,pini);

	// read thread priority
	cfg->th_priority = GetPrivateProfileInt(L"READ",L"thread_priority",cfg->th_priority,pini);
	cfg->th_priority = min(max(cfg->th_priority,-15),+15);

	// read thread priority
	cfg->th_idle = GetPrivateProfileInt(L"READ",L"thread_idle_time",cfg->th_idle,pini);
	cfg->th_idle = min(max(cfg->th_idle,1),100);

	// pipe buffer sizes
	cfg->write_pipe_buf = max(GetPrivateProfileInt(L"PIPES",L"write_pipe_buffer_size",cfg->write_pipe_buf,pini),0);
	cfg->read_pipe_buf = max(GetPrivateProfileInt(L"PIPES",L"read_pipe_buffer_size",cfg->read_pipe_buf,pini),0);
	
	// stdin color
	ini_parse_color(NULL,cfg->console_clr_stdin,cstr,1024);
    wchar_t temp[1024];
    GetPrivateProfileStringW(L"CONSOLE",L"color_stdin",cstr,temp,1024,pini);
	cfg->console_clr_stdin = ini_parse_color(temp);
        

	// stdout color
	ini_parse_color(NULL,cfg->console_clr_stdout,cstr,1024);
	GetPrivateProfileStringW(L"CONSOLE",L"color_stdout",cstr,temp,1024,pini);
    cfg->console_clr_stdout = ini_parse_color(temp);

	// console size
	cfg->console_x = GetPrivateProfileInt(L"CONSOLE",L"buf_size_x",cfg->console_x,pini);
    cfg->console_y = GetPrivateProfileInt(L"CONSOLE",L"buf_size_y",cfg->console_y,pini);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: allocate/initialize
//---------------------------------------------------------------------------
int fifo_alloc(TLVPHndl *proc,int size)
{
	if(!proc)
		return(1);

	// allocate fifo structure
	proc->fifo = (TLVPFifo*)malloc(sizeof(TLVPFifo));
	if(!proc->fifo)
		return(1);

	// allocate buffer
	proc->fifo->data = (char*)malloc(size);
	if(!proc->fifo->data)
	{
		free((void*)proc->fifo);
		proc->fifo = NULL;
		return(1);
	}
	// store current length
	proc->fifo->len = size;

	// set read/write pointers
	proc->fifo->read = proc->fifo->data;
	proc->fifo->write = proc->fifo->data;

  // clear transfered data counters
	proc->fifo->c_stdout_bytes = 0;
	proc->fifo->c_stdin_bytes = 0;

	// init critical section
	InitializeCriticalSection(&proc->fifo->cs);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: loose fifo buffer
//---------------------------------------------------------------------------
int fifo_free(TLVPHndl *proc)
{
	if(!proc || !proc->fifo)
		return(1);

	// loose data buffer
	if(proc->fifo->data)
		free((void*)proc->fifo->data);

	// loose critical section
	DeleteCriticalSection(&proc->fifo->cs);

	// loose fifo structure itself
	free((void*)proc->fifo);
	proc->fifo = NULL;

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: free bytes
//---------------------------------------------------------------------------
int fifo_to_write(TLVPHndl *proc,int *len)
{
	if(len)
		*len = 0;

	if(!proc || (proc && !proc->fifo))
		return(1);

	EnterCriticalSection(&proc->fifo->cs);

	if(!proc || !proc->fifo->data || !len)
	{
		LeaveCriticalSection(&proc->fifo->cs);
		return(1);
	}

	// free space length
	if(proc->fifo->write==proc->fifo->read)
		*len = proc->fifo->len - 1;
	else if(proc->fifo->write<proc->fifo->read)
		*len = proc->fifo->read - proc->fifo->write - 1;
	else
		*len = proc->fifo->read - proc->fifo->write + proc->fifo->len - 1;

	LeaveCriticalSection(&proc->fifo->cs);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: available data bytes
//---------------------------------------------------------------------------
int fifo_to_read(TLVPHndl *proc,int *len)
{
	if(len)
		*len = 0;

	if(!proc || (proc && !proc->fifo))
		return(1);

	EnterCriticalSection(&proc->fifo->cs);

	if(!proc || !proc->fifo->data || !len)
	{
		LeaveCriticalSection(&proc->fifo->cs);
		return(1);
	}

	// data amount in the fifo
	if(proc->fifo->read==proc->fifo->write)
		*len = 0;
	else if(proc->fifo->read<proc->fifo->write)
		*len = proc->fifo->write - proc->fifo->read;
	else
		*len = proc->fifo->write - proc->fifo->read + proc->fifo->len;

	LeaveCriticalSection(&proc->fifo->cs);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: flush fifo content
//---------------------------------------------------------------------------
int fifo_clear(TLVPHndl *proc)
{
  if(!proc || (proc && !proc->fifo))
		return(1);

	EnterCriticalSection(&proc->fifo->cs);

	if(!proc || !proc->fifo->data)
	{
		LeaveCriticalSection(&proc->fifo->cs);
		return(1);
	}

	proc->fifo->read = proc->fifo->data;
	proc->fifo->write = proc->fifo->data;

	LeaveCriticalSection(&proc->fifo->cs);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: write data bytes
//---------------------------------------------------------------------------
int fifo_write(TLVPHndl *proc,char *data,int towr,int *written)
{
	if(written)
		*written = 0;

  if(!proc || (proc && !proc->fifo))
		return(1);

	EnterCriticalSection(&proc->fifo->cs);

	if(!proc || !proc->fifo->data || !data || !towr)
	{
		LeaveCriticalSection(&proc->fifo->cs);
		return(1);
	}

	// get free space
  int len;
	if(proc->fifo->write==proc->fifo->read)
		len = proc->fifo->len - 1;
	else if(proc->fifo->write<proc->fifo->read)
		len = proc->fifo->read - proc->fifo->write - 1;
	else
		len = proc->fifo->read - proc->fifo->write + proc->fifo->len - 1;

	// limit data size to write
	if(towr>len)
		towr = len;

	if(towr)
	{
		// write first block
		int blen_1 = &proc->fifo->data[proc->fifo->len] - proc->fifo->write;
		int blen_2 = 0;
		if(blen_1>towr)
			blen_1 = towr;
		else
			blen_2 = towr - blen_1;
		memcpy((void*)proc->fifo->write,(void*)data,blen_1);
		proc->fifo->write += blen_1;
		if(proc->fifo->write>=&proc->fifo->data[proc->fifo->len])
			proc->fifo->write -= proc->fifo->len;

		// write second block
		if(blen_2)
		{
			memcpy((void*)proc->fifo->write,(void*)&data[blen_1],blen_2);
			proc->fifo->write += blen_2;
			if(proc->fifo->write>=&proc->fifo->data[proc->fifo->len])
				proc->fifo->write -= proc->fifo->len;
		}
	}

  // update stdout bytes counter 
	proc->fifo->c_stdout_bytes += towr;

	LeaveCriticalSection(&proc->fifo->cs);

	// return bytes count written
	if(written)
		*written = towr;

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: read data bytes
//---------------------------------------------------------------------------
int fifo_read(TLVPHndl *proc,char *data,int tord,int *read)
{
	if(read)
		*read = 0;

    if(!proc || (proc && !proc->fifo))
		return(1);

	EnterCriticalSection(&proc->fifo->cs);

	if(!proc || !proc->fifo->data || !data || !tord)
	{
		LeaveCriticalSection(&proc->fifo->cs);
		return(1);
	}

	// get free space
    int len;
	if(proc->fifo->read == proc->fifo->write)
		len = 0;
	else if(proc->fifo->read < proc->fifo->write)
		len = proc->fifo->write - proc->fifo->read;
	else
		len = proc->fifo->write - proc->fifo->read + proc->fifo->len;
  
	// limit read data size
	if(tord > len)
		tord = len;

	if(tord)
	{
		// read first block
		int blen_1 = &proc->fifo->data[proc->fifo->len] - proc->fifo->read;
		int blen_2 = 0;
		if(blen_1>tord)
			blen_1 = tord;
		else
			blen_2 = tord - blen_1;
		memcpy((void*)data,(void*)proc->fifo->read,blen_1);
		proc->fifo->read += blen_1;
		if(proc->fifo->read >= &proc->fifo->data[proc->fifo->len])
			proc->fifo->read -= proc->fifo->len;

		// read second block
		if(blen_2)
		{
			memcpy((void*)&data[blen_1],(void*)proc->fifo->read,blen_2);
			proc->fifo->read += blen_2;
			if(proc->fifo->read>=&proc->fifo->data[proc->fifo->len])
				proc->fifo->read -= proc->fifo->len;
		}
	}

	LeaveCriticalSection(&proc->fifo->cs);

	// return bytes read
	if(read)
		*read = tord;

	return(0);
}

//---------------------------------------------------------------------------
// format capacity to string
//---------------------------------------------------------------------------
wchar_t *fmt_capacity(wchar_t *str,int maxstr,int size)
{
	wchar_t tmp[64];
	if(size < 10000)
		wprintf_s(tmp,64,L"%dB",size);
	else if(size < 1000000)
		wprintf_s(tmp,64,L"%.02fkB",(double)size/1024);
	else
		wprintf_s(tmp,64,L"%.02fMB",(double)size/1048576);
	wcscat_s(str,maxstr,tmp);
	return(str);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: STDOUT readout thread
//---------------------------------------------------------------------------
DWORD WINAPI fifo_read_thread(LPVOID lpParam)
{
	// now we have to make a local copy of the proc handle structure
	// because we can't rely the parent process won't reallocate the structures memory
    TLVPHndl proc;
    memcpy((void*)&proc,(void*)lpParam,sizeof(TLVPHndl));

	// signalize completed thread initialization
	proc.fifo->exit = 0;

	LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
	LARGE_INTEGER t_last; QueryPerformanceCounter(&t_last);

	char buf[STDOUT_TH_BUF_SIZE];
	int exit;
	do{

		exit = 0;
		int read = 0;
		int tord = 0;

		// free space in the fifo?
		int towr;
		if(fifo_to_write(&proc,&towr))
		{
			exit = 1;
			continue;
		}
		// limit to local buffer size
		if(towr > STDOUT_TH_BUF_SIZE)
			towr = STDOUT_TH_BUF_SIZE;

		// try to read stdout (critical section just in case we have debug console)
		if(proc.cout)
			EnterCriticalSection(&proc.fifo->cs);
		int ret = peek_stdout(&proc,&exit,buf,towr,&read,&tord);
		if(proc.cout)
			LeaveCriticalSection(&proc.fifo->cs);
		if(ret)
		{
			exit = 1;
			continue;
		}

		// try to store to fifo
		if(read)
			fifo_write(&proc,buf,read,NULL);

		// sleep?
		if(!exit && !proc.fifo->exit && !tord)
			WaitForSingleObject(proc.rd_event,proc.read_th_idle);

		// update status?
		LARGE_INTEGER t_new; QueryPerformanceCounter(&t_new);
        if(proc.cout && time_get_ms(&t_last,&t_new,&freq)>=STDOUT_TH_UPDATE_TIME)
		{
			wchar_t hdr[256];
            wcscpy_s(hdr,256,L"lv_proc.dll console (read only), stdout = ");
			fmt_capacity(hdr,256,proc.fifo->c_stdout_bytes);
			wcscat_s(hdr,256,L", stdin = ");
			fmt_capacity(hdr,256,proc.fifo->c_stdin_bytes);
			SetConsoleTitle(hdr);
			t_last = t_new;
		}


	}while(!proc.fifo->exit && !exit);

	return(0);
}


//---------------------------------------------------------------------------
// DLL main
//---------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	if(fwdreason == DLL_PROCESS_ATTACH)
	{
		// get DLL path
		GetModuleFileName((HMODULE)hinstDLL,dll_path,MAX_PATH);

        // get DLL folder
        strip_path(dll_path,MAX_PATH,NULL);
	}

	return(1);
}

//---------------------------------------------------------------------------------------------------------------------
// fills the string with DLL version info
//  *str: string buffer to be filled with the version ASCII string
//  maxlen: size of string buffer
//---------------------------------------------------------------------------------------------------------------------
void proc_get_dll_version(char *str,__int32 maxlen)
{    
    strncpy_s(str,maxlen,"LV Process DLL interface by Stanislav Maslan, s.maslan@seznam.cz, V4.1, 2016-12-06",maxlen-1);
}

//---------------------------------------------------------------------------
// Formats error code to string buffer.
//  code: lv proc error code
//  *str: string buffer
//  buflen: string buffer size
//---------------------------------------------------------------------------
__int32 proc_format_error(__int32 code,char *str,__int32 buflen)
{
	// error list
	struct{
		int code;
		char *str;
	}*perr,errlst[]={
		{LVP_EC_NO_PROC,"missing instance handle!"},
		{LVP_EC_CANT_CREATE_PIPE,"can't create pipe!"},
		{LVP_EC_CANT_CREATE_PROC,"can't create process!"},
		{LVP_EC_NO_EXIT,"process is still running!"},
		{LVP_EC_EXIT_TO,"exit code query timeout!"},
		{LVP_EC_NO_BUF,"no data buffer assigned!"},
		{LVP_EC_NO_LEN,"data buffer has zero length!"},
		{LVP_EC_WRITE_FAIL,"writting to stdin pipe failed!"},
		{LVP_EC_WRITE_INCOMPLETE,"incomplete command write!"},
		{LVP_EC_EXITED,"process returned exit code!"},
		{LVP_EC_TIMEOUT,"command response timeout!"},
		{LVP_EC_TERM_FAILED,"process termination failed!"},
		{LVP_EC_SMALL_BUF,"buffer to small for error string!"},
		{LVP_EC_CONS_CRAETE_FAILED,"debug console creation failed!"},
		{LVP_EC_STDOUT_RD_TH_FAILED,"creation of stdout readout thread failed!"},
		{LVP_EC_STDOUT_FIFO_FAILED,"allocation of the stdout fifo buffer failed!"},
		{LVP_EC_STDOUT_EVENT_FAILED,"creating wakup event of the stdout fifo buffer failed!"},
		{0,"unknown error!"}
	};

  // find error code in list
	char buf[256];buf[0]='\0';
	perr=&errlst[-1];
	do
	{
		perr++;
		if((perr->code==0 || perr->code==code) && code)
		{
			sprintf_s(buf,256,"LV process error (%d): %s",code,perr->str);
			break;
		}
	}while(perr->code>0);
    
	// try to return
    strncpy_s(str,buflen,buf,buflen-1);
	if(strlen(buf) >= (unsigned)buflen)
		return(LVP_EC_SMALL_BUF);

	return(0);
}


//---------------------------------------------------------------------------
// Return size of the lv process handle TPHndl in bytes (usefull in LabVIEW).
//---------------------------------------------------------------------------
__int32 proc_handle_size(void)
{
    return(sizeof(TLVPHndl));
}

//---------------------------------------------------------------------------
// Try to create process and its pipes.
// Closes all opened handles if not successfull.
//  *proc: lv process instance handle
//  *folder: working directory for the process
//  *cmd: the command to execute, i.e. path to the executable of the process
//        with eventual parameters, so for instance "cmd.exe" to run wincmd
//  sterr: write 1 to combine stderr to stdout
//  hide: write 1 to hide console
//---------------------------------------------------------------------------
__int32 proc_create(TLVPHndl *proc,char *folder,char *cmd,__int32 sterr,__int32 hide)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// clear handle variables
    memset((void*)proc,0,sizeof(TLVPHndl));

	// try read ini
	TCfg cfg;
	int dbg;
	ini_read_ini(&cfg,&dbg);

    // copy config to lv_process handle
	proc->read_th_idle = cfg.th_idle;

	// store debug file path
	if(dbg)
    {
		// build debug file path
        wchar_t dbgpth[MAX_PATH];
        wcscpy_s(dbgpth,MAX_PATH,dll_path);
        build_path(dbgpth,dll_path,LVPROC_DBG,MAX_PATH);
        // try to initiazlize
        debug_init(proc,dbgpth);
    }

	// clear hide parameter?
	if(cfg.no_hide)
		hide = 0;

	// --- try to create console ---
	if(!hide)
	{
		debug_printf(proc,"allocating console\n");

		if(!AllocConsole())
		{
			// console acraetion failed
			proc_cleanup(proc);
			return(LVP_EC_CONS_CRAETE_FAILED);
		}

		debug_printf(proc," - done\n");

		// get console stdout
		proc->cout = GetStdHandle(STD_OUTPUT_HANDLE);

		// set title
		SetConsoleTitle(L"lv_proc.dll console (read only)");

		// set buffers
		CONSOLE_SCREEN_BUFFER_INFO binf;
		GetConsoleScreenBufferInfo(proc->cout,&binf);
		COORD dwSize;
		dwSize.X = binf.dwSize.X;
		dwSize.Y = binf.dwSize.Y;
		if(cfg.console_x>0)
			dwSize.X = cfg.console_x;
		if(cfg.console_y>0)
			dwSize.Y = cfg.console_y;
		SetConsoleScreenBufferSize(proc->cout,dwSize);

		// set console size
		CONSOLE_SCREEN_BUFFER_INFO cinf;
		GetConsoleScreenBufferInfo(proc->cout,&cinf);
		SMALL_RECT crec = cinf.srWindow;
		if(cfg.console_x>0)
			crec.Right = crec.Left + cfg.console_x - 1;
		SetConsoleWindowInfo(proc->cout,1,&crec);

        // setup stdinput mode
		HANDLE cinp = GetStdHandle(STD_INPUT_HANDLE);
		DWORD cinp_mode;
		GetConsoleMode(cinp,&cinp_mode);
		cinp_mode &= (~ENABLE_PROCESSED_INPUT);
		SetConsoleMode(cinp,cinp_mode);

		// store color scheme
		proc->clr_in = cfg.console_clr_stdin;
		proc->clr_out = cfg.console_clr_stdout;

		debug_printf(proc," - console setup done\n");
	}

    debug_printf(proc,"creating process stdin/stdout pipes\n");

	// --- crate pipes for console stdout and stdin ---
	// pipe secturity attributes
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	if(IsWindowsXPOrGreater())
	{
		InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd,true,NULL,false);
		sa.lpSecurityDescriptor=&sd;
	}
	else
		sa.lpSecurityDescriptor=NULL;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle=true;

	// --- try to create pipes ---
	// stdin pipe
	if(!CreatePipe(&proc->pinp[1],&proc->pinp[0],&sa,cfg.write_pipe_buf))
	{
		// failed - leave
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PIPE);
	}
	// stdout pipe
	if(!CreatePipe(&proc->pout[0],&proc->pout[1],&sa,cfg.read_pipe_buf))
	{
		// failed - close pipes and leave
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PIPE);
	}

	debug_printf(proc," - done\n");

    debug_printf(proc,"creating process\n");

	// --- create process startup info ---
	// general setup
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb=sizeof(si);
	si.wShowWindow=SW_HIDE;
	si.dwFlags|=STARTF_USESHOWWINDOW;
	// assign pipes
	si.dwFlags|=STARTF_USESTDHANDLES;
	si.hStdOutput=proc->pout[1];
	si.hStdInput=proc->pinp[1];
	si.hStdError=sterr?proc->pout[1]:NULL;
        
    // --- try to crate process ---
    if(!CreateProcessA(NULL,cmd,NULL,NULL,true,0,NULL,folder,&si,&pi))
	{
		// failed - close handles
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PROC);
	}
    // store process and thread pointers
	proc->hproc = pi.hProcess;
	proc->hth = pi.hThread;
	proc->pid = pi.dwProcessId;
	proc->tid = pi.dwThreadId;

	debug_printf(proc," - done\n");

	debug_printf(proc,"allocating stdout fifo\n");

	// --- try to create stdout fifo ---
	if(fifo_alloc(proc,STDOUT_FIFO_BUF_LEN))
	{
		// failed
		proc_cleanup(proc);
		return(LVP_EC_STDOUT_FIFO_FAILED);
	}

	debug_printf(proc," - done\n");

	debug_printf(proc,"creating stdout wakup event\n");
	
	// --- try to create read wakeup event ---
	proc->rd_event = CreateEvent(NULL,false,false,NULL);
	if(!proc->rd_event)
	{
		// failed
		proc_cleanup(proc);
		return(LVP_EC_STDOUT_EVENT_FAILED);
	}

	debug_printf(proc," - done\n");
	

	debug_printf(proc,"creating stdout readout thread\n");

	// create stdout fifo read thread
	proc->fifo->exit = -1; // initialization mode
	proc->fifo->th = CreateThread(NULL,0,fifo_read_thread,(PVOID)proc,0,NULL);
	if(!proc->fifo->th)
	{
		// failed
		proc_cleanup(proc);
		return(LVP_EC_STDOUT_RD_TH_FAILED);
	}

	// set thread priority
	SetThreadPriority(proc->fifo->th,cfg.th_priority);

	debug_printf(proc," - done\n");

	debug_printf(proc,"waiting for stdout readout thread initialization\n");

	// wait for stdout fifo read thread initialization
	int to = 50;
	while(--to>0 && proc->fifo->exit<0)
		Sleep(50);
	if(!to)
	{
		// faild - timeout
		debug_printf(proc," - timeout!\n");
		proc_cleanup(proc);
		return(LVP_EC_STDOUT_RD_TH_FAILED);
	}

	debug_printf(proc," - done\n");

	return(0);
}

//---------------------------------------------------------------------------
// Close process instance handle. Call this to cleanup after the process has terminated.
//  *proc: lv process instance handle
//---------------------------------------------------------------------------
__int32 proc_cleanup(TLVPHndl *proc)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	debug_printf(proc,"closing handles:\n");

	// close handles
	if(proc->hth)
		CloseHandle(proc->hth);
	if(proc->hproc)
		CloseHandle(proc->hproc);

	debug_printf(proc," - thread and process handles closed\n");

	if(proc->pinp[0])
		CloseHandle(proc->pinp[0]);
	if(proc->pinp[1])
		CloseHandle(proc->pinp[1]);
	if(proc->pout[0])
		CloseHandle(proc->pout[0]);
	if(proc->pout[1])
		CloseHandle(proc->pout[1]);

	debug_printf(proc," - pipes handles closed\n");


	debug_printf(proc,"closing stdout readout thread:\n");

	// terminate stdout read thread
	if(proc->fifo && proc->fifo->th)
	{
		ResumeThread(proc->fifo->th);
		proc->fifo->exit = 1;
		int max_tries = 50;
		DWORD dw;
		while(--max_tries>0 && GetExitCodeThread(proc->fifo->th,&dw) && dw==STILL_ACTIVE)
			Sleep(50);
		if(!max_tries)
		{
			// timeout - terminate
			TerminateThread(proc->fifo->th,0);
      debug_printf(proc," - stdout readout thread terminated!\n");
		}
		CloseHandle(proc->fifo->th);
	}

	debug_printf(proc," - stdout readout thread closed\n");

	// loose wakeup event
	if(proc->rd_event)
	{
		CloseHandle(proc->rd_event);
	}

	debug_printf(proc," - stdout wakeup event closed\n");

	// destroy console
	if(proc->cout)
	{
		debug_printf(proc,"freeing console\n");
		FreeConsole();
	}

	// loose stdout fifo buffer
	debug_printf(proc,"dealocating stdout fifo\n");
	fifo_free(proc);


	// clear handle variables
	wchar_t path[MAX_PATH];
	wcscpy_s(path,MAX_PATH,proc->dbg_path);
    memset((void*)proc,0,sizeof(TLVPHndl));
	wcscpy_s(proc->dbg_path,MAX_PATH,path);

	return(0);
}

//---------------------------------------------------------------------------
// Try to get process instance exit code.
// Returns LVP_EC_NO_EXIT error if still running.
//  *proc: lv process instance handle
//  *code: pointer to variable that receives the exit code (optional)
//---------------------------------------------------------------------------
__int32 proc_get_exit_code(TLVPHndl *proc,__int32 *code)
{
	// return default exit code
	if(code)
		*code=(int)STILL_ACTIVE;

	// leave if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	// get exit code
	DWORD ec = STILL_ACTIVE;
	int ret = GetExitCodeProcess(proc->hproc,&ec);

	if(!ret)
	{
		ec = STILL_ACTIVE;
		debug_printf(proc,"checking process exit code: failed!\n");
	}
	else if(ec==STILL_ACTIVE)
		debug_printf(proc,"checking process exit code: %d - STILL_ACTIVE\n",ec);
	else
		debug_printf(proc,"checking process exit code: %d\n",ec);

	// return exit code
	if(code)
		*code=(int)ec;

	// no exit code yet
	if(ec==STILL_ACTIVE)
		return(LVP_EC_NO_EXIT);

	return(0);
}

//---------------------------------------------------------------------------
// Wait for process exit code. Set time to 0 if no timeout requested.
//  *proc: lv process instance handle
//  *code: variable that receives exit code (optional)
//  time: timeout in ms, rather don't use 0 :)
//---------------------------------------------------------------------------
__int32 proc_wait_exit(TLVPHndl *proc,__int32 *code,__int32 time)
{
	DWORD ec = 0;

	// leave if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	debug_printf(proc,"waiting for process to return:\n");

	// get processor frequency aand initial time
	LARGE_INTEGER freq;
	LARGE_INTEGER t_last;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t_last);

	// wait for process exit code
	int ret;
	do{
		ret = GetExitCodeProcess(proc->hproc,&ec);
		if(!ret)
		{
			// failed
            debug_printf(proc," - reading exit code failed!\n");
			ec = STILL_ACTIVE;
			continue;
		}

		// wait if still active
		if(ret && ec==STILL_ACTIVE)
			Sleep(5);

		// check timeout
		LARGE_INTEGER t_new;QueryPerformanceCounter(&t_new);
		if(ec==STILL_ACTIVE && time && time_get_ms(&t_last,&t_new,&freq)>=time)
		{
			debug_printf(proc," - timeout!\n");
			break;
		}

	}while(ret && ec==STILL_ACTIVE);

	if(ret)
		debug_printf(proc," - done with exit code %d\n",ec);

	// return exit code if required
	if(code)
		*code=(int)ec;

	// return error if timeout
	if(ec==STILL_ACTIVE)
		return(LVP_EC_EXIT_TO);

	return(0);
}

//---------------------------------------------------------------------------
// Harcore process termination. Call this when the process does not behave or it cannot be 
// terminated at all.
//  *proc: lv process instance handle
//  time: timeout in ms
//---------------------------------------------------------------------------
__int32 proc_terminate(TLVPHndl *proc,__int32 time)
{
	// leave if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	debug_printf(proc,"process termination:\n");

	// terminate process (async)
	if(TerminateProcess(proc->hproc,0)==0)
	{
		debug_printf(proc," - failed!\n");
		return(LVP_EC_TERM_FAILED);
	}

	debug_printf(proc," - waiting for process exit code\n");

	// wait for exit
	return(proc_wait_exit(proc,NULL,time));
}

//---------------------------------------------------------------------------
// Flush stdout pipe data. 'rint' [ms] is maximum interval between incomming
// stdout data blocks.
// Function returns if no data appears on stdout for 'rint'.
//  *proc: lv process instance handle
//  *exit: variable that receives exit code if process exited (optional)
//  rint: maximum 'gap' between incomming stdout data in ms
//---------------------------------------------------------------------------
__int32 proc_flush_stdout(TLVPHndl *proc,__int32 *exit,__int32 rint)
{
	// leave if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	debug_printf(proc,"flushing stdout buffer:\n");

	int tord = 0;
	do{
		// loose data in stdout fifo
		fifo_clear(proc);
		// wait for timeout
		Sleep(rint);
		// check fifo buffer data
		fifo_to_read(proc,&tord);
		// leave if no new data in fifo
	}while(tord);

	debug_printf(proc," - done\n");

    // check process status
    DWORD ret;
	GetExitCodeProcess(proc->hproc,&ret);
	// process retuned?
	int done=(ret!=STILL_ACTIVE);

	// return exit code status
	if(exit)
		*exit=done;

	// signalize error if exited and no exit pointer
	if(done && !exit)
		return(LVP_EC_EXITED);

	return(0);
}

//---------------------------------------------------------------------------
// Write to stdin pipe.
//  *proc: lv process instance handle
//  *buf: data to write
//  towr: number of bytes to write, use negative number to take 'buf' as
//        a null terminated string and detect size automatically, where the 
//        absolute value of 'towr' is maximum expected size of 'buf' data
//        (safety solution for strnlen_s())
//  *written: returns number of actually written bytes (optional)
//---------------------------------------------------------------------------
__int32 proc_write_stdin(TLVPHndl *proc,char *buf,__int32 towr,__int32 *written)
{
	// leave if no proc handle
	if(!proc || !proc->hproc || !proc->pinp[0])
		return(LVP_EC_NO_PROC);

	// leave if no data buffer
	if(!buf)
		return(LVP_EC_NO_BUF);

	// leave if nothing to write
	if(towr == 0)
		return(0);
    else if(towr < 0)
    {
        // buffer is null terminated string - detect buffer size automatically
        towr = strnlen_s(buf,-towr);
    }
       

	debug_printf(proc,"writting data to stdin\n");

	// try to write data buffer
	DWORD wrt;
	int ret = WriteFile(proc->pinp[0],(void*)buf,towr,&wrt,NULL);

	// return written count
	if(written)
		*written = (int)wrt;

    // update stdin bytes counter
	if(proc->fifo)
		proc->fifo->c_stdin_bytes += wrt;

	// write buffer to the console?
	if(ret && proc->cout && proc->fifo)
	{
        debug_printf(proc," - writting copy to the console\n");

		EnterCriticalSection(&proc->fifo->cs);
		SetConsoleTextAttribute(proc->cout,proc->clr_in);
		WriteConsole(proc->cout,(void*)buf,wrt,NULL,NULL);
		LeaveCriticalSection(&proc->fifo->cs);
	}

	// status?
	if(ret)
		return(0);
	else
		return(LVP_EC_WRITE_FAIL);
}

//---------------------------------------------------------------------------
// Peek process stdout pipe, read upto 'blen' chars, return process status.
// Automatically appends '\0' to the buffer data so maximum returned data are bsize - 1.
//  *proc: lv process instance handle
//  *exit: returns exit code if process returned (optional)
//  *buf: read buffer
//  bsize: byte size of the read buffer
//  *rread: returns total read bytes (optional)
//  *rtord: returns remaining bytes to read (optional)
//---------------------------------------------------------------------------
__int32 proc_peek_stdout(TLVPHndl *proc,__int32 *exit,char *buf,__int32 bsize,__int32 *rread,__int32 *rtord)
{
	// leave if no proc handle
	if(!proc || !proc->hproc || !proc->rd_event)
		return(LVP_EC_NO_PROC);

    // wakeup read thread
	SetEvent(proc->rd_event);

	// reserve '\0' in buffer size (string termination)
	bsize = (bsize>0)?(bsize-1):0;
  
	// read data from stdout fifo
    int read = 0;
	fifo_read(proc,buf,bsize,&read);
	buf += read;
	*buf = '\0';
	if(rread)
		*rread = read;

	// return remaining data in the fifo
	int tord = 0;
	fifo_to_read(proc,&tord);
	if(rtord)
		*rtord = tord;

	if(buf && bsize)
		debug_printf(proc,"buffered stdout peek: %dB read, %dB remaining\n",read,tord);

	// check process status
    DWORD ret;
	GetExitCodeProcess(proc->hproc,&ret);
	// process retuned?
	int done=(ret!=STILL_ACTIVE);

	// return exit code status
	if(exit)
		*exit=done;

	return(0);
}
// main stdout readout function, called ONLY by readout thread
int peek_stdout(TLVPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord)
{
	// leave if no proc handle
	if(!proc || !proc->hproc || !proc->pout[0])
		return(LVP_EC_NO_PROC);

	// reserve '\0' in buffer size (string termination)
	bsize=(bsize>0)?(bsize-1):0;

	// nothing read yet
	int read=0;

	// not done yet
	int done=0;

	// nothing to return yet
	//DWORD ret=0;

	int ptord;
	do{
		// peek pipe for data
		PeekNamedPipe(proc->pout[0],NULL,0,NULL,(DWORD*)&ptord,NULL);

		// limit available data block to buffer size
		if(ptord>bsize)
			ptord=bsize;

		// read pipe data into buffer
		if(buf && ptord)
		{
			DWORD bread;
			ReadFile(proc->pout[0],buf,ptord,&bread,NULL);

            debug_printf(proc,"stdout pipe -> fifo: %dB\n",bread);

			// write buffer to the console?
			if(bread && proc->cout)
			{
				SetConsoleTextAttribute(proc->cout,proc->clr_out);
				WriteConsole(proc->cout,(void*)buf,bread,NULL,NULL);
			}

			bsize-=bread;
			buf+=bread;
			*buf='\0';
			read+=bread;
		}

		// check process exit code
		if(!done)
		{
			// check process status
      DWORD ec;
			int ret = GetExitCodeProcess(proc->hproc,&ec);

			// process retuned?
			done = ret && (ec!=STILL_ACTIVE);
		}

		// peek pipe again to get remaining data size
		PeekNamedPipe(proc->pout[0],NULL,0,NULL,(DWORD*)&ptord,NULL);

		// repeat if something to read and some place in buffer
	}while(!done && ptord && bsize);

	// return exit code status
	if(exit)
		*exit=done;

	// return bytes count read
	if(rread)
		*rread=read;

	// return bytes count to read from pipe
	if(rtord)
		*rtord=ptord;

	return(0);
}


//---------------------------------------------------------------------------
// Flush input pipe, send command buffer, wait for process instance answer,
// read output pipe.
//  *proc: lv process instance handle
//  *exit: returns exit code if process returned (optional)
//  *cmd: data to write
//  cmdlen: number of bytes to write, use negative number to take 'cmd' as
//          a null terminated string and detect size automatically, where the 
//          absolute value of 'cmdlen' is maximum expected size of 'cmd' data
//          (safety solution for strnlen_s())
//  *buf: read buffer
//  buflen: read buffer size [B]
//  *bufret: returnes bytes read (optional)
//  rtime: first response byte timeout [ms]
//  rint: maximum receive interval between successive data blocks [ms]
//
// In case no command is sent, function only reads stdout pipe with timeout.
//---------------------------------------------------------------------------
__int32 proc_command(TLVPHndl *proc,__int32 *exit,char *cmd,__int32 cmdlen,char *buf,__int32 buflen,__int32 *bufret,__int32 rtime,__int32 rint)
{
	int ret;
	int done;

	// error if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	// error if missing read buffer
	if(!buf)
		return(LVP_EC_NO_BUF);

	// error no space in read buffer
	if(buflen < 2)
		return(LVP_EC_NO_LEN);

	// nothing read yet
	int dret = 0;
	if(bufret)
		*bufret = '\0';

	// --- flush stdout data ---
	if(cmd && cmdlen != 0)
	{
		debug_printf(proc,"sending command - flushing stdout\n");

		// flush data - fast readout with minimum timeout
		proc_flush_stdout(proc,&done,30);

		// return exit code status
		if(exit)
			*exit = done;

		// leave if process instance returned exit code
		if(done && !exit)
			return(LVP_EC_EXITED);
		else if(done)
			return(0);
	}

	// --- write command ---
	if(cmd && cmdlen != 0)
	{
		debug_printf(proc,"sending command - writting new command\n");

        // something to write
		int written;
		ret = proc_write_stdin(proc,cmd,cmdlen,&written);

        if(cmdlen < 0)
        {
            // buffer is null terminated string - detect buffer size automatically
            cmdlen = strnlen_s(cmd,-cmdlen);
        }
        
		// general write fail
		if(ret)
			return(LVP_EC_WRITE_FAIL);
		else if(cmdlen != written)
			return(LVP_EC_WRITE_INCOMPLETE);
	}

	// get processor frequency
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	debug_printf(proc,"sending command - waiting for answer\n");

	// wakeup read thread
	SetEvent(proc->rd_event);

	// --- try to read response ---
	LARGE_INTEGER t_last;
	QueryPerformanceCounter(&t_last);
	buflen--;
	do{
		// peek data
		int read;
		proc_peek_stdout(proc,&done,buf,buflen,&read,NULL);

		// move return buffer pointer
		buf += read;
		*buf = '\0';
		buflen -= read;
		dret += read;

		// return exit code status
		if(exit)
			*exit = done;

		// return read count
		if(bufret)
			*bufret = dret;

		// error if process returned exit code
		if(done && !exit)
			return(LVP_EC_EXITED);
		else if(done)
			return(0);

		// full buffer - done
		if(buflen < 1)
			return(0);

		// current time
		LARGE_INTEGER t_new;
		QueryPerformanceCounter(&t_new);

		// check response timeout
		if(!dret && time_get_ms(&t_last,&t_new,&freq)>=rtime)
		{
			// timeout - error
			return(LVP_EC_TIMEOUT);
		}

		// byte recieved?
		if(read)
		{
			// yaha - last time = new time
			t_last = t_new;
		}

		// read interval timeout?
		if(dret && time_get_ms(&t_last,&t_new,&freq)>=rint)
		{
			// yaha, done
			return(0);
		}

		// wait a moment
		Sleep(1);

	}while(1);
}



//---------------------------------------------------------------------------
// get time from performance timer values and frequency [ms]
//---------------------------------------------------------------------------
int time_get_ms(LARGE_INTEGER *t1,LARGE_INTEGER *t2,LARGE_INTEGER *f)
{
	double dt=(double)(t2->QuadPart - t1->QuadPart);
	return((int)(1000.0*dt/(double)f->QuadPart));
}
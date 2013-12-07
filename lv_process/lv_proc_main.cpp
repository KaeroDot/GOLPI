//---------------------------------------------------------------------------
// Simple process executer with stdin/stdout pipes. Features:
//  1) creates new process
//  2) creates pipes for stdin/stdout communication with the process
//  3) uses simple struct as handle to the process and its pipes
//  4) implements basic pipe read/write functions with minimum parameters
//  5) implements specialized pipe read+write usable for simple command exec.
//     with consoles like GNU Octave
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------

#include <windows.h>
#include <vcl.h>
#include <IniFiles.hpp>
#include <stdio.h>
#include <stdarg.h>
#include "lv_proc_main.h"

// this should be automatically loaded with DLL folder path
AnsiString dll_path;

int debug_init(TPHndl *proc,char *path)
{
	proc->dbg_path[0] = '\0';
	if(proc && path)
	{
		FILE *fw = fopen(path,"wt");
		if(!fw)
			return(1);
		strcpy(proc->dbg_path,path);
		fclose(fw);
	}
	return(0);
}

int debug_printf(TPHndl *proc,const char *fmt,...)
{
	if(proc && proc->dbg_path[0])
	{
		//ShowMessage(proc->dbg_path);
		FILE *fw = fopen(proc->dbg_path,"a");
		if(!fw)
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


//---------------------------------------------------------------------------
// INI: parse text format flags, optionaly builds text string from flags list 
//---------------------------------------------------------------------------
WORD ini_parse_color(char *str,WORD clr_in,char *clr_str_out,int size)
{
	// flags list
	struct{
		char *str;
		WORD flag;
	}clrlst[]={
		{"FOREGROUND_BLUE",FOREGROUND_BLUE},
		{"FOREGROUND_GREEN",FOREGROUND_GREEN},
		{"FOREGROUND_RED",FOREGROUND_RED},
		{"FOREGROUND_INTENSITY",FOREGROUND_INTENSITY},
		{"BACKGROUND_BLUE",BACKGROUND_BLUE},
		{"BACKGROUND_GREEN",BACKGROUND_GREEN},
		{"BACKGROUND_RED",BACKGROUND_RED},
		{"BACKGROUND_INTENSITY",BACKGROUND_INTENSITY},
		{"COMMON_LVB_LEADING_BYTE",COMMON_LVB_LEADING_BYTE},
		{"COMMON_LVB_TRAILING_BYTE",COMMON_LVB_TRAILING_BYTE},
		{"COMMON_LVB_GRID_HORIZONTAL",COMMON_LVB_GRID_HORIZONTAL},
		{"COMMON_LVB_GRID_LVERTICAL",COMMON_LVB_GRID_LVERTICAL},
		{"COMMON_LVB_GRID_RVERTICAL",COMMON_LVB_GRID_RVERTICAL},
		{"COMMON_LVB_REVERSE_VIDEO",COMMON_LVB_REVERSE_VIDEO},
		{"COMMON_LVB_UNDERSCORE",COMMON_LVB_UNDERSCORE},
		{"",0}
	};

	// parse flags
	WORD flags = 0;
	int i = -1;
	while(str && clrlst[++i].flag)
	{
		if(strstr(str,clrlst[i].str))
			flags |= clrlst[i].flag;
	}

	// inverse conversion
	if(clr_str_out && size>1)
	{
		// clear output string
		memset((void*)clr_str_out,'\0',size);
		size--;

    // combine flags
		i = -1;
		while(clrlst[++i].flag)
		{
			// contains flag?
			if(clr_in&clrlst[i].flag)
			{
				// yaha, enough space in string?
				if((int)strlen(clrlst[i].str)<size)
				{
					// yaha, add separator?
					if(clr_str_out[0])
					{
						strcat(clr_str_out,"|");
						size--;
					}
					// add flag string
					strcat(clr_str_out,clrlst[i].str);
					size -= strlen(clrlst[i].str);
				}
			}
		}
	}

	// return all flags
	return(flags);
}
WORD ini_parse_color(char *str)
{
	return(ini_parse_color(str,0,NULL,0));
}



//---------------------------------------------------------------------------
// INI: load setup from ini (if found)
//---------------------------------------------------------------------------
int ini_read_ini(TCfg *cfg,int *dbg)
{
  char cstr[1024];
	AnsiString str;
	TIniFile *ini;

	// defaults
	cfg->no_hide = 0;
	cfg->console_x = -1;
	cfg->console_y = -1;
	cfg->console_clr_stdin = FOREGROUND_RED|FOREGROUND_INTENSITY;
	cfg->console_clr_stdout = FOREGROUND_GREEN;
	if(dbg)
		*dbg = 0;
  
	// build "config.ini"
	AnsiString ipath = dll_path + "lv_proc.ini";
  
	// no destination buffer
	if(!cfg)
		return(1);

	// leave if file not exists
	if(!FileExists(ipath))
		return(1);

	// open ini file
	ini = new TIniFile(ipath);

  // debug mode
	if(dbg)
		*dbg = ini->ReadInteger("DEBUG","debug_enabled",*dbg);

	// always show console
	cfg->no_hide = ini->ReadInteger("CONSOLE","no_hide",cfg->no_hide);

	// stdin color
	ini_parse_color(NULL,cfg->console_clr_stdin,cstr,1024);
	str = ini->ReadString("CONSOLE","color_stdin",cstr);
	cfg->console_clr_stdin = ini_parse_color(str.c_str());

	// stdout color
	ini_parse_color(NULL,cfg->console_clr_stdout,cstr,1024);
	str = ini->ReadString("CONSOLE","color_stdout",cstr);
	cfg->console_clr_stdout = ini_parse_color(str.c_str());

	// console size
	cfg->console_x = ini->ReadInteger("CONSOLE","buf_size_x",cfg->console_x);
	cfg->console_y = ini->ReadInteger("CONSOLE","buf_size_y",cfg->console_y);

	// close ini file
	delete(ini);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: allocate/initialize
//---------------------------------------------------------------------------
int fifo_alloc(TPHndl *proc,int size)
{
	if(!proc)
		return(1);

	// allocate fifo structure
	proc->fifo = (TFifo*)malloc(sizeof(TFifo));
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

	// init critical section
	InitializeCriticalSection(&proc->fifo->cs);

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: loose fifo buffer
//---------------------------------------------------------------------------
int fifo_free(TPHndl *proc)
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
int fifo_to_write(TPHndl *proc,int *len)
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
int fifo_to_read(TPHndl *proc,int *len)
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
int fifo_clear(TPHndl *proc)
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
int fifo_write(TPHndl *proc,char *data,int towr,int *written)
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

	LeaveCriticalSection(&proc->fifo->cs);

	// return bytes count written
	if(written)
		*written = towr;

	return(0);
}

//---------------------------------------------------------------------------
// STDOUT FIFO: read data bytes
//---------------------------------------------------------------------------
int fifo_read(TPHndl *proc,char *data,int tord,int *read)
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
	if(proc->fifo->read==proc->fifo->write)
		len = 0;
	else if(proc->fifo->read<proc->fifo->write)
		len = proc->fifo->write - proc->fifo->read;
	else
		len = proc->fifo->write - proc->fifo->read + proc->fifo->len;
  
	// limit read data size
	if(tord>len)
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
		if(proc->fifo->read>=&proc->fifo->data[proc->fifo->len])
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
// STDOUT FIFO: STDOUT readout thread
//---------------------------------------------------------------------------
DWORD WINAPI fifo_read_thread(LPVOID lpParam)
{
	// now we have to make a local copy of the proc handle structure
	// because we can't rely the parent process won't reallocate the structures memory
	TPHndl proc;
	memcpy((void*)&proc,(void*)lpParam,sizeof(TPHndl));

	// signalize completed thread initialization
	proc.fifo->exit = 0;
                                                   
	#define STDOUT_TH_BUF_SIZE 8192
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
		if(towr>STDOUT_TH_BUF_SIZE)
			towr = STDOUT_TH_BUF_SIZE;

		// try to read stdout
		EnterCriticalSection(&proc.fifo->cs);
		int ret = peek_stdout(&proc,&exit,buf,towr,&read,&tord);
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
			Sleep(5);

	}while(!proc.fifo->exit && !exit);

	return(0);
}



//---------------------------------------------------------------------------
// DLL main
//---------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	if(fwdreason == DLL_PROCESS_ATTACH)
	{
		// get DLL path
		char path[MAX_PATH];
		GetModuleFileName((HMODULE)hinstDLL,path,MAX_PATH);
		// get DLL folder
		dll_path = ExtractFilePath(path);
	}

	return(1);
}

//---------------------------------------------------------------------------
// Formats error code to string.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_format_error(int code,char *str,int buflen)
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
		{LVP_EC_EXIT_TO,"exit code querry timeout!"},
		{LVP_EC_NO_BUF,"no data buffer assigned!"},
		{LVP_EC_NO_LEN,"data buffer has zero length!"},
		{LVP_EC_WRITE_FAIL,"writting to stdin pipe failed!"},
		{LVP_EC_WRITE_INCOMPLETE,"incomplete command write!"},
		{LVP_EC_EXITED,"process returned exit code!"},
		{LVP_EC_TIMEOUT,"command response timeout!"},
		{LVP_EC_TERM_FAILED,"process termination failed!"},
		{LVP_EC_SMALL_BUF,"buffer to small for error string!"},
		{LVP_EC_CONS_CRAETE_FAILED,"console creation failed!"},
		{LVP_EC_STDOUT_RD_TH_FAILED,"creation of stdout readout thread failed!"},
		{LVP_EC_STDOUT_FIFO_FAILED,"allocation of the stdout fifo buffer failed!"},
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
			sprintf(buf,"LV process error (%d): %s",code,perr->str);
			break;
		}
	}while(perr->code>0);

	// try to return
	if(strlen(buf)>=(unsigned)buflen)
		return(LVP_EC_SMALL_BUF);
	strcpy(str,buf);

	return(0);
}


//---------------------------------------------------------------------------
// Return handles struct size [Bytes].
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_handle_size(void)
{
	return(sizeof(TPHndl));
}

//---------------------------------------------------------------------------
// Try to create process and its pipes.
// Closes all opened handles if not successfull.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_create(TPHndl *proc,char *folder,char *cmd,int sterr,int hide)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// clear handle variables
	memset((void*)proc,0,sizeof(TPHndl));

	// try read ini
	TCfg cfg;
	int dbg;
	ini_read_ini(&cfg,&dbg);

	// store debug file path
	if(dbg)
		debug_init(proc,(dll_path + "debug.log").c_str());

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
		SetConsoleTitle("lv_proc.dll console (read only)");

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

	// --- check OS version ---
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	int ver=osv.dwPlatformId==VER_PLATFORM_WIN32_NT;

	// --- crate pipes for console stdout and stdin ---
	// pipe secturity attributes
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	if(ver)
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
	if(!CreatePipe(&proc->pinp[1],&proc->pinp[0],&sa,0))
	{
		// failed - leave
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PIPE);
	}
	// stdout pipe
	if(!CreatePipe(&proc->pout[0],&proc->pout[1],&sa,0))
	{
		// failed - close pipes and leave
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PIPE);
	}

	debug_printf(proc," - done\n");

  debug_printf(proc,"creating process\n");

	// --- create process startup info ---
	// general setup
	STARTUPINFO si;
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
	int ret = CreateProcess(NULL,cmd,NULL,NULL,true,0,NULL,folder,&si,&pi);
	proc->hproc = pi.hProcess;
	proc->hth = pi.hThread;
	proc->pid = pi.dwProcessId;
	proc->tid = pi.dwThreadId;
	if(!ret)
	{
		// failed - close handles
		proc_cleanup(proc);
		return(LVP_EC_CANT_CREATE_PROC);
	}

	debug_printf(proc," - done\n");

	debug_printf(proc,"allocating stdout fifo\n");

	// --- try to create stdout fifo ---
	if(fifo_alloc(proc,1048576))
	{
		// failed
		proc_cleanup(proc);
		return(LVP_EC_STDOUT_FIFO_FAILED);
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
// Close process instance handles.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_cleanup(TPHndl *proc)
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
	if(proc->fifo->th)
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
	char path[MAX_PATH];
	strcpy(path,proc->dbg_path);
	memset((void*)proc,0,sizeof(TPHndl));
	strcpy(proc->dbg_path,path);

	return(0);
}

//---------------------------------------------------------------------------
// Try to get process instance exit code.
// Returns LVP_EC_NO_EXIT error if still running.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_get_exit_code(TPHndl *proc,int *code)
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
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_wait_exit(TPHndl *proc,int *code,int time)
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
// Terminate process.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_terminate(TPHndl *proc,int time)
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
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_flush_stdout(TPHndl *proc,int *exit,int rint)
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
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_write_stdin(TPHndl *proc,char *buf,int towr,int *written)
{
	// leave if no proc handle
	if(!proc || !proc->hproc || !proc->pinp[0])
		return(LVP_EC_NO_PROC);

	// leave if no data buffer
	if(!buf)
		return(LVP_EC_NO_BUF);

	// leave if nothing to write
	if(!towr)
		return(0);

	debug_printf(proc,"writting data to stdin\n");

	// try to write data buffer
	DWORD wrt;
	int ret=WriteFile(proc->pinp[0],(void*)buf,towr,&wrt,NULL);

	// return written count
	if(written)
		*written=(int)wrt;

	// write buffer to the console?
	if(ret && proc->cout)
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
// Peek process stdout pipe, read upto blen chars, return process status.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_peek_stdout(TPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord)
{
	// leave if no proc handle
	if(!proc || !proc->hproc)
		return(LVP_EC_NO_PROC);

	// reserve '\0' in buffer size (string termination)
	bsize=(bsize>0)?(bsize-1):0;
  
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

int peek_stdout(TPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord)
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
//  rtime - first response byte timeout [ms]
//  rint  - maximum receive interval between successive data blocks [ms]
//
// In case no command is sent, function only reads stdout pipe with timeout.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_command(TPHndl *proc,int *exit,char *cmd,int cmdlen,char *buf,int buflen,int *bufret,int rtime,int rint)
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
	if(buflen<2)
		return(LVP_EC_NO_LEN);

	// nothing read yet
	int dret=0;
	if(bufret)
		*bufret=0;

	// --- flush stdout data ---
	if(cmd && cmdlen)
	{
		debug_printf(proc,"sending command - flushing stdout\n");

		// flush data - fast readout with minimum timeout
		proc_flush_stdout(proc,&done,30);

		// return exit code status
		if(exit)
			*exit=done;

		// leave if process instance returned exit code
		if(done && !exit)
			return(LVP_EC_EXITED);
		else if(done)
			return(0);
	}

	// --- write command ---
	if(cmd && cmdlen)
	{
		debug_printf(proc,"sending command - writting new command\n");

		// something to write
		int written;
		ret=proc_write_stdin(proc,cmd,cmdlen,&written);

		// general write fail
		if(ret)
			return(LVP_EC_WRITE_FAIL);
		else if(cmdlen!=written)
			return(LVP_EC_WRITE_INCOMPLETE);
	}

	// get processor frequency
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	debug_printf(proc,"sending command - waiting for answer\n");

	// --- try to read response ---
	LARGE_INTEGER t_last;
	QueryPerformanceCounter(&t_last);
	buflen--;
	do{
		// peek data
		int read;
		proc_peek_stdout(proc,&done,buf,buflen,&read,NULL);

		// move return buffer pointer
		buf+=read;
		*buf='\0';
		buflen-=read;
		dret+=read;

		// return exit code status
		if(exit)
			*exit=done;

		// return read count
		if(bufret)
			*bufret=dret;

		// error if process returned exit code
		if(done && !exit)
			return(LVP_EC_EXITED);
		else if(done)
			return(0);

		// full buffer - done
		if(!buflen)
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
			t_last=t_new;
		}

		// read interval timeout?
		if(dret && time_get_ms(&t_last,&t_new,&freq)>=rint)
		{
			// yaha, done
			return(0);
		}

		// wait a moment
		Sleep(5);

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
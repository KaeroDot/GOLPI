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
#include <stdio.h>
#include "lv_proc_main.h"

//---------------------------------------------------------------------------
// DLL main
//---------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
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
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_create(TPHndl *proc,char *folder,char *cmd,int sterr,int hide)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

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
		return(LVP_EC_CANT_CREATE_PIPE);
	}
	// stdout pipe
	if(!CreatePipe(&proc->pout[0],&proc->pout[1],&sa,0))
	{
		// failed - close pipes and leave
		CloseHandle(proc->pinp[0]);
		CloseHandle(proc->pinp[1]);
		return(LVP_EC_CANT_CREATE_PIPE);
	}

	// --- create process startup info ---
	// general setup
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb=sizeof(si);
	si.wShowWindow=hide?SW_HIDE:0;
	si.dwFlags|=STARTF_USESHOWWINDOW;
	// assign pipes
	si.dwFlags|=STARTF_USESTDHANDLES;
	si.hStdOutput=proc->pout[1];
	si.hStdInput=proc->pinp[1];
	si.hStdError=sterr?proc->pout[1]:NULL;

	// --- try to crate process ---
	if(!CreateProcess(NULL,cmd,NULL,NULL,true,0/*CREATE_NEW_CONSOLE*/,NULL,folder,&si,&pi))
	{
		// failed - close handles
		CloseHandle(proc->pinp[0]);
		CloseHandle(proc->pinp[1]);
		CloseHandle(proc->pout[0]);
		CloseHandle(proc->pout[1]);

		return(LVP_EC_CANT_CREATE_PROC);
	}

	// return process handles and ids
	proc->hproc=pi.hProcess;
	proc->hth=pi.hThread;
	proc->pid=pi.dwProcessId;
	proc->tid=pi.dwThreadId;

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

	// close handles
	CloseHandle(proc->pinp[0]);
	CloseHandle(proc->pinp[1]);
	CloseHandle(proc->pout[0]);
	CloseHandle(proc->pout[1]);
	CloseHandle(proc->hth);
	CloseHandle(proc->hproc);

	// clear handle variables
	memset((void*)proc,0,sizeof(TPHndl));

	return(0);
}

//---------------------------------------------------------------------------
// Try to get process instance exti code.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_get_exit_code(TPHndl *proc,int *code)
{
	// return default exit code
	if(code)
		*code=(int)STILL_ACTIVE;

	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// get exit code
	DWORD ret;
	GetExitCodeProcess(proc->hproc,&ret);

  // return exit code
	if(code)
		*code=(int)ret;

	// no exit code yet
	if(ret==STILL_ACTIVE)
		return(LVP_EC_NO_EXIT);

	return(0);
}

//---------------------------------------------------------------------------
// Wait for process exit, cleanup. Set time to 0 if no timeout requested.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_wait_exit(TPHndl *proc,int *code,int time)
{
	DWORD ret;

	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

  // get processor frequency aand initial time
	LARGE_INTEGER freq;
	LARGE_INTEGER t_last;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t_last);

	// wait for process exit code
	do{
		GetExitCodeProcess(proc->hproc,&ret);
		if(ret!=STILL_ACTIVE)
			Sleep(5);

		// check timeout
		LARGE_INTEGER t_new;QueryPerformanceCounter(&t_new);
		if(ret==STILL_ACTIVE && time && time_get_ms(&t_last,&t_new,&freq)>=time)
			break;
		t_last=t_new;

	}while(ret==STILL_ACTIVE);

	// cleanup handles
	if(ret!=STILL_ACTIVE)
		proc_cleanup(proc);

	// return exit code if required
	if(code)
		*code=(int)ret;

	// return error
	if(ret==STILL_ACTIVE)
		return(LVP_EC_EXIT_TO);

	return(0);
}

//---------------------------------------------------------------------------
// Terminate process.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_terminate(TPHndl *proc,int time)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// terminate process (async)
	if(TerminateProcess(proc->hproc,0)==0)
		return(LVP_EC_TERM_FAILED);

	// wait for exit
	return(proc_wait_exit(proc,NULL,time));
}

//---------------------------------------------------------------------------
// Flush stdout pipe data. 'rint' is interval between data blocks.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_flush_stdout(TPHndl *proc,int *exit,int rint)
{
	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// flush only if command is to be send
	int done;
	int tord;
	do{
		// read chunk
		char wbuf[4096];
		proc_peek_stdout(proc,&done,wbuf,4096,NULL,&tord);
		if(!tord)
		{
			// wait and check again
			Sleep(rint);
			proc_peek_stdout(proc,&done,NULL,0,NULL,&tord);
		}
	}while(tord && !done);

	// return exit code status
	if(exit)
		*exit=done;

	// signalize erro if exited and no exit pointer
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
	if(!proc)
		return(LVP_EC_NO_PROC);

	// leave if no data buffer
	if(!buf)
		return(LVP_EC_NO_BUF);

	// leave if nothing to write
	if(!towr)
		return(0);

	// try to write data buffer
	DWORD wrt;
	int ret=WriteFile(proc->pinp[0],(void*)buf,towr,&wrt,NULL);

	// return written count
	if(written)
		*written=(int)wrt;

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
	if(!proc)
		return(LVP_EC_NO_PROC);

	// reserve '\0' in buffer size
	bsize=(bsize>0)?(bsize-1):0;

	// nothing read yet
	int read=0;

	// not done yet
	int done=0;

	// nothing to return yet
	DWORD ret=0;

	int ptord;
	do{
		// peek pipe
		PeekNamedPipe(proc->pout[0],NULL,0,NULL,(DWORD*)&ptord,NULL);

		// limit data block to buffer size
		if(ptord>bsize)
			ptord=bsize;

		// read pipe data into buffer
		if(buf && ptord)
		{
			DWORD bread;
			ReadFile(proc->pout[0],buf,ptord,&bread,NULL);
			bsize-=bread;
			buf+=bread;
			*buf='\0';
			read+=bread;
		}

		// check process end if not returned yet
		if(!done)
		{
			// check process status
			GetExitCodeProcess(proc->hproc,&ret);
			// process done?
			done=(ret!=STILL_ACTIVE);
		}

		// peek pipe again to get data amount
		PeekNamedPipe(proc->pout[0],NULL,0,NULL,(DWORD*)&ptord,NULL);

		// repeat if something to read and some place in buffer
	}while(done && ptord && bsize);

	// return exit status
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
//  rtime - first answer byte timeout [ms]
//  rint  - maximum receive interval between successive data blocks [ms]
//
// In case no command is sent, function only reads stdout pipe.
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int proc_command(TPHndl *proc,int *exit,char *cmd,int cmdlen,char *buf,int buflen,int *bufret,int rtime,int rint)
{
	int ret;
	int done;

	// leave if no proc handle
	if(!proc)
		return(LVP_EC_NO_PROC);

	// leave if missing data buffer
	if(!buf)
		return(LVP_EC_NO_BUF);

	// no input buffer
	if(!buflen)
		return(LVP_EC_NO_LEN);

	// nothing read yet
	int dret=0;
	if(bufret)
		*bufret=0;

	// --- flush stdout data ---
	if(cmd && cmdlen)
	{
		// flush data
		proc_flush_stdout(proc,&done,rint);
		
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
    // something to write
		int written;
		ret=proc_write_stdin(proc,cmd,cmdlen,&written);

		// generatl write fail
		if(ret)
			return(LVP_EC_WRITE_FAIL);
		else if(cmdlen!=written)
			return(LVP_EC_WRITE_INCOMPLETE);
	}

	// get processor frequency
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

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

		// check answer timeout
		if(!dret && time_get_ms(&t_last,&t_new,&freq)>=rtime)
		{
			// timeout - error
			return(LVP_EC_TIMEOUT);
		}

		// byte recieved?
		if(read)
		{
			// yaha - last time
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
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

#ifndef lv_procH
#define lv_procH
//---------------------------------------------------------------------------
#include <windows.h>

#ifdef _LVPDLLEXPORT
#define DllExport __declspec(dllexport) 
#else
#define DllExport __declspec(dllimport) 
#endif

// --- process stdout fifo ---
typedef struct{
  HANDLE th;
	int exit;
	char *data;
	char *read;
	char *write;
	int len;
	CRITICAL_SECTION cs;
	int c_stdout_bytes;
	int c_stdin_bytes;
}TLVPFifo;

// --- process instance handles structure ---
typedef struct{
	// process and main thread handles and ids
	HANDLE hproc;
	HANDLE hth;
	DWORD pid;
	DWORD tid;
	HANDLE rd_event;
	// pipes:
	// ...[0] ends for caller reads/writes
	// ...[1] ends attached to the process instance,
	HANDLE pout[2];
	HANDLE pinp[2];
	// optional console:
	// stdout
	HANDLE cout;
	// colors
	WORD clr_in;
	WORD clr_out;
	// stdout FIFO:
    TLVPFifo *fifo;
	// debug
	wchar_t dbg_path[MAX_PATH];
	// config
  int read_th_idle;
}TLVPHndl;


#ifdef _LVPDLLEXPORT
// --- configuration ---
typedef struct{
	int th_priority;
	int th_idle;
	int no_hide;
	short console_x;
	short console_y;
	WORD console_clr_stdin;
	WORD console_clr_stdout;
	int write_pipe_buf;
 	int read_pipe_buf;
}TCfg;
#endif

// --- error codes ---
#define LVP_EC_NO_PROC 0x0001 /*missing instance handle*/
#define LVP_EC_CANT_CREATE_PIPE 0x0002 /*can't create pipe*/
#define LVP_EC_CANT_CREATE_PROC 0x0003 /*can't create process*/
#define LVP_EC_NO_EXIT 0x0004 /*process is still running*/
#define LVP_EC_EXIT_TO 0x0005 /*exit code query timeout*/
#define LVP_EC_NO_BUF 0x0010 /*no data buffer assigned*/
#define LVP_EC_NO_LEN 0x0011 /*data buffer has zero length*/
#define LVP_EC_WRITE_FAIL 0x0012 /*writting to stdin pipe failed*/
#define LVP_EC_WRITE_INCOMPLETE 0x0013 /*incomplete command write*/
#define LVP_EC_EXITED 0x0014 /*process returned exit code*/
#define LVP_EC_TIMEOUT 0x0015 /*command response timeout*/
#define LVP_EC_TERM_FAILED 0x0016 /*process termination failed*/
#define LVP_EC_SMALL_BUF 0x0020 /*buffer to small for error string*/
#define LVP_EC_CONS_CRAETE_FAILED 0x0030 /*debug console creation failed*/
#define LVP_EC_STDOUT_RD_TH_FAILED 0x0040 /*creation of stdout readout thread failed*/
#define LVP_EC_STDOUT_FIFO_FAILED 0x0041 /*allocation of the stdout fifo buffer failed*/
#define LVP_EC_STDOUT_EVENT_FAILED 0x0042 /*creating wakup event of the stdout fifo buffer failed*/


#ifdef _LVPDLLEXPORT

// --- constants ---
#define STDOUT_FIFO_BUF_LEN 1048576
#define STDOUT_TH_BUF_SIZE 32768
#define STDOUT_TH_UPDATE_TIME 1500

// configuation file name
#define LVPROC_INI L"lv_proc.ini"
#define LVPROC_DBG L"debug.log"


// --- internal prototypes ---
// debugs
int debug_init(TLVPHndl *proc,char *path);
int debug_printf(TLVPHndl *proc,const char *fmt,...);
// general
wchar_t *build_path(wchar_t *dest,wchar_t *p1,wchar_t *p2,int maxlen);
void strip_path(wchar_t *path,int size,wchar_t **name);
// inis
WORD ini_parse_color(wchar_t *str,WORD clr_in,wchar_t *clr_str_out,int size);
WORD ini_parse_color(wchar_t *str);
int ini_read_ini(TCfg *cfg,int *dbg);
// stdout fifo
int fifo_alloc(TLVPHndl *proc,int size);
int fifo_free(TLVPHndl *proc);
int fifo_to_write(TLVPHndl *proc,int *len);
int fifo_to_read(TLVPHndl *proc,int *len);
int fifo_clear(TLVPHndl *proc);
int fifo_write(TLVPHndl *proc,char *data,int towr,int *written);
int fifo_read(TLVPHndl *proc,char *data,int tord,int *read);
DWORD WINAPI fifo_read_thread(LPVOID lpParam);
// other
wchar_t *fmt_capacity(wchar_t *str,int maxstr,int size);
int peek_stdout(TLVPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);
int time_get_ms(LARGE_INTEGER *t1,LARGE_INTEGER *t2,LARGE_INTEGER *f);
int peek_stdout(TLVPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);
#endif




//---------------------------------------------------------------------------------------------------------------------
// DLL prototypes
//---------------------------------------------------------------------------------------------------------------------

//====== GENERAL ======
//---------------------------------------------------------------------------------------------------------------------
// fills the string with DLL version info
//  *str: string buffer to be filled with the version ASCII string
//  maxlen: size of string buffer
DllExport void proc_get_dll_version(char *str,__int32 maxlen);

//---------------------------------------------------------------------------
// Formats error code to string buffer.
//  code: lv proc error code
//  *str: string buffer
//  buflen: string buffer size
DllExport __int32 proc_format_error(__int32 code,char *str,__int32 buflen);

//---------------------------------------------------------------------------
// Return size of the lv process handle TPHndl in bytes (usefull in LabVIEW).
DllExport __int32 proc_handle_size(void);


//====== CREATE/KILL/CLEANUP ======
//---------------------------------------------------------------------------
// Try to create process and its pipes.
// Closes all opened handles if not successfull.
//  *proc: lv process instance handle
//  *folder: working directory for the process
//  *cmd: the command to execute, i.e. path to the executable of the process
//        with eventual parameters, so for instance "cmd.exe" to run wincmd
//  sterr: write 1 to combine stderr to stdout
//  hide: write 1 to hide console
DllExport __int32 proc_create(TLVPHndl *proc,char *folder,char *cmd,__int32 sterr,__int32 hide);

//---------------------------------------------------------------------------
// Close process instance handle. Call this to cleanup after the process has terminated.
//  *proc: lv process instance handle
DllExport __int32 proc_cleanup(TLVPHndl *proc);

//---------------------------------------------------------------------------
// Try to get process instance exit code.
// Returns LVP_EC_NO_EXIT error if still running.
//  *proc: lv process instance handle
//  *code: pointer to variable that receives the exit code (optional)
DllExport __int32 proc_get_exit_code(TLVPHndl *proc,__int32 *code);

//---------------------------------------------------------------------------
// Wait for process exit code. Set time to 0 if no timeout requested.
//  *proc: lv process instance handle
//  *code: variable that receives exit code (optional)
//  time: timeout in ms, rather don't use 0 :)
DllExport __int32 proc_wait_exit(TLVPHndl *proc,__int32 *code,__int32 time);

//---------------------------------------------------------------------------
// Harcore process termination. Call this when the process does not behave or it cannot be 
// terminated at all.
//  *proc: lv process instance handle
//  time: timeout in ms
DllExport __int32 proc_terminate(TLVPHndl *proc,__int32 time);



//====== READ/WRITE ======
//---------------------------------------------------------------------------
// Flush stdout pipe data. 'rint' [ms] is maximum interval between incomming
// stdout data blocks.
// Function returns if no data appears on stdout for 'rint'.
//  *proc: lv process instance handle
//  *exit: variable that receives exit code if process exited (optional)
//  rint: maximum 'gap' between incomming stdout data in ms
DllExport __int32 proc_flush_stdout(TLVPHndl *proc,__int32 *exit,__int32 r__int32);

//---------------------------------------------------------------------------
// Write to stdin pipe.
//  *proc: lv process instance handle
//  *buf: data to write
//  towr: number of bytes to write, use negative number to take 'buf' as
//        a null terminated string and detect size automatically, where the 
//        absolute value of 'towr' is maximum expected size of 'buf' data
//        (safety solution for strnlen_s())
//  *written: returns number of actually written bytes (optional)
DllExport __int32 proc_write_stdin(TLVPHndl *proc,char *buf,__int32 towr,__int32 *written);

//---------------------------------------------------------------------------
// Peek process stdout pipe, read upto 'blen' chars, return process status.
//  *proc: lv process instance handle
//  *exit: returns exit code if process returned (optional)
//  *buf: read buffer
//  bsize: byte size of the read buffer
//  *rread: returns total read bytes (optional)
//  *rtord: returns remaining bytes to read (optional)
DllExport __int32 proc_peek_stdout(TLVPHndl *proc,__int32 *exit,char *buf,__int32 bsize,__int32 *rread,__int32 *rtord);

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
DllExport __int32 proc_command(TLVPHndl *proc,__int32 *exit,char *cmd,__int32 cmdlen,char *buf,__int32 buflen,__int32 *bufret,__int32 rtime,__int32 rint);

#endif

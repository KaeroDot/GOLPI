//---------------------------------------------------------------------------
#ifndef lv_proc_mainH
#define lv_proc_mainH
//---------------------------------------------------------------------------

// --- process instance handles structure ---
typedef struct{
	// process and main thread handles and ids
	HANDLE hproc;
	HANDLE hth;
	DWORD pid;
	DWORD tid;
	// pipes:
	// ...[0] ends for caller reads/writes
	// ...[1] ends attached to the process instance,
	HANDLE pout[2];
	HANDLE pinp[2];
}TPHndl;


// --- error codes ---
#define LVP_EC_NO_PROC 0x0001
#define LVP_EC_CANT_CREATE_PIPE 0x0002
#define LVP_EC_CANT_CREATE_PROC 0x0003
#define LVP_EC_NO_EXIT 0x0004
#define LVP_EC_EXIT_TO 0x0005
#define LVP_EC_NO_BUF 0x0010
#define LVP_EC_NO_LEN 0x0011
#define LVP_EC_WRITE_FAIL 0x0012
#define LVP_EC_WRITE_INCOMPLETE 0x0013
#define LVP_EC_EXITED 0x0014
#define LVP_EC_TIMEOUT 0x0015
#define LVP_EC_TERM_FAILED 0x0016
#define LVP_EC_SMALL_BUF 0x0020


// --- internal prototypes ---
int time_get_ms(LARGE_INTEGER *t1,LARGE_INTEGER *t2,LARGE_INTEGER *f);

// --- external prototypes ---
extern "C" __declspec(dllexport) int proc_format_error(int code,char *str,int buflen);
extern "C" __declspec(dllexport) int proc_handle_size(void);
extern "C" __declspec(dllexport) int proc_create(TPHndl *proc,char *folder,char *cmd,int sterr,int hide);
extern "C" __declspec(dllexport) int proc_cleanup(TPHndl *proc);
extern "C" __declspec(dllexport) int proc_get_exit_code(TPHndl *proc,int *code);
extern "C" __declspec(dllexport) int proc_wait_exit(TPHndl *proc,int *code,int time);
extern "C" __declspec(dllexport) int proc_terminate(TPHndl *proc,int time);
extern "C" __declspec(dllexport) int proc_flush_stdout(TPHndl *proc,int *exit,int rint);
extern "C" __declspec(dllexport) int proc_write_stdin(TPHndl *proc,char *buf,int towr,int *written);
extern "C" __declspec(dllexport) int proc_peek_stdout(TPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);
extern "C" __declspec(dllexport) int proc_command(TPHndl *proc,int *exit,char *cmd,int cmdlen,char *buf,int buflen,int *bufret,int rtime,int rint);

#endif

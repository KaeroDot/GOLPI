//---------------------------------------------------------------------------
#ifndef lv_proc_mainH
#define lv_proc_mainH
//---------------------------------------------------------------------------

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
}TFifo;

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
	// optional console:
	// stdout
	HANDLE cout;
	// colors
	DWORD clr_in;
	DWORD clr_out;
	// stdout FIFO:
	TFifo *fifo;
	// debug
	char dbg_path[MAX_PATH];
}TPHndl;

// --- configuration ---
typedef struct{
  int no_hide;
	short console_x;
	short console_y;
	WORD console_clr_stdin;
	WORD console_clr_stdout;
}TCfg;

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
#define LVP_EC_CONS_CRAETE_FAILED 0x0030
#define LVP_EC_STDOUT_RD_TH_FAILED 0x0040
#define LVP_EC_STDOUT_FIFO_FAILED 0x0041

// --- constants ---
#define STDOUT_FIFO_BUF_LEN 1048576
#define STDOUT_TH_BUF_SIZE 8192
#define STDOUT_TH_UPDATE_TIME 1500

// --- internal prototypes ---
// debugs
int debug_init(TPHndl *proc,char *path);
int debug_printf(TPHndl *proc,const char *fmt,...);
// inis
WORD ini_parse_color(char *str,WORD clr_in,char *clr_str_out,int size);
int ini_read_ini(TCfg *cfg,int *dbg);
// stdout fifo
int fifo_alloc(TPHndl *proc,int size);
int fifo_free(TPHndl *proc);
int fifo_to_write(TPHndl *proc,int *len);
int fifo_to_read(TPHndl *proc,int *len);
int fifo_clear(TPHndl *proc);
int fifo_write(TPHndl *proc,char *data,int towr,int *written);
int fifo_read(TPHndl *proc,char *data,int tord,int *read);
DWORD WINAPI fifo_read_thread(LPVOID lpParam);
// other
int peek_stdout(TPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);
int time_get_ms(LARGE_INTEGER *t1,LARGE_INTEGER *t2,LARGE_INTEGER *f);
int peek_stdout(TPHndl *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);

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

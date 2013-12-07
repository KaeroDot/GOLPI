//---------------------------------------------------------------------------
#include <aclapi.h>
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "dll_test.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

extern "C" __declspec(dllimport) int proc_format_error(int code,char *str,int buflen);
extern "C" __declspec(dllimport) int proc_handle_size(void);
extern "C" __declspec(dllexport) int proc_create(void *proc,char *folder,char *cmd,int sterr,int hide);
extern "C" __declspec(dllimport) int proc_cleanup(void *proc);
extern "C" __declspec(dllimport) int proc_get_exit_code(void *proc,int *code);
extern "C" __declspec(dllimport) int proc_wait_exit(void *proc,int *code,int time);
extern "C" __declspec(dllimport) int proc_terminate(void *proc,int *code,int time);
extern "C" __declspec(dllimport) int proc_write_stdin(void *proc,char *buf,int towr,int *written);
extern "C" __declspec(dllimport) int proc_peek_stdout(void *proc,int *exit,char *buf,int bsize,int *rread,int *rtord);
extern "C" __declspec(dllimport) int proc_command(void *proc,int *exit,char *cmd,int cmdlen,char *buf,int buflen,int *bufret,int rtime,int rint);

#define BUFLEN 8192

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	stat=0;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::BtnRunClick(TObject *Sender)
{
	if(!stat)
	{
		// --- run process ---

		// allocate process handles memory
		proc=malloc(proc_handle_size());

		// create process
		int ret=proc_create(proc,Epath->Text.c_str(),(Epath->Text+"//"+Ecmd->Text).c_str(),1,!CBconsole->Checked);

		// clear stdout
		Memo->Clear();

		if(!ret)
		{
			stat=1;
			BtnRun->Caption="Stop";
			Caption="Process is running ...";
		}
     
		/*int exit;
		int read;
		char buf[BUFLEN];
		proc_command(proc,&exit,buf,0,buf,BUFLEN,&read,20000,1000);
		Memo->Text=AnsiString(buf);*/

	}
	else
	{
		// --- exit process ---

    int written;
		proc_write_stdin(proc,"exit\n",5,&written);
		int code;
		if(proc_wait_exit(proc,&code,5000))
			proc_terminate(proc,&code,5000);

		proc_cleanup(proc);

		free((void*)proc);

		// exit
		stat=0;
    
		BtnRun->Caption="Run";
		Caption="STOPPED - process has returned exit code.";
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ParseStdout(TMemo *memo,char *buf,int read)
{
	char eols[3]={'\r','\n','\0'};
	if(read)
	{
		char *str=&buf[0];
		char *pend=&buf[read];
		char *pstr;
		do{

			// EOL at CR+LF or LF
			int eol=0;
			pstr=strpbrk(str,"\r\n");
			if(pstr && *pstr=='\r')
			{
				// CR
				*pstr++='\0';

				// CR+LF?
				if(pstr<pend && *pstr=='\n')
				{
					*pstr++='\0';
					eol=1;
				}
			}
			else if(pstr && *pstr=='\n')
			{
				// LF
				*pstr++='\0';
				eol=1;
			}

			// write data before EOL
			if(str[0]!='\0')
				memo->Text=Memo->Text+AnsiString(str);

			// write EOL mark eventually
			if(eol)
				memo->Text=Memo->Text+AnsiString(eols);

			// and skip original EOL data
			if(pstr)
				str=pstr;

		}while(pstr && str<pend);

		SendMessage(memo->Handle,EM_LINESCROLL,0,1024);
	}
}

void __fastcall TForm1::timerTimer(TObject *Sender)
{
	if(stat)
	{
		// try to read stdout
		int done;
		int read;
		int tord;
		char buf[BUFLEN];
		proc_peek_stdout(proc,&done,buf,BUFLEN,&read,&tord);

		// parse data into memo
		ParseStdout(Memo,buf,read);

		if(done)
			BtnRunClick(NULL);
	}


}
//---------------------------------------------------------------------------


void __fastcall TForm1::EinputKeyPress(TObject *Sender, char &Key)
{
	if(Key==VK_RETURN)
	{
		char buf[BUFLEN];
		strcpy(buf,Einput->Text.c_str());
		int len=strlen(buf);
		buf[len++]='\r';
		buf[len++]='\n';
		buf[len]='\0';

		int written;
		proc_write_stdin(proc,buf,len,&written);

		//Einput->Text="";
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::BtnCmdClick(TObject *Sender)
{
	char buf[BUFLEN];
	strcpy(buf,Einput->Text.c_str());
	int len=strlen(buf);
	buf[len++]='\r';
	buf[len++]='\n';

	int bufret;
	proc_command(proc,NULL,buf,len,buf,BUFLEN,&bufret,1000,100);

	Memo->Clear();
	ParseStdout(Memo,buf,bufret);
}
//---------------------------------------------------------------------------









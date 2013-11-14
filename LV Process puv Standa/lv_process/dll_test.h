//---------------------------------------------------------------------------

#ifndef dll_testH
#define dll_testH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *BtnRun;
	TMemo *Memo;
	TEdit *Epath;
	TEdit *Ecmd;
	TEdit *Einput;
	TTimer *timer;
	TButton *BtnCmd;
	TButton *Button1;
	void __fastcall BtnRunClick(TObject *Sender);
	void __fastcall timerTimer(TObject *Sender);
	void __fastcall EinputKeyPress(TObject *Sender, char &Key);
	void __fastcall BtnCmdClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	void __fastcall ParseStdout(TMemo *memo,char *buf,int read);

	int stat;
	void *proc;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

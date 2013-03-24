//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include "CPort.hpp"
#include "CPortCtl.hpp"
#include <Vcl.ExtCtrls.hpp>

extern "C" {
#include "layer2p.h"
}

//---------------------------------------------------------------------------
class TFMain : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TTrackBar *tbQEI1;
	TTrackBar *tbQEI2;
	TTrackBar *tbQEI6;
	TTrackBar *tbQEI4;
	TTrackBar *tbQEI3;
	TTrackBar *tbQEI5;
	TComPort *ComPort1;
	TTrackBar *tbGoal1;
	TTrackBar *tbGoal4;
	TTrackBar *tbGoal5;
	TTrackBar *tbGoal2;
	TTrackBar *tbGoal6;
	TLabeledEdit *leKp1;
	TLabeledEdit *leKd1;
	TLabeledEdit *leKi1;
	TLabeledEdit *leKp2;
	TLabeledEdit *leKd2;
	TLabeledEdit *leKi2;
	TLabeledEdit *leKp3;
	TLabeledEdit *leKd3;
	TLabeledEdit *leKi3;
	TLabeledEdit *leKp4;
	TLabeledEdit *leKd4;
	TLabeledEdit *leKi4;
	TLabeledEdit *leKp5;
	TLabeledEdit *leKd5;
	TLabeledEdit *leKi5;
	TLabeledEdit *leKp6;
	TLabeledEdit *leKd6;
	TLabeledEdit *leKi6;
	TPanel *Panel1;
	TGroupBox *GroupBox4;
	TComLed *ComLed2;
	TButton *Button7;
	TButton *Button8;
	TButton *Button9;
	TGroupBox *GroupBox5;
	TButton *Button10;
	TButton *Button11;
	TButton *Button12;
	TTrackBar *tbGoal3;
	TTrackBar *tbQEISet3;
	TTrackBar *tbQEISet2;
	TTrackBar *tbQEISet1;
	TTrackBar *tbQEISet4;
	TTrackBar *tbQEISet5;
	TTrackBar *tbQEISet6;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall ComPort1RxChar(TObject *Sender, int Count);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall SetQEIS(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall ComPort1RxFlag(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
private:	// User declarations
	layer2p_ctx l2ctx;
public:		// User declarations
	__fastcall TFMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFMain *FMain;
//---------------------------------------------------------------------------
#endif
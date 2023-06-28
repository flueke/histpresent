#include "TROOT.h"
#include "THprCurlyLineWithArrow.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TObjString.h"
#include "TGMrbValuesAndText.h"
#include "CurlyLineWithArrowDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(CurlyLineWithArrowDialog)

CurlyLineWithArrowDialog::CurlyLineWithArrowDialog()
{
   cout << "InsertCurlyLineWithArrow()" << endl;
static const Char_t helptext[] =
"This widgets inserts a curly / wavy line with optional\n\
arrows at the ends\n\
";
   fCanvas = gPad->GetCanvas();
   TRootCanvas* win = NULL;
   if (fCanvas) {
      win = (TRootCanvas*)fCanvas->GetCanvasImp();
      win->Connect("CloseWindow()", "CurlyLineWithArrowDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
   static TString exc("ExecuteInsert()");
   fRow_lab->Add(new TObjString("DoubleValue_Ampl"));
   fValp[ind++] = &fWaveLength;
   fRow_lab->Add(new TObjString("DoubleValue+WaveL"));
   fValp[ind++] = &fAmplitude;
   fRow_lab->Add(new TObjString("ColorSelect_LColor"));
   fValp[ind++] = &fColor;
   fRow_lab->Add(new TObjString("PlainShtVal+LWidth"));
   fValp[ind++] = &fWidth;
   fRow_lab->Add(new TObjString("LineSSelect+LStyle"));
   fValp[ind++] = &fStyle;
   fRow_lab->Add(new TObjString("CheckButton_Curly"));
   fValp[ind++] = &fCurly;
   fRow_lab->Add(new TObjString("CheckButton+Arrow@Start"));
   fValp[ind++] = &fArrowAtStart;
   fRow_lab->Add(new TObjString("CheckButton+Arrow@End"));
   fValp[ind++] = &fArrowAtEnd;
   fRow_lab->Add(new TObjString("Float_Value_ArLength"));
   fValp[ind++] = &fArrowSize;
//   fRow_lab->Add(new TObjString("DoubleValue+ArAngle"));
//   fValp[ind++] = &fArrowAngle;
   fRow_lab->Add(new TObjString("CommandButt_Execute_Insert"));
   fValp[ind++] = &exc;

   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Curly / Wave line with arrow", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
       GrCanvas* hc = (GrCanvas*)fCanvas;
       hc->Add2ConnectedClasses(this);
   }
}
//______________________________________________________________________________

void CurlyLineWithArrowDialog::ExecuteInsert()
{
//   const char * ArrowOption[] =
//     {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
   TLine * gr = (TLine*)GrCanvas::WaitForCreate("TLine", &fPad);
	if (gr == NULL) {
		return;
	}
   Int_t where = 0;
   if (fArrowAtStart) where += 1;
   if (fArrowAtEnd)   where += 2;
   THprCurlyLineWithArrow* cwa =
     new THprCurlyLineWithArrow(gr->GetX1(), gr->GetY1(), gr->GetX2(), gr->GetY2(),
                             fWaveLength, fAmplitude, where, fArrowSize);
   cwa->TPolyLine::Draw();
   cwa->SetLineColor(fColor);
   cwa->SetLineWidth(fWidth);
   cwa->SetLineStyle(fStyle);
   if (fCurly) cwa->SetCurly();
   else        cwa->SetWavy();
   delete gr;
//   cwa->SetDrawOption(ArrowOption[fArrowStyle]);
//   cwa->SetOption(ArrowOption[fArrowStyle]);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void CurlyLineWithArrowDialog::SaveDefaults()
{
   cout << "CurlyLineWithArrowDialog::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("CurlyLineWithArrowDialog.fWaveLength" , fWaveLength );
   env.SetValue("CurlyLineWithArrowDialog.fAmplitude"  , fAmplitude  );
   env.SetValue("CurlyLineWithArrowDialog.fColor"		 , fColor 	   );
   env.SetValue("CurlyLineWithArrowDialog.fWidth"		 , fWidth 	   );
   env.SetValue("CurlyLineWithArrowDialog.fStyle"      , fStyle      );
   env.SetValue("CurlyLineWithArrowDialog.fCurly"      , fCurly      );
   env.SetValue("CurlyLineWithArrowDialog.fArrowAngle" , fArrowAngle );
   env.SetValue("CurlyLineWithArrowDialog.fArrowSize"  , fArrowSize  );
   env.SetValue("CurlyLineWithArrowDialog.fArrowStyle" , fArrowStyle );
   env.SetValue("CurlyLineWithArrowDialog.fArrowAtStart", fArrowAtStart );
   env.SetValue("CurlyLineWithArrowDialog.fArrowAtEnd"  , fArrowAtEnd );
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void CurlyLineWithArrowDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fWaveLength = env.GetValue("CurlyLineWithArrowDialog.fWaveLength" , 0.02);
   fAmplitude  = env.GetValue("CurlyLineWithArrowDialog.fAmplitude"  , 0.01);
   fColor   	= env.GetValue("CurlyLineWithArrowDialog.fColor"      , 1);
   fStyle      = env.GetValue("CurlyLineWithArrowDialog.fStyle"      , 1);
   fCurly      = env.GetValue("CurlyLineWithArrowDialog.fCurly"      , 0);
   fWidth   	= env.GetValue("CurlyLineWithArrowDialog.fWidth"  	   , 2);
   fArrowAngle = env.GetValue("CurlyLineWithArrowDialog.fArrowAngle" , 30.);
   fArrowSize  = env.GetValue("CurlyLineWithArrowDialog.fArrowSize"  , 0.03);
   fArrowStyle = env.GetValue("CurlyLineWithArrowDialog.fArrowStyle" , 3);
   fArrowAtStart = env.GetValue("CurlyLineWithArrowDialog.fArrowAtStart" , 0);
   fArrowAtEnd   = env.GetValue("CurlyLineWithArrowDialog.fArrowAtEnd" , 1);
}
//_________________________________________________________________________

CurlyLineWithArrowDialog::~CurlyLineWithArrowDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if ( fRow_lab ) {
		fRow_lab->Delete();
		delete fRow_lab;
	}

};
//_______________________________________________________________________

void CurlyLineWithArrowDialog::CloseDialog()
{
//   cout << "FeynmanDiagramDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void CurlyLineWithArrowDialog::CloseDown(Int_t wid)
{
   cout << "CurlyLineWithArrowDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}


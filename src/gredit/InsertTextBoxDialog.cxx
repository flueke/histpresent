#include "TROOT.h"
#include "TBox.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TMarker.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TPavesText.h"
//#include "TString.h"
#include "THprTextBox.h"
#include "InsertTextBoxDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertTextBoxDialog)

InsertTextBoxDialog::InsertTextBoxDialog()
{
static const Char_t helptext[] =
"This widget is for text and boxes. These objects\n\
may be arranged automatically at the corners or centered\n\
at the edges. It also may adopt other objects like polylines\n\
xsplines which end at its borders. In this case these objects remain\n\
attached if the text box gets shifted.\n\
If Npaves > 1 a root-standard TPavesText is drawn\n\
This has not the features of auto-align and adopt.\n\
";
   fCanvas = gPad->GetCanvas();
   TRootCanvas* win = NULL;
   if (fCanvas) {
      win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "InsertTextBoxDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
    static TString execute_cmd("ExecuteInsert()");
   fX1 = fY1 = 0;
   fRow_lab->Add(new TObjString("DoubleValue_  X0"));
   fValp[ind++] = &fX1;
   fRow_lab->Add(new TObjString("DoubleValue+  Y0"));
   fValp[ind++] = &fY1;
   fRow_lab->Add(new TObjString("DoubleValue_X Width"));
   fValp[ind++] = &fDx;
   fRow_lab->Add(new TObjString("DoubleValue+Y Width"));
   fValp[ind++] = &fDy;
   fRow_lab->Add(new TObjString("CheckButton_Show shadow"));
   fValp[ind++] = &fShowShadow;
   fRow_lab->Add(new TObjString("PlainIntVal+Shwid"));
   fValp[ind++] = &fBorderSize;
   fRow_lab->Add(new TObjString("AlignSelect+ShPos"));
   fValp[ind++] = &fShadowPosition;
   fRow_lab->Add(new TObjString("CheckButton_Round Corns"));
   fValp[ind++] = &fRoundCorners;
   fRow_lab->Add(new TObjString("DoubleValue+CornRad;0.0;0.5"));
   fValp[ind++] = &fCornerRadius;
   fRow_lab->Add(new TObjString("PlainIntVal+Npaves"));
   fValp[ind++] = &fNpaves;
   fRow_lab->Add(new TObjString("ColorSelect_LineColor"));
   fValp[ind++] = &fColor;
   fRow_lab->Add(new TObjString("LineSSelect+Style"));
   fValp[ind++] = &fStyle;
   fRow_lab->Add(new TObjString("PlainShtVal+ Width"));
   fValp[ind++] = &fWidth;
   fRow_lab->Add(new TObjString("ColorSelect_FillColor"));
   fValp[ind++] = &fFillColor;
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fValp[ind++] = &fFillStyle;
   fRow_lab->Add(new TObjString("RadioButton_No align"));
   fValp[ind++] = &fNoAlign;
   fRow_lab->Add(new TObjString("RadioButton+Weak align"));
   fValp[ind++] = &fWeakAlign;
   fRow_lab->Add(new TObjString("RadioButton+Strong align"));
   fValp[ind++] = &fStrongAlign;
   fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
   fValp[ind++] = &execute_cmd;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Text Box", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->Add2ConnectedClasses(this);
	}
}
//____________________________________________________________________________

void InsertTextBoxDialog::ExecuteInsert()
{
   TPave *m1 = NULL;
   TMarker *ma = NULL;
   Double_t X2=0, Y2=0;
   if ((fX1 == 0 && fY1 == 0) || (fDx == 0 || fDy == 0)) {
		fPad = NULL;
		if (fDx == 0 || fDy == 0) {
			cout << "Define a box " << endl;
			m1  = (TPave*)GrCanvas::WaitForCreate("TPave", &fPad);
			if (m1 == NULL) {
				cout << "Interrupted Input" << endl;
				return;
			}
			m1 = (TPave *)gPad->GetListOfPrimitives()->Last();
			if (!m1) return;
			fX1 = m1->GetX1();
			fY1 = m1->GetY1();
			X2 = m1->GetX2();
			Y2 = m1->GetY2();
		} else {
			cout << "Define lower left corner" << endl;
			ma  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
			if (ma == NULL) {
				cout << "Interrupted Input" << endl;
				return;
			}
			ma = (TMarker *)gPad->GetListOfPrimitives()->Last();
			if (!ma) return;
			fX1 = ma->GetX();
			fY1 = ma->GetY();
         X2 = fX1 + fDx;
         Y2 = fY1 + fDy;
		}
   } else {
      X2 = fX1 + fDx;
      Y2 = fY1 + fDy;
   }
   if (m1) delete m1;
   if (ma) delete ma;
   THprTextBox *tb;
   TPavesText *tpt;
   TPave *tp;
   if ( fNpaves <= 1 ) {
		tb = new THprTextBox(fX1, fY1, X2, Y2);
		if      (fNoAlign)   tb->SetAlignType(0);
		else if (fWeakAlign) tb->SetAlignType(1);
		else                 tb->SetAlignType(2);
		tb->Draw();
      tp = tb;
   } else {
      tpt = new TPavesText(fX1, fY1, X2, Y2, fNpaves);
		tpt->Draw();
      tp = tpt;
   }
	Int_t halign = fShadowPosition / 10;
	Int_t valign = fShadowPosition % 10;
	TString opt;
	if (valign == 3) opt += "T";
	else             opt += "B";
	if (halign == 1) opt += "L";
	else             opt += "R";
	if (fRoundCorners != 0) {
		fCornerRadius = TMath::Min(fCornerRadius, 0.49999);
		tp->SetCornerRadius(fCornerRadius);
		opt += "arc";
	}
   tp->SetDrawOption(opt);
//   cout <<"Draw(opt)  " << opt << endl;
   tp->SetLineColor(fColor);
   tp->SetLineStyle(fStyle);
   tp->SetLineWidth(fWidth);
   tp->SetFillColor(fFillColor);
   tp->SetFillStyle(fFillStyle);
   if (fShowShadow != 0) tp->SetBorderSize(fBorderSize);
   else                  tp->SetBorderSize(1);
// cout << opt << endl;
   fX1 = fY1 = 0;
   gPad->Modified();
   gPad->Update();
   SaveDefaults();
};
//____________________________________________________________________________

void InsertTextBoxDialog::SaveDefaults()
{
//   cout << "InsertTextBoxDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertTextBoxDialog.fDx"            ,fDx);
   env.SetValue("InsertTextBoxDialog.fDy"            ,fDy);
   env.SetValue("InsertTextBoxDialog.fColor"         ,fColor);
   env.SetValue("InsertTextBoxDialog.fStyle"         ,fStyle);
   env.SetValue("InsertTextBoxDialog.fWidth"         ,fWidth);
   env.SetValue("InsertTextBoxDialog.fFillColor"     ,fFillColor);
   env.SetValue("InsertTextBoxDialog.fFillStyle"     ,fFillStyle);
   env.SetValue("InsertTextBoxDialog.fCornerRadius"  ,fCornerRadius);
   env.SetValue("InsertTextBoxDialog.fRoundCorners"  ,fRoundCorners);
   env.SetValue("InsertTextBoxDialog.fShowShadow"    ,fShowShadow);
   env.SetValue("InsertTextBoxDialog.fBorderSize"    ,fBorderSize);
   env.SetValue("InsertTextBoxDialog.fShadowPosition",fShadowPosition);
   env.SetValue("InsertTextBoxDialog.fNoAlign"       ,fNoAlign);
   env.SetValue("InsertTextBoxDialog.fWeakAlign"     ,fWeakAlign);
   env.SetValue("InsertTextBoxDialog.fStrongAlign"   ,fStrongAlign);
   env.SetValue("InsertTextBoxDialog.fNpaves"        ,fNpaves);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertTextBoxDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fDx            = env.GetValue("InsertTextBoxDialog.fDx" ,0.);
   fDy            = env.GetValue("InsertTextBoxDialog.fDy" ,0.);
   fColor         = env.GetValue("InsertTextBoxDialog.fColor" ,1);
   fStyle         = env.GetValue("InsertTextBoxDialog.fStyle" ,1);
   fWidth         = env.GetValue("InsertTextBoxDialog.fWidth" ,2);
   fFillColor     = env.GetValue("InsertTextBoxDialog.fFillColor" ,1);
   fFillStyle     = env.GetValue("InsertTextBoxDialog.fFillStyle" ,1);
   fCornerRadius  = env.GetValue("InsertTextBoxDialog.fCornerRadius" ,0.02);
   fRoundCorners  = env.GetValue("InsertTextBoxDialog.fRoundCorners" ,0);
   fShowShadow    = env.GetValue("InsertTextBoxDialog.fShowShadow" ,1);
   fBorderSize    = env.GetValue("InsertTextBoxDialog.fBorderSize" ,4);
	if (fBorderSize > 10 || fBorderSize < 0)
		fBorderSize = 1;
   fShadowPosition= env.GetValue("InsertTextBoxDialog.fShadowPosition" ,31);
	if (fShadowPosition > 33 || fShadowPosition < 11)
		fShadowPosition = 31;
	
   fNoAlign       = env.GetValue("InsertTextBoxDialog.fNoAlign"    ,0);
   fWeakAlign     = env.GetValue("InsertTextBoxDialog.fWeakAlign"  ,1);
   fStrongAlign   = env.GetValue("InsertTextBoxDialog.fStrongAlign",0);
   fNpaves        = env.GetValue("InsertTextBoxDialog.fNpaves"     ,1);
}
//_________________________________________________________________________

InsertTextBoxDialog::~InsertTextBoxDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void InsertTextBoxDialog::CloseDialog()
{
//   cout << "InsertTextBoxDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertTextBoxDialog::CloseDown(Int_t wid)
{
//   cout << "InsertTextBoxDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}



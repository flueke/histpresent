#include "TROOT.h"
#include "TEnv.h"
//#include "TPad.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include "ChangeTextDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(ChangeTextDialog)

//_________________________________________________________________________

ChangeTextDialog::ChangeTextDialog()
{
static const Char_t helptext[] =
"no help yet\n\
";
	fCanvas = gPad->GetCanvas();
	TRootCanvas* win = NULL;
	if (fCanvas) {
		win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "ChangeTextDialog", this, "CloseDialog()");
	}
	Int_t ind = 0;
	fRow_lab = new TList();
	RestoreDefaults();
	fDoChangeTextSize = 0;
	fDoChangeTextAngle = 0;
	fDoChangeTextAlign = 0;
	fDoChangeTextColor = 0;
	fDoChangeTextFont = 0;
	fDoChangeTextPrec = 0;

	static TString excmd("ChangeTextExecute()");
	fRow_lab->Add(new TObjString("Float_Value_Min Size"));
	fValp[ind++] = &fChangeTextSizeMin;
	fRow_lab->Add(new TObjString("Float_Value+MaxSize"));
	fValp[ind++] = &fChangeTextSizeMax;

	fRow_lab->Add(new TObjString("Float_Value_New Size"));
	fValp[ind++] = &fChangeTextSize;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextSize;

	fRow_lab->Add(new TObjString("CfontSelect_New Font"));
	fValp[ind++] = &fChangeTextFont;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextFont;

	fRow_lab->Add(new TObjString("PlainIntVal_New Precission"));
	fValp[ind++] = &fChangeTextPrec;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextPrec;

	fRow_lab->Add(new TObjString("ColorSelect_New Color"));
	fValp[ind++] = &fChangeTextColor;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextColor;
	fRow_lab->Add(new TObjString("AlignSelect_New Alignment"));
	fValp[ind++] = &fChangeTextAlign;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextAlign;
	fRow_lab->Add(new TObjString("Float_Value_New Angle"));
	fValp[ind++] = &fChangeTextAngle;
	fRow_lab->Add(new TObjString("CheckButton+Do change"));
	fValp[ind++] = &fDoChangeTextAngle;
	fRow_lab->Add(new TObjString("CommandButt_Execute Change Text"));
	fValp[ind++] = &excmd;

	Int_t itemwidth =  35 * TGMrbValuesAndText::LabelLetterWidth();
	static Int_t ok;
	fDialog =
		new TGMrbValuesAndText("Modify Text", NULL, &ok,itemwidth, win,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
};
//_________________________________________________________________________

void ChangeTextDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("ChangeTextDialog.ChangeTextAlign"		, fChangeTextAlign  	 );
	env.SetValue("ChangeTextDialog.ChangeTextColor"		, fChangeTextColor  	 );
	env.SetValue("ChangeTextDialog.ChangeTextFont" 		, fChangeTextFont		 );
	env.SetValue("ChangeTextDialog.ChangeTextPrec" 		, fChangeTextPrec		 );
	env.SetValue("ChangeTextDialog.ChangeTextSize" 		, fChangeTextSize		 );
	env.SetValue("ChangeTextDialog.ChangeTextSizeMin"  , fChangeTextSizeMin  );
	env.SetValue("ChangeTextDialog.ChangeTextSizeMax"  , fChangeTextSizeMax  );
	env.SetValue("ChangeTextDialog.ChangeTextAngle"		, fChangeTextAngle  	 );
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void ChangeTextDialog::RestoreDefaults()
{
	TEnv env(".hprrc");

	fChangeTextAlign       = env.GetValue("ChangeTextDialog.ChangeTextAlign"  , 11);
	fChangeTextColor       = env.GetValue("ChangeTextDialog.ChangeTextColor"  , 1);
	fChangeTextFont        = env.GetValue("ChangeTextDialog.ChangeTextFont"   , 62);
	fChangeTextPrec        = env.GetValue("ChangeTextDialog.ChangeTextPrec"	  , 2);
	fChangeTextSize        = env.GetValue("ChangeTextDialog.ChangeTextSize"   , 0.02);
	fChangeTextSizeMin     = env.GetValue("ChangeTextDialog.ChangeTextSizeMin"   , 0.0);
	fChangeTextSizeMax     = env.GetValue("ChangeTextDialog.ChangeTextSizeMax"   , 1.0);
	fChangeTextAngle       = env.GetValue("ChangeTextDialog.ChangeTextAngle"  , 0);
}
//_________________________________________________________________________

void ChangeTextDialog::ChangeTextExecute()
{
	cout << "ChangeTextExecute" << endl;
	if (!gPad) return;
	TCutG * cut  = (TCutG*)gPad->GetListOfPrimitives()->FindObject("CUTG");
	TIter next(gPad->GetListOfPrimitives());
	TObject *obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TLatex")) {
			TLatex *t = (TLatex*)obj;
			if (cut) {
				if (!cut->IsInside(t->GetX(), t->GetY())) continue;
			}
			if (fDoChangeTextColor)
				t->SetTextColor(fChangeTextColor);
			if (fDoChangeTextSize) {
				if (t->GetTextSize() > fChangeTextSizeMin &&
					 t->GetTextSize() < fChangeTextSizeMax)
					 t->SetTextSize(fChangeTextSize);
			}
			if (fDoChangeTextAngle)
				t->SetTextAngle(fChangeTextAngle);
			if (fDoChangeTextAlign)
				t->SetTextAlign(fChangeTextAlign);
			if (fDoChangeTextFont)
				t->SetTextFont(10 * (fChangeTextFont/10) + fChangeTextPrec);
		}
		if (obj->InheritsFrom("TPaveText")) {
			TPaveText *t = (TPaveText*)obj;
			if (cut) {
				if (!cut->IsInside(t->GetX1(), t->GetY1())) continue;
			}
			if (fDoChangeTextColor)
				t->SetTextColor(fChangeTextColor);
			if (fDoChangeTextSize) {
				if (t->GetTextSize() > fChangeTextSizeMin &&
					 t->GetTextSize() < fChangeTextSizeMax)
					 t->SetTextSize(fChangeTextSize);
			}
			if (fDoChangeTextAngle)
				t->SetTextAngle(fChangeTextAngle);
			if (fDoChangeTextAlign)
				t->SetTextAlign(fChangeTextAlign);
			if (fDoChangeTextFont)
				t->SetTextFont(10 * (fChangeTextFont/10) + fChangeTextPrec);
		}
	}
	gPad->Modified();
	gPad->Update();
};
//_________________________________________________________________________

ChangeTextDialog::~ChangeTextDialog()
{
	if (fRow_lab ) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void ChangeTextDialog::CloseDialog()
{
//   cout << "FeynmanDiagramDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->   CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void ChangeTextDialog::CloseDown(Int_t wid)
{
	cout << "ChangeTextDialog::CloseDown()" << endl;
	if (wid != -2 )
		SaveDefaults();
	delete this;
}


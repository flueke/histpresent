#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFrame.h"
#include "TH1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "SetCanvasAttDialog.h"
#include "hprbase.h"
#include <iostream>
extern Int_t gHprDebug;

namespace std {} using namespace std;

//_______________________________________________________________________

SetCanvasAttDialog::SetCanvasAttDialog(Int_t /*batch*/)
{
	cout << "ctor SetCanvasAttDialog, non interactive" <<endl;
	fDialog = NULL;
}
//_______________________________________________________________________

SetCanvasAttDialog::SetCanvasAttDialog(TGWindow * win)
{
	static const Char_t helptext[] =" no help yet";
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	RestoreDefaults();
	fRow_lab = new TList();

	Int_t ind = 0;
	static TString stycmd("SetCanvasAttPerm()");
	static TString rescmd("SetAllToDefault()");
	fRow_lab->Add(new TObjString("CommentOnly_Pad Attributes"));
	fValp[ind++] = &fDummy;
//   fRow_lab->Add(new TObjString("ColorSelect_Color"));
//   fRow_lab->Add(new TObjString("PlainShtVal+BorderSize"));
//   fRow_lab->Add(new TObjString("PlainIntVal+BorderMode"));
	fRow_lab->Add(new TObjString("Float_Value_BottomMarg;0;1"));
	fRow_lab->Add(new TObjString("Float_Value+TopMarg;0;1"));
	fRow_lab->Add(new TObjString("Float_Value_LeftMarg;0;1"));
	fRow_lab->Add(new TObjString("Float_Value+RightMarg;0;1"));
	fRow_lab->Add(new TObjString("CheckButton_GridX"));
	fRow_lab->Add(new TObjString("CheckButton+GridY"));
	fRow_lab->Add(new TObjString("CheckButton_   Ticks at top X"));
	fRow_lab->Add(new TObjString("CheckButton+ Ticks at right Y"));
	fRow_lab->Add(new TObjString("CheckButton_  Labels at top X"));
	fRow_lab->Add(new TObjString("CheckButton+Labels at right Y"));
//   fValp[ind++] = &fPadColor;
//  fValp[ind++] = &fPadBorderSize;
//   fValp[ind++] = &fPadBorderMode;
	fValp[ind++] = &fPadBottomMargin;
	fValp[ind++] = &fPadTopMargin;
	fValp[ind++] = &fPadLeftMargin;
	fValp[ind++] = &fPadRightMargin;
	fValp[ind++] = &fPadGridX;
	fValp[ind++] = &fPadGridY;
	fValp[ind++] = &fPadTickX;
	fValp[ind++] = &fPadTickY;
	fValp[ind++] = &fPadLabelX;
	fValp[ind++] = &fPadLabelY;

	fRow_lab->Add(new TObjString("CommentOnly_Frame Attributes"));
	fValp[ind++] = &fDummy;
	fRow_lab->Add(new TObjString("ColorSelect_FillColor"));
	fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
	fRow_lab->Add(new TObjString("ColorSelect_LineColor"));
	fRow_lab->Add(new TObjString("LineSSelect+LineStyle"));
	fRow_lab->Add(new TObjString("PlainShtVal+LineWidth"));
	fRow_lab->Add(new TObjString("PlainShtVal_BorderSize"));
	fRow_lab->Add(new TObjString("PlainIntVal+BorderMode"));
	fValp[ind++] = &fFrameFillColor;
	fValp[ind++] = &fFrameFillStyle;
	fValp[ind++] = &fFrameLineColor;
	fValp[ind++] = &fFrameLineStyle;
	fValp[ind++] = &fFrameLineWidth;
	fValp[ind++] = &fFrameBorderSize;
	fValp[ind++] = &fFrameBorderMode;

	fRow_lab->Add(new TObjString("CommentOnly_Canvas Attributes"));
	fValp[ind++] = &fDummy;
	fRow_lab->Add(new TObjString("ColorSelect_Color"));
	fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
//   fRow_lab->Add(new TObjString("PlainShtVal+BorderSize"));
//   fRow_lab->Add(new TObjString("PlainIntVal+BorderMode"));
	fRow_lab->Add(new TObjString("PlainIntVal_DefHeight;20;1200"));
	fRow_lab->Add(new TObjString("PlainIntVal+DefWidth;20;1600"));
	fRow_lab->Add(new TObjString("PlainIntVal_DefXpos;0;1600"));
	fRow_lab->Add(new TObjString("PlainIntVal+DefYpos;0;1200"));
	fValp[ind++] = &fCanvasColor;
	fValp[ind++] = &fCanvasFillStyle;
//   fValp[ind++] = &fCanvasBorderSize;
//   fValp[ind++] = &fCanvasBorderMode;
	fValp[ind++] = &fCanvasDefH;
	fValp[ind++] = &fCanvasDefW;
	fValp[ind++] = &fCanvasDefX;
	fValp[ind++] = &fCanvasDefY;
	fRow_lab->Add(new TObjString("CommandButt_Reset all to def"));
	fValp[ind++] = &rescmd;
	fRow_lab->Add(new TObjString("CommandButt+Set as global default"));
	fValp[ind++] = &stycmd;
	fOk = 0;
	Int_t itemwidth =  60 * TGMrbValuesAndText::LabelLetterWidth();
	fDialog = new TGMrbValuesAndText(fCanvas->GetName(), NULL, &fOk,itemwidth, win,
				NULL, NULL, fRow_lab, fValp,
				NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

SetCanvasAttDialog::~SetCanvasAttDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________

void SetCanvasAttDialog::RecursiveRemove(TObject * obj)
{
	if (obj == fCanvas) {
 //     cout << "SetCanvasAttDialog: CloseDialog "  << endl;
		CloseDialog();
	}
}
//_______________________________________________________________________

void SetCanvasAttDialog::CloseDialog()
{
//   cout << "SetCanvasAttDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	if(fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
	delete this;
}
//_______________________________________________________________________

void SetCanvasAttDialog::SetCanvasAtt(TCanvas *canvas)
{
	if (!canvas) return;
	TIter next(canvas->GetListOfPrimitives());
	TObject *obj;
 	canvas->cd();
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TPad"))
			SetPadAtt((TPad*)obj);
	}
	SetPadAtt((TPad*)canvas);
	canvas->Pop();
	canvas->cd();
	canvas->Modified();
	canvas->Update();
}
//_______________________________________________________________________

void SetCanvasAttDialog::SetPadAtt(TPad *pad)
{
	if (!pad) return;
	pad->SetBottomMargin(fPadBottomMargin);
	pad->SetTopMargin   (fPadTopMargin   );
	pad->SetLeftMargin  (fPadLeftMargin  );
	pad->SetRightMargin (fPadRightMargin );
	pad->SetGridx       ((Bool_t)fPadGridX);
	pad->SetGridy       ((Bool_t)fPadGridY);
	if ( fPadLabelX )
		fPadTickX = 1;
	if ( fPadLabelY )
		fPadTickY = 1;
	pad->SetTickx       (fPadTickX + fPadLabelX);
	pad->SetTicky       (fPadTickY + fPadLabelY);
	pad->GetFrame()->SetFillColor (fFrameFillColor  );
	pad->GetFrame()->SetLineColor (fFrameLineColor  );
	if (fFrameFillColor == 0)
		fFrameFillStyle = 0;
	pad->GetFrame()->SetFillStyle (fFrameFillStyle  );
	pad->GetFrame()->SetLineStyle (fFrameLineStyle  );
	pad->GetFrame()->SetLineWidth (fFrameLineWidth  );
	pad->GetFrame()->SetBorderSize(fFrameBorderSize );
	pad->GetFrame()->SetBorderMode(fFrameBorderMode );
	pad->SetFillColor  	  (fCanvasColor     );
	pad->SetFillStyle  	  (fCanvasFillStyle );
	pad->SetBorderSize  (fCanvasBorderSize);
	pad->SetBorderMode  (fCanvasBorderMode);
	pad->Modified();
}
//______________________________________________________________________

void SetCanvasAttDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void SetCanvasAttDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("SetCanvasAttDialog.fPadBottomMargin", fPadBottomMargin);
	env.SetValue("SetCanvasAttDialog.fPadTopMargin", fPadTopMargin);
	env.SetValue("SetCanvasAttDialog.fPadLeftMargin", fPadLeftMargin);
	env.SetValue("SetCanvasAttDialog.fPadRightMargin", fPadRightMargin);
	env.SetValue("SetCanvasAttDialog.fPadGridX", fPadGridX);
	env.SetValue("SetCanvasAttDialog.fPadGridY", fPadGridY);
	env.SetValue("SetCanvasAttDialog.fPadTickX", fPadTickX);
	env.SetValue("SetCanvasAttDialog.fPadTickY", fPadTickY);
	env.SetValue("SetCanvasAttDialog.fFrameFillColor", fFrameFillColor);
	env.SetValue("SetCanvasAttDialog.fFrameLineColor", fFrameLineColor);
	env.SetValue("SetCanvasAttDialog.fFrameFillStyle", fFrameFillStyle);
	env.SetValue("SetCanvasAttDialog.fFrameLineStyle", fFrameLineStyle);
	env.SetValue("SetCanvasAttDialog.fFrameLineWidth", fFrameLineWidth);
	env.SetValue("SetCanvasAttDialog.fFrameBorderSize", fFrameBorderSize);
	env.SetValue("SetCanvasAttDialog.fFrameBorderMode", fFrameBorderMode);
	env.SetValue("SetCanvasAttDialog.fCanvasColor", fCanvasColor);
	env.SetValue("SetCanvasAttDialog.fCanvasFillStyle", fCanvasFillStyle);
	env.SetValue("SetCanvasAttDialog.fCanvasDefH", fCanvasDefH);
	env.SetValue("SetCanvasAttDialog.fCanvasDefW", fCanvasDefW);
	env.SetValue("SetCanvasAttDialog.fCanvasDefX", fCanvasDefX);
	env.SetValue("SetCanvasAttDialog.fCanvasDefY", fCanvasDefY);
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void SetCanvasAttDialog::RestoreDefaults(Int_t resetall)
{
	if (gHprDebug > 0)
	cout << "SetCanvasAttDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
	TEnv env(envname);
	fPadBottomMargin  = env.GetValue("SetCanvasAttDialog.fPadBottomMargin", 0.1);
	fPadTopMargin     = env.GetValue("SetCanvasAttDialog.fPadTopMargin",    0.1);
	fPadLeftMargin    = env.GetValue("SetCanvasAttDialog.fPadLeftMargin",   0.1);
	fPadRightMargin   = env.GetValue("SetCanvasAttDialog.fPadRightMargin",  0.1);
	fPadGridX         = env.GetValue("SetCanvasAttDialog.fPadGridX",          0);
	fPadGridY         = env.GetValue("SetCanvasAttDialog.fPadGridY",          0);
	fPadTickX         = env.GetValue("SetCanvasAttDialog.fPadTickX",          0);
	fPadTickY         = env.GetValue("SetCanvasAttDialog.fPadTickY",          0);
	fPadLabelX        = env.GetValue("SetCanvasAttDialog.fPadLabelX",         0);
	fPadLabelY        = env.GetValue("SetCanvasAttDialog.fPadLabelY",         0);
	fFrameFillColor   = env.GetValue("SetCanvasAttDialog.fFrameFillColor",    0);
	fFrameLineColor   = env.GetValue("SetCanvasAttDialog.fFrameLineColor",    1);
	fFrameFillStyle   = env.GetValue("SetCanvasAttDialog.fFrameFillStyle",    0);
	fFrameLineStyle   = env.GetValue("SetCanvasAttDialog.fFrameLineStyle",    1);
	fFrameLineWidth   = env.GetValue("SetCanvasAttDialog.fFrameLineWidth",    1);
	fFrameBorderSize  = env.GetValue("SetCanvasAttDialog.fFrameBorderSize",   1);
	fFrameBorderMode  = env.GetValue("SetCanvasAttDialog.fFrameBorderMode",   0);
	fCanvasColor      = env.GetValue("SetCanvasAttDialog.fCanvasColor",       0);
	fCanvasFillStyle  = env.GetValue("SetCanvasAttDialog.fCanvasFillStyle",   0);
	fCanvasBorderSize = env.GetValue("SetCanvasAttDialog.fCanvasBorderSize",  1);
	fCanvasBorderMode = env.GetValue("SetCanvasAttDialog.fCanvasBorderMode",  0);
	fCanvasDefH       = env.GetValue("SetCanvasAttDialog.fCanvasDefH",      500);
	fCanvasDefW       = env.GetValue("SetCanvasAttDialog.fCanvasDefW",      700);
	fCanvasDefX       = env.GetValue("SetCanvasAttDialog.fCanvasDefX",      400);
	fCanvasDefY       = env.GetValue("SetCanvasAttDialog.fCanvasDefY",       10);
	if ( fCanvasDefH == 0 ) fCanvasDefH = 500;
	if ( fCanvasDefW == 0 ) fCanvasDefW = 700;
	if ( fCanvasDefX == 0 ) fCanvasDefX = 400;
	if ( fCanvasDefY == 0 ) fCanvasDefY = 10;

}
//______________________________________________________________________

void SetCanvasAttDialog::SetDefaults()
{
	TEnv env(".hprrc");
	gStyle->SetPadBottomMargin (env.GetValue("SetCanvasAttDialog.fPadBottomMargin", 0.1));
	gStyle->SetPadTopMargin    (env.GetValue("SetCanvasAttDialog.fPadTopMargin",    0.1));
	gStyle->SetPadLeftMargin   (env.GetValue("SetCanvasAttDialog.fPadLeftMargin",   0.1));
	gStyle->SetPadRightMargin  (env.GetValue("SetCanvasAttDialog.fPadRightMargin",  0.1));
	gStyle->SetPadGridX        (env.GetValue("SetCanvasAttDialog.fPadGridX",          0));
	gStyle->SetPadGridY        (env.GetValue("SetCanvasAttDialog.fPadGridY",          0));

	gStyle->SetPadTickX        (env.GetValue("SetCanvasAttDialog.fPadTickX",0) +
										 env.GetValue("SetCanvasAttDialog.fPadLabelX", 0));
	gStyle->SetPadTickY        (env.GetValue("SetCanvasAttDialog.fPadTickY", 0) +
										 env.GetValue("SetCanvasAttDialog.fPadLabelY", 0));
	gStyle->SetFrameFillColor  (env.GetValue("SetCanvasAttDialog.fFrameFillColor", 1));
	gStyle->SetFrameLineColor  (env.GetValue("SetCanvasAttDialog.fFrameLineColor", 1));
	gStyle->SetFrameFillStyle  (env.GetValue("SetCanvasAttDialog.fFrameFillStyle", 0));
	gStyle->SetFrameLineStyle  (env.GetValue("SetCanvasAttDialog.fFrameLineStyle", 1));
	gStyle->SetFrameLineWidth  (env.GetValue("SetCanvasAttDialog.fFrameLineWidth", 1));
	gStyle->SetFrameBorderSize (env.GetValue("SetCanvasAttDialog.fFrameBorderSize",1));
	gStyle->SetFrameBorderMode (env.GetValue("SetCanvasAttDialog.fFrameBorderMode",1));
	gStyle->SetCanvasColor     (env.GetValue("SetCanvasAttDialog.fCanvasColor",    0));
	gStyle->SetCanvasDefH      (env.GetValue("SetCanvasAttDialog.fCanvasDefH",   500));
	gStyle->SetCanvasDefW      (env.GetValue("SetCanvasAttDialog.fCanvasDefW",   700));
	gStyle->SetCanvasDefX      (env.GetValue("SetCanvasAttDialog.fCanvasDefX",   400));
	gStyle->SetCanvasDefY      (env.GetValue("SetCanvasAttDialog.fCanvasDefY",    10));
	if ( gStyle->GetCanvasDefH()  == 0 )  gStyle->SetCanvasDefH(500);
	if ( gStyle->GetCanvasDefW()  == 0 )  gStyle->SetCanvasDefW(700);
	if ( gStyle->GetCanvasDefX()  == 0 )  gStyle->SetCanvasDefX(400);
	if ( gStyle->GetCanvasDefY()  == 0 )  gStyle->SetCanvasDefY(10);

}
//______________________________________________________________________

void SetCanvasAttDialog::SetCanvasAttPerm()
{
	SaveDefaults();
//   gStyle->SetPadColor        (fPadColor        );
//   gStyle->SetPadBorderSize   (fPadBorderSize   );
//   gStyle->SetPadBorderMode   (fPadBorderMode   );
	gStyle->SetPadBottomMargin (fPadBottomMargin );
	gStyle->SetPadTopMargin    (fPadTopMargin    );
	gStyle->SetPadLeftMargin   (fPadLeftMargin   );
	gStyle->SetPadRightMargin  (fPadRightMargin  );
	gStyle->SetPadGridX        ((Bool_t)fPadGridX);
	gStyle->SetPadGridY        ((Bool_t)fPadGridY);
	if ( fPadLabelX )
		fPadTickX = 1;
	if ( fPadLabelY )
		fPadTickY = 1;
	gStyle->SetPadTickX        (fPadTickX + fPadLabelX);
	gStyle->SetPadTickY        (fPadTickY + fPadLabelY);
	gStyle->SetFrameFillColor  (fFrameFillColor  );
	gStyle->SetFrameLineColor  (fFrameLineColor  );
	gStyle->SetFrameFillStyle  (fFrameFillStyle  );
	gStyle->SetFrameLineStyle  (fFrameLineStyle  );
	gStyle->SetFrameLineWidth  (fFrameLineWidth  );
	gStyle->SetFrameBorderSize (fFrameBorderSize );
	gStyle->SetFrameBorderMode (fFrameBorderMode );
	gStyle->SetCanvasColor     (fCanvasColor     );
	gStyle->SetCanvasBorderSize(fCanvasBorderSize);
	gStyle->SetCanvasBorderMode(fCanvasBorderMode);
	gStyle->SetCanvasDefH      (fCanvasDefH      );
	gStyle->SetCanvasDefW      (fCanvasDefW      );
	gStyle->SetCanvasDefX      (fCanvasDefX      );
	gStyle->SetCanvasDefY      (fCanvasDefY      );
}
//______________________________________________________________________

void SetCanvasAttDialog::CloseDown(Int_t wid)
{
	if (gHprDebug > 0)
	cout << "SetCanvasAttDialog::CloseDown(" << wid<< ")" <<endl;
	fDialog = NULL;
	if (wid == -1)
		SaveDefaults();
}
//______________________________________________________________________

void SetCanvasAttDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject *obj)
{
	TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
 //  if (obj) cout  << ", " << canvas->GetName() << ")";
 //  cout << endl;
	SetCanvasAtt(canvas);
}


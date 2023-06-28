#include "TROOT.h"
#include "TEnv.h"
#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TString.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "TGMrbValuesAndText.h"
#include "ShiftScale.h"
#include "SetCanvasAttDialog.h"
#include "SetHistOptDialog.h"
#include "FitHist.h"
#include "WindowSizeDialog.h"
#include <fstream>
//______________________________________________________________________________

ClassImp(ShiftScale)

enum Ecmds {M_FitFormula, M_Save2File, M_Graph2File, M_OptionHist,
				M_OptionPad};

ShiftScale::ShiftScale(TH1 * hist, HTCanvas *canvas)
{
  
static const Char_t helpText[] = 
"Create a new histogram applying a linear transformation\n\
to the bincenters. To avoid (visible) binning effects values \n\
are randomly shifted within a bin or cell (2dim)\n\
If you are sure the shift is a multiple of bins the randomizing\n\
may be suppressed optionally.\n\
As default the new histogram has the parameters of the original\n\
(nbins, min max etc.)\n\
Note: Under and Overflow bins of the original histogram are not\n\
taken into account\n\
";
   fCanvasOrig = canvas;
   fHistOrig   = hist;
	if ( !fHistOrig || fHistOrig->GetDimension() > 2 ) {
		cout << "Histogram must be 1 or 2dim" << endl;
		return;
	}
//   cout << endl << "fCanvasOrig " << fCanvasOrig<< endl;
//   cout << "fHistOrig   " <<fHistOrig << endl;
	fCanvasOrig->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");

   Int_t ind = 0;
   fCommand = "Draw_The_Hist()";
   fCanvas = NULL;
	TRootCanvas *win = (TRootCanvas*)fCanvasOrig->GetCanvasImp();
//   fSave2FileDialog = NULL;
//   fDialog = NULL;
	fHistName   = fHistOrig->GetName();
	fHistName += "_ShSc";
	fHistTitle  = fHistOrig->GetTitle();
	fHistTitle += "_ShSc";
	fHistXtitle = fHistOrig->GetXaxis()->GetTitle();
	fHistYtitle = fHistOrig->GetYaxis()->GetTitle();
	fNbinsX     = fHistOrig->GetNbinsX();
	if (fHistOrig->GetDimension() == 2 )
		fNbinsY = fHistOrig->GetNbinsY();
	fXaxisMin  = fHistOrig->GetXaxis()->GetXmin();
	fXaxisMax  = fHistOrig->GetXaxis()->GetXmax();
	fYaxisMin  = fHistOrig->GetYaxis()->GetXmin();
	fYaxisMax  = fHistOrig->GetYaxis()->GetXmin();
	
   RestoreDefaults();
	
   fRow_lab = new TList(); 

   fRow_lab->Add(new TObjString("StringValue_ Name"));
	fRow_lab->Add(new TObjString("StringValue+Title"));
	fRow_lab->Add(new TObjString("StringValue_Tit X"));
   fRow_lab->Add(new TObjString("StringValue+Tit Y"));
   fRow_lab->Add(new TObjString("PlainIntVal_NbinsX"));
   fRow_lab->Add(new TObjString("DoubleValue+Xmin"));
   fRow_lab->Add(new TObjString("DoubleValue+Xmax"));
	if ( fHistOrig->GetDimension() == 2) {
		fRow_lab->Add(new TObjString("PlainIntVal_NbinsY"));
		fRow_lab->Add(new TObjString("DoubleValue+Ymin"));
		fRow_lab->Add(new TObjString("DoubleValue+Ymax"));
	}
	
	fRow_lab->Add(new TObjString("DoubleValue_Shift X"));
	fRow_lab->Add(new TObjString("DoubleValue+Scale X"));
	fRow_lab->Add(new TObjString("CheckButton+Randomize"));
	if ( fHistOrig->GetDimension() == 2) {
		fRow_lab->Add(new TObjString("DoubleValue_Shift Y"));
		fRow_lab->Add(new TObjString("DoubleValue+Scale Y"));
		fRow_lab->Add(new TObjString("CheckButton+Randomize"));
	}
   fRow_lab->Add(new TObjString("CommandButt_Draw_the_Hist"));
//   fRow_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = fRow_lab->GetSize();

   fValp[ind++] = &fHistName;
	fValp[ind++] = &fHistTitle;
	fValp[ind++] = &fHistXtitle;
   fValp[ind++] = &fHistYtitle;
   fValp[ind++] = &fNbinsX;
   fValp[ind++] = &fXaxisMin;
   fValp[ind++] = &fXaxisMax;
	if ( fHistOrig->GetDimension() == 2) {
		fValp[ind++] = &fNbinsY;
		fValp[ind++] = &fYaxisMin;
		fValp[ind++] = &fYaxisMax;
	}
	fValp[ind++] = &fShiftX;
	fValp[ind++] = &fScaleX;
	fValp[ind++] = &fRandomX;
	if ( fHistOrig->GetDimension() == 2) {
		fValp[ind++] = &fShiftY;
		fValp[ind++] = &fScaleY;
		fValp[ind++] = &fRandomY;
	}
   fValp[ind++] = &fCommand;
   Int_t itemwidth = 400;
   fOk = 0;
   fDialog =
      new TGMrbValuesAndText("Hists parameters", NULL, &fOk,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helpText, this, this->ClassName());
};  
//_________________________________________________________________________
            
ShiftScale::~ShiftScale() 
{
	if (fCanvasOrig)
		fCanvasOrig->Disconnect("HTCanvasClosed()", this, "CloseDialog()");
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab) {
	   fRow_lab->Delete();
      delete fRow_lab;
      fRow_lab = NULL;
	}
};
//_______________________________________________________________________

void ShiftScale::CloseDialog()
{
	if (gDebug>0)
		cout << "ShiftScale::CloseDialog() " << fDialog << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab) {
	   fRow_lab->Delete();
      delete fRow_lab;
      fRow_lab = NULL;
	}
	delete this;
}
//_________________________________________________________________________
            
void ShiftScale::Draw_The_Hist()
{
	TString cn(fCanvasOrig->GetName());
	cn+= "_shsc";
	TString ct(fCanvasOrig->GetTitle());
	ct+= "_shsc";
	Int_t cw = fCanvasOrig->GetWindowWidth();
	Int_t ch = fCanvasOrig->GetWindowHeight();
	Int_t cx = fCanvasOrig->GetWindowTopX() + WindowSizeDialog::fWinshiftx;
	Int_t cy = fCanvasOrig->GetWindowTopY() + WindowSizeDialog::fWinshifty;
	
//	fCanvas = new HTCanvas(cn, ct, cx, cy, cw, cy);
	TString clname(fHistOrig->ClassName());
	if        (clname == "TH1C") {
		fHist = new TH1C(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH1I") {
		fHist = new TH1I(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH1F") {
		fHist = new TH1F(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH1D") {
		fHist = new TH1D(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH2C") {
		fHist = new TH2C(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax,
								fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH2I") {
		fHist = new TH2I(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax,
								fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH2F") {
		fHist = new TH2F(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax,
								fNbinsX, fXaxisMin, fXaxisMax);
	} else if (clname == "TH2D") {
		fHist = new TH2D(fHistName, fHistTitle, fNbinsX, fXaxisMin, fXaxisMax,
								fNbinsX, fXaxisMin, fXaxisMax);
	}	
	if (fHistXtitle.Length() > 0)
		fHist->GetXaxis()->SetTitle(fHistXtitle.Data());
	if (fHistYtitle.Length() > 0)
		fHist->GetYaxis()->SetTitle(fHistYtitle.Data());
	Double_t wx = fHistOrig->GetBinWidth(1);
	if (!fRandomX && TMath::Abs((Int_t(fShiftX / wx) * wx) - fShiftX ) > 0.000001) {
		cout << endl<< "Warning: Shift X is not a multiple of BinWidth" << endl
				<< "Consider to activate Randomize" << endl;
	}
	if (fHist->GetDimension() == 1) {
		for (Int_t i = 1; i <= fHistOrig->GetNbinsX(); i++) {
			Int_t c = (Int_t)fHistOrig->GetBinContent(i);
			Double_t x = fHistOrig->GetBinLowEdge(i);
			wx = fHistOrig->GetBinWidth(i);
			for (Int_t k = 0; k < c; k++) {
				Double_t xr = x;
				if ( fRandomX )
					xr += gRandom->Rndm() * wx;
				else 
					xr += (0.5 * wx);
				xr = fShiftX + fScaleX * xr;
				fHist->Fill(xr);
			}
		}
	} else {
		Double_t wy = fHistOrig->GetYaxis()->GetBinWidth(1);
		if (!fRandomY && TMath::Abs((Int_t(fShiftY / wy) * wy) - fShiftY ) > 0.000001) {
			cout << endl<< "Warning: Shift Y is not a multiple of BinWidth" << endl
					<< "Consider to activate Randomize" << endl;
		}
		for (Int_t i = 1; i <= fHistOrig->GetNbinsX(); i++) {
			for (Int_t j = 1; j <= fHistOrig->GetNbinsY(); j++) {
				Int_t c = (Int_t)fHistOrig->GetBinContent(i, j);
				Double_t x = fHistOrig->GetXaxis()->GetBinLowEdge(i);
				wx = fHistOrig->GetXaxis()->GetBinWidth(i);
				Double_t y = fHistOrig->GetYaxis()->GetBinLowEdge(j);
				wy = fHistOrig->GetYaxis()->GetBinWidth(j);
				for (Int_t k = 0; k < c; k++) {
					Double_t xr = x;
					if ( fRandomX )
						xr += gRandom->Rndm() * wx;
					else 
						xr += 0.5 * wx;
					xr = fShiftX + fScaleX * xr;
					Double_t yr = y;
					if ( fRandomY )
						yr += gRandom->Rndm() * wy;
					else 
						yr += 0.5 * wy;
					yr = fShiftY + fScaleY * yr;
					
					fHist->Fill(xr, yr);
				}
			}
		}
	}
	new FitHist(cn, ct, fHist, cn, cx,cy, cw, ch);
};
//_________________________________________________________________________
            
void ShiftScale::SaveDefaults()
{
   cout << "ShiftScale::SaveDefaults() " << endl;
   TEnv env(".hprrc");
   env.SetValue("ShiftScale.fShiftX"		 , fShiftX);
   env.SetValue("ShiftScale.fShiftY"		 , fShiftY);
   env.SetValue("ShiftScale.fScaleX"		 , fScaleX);
   env.SetValue("ShiftScale.fScaleY"		 , fScaleY);
   env.SetValue("ShiftScale.fRandomX"		 , fRandomX);
   env.SetValue("ShiftScale.fRandomY"		 , fRandomY);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void ShiftScale::RestoreDefaults()
{
	TEnv env(".hprrc");
   fShiftX = env.GetValue("ShiftScale.fShiftX"		 , 0.);
   fShiftY = env.GetValue("ShiftScale.fShiftY"		 , 0.);
   fScaleX = env.GetValue("ShiftScale.fScaleX"		 , 1.);
   fScaleY = env.GetValue("ShiftScale.fScaleY"		 , 1.);
   fRandomX = env.GetValue("ShiftScale.fRandomX"	 , 1);
   fRandomX = env.GetValue("ShiftScale.fRandomY"	 , 1);
}
//_________________________________________________________________________

void ShiftScale::CloseDown(Int_t wid)
{
	cout << "ShiftScale::CloseDown(" << wid << ")" << endl;
   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}


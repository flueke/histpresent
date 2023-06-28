#include "TROOT.h"
#include "TEnv.h"
#include "TH1D.h"
#include "TPad.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TString.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "hprbase.h"
#include "TGMrbValuesAndText.h"
#include "EmptyHistDialog.h"
#include "SetCanvasAttDialog.h"
#include "SetHistOptDialog.h"
#include <fstream>
//______________________________________________________________________________

ClassImp(EmptyHistDialog)

enum Ecmds {M_FitFormula, M_Save2File, M_Graph2File, M_OptionHist,
				M_OptionPad};

EmptyHistDialog::EmptyHistDialog(TGWindow * win, Int_t winx,  Int_t winy)
              : fWinx(winx), fWiny(winy) 
{
  
static const Char_t helpText[] = 
"This widget helps to create an empty histogram\n\
which can be used to hold a function or randomly\n\
filled according to a function. The canvas can\n\
divided into pads to allow for more than one histogram\n\
";

   static void *valp[50];
   Int_t ind = 0;
 //  Bool_t ok = kTRUE;
   fCommand = "Draw_The_Hist()";
   fCanvas = NULL;
//   fSave2FileDialog = NULL;
//   fDialog = NULL;
   gROOT->GetListOfCleanups()->Add(this);
   RestoreDefaults();
   TList *row_lab = new TList(); 

   row_lab->Add(new TObjString("StringValue_Name"));
	row_lab->Add(new TObjString("StringValue_Title"));
	row_lab->Add(new TObjString("StringValue_Title X  "));
   row_lab->Add(new TObjString("StringValue+Title Y  "));
   row_lab->Add(new TObjString("PlainIntVal_Nbins"));
   row_lab->Add(new TObjString("DoubleValue+Xmin"));
   row_lab->Add(new TObjString("DoubleValue+Xmax"));
   row_lab->Add(new TObjString("DoubleValue_Ymin"));
   row_lab->Add(new TObjString("DoubleValue-Ymax"));

   row_lab->Add(new TObjString("CheckButton_Use for Graph"));
	row_lab->Add(new TObjString("CheckButton+Draw/Overl in a sel pad"));
	//   row_lab->Add(new TObjString("CheckButton_Draw in a new canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Xsize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal-Ysize of canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div X of canvas"));
   row_lab->Add(new TObjString("PlainIntVal-Div Y of canvas"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Hist"));
//   row_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = row_lab->GetSize();

   valp[ind++] = &fHistName;
	valp[ind++] = &fHistTitle;
	valp[ind++] = &fHistXtitle;
   valp[ind++] = &fHistYtitle;
   valp[ind++] = &fNbins;
   valp[ind++] = &fXaxisMin;
   valp[ind++] = &fXaxisMax;
   valp[ind++] = &fYaxisMin;
   valp[ind++] = &fYaxisMax;
   valp[ind++] = &fUseForGraph;
	valp[ind++] = &fHistSelPad;
	//   valp[ind++] = &fHistNewPad;
   valp[ind++] = &fHistXsize;
   valp[ind++] = &fHistYsize;
   valp[ind++] = &fHistXdiv;
   valp[ind++] = &fHistYdiv;
   valp[ind++] = &fCommand;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   GetStringExt("Hists parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};  
//_________________________________________________________________________
            
EmptyHistDialog::~EmptyHistDialog() 
{
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fCanvas ) SaveDefaults();
};
//__________________________________________________________________________

void EmptyHistDialog::RecursiveRemove(TObject * obj)
{
//   cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
//        << obj << " fSelHist " <<  fSelHist <<  endl;
   if (obj == fCanvas) { 
 //      cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
 //       << obj << " fSelHist " <<  fSelHist <<  endl;
     fCanvas = NULL;
   }
}
//_________________________________________________________________________
            
void EmptyHistDialog::Draw_The_Hist()
{
	cout << "Empty pad only" << endl;
	if ( fUseForGraph ){
		fHistName = "hist_for_graph";
	}
//	TGraph *graph = new TGraph();
   if (fHistSelPad |= 0) {
      if (gPad == NULL) {
         cout << "Please select a pad first" << endl;
         return;
      }
   } else {
	   fCanvas = new HTCanvas("Empty", "Empty", fWinx, fWiny,
						fHistXsize, fHistYsize);
   }
//   BuildMenu();
	if (fHistXdiv > 1 || fHistYdiv > 1) {
		fCanvas->Divide(fHistXdiv, fHistYdiv);
		fCanvas->cd(1);
	}
	Double_t xmin = 0, xmax = 100;
//	Double_t ymin = 0;
	Double_t ymax = 100;
	if (fXaxisMin != 0 || fXaxisMax != 0) {
		xmin = fXaxisMin;
		xmax = fXaxisMax;
	}
	if (fYaxisMin != 0 || fYaxisMax != 0) {
//		ymin = fYaxisMin;
		ymax = fYaxisMax;
	}
//	gStyle->SetOptStat(0);
	fHist = new TH1D(fHistName, fHistTitle, fNbins, xmin, xmax);
	fCanvas->GetHandleMenus()->SetHist(fHist);
	fHist->Draw();
//	fHist->SetMinimum(ymin);
	fHist->SetMaximum(ymax);
	if (fHistXtitle.Length() > 0)
		fHist->GetXaxis()->SetTitle(fHistXtitle.Data());
	if (fHistYtitle.Length() > 0)
		fHist->GetYaxis()->SetTitle(fHistYtitle.Data());
//	graph->SetHistogram(gh);
	gPad->Update();
};
//_________________________________________________________________________
            
void EmptyHistDialog::SaveDefaults()
{
   cout << "EmptyHistDialog::SaveDefaults() " << endl;
   TEnv env(".hprrc");
   env.SetValue("EmptyHistDialog.HistName"		 , fHistName       );
	env.SetValue("EmptyHistDialog.HistTitle"		 , fHistName       );
	env.SetValue("EmptyHistDialog.fUseForGraph"	 , fUseForGraph    );
	env.SetValue("EmptyHistDialog.HistSelPad" 	 , fHistSelPad     );
   env.SetValue("EmptyHistDialog.HistNewPad" 	 , fHistNewPad     );
   env.SetValue("EmptyHistDialog.HistXsize"  	 , fHistXsize      );
   env.SetValue("EmptyHistDialog.HistYsize"  	 , fHistYsize      );
   env.SetValue("EmptyHistDialog.HistXtitle" 	 , fHistXtitle     );
   env.SetValue("EmptyHistDialog.HistYtitle" 	 , fHistYtitle     );
   env.SetValue("EmptyHistDialog.Nbins"  		    , fNbins           );
   env.SetValue("EmptyHistDialog.XaxisMin"  		 , fXaxisMin        );
   env.SetValue("EmptyHistDialog.YaxisMin"  		 , fYaxisMin        );
   env.SetValue("EmptyHistDialog.XaxisMax"  		 , fXaxisMax        );
   env.SetValue("EmptyHistDialog.YaxisMax"  		 , fYaxisMax        );
   env.SetValue("EmptyHistDialog.HistXdiv"		 , fHistXdiv       );
   env.SetValue("EmptyHistDialog.HistYdiv"		 , fHistYdiv       );
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void EmptyHistDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fHistName        = env.GetValue("EmptyHistDialog.HistName"  		, "empty");
	fHistName        = env.GetValue("EmptyHistDialog.HistTitle"  		, "empty");
	fUseForGraph     = env.GetValue("EmptyHistDialog.fUseForGraph"	, 0);
	fHistSelPad      = env.GetValue("EmptyHistDialog.HistSelPad"		, 0);
	fHistNewPad      = env.GetValue("EmptyHistDialog.HistNewPad"		, 1);
	fHistXsize       = env.GetValue("EmptyHistDialog.HistXsize" 		, 800);
	fHistYsize       = env.GetValue("EmptyHistDialog.HistYsize" 		, 600);
	fHistXtitle      = env.GetValue("EmptyHistDialog.HistXtitle"	   , "Xvalues");
	fHistYtitle      = env.GetValue("EmptyHistDialog.HistYtitle"		, "Yvalues");
	fHistXdiv        = env.GetValue("EmptyHistDialog.HistXdiv"  		, 1);
	fNbins           = env.GetValue("EmptyHistDialog.Nbins"  		   , 100);
	fXaxisMin        = env.GetValue("EmptyHistDialog.XaxisMin"  		, 0);
	fYaxisMin        = env.GetValue("EmptyHistDialog.YaxisMin"  		, 0);
	fXaxisMax        = env.GetValue("EmptyHistDialog.XaxisMax"  		, 100);
	fYaxisMax        = env.GetValue("EmptyHistDialog.YaxisMax"  		, 10);
	fHistYdiv        = env.GetValue("EmptyHistDialog.HistYdiv"  		, 1);
}
//_________________________________________________________________________

void EmptyHistDialog::CloseDown(Int_t /*wid*/)
{
	//   cout << "EmptyHistDialog::CloseDown() " << endl;
	//   if (fCanvas) delete fCanvas;
	delete this;
}


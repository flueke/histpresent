#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "WhatToShowDialog.h"
#include "TMrbHelpBrowser.h"
#include "hprbase.h"
#include "support.h"
#include <iostream>

extern Int_t gHprDebug;

namespace std {} using namespace std;

//TString WhatToShowDialog::fDrawOpt2Dim = "COLZ";
//_______________________________________________________________________
//WhatToShowDialog::WhatToShowDialog(){};
//_______________________________________________________________________

WhatToShowDialog::WhatToShowDialog(TGWindow * win)
{
fHelptextBr=
"<!DOCTYPE HTML>\n\
<HTML>\n\
<TITLE>HelpWhatToShow</TITLE>\n\
This determines what to show for a histogram,\n\
especially which values to display in the statistics box\n\
2dim case: <b>Suppr Zero</b>: dont draw channels with 0 content\n\
</HTML>\n\
";

	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
// is it 1 or 2dim
	fHist = NULL;
   TIter next(fCanvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
		if (obj->InheritsFrom("TH1")) {
			fHist = (TH1*)obj;
		}
   }
   if ( fHist == NULL ) {
      cout << "WARNING: No hist found in pad" << endl;
		return;
	}
	if (gHprDebug > 0) {
		cout << "ctor WhatToShowDialog: " << this << " fHist: " << fHist << endl;
	}
	fRow_lab = new TList();
   
   Int_t ind = 0;
   static Int_t dummy;
   static TString stycmd("SaveDefaults()");
//   static TString helpcmd("HelpBr()");

   RestoreDefaults();
   fRow_lab->Add(new TObjString("CheckButton_  Title"));
   fValp[ind++] = &fShowTitle ;
   fRow_lab->Add(new TObjString("CheckButton+StatBox"));
   fValp[ind++] = &fShowStatBox ;
   
	if ( fHist->GetDimension() == 2) {
		fRow_lab->Add(new TObjString("CheckButton_Suppr Zero"));
		fValp[ind++] = &fLegoSuppressZero;
	}
   fRow_lab->Add(new TObjString("CheckButton+  Date"));
   fRow_lab->Add(new TObjString("CheckButton+CurrTime\
&Choose between current time\n\
and creation time"));
   fRow_lab->Add(new TObjString("CommentOnly_Content in Statistics Box"));
   fRow_lab->Add(new TObjString("CheckButton_HistName"));
   fRow_lab->Add(new TObjString("CheckButton+ Entries"));
   fRow_lab->Add(new TObjString("CheckButton+    Mean"));
   fRow_lab->Add(new TObjString("CheckButton+ Std Dev"));
   fRow_lab->Add(new TObjString("CheckButton_ Undflow"));
   fRow_lab->Add(new TObjString("CheckButton+ Ovrflow"));
   fRow_lab->Add(new TObjString("CheckButton+Integral"));
   fRow_lab->Add(new TObjString("CheckButton+Skewness"));
   fRow_lab->Add(new TObjString("CheckButton_Kurtosis"));
   fRow_lab->Add(new TObjString("CheckButton+Fit Pars"));
   fRow_lab->Add(new TObjString("CommentOnly_ "));
   fRow_lab->Add(new TObjString("CommentOnly_ "));
	fValp[ind++] = &fShowDateBox ;
	fValp[ind++] = &fUseTimeOfDisplay ;
   fValp[ind++] = &dummy;

   Int_t mask = 1;;
   for (Int_t i = 0; i < kNopts; i++) {
      if (((fOptStat / mask) % 10) != 0) fOpts[i] = 1; 
      else                               fOpts[i] = 0; 
      fValp[ind++] = &fOpts[i];
      mask *= 10;
   }
   fValp[ind++] = &fShowFitBox;
   fValp[ind++] = &dummy;
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;
//   fRow_lab->Add(new TObjString("CommandButt+Help"));
//   fValp[ind++] = &helpcmd;
     
 //  fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
//   fValp[ind++] = &stycmd;

   static Int_t ok; 
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog = 
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, fHelptextBr.Data(), this, this->ClassName());
}
//_______________________________________________________________________

WhatToShowDialog::~WhatToShowDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________

void WhatToShowDialog::CloseDialog()
{
//   cout << "WhatToShowDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fDialog) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void WhatToShowDialog::SetWhatToShowNow(TCanvas *canvas)
{
   if (!canvas) return;
   Int_t mask = 1;
//   Int_t save_OptStat  = gStyle->GetOptStat();
//   Int_t save_OptFit   = gStyle->GetOptFit();
//   Int_t save_OptTitle = gStyle->GetOptTitle();

   Int_t save_opt = 0, active_opt = 0;
   save_opt = fOptStat;
	for (Int_t i = 0; i < kNopts; i++) {
	   if (fOpts[i] == 1)
	      active_opt += mask;
	    mask *= 10;
	}
   gStyle->SetOptStat(active_opt);
	if ( fHist && HasFunctions(fHist) ) {
		gStyle->SetOptFit(fShowFitBox);
	}
	gStyle->SetOptTitle(fShowTitle);
//	TEnv env(".hpprc");
	// avoid saving of statbox size
//	Int_t rstatbox = env.GetValue("GeneralAttDialog.fRememberStatBox", 0);
   TIter next(canvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) {
         TH1* hh = (TH1*)obj;
		   if ( active_opt && fShowStatBox ) {
            if ( active_opt != save_opt ) {
				   hh->SetStats(0);
//				   cout << "hh->SetStats(0); " << endl;
               canvas->Modified();
               canvas->Update();
            }
				hh->SetStats(1);
//				cout << "hh->SetStats(1); " << endl;
		   } else {
				hh->SetStats(0);
//				cout << "hh->SetStats(0); "<< hh << endl;
		   } 
      }
   }
   canvas->Modified();
   canvas->Update();
   fOptStat = active_opt;
//	if ( rstatbox )
//		env.SetValue("GeneralAttDialog.fRememberStatBox", 1);
//   gStyle->SetOptStat(save_OptStat);
//   gStyle->SetOptFit(save_OptFit);
//   gStyle->SetOptTitle(save_OptTitle);
//   SaveDefaults();
}
//_______________________________________________________________________

void WhatToShowDialog::SetDefaults()
{
   TEnv env(".hprrc");
   gStyle->SetOptStat (env.GetValue("WhatToShowDialog.fOptStat1Dim", 1001111));
//   gStyle->SetOptFit  (env.GetValue("WhatToShowDialog.fShowFitBox1Dim", 1));
   gStyle->SetOptTitle(env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1));
}
//_______________________________________________________________________

void WhatToShowDialog::SaveDefaults()
{
   TEnv env(".hprrc");
//   cout << " fOptStat "<< fOptStat<< " dim "  << fHist->GetDimension() << endl;
   if (fHist->GetDimension() == 1) {
//		gStyle->SetOptStat(fOptStat1Dim);
//		gStyle->SetOptFit(fShowFitBox1Dim);
//		gStyle->SetOptTitle(fShowTitle1Dim);
		env.SetValue("WhatToShowDialog.fOptStat1Dim",          fOptStat);
		env.SetValue("WhatToShowDialog.fShowDateBox1Dim",      fShowDateBox);
		env.SetValue("WhatToShowDialog.fShowStatBox1Dim",      fShowStatBox);
		env.SetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", fUseTimeOfDisplay);
		env.SetValue("WhatToShowDialog.fShowTitle1Dim",        fShowTitle);
		env.SetValue("WhatToShowDialog.fShowFitBox1Dim",       fShowFitBox);
   } else if ( fHist->GetDimension() == 2) {
//		gStyle->SetOptStat(fOptStat2Dim);
//		gStyle->SetOptFit(fShowFitBox2Dim);
//		gStyle->SetOptTitle(fShowTitle2Dim);
		env.SetValue("WhatToShowDialog.fOptStat2Dim",          fOptStat);
		env.SetValue("WhatToShowDialog.fShowDateBox2Dim",      fShowDateBox);
		env.SetValue("WhatToShowDialog.fShowStatBox2Dim",      fShowStatBox);
		env.SetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", fUseTimeOfDisplay);
		env.SetValue("WhatToShowDialog.fShowTitle2Dim",        fShowTitle);
		env.SetValue("WhatToShowDialog.fShowFitBox2Dim",       fShowFitBox);
		env.SetValue("WhatToShowDialog.fLegoSuppressZero",     fLegoSuppressZero);
   } else {
		env.SetValue("WhatToShowDialog.fOptStat3Dim",          fOptStat);
		env.SetValue("WhatToShowDialog.fShowDateBox3Dim",      fShowDateBox);
		env.SetValue("WhatToShowDialog.fShowStatBox3Dim",      fShowStatBox);
		env.SetValue("WhatToShowDialog.fUseTimeOfDisplay3Dim", fUseTimeOfDisplay);
		env.SetValue("WhatToShowDialog.fShowTitle3Dim",        fShowTitle);
   }
   env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void WhatToShowDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   if (fHist->GetDimension() == 1) {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat1Dim", 1001111);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox1Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox1Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox1Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", 1);
   } else if (fHist->GetDimension() == 2) {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat2Dim", 11111);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox2Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle2Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox2Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox2Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", 1);
		fLegoSuppressZero = env.GetValue("WhatToShowDialog.fLegoSuppressZero",  1);
   } else {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat3Dim", 11111);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle3Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox3Dim", 0);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox3Dim", 0);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", 1);
	}
//   gStyle->SetOptFit(fShowFitBox);
}
//______________________________________________________________________

void WhatToShowDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void WhatToShowDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   SetWhatToShowNow(canvas);
}


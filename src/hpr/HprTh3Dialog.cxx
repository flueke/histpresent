
#include "TROOT.h"
#include "TEnv.h"
#include "TStyle.h"
#include "HprTh3Dialog.h"
#ifdef MARABOUVERS
#include "HistPresent.h"
#include "hprbase.h"
#endif
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(HprTh3Dialog)

HprTh3Dialog::HprTh3Dialog(TH3 *hist, TGWindow * win) : fHist(hist)
{
static const Char_t helptext[] =
"Manipulate 3d (TH3) histograms\n\
Projections are defined by \"xy\" \"xz\" etc.\n\
";
//   gROOT->GetListOfCleanups()->Add(this);
	cout << "HprTh3Dialog " << win << endl;
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->cd();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
   Int_t ind = 0;
// find all 3d hists in pad
   TIter next(gPad->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH3"))
         fHistList.Add(obj);
   }
   fHistList.SetName("TH3_list");
   fNhists = fHistList.GetSize();
   cout << "3d hists found: " << fNhists << endl;
   if (fNhists > kMaxHists) {
       fNhists = kMaxHists;
       cout << "set to: " << kMaxHists << endl;
   }
//   fNhists = 1;
   RestoreDefaults();
   for (Int_t  i = 0; i < kMaxHists*3 + 10; i++)
      fSetIndex[i] = -1;
   fRow_lab = new TList();
   static TString execute_cmd("ExecuteProject()");
   static Int_t dummy = 0;
   for (Int_t i =0; i < fNhists; i++) {
      TH3 *h = (TH3*)fHistList.At(i);
      fMarkerStyle[i] = h->GetMarkerStyle();
      fMarkerColor[i] = h->GetMarkerColor();
      fMarkerSize[i]  = h->GetMarkerSize();
      TString temp("Mark_Select_Hist_");
      temp += i;
      fRow_lab->Add(new TObjString(temp));
      fValp[ind++] = &fMarkerStyle[i];
      fRow_lab->Add(new TObjString("ColorSelect+Color"));
      fValp[ind++] = &fMarkerColor[i];
      fRow_lab->Add(new TObjString("Float_Value+Size"));
      fValp[ind++] = &fMarkerSize[i];
      fRow_lab->Add(new TObjString("Exec_Button+Set"));
      fSetIndex[ind] = i;
      fValp[ind++] = &dummy;
//      cout << "ind, i " << ind << " " << i << endl;
   }
   fRow_lab->Add(new TObjString("StringValue_Project on"));
   fValp[ind++] = &fProjectPlane;

   fRow_lab->Add(new TObjString("CommandButt+Execute Project()"));
   fValp[ind++] = &execute_cmd;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("TH3 dialog", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

HprTh3Dialog::~HprTh3Dialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
	fHistList.Clear("nodelete");
	if (fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
}
//____________________________________________________________________________

void HprTh3Dialog::ExecuteProject()
{
   TH1 *hproj = fHist->Project3D(fProjectPlane);
#ifdef MARABOUVERS
   HistPresent *hpr = (HistPresent*)gROOT->GetList()->FindObject("mypres");
   if ( hpr ) {
      hpr->ShowHist(hproj);
      gPad->Modified();
      gPad->Update();
      return;
   }
#endif
   TString title(hproj->GetName());
   title.Prepend("C_");
   TCanvas *ch = new TCanvas(title, title, 500, 100, 600,400);
   hproj->Draw();
   ch->Update();
};
//____________________________________________________________________________

void HprTh3Dialog::SaveDefaults()
{
//   cout << "HprTh3Dialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   TString temp;
   for (Int_t i =0; i < fNhists; i++) {
      temp = "HprTh3Dialog.";
      temp += "fMarkerStyle_";
      temp += i;
      env.SetValue(temp, fMarkerStyle[i]);
      temp = "HprTh3Dialog.";
      temp += "fMarkerSize_";
      temp += i;
      env.SetValue(temp, fMarkerSize[i]);
      temp = "HprTh3Dialog.";
      temp += "fMarkerColor_";
      temp += i;
      env.SetValue(temp, fMarkerColor[i]);
   }
   env.SetValue("HprTh3Dialog.fProjectPlane", fProjectPlane);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void HprTh3Dialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   TString temp;
   for (Int_t i =0; i < fNhists; i++) {
      temp = "HprTh3Dialog.";
      temp += "fMarkerStyle_";
      temp += i;
      fMarkerStyle[i] = env.GetValue(temp, 2);
      temp = "HprTh3Dialog.";
      temp += "fMarkerSize_";
      temp += i;
      fMarkerSize[i] = env.GetValue(temp, 0.2);
      temp = "HprTh3Dialog.";
      temp += "fMarkerColor_";
      temp += i;
      fMarkerColor[i] = env.GetValue(temp, i + 2);
   }
   fProjectPlane = env.GetValue("HprTh3Dialog.fProjectPlane", "xy");
}
//_______________________________________________________________________

void HprTh3Dialog::CloseDialog()
{
//   cout << "HprTh3Dialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void HprTh3Dialog::CloseDown(Int_t wid)
{
//   cout << "HprTh3Dialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}
//__________________________________________________________________

void HprTh3Dialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
   
//   cout << "CRButtonPressed: wid "  << wid<< " bid " <<bid
//        << endl;
   if (bid < 0 || bid > kMaxHists*3 + 10 ) return;
   Int_t iHist = fSetIndex[bid];
   if (iHist < 0 ) return;

   TH3 *h = (TH3*)fHistList.At(iHist);
   if ( h ) {
      h->SetMarkerColor(fMarkerColor[iHist]);
      h->SetMarkerStyle(fMarkerStyle[iHist]);
      h->SetMarkerSize(fMarkerSize[iHist]);
      gPad->Modified();
      gPad->Update();
   }
}



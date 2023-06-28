
#include "TROOT.h"
#include "TEnv.h"
#include "TAxis.h"
#include "THprGaxis.h"
#include "TLine.h"
#include "TMath.h"
#include "TStyle.h"
#include "InsertAxisDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertAxisDialog)

InsertAxisDialog::InsertAxisDialog()
{
static const Char_t helptext[] =
"Insert axis\n\
";
   fCanvas = gPad->GetCanvas();
   fWindow = NULL;
   if (fCanvas) {
      fWindow = (TRootCanvas*)fCanvas->GetCanvasImp();
      fWindow->Connect("CloseWindow()", "InsertAxisDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
   static TString execute_cmd("ExecuteInsert()");

   fRow_lab->Add(new TObjString("DoubleValue_X Start"));
   fValp[ind++] = &fX0;
   fRow_lab->Add(new TObjString("DoubleValue+Y Start"));
   fValp[ind++] = &fY0;
   fRow_lab->Add(new TObjString("DoubleValue_X End"));
   fValp[ind++] = &fX1;
   fRow_lab->Add(new TObjString("DoubleValue+Y End"));
   fValp[ind++] = &fY1;

   fRow_lab->Add(new TObjString("DoubleValue_Value at Start"));
   fValp[ind++] = &fWmin;
   fRow_lab->Add(new TObjString("DoubleValue+Value at End"));
   fValp[ind++] = &fWmax;
   fRow_lab->Add(new TObjString("PlainIntVal_N divisions"));
   fValp[ind++] = &fNdiv;
   fRow_lab->Add(new TObjString("CheckButton+Log scale"));
   fValp[ind++] = &fLogScale;
   fRow_lab->Add(new TObjString("CheckButton_Use"));
   fValp[ind++] = &fUseTimeFormat;
   fRow_lab->Add(new TObjString("StringValue-Timeformat"));
   fValp[ind++] = &fTformat;
   fRow_lab->Add(new TObjString("PlainIntVal_Timeoffset"));
   fValp[ind++] = &fTimeZero;

   fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
   fValp[ind++] = &execute_cmd;

   Int_t itemwidth =  35 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Insert Axis", NULL, &ok,itemwidth, fWindow,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
       GrCanvas* hc = (GrCanvas*)fCanvas;
       hc->Add2ConnectedClasses(this);
   }
}
//____________________________________________________________________________

void InsertAxisDialog::ExecuteInsert()
{
   TString chopt;
   Bool_t bycursor = kFALSE;
   if (fUseTimeFormat > 0) chopt += "t";
   if (fLogScale   > 0) chopt += "G";

   if (fX0 == 0 && fY0 == 0 && fX1 == 0 && fY1 == 0) {
   	cout << "Input a TLine defining the axis" << endl;
      TLine * l = (TLine*)GrCanvas::WaitForCreate("TLine", &fPad);
      if (l) {
         fX0 = l->GetX1();
         fY0 = l->GetY1();
         fX1 = l->GetX2();
         fY1 = l->GetY2();
         delete l;
         if (TMath::Sqrt((fX1-fX0)*(fX1-fX0) + (fY1-fY0)*(fY1-fY0)) < 0.00001) {
				cout << "Line length 0" << endl;
				return;
			}
         bycursor = kTRUE;
      } else {
         cout << "No TLine found, try again" << endl;
         return;
      }
   }
   THprGaxis *ax = new THprGaxis(fX0, fY0, fX1, fY1, fWmin, fWmax, fNdiv, chopt.Data());
   TString whichA;
   if (TMath::Abs(fY1-fY0) < TMath::Abs(fX1-fX0)) whichA = "X";
   else                                       whichA = "Y";
   ax->SetLineColor(gStyle->GetAxisColor(whichA.Data()));
   ax->SetLabelColor(gStyle->GetLabelColor(whichA.Data()));
   ax->SetLabelOffset(gStyle->GetLabelOffset(whichA.Data()));
   ax->SetLabelFont(gStyle->GetLabelFont(whichA.Data()));
   ax->SetLabelSize(gStyle->GetLabelSize(whichA.Data()));
   ax->SetTickSize(gStyle->GetTickLength(whichA.Data()));
   ax->SetTitleSize(gStyle->GetTitleSize(whichA.Data()));
   ax->SetTitleOffset(gStyle->GetTitleOffset(whichA.Data()));

   if (fUseTimeFormat) ax->SetTimeFormat(fTformat.Data());
   ax->Draw();
   gPad->Modified();
   gPad->Update();
   if ( bycursor ) {
     fX0 = fY0 = fX1 = fY1 = 0;
   }
   SaveDefaults();
};
//____________________________________________________________________________

void InsertAxisDialog::SaveDefaults()
{
//   cout << "InsertAxisDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertAxisDialog.fX0", fX0);
   env.SetValue("InsertAxisDialog.fY0", fY0);
   env.SetValue("InsertAxisDialog.fX1", fX1);
   env.SetValue("InsertAxisDialog.fY1", fY1);
   env.SetValue("InsertAxisDialog.fWmin", fWmin);
   env.SetValue("InsertAxisDialog.fWmax", fWmax);
   env.SetValue("InsertAxisDialog.fNdiv", fNdiv);
   env.SetValue("InsertAxisDialog.fLogScale", fLogScale);
   env.SetValue("InsertAxisDialog.fUseTimeFormat", fUseTimeFormat);
   env.SetValue("InsertAxisDialog.fTimeZero", fTimeZero);
   env.SetValue("InsertAxisDialog.fTformat", fTformat);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertAxisDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fX0 = env.GetValue("InsertAxisDialog.fX0", 0);
   fY0 = env.GetValue("InsertAxisDialog.fY0", 0);
   fX1 = env.GetValue("InsertAxisDialog.fX1", 0);
   fY1 = env.GetValue("InsertAxisDialog.fY1", 0);
   fWmin = env.GetValue("InsertAxisDialog.fWmin", 0);
   fWmax = env.GetValue("InsertAxisDialog.fWmax", 10);
   fNdiv = env.GetValue("InsertAxisDialog.fNdiv", 510);
   fLogScale = env.GetValue("InsertAxisDialog.fLogScale", 0);
   fUseTimeFormat = env.GetValue("InsertAxisDialog.fUseTimeFormat", 0);
   fTimeZero = env.GetValue("InsertAxisDialog.fTimeZero", 0);
   fTformat = env.GetValue("InsertAxisDialog.fTformat", "H.%H M.%M S.%S");
}
//_________________________________________________________________________

InsertAxisDialog::~InsertAxisDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if (fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void InsertAxisDialog::CloseDialog()
{
//   cout << "InsertAxisDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertAxisDialog::CloseDown(Int_t wid)
{
//   cout << "InsertAxisDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}


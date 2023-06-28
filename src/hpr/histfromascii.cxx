#include "TArrayD.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TNtupleD.h"
#include "TString.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TRegexp.h"
#include "TPaveStats.h"
#include "HistPresent.h"
#include "Ascii2GraphDialog.h"
#include "Ascii2HistDialog.h"
#include "Ascii2NtupleDialog.h"
#include "support.h"
#include "TGMrbValuesAndText.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "FitOneDimDialog.h"
#include <iostream>
#include <fstream>

namespace std {} using namespace std;

//________________________________________________________________________________________

void HistPresent::NtupleFromASCII(TGWindow * win)
{
    new Ascii2NtupleDialog(win);
    return;
}
//________________________________________________________________________________________

void HistPresent::GraphFromASCII(TGWindow * win)
{
   static Int_t winx = 100,  winy = 100;
   GetWindowPosition(&winx, &winy);
   new Ascii2GraphDialog(win, winx, winy);
   return;
}
;
//________________________________________________________________________________________

void HistPresent::Graph2DFromASCII(TGWindow * win)
{
   static Int_t winx = 100,  winy = 100;
   GetWindowPosition(&winx, &winy);
   new Ascii2GraphDialog(win, winx, winy, 2);
   return;
}
;
//________________________________________________________________________________________

void HistPresent::HistFromASCII(TGWindow * win)
{
    new Ascii2HistDialog(win);
    return;
}
;
//____________________________________________________________________________________

const char Poly2[]=
"/*Polynomial a0 + b*x + c*x*x (formula)*/\n\
fit_user_function(const char *gname)\n\
//\n\
// example with fit function given as formula\n\
//\n\
// This is a template macro to fit a user defined function\n\
// A polynomial 2nd deg is defined  as a formula\n\
{\n\
   TGraph * graph = 0;\n\
   if (gPad) {\n\
      graph = (TGraph*)gPad->GetListOfPrimitives()->FindObject(gname);\n\
   }\n\
   if(!graph){\n\
     cout << \"graph: \" << gname << \" not found\" << endl;\n\
     return 0;\n\
   }\n\
   Float_t from =  0; \n\
   Float_t to   = 20;\n\
   TF1* f = new TF1(\"poly2\",\"pol2\",from,to);\n\
   f->SetParameters(0, 1, 0);\n\
   f->SetParName(0,\"const \");\n\
   f->SetParName(1,\"slope \");\n\
   f->SetParName(2,\"quadr \");\n\
   f->SetLineColor(graph->GetMarkerColor());\n\
   graph->Fit(\"poly2\",\"R+\",\"same\");\n\
}\n\
";

const char ExpGaussG[]=
"/*Exponential + Gauss (formula)*/\n\
fit_user_function(const char *gname)\n\
//\n\
// example with fit function given as formula\n\
//\n\
// This is a template macro to fit a user defined function\n\
// As an example an exponential + a gauss function is provided\n\
// The function is defined  as a formula\n\
{\n\
   TGraph * graph = 0;\n\
   if (gPad) {\n\
      graph = (TGraph*)gPad->GetListOfPrimitives()->FindObject(gname);\n\
   }\n\
   if(!graph){\n\
     cout << \"graph: \" << gname << \" not found\" << endl;\n\
     return 0;\n\
   }\n\
   Float_t from =  0; \n\
   Float_t to   = 10;\n\
   TF1* f = new TF1(\"exp_gaus\",\"[0] + [1]*exp([2]*x) + gaus(3)\",from,to);\n\
   f->SetParameters(50, 200, -1, 200, 4, 1);\n\
   f->SetParName(0,\"lin_const \");\n\
   f->SetParName(1,\"exp_const \");\n\
   f->SetParName(2,\"exp_slope \");\n\
   f->SetParName(3,\"gaus_const\");\n\
   f->SetParName(4,\"gaus_mean \");\n\
   f->SetParName(5,\"gaus_sigma\");\n\
   f->SetLineColor(graph->GetMarkerColor());\n\
   graph->Fit(\"exp_gaus\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char LandauG[]=
"/*Three landaus*/\n\
Double_t Landau_f ( Double_t *x, Double_t *par) \n\
{\n\
    Double_t BinW = 9;\n\
//   linear background + 3 landau functions\n\
//   tail may be on high or low energy side\n\
//\n\
   double x0 = 2 * par[3] - x[0];     // low energy tail\n\
   double x1 = 2 * par[6] - x[0];     // low energy tail\n\
   double x2 = 2 * par[9] - x[0];     // low energy tail\n\
//   double x0 = x[0];             // high  energy tail\n\
//   double x1 = x[0];             // high  energy tail\n\
//   double x2 = x[0];             // high  energy tail\n\
   if(par[4] == 0)  par[4] = 1;\n\
   if(par[7] == 0)  par[7] = 1;\n\
   if(par[10] == 0) par[10]= 1;\n\
   double val =  par[0] + x[0] * par[1]\n\
   + par[2] / par[4]  * TMath::Landau(x0, par[3], par[4])\n\
   + par[5] / par[7] * TMath::Landau(x1, par[6], par[7])\n\
   + par[8] / par[10]* TMath::Landau(x2, par[9], par[10]);\n\
   return BinW * val;\n\
}\n\
\n\
fit_user_function(const char *gname){\n\
\n\
//  ----------------------------------------------------------------------------   \n\
   TGraph * graph = 0;\n\
   if (gPad) {\n\
      graph = (TGraph*)gPad->GetListOfPrimitives()->FindObject(gname);\n\
   }\n\
   if(!graph){\n\
     cout << \"graph not found\" << endl;\n\
     return 0;\n\
   }\n\
   Double_t from = 1600; \n\
   Double_t to   = 2150;\n\
   Int_t npar    = 11;\n\
   TF1 * f    = new TF1(\"landf\",Landau_f, from, to, npar);\n\
\n\
   Double_t backconst = 120;\n\
   Double_t backslope = - 0.05;\n\
\n\
   Double_t cont0  = 2800;\n\
   Double_t mean0  = 1750;\n\
   Double_t width0 = 5;\n\
   Double_t cont1  = 650;\n\
   Double_t mean1  = 2010;\n\
   Double_t width1 = 5;\n\
   Double_t cont2  = 200;\n\
   Double_t mean2  = 2055;\n\
   Double_t width2 = 5;\n\
  \n\
   f->SetParameters(backconst, backslope,\n\
                    cont0, mean0, width0, \n\
                    cont1, mean1, width1); \n\
   f->SetParameter(8,  cont2);        // SetParameters allows only 9 arguments\n\
   f->SetParameter(9,  mean2);\n\
   f->SetParameter(10, width2);\n\
   f->Draw(\"same\");                        //  draw function with start parameters\n\
   f->SetLineColor(2);    \n\
   f->SetLineWidth(3);\n\
\n\
   f->SetParName(0,\"const\");\n\
   f->SetParName(1,\"slope\");\n\
   f->SetParName(2,\"cont_0 \");\n\
   f->SetParName(3,\"mean_0 \");\n\
   f->SetParName(4,\"width_0\");\n\
   f->SetParName(5,\"cont_1 \");\n\
   f->SetParName(6,\"mean_1 \");\n\
   f->SetParName(7,\"width_1\");\n\
   f->SetParName(8,\"cont_2 \");\n\
   f->SetParName(9,\"mean_2 \");\n\
   f->SetParName(10,\"width_2\");\n\
//   f-Draw,\"same\");   // only draw function, dont do fit\n\
\n\
   graph->Fit(\"landf\",\"R+\",\"same\");   // the real fit\n\
\n\
\n\
}\n\
";
const Int_t nFitTemplatesG = 3;
const char *FitMacroTemplatesG[nFitTemplatesG] = {
   &Poly2[0],
   &ExpGaussG[0],
   &LandauG[0],
};

TString gFitMacroNameG;

//____________________________________________________________________________________

void EditFitMacroG(TGWindow * win)
{
   const char hist_file[] = {"hprFitMacrosG.txt"};
   const char * hf = hist_file;
   if (gROOT->GetVersionInt() < 40101) hf = NULL;
   static TString name(gFitMacroNameG.Data());
   Bool_t ok;
   name = GetString("Name of Fit Macro", name.Data(), &ok, win,
                     0,0,0,0,0, hf);
   if (!ok)
      return;
   if (gSystem->AccessPathName(name.Data())) {
      cout << "Try to open: " << name.Data() << endl;

      ofstream tmpfile(name.Data(), ios::out);
      if (!tmpfile.good()) {
         cerr << "EditFitMacroG: "
             << gSystem->GetError() << " - " << name.Data()
             << endl;
         return;
      }
      if (!tmpfile) {
         cout << "Cant open new " << name.Data() << endl;
         return;
      }
      TOrdCollection *svalues = new TOrdCollection();
      for (Int_t i = 0; i < nFitTemplatesG; i++) {
         TString temp(FitMacroTemplatesG[i]);
         Int_t cstart = temp.Index("/*");
         Int_t cend = -1;
         if (cstart >= 0) cend = temp.Index("*/");
         TString mname("NoName");
         if (cstart >= 0 && cend > cstart)
            mname = temp(cstart + 2, cend - cstart - 2);

         svalues->Add(new TObjString(mname.Data()));
      }

      TArrayI flags(nFitTemplatesG);
      TString title("Template Fit Macros");
      Int_t retval = 0;
      Int_t itemwidth = 240;
      new TGMrbTableFrame(win, &retval,
      						  title.Data(),
      						  itemwidth, 1,
      						  nFitTemplatesG,
      						  svalues, 0, 0, &flags,
      						  nFitTemplatesG);
      if (retval < 0) {
         return;
      }
		ok = kFALSE;
      for (Int_t i = 0; i < nFitTemplatesG; i++) {
         if (flags[i] == 1) {
            tmpfile << FitMacroTemplatesG[i];
            ok = kTRUE;
            break;
         }
      }
      if (!ok) {
         cout << "no selection" << endl;
         return;
      }
      tmpfile.close();
   }
   TString EditCmd = gSystem->Getenv("EDITOR");
	if ( EditCmd.Length() == 0 )
		EditCmd = "kate";
	EditCmd += " ";
   EditCmd += name.Data();
   EditCmd += "&";
   gSystem->Exec(EditCmd.Data());
   gFitMacroNameG = name;
}

//____________________________________________________________________________

void ExecFitMacroG(TGraph * graph, TGWindow * win)
{
//   cout << gFitMacroName.Data()<< endl;
   if (!gPad || !graph) return;
   static TGWindow * winold = 0;
   const char hist_file[] = {"hprFitMacrosG.txt"};
   const char * hf = hist_file;
   if (gROOT->GetVersionInt() < 40101) hf = NULL;
   TList logr;
   logr.SetOwner(kFALSE);
   TList pads;
   pads.SetOwner(kFALSE);
   TGraph * gr = NULL;
   TPad * pad = NULL;
   Int_t ngr = FindGraphs(gPad, &logr, &pads);
   if (ngr == 0) {
      cout << "No graph in current pad" << endl;
      return;
   }

   if (ngr > 1) {
      cout << "More than 1 graph in current canvas" << endl;
      TOrdCollection *values  = new TOrdCollection();
      TArrayI flags(ngr);
      for (Int_t i = 0; i < ngr; i++) {
         const char * name = ((TGraph*)logr.At(i))->GetName();
         values->Add(new TObjString(name));
         if (i == 0) flags[i] = 1;
         else        flags[i] = 0;
      }
      Int_t retval = 0;
      Int_t itemwidth = 280;
      new TGMrbTableFrame(win, &retval,
      						  "Select graph",
      						  itemwidth, 1,
      						  ngr,
      						  values, 0, 0, &flags,
      						  ngr);
      if (retval < 0) {
         logr.Clear("nodelete");
         pads.Clear("nodelete");
         delete values;
         return;
      }
      Bool_t ok = kFALSE;
      for (Int_t i = 0; i < ngr; i++) {
         if (flags[i] == 1) {
            ok = kTRUE;
            gr = (TGraph*)logr.At(i);
            pad = (TPad*)pads.At(i);
            break;
         }
      }
      delete values;
      if (!ok) {
         cout << "no selection" << endl;
         logr.Clear("nodelete");
         pads.Clear("nodelete");
         return;
      }
   } else {
      gr = (TGraph*)logr.First();
      pad = (TPad*)pads.First();
      cout << "gPad " << gPad  << " pad " << pad << endl;
//      pad->cd();
   }
   if (winold != win) {
      Bool_t ok;
      gFitMacroNameG =
          GetString("Use Macro:", gFitMacroNameG.Data(), &ok, win,
          0,0,0,0,0, hf);
      if (!ok) {
         logr.Clear("nodelete");
         pads.Clear("nodelete");
         return;
      }
      winold = win;
   }
   if (!gSystem->AccessPathName(gFitMacroNameG.Data(), kFileExists)) {
// look if there is already a statbox, take the new one doesnt hide it
      TIter next(&logr);
      TPaveStats *st;
      Double_t yoff = 0.;
      while (TGraph* gro = (TGraph*)next()) {
         st = (TPaveStats*)gro->GetListOfFunctions()->FindObject("stats");
         if (st) yoff = st->GetY2NDC() - st->GetY1NDC();
      }
      gROOT->LoadMacro(gFitMacroNameG.Data());
      TString cmd = "fit_user_function";
//      Int_t p = cmd.Index(".");
//      cmd.Remove(p,2);
      cmd = cmd + "(\"" + gr->GetName() + "\")";
      cout <<"Execute: " <<  cmd << endl;
//      cout << "gPad " << gPad  << " pad " << pad << endl;
      pad->cd();
      gROOT->ProcessLine((const char *) cmd);
      gPad->Modified(kTRUE);
      gPad->Update();
      st = (TPaveStats*)gr->GetListOfFunctions()->FindObject("stats");
      if (st) {
         st->SetLineColor(gr->GetMarkerColor());
         st->AddText(0.1, 0.1, gr->GetName());
         if (yoff > 0) {
            st->SetY1NDC(st->GetY1NDC() - yoff);
            st->SetY2NDC(st->GetY2NDC() - yoff);
         }
      }
      gPad->Modified(kTRUE);
      gPad->Update();
   } else
      cout << "Macro not found" << endl;
   logr.Clear("nodelete");
   pads.Clear("nodelete");
}

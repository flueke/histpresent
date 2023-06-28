#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "TROOT.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TCutG.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TGObject.h"
#include "TObjectTable.h"
#include "TProfile.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TPolyLine.h"
#include "TPaveLabel.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "Buttons.h"
#include "TButton.h"
#include "TDiamond.h"
#include "HTCanvas.h"
#include "TContextMenu.h"
#include "TArray.h"
//#include "TArrayF.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TRint.h"
#include "TList.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TArrayC.h"
#include "TArrayI.h"

#include "TGMsgBox.h"
#include "TGWidget.h"
#include "TGaxis.h"
#include "TRootHelpDialog.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitOneDimDialog.h"
//#include "FitHist_Help.h"
#include "CmdListEntry.h"
#include "HistPresent.h"

#include "FhMarker.h"
#include "support.h"
#include "TGMrbInputDialog.h"
#include "TGMrbTableFrame.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "Save2FileDialog.h"

extern HistPresent *gHpr;
extern Int_t gHprDebug;

enum dowhat { expand, projectx, projecty, statonly, projectf,
		 projectboth };

//____________________________________________________________________________________
/* *INDENT-OFF* */
//____________________________________________________________________________________
const char SliceYTemplate[]=
"Double_t bw( Double_t *x, Double_t *par)\n\
{\n\
//  1 dim function  breitwigner\n\
\n\
	Double_t norm = par[0];  \n\
	Double_t meanbw = par[1];  \n\
	if (par[2] == 0) par[2] = 1;\n\
	Double_t gamma2 = par[2] * par[2];    // Gamma square of bw\n\
\n\
	Double_t eb2 = (x[0] - meanbw) * (x[0] - meanbw);\n\
	Double_t denominator = eb2 + 0.25 * gamma2;\n\
	if (denominator == 0)denominator = 1e-10;\n\
	Double_t yval =  gamma2 / denominator;\n\
 \n\
	return norm * yval;\n\
};\n\
//___________________________________________________________________\n\
\n\
fit_slice_function(const char *hname, Double_t from, Double_t to,\n\
						 Int_t binX_min, Int_t binX_max)\n\
// This is a template macro to fit a BreitWigner to slices\n\
{\n\
	TH2* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
	  return 0;\n\
	}\n\
\n\
//  range used along  y\n\
//  from = -8; \n\
//  to   =  8; \n\
//  range used along  x \n\
//   binX_min = 30;\n\
//   binX_max = 70;\n\
//  use only bins with at least nBin_filled bins with entries\n\
	Int_t nBin_filled = 10;\n\
\n\
//   if (c_from) from = atof(c_from);\n\
 //  if (c_to)   to   = atof(c_to);\n\
 //  if (c_bmin) binX_min = atoi(c_bmin);\n\
//   if (c_bmax) binX_max = atoi(c_bmax);\n\
	// define function\n\
	TF1 * f1 = new TF1(\"bw\", bw, from, to, 3);\n\
\n\
	f1->SetParName(0, \"norm\");\n\
	f1->SetParName(1, \"mean\");\n\
	f1->SetParName(2, \"gamma\");\n\
\n\
	f1->SetParameters(1000, -.3, 1);\n\
\n\
// option: \"R\" use range of function (from, to)\n\
//         \"Q\" quiet\n\
//         \"N\" avoid clearing of current pad \n\
//         \"G2\" merge 2 consecutive bins along Y\n\
//         \"G3\" merge 3 consecutive bins along Y\n\
//         \"G4\" merge 4 consecutive bins along Y\n\
//         \"G5\" merge 5 consecutive bins along Y\n\
	char opt[] = \"RQN\";\n\
\n\
	hist->FitSlicesY(f1, binX_min, binX_max, nBin_filled, opt); \n\
}\n\
";
//_____________________________________________________________________________________

const char Gauss2d[]=
"Double_t gauss2d( Double_t *x, Double_t *par)\n\
// a 2d gauss function\n\
{\n\
	Double_t sqrt2pi = sqrt(2*TMath::Pi()), sqrt2 = sqrt(2.);\n\
	Double_t cx = par[0];    // const x\n\
	Double_t mx = par[1];    // mean x\n\
	if (par[2] == 0) par[2] = 1;\n\
	Double_t sx = par[2];    // sigma x\n\
	Double_t cy = par[3];    // const y\n\
	Double_t my = par[4];    // mean y\n\
	if (par[5] == 0) par[5] = 1;\n\
	Double_t sy = par[5];    // sigma \n\
	Double_t xij = x[0] - mx;  \n\
	Double_t argx = xij/(sqrt2*sx);\n\
	Double_t xval = cx * exp(-argx * argx)/(sqrt2pi*sx);\n\
	Double_t yij = x[1] - my;  \n\
	Double_t argy = yij/(sqrt2*sy);\n\
	Double_t yval = cy * exp(-argy * argy)/(sqrt2pi*sy);\n\
	return xval * yval;\n\
};\n\
//__________________________________________________________\n\
\n\
Double_t double_gauss2d( Double_t *x, Double_t *par)\n\
{\n\
// Sum of two 2d gauss functions\n\
	Double_t* par2 = &par[6];\n\
	return gauss2d(x, par) + gauss2d(x, par2);\n\
}\n\
//__________________________________________________________\n\
\n\
void fit_user_function(const char *hname)\n\
// This is a macro to fit a user defined 2d function\n\
// it uses a sum of two 2d gauss functions (12 parameters) \n\
{\n\
//   gROOT->Reset();\n\
	TH2* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
	  return 0;\n\
	}\n\
//  try to use same canvas dimensions for fitted function as used for histogram \n\
	TCanvas * mc  = (TCanvas *) gPad;\n\
	Int_t wtopx = 100, wtopy = 100;\n\
	UInt_t ww = 750, wh = 750;\n\
	Double_t theta = 0, phi = 0; projectf\n\
	TString cname(\"canvas_2d\");\n\
	if (mc) {\n\
		mc->GetCanvasPar(wtopx, wtopy, ww, wh);\n\
		cname = mc->GetName();\n\
		cname += \"_2df\";\n\
		wtopx = TMath::Max(0,wtopx - 100);\n\
		wtopy = TMath::Max((Int_t)wh, wtopy + 100);\n\
		theta = gPad->GetTheta();\n\
		phi = gPad->GetPhi();\n\
 //      cout << wtopx<<  << wtopy <<  <<  ww<<  <<  wh << endl;\n\
	}\n\
\n\
//  nb: fit range may be different from histo range\n\
	Float_t x_from = -4; \n\
	Float_t x_to   =  4; \n\
	Float_t y_from = -4; \n\
	Float_t y_to   =  4; \n\
	TF2 * f2 = new TF2(\"gauss2d\", double_gauss2d, x_from, x_to, y_from, y_to , 12);\n\
	f2->SetParName(0, \"const_x1\");\n\
	f2->SetParName(1, \"mean_x1\");\n\
	f2->SetParName(2, \"sigma_x1\");\n\
	f2->SetParName(3, \"const_y1\");\n\
	f2->SetParName(4, \"mean_y1\");\n\
	f2->SetParName(5, \"sigma_y1\");\n\
\n\
	f2->SetParName(6, \"const_x2\");\n\
	f2->SetParName(7, \"mean_x2\");\n\
	f2->SetParName(8, \"sigma_x2\");\n\
	f2->SetParName(9, \"const_y2\");\n\
	f2->SetParName(10, \"mean_y2\");\n\
	f2->SetParName(11, \"sigma_y2\");\n\
\n\
	f2->SetParameter(0, 10);\n\
	f2->SetParameter(1, -1);\n\
	f2->SetParameter(2, 0.5);\n\
	f2->SetParameter(3, 10);\n\
	f2->SetParameter(4, -.5);\n\
	f2->SetParameter(5, 0.5);\n\
\n\
	f2->SetParameter(6, 10);\n\
	f2->SetParameter(7, 1);\n\
	f2->SetParameter(8, 0.3);\n\
	f2->SetParameter(9, 10);\n\
	f2->SetParameter(10,0.5);\n\
	f2->SetParameter(11,0.7);\n\
\n\
 //  f2->Draw();\n\
	hist->Fit(\"gauss2d\",\"R0\",\"same\");  // dont in same picture \n\
	TCanvas *cc = new TCanvas(cname , cname, wtopx, wtopy, ww, wh);\n\
//  draw in same range as histogram with same drawing options\n\
	f2->SetRange(hist->GetXaxis()->GetXmin(), hist->GetYaxis()->GetXmin(),\n\
					 hist->GetXaxis()->GetXmax(), hist->GetYaxis()->GetXmax()); \n\
	f2->SetNpx(hist->GetNbinsX());\n\
	f2->SetNpy(hist->GetNbinsY());\n\
	if (theta != 0 || phi != 0) {\n\
		(TPad*)cc->SetTheta(theta);\n\
		(TPad*)cc->SetPhi(phi);\n\
	}\n\
	f2->Draw(hist->GetOption()); \n\
}\n\
";
//____________________________________________________________________________________

const char ExpGauss[]=
"/*Exponential + Gauss (formula)*/\n\
void fit_user_function(const char *hname)\n\
//\n\
// example with fit function given as formula\n\
//\n\
// This is a template macro to fit a user defined function\n\
// As an example an exponential + a gauss function is provided\n\
// The function is defined  as a formula\n\
{\n\
//   gROOT->Reset();\n\
	TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
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
	hist->Fit(\"exp_gaus\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char TwoGaussCM[]=
"/*Two Gaussians, common mean*/\n\
Double_t TwoGaussCM     ( Double_t *x, Double_t *par) \n\
{\n\
// two gaussians with common mean\n\
  Double_t sqrt2pi = sqrt(2*TMath::Pi()), sqrt2 = sqrt(2.);\n\
  Double_t BinW = 0.25;\n\
  Double_t mean = par[2];\n\
//  force widths /= 0\n\
	if (par[6] == 0)par[6]=1;\n\
	if (par[4] == 0)par[4]=1;\n\
	Double_t xij = x[0] - par[2];   \n\
	Double_t arg0 = xij/(sqrt2*par[4]);\n\
	Double_t arg1 = xij/(sqrt2*par[6]);\n\
	Double_t fitval = par[0] + x[0]*par[1]\n\
			  + par[3] * exp(-arg0*arg0)/(sqrt2pi*par[4])\n\
			  + par[5] * exp(-arg1*arg1)/(sqrt2pi*par[6]);\n\
  return BinW * fitval;\n\
}\n\
\n\
void fit_user_function(const char *hname)\n\
// Fit 2 gaussians with common mean\n\
{\n\
//   gROOT->Reset();\n\
	TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
	  return 0;\n\
	}\n\
	Double_t par[7];\n\
	Double_t from = -4; \n\
	Double_t to   =  4;\n\
\n\
	Double_t lin_const  = 0;\n\
	Double_t lin_slope  = 0;\n\
	Double_t gaus_mean  = 0;\n\
	Double_t gaus0_const= 10000;\n\
	Double_t gaus0_sigma= 2;\n\
	Double_t gaus1_const= 10000;\n\
	Double_t gaus1_sigma= 4;\n\
	TF1* f = new TF1(\"two_gaus\",TwoGaussCM,from,to, 7);\n\
	f->SetParameters(lin_const  ,\n\
						  lin_slope  ,\n\
						  gaus_mean  ,\n\
						  gaus0_const,\n\
						  gaus0_sigma,\n\
						  gaus1_const,\n\
						  gaus1_sigma);\n\
	f->SetParName(0,\"lin_const  \");\n\
	f->SetParName(1,\"lin_slope  \");\n\
	f->SetParName(2,\"gaus_mean  \");\n\
	f->SetParName(3,\"gaus0_const\");\n\
	f->SetParName(4,\"gaus0_sigma\");\n\
	f->SetParName(5,\"gaus1_const\");\n\
	f->SetParName(6,\"gaus1_sigma\");\n\
	f->SetLineColor(4);\n\
	f->SetLineWidth(3);\n\
//  Fill a histogram randomly using function\n\
//   Int_t entries = gaus0_const  + gaus1_const ;\n\
//   TH1F * twogran = new TH1F(\"twogran\",\n\
//                               \"Two gaussians with common mean\",\n\
//                              400, 0, 100);\n\
//   twogran->FillRandom(\"two_gaus\", entries);\n\
//   twogran->Draw();\n\
//   f->Draw(\"same\");   // only draw function, dont do fit\n\
	hist->Fit(\"two_gaus\",\"R+\",\"same\");\n\
\n\
	f->GetParameters(&par[0]);\n\
//  draw first gauss\n\
	TF1* f1 = new TF1(\"gaus1\",TwoGaussCM,from,to, 7);\n\
	f1->SetParameters(0, 0, par[2], par[3], par[4], 0, par[6]);\n\
	f1->SetLineColor(2);\n\
	f1->SetLineWidth(2);\n\
	f1->Draw(\"same\");\n\
\n\
//  draw second gauss\n\
	TF1* f2 = new TF1(\"gaus2\",TwoGaussCM,from,to, 7);\n\
	f2->SetParameters(0, 0, par[2], 0, par[4], par[5], par[6]);\n\
	f2->SetLineColor(6);\n\
	f2->SetLineWidth(2);\n\
	f2->Draw(\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char BreitWigner[]=
"/*Breit Wigner*/\n\
Double_t BreitWigner  ( Double_t *x, Double_t *par) \n\
{\n\
//\n\
// example with fit function defined as function \n\
//  \n\
  // Breit- Wigner function\n\
  Double_t mw = par[0], gw = par[1], mw2, gw2, eb2;\n\
  gw2 = gw*gw;\n\
  eb2 = (x[0] - mw) * (x[0] - mw);\n\
  Double_t dem = eb2 + 0.25 * gw2;\n\
  if (dem == 0) dem = 1e-10;\n\
  return par[2] * gw2 / dem;\n\
}\n\
 \n\
void fit_user_function(const char *hname)\n\
// This is a template macro to fit a user defined function\n\
// As an example a Breit Wigner is provided\n\
// The function is defined  as a call\n\
{\n\
//   gROOT->Reset();\n\
//  ------- comment out this piece if only a sample histogram should be produced ---\n\
	TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
	  return 0;\n\
	}\n\
//  ----------------------------------------------------------------------------   \n\
	Float_t from = -4; \n\
	Float_t to   = 4;\n\
	TF1* f = new TF1(\"BreitWigner\",BreitWigner,from,to, 3);\n\
	f->SetParameters(0, 1, 1000);\n\
	f->SetParName(0,\"mean \");\n\
	f->SetParName(1,\"width \");\n\
	f->SetParName(2,\"cont \");\n\
//  ------- activate  this peace if only a sample histogram should be produced --- \n\
\n\
//  Fill a histogram randomly using function\n\
//   TH1F * breitw = new TH1F(\"breitw\", \"A BreitWigner\",\n\
//                              400, -10, 10);\n\
//   breitw->FillRandom(\"BreitWigner\", 10000);\n\
//  ----------------------------------------------------------------------------   \n\
//   f->Draw(\"same\");   // only draw function, dont do fit\n\
	hist->Fit(\"BreitWigner\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char Landau[]=
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
void fit_user_function(const char *hname){\n\
\n\
//   gROOT->Reset();\n\
//  ------- comment out this piece if only a sample histogram should be produced ---\n\
	TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
	if(!hist){\n\
	  cout << \"histogram not found\" << endl;\n\
	  return 0;\n\
	}\n\
//                                                                             ^   \n\
//  ----------------------------------------------------------------------------   \n\
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
	hist->Fit(\"landf\",\"R+\",\"same\");   // the real fit\n\
\n\
//  ------- activate  this peace if only a sample histogram should be produced --- \n\
\n\
//   TFile * fout = new TFile(\"h_land_6.root\", \"RECREATE\");\n\
//   TH1F * hland = new TH1F(\"hland\",\"Landau functions\", 100, 1600, 2200);\n\
//   hland->FillRandom(\"landf\", 100000);\n\
//   fout->Write(); fout->Close();  \n\
																									 \n\
//  -------------------------------------------------------------------------------   \n\
\n\
}\n\
";
//____________________________________________________________________________________

const char TwoGauss[]=
"/*Two gaussians, diff mean*/ \n\
void fit_user_function(const char *hname) \n\
// \n\
// example with fit function given as formula \n\
// \n\
// This is a template macro to fit a user defined function \n\
// As an example sum of 2 gaussians is provided\n\
// The function is defined  as a formula \n\
{ \n\
	TH1* hist = (TH1*)gROOT->FindObject(hname); \n\
	if(!hist){ \n\
	  cout << \"histogram not found\" << endl; \n\
	  return 0; \n\
	} \n\
	Float_t from = -3; \n\
	Float_t to   =  3;\n\
	TF1* f = new TF1(\"gaus2\",\"gaus + gaus(3)\",from,to); \n\
	f->SetParameters(100, -1, 1, 100, 1, 1); \n\
	f->SetLineColor(4); \n\
	f->SetLineWidth(3); \n\
//   f->Draw(\"same\");                // dont fit, draw only \n\
	hist->Fit(\"gaus2\",\"R\",\"same\"); \n\
}\n\
";

//____________________________________________________________________________________

const char Pol2Sine[]=
"/*Pol2 + Sine, (formula)*/ \n\
void fit_user_function(const char *hname) \n\
// \n\
// example with fit function given as formula \n\
// \n\
// This is a template macro to fit a user defined function \n\
// As an example a sine with an amplitude modulated by a polynomial \n\
// of degree 2 (parabola) is provided \n\
// The function is defined  as a formula \n\
{ \n\
	TH1* hist = (TH1*)gROOT->FindObject(hname); \n\
	if(!hist){ \n\
	  cout << \"histogram not found\" << endl; \n\
	  return 0; \n\
	} \n\
	Float_t from = 500; \n\
	Float_t to   = 3500;\n\
	TF1* f = new TF1(\"sinf\",\"pol2 * sin([3]*x+[4])\",from,to); \n\
	f->SetParameters(0.366, 0, 0, 0.087, -.7); \n\
		f->SetLineColor(4); \n\
	f->SetLineWidth(1); \n\
	f->SetNpx(500); \n\
//   f->Draw(\"same\");                // dont fit, draw only \n\
	hist->Fit(\"sinf\",\"R\",\"same\"); \n\
}\n\
";

//______________________________________________________________________
/* *INDENT-ON* */
const Int_t nFitTemplates = 6;
const char *FitMacroTemplates[nFitTemplates] = {
	&TwoGauss[0],
	&TwoGaussCM[0],
	&ExpGauss[0],
	&BreitWigner[0],
	&Landau[0],
	&Pol2Sine[0]
};

//______________________________________________________________________

void FitHist::HelpFit2dimMarks()
{
	const char * helptext =
"\"FitPolyMarks\"\n\
Fit a polynomial to marks set manually by user.\n\
\n\
";
	TRootHelpDialog * hd = new TRootHelpDialog(mycanvas, "FitPolyMarks", 450, 200);
	hd->SetText(helptext);
	hd->Popup();
}

//______________________________________________________________________

void FitHist::HelpFit2dim()
{
	const char * helptext =
"\"FitPolyHist\"\n\
This isnt a real fit to 2dim data but rather a 1dim fit\n\
to the profile of the 2dim histogram normally with a\n\
cut applied to select a subregion of the histogram.\n\
Within the cut mean and error of the mean value of bins\n\
along Y for each bin in X are calculated and a polynomial\n\
is fitted to the resulting distribution.\n\
If \"User formula\" is selected the formula must be provide\n\
in \".hprrc\" as resource  e.g. like this:\n\
\"FitOneiDialog.fFormula:   [0]+[1]*TMath::Sqrt([3]*x) \" \n\
Note: This is the same resource used in the 1dim fitmenu:\n\
\"User Formula\" and may be prepared there including \n\
start parameters and possible fix parameter settings.\n\
\n\
To visualize the result the variable \"gHprDebug = 1\"\n\
should be set at the ROOt prompt.\n\
This is essentially the same procedure as described for\n\
the ROOT class TProfile with some extra user selections:\n\
\"GeneralAttDialog::fContentLowLimit\"\n\
Use bin only if sum of contents in slice is above this value.\n\
\n\
\"GeneralAttDialog::fVertAdjustLimit\" (Unit bins)\n\
Force left and right limits of a cut to be vertical.\n\
i.e. all points of cut closer to xmin or xmax than \n\
his limit are set to xmin or xmax respectively.\n\
\n\
To customize the default function name the entry\n\
\"Fit2dim.FuncName\" in the resource file \".hprrc\"\n\
should be modified.\n\
The fitted functions may be saved to a ROOT file for later use\n\
This can be done in 2 ways:\n\
1. Check the entry in menu:\n\
   \"Auto display Save2FileDialog after fit\"\n\
   In this case the Save2FileDialog is called automatically\n\
   after the fit.\n\
2. Use the menu item:\n\
   \"Write Functions to File\"\n\
   All currently functions willbe written\n\
   N.B. Names of functions can not be changed in the\n\
   Save2FileDialog, if needed it should be done from\n\
   the functions context menu, click with right mouse\n\
   and use \"SetName\"\n\
The default name appended by an auto increasing index may\n\
modified and are remembered across sessions.\n\
If other functions than polynomials are needed you should\n\
use the option \"ProfileX\" from the \"Display\" menu\n\
to produces a 1dim Profile histogram and use the 1dim fitting\n\
procedures with this. \"ProfileX\" allows application of cuts.\n\
";
	TRootHelpDialog * hd = new TRootHelpDialog(mycanvas, "FitPolyHist", 450, 450);
	hd->SetText(helptext);
	hd->Popup();
}
//______________________________________________________________________
Int_t FitHist::FitPolyHist(Int_t degree)
{
	return Fit2dim(0, degree);
}

//____________________________________________________________________________

Int_t FitHist::FitPolyMarks(Int_t degree)
{
	 return Fit2dim(1, degree);
}


//____________________________________________________________________________

Int_t FitHist::Fit2dim(Int_t what, Int_t ndim)
{

	if (!fSelPad || !fOrigHist) {
		cout << "Cant find pad or hist" << endl;
		return -1;
	}
//  now find limits

	if (!is2dim(fSelHist)) {
		WarnBox("Not 2 dim");
		return -1;
	}
	if (ndim < -1 || ndim > 8) {
		WarnBox("wrong dim, must be 0-8");
		return -1;
	}

	int nval = GetMarks(fSelHist);
	if (nval < 2) {
		WarnBox("Not enough marks");
		return -1;
	}
//  cout << "Fit2dim ----------------" << endl;
//  fMarkers->Print();
//     get values in orig hist
	TAxis *xaxis = fOrigHist->GetXaxis();
	Axis_t xmin = xaxis->GetXmin();
	Axis_t xmax = xaxis->GetXmax();
	Int_t nxb = fOrigHist->GetNbinsX();
	Axis_t xbinwidth = (xmax - xmin) / nxb;

	TAxis *yaxis = fOrigHist->GetYaxis();
	Axis_t ymin = yaxis->GetXmin();
	Axis_t ymax = yaxis->GetXmax();
	Int_t nyb = fOrigHist->GetNbinsY();
	Axis_t ybinwidth = (ymax - ymin) / nyb;

//     get values in expanded hist

	FhMarker *pfirst = (FhMarker *) fMarkers->First();
	FhMarker *plast = (FhMarker *) fMarkers->Last();
	Float_t xlow = pfirst->GetX();
	Float_t xup = plast->GetX();

	UpdateCut();
//   CheckList(fActiveCuts);
//   if(Ncuts() > 0){
//
//   }
	Int_t binlx = XBinNumber(fSelHist, xlow);
	Int_t binux = XBinNumber(fSelHist, xup);
	if (binlx > binux) {
		Int_t temp = binux;
		binux = binlx;
		binlx = temp;
	}
	Int_t binly = YBinNumber(fSelHist, pfirst->GetY());
	Int_t binuy = YBinNumber(fSelHist, plast->GetY());
	if (binly > binuy) {
		Int_t temp = binuy;
		binuy = binly;
		binly = temp;
	}

	TAxis *xaxis_new = fSelHist->GetXaxis();
	Axis_t edgelx = xaxis_new->GetBinLowEdge(binlx);
	Axis_t edgeux = xaxis_new->GetBinLowEdge(binux) +
		 xaxis_new->GetBinWidth(binlx);
	TAxis *yaxis_new = fSelHist->GetYaxis();
	Axis_t edgely = yaxis_new->GetBinLowEdge(binly);
	Axis_t edgeuy = yaxis_new->GetBinLowEdge(binuy) +
		 yaxis_new->GetBinWidth(binly);
// recalculate bin numbers

	binlx = (Int_t) ((edgelx - xmin) / xbinwidth + 1.5);
	binux = (Int_t) ((edgeux - xmin) / xbinwidth + 0.5);
	Int_t nxbins = binux - binlx + 1;

	binly = (Int_t) ((edgely - ymin) / ybinwidth + 1.5);
	binuy = (Int_t) ((edgeuy - ymin) / ybinwidth + 0.5);
	Int_t nybins = binuy - binly + 1;
	if (nxbins < 2 || nybins < 1) {
		WarnBox("nxbins < 2 || nybins < 1");
		return -1;
	}
	double par[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double fpar[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	TEnv env(".hprrc");
	
//	const char *funcname;
	TString sfunc = fHname;
	Int_t ip = sfunc.Index(";");
	if (ip > 0)sfunc.Resize(ip);
	sfunc.Prepend("func_");
	TString funcname = env.GetValue("Fit2dim.FuncName", sfunc);
	funcname = funcname.Strip(TString::kBoth);
	funcname += "_";
	funcname += fFuncNumb;
//	func.Prepend(Form("%d_", fFuncNumb));
//	sfunc.Prepend("f");
//   Bool_t ok;
//   sfunc = GetString("Function name", (const char *) sfunc, &ok, mycanvas);
//   if (!ok)
//      return 0;
//	funcname = (const char *) sfunc;
	TString temp_hname ("fithist_temp");
	temp_hname +=fFuncNumb;
	cout << " <<<<<<<<<<<<<<<<<<<<<" << endl;

	TH1D *fithist;
	fithist = new TH1D(temp_hname, funcname, nxbins, edgelx, edgeux);
	cout << " >>>>>>>>>>>>>>>>>>>>>" <<endl;
// case fit to histogram

	if (what == 0) {
		UpdateCut();
		Stat_t low_limit = GeneralAttDialog::fContentLowLimit;
		if ( fActiveCuts && fActiveCuts->GetSize() > 0 && GeneralAttDialog::fVertAdjustLimit > 0 ) {
			if ( fActiveCuts->GetSize() > 1 )
				cout << setred << "WARNING: More than 1 actice cut" << setblack << endl;
			TCutG *cut = (TCutG*)fActiveCuts->At(0);
			Int_t np = cut->GetN();
			xmin = 1e12,  xmax = -1e12;
			Double_t x, y;
			Double_t cor_limit = GeneralAttDialog::fVertAdjustLimit *
										fithist->GetBinWidth(1);
			for ( Int_t i = 0; i < np; i++ ) {
				cut->GetPoint(i, x, y);
				if ( x < xmin ) xmin = x;
				if ( x > xmax ) xmax = x;
			}
			for ( Int_t i = 0; i < np; i++ ) {
				cut->GetPoint(i, x, y);
				if ( x != xmin  && x - xmin <  cor_limit ) {
					cut->SetPoint(i, xmin, y);
					cout << "Adjust point " << i << " from " << x << " to " << xmin << endl;
				} else if ( x != xmax &&  xmax - x <  cor_limit ) {
					cut->SetPoint(i, xmax, y);
					cout << "Adjust point " << i << " from " << x << " to " << xmax << endl;
				}
			}
			fCanvas->Modified();
			fCanvas->Update();
		}
//      Double_t cont = 0;
		double *sum = new double[nxbins];
		double *sumx = new double[nxbins];
		double *sumx2 = new double[nxbins];
		double *mean = new double[nxbins + 2];
		double *sigma = new double[nxbins + 2];
		//     is there a cut active?
		for (Int_t i = binlx; i <= binux; i++) {
			Int_t indx = i - binlx;
			sum[indx] = 0.;
			sumx[indx] = 0.;
			sumx2[indx] = 0.;
			Axis_t xcent = xaxis->GetBinCenter(i);
			for (Int_t j = binly; j <= binuy; j++) {
				Axis_t ycent = yaxis->GetBinCenter(j);
				if (Ncuts()) {
					if (!InsideCut((float) xcent, (float) ycent))
						continue;
				}
				Stat_t cont = fOrigHist->GetBinContent(i, j);
				sum[indx] += cont;
				sumx[indx] += ycent * cont;
				sumx2[indx] += ycent * ycent * cont;
			}
		}
		int nonzero = 0, nx1 = 0, nx2 = 0;
		double mean1 = 0., mean2 = 0., sigma_max = 0.;
		int nxhalf = nxbins / 2;
//    under/overflow cells
		mean[0] = mean[nxbins + 1] = sigma[0] = sigma[nxbins + 1] = 0;
		for (Int_t i = 0; i < nxbins; i++) {
//         if (sum[i] > 0) {
			if (sum[i] > low_limit) {
				mean[i + 1] = sumx[i] / sum[i];
				if (sum[i] > 1.1) {
					double s2 = sumx2[i] / sum[i] - mean[i + 1] * mean[i + 1];
					if (s2 > 0)
						sigma[i + 1] = sqrt(s2) / sqrt(sum[i]);
					else
						sigma[i + 1] = 1;
//               sigma[i]= sqrt(sumx2[i]/sum[i]-mean[i]*mean[i])/sqrt(sum[i]);
				} else
					sigma[i + 1] = 1;
				if (sigma[i + 1] > sigma_max)
					sigma_max = sigma[i + 1];
				if (i < nxhalf) {
					mean1 += mean[i + 1];
					nx1++;
				} else {
					mean2 += mean[i + 1];
					nx2++;
				}
				nonzero++;
			} else {
				mean[i + 1] = 0.;
				sigma[i + 1] = 0.;
			}
//         cout << i << " " << sum[i]<< " "<< sumx[i] <<" " << sumx2[i] << " " << mean[i] << " " << sigma[i] << endl;
		}
//     if nentries =1 replace error by max
		for (Int_t i = 0; i < nxbins; i++) {
			if (sum[i] > 0 && sum[i] > low_limit && sum[i] < 1.1)
				sigma[i + 1] = sigma_max;
		}
		fithist->SetContent((Stat_t *) mean);
		fithist->SetError((Stat_t *) sigma);

		if (nx1 > 0)
			mean1 /= nx1;
		if (nx2 > 0)
			mean2 /= nx2;
		int nx14 = binlx + nxhalf / 2;
		int nx34 = binlx + nxhalf + nxhalf / 2;
		if (nx34 <= nx14) {
			cout << "Fit2dim: wrong range, nx14, nx34"
				 << nx14 << ", " << nx34 << endl;
			return -1;
		}
		Axis_t x14 = xaxis->GetBinCenter(nx14);
		Axis_t x34 = xaxis->GetBinCenter(nx34);
		if (ndim <= 0) {
			par[0] = 0.5 * (mean2 - mean1);
		} else {
			par[1] = (mean2 - mean1) / (x34 - x14);
			par[0] = mean1 - x14 * par[1];
		}
		delete [] sum;
		delete [] sumx;
		delete [] sumx2;
		delete [] mean;
		delete [] sigma;
	} else {
		FhMarker *p;
		TIter next(fMarkers);
		double mean = 0., entries = 0.;
		xaxis = fSelHist->GetXaxis();
		xmin = xaxis->GetXmin();
		xmax = xaxis->GetXmax();
		while ( (p = (FhMarker *) next()) ) {
			cout << p->GetX() << " " << p->GetY() << endl;
//         dont take default marks at corners
			if (p->GetX() > xmin && p->GetX() < xmax) {
				Int_t nbinx = fithist->FindBin(p->GetX());
				
				fithist->SetBinContent(nbinx, p->GetY());
				if ( p->GetErrY() > 0) {
					fithist->SetBinError(nbinx, p->GetErrY());
				}
				mean += p->GetY();
				entries++;
			}
			if (entries > 0)
				mean /= entries;
		}
		if (entries > 0) {
			if (ndim >= 0 && entries < ndim + 1)
				WarnBox("Too many fit parameters");
			mean /= entries;
		} else {
			WarnBox("No marks set");
			return -1;
		}
		par[0] = mean;
	}
//   cout << "Fitting: " << funcname << " (Pol " << ndim << ") to ";
	if (what == 0)
		cout << fSelHist->GetName();
	if (what == 1)
		cout << " user supplied marks";
	cout << " from " << edgelx << " to " << edgeux << endl;
	cout <<
		 "-------------------------------------------------------------"
		 << endl;
	TString form("pol");
	TF1 *pol;
	if (ndim >= 0) {
		form += ndim;
		pol = new TF1(funcname, form.Data(), (Double_t)edgelx,  (Double_t)edgeux);
	} else {
		TEnv env(".hprrc");
		if (env.Defined("FitOneDimDialog.fFormula")) {
			form = env.GetValue("FitOneDimDialog.fFormula", "[0]+[1]*x + [2]*x*x");
			TFormula ff("NNN", form);
#if ROOTVERSION >= 60000
			if (!ff.IsValid())
#else
			if (! (ff.GetNdim() <= 0))
#endif
			{
				cout << setred << "Formula: " << form << " is not valid " << endl;
				return -2;
			}
			pol = new TF1(funcname, form.Data(), (Double_t)edgelx, (Double_t)edgeux);
			TString resname;
			for (Int_t i = 0; i < ff.GetNpar(); i++) {
				resname = "FitOneDimDialog.fFormPar";
				resname += i;
				if (env.Defined(resname)) {
					pol->SetParameter(i, env.GetValue(resname, 0.));
					cout << "FormPar" << i << " " << pol->GetParameter(i) << endl;
				}
				resname = "FitOneDimDialog.fFormFixPar";
				resname += i;
				if (env.Defined(resname)) {
					Int_t ffix =  env.GetValue(resname, 0);
					if (ffix) {
						pol->FixParameter(i,  pol->GetParameter(i));
						cout << "Fix Par" << i << endl;
					}
				}
			}
			cout << "Fitting: " << form << " from " << edgelx << " to " << edgeux<< endl;
		} else {
			cout << setred << "No \"FitOneDimDialog.fFormula\" in .hpprc provided" 
			<< setblack << endl;
			return -1;
		}
	}
//	TF1 *pol = new TF1(funcname, form.Data(), (float) edgelx, (float) edgeux);
//   TF1 *pol = new TF1(funcname, "func", (float) edgelx, (float) edgeux);
//   DisplayHist(fithist);
	pol->SetBit(kMustCleanup);
	if (ndim >= 0)
		pol->SetLineColor(ndim + 1);
	else
		pol->SetLineColor(2);
	pol->SetLineWidth(2);

	fithist->Fit(funcname, "R0");
	if (std::isnan(pol->GetChisquare()) != 0) {
		cout << "Something went wrong, FCN=Chisquare is infinite " << endl;
		return -1;
	}
	cout << "Chisquare/NDF " << pol->GetChisquare()<<"/"<<  pol->GetNDF()<< endl;
	fithist->GetFunction(funcname)->ResetBit(TF1::kNotDraw);
	TString FHname(fithist->GetName());
	FHname.Prepend("Fh_");
//	FitHist * fith = NULL;
	if (gHprDebug > 0)
		new FitHist(FHname,FHname,fithist,fithist->GetName() ,750,800, 800,400);
	if (fOrigHist != fSelHist)
		fOrigHist->GetListOfFunctions()->Add(pol);
	fSelHist->GetListOfFunctions()->Add(pol);
//   fithist->SetMinimum(edgely);
//--------------------------------------
	pol->GetParameters(fpar);

	TString scorr = "$1/(";
	for (int i = 0; i <= ndim; i++) {
		if (i > 0) {
			if (fpar[i] > 0) scorr += "+";
			 scorr += Form("%f", fpar[i]);
			 scorr += "*$2";
			if (i > 1) {
				scorr += "^";
				scorr += i;
			}
		} else {
			scorr += Form("%f", fpar[i]);
		}
	}
	scorr += "):$2";
	cout << scorr << endl;
// draw in 2 dim histogram
	fSelPad->cd();
	pol->Draw("same");
	fSelPad->Update();
//  if ( fDialog != NULL )
//     fDialog->CloseDialog();
	if (GetAutoDisplayS2FD()) {
		Save2FileDialog sfd(pol, NULL, (TRootCanvas*)fCanvas->GetCanvasImp());
		if ( sfd.Canceled() == 0 ) {
			// find out if  user changed name and seqnumber
			TString fn(sfd.GetObjName()); 
			cout << "sfd.GetObjName() " << sfd.GetObjName() << endl;
			TRegexp re("_[0-9]+$");  // e.g. _123 at end of string
			Ssiz_t len;
			Ssiz_t pos = re.Index(fn, &len);
			if (pos >= 0 ) {
				TString sval = fn(pos+1, len-1);
				fFuncNumb = sval.Atoi();
			}
			fn(re) = "";
			cout << "fn " << fn << " funcname " <<funcname  << endl;
			if ( fn != funcname ) {
				env.SetValue("Fit2dim.FuncName", fn);
				env.SaveLevel(kEnvLocal);
			}
			fFuncNumb++;
		}
	} else {
		fFuncNumb++;
		cout << setblue << "You may save the function from the popup menu" << setblack << endl;
	}
	cout << setblue << "Dont forget to clear marks if no longer needed" << setblack << endl;
//	if (what == 0)
//		ClearMarks();
//	if (what == 1)
//		ClearMarks();
	return 1;
};

//__________________________________________________________________________________

void FitHist::DrawSelectedFunctions()
{
	if (!gHpr) return;
	TIter next(gHpr->fAllFunctions);
	TObjString * tobjs;
	TFile * f;
	fCanvas->cd();
	while ( (tobjs = (TObjString *)next()) ) {
		TString ent = tobjs->GetString();
		TString filename(ent);
		Int_t isp = filename.Index(" ");
		filename.Resize(isp);
		f = new TFile(filename);
		TString funcname(ent);
		funcname.Remove(0, isp+1);
		TF1 * func = (TF1*)f->Get(funcname);
		if (func) {
			func->SetLineColor(gStyle->GetFuncColor());
			func->Draw("same");
			if (fSelHist)
				fSelHist->GetListOfFunctions()->Add(func);
//			func->Print();
		}
		f->Close();
	}
	fCanvas->Modified();
	fCanvas->Update();
}
//__________________________________________________________________________________

void FitHist::SubtractFunction()
{
	if (!gHpr) return;
	if (!fSelHist) return;
	TIter next(fSelHist->GetListOfFunctions());
	TObject * obj;
	TF1 * func = NULL;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TF1") ) {
			if (func != NULL) {
				cout << "More than 1 function selected" << endl;
				return;
			}
			func = (TF1*)obj;
		}
	}
	if (!func) {
		cout << "No function found" << endl;
		return;
	}
	fSelHist->Print();
	func->Print();
	for (Int_t i=1; i<fSelHist->GetNbinsX(); i++) {
		Double_t x = fSelHist->GetBinCenter(i);
		if ( fSelHist->GetBinContent(i) != 0)
			fSelHist->SetBinContent(i, fSelHist->GetBinContent(i) - func->Eval(x));
	}
	fCanvas->Modified();
	fCanvas->Update();
}
//____________________________________________________________________________________

void FitHist::EditFitMacro()
{
	const char hist_file[] = {"hprFitMacros.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40101) hf = NULL;
	static TString name(fFitMacroName.Data());
	Bool_t ok;
	name = GetString("Name of Fit Macro", name.Data(), &ok, mycanvas,
							0,0,0,0,0, hf);
	if (!ok)
		return;
	if (gSystem->AccessPathName(name.Data())) {
		cout << "Try to open: " << name.Data() << endl;

		ofstream tmpfile(name.Data(), ios::out);
		if (!tmpfile.good()) {
			cerr << "EditFitMacro: "
				 << gSystem->GetError() << " - " << name.Data()
				 << endl;
			return;
		}
		if (!tmpfile) {
			cout << "Cant open new " << name.Data() << endl;
			return;
		}
		if (fSelHist->GetDimension() == 1) {
			TOrdCollection *svalues = new TOrdCollection();
			for (Int_t i = 0; i < nFitTemplates; i++) {
				TString temp(FitMacroTemplates[i]);
				Int_t cstart = temp.Index("/*");
				Int_t cend = -1;
				if (cstart >= 0) cend = temp.Index("*/");
				TString mname("NoName");
				if (cstart >= 0 && cend > cstart)
					mname = temp(cstart + 2, cend - cstart - 2);

				svalues->Add(new TObjString(mname.Data()));
			}

			TArrayI flags(nFitTemplates);
			TString title("Template Fit Macros");
			Int_t retval = 0;
			Int_t itemwidth = 240;
			new TGMrbTableFrame(mycanvas, &retval,
									  title.Data(),
									  itemwidth, 1,
									  nFitTemplates,
									  svalues, 0, 0, &flags,
									  nFitTemplates);
			if (retval < 0) {
				return;
			}
			ok = kFALSE;
			for (Int_t i = 0; i < nFitTemplates; i++) {
				if (flags[i] == 1) {
					tmpfile << FitMacroTemplates[i];
					ok = kTRUE;
					break;
				}
			}
			if (!ok) {
				cout << "no selection" << endl;
				return;
			}
		} else if (fSelHist->GetDimension() == 2) {
			 tmpfile << Gauss2d;
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
	fFitMacroName = name;
}

//____________________________________________________________________________

void FitHist::ExecFitMacro()
{
//   cout << fFitMacroName.Data()<< endl;
//   static TString name(fFitMacroName.Data());
	const char hist_file[] = {"hprFitMacros.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40101) hf = NULL;
	if (fFirstUse) {
		Bool_t ok;
		fFitMacroName =
			 GetString("Use Macro:", fFitMacroName.Data(), &ok, mycanvas,
			 0,0,0,0,0, hf);
		if (!ok)
			return;
		fFirstUse = 0;
	}
	if (!gSystem->AccessPathName(fFitMacroName.Data(), kFileExists)) {
		gROOT->LoadMacro(fFitMacroName.Data());
		TString cmd = "fit_user_function";
//      Int_t p = cmd.Index(".");
//      cmd.Remove(p,2);
		cmd = cmd + "(\"" + fSelHist->GetName() + "\")";
		cout << cmd << endl;
		fCanvas->cd();
		gROOT->ProcessLine((const char *) cmd);
//      fSelPad->Modified(kTRUE);
//      fSelPad->Update();
		fCanvas->Modified(kTRUE);
		fCanvas->Update();
	} else
		WarnBox("Macro not found");
}
//____________________________________________________________________________________
//____________________________________________________________________________________

void FitHist::EditFitSliceYMacro()
{
	const char history_file[] = {"hprFitSliceYMacros.txt"};
	const char * hf = history_file;
	if (gROOT->GetVersionInt() < 40101) hf = NULL;
	static TString name(fFitSliceYMacroName.Data());
	Bool_t ok;
	name = GetString("Name of Fit Macro", name.Data(), &ok, mycanvas,
							0,0,0,0,0, hf);
	if (!ok)
		return;
	if (gSystem->AccessPathName(name.Data())) {
		cout << "Try to open: " << name.Data() << endl;

		ofstream tmpfile(name.Data(), ios::out);
		if (!tmpfile.good()) {
			cerr << "EditFitSliceYMacro: "
				 << gSystem->GetError() << " - " << name.Data()
				 << endl;
			return;
		}
		if (!tmpfile) {
			cout << "Cant open new " << name.Data() << endl;
			return;
		}
		if (fSelHist->GetDimension() == 2) {
			 tmpfile << SliceYTemplate;
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
	fFitSliceYMacroName = name;
}
//____________________________________________________________________________

void FitHist::ExecFitSliceYMacro()
{
//   cout << fFitMacroName.Data()<< endl;
//   static TString name(fFitMacroName.Data());
	const char hist_file[] = {"hprFitMacros.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40101) hf = NULL;
	if (fFirstUse) {
		Bool_t ok;
		fFitSliceYMacroName =
			 GetString("Use Macro:", fFitSliceYMacroName.Data(), &ok, mycanvas,
			 0,0,0,0,0, hf);
		if (!ok)
			return;
		fFirstUse = 0;
	}
	if (!gSystem->AccessPathName(fFitSliceYMacroName.Data(), kFileExists)) {
//      fFitSliceYMacroName += "+";
		gROOT->LoadMacro(fFitSliceYMacroName.Data());
		TString cmd = "fit_slice_function";
//      Int_t p = cmd.Index(".");
//      cmd.Remove(p,2);
	Int_t nval = GetMarks(fSelHist);
	if (nval < 2) {
		WarnBox("Not enough marks");
		return;
	}
	TString hname;
	TString sel;
	for (Int_t ipar = 0; ipar < 100; ipar++) {
		hname = fSelHist->GetName();
		hname += "_";
		hname += ipar;
		TH1 * parhist = (TH1*)gROOT->GetList()->FindObject(hname.Data());
		if (parhist) delete parhist;
		else        break;
	}
	hname = fSelHist->GetName();
	hname += "_chi2";
	TH1 * parhist = (TH1*)gROOT->GetList()->FindObject(hname.Data());
	if (parhist) delete parhist;
 //  	cout << "ExecFitSliceYMacro ----------------" << endl;
 //  	fMarkers->Print();

		FhMarker *pfirst = (FhMarker *) fMarkers->First();
		FhMarker *plast = (FhMarker *) fMarkers->Last();
		Float_t xlow = pfirst->GetX();
		Float_t xup = plast->GetX();
		Float_t ylow = pfirst->GetY();
		Float_t yup = plast->GetY();
		Int_t binXlow = fSelHist->GetXaxis()->FindBin(xlow);
		Int_t binXup  = fSelHist->GetXaxis()->FindBin(xup);

		cmd = cmd + "(\"" + fSelHist->GetName() +
						"\","
						+ Form("%f",ylow) + ","
						+ Form("%f",yup)  + ","
						+ Form("%d",binXlow)  + ","
						+ Form("%d",binXup)  + ")";
//                  "\", 0, 0, 0, 0)";
		cout << cmd << endl;
		fCanvas->cd();
		gROOT->ProcessLine((const char *) cmd);
		if (gHpr) {
			gHpr->ClearSelect();
			hname = fSelHist->GetName();
			hname += "_chi2";

			TH1 * chi2hist = (TH1*)gROOT->GetList()->FindObject(hname.Data());
			if (chi2hist){
				hname += " ";
				hname.Prepend("Memory ");
				gHpr->fSelectHist->Add(new TObjString((const char *)hname));
			}
			for (Int_t ipar = 0; ipar < 100; ipar++) {
				hname = fSelHist->GetName();
				hname += "_";
				hname += ipar;
				parhist = (TH1*)gROOT->GetList()->FindObject(hname.Data());
				if (parhist){
					hname.Prepend("Memory ");
					hname += " ";
					gHpr->fSelectHist->Add(new TObjString((const char *)hname));
					if (chi2hist) {
						for (Int_t bin = 1; bin <= chi2hist->GetNbinsX(); bin++) {
							if (chi2hist->GetBinContent(bin) > 10) {
								parhist->SetBinContent(bin, 0);
								parhist->SetBinError(bin, 0);
							}
						}
					}
				}
				else break;
			}
			gHpr->ShowSelectedHists(gHpr->fSelectHist, "Fitted values");
		}

//      fSelPad->Modified(kTRUE);
//      fSelPad->Update();
//      fCanvas->Modified(kTRUE);
//      fCanvas->Update();
	} else
		WarnBox("Macro not found");
}


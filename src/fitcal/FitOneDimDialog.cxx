// #include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TEnv.h"
#include "TFrame.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TGMsgBox.h"
#include "TMath.h"
#include "TPaveStats.h"
#include "TVirtualFitter.h"
#include "TVirtualPad.h"
#include "TGMrbTableFrame.h"
#include "FitOneDimDialog.h"
#include "Save2FileDialog.h"
#include "GraphAttDialog.h"
#include "FhPeak.h"
#include "TF1Range.h"
#include "SetColor.h"
#include "hprbase.h"
#include <iostream>
//#ifdef MARABOUVERS
//#include "FitHist.h"
//#endif

using std::cout;
using std::endl;
//__________________________________________________________________________
//
//  globals needed for non member fitting functions
//Int_t	 lNpeaks;
//Double_t lTailSide;	// if tail determines side: 1 left(low), -1 high(right)
//Float_t  lBinW;
static const Int_t kFix = 3;

//___________________________________________________________________________
//___________________________________________________________________________

Double_t gaus_only(Double_t * x, Double_t * par)
{
/*
  gaus 
  par[kFix +0]	gauss width
  par[kFix +1]	gauss0 constant
  par[kFix +2]	gauss0 mean
  ...
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];

	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()),
						 sqrt2	= TMath::Sqrt(2.);
	Double_t arg;
	Double_t fitval  = 0;
	Double_t sigma	= par[kFix +0];
	if (sigma == 0)
		sigma = 1;					//  force widths /= 0
	for (Int_t i = 0; i < lNpeaks; i ++) {
		arg = (x[0] - par[kFix +2 + 2 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +1 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
//	cout << x[0] << " "<< par[kFix +0]<< " " << par[kFix +1]<< " " << par[kFix +2]<< " " << lBinW << " "<< fitval << endl;
	return lBinW * fitval;
}
//____________________________________________________________________________

Double_t gaus_only_vsig(Double_t * x, Double_t * par)
{
/*
  gaus + linear background
  par[kFix +0]	gauss0 constant
  par[kFix +1]	gauss0 mean
  par[kFix +2]	gauss width
  par[kFix +3]	gauss1 constant
  ...
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t arg;
	Double_t fitval  = 0;
	Double_t sigma	= par[kFix +0];
	if (sigma == 0)
		sigma = 1;					//  force widths /= 0
	for (Int_t i = 0; i < lNpeaks; i ++) {
		sigma	= par[kFix +2 + 3 * i];
		if (sigma == 0) sigma = 1;					//  force widths /= 0
		arg = (x[0] - par[kFix +1 + 3 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +0 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}
//____________________________________________________________________________

Double_t gaus_cbg(Double_t * x, Double_t * par)
{
/*
  gaus + const background
  par[kFix +0]	back ground
  par[kFix +1]	gauss width
  par[kFix +2]	gauss0 constant
  par[kFix +3]	gauss0 mean
  par[kFix +4]	gauss1 constant
  ...
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);

	Double_t arg;
	Double_t fitval  = 0;
	Double_t sigma	= par[kFix +1];
	if (sigma == 0)
		sigma = 1;					//  force widths /= 0
	fitval = par[kFix +0];
	for (Int_t i = 0; i < lNpeaks; i ++) {
		arg = (x[0] - par[kFix +3 + 2 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +2 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}
//____________________________________________________________________________

Double_t gaus_cbg_vsig(Double_t * x, Double_t * par)
{
/*
  gaus + const background
  par[kFix +0]	back ground
  par[kFix +1]	gauss0 constant
  par[kFix +2]	gauss0 mean
  par[kFix +3]	gauss width
  par[kFix +4]	gauss1 constant
  ...
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];

  static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t arg;
	Double_t fitval;
	Double_t sigma ;
	fitval = par[kFix +0];
	for (Int_t i = 0; i < lNpeaks; i ++) {
		sigma	= par[kFix +3 + 3 * i];
		if (sigma == 0) sigma = 1;					//  force widths /= 0
		arg = (x[0] - par[kFix +2 + 3 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +1 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}
//____________________________________________________________________________

Double_t gaus_lbg(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]	background constant
  par[kFix +1]	background slope
  par[kFix +2]	gauss width
  par[kFix +3]	gauss0 constant
  par[kFix +4]	gauss0 mean
  par[kFix +5]	gauss1 constant
  par[kFix +6]	gauss1 mean
  par[kFix +7]	gauss2 constant
  par[kFix +8]	gauss2 mean
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];

	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t arg;
	Double_t fitval  = 0;
	Double_t bgconst = par[kFix +0];
	Double_t bgslope = par[kFix +1];
	Double_t sigma	= par[kFix +2];
	if (sigma == 0) sigma = 1;					//  force widths /= 0
	fitval = fitval + bgconst + x[0] * bgslope;
	for (Int_t i = 0; i < lNpeaks; i ++) {
		arg = (x[0] - par[kFix +4 + 2 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}
//____________________________________________________________________________

Double_t gaus_lbg_vsig(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]	background constant
  par[kFix +1]	background slope
  par[kFix +2]	gauss0 constant
  par[kFix +3]	gauss0 mean
  par[kFix +4]	gauss0 width
  par[kFix +5]	gauss1 constant
  par[kFix +6]	gauss1 mean
  .....
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t arg;
	Double_t fitval  = 0;
	Double_t bgconst = par[kFix +0];
	Double_t bgslope = par[kFix +1];
	Double_t sigma;
	fitval = fitval + bgconst + x[0] * bgslope;
	for (Int_t i = 0; i < lNpeaks; i ++) {
		sigma = par[kFix +4 + 3 * i];
		if (sigma == 0) sigma = 1;					//  force widths /= 0
		arg = (x[0] - par[kFix +3 + 3 * i]) / (sqrt2 * sigma);
		fitval = fitval + par[kFix +2 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}

//_____________________________________________________________________________

Double_t gaus_tail(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]	tail fraction
  par[kFix +1]	tail width
  par[kFix +2]	background constant
  par[kFix +3]	background slope
  par[kFix +4]	gauss width
  par[kFix +5]	gauss0 constant
  par[kFix +6]	gauss0 mean
  par[kFix +7]	gauss1 constant
  par[kFix +8]	gauss1 mean
  par[kFix +9]	gauss2 constant
  par[kFix +10]  gauss2 mean
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
	Double_t lTailSide = par[2];

	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	Double_t xij;
	Double_t arg;
	Double_t tail;
	Double_t g2b;
	Double_t err;
	Double_t norm;
	Double_t fitval = 0;
	Double_t tailfrac  = par[kFix +0];
	Double_t tailwidth = par[kFix +1];
	Double_t bgconst	= par[kFix +2];
	Double_t bgslope	= par[kFix +3];
	Double_t sigma	  = par[kFix +4];

//  force widths /= 0
	if (tailwidth == 0)
		tailwidth = 1;
	if (sigma == 0)
		sigma = 1;

	fitval = fitval + bgconst + x[0] * bgslope;
	for (Int_t i = 0; i < lNpeaks; i++) {
		xij = (x[0] - par[kFix +6 + 2 * i]);
		arg = xij / (sqrt2 * sigma);
		xij *= lTailSide;
		tail = exp(xij / tailwidth) / tailwidth;
		g2b = 0.5 * sigma / tailwidth;
		err = 0.5 * tailfrac * par[kFix +5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
		norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
		fitval = fitval + norm * err * tail
							 + par[kFix +5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
	}
	return lBinW * fitval;
}
//_____________________________________________________________________________

Double_t tail_only(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]	background constant
  par[kFix +1]	background slope
  par[kFix +2]	gauss width
  par[kFix +3]	tail width
  par[kFix +4]	constant0
  par[kFix +5]	mean0
  par[kFix +6]	constant1
  par[kFix +7]	mean1
  par[kFix +8]	constant2
  par[kFix +9]	mean2
  .....
*/
	Double_t lBinW = par[0];
	Int_t lNpeaks = (Int_t)par[1];
	Double_t lTailSide = par[2];

//	static Double_t sqrt2 = TMath::Sqrt(2.);
	Double_t xij;
//	Double_t arg;
	Double_t tail;
	Double_t g2b;
	Double_t err;
	Double_t norm;
	Double_t fitval = 0;
	Double_t bgconst	= par[kFix +0];
	Double_t bgslope	= par[kFix +1];
	Double_t sigma	  = par[kFix +2];
	Double_t tailwidth = par[kFix +3];

//  force widths /= 0
	if (tailwidth == 0)
		tailwidth = 1;
	if (sigma == 0)
		sigma = 1;

	fitval = fitval + bgconst + x[0] * bgslope;
	for (Int_t i = 0; i < lNpeaks; i++) {
		xij = (x[0] - par[kFix + 5 + 2 * i]);
 //	  arg = xij / (sqrt2 * sigma);
		xij *= lTailSide;
		tail = exp(xij / tailwidth) / tailwidth;
		g2b = 0.5 * sigma / tailwidth;
		err = 0.5 * par[kFix + 4 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
		norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
		fitval = fitval + norm * err * tail;
	}
	return lBinW * fitval;
}


//____________________________________________________________________________________

Double_t tailf(Double_t * x, Double_t * par)
{
//  force widths /= 0
	Double_t lBinW = par[0];
//	Int_t lNpeaks = (Int_t)par[1];
	Double_t lTailSide = par[2];

	Double_t const0	 = par[kFix +0];
	Double_t mean		= par[kFix +1];
	Double_t gauswidth = par[kFix +2];
	Double_t tailwidth = par[kFix +3];

	if (gauswidth == 0)
		gauswidth = 1;
	if (tailwidth == 0)
		tailwidth = 1;

	Double_t xij = (x[0] - mean) *lTailSide ;

	Double_t tail = exp(xij / tailwidth) / tailwidth;
	Double_t g2b = 0.5 * gauswidth / tailwidth;
	Double_t err = 0.5 * const0 * TMath::Erfc(xij / gauswidth + g2b);
	Double_t norm = exp(0.25 * pow(gauswidth, 2) / pow(tailwidth, 2));
	Double_t fitval = norm * err * tail;
	return lBinW * fitval;
}

//____________________________________________________________________________________

Double_t backf(Double_t * x, Double_t * par)
{
	Double_t lBinW = par[0];
//	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];
	Double_t fitval = par[kFix +0] + x[0] * par[kFix +1];
	return lBinW * fitval;
}

//____________________________________________________________________________________

Double_t gausf(Double_t * x, Double_t * par)
{
	Double_t lBinW = par[0];
//	Int_t lNpeaks = (Int_t)par[1];
//	Double_t lTailSide = par[2];
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
	if (par[kFix +2] == 0)
		par[kFix +2] = 1;

	Double_t xij = x[0] - par[kFix +1];

	Double_t arggaus = xij / (sqrt2 * par[kFix +2]);
	Double_t fitval = par[kFix +0] * exp(-arggaus * arggaus) / (sqrt2pi * par[kFix +2]);
	return lBinW * fitval;
}
//____________________________________________________________________________________
FitOneDimDialog::FitOneDimDialog()
{
	fInteractive = 0;
	fSelHist = NULL;
	fSelPad = NULL;
	fDialog = NULL;
}
//____________________________________________________________________________________

FitOneDimDialog::FitOneDimDialog(TH1 * hist, Int_t type, Int_t interactive)
{
	fInteractive = interactive;
	fSelPad = NULL;
	fFitPeakListDone = kFALSE;
	fSelHist = hist;
	if (fSelHist == NULL ) {
		TObject * obj = gPad->GetCanvas()->GetClickSelected();
		if (obj && obj->InheritsFrom("TH1") ) {
			fSelHist = (TH1*)obj;
			fSelPad = gPad->GetCanvas();
		}
	}
	fGraph = NULL;
	if (!fSelHist) {
		cout << "No hist selected, please click on one first" << endl;
		return;
	}

	if (fSelHist && fSelHist->GetDimension() != 1) {
		cout << "Can only be used with 1-dim hist" << endl;
		return;
	}
 //  lBinW	= fSelHist->GetBinWidth(1);
	if (fSelHist) fName = fSelHist->GetName();
	DisplayMenu(type);
}
//____________________________________________________________________________________

FitOneDimDialog::FitOneDimDialog(TGraph * graph, Int_t type, Int_t interactive)
{
	fSelPad = NULL;
	fInteractive = interactive;
	if (!graph) {
		cout << "No graph selected" << endl;
		return;
	}
	if (graph == NULL ) {
		TObject * obj = gPad->GetCanvas()->GetClickSelected();
		if (obj && obj->InheritsFrom("TGraph") ) {
			graph = (TGraph*)obj;
			fSelPad = gPad->GetCanvas();
		}
	}
	if (!graph) {
		cout << "No graph selected, please click on one first" << endl;
		return;
	}
	fGraph = graph;
	fSelHist = graph->GetHistogram();
	if (!fSelHist) {
		cout << "Graph must be drawn" << endl;
		return;
	}

	if (fSelHist->GetDimension() != 1) {
		cout << "Can only be used with 1-dim hist" << endl;
		return;
	}
//	lBinW	= 1;
	fName = fGraph->GetName();
	DisplayMenu(type);
}
//_________________________________________________________________________
//
// This widget allows fitting of any number of gaussian shaped peaks
// optionally with a low or high energy tail plus a linear background.
//
//_________________________________________________________________________
//____________________________________________________________________________________

void FitOneDimDialog::DisplayMenu(Int_t type)
{
static const Char_t helptext_gaus[] =
"This widget allows fitting of any number of\n\
gaussian shaped peaks optionally with a low or high\n\
energy tail plus a linear background.\n\
With option \"Tailf Only\" a tail function without a pure\n\
gaussian part is used. This is advised if the fitted\n\
tail fraction becomes large (>5).\n\
The linear background may be forced to zero (Force BG = 0)\n\
or to be constant(Force BG slope = 0)\n\
\n\
The constants determining the contents of the fitted function\n\
are adjusted such that the integral corresponds to the\n\
sum of the bin content for a given region.\n\
This means that the constant in a linear background\n\
is the average height of the bincontent in the region\n\
divided by the binwidth. This must also be accounted for\n\
when using a fixed background.\n\
\n\
The gauss width may be forced to the same for all peaks\n\
In case with tail the parameter Ta_fract (tail\n\
fraction) is the contents of the tail part relative\n\
to the pure gaussian part. In this case the total content\n\
of the fitted function is \"GaCont * (1 + Ta_fract)\"\n\
If more than one peak is fitted this fraction, the width \n\
of the tail and of the gaussian is common for all peaks.\n\
\n\
Marks (use middle mouse button to define them) are used\n\
to provide lower and upper limits of the fit region and\n\
the positions of peaks to be fitted. So if 3 peaks should be\n\
fitted 5 marks need to be defined \n\
\n\
The program estimates from this the start\n\
parameters of the fit which are presented and may be\n\
changed. Values may be fixed and bound. When using\n\
bounds please be aware of the warnings in the MINUIT\n\
manual on this. Fitted parameters may be kept as\n\
start for a new fit.\n\
This feature can also be used to preserve e.g. the shape\n\
of a tail function in subsequent fits in different regions\n\
by fixing Gauss_Width, Tail_Fraction and Tail_Width\n\
\n\
A linear background determined from 2 regions in\n\
the spectrum outside the peaks can be incorporated\n\
in the following way:\n\
\n\
Define 4 marks to select the 2 regions. Then use the\n\
command \"Determine linear background\" from the Fit menu\n\
to calculate the parameters const and slope of the \n\
linear background.In a subsequent fit \n\
the parameters Bg_Const and Bg_Slope will be fixed \n\
to the above calculated values.  If needed these \n\
values can still be changed manually. Unfixing them \n\
will let MINUIT vary their values.\n\
If the option \"Bg Slope=0\" is selected only 2 marks\n\
are required, the Bg const is the average in the region.\n\
\n\
The quality of the start parameters of a fit determine\n\
the speed of convergence. Bad start parameters may lead\n\
to unresonable results. Start parameters may be checked\n\
by drawing the fit function with these parameters without \n\
actually doing a fit.\n\
\n\
Note: If you want to use the fitting values in a\n\
		subsequent calibration step the option:\n\
		\"Add all functions to histogram\" must be active\n\
\n\
The command \"FitPeakList\" can be used to fit gaussians\n\
(optionally with tails) to many peaks automatically\n\
The estimated positions of the peaks must previously\n\
be defined with \"FindPeakDialog\"\n\
\"FindPeakDialog\" allows to write to and read a peaklist\n\
from an ASCII- file containing 1 line / peak:\n\
Mean Width Content MeanError Chi2oNdf\n\
Only Mean is obligatory the others are optional\n\
\"Half Window\" determines the width of the fit window\n\
The value \"fPeakSep\" (\"Peak Separation\")determines\n\
when close peaks should be treated by a common fit.\n\
Note: These two values are in units of \"Sigma\" used\n\
as used in the \"FindPeak\" operation.\n\
This procedure may be exercised manually setting\n\
\"fConfirmStartValues\" on\n\
Normally the fitted functions are added to the \n\
histogram for a following calibration procedure\n\
\"CalibrationDialog\"\n\
";

static const Char_t helptext_exp[] =
"Fit exponential with offset in y:\n\
f(x) = a + b * exp (x * c)\n\
Start parameters may can be calculated (estimated)\n\
For this however \"a\" (the y-offset) must be preset\n\
to a reasonable value\n\
Function limits may be given by modifying fFrom\n\
and fTo or by setting 2 marks.\n\
Fitting of any combination of functions is provided\n\
in the \"Fit User formula\" popup menu.\n\
";
static const Char_t helptext_landau[] =
"Landau function\n\
The Landau function has 3 parameters:\n\
Normalization, Location and Scale\n\
";

static const Char_t helptext_pol[] =
"Fit polynomial f(x) = a0 + a1*x +a2*x*x ..)\n\
Degree must be >= 0, any degree is allowed\n\
but may be not meaningful.\n\
Only a0, a1, a2, a3 may be given start values\n\
higher coefficients get start values = 0\n\
Function limits may be given by modifying fFrom\n\
and fTo or by setting 2 marks\n\
Fitting of any combination of functions is provided\n\
in the \"Fit User formula\" popup menu;.\n\
\n\
To exclude regions of the histogram from the fit the\n\
following method can be used:\n\
- Set exactly 2 marks with the middle mouse.\n\
- Use Cuts/Windows Popup Menu: \"Clear Region\"\n\
- Clear marks and repeat this for the required regions\n\
- Execute Fit and store fitted function in a file\n\
- Redisplay histogram, open file with stored function\n\
- Select function and use \"Draw selected functions\" \n\
  from Draw/Fit/Calib  Popup Menu\n\
- You might use \"Subtract function\" from the View Menu\n\
  to show residuals to the fitted function.\n\
";

static const Char_t helptext_form[] =
"Fit a user defined function:\n\
e.g.: [0]*sin([1]*(x+[2])\n\
Note: The number of parameters is defined\n\
implicitly by the formula. Maximimum 6 parameters are\n\
allowed, if more parameters are needed use a fit macro.\n\
Function limits may be given by modifying fFrom\n\
and fTo explicitely or by setting 2 marks\n\
Examples:\n\
Error function with y offset [0], constant [1]\n\
x offset [2] and width[3]:\n\
[0] + [1]*TMath::Erf((x-[2]) / [3])\n\
\n\
Sum of sine functions producing a beat:\n\
[0] + [1]*sin([2]*x+ [3]) + [2] * sin([4]*x + [5])\n\
\n\
A BreitWigner with constant [0], mean [1] and gamma [2]\n\
[0] * TMath::BreitWigner(x,[1], [2])\n\
\n\
For a complete list of implemented functions consult\n\
the root doc at: http://root.cern.ch\n\
Nota bene:\n\
If expression contains a power function with non integer\n\
exponent, x must not be negative (fFrom >= 0)\n\
e.g. [0]*TMath::Power(x, 2.3)\n\
";

	fParentWindow = NULL;

	fFuncNumber = 0;
	fLinBgConst = 0;
	fLinBgSlope = 0;
	fUsedbg	  = 0;
//	fGaussOnly	= 0;
//	fTailOnly	= 0;
//	fGaussTail	= 1;
	fUseoldpars = 0;
	fConstant	= 0;
	fMean		 = 0;
	fMeanError  = 0;
	fAdded		= -1;
	RestoreDefaults();
//	fSelPad = NULL;
	SetBit(kMustCleanup);
	if ( fSelHist ) {
//		fFuncName = fSelHist->GetName();
//		fFuncName += "_";
		TAxis *xaxis = fSelHist->GetXaxis();
		fFrom = xaxis->GetBinLowEdge(xaxis->GetFirst());
		fTo	= xaxis->GetBinUpEdge(xaxis->GetLast());
	} else {
		fFrom = 0;
		fTo	= 100;
	}
//	fFuncName += fFuncNumber;
//	fFuncNumber++;
	fNpeaks = 1;
	fNpeaksList = 0;
	fUseoldpars = 0;
	fLinBgSet = kFALSE;
	fMarkers = NULL;
	fCalFunc = NULL;
	fFitFunc = NULL;
	fReqNmarks= 0;
	fDialog = NULL;

	TIter next(gROOT->GetListOfCanvases());
	TCanvas *c;
	if ( fSelPad == NULL ) {
		while ( (c = (TCanvas*)next()) ) {
			if (c->GetListOfPrimitives()->FindObject(fName)) {
				fSelPad = c;
				fSelPad->cd();
	//			cout << "fSelPad " << fSelPad << endl;
				break;
			}
		}
	}
	if (fSelPad == NULL) {
	  cout << "fSelPad = 0!!" <<  endl;
	} else {
		fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
	}
	if (fInteractive > 0 ) {
		TString title;
		TList *row_lab = new TList();
		static void *valp[50];
		Int_t ind = 0;
		static TString explcmd("FitPeakList()");
		static TString exgcmd("FitGausExecute()");
		static TString expcmd("FitExpExecute()");
		static TString exlandcmd("FitLandauExecute()");
		static TString drlandcmd("DrawLandauExecute()");
		static TString exdcmd("DrawExpExecute()");
		static TString exstpcmd("CalcStartParExp()");
		static TString expolcmd("FitPolExecute()");
		static TString exdpolcmd("DrawPolExecute()");
		static TString exformcmd("FitFormExecute()");
		static TString exdformcmd("DrawFormExecute()");
		static TString exgformcmd("GraphFormExecute()");
	//	static TString accmd("AddToCalibration()");
		static TString lbgcmd("DetLinearBackground()");
		static TString clmcmd("ClearMarkers()");
		static TString clfcmd("ClearFunctionList()");
		static TString wrfcmd("WriteoutFunction()");
		static TString setmcmd("SetMarkers()");
		static TString prtcmd("PrintMarkers()");
		static TString sfocmd("SetFittingOptions()");
		static TString fhrcmd("FillHistRandom()");
		static TString svcmd("SaveFunction()");
		static TString gfcmd("GetFunction()");
		static TString gsfcmd("GetSelectedFunction()");
		static Int_t dummy = 0;
		TString * text = NULL;
		const char * history = NULL;
		const char hist_file[] = {"func_formulae_hist.txt"};
	//	history = hist_file;
	//	if (gROOT->GetVersionInt() < 40000) history = NULL;

	//	fSelHist->Dump();
		const char * helptext = NULL;
		TString tagname;
		title = fSelHist->GetName();
		if (type == 1) {
			title.Prepend("Fit Gauss: ");
			helptext = helptext_gaus;
			fFuncName = fGausFuncName;
			fFuncType = "fGausFuncName";
			fNmarks = GetMarkers();
	//		row_lab->Add(new TObjString("PlainIntVal_N Peaks"));
	//		valp[ind++] = &fNpeaks;
			row_lab->Add(new TObjString("RadioButton_Gauss Only"));
			valp[ind++] = &fGaussOnly;
			row_lab->Add(new TObjString("RadioButton+Gauss+Tail"));
			valp[ind++] = &fGaussTail;
			row_lab->Add(new TObjString("RadioButton+TailF Only"));
			valp[ind++] = &fTailOnly;
			row_lab->Add(new TObjString("CheckButton_  Low Tail"));
			valp[ind++] = &fLowtail;
			row_lab->Add(new TObjString("CheckButton+ High Tail"));
			valp[ind++] = &fHightail;
			row_lab->Add(new TObjString("CheckButton+Same GaWidth"));
			valp[ind++] = &fOnesig;
			
			row_lab->Add(new TObjString("CheckButton_Bg Const=0"));
			valp[ind++] = &fBackg0;
			row_lab->Add(new TObjString("CheckButton+Bg Slope=0"));
			valp[ind++] = &fSlope0;
			row_lab->Add(new TObjString("CheckButton+Pre detmd Bg"));
			valp[ind++] = &fUsedbg;
//			row_lab->Add(new TObjString("CheckButton+Pre det Bg"));
//			valp[ind++] = &fUsedbg;
			row_lab->Add(new TObjString("CheckButton_Use pars of prev fit"));
			valp[ind++] = &fUseoldpars;
			row_lab->Add(new TObjString("CheckButton+Confirm start values"));
			valp[ind++] = &fConfirmStartValues;
		} else if (type == 2) {
			title.Prepend("Exp: ");
			helptext = helptext_exp;
			fFuncName = fExpFuncName;
			fFuncType = "fExpFuncName";
			row_lab->Add(new TObjString("CommentOnly_Function: a + b*exp(c*(x-d))"));
//			row_lab->Add(new TObjString("CommentOnly_Function: a + b*exp(c*x)"));
			valp[ind++] = &dummy;
			row_lab->Add(new TObjString("DoubleV+CbF_a"));
			valp[ind++] = &fExpA;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fExpFixA;
			row_lab->Add(new TObjString("DoubleV+CbF+b"));
			valp[ind++] = &fExpB;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fExpFixB;
			row_lab->Add(new TObjString("DoubleV+CbF_c"));
			valp[ind++] = &fExpC;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fExpFixC;
			row_lab->Add(new TObjString("DoubleV+CbF+d"));
			valp[ind++] = &fExpO;
//			row_lab->Add(new TObjString("CheckButton-Fix"));
			valp[ind++] = &fExpFixO;
//			row_lab->Add(new TObjString("CommentOnly+ -----"));
//			valp[ind++] = &dummy;
		} else if (type == 5) {
			title.Prepend("Landau: ");
			helptext = helptext_landau;
			fFuncName.Prepend(fLandauFuncName);
			row_lab->Add(new TObjString("CommentOnly_Landau Function"));
			valp[ind++] = &dummy;
			row_lab->Add(new TObjString("DoubleV+CbF_Norm"));
			valp[ind++] = &fLandauA;
			valp[ind++] = &fLandauFixA;
			row_lab->Add(new TObjString("DoubleV+CbF+Loca"));
			valp[ind++] = &fLandauB;
			valp[ind++] = &fLandauFixB;
			row_lab->Add(new TObjString("DoubleV+CbF_Scal"));
			valp[ind++] = &fLandauC;
			valp[ind++] = &fLandauFixC;
			row_lab->Add(new TObjString("CommentOnly+  "));
			valp[ind++] = &dummy;

		} else if (type == 3) {
			title.Prepend("Poly: ");
			helptext = helptext_pol;
			fFuncName = fPolFuncName;
			fFuncType = "fPolFuncName";
			row_lab->Add(new TObjString("CommentOnly_Pol: a0 + a1*x +.."));
			valp[ind++] = &dummy;
			row_lab->Add(new TObjString("PlainIntVal+Degree"));
			valp[ind++] = &fPolN;

			for ( Int_t i = 0; i < 6; i ++ ) {
				tagname = "DoubleV+CbF";
				if ( i%2 == 0 ) tagname += "_a";
				else		 tagname += "+a";
				tagname += i;
				row_lab->Add(new TObjString(tagname));
				valp[ind++] = &fPolPar[i];
//				tagname = "CheckButton";
//			  tagname += "+Fix";
//			 	row_lab->Add(new TObjString(tagname));
				valp[ind++] = &fPolFixPar[i];
			}
		} else if (type == 4) {
			title.Prepend("User def form: ");
			helptext = helptext_form;
			fFuncName = fFormFuncName;
			fFuncType = "fFormFuncName";
			row_lab->Add(new TObjString("CommentOnly_User defined formula"));
			valp[ind++] = &dummy;
			for ( Int_t i = 0; i < 6; i ++ ) {
				tagname = "DoubleV+CbF";
				if ( i%2 == 0 ) tagname += "_a";
				else			  tagname += "+a";
				tagname += i;
				row_lab->Add(new TObjString(tagname));
				valp[ind++] = &fFormPar[i];
//				tagname = "CheckButton";
//				tagname += "-Fix";
//			 	row_lab->Add(new TObjString(tagname));
				valp[ind++] = &fFormFixPar[i];
			}
		}
		row_lab->Add(new TObjString("DoubleValue_From"));
		valp[ind++] = &fFrom;
		row_lab->Add(new TObjString("DoubleValue+To"));
		valp[ind++] = &fTo;
		row_lab->Add(new TObjString("CheckButton_   Add funcs to hist"));
		valp[ind++] = &fFitOptAddAll;
		row_lab->Add(new TObjString("CheckButton+   Auto clear marks"));
		valp[ind++] = &fAutoClearMarks;
		row_lab->Add(new TObjString("ColorSelect_LineCol"));
		valp[ind++] = &fColor;
		row_lab->Add(new TObjString("PlainShtVal+LWid"));
		valp[ind++] = &fWidth;
		row_lab->Add(new TObjString("LineSSelect+LSty"));
		valp[ind++] = &fStyle;
		row_lab->Add(new TObjString("StringValue_FuncName"));
		valp[ind++] = &fFuncName;
		if (type == 4) {
			row_lab->Add(new TObjString("PlainIntVal+N Segs Graph"));
			valp[ind++] = &fNsegsGraph;
		}
		row_lab->Add(new TObjString("CommandButt_Exe Fit/Draw"));
		if (type == 1) { 
			valp[ind++] = &exgcmd;
			row_lab->Add(new TObjString("CommandButt+Det Lin BG"));
			valp[ind++] = &lbgcmd;
			row_lab->Add(new TObjString("CommandButt+FitPeakList"));
			valp[ind++] = &explcmd;
		} else if (type == 2) {
			valp[ind++] = &expcmd;
			row_lab->Add(new TObjString("CommandButt+Draw only"));
			valp[ind++] = &exdcmd;
			row_lab->Add(new TObjString("CommandButt+Calc Start Pars"));
			valp[ind++] = &exstpcmd;
		} else if (type == 5) {
			valp[ind++] = &exlandcmd;
			row_lab->Add(new TObjString("CommandButt+Draw only"));
			valp[ind++] = &drlandcmd;
		} else if (type == 3) {
			valp[ind++] = &expolcmd;
			row_lab->Add(new TObjString("CommandButt+Draw only"));
			valp[ind++] = &exdpolcmd;
		} else if (type == 4) {
			valp[ind++] = &exformcmd;
			row_lab->Add(new TObjString("CommandButt+Draw only"));
			valp[ind++] = &exdformcmd;
			row_lab->Add(new TObjString("CommandButt+Draw as Graph"));
			valp[ind++] = &exgformcmd;
			history = hist_file;
			text = &fFormula;
		}
		row_lab->Add(new TObjString("CommandButt_Opts/Params"));
		valp[ind++] = &sfocmd;
		row_lab->Add(new TObjString("CommandButt+Clear FuncList"));
		valp[ind++] = &clfcmd;
		row_lab->Add(new TObjString("CommandButt+Writeout Func"));
		valp[ind++] = &wrfcmd;
		row_lab->Add(new TObjString("CommandButt_Print Marks"));
		valp[ind++] = &prtcmd;
		row_lab->Add(new TObjString("CommandButt+Clear Marks"));
		valp[ind++] = &clmcmd;
		if (type == 1) {
			row_lab->Add(new TObjString("CommandButt-Set N Marks"));
			valp[ind++] = &setmcmd;
			row_lab->Add(new TObjString("PlainIntVal-N"));
			valp[ind++] = &fReqNmarks;
		} else {
			row_lab->Add(new TObjString("CommandButt+Set 2 Marks"));
			fReqNmarks = 2;
			valp[ind++] = &setmcmd;
		}
		if (type == 4) {
			row_lab->Add(new TObjString("CommandButt_Func to File"));
			valp[ind++] = &svcmd;
			row_lab->Add(new TObjString("CommandButt+Func from File"));
			valp[ind++] = &gfcmd;
			row_lab->Add(new TObjString("CommandButt+Get Sel Func"));
			valp[ind++] = &gsfcmd;
			
			row_lab->Add(new TObjString("CommandButt_Fill random"));
			valp[ind++] = &fhrcmd;
			row_lab->Add(new TObjString("PlainIntVal+N events"));
			valp[ind++] = &fNevents;
		}
		Int_t ok = 0;
		Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
		fDialog =
		new TGMrbValuesAndText (title.Data(), text, &ok, itemwidth,
								fParentWindow, history, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
		gROOT->GetListOfCleanups()->Add(this);
	}
}
//__________________________________________________________________________

FitOneDimDialog::~FitOneDimDialog()
{
// cout << "dtor FitOneDimDialog: " << this << endl;
	if (fInteractive > 0)
		gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void FitOneDimDialog::RecursiveRemove(TObject * obj)
{
	if (fInteractive = 0) return;
//	cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "
//		  << obj << " fSelHist " <<  fSelHist <<  " fSelPad " <<  fSelPad << endl;
	if ((fSelPad && obj == fSelPad) || obj == fSelHist) {
//		 cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "
//		  << obj << " fSelHist " <<  fSelHist <<  endl;
		if(fDialog) CloseDialog();
	}
}
//__________________________________________________________________________

void FitOneDimDialog::ClearFunctionList()
{
	Check4Reselect();
	TList temp;
	gStyle->SetOptFit(0);
	TPaveStats * stats = (TPaveStats*)fSelHist->GetListOfFunctions()->FindObject("stats");
	if ( stats )
		delete stats;
	TList *lof = fSelHist->GetListOfFunctions();
	TIter next(lof);
	TObject *obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TF1"))
			temp.Add(obj);
	}
	TIter next1(&temp);
	while ( (obj = next1()) ) {
		lof->Remove(obj);
//		delete obj;
	}
	temp.Delete();

	if (gPad) {
		TList temp1;
		TList *lof1 = gPad->GetListOfPrimitives();
		TIter next2(lof1);
		while ( (obj = next2()) ) {
			if (obj->InheritsFrom("TF1"))
				temp1.Add(obj);
		}
		TIter next3(&temp);
		while ( (obj = next3()) ) {
			lof1->Remove(obj);
	//		delete obj;
		}
		temp1.Delete();
		gPad->Modified();
		gPad->Update();
	}
	fFitFunc = NULL;
}
//__________________________________________________________________________

Bool_t FitOneDimDialog::FitPeakList()
{
	Int_t confirm = fConfirmStartValues;
	TList * p = (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
	if (!p || p->GetSize() == 0 ) {
		cout << "No peaks found" << endl;
		return kFALSE;
	}
	
	if ( fDialog ) fDialog->DisableCancelButton();
	Int_t addall_save = fFitOptAddAll;
	Int_t showcof_save = fShowcof;
	fFitOptAddAll = 1;
	fShowcof = 0;
	p->Sort();
	p->Print();
	fNpeaksList = p->GetSize();
	fNpeaksListIndex = 0;

	fConstantList.Set(fNpeaksList);
	fMeanList.Set(fNpeaksList);
	fSigmaList.Set(fNpeaksList);
	fChi2List.Set(fNpeaksList);
	Int_t i = 0;
	Bool_t started = kFALSE;
	Double_t xmin = fSelHist->GetBinCenter(1);
	Double_t xmax = fSelHist->GetBinCenter(fSelHist->GetNbinsX());
	Double_t xp = 0, xpn = 0;
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL) {
		fMarkers = new  FhMarkerList();
		fSelHist->GetListOfFunctions()->Add(fMarkers);
	}
	ClearMarkers();
	FhPeak *peak;
// expand picture to make all peaks visible
	peak = (FhPeak*)p->At(0);
	xp = peak->GetMean();
	Double_t low = TMath::Max(xmin, xp - fFitWindow * peak->GetWidth());
	peak = (FhPeak*)p->At(fNpeaksList - 1);
	xp = peak->GetMean();
	Double_t upl = TMath::Min(xmax, xp + fFitWindow * peak->GetWidth());
	Int_t lbin = fSelHist->FindBin(low);
	Int_t ubin = fSelHist->FindBin(upl);
	TAxis *xa = fSelHist->GetXaxis();
	Int_t lbin_save = -1, ubin_save = -1;
	if (  lbin < xa->GetFirst()
		|| ubin > xa->GetLast() ) {
		lbin_save = xa->GetFirst();
		ubin_save = xa->GetLast();
		lbin = TMath::Min(lbin, xa->GetFirst());
		ubin = TMath::Max(ubin, xa->GetLast());
		xa->SetRange(lbin, ubin);
	}
		
	Bool_t close_peak = kFALSE;
	while (i < fNpeaksList) {
		peak = (FhPeak*)p->At(i);
		xp = peak->GetMean();
		fMarkers->Add(new FhMarker(xp));
		Float_t mark;
		if (fPrintStartValues)
			cout << "Enter Peak at: " << xp << endl;
		if (!started) {
			mark = TMath::Max(xmin, xp - fFitWindow * peak->GetWidth());
			started = kTRUE;
			fMarkers->Add(new FhMarker(mark));
//			fMarkers->Add(new FhMarker(xp));
			if (fPrintStartValues) {
			  cout << "Start Mark at: " << mark << endl;
			}
	  }
//  last peak
	  if (i == fNpeaksList - 1) {
		  mark = TMath::Min(xmax, xp + fFitWindow * peak->GetWidth());
		  fMarkers->Add(new FhMarker(mark));
		  if (fPrintStartValues)
			  cout << "Exit Mark at: " << mark << endl;
		  fMarkers->Sort();
		  if (!FitGausExecute()) return kFALSE;
		  ClearMarkers();
		  break;
	  }
	  FhPeak *peakn = (FhPeak*)p->At(i+1);
	  xpn = peakn->GetMean();
	  if (xpn - xp < fPeakSep *  peak->GetWidth()) {
//		  fMarkers->Add(new FhMarker(xp));
		  close_peak = kTRUE;
		  if (fPrintStartValues)
			  cout << "Close Peak at: " << xp << endl;
		  i++;
	  } else {
			if (close_peak) {
//				fMarkers->Add(new FhMarker(xp));
				close_peak = kFALSE;
			}
			mark = TMath::Min(xmax, xp + fFitWindow * peak->GetWidth());
			fMarkers->Add(new FhMarker(mark));
			i++;
			started = kFALSE;
			if (fPrintStartValues)
				cout << "End Mark at: " << mark << endl;
			fMarkers->Sort();
			FitGausExecute();
			ClearMarkers();
		}
	}
	if ( fFitOptOneLine ) {
		PrintPeakList();
	}
	if (lbin_save >= 0) {
		cout << "xa->SetRange(lbin_save, ubin_save) " <<lbin_save << " " <<ubin_save << endl;
		xa->SetRange(lbin_save, ubin_save);
		gPad->Modified();
		gPad->Update();
	}
	fFitOptAddAll = addall_save;
	fShowcof = showcof_save;
	fConfirmStartValues = confirm;
	fFitPeakListDone = kTRUE;
	if (fDialog) fDialog->EnableCancelButton();
	return kTRUE;
}
//__________________________________________________________________________

void FitOneDimDialog::PrintPeakList()
{
	Double_t median;
	Int_t np = fConstantList.GetSize();
	median = TMath::Median(np, fConstantList.GetArray());
	Double_t minc = 1, maxc = median *100;
	median = TMath::Median(np, fSigmaList.GetArray());
	Double_t mins = 0, maxs = median * 3;
//	median = TMath::Median(np, fChi2List.GetArray());
//	Double_t minch =0, maxch = median * 10;

	cout << endl
		  << "-------------------------------------------" << endl;
	cout << "Fitting results for histogram: " << fSelHist->GetName() << endl;
	cout << "Fitting function: " << fFuncName << " Chi2/Ndf: ";
	Double_t chi2NDF = fFitFunc->GetChisquare()/fFitFunc->GetNDF();
	Double_t tailfrac  = GetParValue("TaFract");
	Double_t tailwidth = GetParValue("TaWidth");
	if ( chi2NDF > 5 )
		cout << setred;
	cout << fFitFunc->GetChisquare()/fFitFunc->GetNDF();
	cout << setblack << endl;
	if (tailwidth > 0)
		cout << endl << "TailWidth: " << tailwidth;
	if (tailfrac > 0)
		cout << " TailFract: " << tailfrac;
	cout << endl;
	cout << "-------------------------------------------" << endl;
	cout << "   Content      Mean     Sigma"<< endl;
	for (Int_t i = 0; i < np; i++) {
		if ( fConstantList[i] > maxc || fConstantList[i] < minc )
			cout << setred;
		cout << Form("%10.4g", fConstantList[i]); cout << setblack;
		cout << Form("%10.4g", fMeanList[i]); cout << setblack;
		if ( fSigmaList[i] > maxs || fSigmaList[i] < mins )
			cout << setred;
		cout << Form("%10.4g", fSigmaList[i]); cout << setblack;
//		if ( fChi2List[i] > maxch || fChi2List[i] < minch )
//			cout << setred;
//		cout << Form("%10.4g", fChi2List[i]); cout << setblack;
		cout << endl;
	}
}
 //__________________________________________________________________________

Bool_t FitOneDimDialog::FitGausExecute()
{
	Int_t retval = 0;
	Check4Reselect();
	if (fGraph != NULL && fGraph->GetN() == 0) {
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning",
					 "No histogram nore graph defined\n Use Draw only" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return kFALSE;
	}
	static TArrayD * par = NULL;
	Int_t ind = 0;
	if (fTailOnly) {
		fOnesig = 1;
		if ( fHightail == 0 && fLowtail == 0 ) {
			fLowtail = 1;
			cout << "No tail side given, use low tail" << endl;
		}
	}
	if ( fGaussOnly ) {
		fLowtail = fHightail = 0;
	}
	Double_t lTailSide = 0;
	if ( fGaussTail ) {
		if (fHightail == 1) {
			lTailSide = -1;
			if (fLowtail) {
				cout << "Tail side dubious, use low tail" << endl;
				fHightail = 0;
			}
		}
		if (fOnesig == 0) {
			cout << setred << "Gauss+Tail selected: Force Same Gauss width" << setblack << endl;
			fOnesig = 1;
		}
	}
	if (fLowtail) {
		lTailSide = 1;
	}
	if (fBackg0 != 0) {
		cout << "With Background == 0, Force BG slope = 0" << endl;
		fSlope0 = 1;
	}
	if (fUsedbg != 0) {
		cout << "With Use det backg , need const + slope" << endl;
		fSlope0 = 0;
		fBackg0 = 0;
	}
	Double_t lBinW = fSelHist->GetBinWidth(1);
	Bool_t setpars = (fUseoldpars == 0);
//	if (setpars) cout << "ent setpars true" << endl;

	Int_t npars;
	if (fPrintStartValues) {
		PrintMarkers();
	}
	Double_t from_save = fFrom;
	Double_t to_save = fTo;
	
	if ( GetMarkers() < 2 ) {
		cout << setblue<< "Warning: No marks set" << endl
		<< "Using entire range [" << fFrom << ", " << fTo << "]"
		<< setblack << endl;
		/*
		if (fMarkers == NULL) {
			fMarkers = new  FhMarkerList();
			fSelHist->GetListOfFunctions()->Add(fMarkers);
		}
		Double_t x = fSelHist->GetBinLowEdge(fSelHist->GetXaxis()->GetFirst());
		FhMarker *m = new FhMarker(x, 0, 28);
		fMarkers->Add(m);
		x = fSelHist->GetBinLowEdge(fSelHist->GetXaxis()->GetLast())
						 + fSelHist->GetBinWidth(1);

		m = new FhMarker(x, 0, 28);
		fMarkers->Add(m);
		//		return kFALSE;
		GetMarkers();
		*/
	}	
	if (fDialog) fDialog->ReloadValues();

	if (fTailOnly)
		fOnesig = 1;
	Bool_t range_changed = 
		TMath::Abs(fFrom-from_save) > fSelHist->GetBinWidth(1) ||
		TMath::Abs(fTo-to_save) > fSelHist->GetBinWidth(1);
	if (fOnesig == 0)
		npars = fNpeaks * 3;
	else
		npars = 1 + fNpeaks * 2;
//	npars += kFix;
	if (fBackg0 == 0 || lTailSide != 0)  npars++;
	if ((fBackg0 == 0 && fSlope0 == 0) || lTailSide != 0)  npars++;
	if (lTailSide != 0) npars += 2;
	if (fTailOnly !=0 ) npars--;	 // no tailfraction
	if (par == NULL) {
		par	 = new TArrayD(3 * npars);
		setpars = kTRUE;
//		if (setpars)  cout << "new setpars true: npars " << npars << endl;
	} else {
		if (par->GetSize() != 3 * npars) {
			par->Set(3 * npars);
			setpars = kTRUE;
//			if (setpars)  cout << "size setpars true: npars "  << npars<< endl;;
		}
	}
//	cout << "npars fFrom fTo: " << npars << " " << fFrom<< " " << fTo<< endl;
//	cout << (*par)[0] << " " << (*par)[1] << " "  << (*par)[2] << endl;

	TArrayI * fbflags	= new TArrayI(2 * npars);
// get estimates
	TArrayD gpar(3);
	ind = 0;
	Int_t bin_from = fSelHist->FindBin(fFrom);
	Int_t bin_to	= fSelHist->FindBin(fTo);
//	Int_t bin = 0;
//	TF1 * func = NULL;
	fFuncName = Form("_%d", fFuncNumber);
	fFuncNumber++;
	if (lTailSide) {
		if (	fTailOnly == 0 ) {
			fFuncName.Prepend("gauss_tail_lbg");
			fFitFunc = new TF1(fFuncName, gaus_tail, fFrom, fTo, npars+kFix);
		} else {
			fFuncName.Prepend("tail_only_lbg");
			fFitFunc = new TF1(fFuncName, tail_only, fFrom, fTo, npars+kFix);
		}
	} else {
		if (fOnesig) {
			if (fBackg0 != 0) {
				fFuncName.Prepend("gauss_only");
				fFitFunc = new TF1(fFuncName, gaus_only, fFrom, fTo, npars+kFix);
			} else if (fSlope0 != 0) {
				fFuncName.Prepend("gauss_cbg");
				fFitFunc = new TF1(fFuncName, gaus_cbg,  fFrom, fTo, npars+kFix);
			} else {
				fFuncName.Prepend("gauss_lbg");
				fFitFunc = new TF1(fFuncName, gaus_lbg,  fFrom, fTo, npars+kFix);
			}
		} else {
			if (fBackg0 != 0) {
				fFuncName.Prepend("gauss_only");
				fFitFunc = new TF1(fFuncName, gaus_only_vsig, fFrom, fTo, npars+kFix);
			} else if (fSlope0 != 0) {
				fFuncName.Prepend("gauss_cbg");
				fFitFunc = new TF1(fFuncName, gaus_cbg_vsig,  fFrom, fTo, npars+kFix);
			} else {
				fFuncName.Prepend("gauss_lbg");
				fFitFunc = new TF1(fFuncName, gaus_lbg_vsig,  fFrom, fTo, npars+kFix);
			}
		}
	}
//	TEnv env(".hprrc");
//	env.SetValue("FitOneDimDialog.fGausFuncName", fFuncName);
//	env.SaveLevel(kEnvLocal);

	fFitFunc->SetParameter(0, lBinW);
	fFitFunc->SetParameter(1, fNpeaks);
	fFitFunc->SetParameter(2, lTailSide);
	fFitFunc->SetParName(0, "BinW");
	fFitFunc->SetParName(1, "Npeaks");
	fFitFunc->SetParName(2, "TailSide");

//	cout << " ????????????????????????????? " << endl;
//	cout << "Formula: |" << fFitFunc->GetTitle() << "|"<< endl;
// in case it was not incremented
	ind = npars;
// add bound and fixflags
	for (Int_t i = 0; i < 2 * npars; i++) {
		(*fbflags)[i] = 0;
		(*par)[ind++] = 0;
	}
	ind = 0;
	TOrdCollection row_lab;
//	cout << par->GetSize() << endl;
	if (lTailSide != 0 && fTailOnly == 0) {
		fFitFunc->SetParName(ind+kFix,		"TaFract");
		row_lab.Add(new TObjString("TaFract"));
		if (setpars) (*par)[ind] = 1;
		ind++;
		fFitFunc->SetParName(ind+kFix,		"TaWidth");
		row_lab.Add(new TObjString("TaWidth"));
		if (setpars) (*par)[ind] = 5;
		ind++;
		(*fbflags)[2] = fBackg0;
		(*fbflags)[3] = fSlope0;
	}

	Double_t bgest = 0.5 * (fSelHist->GetBinContent(bin_from)
								  + fSelHist->GetBinContent(bin_to));
	(*par)[ind] = 0;						  
	if (fBackg0 == 0 || fUsedbg == 1|| lTailSide != 0) {
		fFitFunc->SetParName(ind+kFix,		"BgConst");
		row_lab.Add(new TObjString("BgConst"));
		if (setpars) (*par)[ind] = bgest/fSelHist->GetBinWidth(bin_from);
		ind++;
	}
	(*par)[ind] = 0;						  
	if (fSlope0 == 0 || fUsedbg == 1 || lTailSide != 0) {
		fFitFunc->SetParName(ind+kFix,		"BgSlope");
		row_lab.Add(new TObjString("BgSlope"));
		if (setpars) (*par)[ind] = 0;
		ind++;
	}
		
//	cout << "fNmarks " <<  fNmarks<< endl;
	// In case of gauss + tail content of peak = ga_cont*(1 + tailfraction)
	// with tail only = content
	TString cont_name;
	if (fTailOnly)
		cont_name = "Content";
	else
		cont_name = "Ga_Cont";
	TString lab;
	Int_t ind_sigma = 0;
	if (fOnesig == 1) {
//  use same sigma for all peaks
		fFitFunc->SetParName(ind+kFix,"GaSigma_");
		row_lab.Add(new TObjString("GaSigma_"));
		ind_sigma = ind;
		if (setpars) (*par)[ind] = -1;								// Sigma
		ind++;
	}
	// tail only
	if (fTailOnly != 0) {
		fFitFunc->SetParName(ind+kFix,"TaWidth");
		row_lab.Add(new TObjString("TaWidth"));
		if (setpars) (*par)[ind] = 5;
		ind++;

	// tail only always has bg, fix it if needed
		(*fbflags)[0] = fBackg0;
		(*fbflags)[1] = fSlope0;
	}
	if (fNmarks == 2 || fNmarks == 0) {
// 2 marks only assume peak in between
//		bin = GetMaxBin(fSelHist, bin_from, bin_to);
		if (fGraph == NULL)
			GetGaussEstimate(fSelHist, fFrom, fTo, bgest, gpar);
		else
			GetGaussEstimate(fGraph, fFrom, fTo, bgest, gpar);
		lab = cont_name;
		lab += "0";
		fFitFunc->SetParName(ind+kFix, lab);
		row_lab.Add(new TObjString(lab));
		if (setpars || range_changed) {
			if ( gpar[0] <= 0 ) gpar[0] = 10;
			(*par)[ind]	  = gpar[0];
		}
		ind++;
		if (lTailSide != 0) (*par)[ind -1] *= 0.5;
		fFitFunc->SetParName(ind+kFix, "Ga_Mean0");
		 row_lab.Add(new TObjString("Ga_Mean0"));
		if (setpars|| range_changed) (*par)[ind]	  = gpar[1];
		ind++;
		if (fOnesig == 1) {
			if (setpars) {
				(*par)[ind_sigma] = gpar[2];
//				if (ind_tail > 0)
//					(*par)[ind_tail] = gpar[2];
			}
		} else {
			fFitFunc->SetParName(ind+kFix, "GaSigma_");
			row_lab.Add(new TObjString("GaSigma_"));
			if (setpars) (*par)[ind] = gpar[2];					 // Sigma
			ind++;
		}

	} else {
//	 look for peaks at intermediate marks
		for (Int_t i = 1; i < fNmarks - 1; i++) {
			Double_t mpos = ((FhMarker *) fMarkers->At(i))->GetX();
			Double_t mprev = ((FhMarker *) fMarkers->At(i-1))->GetX();
			Double_t mnext = ((FhMarker *) fMarkers->At(i+1))->GetX();
//			bin			  = fSelHist->FindBin(mpos); 
			Double_t d1 = (mpos-mprev);
			Double_t d2 = (mnext-mpos);
			d1 = 0.5 * TMath::Min(d1, d2);
			if (fGraph == NULL)
				GetGaussEstimate(fSelHist, mpos-d1, mpos+d1, bgest, gpar);
			else
				GetGaussEstimate(fGraph, mpos-d1, mpos+d1, bgest, gpar);
			lab = cont_name;
			lab += (i - 1);
			fFitFunc->SetParName(ind+kFix,  lab.Data() );
			row_lab.Add(new TObjString(lab.Data()));
			if (setpars || range_changed) {
				// make sure content is > 0
				if ( gpar[0] <= 0 ) gpar[0] = 10;
				(*par)[ind] = gpar[0];
			}
			ind++;
			lab = "Ga_Mean";
			lab += (i - 1);
			fFitFunc->SetParName(ind+kFix,  lab.Data() );
			row_lab.Add(new TObjString(lab.Data()));
			if (setpars || range_changed)
				(*par)[ind]	  = gpar[1];
			ind++;
			if (fOnesig == 1) {
				if (gpar[2] > (*par)[ind_sigma] && setpars)  {		 // use biggest sigma as est
					(*par)[ind_sigma] = gpar[2];
//					(*par)[ind_tail]  = gpar[2];
				}
			} else {
				lab = "GaSigma";
				lab += (i - 1);
				fFitFunc->SetParName(ind+kFix,  lab.Data() );
				row_lab.Add(new TObjString(lab.Data()));
				if (setpars) (*par)[ind]	  = gpar[2];
				ind++;
			}

		}
	}
	TString temp;
	for (Int_t i = 0; i < fFitFunc->GetNpar(); i++) {
		temp= fFitFunc->GetParName(i);
//		cout << i << " " << temp << endl;
		if (temp.BeginsWith("BgConst")) {
			if ( fBackg0 ) {
				fFitFunc->SetParameter(i, 0);
				fFitFunc->FixParameter(i,0);
				(*par)[i-kFix] = 0;
			} else if (fUsedbg) {
				fFitFunc->SetParameter(i,fLinBgConst / lBinW);
				fFitFunc->FixParameter(i,fLinBgConst / lBinW);
				(*par)[i-kFix] = fLinBgConst / lBinW;
				(*fbflags)[i-kFix] = 1;
			}
		}
		if (temp.BeginsWith("BgSlope")) {
			if ( fSlope0 ) {
				fFitFunc->SetParameter(i, 0);
				fFitFunc->FixParameter(i,0);
				(*par)[i-kFix] = 0;
			} else if (fUsedbg) {
				fFitFunc->SetParameter(i,fLinBgSlope / lBinW);
				fFitFunc->FixParameter(i,fLinBgSlope / lBinW);
				(*par)[i-kFix] = fLinBgSlope / lBinW;
				(*fbflags)[i-kFix] = 1;
			}
		}
	}
	Int_t ret = 0;
	Bool_t do_fit = kTRUE;
	if (fConfirmStartValues) {

		Int_t ncols = 3;
		TOrdCollection col_lab ;
		col_lab.Add(new TObjString("StartVal"));
		col_lab.Add(new TObjString("Low Lim "));
		col_lab.Add(new TObjString("Up Lim  "));
		col_lab.Add(new TObjString("fix it"));
		col_lab.Add(new TObjString("bound"));
		TString title("Start pars, fit from: ");
		title += Form("%g", fFrom);
		title += " to ";
		title += Form("%g", fTo);
		Int_t precission = 5;
	//	row_lab.Print();
		Int_t itemwidth =  30 * TGMrbValuesAndText::LabelLetterWidth();

	//	TGMrbTableOfDoubles(NULL, &ret, title.Data(), itemwidth,
		TGMrbTableOfDoubles(fParentWindow, &ret, title.Data(), itemwidth,
								ncols, npars, *par, precission,
								&col_lab, &row_lab, fbflags, 0,
								NULL, "Do fit", "Draw only");
		if (ret < 0) return kFALSE;
		do_fit = (ret == 0);
		if (fPrintStartValues) {
			cout <<
				"-------------------------------------------------------------"
				<< endl;
			if (ret == 0)
				cout << "Fitting Gauss ";
			else
				cout << "Drawing Gauss ";
			if (fSlope0 == 0) {
				cout << "+ linear BG ";
			} else if (fBackg0 == 0) {
				cout << "+ const BG ";
			}
			cout << "to Histogram:" << fSelHist->GetName()
				<< " from " << fFrom << " fTo " << fTo << endl;
			cout <<
				"-------------------------------------------------------------"
				<< endl;
		}
	}
//  Look for fix /bound flag
//	Bool_t bound = kFALSE;
	fFitFunc->FixParameter(0, fFitFunc->GetParameter(0));
	fFitFunc->FixParameter(1, fFitFunc->GetParameter(1));
	fFitFunc->FixParameter(2, fFitFunc->GetParameter(2));

	for (Int_t i = 0; i < npars; i++) {
		if ((*fbflags)[i] == 1) {
			fFitFunc->FixParameter(i+kFix, (*par)[i]);
		}
		fFitFunc->SetParameter(i+kFix, (*par)[i]);
// bound flag
		if ((*fbflags)[i + npars] == 1) {
			fFitFunc->SetParLimits(i+kFix, (*par)[i + npars], (*par)[i + 2 * npars]);
//			bound = kTRUE;
			if ((*par)[i + 2 * npars] == (*par)[i + npars]) {
				cout << "warning upper = lower bound" << endl;
			}
		} else {
//			cout << fFitFunc->GetParName(i+kFix)<< endl;
			lab = fFitFunc->GetParName(i+kFix);
			if ( lab.Contains("Ga_Mean") ){
				if ( fMeanBond > 0 )
					fFitFunc->SetParLimits(i+kFix, (*par)[i] - fMeanBond, (*par)[i] + fMeanBond);
				else
					fFitFunc->SetParLimits(i+kFix, fFrom, fTo);
//				cout << " fFitFunc->SetParLimits" <<  fFitFunc->GetParName(i+kFix)<< endl;
			} else if ( lab.Contains("Cont") ) {
//				cout << " fFitFunc->SetParLimits" <<  fFitFunc->GetParName(i+kFix)<< endl;
				fFitFunc->SetParLimits(i+kFix, 0, fSelHist->Integral());
			} else if ( lab.Contains("Sigma") ) {
//				cout << " fFitFunc->SetParLimits" <<  fFitFunc->GetParName(i+kFix)<< endl;
				fFitFunc->SetParLimits(i+kFix, 0, fTo - fFrom);
			}
		}
	}
	fFitFunc->SetLineWidth(fWidth);
	fFitFunc->SetLineColor(fColor);

//	fFitFunc->SetParameters(par->GetArray());

	if (fPrintStartValues) {
		fFitFunc->Print();
	}
//  now fit it
	gPad->cd();
	if (do_fit) {
		TString fitopt = "R";	  // fit in range
		if (fFitOptLikelihood)fitopt += "L";
		if (fFitOptQuiet || fFitOptOneLine)	fitopt += "Q";
		if (fFitOptVerbose)  fitopt += "V";
		if (fFitOptMinos)    fitopt += "E";
		if (fFitOptErrors1)  fitopt += "W";
		if (fFitOptIntegral) fitopt += "I";
//		if (fFitOptNoDraw)	 fitopt += "0";
		if (fFitOptAddAll)   fitopt += "+";
//		if (bound)				fitopt += "B";	// some pars are bound
		fitopt += "B";                      // gaus const, mean are always bound
		fitopt += "0";
		Double_t statwidth = 0;
		TPaveStats * stbox=(TPaveStats*)fSelHist->GetListOfFunctions()->FindObject("stats");
		if (stbox) {
			statwidth = stbox->GetX2NDC() - stbox->GetX1NDC();
			cout << "Enter statbox: " << statwidth << " " << stbox->GetX2NDC() - stbox->GetX1NDC()<< endl;
		}
		cout << fFuncName.Data()<< " fitopt.Data() " << fitopt.Data() << endl;
		if (fGraph != NULL) {
			fGraph->Fit(fFuncName.Data(), fitopt.Data(), "");	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (!fFitOptNoDraw) {
				fFitFunc->Draw("same");
// need to reset bit stored in histogram
				if ( fSelHist && fSelHist->GetListOfFunctions()->Last() )
					fSelHist->GetListOfFunctions()->Last()->ResetBit(TF1::kNotDraw);
			}
			if (fFitOptAddAll) {
				TList *lof = fGraph->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}

		} else {
	 //  here fitting is done
			TPaveStats * stats = (TPaveStats*)fSelHist->GetListOfFunctions()->FindObject("stats");
			gStyle->SetOptFit(1);
			if (stats)
				delete stats;
//				stats->SetOptFit(1);
			fSelHist->Fit(fFuncName.Data(), fitopt.Data(), "");
			if (!fFitOptNoDraw) {
				if ( fSelHist && fSelHist->GetListOfFunctions()->Last() )
					fSelHist->GetListOfFunctions()->Last()->ResetBit(TF1::kNotDraw);
				fFitFunc->Draw("same");
			}
	//	  add to ListOfFunctions if requested
		  if (fFitOptAddAll) {
				TList *lof = fSelHist->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}
		}
		PrintCorrelation();
		fFitFunc->SetFillStyle(0);
		if (fAutoClearMarks) ClearMarkers();
		// Resize statbox blown up by THistPainter
		gPad->Update();
		if ( fSelHist ) {
			stbox=(TPaveStats*)fSelHist->GetListOfFunctions()->FindObject("stats");
			if (stbox) {
				cout << "Resize statbox: " << statwidth << " " << stbox->GetX2NDC() - stbox->GetX1NDC()<< endl;
				if ( TMath::Abs(statwidth - (stbox->GetX2NDC() - stbox->GetX1NDC()) ) > 0.001)
					stbox->SetX1NDC(stbox->GetX2NDC() - statwidth);
			}
		}
	} else {
//	 no fit requested draw
		gPad->cd();
		fFitFunc->Draw("same");
		if ( fFitFunc->GetMaximum() > fSelHist->GetMaximum() )
			fSelHist->SetMaximum(1.05 * fFitFunc->GetMaximum());
		cout << setblue << "No fit done, function drawn with start parameters"
			 << setblack << endl;
	}

//  draw components of fit, skip simple gaus without bg

	for (Int_t i = 0; i < npars; i++) {
	  (*par)[i] = fFitFunc->GetParameter(i+kFix);
//	  cout << (*par)[i] << endl;
	}

	if ( fFitOptOneLine ) {
		if ( fNpeaksList == 0) {
			fNpeaksListIndex = 0;
			fConstantList.Set(fNpeaks);
			fMeanList.Set(fNpeaks);
			fSigmaList.Set(fNpeaks);
			fChi2List.Set(fNpeaks);
		}
		if (fFitFunc) 
			AddPeaktoList(fFitFunc);
		if ( fNpeaksList == 0)
			 PrintPeakList();
	}
	if (fShowcof != 0 && fGraph == NULL) {
		if ( fTailOnly != 0 )
			lTailSide = 0;	  // no need to draw tail
//		fFitFunc->GetParameters(par->GetArray());
		Double_t fdpar[4];
		if ( lTailSide != 0 ) {
			fdpar[0] = (*par)[2] ;
			fdpar[1] = (*par)[3];
		} else {
			fdpar[0] = (*par)[0];
			fdpar[1] = (*par)[1];
		}
		if (fBackg0 == 0) {
			TF1 *back = new TF1("backf", backf, fFrom, fTo, 2+kFix);
			back->SetParameter(0, lBinW);
			back->SetParName(0+kFix, "Bg_Const");
			back->SetParameter(0+kFix, fdpar[0]);
			back->SetParName(1+kFix, "Bg_Slope");
			if (fSlope0 == 0) back->SetParameter(1+kFix, fdpar[1]);
			else				  back->SetParameter(1+kFix, 0);
			back->Save(fFrom, fTo, 0, 0, 0, 0);
			back->SetLineColor(kRed);
			back->SetLineStyle(3);
//			back->Print();
//			cout << ">>>back->Eval(fFrom) " << back->Eval(fFrom) << endl;
			back->Draw("same");
		}
		if ( fTailOnly != 0 )
			goto ALLDONE;
		Int_t offset = 0;
		if (lTailSide != 0) {
			offset = 4;
		} else {
			if (fSlope0 == 0)	 offset++;
			if (fBackg0 == 0)	 offset++;
		}
		Int_t mult = 3;
		if (fOnesig == 1) {
			offset++;
			mult = 2;
		}

		for (Int_t j = 0; j < fNpeaks; j++) {
			fdpar[0] = (*par)[0 + offset + mult * j];
			fdpar[1] = (*par)[1 + offset + mult * j];
			if (fOnesig == 1)
				fdpar[2] = (*par)[offset -1];
			else
				fdpar[2] = (*par)[2 + offset + mult * j];
			TF1 *gaus = new TF1("gausf", gausf, fFrom, fTo, 3+kFix);
			gaus->SetParName(0,      "BinWidth");
			gaus->SetParName(0+kFix, "Ga_Cont");
			gaus->SetParName(1+kFix, "Ga_Mean");
			gaus->SetParName(2+kFix, "Ga_Sigm");
			gaus->SetParameter(0, lBinW);
			
			for (Int_t i = 0; i<3; i++)
				gaus->SetParameter(i+kFix, fdpar[i]);
			TF1 *g1 = new TF1();
			gaus->Copy(*g1);
			g1->SetLineColor(6);
			g1->SetLineStyle(4);
			g1->Draw("same");
			if (lTailSide != 0) {
				fdpar[0] = (*par)[0] * (*par)[0 + offset + mult * j];	// const
				fdpar[1] = (*par)[1 + offset + mult * j];	// position
				if (fOnesig == 1)
					fdpar[2] = (*par)[offset -1];
				else
					fdpar[2] = (*par)[2 + offset + mult * j];
				fdpar[3] = (*par)[1]; // tail width
				TF1 *tail = new TF1("tailf", tailf, fFrom, fTo, 4+kFix);
				tail->SetParName(0,     "BinWidth");
				tail->SetParName(1,     "        ");
				tail->SetParName(2,     "Ta_Side ");
				tail->SetParName(0+kFix, "Ta_Const");
				tail->SetParName(1+kFix, "Ta_Mean ");
				tail->SetParName(2+kFix, "Ta_Sigma");
				tail->SetParName(3+kFix, "Ta_Width");
				
				tail->SetParameter(0, lBinW);
				tail->SetParameter(2, lTailSide);
				for (Int_t i = 0; i<4; i++)
					tail->SetParameter(i+kFix, fdpar[i]);
				tail->Save(fFrom, fTo, 0, 0, 0, 0);
				tail->SetLineColor(7);
				tail->SetLineStyle(2);
				tail->Draw("same");
			}
		}
	}
	ALLDONE:
	delete fbflags;
	gPad->Modified(kTRUE);
	gPad->Update();
	gPad->GetFrame()->SetBit(TBox::kCannotMove);
	SaveDefaults();
	return kTRUE;
}
//__________________________________________________________________________

Double_t FitOneDimDialog::GetParValue(const char * pname)
{
	TString parname;
	Int_t npar = fFitFunc->GetNpar();
	for (Int_t i = 0; i < npar; i++) {
		parname = fFitFunc->GetParName(i);
		if (parname.Contains(pname)) {
			return fFitFunc->GetParameter(i);
		}
	}
	return 0;
}
//__________________________________________________________________________

void FitOneDimDialog::AddPeaktoList(TF1 */*func*/)
{
	cout << "AddPeaktoList " << endl;
	Double_t sigma = 0, mean = 0 , cont = 0 , chi2 = 0,
	tailfrac = 0;
	Int_t npar_to_find = 3;
	Int_t npar = fFitFunc->GetNpar();
	chi2 = fFitFunc->GetChisquare() / fFitFunc->GetNDF();
	TString parname;
	if ( fOnesig ) {
		sigma = GetParValue("Sigma");
		npar_to_find --;
	}
	if ( fTailOnly == 0 && (fLowtail != 0 || fHightail != 0 ) ) {
		tailfrac  = GetParValue("TaFract");
//		tailwidth = GetParValue("TaWidth");
	}
	Int_t np = npar_to_find;
	for (Int_t i = 0; i < npar; i++) {
		parname = fFitFunc->GetParName(i);
//		cout << "parname " <<parname << endl;
		if (!fOnesig && parname.Contains("Sigma")) {
			sigma = fFitFunc->GetParameter(i);
			np--;
		}
		if (parname.Contains("Cont")) {
			cont = fFitFunc->GetParameter(i) * ( 1 + tailfrac);
			np--;
		}
		if (parname.Contains("Mean")) {
			mean = fFitFunc->GetParameter(i);
			np--;
		}
		if ( np  == 0 ) {
			if (fConstantList.GetSize() <= fNpeaksListIndex) {
				fConstantList.Set(fNpeaksListIndex + 1);
				fMeanList.Set(fNpeaksListIndex + 1);
				fSigmaList.Set(fNpeaksListIndex + 1);
				fChi2List.Set(fNpeaksListIndex + 1);
			}
			fConstantList[fNpeaksListIndex] = cont;
			fMeanList[fNpeaksListIndex]     = mean;
			fSigmaList[fNpeaksListIndex]    = sigma;
			fChi2List[fNpeaksListIndex]     = chi2;
			fNpeaksListIndex++;
			np = npar_to_find;
		}
	}
}

//____________________________________________________________________________________

void FitOneDimDialog::GetGaussEstimate(TH1 * h, Double_t from, Double_t to,
													Double_t bg, TArrayD &par)
{
	Int_t frombin = h->FindBin(from);
	Int_t tobin = h->FindBin(to);
	Double_t sum = 0;
	Double_t sumx = 0;
	Double_t sumx2 = 0;
	for (Int_t i = frombin; i <= tobin; i++) {
		Double_t cont = TMath::Max(h->GetBinContent(i) - bg, 0.);
		Double_t x = h->GetBinCenter(i);
		sum += cont;
		sumx += x * cont;
		sumx2 += x * x * cont;
	}
	par[0] = sum;
	if (sum > 0.) {
		par[1] = sumx / sum;
		if ( (sumx2 /sum - par[1]*par[1]) > 0)
			par[2] = TMath::Sqrt(sumx2 / sum - par[1]*par[1]);
		else
			par[2] = 1;
	} else {
		par[1] = 0.5 * (to + from);
		par[2] = 1;
	}
}
//____________________________________________________________________________________

void FitOneDimDialog::GetGaussEstimate(TGraph * g, Double_t from, Double_t to,
													Double_t bg, TArrayD & par)
{
	Double_t sum = 0;
	Double_t sumx = 0;
	Double_t sumx2 = 0;
	Double_t x, y;
	for (Int_t i = 0; i < g->GetN(); i++) {
		g->GetPoint(i, x, y);
		if (x >= from && x <= to) {
			Double_t cont = y - bg;
			sum += cont;
			sumx += x * cont;
			sumx2 += x * x * cont;
		}
	}
	par[0] = sum;
	if (sum > 0.) {
		par[1] = sumx / sum;
		if ( (sumx2 /sum - par[1]*par[1]) > 0)
			par[2] = TMath::Sqrt(sumx2 / sum - par[1]*par[1]);
		else
			par[2] = 1;
	} else {
		par[1] = 0.5 * (to - from);
		par[2] = 1;
	}
}
//____________________________________________________________________________________

Int_t FitOneDimDialog::GetMaxBin(TH1 * h1, Int_t binl, Int_t binu)
{
	Double_t maxcont = 0.;
	Int_t bin = (binu - binl) / 2;
	if (bin <= 0) {
		bin = -bin;
		return bin;
	}
	for (Int_t i = binl; i <= binu; i++) {
		Double_t cont = (Double_t) h1->GetBinContent(i);
		if (cont > maxcont) {
			maxcont = cont;
			bin = i;
		}
	};
//  cout << "MaxBin " << bin  << endl;
	return bin;
};
//____________________________________________________________________________________
void FitOneDimDialog::PrintMarkers()
{
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL || fMarkers->GetEntries() <= 0) {
		cout << " No markers set" << endl;
	} else {
		fMarkers->Print();
	}
}
//____________________________________________________________________________________

Int_t FitOneDimDialog::GetMarkers()
{
	fNmarks = 0;
	fNpeaks = 1;
//	if (fSelHist) {
//		fFrom = fSelHist->GetXaxis()->GetMinimum();
//		fTo	= fSelHist->GetXaxis()->GetMaximum();
//	}
// find number of peaks to fit
	TAxis * xa = fSelHist->GetXaxis();
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
		fMarkers->Sort();
		fNmarks = fMarkers->GetEntries();
		if (fNmarks > 0)
			fFrom = ((FhMarker *) fMarkers->At(0))->GetX();
		if (fNmarks > 1)
			fTo	= ((FhMarker *) fMarkers->At(fNmarks - 1))->GetX();
		if ( fFrom < xa->GetBinLowEdge(xa->GetFirst()) )
		  fFrom = xa->GetBinLowEdge(xa->GetFirst());
		if ( fTo > xa->GetBinLowEdge(xa->GetLast()) )
		  fTo = xa->GetBinLowEdge(xa->GetLast());
	} else {
		fFrom = xa->GetBinUpEdge(xa->GetFirst());
		fTo	= xa->GetBinLowEdge(xa->GetLast());
		fNpeaks = 1;
	}
	if (fNmarks > 2) {
		fNpeaks = fNmarks - 2;
	}
	cout << setblue << fNmarks << " markers found i.e. "
		  << fNpeaks << " peak(s) will be fitted" << setblack << endl;
	return fNmarks;
}
//____________________________________________________________________________________

void FitOneDimDialog::ClearMarkers() {
	if (fMarkers) fMarkers->Delete();
//	fMarkers = NULL;
	if (fSelPad) {
		fSelPad->Modified();
		fSelPad->Update();
	}
//	cout << "Clear Markers" << endl;
};
//____________________________________________________________________________________

Int_t  FitOneDimDialog::SetMarkers() {
	if (fSelPad == NULL ) 
		return 0;
	else 
		fSelPad->cd();
	cout << "Request " << fReqNmarks << " Markers" << endl;
	Int_t nmarks = 0;
	if (fReqNmarks <= 0) {
		Int_t retval = 0;
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning","fNmarks <= 0" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return 0;
	}
	if ( fSelPad->GetAutoExec() )
		fSelPad->ToggleAutoExec();
	ClearMarkers();
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL) {
		fMarkers = new  FhMarkerList();
		fSelHist->GetListOfFunctions()->Add(fMarkers);
	}
	for (Int_t i = 0; i < fReqNmarks; i++) {
		TMarker * m1  = (TMarker*)gPad->WaitPrimitive("TMarker");
		m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
		if (m1 == NULL) break;
		Double_t x = m1->GetX();
		Double_t y = m1->GetY();
		if (i == 0) fFrom = x;
		if (i == 1) fTo = x;

//		if (fSelPad->GetLogx()) x = TMath::Power(10, x);
//		if (fSelPad->GetLogy()) y = TMath::Power(10, y);
		FhMarker *m = new FhMarker(x, y, 28);
		m->SetMarkerColor(6);
		m->SetMarkerSize(1);
		m->Paint();
		fMarkers->Add(m);
		delete m1;
		nmarks++;
	}
	PrintMarkers();
	fSelPad->Modified();
	fSelPad->Update();
	if (fDialog) fDialog->ReloadValues();
	return nmarks;
};
//____________________________________________________________________________________

void FitOneDimDialog::DetLinearBackground()
{
	if (fSlope0 == 0  && GetMarkers() != 4) {
		Int_t retval = 0;
		TGWindow* win = (TGWindow*)fParentWindow;
		new TGMsgBox(gClient->GetRoot(), win,
					 "Warning", "Please set exactly 4 marks" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	if (fSlope0 != 0  && GetMarkers() != 2) {
		Int_t retval = 0;
		TGWindow* win = (TGWindow*)fParentWindow;
		new TGMsgBox(gClient->GetRoot(), win,
					 "Warning", "Please set exactly 2 marks" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	Double_t sum1 = 0, sum2 = 0,sumx = 0, 
			meanx1 = 0, meany1 = 0, meanx2 = 0, meany2 = 0;
	Double_t xlow1 = ((FhMarker *) fMarkers->At(0))->GetX();
	Double_t xup1  = ((FhMarker *) fMarkers->At(1))->GetX();
	Int_t binlow1  = fSelHist->FindBin(xlow1);
	Int_t binup1	= fSelHist->FindBin(xup1);
	Double_t xlow2 = 0;
	Double_t xup2  = 0;
	Int_t binlow2  = 0;
	Int_t binup2	= 0;
	for (Int_t i = binlow1; i <= binup1; i++) {
		sum1 += fSelHist->GetBinContent(i);
		sumx += fSelHist->GetBinContent(i) * fSelHist->GetBinCenter(i);
	}
	meanx1 = sumx / sum1;
	meany1 = sum1 / (binup1 - binlow1 + 1);
	if ( fSlope0 == 0 ) {
		xlow2 = ((FhMarker *) fMarkers->At(2))->GetX();
		xup2  = ((FhMarker *) fMarkers->At(3))->GetX();
		binlow2  = fSelHist->FindBin(xlow2);
		binup2	= fSelHist->FindBin(xup2);
		sumx = 0;
		for (Int_t i = binlow2; i <= binup2; i++) {
			sum2 += fSelHist->GetBinContent(i);
			sumx += fSelHist->GetBinContent(i) * fSelHist->GetBinCenter(i);
		}
		meanx2 = sumx / sum2;
		meany2 = sum2 /(binup2 - binlow2 + 1);
	}

//	cout << "meanx1 " << meanx1 << " meany1 " << meany1
//			<< " meanx2 " << meanx2 << " meany2 " <<  meany2 << endl;
	if ( fSlope0 == 0 ) {
		fLinBgSlope =  (meany2 - meany1) / (meanx2 - meanx1);
		fLinBgConst = meany1 - fLinBgSlope * meanx1;
	} else {
		fLinBgSlope = 0;
		fLinBgConst = meany1;
	}
//	TF1 *lin_bg = new TF1("lin_bg", "pol1", xlow1, xup2);
//	lin_bg->SetParameters(fLinBgConst, fLinBgSlope);
//	cHist->cd();
//	lin_bg->Draw("same");
	cout << "Linear Background: [" << xlow1 << ", " << xup1 << "], ["
		  << xlow2 << ", " << xup2 << "]" << endl
		  << "Const = " << fLinBgConst
		  << " Slope = " << fLinBgSlope;
		  if (fSlope0 != 0 ) {
				cout << " fixed" << endl;
			}
			cout << endl;
	ClearMarkers();
	fLinBgSet = kTRUE;
	return ;
}
//_______________________________________________________________________

void FitOneDimDialog::SetFittingOptions()
{
	const char helptext_par[] =
	"Most parameters are obvious, given rudimentary knowlegde\n\
	of Minuit\n\
	\"Confirm start values\" allows to adjust the starting\n\
	values of the fit and fix or set limits on them.\n\
	Extreme care must be taken when setting limits. Minuit\n\
	might give unreasonable results.\n\
	For \"Params for FitPeakList\" consult the Help in the FitDialog.\n\
	";
	TList *row_lab = new TList();
	static void *valp[50];
	Int_t ind = 0;
	Double_t dummy;
	row_lab->Add(new TObjString("CheckButton_   Use Loglikelihood method"));
	row_lab->Add(new TObjString("RadioButton_    Quiet, minimal printout"));
	row_lab->Add(new TObjString("RadioButton_      Print one line / peak"));
	row_lab->Add(new TObjString("RadioButton_           Verbose printout"));
	row_lab->Add(new TObjString("CheckButton_   Use Minos to improve fit"));
	row_lab->Add(new TObjString("CheckButton_        Set all errors to 1"));
	row_lab->Add(new TObjString("CheckButton_Use Integral of func in bin"));
	row_lab->Add(new TObjString("CheckButton_  Dont draw result function"));
	row_lab->Add(new TObjString("CheckButton_Add all fittd funcs to hist"));
	row_lab->Add(new TObjString("CheckButton_    Print covariance matrix"));
	row_lab->Add(new TObjString("CheckButton_       Confirm start values"));
	row_lab->Add(new TObjString("CheckButton_         Print start values"));
	row_lab->Add(new TObjString("CheckButton_     Draw components of fit"));
	row_lab->Add(new TObjString("DoubleValue_   Bind gauss mean +- value"));
	row_lab->Add(new TObjString("CommentOnly_     Params for FitPeakList"));
	row_lab->Add(new TObjString("DoubleValue_     Half Fit Window[sigma]"));
	row_lab->Add(new TObjString("DoubleValue_ Two Peak Separation[sigma]"));

	valp[ind++] = &fFitOptLikelihood;
	valp[ind++] = &fFitOptQuiet;
	valp[ind++] = &fFitOptOneLine;
	valp[ind++] = &fFitOptVerbose;
	valp[ind++] = &fFitOptMinos;
	valp[ind++] = &fFitOptErrors1;
	valp[ind++] = &fFitOptIntegral;
	valp[ind++] = &fFitOptNoDraw;
	valp[ind++] = &fFitOptAddAll;
	valp[ind++] = &fFitPrintCovariance;
	valp[ind++] = &fConfirmStartValues;
	valp[ind++] = &fPrintStartValues;
	valp[ind++] = &fShowcof;
	valp[ind++] = &fMeanBond;
	valp[ind++] = &dummy;
	valp[ind++] = &fFitWindow;
	valp[ind++] = &fPeakSep;
	Int_t ok = -2;
	Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();

	new TGMrbValuesAndText ("Fitting options", NULL, &ok, itemwidth,
								fParentWindow, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext_par);
	if (ok == 0) SaveDefaults();
}
//_______________________________________________________________________

//void FitOneDimDialog::ClearFunctionList()
//{
//	fSelHist->GetListOfFunctions()->Delete();
//}
//________________________________________________________________________

void FitOneDimDialog::FitExpExecute()
{
	ExpExecute(0);	 // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawExpExecute()
{
	ExpExecute(1);	 // draw only
}
//________________________________________________________________________

void FitOneDimDialog::ExpExecute(Int_t draw_only)
{
	Int_t retval = 0;
	Check4Reselect();
	if (draw_only == 0 && fSelHist == NULL && (fGraph != NULL && fGraph->GetN()) == 0) {
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning",
					 "No histogram nore graph defined\n Use Draw only" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	 GetMarkers();
//	if ( GetMarkers() <= 0 ) {
 //	  new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
//					 "Warning", "No marks set,\n need at least 2" ,
//					 kMBIconExclamation, kMBDismiss, &retval);
//		return;
//	}
	fFitFunc = new TF1(fFuncName.Data(),"[0] + [1]*exp([2]*(x - [3]))",fFrom,fTo);
	fFitFunc->SetParameter(0, fExpA);
	if (fExpFixA != 0) fFitFunc->FixParameter(0, fExpA);
	fFitFunc->SetParName(0, "a (y_off)");
	fFitFunc->SetParameter(1, fExpB);
	if (fExpFixB != 0) fFitFunc->FixParameter(1, fExpB);
	fFitFunc->SetParName(1, "b (const)");
	fFitFunc->SetParameter(2, fExpC);
	if (fExpFixC != 0) fFitFunc->FixParameter(2, fExpC);
	fFitFunc->SetParName(2, "c (slope)");
	fFitFunc->SetParameter(3, fExpO);
	if (fExpFixO != 0) fFitFunc->FixParameter(3, fExpO);
	fFitFunc->SetParName(3, "d (x_off)");
	fFitFunc->SetLineWidth(fWidth);
	fFitFunc->SetLineColor(fColor);
//	cout << " ????????????????????????????? " << endl;
//	cout << "Formula: |" << fFitFunc->GetTitle() << "|"<< endl;
	if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
		fFitFunc->Draw("same");
		fFitFunc->Print();
	} else {
		TString fitopt = "R";	  // fit in range
		if (fFitOptLikelihood)fitopt += "L";
		if (fFitOptQuiet)	  fitopt += "Q";
		if (fFitOptVerbose)	fitopt += "V";
		if (fFitOptMinos)	  fitopt += "E";
		if (fFitOptErrors1)	fitopt += "W";
		if (fFitOptIntegral)  fitopt += "I";
		if (fFitOptNoDraw)	 fitopt += "0";
		if (fFitOptAddAll)	 fitopt += "+";
//		Bool_t bound = (fExpFixA + fExpFixB + fExpFixC + fExpFixD) != 0;
		Bool_t bound = (fExpFixA + fExpFixB + fExpFixC) != 0;
		if (bound)				fitopt += "B";	// some pars are bound
		if (fGraph != NULL) {
			fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fGraph->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}

		} else if (fSelHist != NULL) {
			gStyle->SetOptFit(1);
			TPaveStats *stats = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
			if ( stats )
				delete stats;
			fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fSelHist->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}
		}
		fExpA = fFitFunc->GetParameter(0);
		fExpB = fFitFunc->GetParameter(1);
		fExpC = fFitFunc->GetParameter(2);
		fExpO = fFitFunc->GetParameter(3);
		if (fAutoClearMarks) ClearMarkers();
//		IncrementIndex(&fFuncName);
//		TEnv env1(".hprrc");
//		env1.SetValue("FitOneDimDialog.fExpFuncName", fFuncName);
//		env1.SaveLevel(kEnvLocal);
		PrintCorrelation();
	}
	gPad->Modified(kTRUE);
	gPad->Update();
}
//________________________________________________________________________

void FitOneDimDialog::FitLandauExecute()
{
	LandauExecute(0);	 // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawLandauExecute()
{
	LandauExecute(1);	 // draw only
}
//________________________________________________________________________

void FitOneDimDialog::LandauExecute(Int_t draw_only)
{
	Int_t retval = 0;
	Check4Reselect();
	if (draw_only == 0 && fSelHist == NULL && (fGraph != NULL && fGraph->GetN()) == 0) {
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning",
					 "No histogram nore graph defined\n Use Draw only" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	 GetMarkers();
//	if ( GetMarkers() <= 0 ) {
 //	  new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
//					 "Warning", "No marks set,\n need at least 2" ,
//					 kMBIconExclamation, kMBDismiss, &retval);
//		return;
//	}
	fFitFunc = new TF1(fFuncName.Data(),"[0]*TMath::Landau(x, [1], [2])",fFrom,fTo);
	fFitFunc->SetParameter(0, fLandauA);
	if (fLandauFixA |= 0) fFitFunc->FixParameter(0, fLandauA);
	fFitFunc->SetParName(0, "Norm");
	fFitFunc->SetParameter(1, fLandauB);
	if (fLandauFixB |= 0) fFitFunc->FixParameter(1, fLandauB);
	fFitFunc->SetParName(1, "Location");
	fFitFunc->SetParameter(2, fLandauC);
	if (fLandauFixC |= 0) fFitFunc->FixParameter(2, fLandauC);
	fFitFunc->SetParName(2, "Scale");
	
	fFitFunc->SetLineWidth(fWidth);
	fFitFunc->SetLineColor(fColor);
//	cout << " ????????????????????????????? " << endl;
//	cout << "Formula: |" << fFitFunc->GetTitle() << "|"<< endl;
	if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
		fFitFunc->Draw("same");
//		fFitFunc->Print();
	} else {
		TString fitopt = "R";	  // fit in range
		if (fFitOptLikelihood)fitopt += "L";
		if (fFitOptQuiet)	  fitopt += "Q";
		if (fFitOptVerbose)	fitopt += "V";
		if (fFitOptMinos)	  fitopt += "E";
		if (fFitOptErrors1)	fitopt += "W";
		if (fFitOptIntegral)  fitopt += "I";
		if (fFitOptNoDraw)	 fitopt += "0";
		if (fFitOptAddAll)	 fitopt += "+";
//		Bool_t bound = (fExpFixA + fExpFixB + fExpFixC + fExpFixD) != 0;
		Bool_t bound = (fExpFixA + fExpFixB + fExpFixC) != 0;
		if (bound)				fitopt += "B";	// some pars are bound
		
		if (fGraph != NULL) {
			fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fGraph->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}

		} else if (fSelHist != NULL) {
			gStyle->SetOptFit(1);
			TPaveStats *stats = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
			if ( stats )
				delete stats;
//			fFitFunc->Print();
			fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fSelHist->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}
		}
		fLandauA = fFitFunc->GetParameter(0);
		fLandauB = fFitFunc->GetParameter(1);
		fLandauC = fFitFunc->GetParameter(2);
		if (fAutoClearMarks) ClearMarkers();
		IncrementIndex(&fFuncName);
		PrintCorrelation();
	}
	gPad->Modified(kTRUE);
	gPad->Update();
}
//________________________________________________________________________

void FitOneDimDialog::CalcStartParExp()
{
	Double_t xlow, xup, ylow, yup;
	Int_t nmarks = GetMarkers();
	if (nmarks > 0) {
	  xlow = ((FhMarker *) fMarkers->At(0))->GetX();
	} else {
	  xlow = fFrom;
	}
	if (nmarks > 1) {
		xup  = ((FhMarker *) fMarkers->At(nmarks-1))->GetX();
	} else {
	  xup = fTo;
	}


	Int_t binlow = fSelHist->FindBin(xlow);
	Int_t binup  = fSelHist->FindBin(xup);
	ylow = fSelHist->GetBinContent(binlow);
	yup  = fSelHist->GetBinContent(binup);
	if ( TMath::Abs(fExpA) >  yup ) {
		cout << setred << "Warning: you might want to set Parameter \"a\", see \"Help\"" << setblack << endl;
	}
	ylow = ylow - fExpA;
	if (ylow < 1) ylow = 1;
	yup = yup - fExpA;
	if (yup < 1) yup = 1;
	fExpC = TMath::Log(yup/ylow) / (xup - xlow);
	fExpB = TMath::Exp(TMath::Log(yup) - fExpC * xup);
//	cout << fExpA << " " << fExpB << " " << fExpC
 //	<< " " << xup << " " << yup << endl;
}

//________________________________________________________________________

void FitOneDimDialog::FitPolExecute()
{
	PolExecute(0);	 // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawPolExecute()
{
	PolExecute(1);	 // draw only
}
//________________________________________________________________________

void FitOneDimDialog::PolExecute(Int_t draw_only)
{
	Int_t retval = 0;
	Check4Reselect();
	if (draw_only == 0 && fSelHist == NULL && (fGraph != NULL && fGraph->GetN()) == 0) {
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning",
					 "No histogram nore graph defined\n Use Draw only" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	if (fPolN < 0) {
		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
					 "Warning", "Degree of Polynom < 0" ,
					 kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
//	if ( GetMarkers() <= 0 ) {
//		new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
 //					"Warning", "No marks set,\n need at least 2" ,
 //					kMBIconExclamation, kMBDismiss, &retval);
//		return;
//	}
	GetMarkers();
	cout << "Fitting interval: " << fFrom << " : " << fTo << endl;
	TString fn;
	TString pn;
//	TArrayD par(fPolN+1);

	for (Int_t i = 0; i <= fPolN; i++) {
		if (i > 0) fn += "+";
		fn += "["; fn += i;fn += "]";
		for (Int_t k = 1; k <= i; k++) {
			fn += "*x";
		}
	}
//	cout << "fn: " << fn << endl;
	fFitFunc = new TF1(fFuncName.Data(),fn.Data(), fFrom, fTo);
	for (Int_t i = 0; i <= fPolN; i++) {
		pn = "a"; pn += i;
		fFitFunc->SetParName(i, pn);
		fFitFunc->SetParameter(i, fPolPar[i]);
		if (fPolFixPar[i] |= 0) fFitFunc->FixParameter(i, fPolPar[i]);
	}
	fFitFunc->SetLineWidth(fWidth);
	fFitFunc->SetLineColor(fColor);
	if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
		fFitFunc->Draw("same");
//		fFitFunc->Print();
	} else {
		TString fitopt = "R";	  // fit in range
		if (fFitOptLikelihood)fitopt += "L";
		if (fFitOptQuiet)	  fitopt += "Q";
		if (fFitOptVerbose)	fitopt += "V";
		if (fFitOptMinos)	  fitopt += "E";
		if (fFitOptErrors1)	fitopt += "W";
		if (fFitOptIntegral)  fitopt += "I";
//		if (fFitOptNoDraw)	 fitopt += "0";
		if (fFitOptAddAll)	 fitopt += "+";
		fitopt += "0";	 // dont draw
		Int_t bound = 0;
		for (Int_t i = 0; i < 6; i++) {
			bound += fFormFixPar[i];
		}
		if (bound > 0)				fitopt += "B";	// some pars are bound
		if (fGraph != NULL) {
			gStyle->SetOptFit(1);
			fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fGraph->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}

		} else if (fSelHist != NULL){
			TPaveStats *stats = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
			gStyle->SetOptFit(1);
			if ( stats )
				delete stats;
			fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fSelHist->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}
		}
		if ( !fFitOptNoDraw )
			fFitFunc->Draw("same");
		for (Int_t i = 0; i <= fPolN; i++) {
			fPolPar[i] = fFitFunc->GetParameter(i);
		}
//		IncrementIndex(&fFuncName);
//		TEnv env1(".hprrc");
//		env1.SetValue("FitOneDimDialog.fPolFuncName", fFuncName);
//		env1.SaveLevel(kEnvLocal);
		if (fAutoClearMarks) ClearMarkers();
		PrintCorrelation();
	}
	gPad->Modified(kTRUE);
	gPad->Update();
}
//________________________________________________________________________

void FitOneDimDialog::FitFormExecute()
{
	FormExecute(0);	 // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawFormExecute()
{
	FormExecute(1);	 // draw only
}
//________________________________________________________________________

void FitOneDimDialog::GraphFormExecute()
{
	FormExecute(fNsegsGraph);	 // draw only
}
//________________________________________________________________________

void FitOneDimDialog::FormExecute(Int_t draw_only)
{
	Int_t retval = 0;
	Check4Reselect(); 
	cout << "FormExecute  fSelHist: " << fSelHist ;
	if ( fSelHist )
		cout << " fSelHist->GetEntries() " << fSelHist->GetEntries();
	cout << " draw_only " << draw_only<< endl;
	if (draw_only == 0 && (fSelHist == 0 || fSelHist->GetEntries() < 1)
		  && (fGraph == NULL || fGraph->GetN() == 0)) {
//		fSelHist = Hpr::FindHistInPad(fSelPad);
		TIter next(fSelPad->GetListOfPrimitives());
		while (TObject * obj = next()) {
			if (obj->InheritsFrom("TH1")) {
				fSelHist = (TH1*)obj;
				break;
			}
		}
		
		cout << "FindHistInPad: fSelHist: " << fSelHist ;
		if ( fSelHist )
			cout << " fSelHist->GetEntries() " << fSelHist->GetEntries();
		cout << endl;
	
		if (fSelHist == 0 || fSelHist->GetEntries() < 1 ) {
//			fGraph = Hpr::FindGraphInPad(fSelPad);
			TIter next1(fSelPad->GetListOfPrimitives());
			while (TObject * obj = next1()) {
				if (obj->InheritsFrom("TGraph")) {
					fGraph = (TGraph*)obj;
					break;
				}
			}
			if ( fGraph == NULL ) {
				new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
						"Warning",
						"No histogram nore graph defined\n Use Draw only" ,
						kMBIconExclamation, kMBDismiss, &retval);
				return;
			} else {
				fSelHist = NULL;
			}
		} else {
			fGraph = NULL;
		}
	}
	if ( fSelHist )
		GetMarkers();
	cout << "Fitting interval: " << fFrom << " : " << fTo 
	<< " fSelHist " << fSelHist << " fGraph: " << fGraph<< endl;

	TString fn;
	TString pn;
	fFitFunc = new TF1(fFuncName,(const char*)fFormula, fFrom, fTo);
	if (fFitFunc->GetNdim() <= 0) {
		cout << setred << "Something wrong with formula"<< setblack << endl;
		return;
	}
	Int_t npars = fFitFunc->GetNpar();
	if (npars > 6) {
		cout << "Max 6 parameters allowed, correct function" << endl;
		return;
	}
	for (Int_t i = 0; i < npars; i++) {
		pn = "a"; pn += i;
		fFitFunc->SetParName(i, pn);
		fFitFunc->SetParameter(i, fFormPar[i]);
		if (fFormFixPar[i] |= 0) fFitFunc->FixParameter(i, fFormPar[i]);
	}
	fFitFunc->Print();
	cout << endl<< "Formula " << (const char*)fFormula << endl;
	fFitFunc->SetLineWidth(fWidth);
	fFitFunc->SetLineColor(fColor);
	if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
		if ( draw_only == 1 ) { 
			fFitFunc->SetNpx(1000);
			fFitFunc->Draw("same");
		} else {
			Int_t np = draw_only;
			if ( np < 0 )
				np = -np;
			np++;
			TGraph *gr = new TGraph(np);
			Double_t dx = (fTo - fFrom) / np;
			Double_t x = fFrom;
			for (Int_t i=0; i<np; i++) {
				gr->SetPoint(i,x, fFitFunc->Eval(x));
				x += dx;
			}
			TEnv env(".hprrc");
			gr->Draw(env.GetValue("GraphAttDialog.fDrawOpt","PA"));
//			GraphAttDialog::SetGraphAtt(fSelPad);
		}
//		cout << "Val[2] " << fFitFunc->Eval(2.) << endl;
//		fFitFunc->Print();
	} else {
		TString fitopt = "R";	  // fit in range
		if (fFitOptLikelihood)fitopt += "L";
		if (fFitOptQuiet)	  fitopt += "Q";
		if (fFitOptVerbose)	fitopt += "V";
		if (fFitOptMinos)	  fitopt += "E";
		if (fFitOptErrors1)	fitopt += "W";
		if (fFitOptIntegral)  fitopt += "I";
//		if (fFitOptNoDraw)	 fitopt += "0";
		if (fFitOptAddAll)	 fitopt += "+";
		fitopt += "0";
		Int_t bound = 0;
		for (Int_t i = 0; i < 6; i++) {
			bound += fFormFixPar[i];
		}
		if (bound > 0)				fitopt += "B";	// some pars are bound
		if ( fGraph != NULL && fGraph->GetN() > 1) {
			fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fGraph->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}

		} else if (fSelHist != NULL){
			TPaveStats *stats = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
			gStyle->SetOptFit(1);
			if ( stats )
				delete stats;
			fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
	//	  add to ListOfFunctions if requested
			if (fFitOptAddAll) {
				TList *lof = fSelHist->GetListOfFunctions();
				if (lof->GetSize() > 1) {
					TObject *last = lof->Last();
					lof->Remove(last);
					lof->AddFirst(last);
				}
			}
		}
		if ( !fFitOptNoDraw )
			fFitFunc->Draw("same");
		for (Int_t i = 0; i < npars; i++) {
			fFormPar[i] = fFitFunc->GetParameter(i);
		}
		fCalFunc = fFitFunc;
		// get function serial namber
//		TRegexp snr("[0-9]*$");
//		IncrementIndex(&fFuncName);
//		TEnv env1(".hprrc");
//		env1.SetValue("FitOneDimDialog.fFormFuncName", fFuncName);
//		env1.SaveLevel(kEnvLocal);
		if (fAutoClearMarks) ClearMarkers();
		PrintCorrelation();
	}
	gPad->Modified(kTRUE);
	gPad->Update();
}
//________________________________________________________________________
void FitOneDimDialog::PrintCorrelation()
{
  if (fFitPrintCovariance) {
		TVirtualFitter *fitter = TVirtualFitter::GetFitter();
		if (fitter->GetCovarianceMatrix() == 0) {
			cout << "Invalid fit result, cant get covariance matrix" << endl;
			return;
		}
		Int_t ntot = fitter->GetNumberTotalParameters();
		cout << "-- Correlationmatrix (Cov(i,j) / (err i * err j)) -----" << endl;
		printf("%9s", "			 ");
		for (Int_t i=0; i < ntot; i++) {
			if (fitter->IsFixed(i) == 0)printf("%11s", fitter->GetParName(i));
		}
		cout << endl;
		Int_t indi = 0;
		for (Int_t i =0; i < ntot; i++) {
			if (fitter->IsFixed(i) == 0) {
				printf("%10s", fitter->GetParName(i));
				Int_t indj = 0;
				for (Int_t j=0; j < ntot; j++) {
					if (fitter->IsFixed(j) == 0) {
						Double_t ei =  fitter->GetParError(i);
						Double_t ej =  fitter->GetParError(j);
						Double_t cor = fitter->GetCovarianceMatrixElement(indi, indj)/(ei*ej);
						printf("%10.3g ", cor);
						indj++;
					}
				}
				indi++;
			cout << endl;
			}
		}
		for (Int_t i=0; i < ntot; i++) {
			if (fitter->IsFixed(i) == 0) cout << "-----------";
		}
		cout << endl;
	}
}
//________________________________________________________________________

TH1 *FitOneDimDialog::FindHistInPad()
{
	TList * lop = gPad->GetListOfPrimitives();
	TIter next(lop);
	TH1 *hist = NULL;
	while (TObject *obj = next()) {
		if(obj->InheritsFrom("TH1")) {
			hist = (TH1*)obj;
			break;
		}
	}
	return hist;
}
//________________________________________________________________________

void FitOneDimDialog::FillHistRandom()
{
	TH1 *hist = FindHistInPad();
	if (!hist) {
		cout << "No hist found" << endl;
		return;
	}
	gStyle->SetOptStat(1111111);
	TString fn;
	TString pn;
	TString newname(fFuncName);
	newname += "_range";
	TF1Range *func = new TF1Range(newname,(const char*)fFormula, fFrom, fTo);
	if (func->GetNdim() <= 0) {
		cout << "Something wrong with formula" << endl;
		return;
	}
	Int_t npars = func->GetNpar();
	if (npars > 6) {
		cout << "Max 6 parameters allowed, correct function" << endl;
		return;
	}
	for (Int_t i = 0; i < npars; i++) {
		pn = "a"; pn += i;
		func->SetParName(i, pn);
		func->SetParameter(i, fFormPar[i]);
	}
	Double_t integral = func->Integral(fFrom, fTo);
	cout << "Integral[" << fFrom << ", "<< fTo  << "] = " << integral << endl;
//	func->Dump();
	Int_t fillN = fNevents;
	if ( fillN <= 0 )
		fillN = (Int_t)integral;
	hist->FillRandom(newname, fillN);
	hist->SetMaximum(hist->GetBinContent(hist->GetMaximumBin()));
//	hist->Print();
	gPad->Modified();
	gPad->Update();
}
//_______________________________________________________________________

void FitOneDimDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fFitOptLikelihood = env.GetValue("FitOneDimDialog.FitOptLikelihood", 0);
	fFitOptQuiet		= env.GetValue("FitOneDimDialog.FitOptQuiet", 0);
	fFitOptOneLine	 = env.GetValue("FitOneDimDialog.FitOptOneLine", 1);
	fFitOptVerbose	 = env.GetValue("FitOneDimDialog.FitOptVerbose", 0);
	fFitOptMinos		= env.GetValue("FitOneDimDialog.FitOptMinos", 0);
	fFitOptErrors1	 = env.GetValue("FitOneDimDialog.FitOptErrors1", 0);
	fFitOptIntegral	= env.GetValue("FitOneDimDialog.FitOptIntegral", 0);
	fFitOptNoDraw	  = env.GetValue("FitOneDimDialog.FitOptNoDraw", 0);
	fFitOptAddAll	  = env.GetValue("FitOneDimDialog.FitOptAddAll", 1);
	fFitPrintCovariance= env.GetValue("FitOneDimDialog.fFitPrintCovariance", 0);
	fAutoClearMarks	= env.GetValue("FitOneDimDialog.fAutoClearMarks", 0);
	fColor				= env.GetValue("FitOneDimDialog.Color", 4);
	fWidth				= env.GetValue("FitOneDimDialog.Width", 1);
	fStyle				= env.GetValue("FitOneDimDialog.Style", 1);
	fBackg0			  = env.GetValue("FitOneDimDialog.Backg0", 0);
	fSlope0			  = env.GetValue("FitOneDimDialog.Slope0", 0);
	fOnesig			  = env.GetValue("FitOneDimDialog.Onesig", 0);
	fTailOnly			= env.GetValue("FitOneDimDialog.fTailOnly", 0);
	fGaussOnly			= env.GetValue("FitOneDimDialog.fGaussOnly", 0);
	fGaussTail			= env.GetValue("FitOneDimDialog.fGaussTail", 1);
	fLowtail			 = env.GetValue("FitOneDimDialog.Lowtail", 0);
	fHightail			= env.GetValue("FitOneDimDialog.Hightail",0);
	fShowcof			 = env.GetValue("FitOneDimDialog.Showcof", 1);
	fMeanBond		= env.GetValue("FitOneDimDialog.fMeanBond", -1);
	fExpA				 = env.GetValue("FitOneDimDialog.fExpA", 0.);
	fExpB				 = env.GetValue("FitOneDimDialog.fExpB", 1.);
	fExpC				 = env.GetValue("FitOneDimDialog.fExpC", 1.);
	fExpO				 = env.GetValue("FitOneDimDialog.fExpO", 0.);
	fLandauA				 = env.GetValue("FitOneDimDialog.fLandauA", 10.);
	fLandauB				 = env.GetValue("FitOneDimDialog.fLandauB", 10.);
	fLandauC				 = env.GetValue("FitOneDimDialog.fLandauB", 1.);
	fLandauFixA			 = env.GetValue("FitOneDimDialog.fLandauFixA", 0);
	fLandauFixB			 = env.GetValue("FitOneDimDialog.fLandauFixB", 0);
	fLandauFixC			 = env.GetValue("FitOneDimDialog.fLandauFixC", 0);
//	fExpD				 = env.GetValue("FitOneDimDialog.fExpD", 0.);
	fExpFixA			 = env.GetValue("FitOneDimDialog.fExpFixA", 0);
	fExpFixB			 = env.GetValue("FitOneDimDialog.fExpFixB", 0);
	fExpFixC			 = env.GetValue("FitOneDimDialog.fExpFixC", 0);
//	fExpFixD			 = env.GetValue("FitOneDimDialog.fExpFixD", 1);
	fPolN				 = env.GetValue("FitOneDimDialog.fPolN", 1);
	fNsegsGraph		 = env.GetValue("FitOneDimDialog.fNsegsGraph", 100);
	TString tagname;
	for ( Int_t i = 0; i < 6; i++ ) {
		tagname = "FitOneDimDialog.fPolPar";
		tagname += i;
		fPolPar[i] = env.GetValue(tagname, 1.);
		tagname = "FitOneDimDialog.fPolFixPar";
		tagname += i;
		fPolFixPar[i] = env.GetValue(tagname, 0);
	}
	for ( Int_t i = 0; i < 6; i++ ) {
		tagname = "FitOneDimDialog.fFormPar";
		tagname += i;
		fFormPar[i] = env.GetValue(tagname, 1.);
		tagname = "FitOneDimDialog.fFormFixPar";
		tagname += i;
		fFormFixPar[i] = env.GetValue(tagname, 0);
	}
	fGausFuncName			= env.GetValue("FitOneDimDialog.fGausFuncName", "gaus_f");
	fExpFuncName			= env.GetValue("FitOneDimDialog.fExpFuncName", "exp_f");
	fPolFuncName			= env.GetValue("FitOneDimDialog.fPolFuncName", "pol_f");
	fLandauFuncName		= env.GetValue("FitOneDimDialog.fLandauFuncName", "landau_f");
	fFormFuncName			= env.GetValue("FitOneDimDialog.fFormFuncName", "user_f");
	fNevents					= env.GetValue("FitOneDimDialog.fNevents", 10000);
	fPeakSep					= env.GetValue("FitOneDimDialog.fPeakSep", 3);
	fFitWindow				= env.GetValue("FitOneDimDialog.fFitWindow", 3);
	fUseoldpars				= env.GetValue("FitOneDimDialog.fUseoldpars", 0);
	fConfirmStartValues	= env.GetValue("FitOneDimDialog.fConfirmStartValues", 0);
	fPrintStartValues		= env.GetValue("FitOneDimDialog.fPrintStartValues", 0);
	fFuncFromFile			= env.GetValue("FitOneDimDialog.fFuncFromFile", "workfile.root|TF1|f1");
}
//_______________________________________________________________________

void FitOneDimDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("FitOneDimDialog.FitOptLikelihood", fFitOptLikelihood);
	env.SetValue("FitOneDimDialog.FitOptQuiet",		fFitOptQuiet);
	env.SetValue("FitOneDimDialog.FitOptOneLine",	 fFitOptOneLine);
	env.SetValue("FitOneDimDialog.FitOptVerbose",	 fFitOptVerbose);
	env.SetValue("FitOneDimDialog.FitOptMinos",		fFitOptMinos);
	env.SetValue("FitOneDimDialog.FitOptErrors1",	 fFitOptErrors1);
	env.SetValue("FitOneDimDialog.FitOptIntegral",	fFitOptIntegral);
	env.SetValue("FitOneDimDialog.FitOptNoDraw",	  fFitOptNoDraw);
	env.SetValue("FitOneDimDialog.FitOptAddAll",	  fFitOptAddAll);
	env.SetValue("FitOneDimDialog.fFitPrintCovariance", fFitPrintCovariance);
	env.SetValue("FitOneDimDialog.fAutoClearMarks",  fAutoClearMarks);
	env.SetValue("FitOneDimDialog.Color",				fColor);
	env.SetValue("FitOneDimDialog.Width",				fWidth);
	env.SetValue("FitOneDimDialog.Style",				fStyle);
	env.SetValue("FitOneDimDialog.Backg0",			  fBackg0);
	env.SetValue("FitOneDimDialog.Slope0",			  fSlope0);
	env.SetValue("FitOneDimDialog.Onesig",			  fOnesig);
	env.SetValue("FitOneDimDialog.fGaussOnly",		fGaussOnly);
	env.SetValue("FitOneDimDialog.fTailOnly",		  fTailOnly);
	env.SetValue("FitOneDimDialog.fGaussTail",		fGaussTail);
	env.SetValue("FitOneDimDialog.Lowtail",			 fLowtail);
	env.SetValue("FitOneDimDialog.Hightail",			fHightail);
	env.SetValue("FitOneDimDialog.Showcof",			 fShowcof);
	env.SetValue("FitOneDimDialog.fMeanBond",			 fMeanBond);
	env.SetValue("FitOneDimDialog.fExpA",	fExpA	 );
	env.SetValue("FitOneDimDialog.fExpB",	fExpB	 );
	env.SetValue("FitOneDimDialog.fExpC",	fExpC	 );
	env.SetValue("FitOneDimDialog.fExpO",	fExpO	 );
	env.SetValue("FitOneDimDialog.fExpFixA",fExpFixA );
	env.SetValue("FitOneDimDialog.fExpFixB",fExpFixB );
	env.SetValue("FitOneDimDialog.fExpFixC",fExpFixC );
	env.SetValue("FitOneDimDialog.fExpFixD",fExpFixO );
	env.SetValue("FitOneDimDialog.fPolN",	fPolN	 );
	env.SetValue("FitOneDimDialog.fNsegsGraph", fNsegsGraph);
	TString tagname;
	for ( Int_t i = 0; i < 6; i++ ) {
		tagname = "FitOneDimDialog.fPolPar";
		tagname += i;
		env.SetValue(tagname, fPolPar[i]);
		tagname = "FitOneDimDialog.fPolFixPar";
		tagname += i;
		env.SetValue(tagname, fPolFixPar[i]);
	}
	for ( Int_t i = 0; i < 6; i++ ) {
		tagname = "FitOneDimDialog.fFormPar";
		tagname += i;
		env.SetValue(tagname, fFormPar[i]);
		tagname = "FitOneDimDialog.fFormFixPar";
		tagname += i;
		env.SetValue(tagname, fFormFixPar[i]);
	}
	env.SetValue("FitOneDimDialog.fLandauA",   fLandauA);
	env.SetValue("FitOneDimDialog.fLandauB",   fLandauB);
	env.SetValue("FitOneDimDialog.fLandauB",   fLandauC);
	env.SetValue("FitOneDimDialog.fLandauFixA",fLandauFixA);
	env.SetValue("FitOneDimDialog.fLandauFixB",fLandauFixB);
	env.SetValue("FitOneDimDialog.fLandauFixC",fLandauFixC);
//  env.SetValue("FitOneDimDialog.fGausFuncName", fGausFuncName);
//	env.SetValue("FitOneDimDialog.fExpFuncName",  fExpFuncName );
//	env.SetValue("FitOneDimDialog.fPolFuncName",  fPolFuncName );
//	env.SetValue("FitOneDimDialog.fFormFuncName", fFormFuncName);
	if (fFormula.Length() > 1)
		env.SetValue("FitOneDimDialog.fFormula", fFormula);
	env.SetValue("FitOneDimDialog.fNevents", fNevents);
	env.SetValue("FitOneDimDialog.fPeakSep", fPeakSep);
	env.SetValue("FitOneDimDialog.fFitWindow", fFitWindow);
	env.SetValue("FitOneDimDialog.fConfirmStartValues", fConfirmStartValues);
	env.SetValue("FitOneDimDialog.fUseoldpars", fUseoldpars);
	env.SetValue("FitOneDimDialog.fPrintStartValues", fPrintStartValues);
	env.SetValue("FitOneDimDialog.fFuncFromFile", fFuncFromFile);
	env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FitOneDimDialog::CloseDialog()
{
 //  cout << "FitOneDimDialog::CloseDialog() " << endl;
	gROOT->GetListOfCleanups()->Remove(this);
	if (fDialog) fDialog->CloseWindowExt();
	delete this;
}
//_______________________________________________________________________

void FitOneDimDialog::CloseDown(Int_t /*wid*/)
{
//	cout << "FitOneDimDialog::CloseDown() " << endl;
	SaveDefaults();
	delete this;
}
//________________________________________________________________
void FitOneDimDialog::IncrementIndex(TString * arg)
{
// find number at end of string and increment,
// if no number found add "_0";
	Int_t len = arg->Length();
	if (len < 0) return;
	Int_t ind = len - 1;
	Int_t first_digit = ind;
	TString subs;
	while (ind > 0) {
		subs = (*arg)(ind, len - ind);
		cout << subs << endl;
		if (!subs.IsDigit()) break;
		first_digit = ind;
		ind--;
	}
	if (first_digit == ind) {
	  *arg += "_0";
	} else {
		subs = (*arg)(first_digit, len - first_digit);
		Int_t num = atol(subs.Data());
		arg->Resize(first_digit);
		*arg += (num + 1);
	}
}
//________________________________________________________________________________

void FitOneDimDialog::GetFunction()
{
static const Char_t helptext[] =
"Select file name with the browser (use arrow)\n\
Click on the required function name to select it\n\
Then use \"Read function\" to read it in.\n\
Note: As default the last entry is selected\n\
";
	if ( !fInteractive ) {
		cout << setred << "GetFunction() only useful in interactive mode "
			  << setblack << endl;
		return;
	}
	cout << "fFuncFromFile " << fFuncFromFile<< endl;
	TList *row_lab = new TList();
	static TString nvcmd("SetValues()");
	static void *valp[100];
	Int_t ind = 0;
//	static Double_t dummy = 1;
//	static Double_t dummy1 = 2;
//	static Double_t dummy2 = 3;
	static TString  dummy4;
	static TString gfcmd("ExecuteGetFunction()");

	row_lab->Add(new TObjString("FileContReq_File name"));
	valp[ind++] = &fFuncFromFile;
/*
	row_lab->Add(new TObjString("DoubleValue_dummy"));
	valp[ind++] = &dummy;
	row_lab->Add(new TObjString("DoubleValue_dummy1"));
	valp[ind++] = &dummy1;
	row_lab->Add(new TObjString("DoubleValue_dummy2"));
	valp[ind++] = &dummy2;
*/
	row_lab->Add(new TObjString("CommandButt_Read function"));
	valp[ind++] = &gfcmd;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
	Int_t ok = 0;
	new TGMrbValuesAndText ("Get Function from file", NULL, &ok, -itemwidth,
							 fParentWindow, NULL, NULL, row_lab, valp,
							 NULL, NULL, helptext, this, this->ClassName());
}
//________________________________________________________________________________

void FitOneDimDialog::ExecuteGetFunction()
{
	TString fname;
	TString cname;
	TString oname;

	TObjArray * oa = fFuncFromFile.Tokenize("|");
	Int_t nent = oa->GetEntries();
	if (nent < 3) {
		cout << "fFuncFromFile not enough (3) fields" << endl;
		return;
	}
	fname =((TObjString*)oa->At(0))->String();
	cname =((TObjString*)oa->At(1))->String();
	if (cname != "TF1") {
		cout << "fFuncFromFile " << oname << " is not a function " << endl;
		return;
	}
	oname =((TObjString*)oa->At(2))->String();
	TFile f(fname);
	fCalFunc = (TF1*)f.Get(oname);
	if (fCalFunc == NULL) {
		cout << "No function found / selected" << endl;
		return;
	} else {
		SetStartParameters();
	}
}
//________________________________________________________________________________

void FitOneDimDialog::GetSelectedFunction()
{
	if (!fSelHist) {
		cout << "No histogram" << endl;
		return;
	}
	TIter next(fSelHist->GetListOfFunctions());
	TObject *obj;
	TF1* func = NULL;
	while( obj = next() ) {
		if (obj->InheritsFrom("TF1") && obj->TestBit(kSelected)){
			if ( func != NULL) {
				cout << setred << "More than 1 function selected" << endl;
			} else {
				func = (TF1*)obj;
			}
		}
	}
	if (!func) {
		cout << "No function found" << endl;
		return;
	} else {
		fCalFunc = func;
		SetStartParameters();
	}
}
//________________________________________________________________________________

void FitOneDimDialog::SetStartParameters()
{
	
	fCalFunc->Print();
	TString formula(fCalFunc->GetExpFormula());
	if ( formula.Length() < 2 ) {
		cout << "Function is not a Formula" << endl;
	} else {
		fFormula = formula;
		Int_t npars = fCalFunc->GetNpar();
		if ( npars < 1 ) {
			cout << "Illegal formula Npars: " << npars << endl;
			return;
		}
		if ( npars > 6 ) {
			cout << "WARNING Npars: " << npars << endl;
		}
		for (Int_t i = 0; i < npars; i++) {
			fFormPar[i] = fCalFunc->GetParameter(i);
			cout << " fFormPar[" << i << "] " <<  fFormPar[i] << endl;
		}
		if (fDialog) fDialog->ReloadValues();
	}
}
//____________________________________________________________________________________

void FitOneDimDialog::WriteoutFunction()
{
	if (!fFitFunc) {
		cout << "No function defined" << endl;
		return;
	}
	Save2FileDialog sfd(fFitFunc, NULL, fParentWindow);
	IncrementIndex(&fFuncName);
	TEnv env1(".hprrc");
	fFuncName = env1.GetValue("Save2FileDialog.fObjName", fFuncName.Data());
	IncrementIndex(&fFuncName);
	TString resname("FitOneDimDialog.");
	resname += fFuncType;
	env1.SetValue(resname, fFuncName);
	env1.SaveLevel(kEnvLocal);
}
//____________________________________________________________________________________

void FitOneDimDialog::SaveFunction()
{
	if (!fCalFunc) {
		cout << "No function defined" << endl;
		return;
	}
//	if (!fCalHist) {
//	  cout << "Execute Fill histogram first" <<  endl;
//		return;
//	}
//	fCalFunc->SetParent(fCalHist);
//	fSelHist->GetListOfFunctions()->Add(fCalFunc);
	Save2FileDialog sfd(fCalFunc);
}
//____________________________________________________________________________________

void FitOneDimDialog::Check4Reselect()
{
	TH1 * nhist = NULL;
	if ( fSelPad && fSelPad->GetClickSelected()
			&& fSelPad->GetClickSelected()->InheritsFrom("TH1")) {
		nhist = (TH1*)fSelPad->GetClickSelected();
		if (nhist != fSelHist) {
			fSelHist = nhist;
			cout << "Selected hist now: " << fSelHist->GetName() << endl;
			TString tit("Use: ");
			tit+= fSelHist->GetName();
			fDialog->SetWindowName(tit.Data());
		}
	}
}

#ifndef FIT1DIMDIALOG
#define FIT1DIMDIALOG
#include "TH2.h"
#include "TF2.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________

static const Int_t MAXP = 5;
class Fit2DimDialog : public TObject {

private:
   TRootCanvas* fParentWindow;
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   Int_t   fFuncNumber;
   TString fFuncFromFile;
   TCanvas *fSelPad;
   TH2     *fSelHist;
   TH2     *fHistFitResult;
   TF2     *fSelFunc;
   TF2     *fFitFunc;
   Color_t fColor;
   Width_t fWidth;
   Style_t fStyle;
   TString fName;
   TString fGausFuncName;
   FhMarkerList * fMarkers;
   Int_t fNpar;
   Double_t fFromX;
   Double_t fToX;
   Double_t fFromY;
   Double_t fToY;
	Int_t fNpeaks;
   Double_t fSigmaX[MAXP]  ;
   Double_t fSigmaY[MAXP]  ;
   Double_t fConstant[MAXP];
   Double_t fOffX[MAXP]    ;
   Double_t fOffY[MAXP]    ;
   Double_t fPhi[MAXP]     ;
   Int_t fFixSigmaX[MAXP];
   Int_t fFixSigmaY[MAXP];
   Int_t fFixConstant[MAXP];
   Int_t fFixOffX[MAXP];
   Int_t fFixOffY[MAXP];
   Int_t fFixPhi[MAXP];
   Double_t fBgX0;
   Double_t fBgY0;
   Double_t fBgSlopeX;
   Double_t fBgSlopeY;

   Int_t fUseoldpars;        // use parameters of previous fit
   Int_t fUsedbg;            // use prederminated linear background
   Int_t fReqNmarks;
   Int_t fNmarks;
   Int_t fNevents;
   Int_t fFitOptAddAll;      // Add all fitted functions to histogram
   Int_t fAutoClearMarks;    // Clear marks after fit
// The following parameters should be set with "FitPeakList"
   Int_t    fInteractive;    // Run interactivly, ie. really present dialog widgets
   Int_t fShowcof;           // Display components of fit
   Int_t fFitOptLikelihood;  // Use Likelihood method (default chi2)
   Int_t fFitOptQuiet;       // Suppress printout during fitting
   Int_t fFitOptOneLine;     // One line / peak
    Int_t fFitOptVerbose;     // Verbose printout during fitting
   Int_t fFitOptMinos;       // Use minos to improve fit after migrad
   Int_t fFitOptErrors1;     // Set all errors to 1`
   Int_t fFitOptIntegral;    // Use integral of bin
   Int_t fFitOptNoDraw;      // dont draw fit result
   Int_t fFitPrintCovariance;// Print Correlation matrix (normalized covariance)
   Int_t fShowAsHist;

public:
   Fit2DimDialog(TObject *obj, Int_t npeaks = 0, Int_t interactive = 1);
   void DisplayMenu();
   Fit2DimDialog(){fSelPad = NULL; fInteractive = 0;};
   virtual ~Fit2DimDialog();
   void RecursiveRemove(TObject * obj);
   //fit function as class member
   Double_t TwoDimGaus(Double_t *x, Double_t *p);
   Double_t MultTwoDimGaus(Double_t *x, Double_t *p);
   void GausExecute(Int_t draw_only);
   void FitGausExecute();
   void DrawGausExecute();
   void FillHistRandom();
   TH1  *FindHistInPad();
   void PrintCorrelation();
   void SetFitOptions(){};
//   Int_t GetMaxBin(TH2 * h1, Int_t binlX, Int_t binuX, Int_t binlY, Int_t binuY);
/*   void GetGaussEstimate(TH2 *h, Double_t fromX, Double_t toX,
                                 Double_t fromY, Double_t toY,
                                 Double_t bg,TArrayD & par);*/
//   void DetLinearBackground();
   Int_t GetMarkers();
   void ClearMarkers();
   void ClearFunctionList();
   void PrintMarkers();
   Int_t SetMarkers();
   void SetFittingOptions();
   
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown(Int_t wid);
   void IncrementIndex(TString * arg);
   void SaveFunction();
   TF2* GetFitFunction() { return fFitFunc; };
//   void ExecuteGetFunction();
//
   void SetUseoldpars( Int_t useoldpars) { fUseoldpars = useoldpars; };
   void SetUsedbg( Int_t usedbg) { fUsedbg = usedbg; };
   void SetReqNmarks( Int_t reqnmarks) { fReqNmarks = reqnmarks; };
   void SetNmarks( Int_t nmarks) { fNmarks = nmarks; };
   void SetNevents( Int_t nevents) { fNevents = nevents; };
   void SetShowcof( Int_t showcof) { fShowcof = showcof; };
   void SetAutoClearMarks( Int_t autoclearmarks) { fAutoClearMarks = autoclearmarks; };
   void SetFitOptLikelihood( Int_t fitoptlikelihood) { fFitOptLikelihood = fitoptlikelihood; };
   void SetFitOptQuiet( Int_t fitoptquiet) { fFitOptQuiet = fitoptquiet; };
   void SetFitOptOneLine( Int_t val) {fFitOptOneLine  = val; };
   void SetFitOptVerbose( Int_t fitoptverbose) { fFitOptVerbose = fitoptverbose; };
   void SetFitOptMinos( Int_t fitoptminos) { fFitOptMinos = fitoptminos; };
   void SetFitOptErrors1( Int_t fitopterrors1) { fFitOptErrors1 = fitopterrors1; };
   void SetFitOptIntegral( Int_t fitoptintegral) { fFitOptIntegral = fitoptintegral; };
   void SetFitOptNoDraw( Int_t fitoptnodraw) { fFitOptNoDraw = fitoptnodraw; };
   void SetFitOptAddAll( Int_t fitoptaddall) { fFitOptAddAll = fitoptaddall; };
   void SetFitPrintCovariance( Int_t fitprintcovariance) { fFitPrintCovariance = fitprintcovariance; };
//
   Int_t GetUseoldpars() { return fUseoldpars; };
   Int_t GetUsedbg() { return fUsedbg; };
   Int_t GetReqNmarks() { return fReqNmarks; };
   Int_t GetNmarks() { return fNmarks; };
   Int_t GetNevents() { return fNevents; };
   Int_t GetShowcof() { return fShowcof; };
   Int_t GetAutoClearMarks() { return fAutoClearMarks; };
   Int_t GetFitOptLikelihood() { return fFitOptLikelihood; };
   Int_t GetFitOptQuiet() { return fFitOptQuiet; };
   Int_t GetFitOptOneLine() {return fFitOptOneLine; };
   Int_t GetFitOptVerbose() { return fFitOptVerbose; };
   Int_t GetFitOptMinos() { return fFitOptMinos; };
   Int_t GetFitOptErrors1() { return fFitOptErrors1; };
   Int_t GetFitOptIntegral() { return fFitOptIntegral; };
   Int_t GetFitOptNoDraw() { return fFitOptNoDraw; };
   Int_t GetFitOptAddAll() { return fFitOptAddAll; };
   Int_t GetFitPrintCovariance() { return fFitPrintCovariance; };
   void CRButtonPressed(Int_t, Int_t, TObject *){};

ClassDef(Fit2DimDialog,0)
};
#endif

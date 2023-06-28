#ifndef FITONEDIMDIALOG
#define FITONEDIMDIALOG
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class FitOneDimDialog : public TObject {

private:
   TRootCanvas* fParentWindow;
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   TString fFuncType;
   Int_t   fFuncNumber;
   TString fFuncFromFile;
   TCanvas *fSelPad;
   TH1     *fSelHist;
   TF1     *fCalFunc;
   TF1     *fFitFunc;
   TGraph  *fGraph;
   Color_t fColor;
   Width_t fWidth;
   Style_t fStyle;
   TString fName;
   TString fGausFuncName;
   TString fExpFuncName;
   TString fLandauFuncName;
   TString fPolFuncName;
   TString fFormFuncName;
   TString fFormula;
   FhMarkerList * fMarkers;
   Double_t fFrom;
   Double_t fTo;
   Double_t fMean;
   Double_t fMeanError;
   Double_t fConstant;
   Double_t fExpA;
   Double_t fExpB;
   Double_t fExpC;
   Double_t fExpFixA;
   Double_t fExpFixB;
   Double_t fExpFixC;
   Double_t fExpO;     // Offset
   Double_t fExpFixO;     // Offset
   
   Double_t fLandauA;
   Double_t fLandauB;
   Double_t fLandauC;
   Int_t fLandauFixA;
   Int_t fLandauFixB;
   Int_t fLandauFixC;
   Int_t fPolN;
   Double_t fPolPar[6];
   Int_t    fPolFixPar[6];
   Double_t fFormPar[6];
   Int_t    fFormFixPar[6];

   Int_t    fAdded;
   Double_t fLinBgConst;
   Double_t fLinBgSlope;
   Bool_t   fLinBgSet;
   Int_t fUseoldpars;        // use parameters of previous fit
   Int_t fUsedbg;            // use prederminated linear background
   Int_t fNpeaks;            // number peaks used
   Int_t fReqNmarks;
   Int_t fNmarks;
   Int_t fNevents;
   Int_t fNsegsGraph;        // Number of segments (np - 1) of a graph
   Int_t fFitOptAddAll;      // Add all fitted functions to histogram
   Bool_t fFitPeakListDone;
   Int_t fAutoClearMarks;    // Clear marks after fit
// The following parameters should be set with "FitPeakList"
   Int_t    fInteractive;    // Run interactivly, ie. really present dialog widgets
   Double_t fPeakSep;        // if peaks are closer (unit of sigma) they are fitted together
   Double_t fFitWindow;      // Fit window: peak pos += fFitWindow
   Int_t fBackg0;            // Force background = 0
   Int_t fSlope0;            // Force background slope = 0 (use constant background)
   Int_t fOnesig;            // Use same sigma for all peaks ( if fittted together)
   Int_t fGaussOnly;         // Pure gauss function, no tail
   Int_t fGaussTail;         // Pure gauss + tail function
   Int_t fTailOnly;          // Pure tail function, no gauss
   Int_t fLowtail;           // Use low tail
   Int_t fHightail;          // Use high tail
   Int_t fShowcof;           // Display components of fit
   Double_t fMeanBond;       // Bind variation of gauss mean +- this value
   Int_t fConfirmStartValues;// Present a widget with start values before fitting
   Int_t fPrintStartValues;  // Print start values
   Int_t fFitOptLikelihood;  // Use Likelihood method (default chi2)
   Int_t fFitOptQuiet;       // Suppress printout during fitting
   Int_t fFitOptOneLine;     // One line / peak
   Int_t fFitOptVerbose;     // Verbose printout during fitting
   Int_t fFitOptMinos;       // Use minos to improve fit after migrad
   Int_t fFitOptErrors1;     // Set all errors to 1`
   Int_t fFitOptIntegral;    // Use integral of bin
   Int_t fFitOptNoDraw;      // dont draw fit result
   Int_t fFitPrintCovariance;// Print Correlation matrix (normalized covariance)
   Int_t   fNpeaksList;
   Int_t   fNpeaksListIndex;
   TArrayD fConstantList;
   TArrayD fMeanList;
   TArrayD fSigmaList;
   TArrayD fChi2List;
   
public:   
	enum EFitHistBits {
		 kSelected = BIT(15)
	};

public:
   FitOneDimDialog();
   FitOneDimDialog(TH1 * hist, Int_t type = 1, Int_t interactive = 1);
   FitOneDimDialog(TGraph * graph, Int_t type = 1, Int_t interactive = 1);
   void DisplayMenu(Int_t type = 1);
   virtual ~FitOneDimDialog();
   void RecursiveRemove(TObject * obj);
   Bool_t FitPeakList();
   Bool_t FitGausExecute();
   void FitExpExecute();
   void DrawExpExecute();
   void FitLandauExecute();
   void DrawLandauExecute();
   void CalcStartParExp();
   void ExpExecute(Int_t draw_only = 0);
   void LandauExecute(Int_t draw_only = 0);
   void FitPolExecute();
   void DrawPolExecute();
   void PolExecute(Int_t draw_only = 0);
   void PrintCorrelation();
   void AddPeaktoList(TF1 *func);
   Double_t GetParValue(const char * pname);
   void PrintPeakList();
   void FitFormExecute();
   void DrawFormExecute();
   void GraphFormExecute();
   void FormExecute(Int_t draw_only = 0);
   void FillHistRandom();
   TH1  *FindHistInPad();
//   void AddToCalibration();
   void SetFitOptions(){};
   Int_t GetMaxBin(TH1 * h1, Int_t binl, Int_t binu);
   void GetGaussEstimate(TH1 *h, Double_t from, Double_t to,
                         Double_t bg,TArrayD & par);
   void GetGaussEstimate(TGraph *g, Double_t from, Double_t to,
                         Double_t bg,TArrayD & par);
   void DetLinearBackground();
   Int_t GetMarkers();
   void ClearMarkers();
   void ClearFunctionList();
   void WriteoutFunction();
   void PrintMarkers();
   Int_t SetMarkers();
   void SetFittingOptions();
   Bool_t FitPeakListDone() { return fFitPeakListDone; };
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown(Int_t wid);
   void IncrementIndex(TString * arg);
   void SaveFunction();
   void GetFunction();
   void ExecuteGetFunction();
   void GetSelectedFunction();
   void SetStartParameters();
//
   void SetPeakSep( Double_t peaksep) { fPeakSep = peaksep; };
   void SetFitWindow( Double_t fitwindow) { fFitWindow = fitwindow; };
   void SetUseoldpars( Int_t useoldpars) { fUseoldpars = useoldpars; };
   void SetUsedbg( Int_t usedbg) { fUsedbg = usedbg; };
   void SetNpeaks( Int_t npeaks) { fNpeaks = npeaks; };
   void SetReqNmarks( Int_t reqnmarks) { fReqNmarks = reqnmarks; };
   void SetNmarks( Int_t nmarks) { fNmarks = nmarks; };
   void SetNevents( Int_t nevents) { fNevents = nevents; };
   void SetBackg0( Int_t backg0) { fBackg0 = backg0; };
   void SetSlope0( Int_t slope0) { fSlope0 = slope0; };
   void SetOnesig( Int_t onesig) { fOnesig = onesig; };
   void SetLowtail( Int_t lowtail) { fLowtail = lowtail; };
   void SetHightail( Int_t hightail) { fHightail = hightail; };
   void SetShowcof( Int_t showcof) { fShowcof = showcof; };
   void SetAutoClearMarks( Int_t autoclearmarks) { fAutoClearMarks = autoclearmarks; };
   void SetConfirmStartValues( Int_t confirmstartvalues) { fConfirmStartValues = confirmstartvalues; };
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
   Double_t GetPeakSep() { return fPeakSep; };
   Double_t GetFitWindow() { return fFitWindow; };
   Int_t GetUseoldpars() { return fUseoldpars; };
   Int_t GetUsedbg() { return fUsedbg; };
   Int_t GetNpeaks() { return fNpeaks; };
   Int_t GetReqNmarks() { return fReqNmarks; };
   Int_t GetNmarks() { return fNmarks; };
   Int_t GetNevents() { return fNevents; };
   Int_t GetBackg0() { return fBackg0; };
   Int_t GetSlope0() { return fSlope0; };
   Int_t GetOnesig() { return fOnesig; };
   Int_t GetLowtail() { return fLowtail; };
   Int_t GetHightail() { return fHightail; };
   Int_t GetShowcof() { return fShowcof; };
   Int_t GetAutoClearMarks() { return fAutoClearMarks; };
   Int_t GetConfirmStartValues() { return fConfirmStartValues; };
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
	void Check4Reselect();
ClassDef(FitOneDimDialog,0)
};
#endif

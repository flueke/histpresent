#ifndef FINDPEAKDIALOG2D
#define FINDPEAKDIALOG2D
#include "TH2.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class FindPeakDialog2D : public TObject {

private:
   Int_t fInteractive;
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
//   TString fFuncName;
   TVirtualPad *fSelPad;
   TH2     *fSelHist;
   TH2F    *fDeconvHist;
   TCanvas *fDeconvCanvas;
   TString fName;
//	TString fPeakListName;
	FhMarkerList * fMarkers;
	Int_t    fNpeaks;
   Double_t fFromX;               // Used range, lower value
   Double_t fToX;                 // Used range, upper value
   Double_t fFromY;               // Used range, lower value
   Double_t fToY;                 // Used range, upper value
   Double_t fThreshold;          // Minimum peaks size used, unit: fraction compared to highest peak in range
                                 // Note: If height of peaks vary strongly use several ranges
                                 //       use dont clear list (see below)
   Double_t fSigma;              // Assumed peakwidth
//   Double_t fTwoPeakSeparation;  // mimimum separation of two peaks, unit: sigma
   Int_t    fMarkov;					// use Markow algorithm
   Int_t		fHighRes;				// use SearchHighRes
   Int_t    fAverageWindow;		// averanging window of searched peaks
   Int_t    fRemoveBG;           // remove background before deconvolution
   Int_t    fShowDeconv;				// show deconvoluted spectrum
   Int_t    fFindPeakDone;       // number of peaksearches executed
   Int_t    fAutoClear;
	Color_t fMarkerColor;
	Style_t fMarkerStyle;
	Float_t fMarkerSize;
//   Int_t    fUseTSpectrum ;      // trigger TSpectrum method
   
 //  Int_t    fThresholdEntry;
 //  Int_t    fSigmaEntry;
 //  Int_t    fMarkovEntry;
//   Int_t    fRemoveBGEntry;
 //  Int_t    fThresholdSigmaEntry;
 //  Int_t    fPeakMwidthEntry;

public:
   FindPeakDialog2D(TH2 * hist, Int_t interactive =1);
   FindPeakDialog2D();
   virtual ~FindPeakDialog2D();
   void RecursiveRemove(TObject * obj);
   void ExecuteFindPeak();
   void ClearList();
	void PrintList();
	void ReadList();
	Int_t FindPeakDone() { return fFindPeakDone; };
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t, Int_t, TObject*);
   void SetFromX( Double_t from) { fFromX = from; };
   void SetToX( Double_t to) { fToX = to; };
   void SetFromY( Double_t from) { fFromY = from; };
   void SetToY( Double_t to) { fToY = to; };
	void SetThreshold( Double_t threshold) { fThreshold = threshold; };
   void SetSigma( Double_t sigma) { fSigma = sigma; };
//   void SetTwoPeakSeparation( Double_t twopeakseparation) { fTwoPeakSeparation = twopeakseparation; };
   void SetMarkow( Int_t markow) { fMarkov = markow; };
   void SetRemoveBG( Int_t removebg) { fRemoveBG = removebg; };
   void SetShowDeconv( Int_t showd) { fShowDeconv = showd; };
   void SetHighRes() {fHighRes =1;};
//   void SetThresholdSigma(Double_t val) {fThresholdSigma = val;};
 //  void SetPeakMwidth(Int_t val) {fPeakMwidth = val;};
   Double_t GetFromX() { return fFromX; };
   Double_t GetToX() { return fToX; };
   Double_t GetFromY() { return fFromY; };
   Double_t GetToY() { return fToY; };
   Double_t GetThreshold() { return fThreshold; };
   Double_t GetSigma() { return fSigma; };
//   Double_t GetTwoPeakSeparation() { return fTwoPeakSeparation; };
   Int_t GetMarkow() { return fMarkov; };
   Int_t GetRemoveBG() { return fRemoveBG; };
   Int_t GetShowDeconv() { return fShowDeconv; };
   Int_t GetHighRes() {return fHighRes;};
//   Double_t GetThresholdSigma() {return fThresholdSigma;};
//   Int_t GetPeakMwidth() {return (Int_t)fPeakMwidth;};

ClassDef(FindPeakDialog2D,0)
};
#endif

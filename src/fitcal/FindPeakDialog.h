#ifndef FINDPEAKDIALOG
#define FINDPEAKDIALOG
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class FindPeakDialog : public TObject {

private:
   Int_t fInteractive;
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   TString fName;
	TString fPeakListName;
	FhMarkerList * fMarkers;
   Double_t fFrom;               // Used range, lower value
   Double_t fTo;                 // Used range, upper value
   Double_t fThreshold;          // Minimum peaks size used, unit: fraction compared to highest peak in range
                                 // Note: If height of peaks vary strongly use several ranges
                                 //       use dont clear list (see below)
   Double_t fSigma;              // Assumed peakwidth
   Double_t fTwoPeakSeparation;  // mimimum separation of two peaks, unit: sigma
   Int_t    fMarkow;					// use Markow algorithm
   Int_t    fRemoveBG;           // remove background before deconvolution
   Int_t    fShowMarkers;        // draw polymarker at found peak
   Int_t    fFindPeakDone;       // number of peaksearches executed
   Int_t    fUseTSpectrum ;      // trigger TSpectrum method
   Int_t    fUseSQWaveFold;      // trigger Square wave convolution  method
   Double_t fThresholdSigma;     // threshold in Square wave folding search
   Double_t fPeakMwidth;         // width Square wave folding search

   Int_t    fUseTSpectrumEntry ;
   Int_t    fUseSQWaveFoldEntry;
   Int_t    fThresholdEntry;
   Int_t    fSigmaEntry;
   Int_t    fMarkowEntry;
   Int_t    fRemoveBGEntry;
   Int_t    fThresholdSigmaEntry;
   Int_t    fPeakMwidthEntry;

public:
   FindPeakDialog(TH1 * hist, Int_t interactive =1);
   FindPeakDialog();
   virtual ~FindPeakDialog();
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
   void SetFrom( Double_t from) { fFrom = from; };
   void SetTo( Double_t to) { fTo = to; };
   void SetThreshold( Double_t threshold) { fThreshold = threshold; };
   void SetSigma( Double_t sigma) { fSigma = sigma; };
   void SetTwoPeakSeparation( Double_t twopeakseparation) { fTwoPeakSeparation = twopeakseparation; };
   void SetMarkow( Int_t markow) { fMarkow = markow; };
   void SetRemoveBG( Int_t removebg) { fRemoveBG = removebg; };
   void SetShowMarkers( Int_t showmarkers) { fShowMarkers = showmarkers; };
   void SetUseTSpectrum() {fUseTSpectrum =1; fUseSQWaveFold =0;};
   void SetUseSQWaveFold() {fUseTSpectrum =0; fUseSQWaveFold =1;};
   void SetThresholdSigma(Double_t val) {fThresholdSigma = val;};
   void SetPeakMwidth(Int_t val) {fPeakMwidth = val;};
   Double_t GetFrom() { return fFrom; };
   Double_t GetTo() { return fTo; };
   Double_t GetThreshold() { return fThreshold; };
   Double_t GetSigma() { return fSigma; };
   Double_t GetTwoPeakSeparation() { return fTwoPeakSeparation; };
   Int_t GetMarkow() { return fMarkow; };
   Int_t GetRemoveBG() { return fRemoveBG; };
   Int_t GetShowMarkers() { return fShowMarkers; };
   Int_t GetUseTSpectrum() {return fUseTSpectrum;};
   Int_t GetUseSQWaveFold() {return fUseSQWaveFold;};
   Double_t GetThresholdSigma() {return fThresholdSigma;};
   Int_t GetPeakMwidth() {return (Int_t)fPeakMwidth;};

ClassDef(FindPeakDialog,0)
};
#endif

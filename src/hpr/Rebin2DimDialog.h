#ifndef REBIN2DIMDIALOG
#define REBIN2DIMDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "HistPresent.h"

//_____________________________________________________________________________________


class Rebin2DimDialog : public TObject {

private:
   void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TH1          *fHist;
	TString fNewName;
	TString fNewTitle;
	Int_t fBinLowX;
	Int_t fBinUpX;
	Int_t fBinLowY;
	Int_t fBinUpY;
	Int_t fRebinX;
	Int_t fRebinY;
	Int_t fNewNbinsX;
	Int_t fNewNbinsY;
	Int_t   fBidBinLowX;
	Int_t   fBidBinUpX;
	Int_t   fBidBinLowY;
	Int_t   fBidBinUpY;
	Int_t   fBidRebinX;
	Int_t   fBidRebinY;
	Int_t   fBidNewNbinsX;
	Int_t   fBidNewNbinsY;
	
	public:
		Rebin2DimDialog(TH2* hist, TGWindow * win = NULL);
		Rebin2DimDialog() {};
		~Rebin2DimDialog();
		void PrintHistParams();
		void ExecuteRebin();
		void RestoreDefaults();
		void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
		void CloseDialog();
		void CloseDown(Int_t wid);
		ClassDef(Rebin2DimDialog,0)
}
;
#endif

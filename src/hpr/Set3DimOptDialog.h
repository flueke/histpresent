#ifndef SET3DIMOPTHISTDIALOG
#define SET3DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>

//extern Int_t gAlphaOfPalette;
//_____________________________________________________________________________________
static const Int_t kNopt3 = 4;

class TButton;
class TView3D;

class Set3DimOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TH1          *fHist;
	TView3D      *fView3D;
	TButton      *fCmdButton;
   TString fDrawOpt;
   TString fDrawOptPrev;
   Int_t fOptRadio[kNopt3];
   const char *fDrawOpt3DimArray[kNopt3];
   TString fDrawOpt3Dim;
   Color_t fHistFillColor3Dim;
   Color_t fHistLineColor3Dim;
   Color_t fMarkerColor3Dim; 
	Color_t f3DimBackgroundColor;
   Style_t fMarkerStyle3Dim;  
   Size_t  fMarkerSize3Dim; 
	Int_t   fBidSCAT;
	Int_t   fBidBOX;
	Int_t   fBidGL;
	Int_t   fBidPolyM;
	Int_t   fBidView3D;
	Int_t   fBidARR;
	Int_t   fBidTEXT;
	Int_t   fBidFillColor;
	Int_t   fBidLineColor;
	Int_t   fBidMarkerColor;
	Int_t   fBidMarkerStyle;
	Int_t   fBidMarkerSize;
	Int_t   fBidApplyTransp;
	Int_t   fBidTranspBelow;
	Int_t   fBidTranspAbove;
	Int_t   fApplyTranspCut;
	Int_t   fRotDx;
	Int_t   fRotDy;
	Int_t   fRotSleep;
	Int_t   fRotCycles;
	Int_t    fBidPhi;
   Double_t fPhi3Dim;
	Int_t    fBidTheta;
   Double_t fTheta3Dim;
	Double_t fRangeX0;
	Double_t fRangeX1;
	Double_t fRangeY0;
	Double_t fRangeY1;
	Double_t fRangeZ0;
	Double_t fRangeZ1;
	Int_t   fBidRangeX0;
	Int_t   fBidRangeZ1;
	Bool_t  fRangeChanged;
	Double_t fContMin;
	Double_t fContMax;
	Int_t fContLog;
	Int_t fShowZScale;
	Int_t fBidContMin;
	Int_t fBidContMax;
	Int_t fBidContLog;
	Int_t fBidShowZScale;

public:

   Set3DimOptDialog(TGWindow * win = NULL, TButton *b = NULL);
//#if ROOTVERSION > 60000
//	Set3DimOptDialog();
//#endif
   ~Set3DimOptDialog();
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas);
   void SetHistAtt(TCanvas *canvas);
	void Rotate();
//   void SetHistAttPermLocal();
   void SetRanges();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
//	void SetAlphaOfPalette(Int_t alpha) { gAlphaOfPalette = alpha; };
//	Int_t GetAlphaOfPalette() { return gAlphaOfPalette; };
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(Set3DimOptDialog,0)
};
#endif

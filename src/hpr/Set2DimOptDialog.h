#ifndef SET2DIMOPTHISTDIALOG
#define SET2DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TF2.h"
#include "TH2.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

const Int_t kNdrawopt = 32;

class Set2DimOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   TPad         *fPad;
	TH2          *fHist;
 	TF2          *fFunc2;
	TString fDrawOpt;
   TString fSameOpt;
   Int_t fNPads;
   Int_t fOptRadio[kNdrawopt];
   const char *fDrawOpt2DimArray[kNdrawopt];
   TString fDrawOpt2Dim;
   Int_t fHideFrontBox;
   Int_t fHideBackBox;
   Int_t fShowZScale;
   Int_t fLiveStat2Dim;
   TString fViewingAngle;
   Double_t fLogScaleMin;
   Double_t fLogScaleMax;
   Color_t fHistFillColor2Dim;
	Style_t fHistFillStyle2Dim;
	Color_t fHistLineColor2Dim;
	Style_t fHistLineStyle2Dim;
	Int_t   fHistLineWidth2Dim;
	Color_t fMarkerColor2Dim; 
	Color_t f2DimBackgroundColor;
   Style_t fMarkerStyle2Dim;  
   Size_t  fMarkerSize2Dim; 
	Int_t   fBidSCAT;
	Int_t   fBidBOX;
	Int_t   fBidBOX1;
	Int_t   fBidARR;
	Int_t   fBidTEXT;
	Int_t   fBidMarkerColor;
	Int_t   fBidMarkerStyle;
	Int_t   fBidMarkerSize;
	Int_t   fBidLineColor;
	Int_t   fBidFillColor;
	Int_t   fBidLiveStat;
	Int_t   fTwoDimLogX;
	Int_t   fTwoDimLogY;
	Int_t   fTwoDimLogZ;
	Int_t   fUseGL;
	Int_t   fBidUseGL;
	Int_t   fContourLevels;
	Int_t   fHistNo;
	Int_t   fBidHistNo;
	Int_t   fNbinSliceX;
	Int_t   fNbinSliceY;
	Int_t   fBidNbinSliceX;
	Int_t   fBidNbinSliceY;
	
public:

   Set2DimOptDialog(TGWindow * win = NULL);
	Set2DimOptDialog(Int_t batch);
   ~Set2DimOptDialog();
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas);
   void SetHistAtt(TPad *canvas, TObject *obj);
   void SetHistAttPermLocal();
   void SetHistAttAll(TCanvas *canvas);
   void SetHistAttPerm();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
	void GetValuesFromFunc2();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(Set2DimOptDialog,0)
};
#endif

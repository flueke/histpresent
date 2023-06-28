#ifndef SET2DIMGRAPHDIALOG
#define SET2DIMGRAPHDIALOG
#include "TGraph2D.h"
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "HTCanvas.h"
#include "TButton.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

const Int_t kNGraph2Dopt = 22;

class Set2DimGraphDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   HTCanvas      *fCanvas;
   TGraph2D     *fGraph2D;
	TH2          *fHist;
	TButton      *fCmdButton;
   TString fDrawOpt;
   TString fSameOpt;
   Int_t fOptRadio[kNGraph2Dopt];
   const char *fDrawOpt2DimArray[kNGraph2Dopt];
   TString fDrawOpt2Dim;
   Int_t fHideFrontBox;
   Int_t fHideBackBox;
   Int_t fShowZScale;
   Double_t fLogScaleMin;
   Double_t fLogScaleMax;
	Color_t f2DimBackgroundColor;
   Style_t fMarkerColor2Dim;  
   Style_t fMarkerStyle2Dim;  
   Size_t  fMarkerSize2Dim; 
   
	Int_t   fBidSCAT;
	Int_t   fBidBOX;
	Int_t   fBidBOX1;
	Int_t   fBidARR;
	Int_t   fBidGL;
	Int_t   fBidMarkerColor;
	Int_t   fBidMarkerStyle;
	Int_t   fBidMarkerSize;
	Int_t   fBidLineColor;
	Int_t   fBidFillColor;
	Int_t   fBidLiveStat;
	Int_t   fTwoDimLogX;
	Int_t   fTwoDimLogY;
	Int_t   fTwoDimLogZ;
	TString fViewingAngle;	// 0: default 60,120; 1: ;Top(XY) 0,-180;Front 90,-90;Side 90,-180 
	Int_t   fBidViewingAngle;
	Int_t   fUseGL;
	Int_t   fContourLevels;
	Int_t   fRandomizePoints;
	Int_t   fUseNPoints;
public:

   Set2DimGraphDialog(TGWindow * win = NULL);
	Set2DimGraphDialog(Int_t batch);
   ~Set2DimGraphDialog();
   void CloseDialog();
   void SetAttNow(TCanvas *canvas);
   void SetPadAtt(TPad *canvas);
   void SetAttAll(TCanvas *canvas);
   void SetAttPerm();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFrom();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(Set2DimGraphDialog,0)
};
#endif

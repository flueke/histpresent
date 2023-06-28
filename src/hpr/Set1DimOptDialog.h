#ifndef SET1DIMOPTHISTDIALOG
#define SET1DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________
class TH1;

class Set1DimOptDialog : public TObject , public TQObject{

private:
	void *fValp[100];
	Int_t fFlags[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TList fHistList;
	TList fPadList;
	Int_t fNHists;
	Int_t fNPads;

	TCanvas *fCanvas;
	Int_t   fOk;

	TString *fTitle;
	Int_t   *fTitleModBid;
	Int_t   *fShowContour;
	Int_t   *fFill;
	Color_t *fFillColor ;
	Style_t *fFillStyle;
	Color_t *fLineColor;
	Style_t *fLineStyle;
	Width_t *fLineWidth;
	TString *fErrorMode;
	Color_t *fMarkerColor;
	Style_t *fMarkerStyle;
	Float_t *fMarkerSize;
	Int_t   *fShowMarkers;
	Int_t   *fSame;

	TString fTitleX;
	TString fTitleY;
	Float_t fEndErrorSize;
	Float_t fErrorX;
	Int_t   fSmoothLine;
	Int_t   fSimpleLine;
	Int_t   fBarChart  ;
	Int_t   fBarChart3D;
	Int_t   fBarChartH ;
	Int_t   fPieChart  ;
	Int_t   fText      ;
	Int_t   fTextAngle ;
	Int_t   fLiveStat1Dim;
	Int_t   fLiveGauss;
	Int_t   fLiveBG;
	Int_t   fLiveConstBG;

	Int_t   fDrawAxisAtTop;
	Int_t   fLabelsTopX;
	Int_t   fLabelsRightY;
	Int_t   fOneDimLogX;
	Int_t   fOneDimLogY;
	Int_t   fAdvanced1Dim;
	Int_t   fAdjustMinY;
	Int_t   fAdjustMinYButton;
public:

	Set1DimOptDialog(TGWindow * win = NULL);
	~Set1DimOptDialog();
	Set1DimOptDialog(Int_t batch);
//	void Add2ConnectedClasses(TObject *obj);
	void LinLogChanged(TObject* o);
	void CloseDialog();
	void SetHistAttNow(TCanvas *canvas = NULL, Int_t id = 0);
	void SetHistAtt(TCanvas *canvas = NULL, Int_t id = 0);
	void SetAtt(TH1* hist);
	void SetHistAttPermLocal();
	void SetHistAttPerm();
	static void SetDefaults();
	void SaveDefaults();
	void SetAllToDefault();
	void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
	void CloseDown(Int_t wid);
	void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
ClassDef(Set1DimOptDialog,0)
};
#endif

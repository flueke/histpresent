#ifndef GRAPHATTDIALOG
#define GRAPHATTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


class GraphAttDialog : public TObject {

private:

	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TList fGraphList;
	Int_t fNGraphs;

public:
	TString * fDrawOpt;
	TString * fTitle;
	Int_t   * fTitleModBid;
	Style_t * fFillStyle;
	Color_t * fFillColor;
	Style_t * fLineStyle;
	Width_t * fLineWidth;
	Color_t * fLineColor;
	Style_t * fMarkerStyle;
	Size_t  * fMarkerSize;
	Color_t * fMarkerColor;

	TString * fLineMode;
	TString * fErrorMode;
	Float_t fEndErrorSize;
	Float_t fErrorX;
	Int_t   * fShowMarkers;
	Int_t   * fBarChart;
	Int_t   * fFill;
	Int_t   fShowAxis;
	Int_t   fShowTitle;
	Int_t   fLogX;
	Int_t   fLogY;
	Int_t   fLogZ;
	TString fTitleX;
	TString fTitleY;
	
	GraphAttDialog(TGWindow * win = NULL);
   ~GraphAttDialog();
   void CloseDialog();
   void SetGraphAtt(TCanvas *ca = NULL, Int_t bid = 0);
   void SetAsDefault();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(GraphAttDialog,0)
};
#endif

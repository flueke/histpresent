#ifndef ASCII2GRAPHDIALOG
#define ASCII2GRAPHDIALOG
#include "TVirtualPad.h"

#include <iostream>
//_____________________________________________________________________________________

class TGraph;
class TGraph2D;
class HTCanvas;
class TGMrbValuesAndText;
namespace std {} using namespace std;

class Ascii2GraphDialog : public TObject {

private:
   TGMrbValuesAndText *fDialog;
	TGraph *fGraph1D;
	TGraph2D *fGraph2D;
	HTCanvas *fCanvas;
   Int_t   fWinx;
   Int_t   fWiny;
   Int_t	  fDim;
   TString fCommand;
   TString fCommandHead;
   TString fCommandTail;
   TString fCommandSave;
   TString fGraphFileName;
   TString fGraphName;
   TString fGraphXtitle;
   TString fGraphYtitle;
   TString fGraphZtitle;
   Double_t fXaxisMin;
   Double_t fYaxisMin;
   Double_t fZaxisMin;
   Double_t fXaxisMax;
   Double_t fYaxisMax;
   Double_t fZaxisMax;
   Int_t fUseXaxisMax;
   Int_t fUseYaxisMax;
   Int_t fUseZaxisMax;
   Int_t   fMinMaxZero;
   Int_t   fGraphColSelect;
   Int_t   fGraphColSelX;
   Int_t   fGraphColSelY;
   Int_t   fGraphColSelZ;
   Int_t   fGraphColSelEx;
   Int_t   fGraphColSelEy;
   Int_t   fGraphXsize;
   Int_t   fGraphYsize;
   Int_t   fGraphXdiv;
   Int_t   fGraphYdiv;
   Int_t   fEmptyPad;
   Int_t   fGraph_Simple;
   Int_t   fGraph_AsHist;
   Int_t   fGraph_Error;
   Int_t   fGraph_AsymError;
   Int_t   fGraphSerialNr;
   Int_t   fGraphSelPad;
   Int_t   fGraphNewPad;
	Int_t   fGraphShowTitle;
	TString fDrawOpt;
	TString fErrorMode;
	Float_t fEndErrorSize;
	Float_t fErrorX;
	Int_t   fGraphPolyMarker;
   Int_t   fGraphSimpleLine;
	Int_t   fGraphSmoothLine;
	Int_t   fGraphBarChart;
	Int_t   fGraphFill;
	Size_t  fGraphMarkerSize;
   Style_t fGraphMarkerStyle;
   Color_t fGraphMarkerColor;
   Style_t fGraphLineStyle;
   Color_t fGraphLineColor;
   Short_t fGraphLineWidth;
   Style_t fGraphFillStyle;
   Color_t fGraphFillColor;
	Int_t   fGraphLogX;
	Int_t   fGraphLogY;
	Int_t   fGraphLogZ;
	
public:
   Ascii2GraphDialog(TGWindow * win = NULL, Int_t winx = 100, Int_t winy = 100, Int_t dim=1);
   ~Ascii2GraphDialog();
   void Draw_The_Graph();
   void Show_Head_of_File();
   void Show_Tail_of_File();
   Int_t FindGraphs(TVirtualPad * ca, TList * logr, TList * pads);
   void WriteoutGraph();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};
ClassDef(Ascii2GraphDialog,0)
};
#endif

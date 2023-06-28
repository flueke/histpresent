#ifndef MARKERLINEDIALOG
#define MARKERLINEDIALOG
#include "TObject.h"
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
//_____________________________________________________________________________________


class MarkerLineDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas *fCanvas;
	TPad    *fPad;
	Color_t fMarkerColor;
	Size_t  fMarkerSize ;
	Style_t fMarkerStyle;
	Color_t fLineColor  ;
	Width_t fLineWidth   ;
	Style_t fLineStyle  ;
	Color_t fArrowColor  ;
	Float_t fArrowSize   ;
	Int_t   fArrowStyle ;

public:
   MarkerLineDialog();
   virtual ~MarkerLineDialog();
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
   void    Arrow();
   void    SimpleLine();
   void    PolyLine();
   void    Marker();
   void    SaveDefaults();
   void    RestoreDefaults();
//   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
ClassDef(MarkerLineDialog,0)
};
#endif

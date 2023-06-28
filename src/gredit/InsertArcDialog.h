#ifndef INSERTARCDIALOG
#define INSERTARCDIALOG
#include "TObject.h"
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
//_____________________________________________________________________________________


class InsertArcDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TPad       * fPad;
	Double_t   fXcenter;    // center X
	Double_t   fYcenter;    // center Y
	Double_t   fX1;         // 1. point on circumference
	Double_t   fY1;         //
	Double_t   fX2;         // 2. point on circumference
	Double_t   fY2;         //
	Double_t   fR1;          // radius
	Double_t   fR2;          // radius2 / radius1 of ellipse
	Color_t    fColor;   	// line color
	Width_t    fWidth;  	   // line width
	Style_t    fStyle;      // line style
	Color_t    fFillColor;  // fill color
	Style_t    fFillStyle;  // fill style
	Double_t   fPhi1; 		// Phi1 angle
	Double_t   fPhi2; 		// Phi2 angle
	Int_t      fSense;      // + counter clock wise, -1 clock wise
	Int_t      fShowEdges;  // for none full circles, show edges or not
	Int_t      fKeepRadius; //
public:
	InsertArcDialog();
	virtual ~InsertArcDialog();
	void    ArcByCenterAndRadius();
	void    ArcByPointsOnCF();
	Bool_t  PoCftoCenterPhi(Double_t x1, Double_t y1,
							  Double_t x2, Double_t y2, Double_t r, Int_t sense,
							  Double_t *xcenter, Double_t *ycenter,
							  Double_t *phimin, Double_t *phimax);
	Double_t GetRatioXY();
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(InsertArcDialog,0)
};
#endif

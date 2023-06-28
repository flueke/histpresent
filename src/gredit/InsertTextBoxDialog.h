#ifndef INSERTTEXTBOXDIALOG
#define INSERTTEXTBOXDIALOG
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
#include "TObject.h"
//_____________________________________________________________________________________


class InsertTextBoxDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TPad       * fPad;
	Double_t   fX1;         //
	Double_t   fY1;         // 
	Double_t   fDx;         //
	Double_t   fDy;         // 
	Color_t    fColor;      // line color 
	Width_t    fWidth;  	   // line width
	Style_t    fStyle;      // line style
	Color_t    fFillColor;  // fill color 
	Style_t    fFillStyle;  // fill style
	Double_t   fCornerRadius;
	Int_t      fRoundCorners;
	Int_t      fShowShadow;
	Int_t      fBorderSize;          // Size of Shadow
	Short_t    fShadowPosition;     // convention like in text align
	Int_t      fNoAlign;
	Int_t      fWeakAlign;
	Int_t      fStrongAlign;
	Int_t      fNpaves;
public:
	InsertTextBoxDialog();
	virtual ~InsertTextBoxDialog();
	void    ExecuteInsert();
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(InsertTextBoxDialog,0)
};
#endif

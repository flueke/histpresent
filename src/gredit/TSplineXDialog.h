#ifndef TSPLINEXDIALOG
#define TSPLINEXDIALOG
#include "Gtypes.h"
#include "TGWindow.h"
#include "TObject.h"
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class TSplineXDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas  *fCanvas;
	TPad     *fPad;
	TString   fCommand;
	Int_t    fClosed;  
	Int_t    fApprox; 
	Int_t 	fFixends; 
	Double_t fPrec;    
	Int_t    fShowcp;  
	Color_t  fColor;   
	Width_t  fLwidth;  
	Style_t  fLstyle;  
	Color_t  fFcolor;  
	Style_t  fFstyle; 
	Int_t		fDoFill; 
	TString	fRailOption;
	Double_t fSleeperLength;		  
	Double_t fSleeperDist; 		  
	Width_t  fSleeperWidth; 		  
	Color_t  fSleeperColor;   
	Double_t fGage;  		  
	Int_t    fArrow_at_start;
	Int_t    fArrow_at_end;  
	Int_t    fArrow_filled;  
	Double_t fArrow_size;    
	Double_t fArrow_angle;   
	Double_t fArrow_indent_angle;
public:
	TSplineXDialog();
	virtual ~TSplineXDialog();
	void Draw_The_TSplineX();
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
	void SaveDefaults();
	void RestoreDefaults();
	void CloseDialog();
	void CloseDown(Int_t wid);
	void CRButtonPressed(Int_t, Int_t, TObject *){};

ClassDef(TSplineXDialog,0)
};
#endif

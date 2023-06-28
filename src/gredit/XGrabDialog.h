#ifndef XGRABDIALOG
#define XGRABDIALOG
#include "TGMrbValuesAndText.h"
#include "TRootCanvas.h"
#include "GrCanvas.h"
#include "TObject.h"
//_____________________________________________________________________________________


class XGrabDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TPad         *fPad;
	TRootCanvas  *fWindow; 
	Int_t  fFixedSize;
	Short_t fAlign;
	Int_t fDelay;
	Int_t fXwidth;
	Int_t fYwidth;
	Int_t fSerNr;
	Int_t fQuery_ok;
	TString fPname;
	TString fPext;
public:
	XGrabDialog();
	virtual ~XGrabDialog();
	void    ExecuteInsert();
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(XGrabDialog,0)
};
#endif

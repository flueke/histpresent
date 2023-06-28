#ifndef INSERTFUNCTIONDIALOG
#define INSERTFUNCTIONDIALOG
#include "TObject.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class InsertFunctionDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas  *fCanvas;
//	TPad     *fPad;
	TString  *fTpointer;
	Double_t fPar[10];
	Int_t 	fNpar;
	TString  fName;
	TString  fXtitle;
	TString  fYtitle;
	Double_t fFrom;
	Double_t fTo;
	Color_t  fCol;
	Int_t    fPadOpacity;
	Int_t    fNew_canvas;

public:
	InsertFunctionDialog();
	virtual ~InsertFunctionDialog();
	void InsertFunctionExecute();
//	void    ObjCreated(Int_t px, Int_t py, TObject *obj) {fPad = (TPad*)obj;};
//	void    ObjMoved(Int_t px, Int_t py, TObject *obj) { };
	void SaveDefaults();
	void RestoreDefaults();
	void CloseDialog();
	void CloseDown(Int_t wid);
	void IncrementIndex(TString * arg);
	Int_t GetFunctionPad(TPad *ipad = NULL);
	void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(InsertFunctionDialog,0)
};
#endif

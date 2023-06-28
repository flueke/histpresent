#ifndef INSERTHISTRGRAPHDIALOG
#define INSERTHISTRGRAPHDIALOG
#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TObject.h"
#include "TString.h"
//_____________________________________________________________________________________


class InsertHistGraphDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TRootCanvas  *fWindow;
	Int_t        fWhat;
	TString      fHistFromFile;
	Double_t     fScale;
	TString      fFileName;
	TString      fDrawopt;

public:
	InsertHistGraphDialog(Int_t what = 0);
	virtual ~InsertHistGraphDialog();
	void    ExecuteInsert();
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(InsertHistGraphDialog,0)
};
#endif

#ifndef WHATTOSHOWDIALOG
#define WHATTOSHOWDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class TH1;

const Int_t kNopts = 9;

class WhatToShowDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TH1          *fHist;
	TString fHelptextBr;
	Int_t fOpts[kNopts];
	Int_t fOptStat;
	Int_t fShowDateBox;
	Int_t fShowStatBox;
	Int_t fUseTimeOfDisplay;
	Int_t fShowTitle;
	Int_t fShowFitBox;
	Int_t fLegoSuppressZero;
	
public:

	WhatToShowDialog(TGWindow * win = NULL);
//	WhatToShowDialog();
	~WhatToShowDialog();
	void CloseDialog();
	void SetWhatToShowNow(TCanvas *canvas);
	static void SetDefaults();
	void SaveDefaults();
	void RestoreDefaults();
	void CloseDown(Int_t wid);
	void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(WhatToShowDialog,0)
};
#endif

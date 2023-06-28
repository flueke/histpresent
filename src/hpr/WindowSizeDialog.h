#ifndef WINDOWSIZEDIALOG
#define WINDOWSIZEDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


class WindowSizeDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
public:
	static Int_t fMainWidth;
	static Int_t fNwindows;
	static Int_t fWintopx;     // origin of window
	static Int_t fWintopy;
	static Int_t fWincury;
	static Int_t fWincurx;
	static Int_t fWinwidx_2dim;
	static Int_t fWinwidy_2dim;
	static Int_t fWinwidx_1dim;
	static Int_t fWinwidy_1dim;
	static Int_t fWinshiftx;
	static Int_t fWinshifty;
	static Int_t fWinwidx_hlist;
	static Int_t fEntryHeight;
	static Double_t fProjectBothRatio;

	WindowSizeDialog(TGWindow * win = NULL);
//#if ROOTVERSION < 60000
//	WindowSizeDialog() {};
//#endif
	~WindowSizeDialog();
	void CloseDialog();
	void SaveDefaults();
	static void RestoreDefaults();
	void CloseDown(Int_t wid);
	void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(WindowSizeDialog,0)
};
#endif

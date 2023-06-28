#ifndef GENERALATTDIALOG
#define GENERALATTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class GeneralAttDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	Int_t        fGlobalStyleButton;
	Int_t        fAdjustMinYButton;
public:
	static Int_t fPrependFilenameToName;
	static Int_t fPrependFilenameToTitle;
	static Int_t fAppendTimestampToHistname;
   static Int_t fForceStyle;
   static Int_t fShowPSFile;
   static Int_t fSuppressWarnings;
   static Int_t fUseRegexp;
   static Int_t fShowListsOnly;
   static Int_t fRememberLastSet;
   static Int_t fRememberZoom;
   static Int_t fRememberTitle;
   static Int_t fRememberStatBox;
	static Int_t fRememberLegendBox;
	static Int_t fUseAttributeMacro;
   static Int_t fMaxListEntries;
   static Int_t fMaxFileListEntries;
   static Int_t fSkipFirst;
   static Int_t fContentLowLimit;
   static Int_t fVertAdjustLimit;
   static Int_t fAdjustMinY;
	static Int_t fScaleStack;
   static Int_t fStackedReally;
   static Int_t fStackedNostack;
   static Int_t fStackedPads;
	static Int_t fLegoSuppressZero;
	static TString fGlobalStyle;

   GeneralAttDialog(TGWindow * win = NULL);
   ~GeneralAttDialog();
   void CloseDialog();
	void RestoreSavedSettings();
	static void GetCustomStyles();
	void SetSelectionMask();
   static void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(GeneralAttDialog,0)
};
#endif

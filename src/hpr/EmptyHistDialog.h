#ifndef EMPTYHISTDIALOG
#define EMPTYHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMenu.h"
#include "TString.h"
#include "FitOneDimDialog.h"
#include "Save2FileDialog.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class HTCanvas;
class EmptyHistDialog : public TObject {

private:
   HTCanvas *fCanvas;
//   FitOneDimDialog * fDialog;
 //  Save2FileDialog * fSave2FileDialog;
   TH1D    *fHist;
   Int_t   fWinx;
   Int_t   fWiny;
   TString fCommand;
   TString fHistName;
   TString fHistTitle;
	TString fHistXtitle;
	TString fHistYtitle;
   Double_t fXaxisMin;
   Double_t fYaxisMin;
   Double_t fXaxisMax;
   Double_t fYaxisMax;
   Int_t   fNbins;
   Int_t   fHistXsize;
   Int_t   fHistYsize;  
   Int_t   fHistXdiv;   
   Int_t   fHistYdiv;   
   Int_t   fHistSelPad;   
   Int_t   fHistNewPad;   
	Int_t   fUseForGraph;   
	TRootCanvas *fRootCanvas;
   TGPopupMenu *fMenu;
	TGPopupMenu *fAttrMenu;
	
public:
//#if ROOTVERSION > 60000
 //  EmptyHistDialog() {fCanvas = NULL;};
//#endif
   EmptyHistDialog(TGWindow * win = NULL, Int_t winx = 100, Int_t winy = 100);
   ~EmptyHistDialog();
   void RecursiveRemove(TObject*);
   void Draw_The_Hist();
//   void BuildMenu();
//   void HandleMenu(Int_t id);
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t, Int_t, TObject*) {};

ClassDef(EmptyHistDialog,0)
};
#endif

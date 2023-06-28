#ifndef SAVE2FILEDIALOG
#define SAVE2FILEDIALOG
#include "TString.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class Save2FileDialog : public TObject {

private:
   TObject * fObject;
   TList * fList;
   TList fNames;
   TList fTitles;
   TList fNamesSave;
   TString fCommand;
   TString fObjName;
   TString fOrigName;
   TString fFileName;
   TString fDir;
   Int_t   fKeepDialog;
   Int_t   fAsList;
   Int_t   fRestoreNames;
   Int_t   fCanceled;
   TGMrbValuesAndText * fDialog;

public:
   Save2FileDialog(TObject *obj, const char *lname = NULL, TRootCanvas *win = NULL);
   ~Save2FileDialog();
   void ExecuteSave();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid = 0);
   void CloseDialog();
   void CRButtonPressed(Int_t, Int_t, TObject *){};
   const char * GetObjName(){return fObjName.Data();};
	Int_t Canceled() { return fCanceled; };
ClassDef(Save2FileDialog,0)
};
#endif

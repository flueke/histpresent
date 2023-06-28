#ifndef ASCII2NTUPLEDIALOG
#define ASCII2NTUPLEDIALOG
#include "TNtupleD.h"
#include "TString.h"
#include "TGWindow.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class Ascii2NtupleDialog : public TObject {

private:
   TString fCommand;
   TString fCommandHead;
   TString fInputFileName;
   TString fOutputFileName;
   TString fNtupleName;
   TString fNtupleTitle;
   TString fListOfVars;
 
public:
   Ascii2NtupleDialog(TGWindow * win = NULL);
   ~Ascii2NtupleDialog();
   void Fill_The_Ntuple();
   void Show_Head_of_File();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t widgetId, Int_t buttonId, TObject *obj);
ClassDef(Ascii2NtupleDialog,0)
};
#endif

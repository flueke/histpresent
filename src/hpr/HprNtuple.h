#ifndef HPRNTUPLE
#define HPRNTUPLE
#include "TNtupleD.h"
#include "TString.h"
#include "TGWindow.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class HistPresent;

class HprNtuple : public TObject {

private:
   HistPresent *fHistPresent;
   TString fCommand;
   TString fCommandHead;
   TString fInputFileName;
   TString fOutputFileName;
   TString fNtupleName;
   TString fNtupleTitle;
   TString fListOfVars;
 
public:
   HprNtuple(HistPresent * hpr = NULL, TGWindow * win = NULL);
   ~HprNtuple();
   void Fill_The_Ntuple();
   void Show_Head_of_File();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);

ClassDef(HprNtuple,0)
};
#endif

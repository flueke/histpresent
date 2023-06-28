#include "TEnv.h"
#include "TFile.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TGMrbValuesAndText.h"
#include "Ascii2NtupleDialog.h"
#include "hprbase.h"

ClassImp(Ascii2NtupleDialog)

Ascii2NtupleDialog::Ascii2NtupleDialog(TGWindow * win)
{
   const Char_t helpText[] = "Read values from ASCII file and fill ntuple \n\
Values in variable list must ne separated by colons (:)";


   static void *valp[50];
   Int_t ind = 0;
//   Bool_t ok = kTRUE;
   fCommand = "Fill_The_Ntuple()";
   fCommandHead = "Show_Head_of_File()";
   RestoreDefaults();
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("FileRequest_Name of Inputfile"));
   row_lab->Add(new TObjString("StringValue_Name of Outputfile"));
   row_lab->Add(new TObjString("StringValue_Name of Ntuple"));
   row_lab->Add(new TObjString("StringValue_Title of Ntuple"));
   row_lab->Add(new TObjString("StringValue_List of Variables"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt_Fill_The_Ntuple"));
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();

   valp[ind++] = &fInputFileName;
   valp[ind++] = &fOutputFileName;
   valp[ind++] = &fNtupleName;
   valp[ind++] = &fNtupleTitle;
   valp[ind++] = &fListOfVars;
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fCommand;

   GetStringExt("Define parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};
//_________________________________________________________________________

Ascii2NtupleDialog::~Ascii2NtupleDialog()
{
   SaveDefaults();
};
//_________________________________________________________________________

void Ascii2NtupleDialog::Fill_The_Ntuple()
{
   if (gSystem->AccessPathName((const char *) fInputFileName, kFileExists)) {
      cout << "No such file: " << fInputFileName << endl;
      return;
   }
   TFile * outf = new TFile(fOutputFileName, "RECREATE");
   TNtupleD * nt = new TNtupleD(fNtupleName, fNtupleTitle, fListOfVars);
   Long64_t n = nt->ReadFile(fInputFileName);
   nt->Write();
   outf->Close();
   cout << "Rows read: " << n << endl;
   return;
};
//_________________________________________________________________________

void Ascii2NtupleDialog::SaveDefaults()
{
   cout << "Ascii2NtupleDialog::SaveDefaults() " << endl;
   TEnv env(".hprrc");
	env.SetValue("Ascii2NtupleDialog.InputFileName"  , fInputFileName );
	env.SetValue("Ascii2NtupleDialog.OutputFileName" , fOutputFileName);
	env.SetValue("Ascii2NtupleDialog.NtupleName"  	, fNtupleName    );
	env.SetValue("Ascii2NtupleDialog.NtupleTitle" 	, fNtupleTitle   );
	env.SetValue("Ascii2NtupleDialog.ListOfVars"  	, fListOfVars    );
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Ascii2NtupleDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
	fInputFileName  = env.GetValue("Ascii2NtupleDialog.InputFileName"  ,"nt.asc");
	fOutputFileName = env.GetValue("Ascii2NtupleDialog.OutputFileName" ,"nt.root");
	fNtupleName 	 = env.GetValue("Ascii2NtupleDialog.NtupleName"  	,"nt0");
	fNtupleTitle	 = env.GetValue("Ascii2NtupleDialog.NtupleTitle" 	,"My Ntuples");
	fListOfVars 	 = env.GetValue("Ascii2NtupleDialog.ListOfVars"  	,"x:y:z");
}
//_________________________________________________________________________

void Ascii2NtupleDialog::Show_Head_of_File()
{
   TString cmd(fInputFileName.Data());
   cmd.Prepend("head ");
//   cmd.Append("\")");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2NtupleDialog::CloseDown(Int_t /*wid*/)
{
   cout << "Ascii2NtupleDialog::CloseDown()" << endl;
   delete this;
}
//_________________________________________________________________________

void Ascii2NtupleDialog::CRButtonPressed(Int_t /*widgetId*/, Int_t /*buttonId*/, TObject */*obj*/)
{
}

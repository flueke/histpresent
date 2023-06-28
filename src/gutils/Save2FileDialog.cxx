#include "TROOT.h"
#include "TCanvas.h"
#include "TCanvasImp.h"
#include "TEnv.h"
#include "TFile.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TRootCanvas.h"
#include "TSystem.h"
#include "TH1.h"
//#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "HTCanvas.h"

ClassImp(Save2FileDialog)

Save2FileDialog::Save2FileDialog(TObject * obj, const char */*lname*/, TRootCanvas *win)
{
   static const Char_t helpText[] =
"Save object(s) to file in current Unix directory.\n\
It can be saved into a subdir in the rootfile.\n\
If this does not exist it will be created. \n\
More than one level of subdirs is allowed\n\
Names may be changed before storing.\n\
Original names may optionally be restored on exit.\n\
";

   static void *valp[50];
   
   Int_t ind = 0;
//   Bool_t ok = kTRUE;
	fCanceled = 0;
   fCommand = "ExecuteSave()";
   Int_t dummy;
   RestoreDefaults();
   SetBit(kMustCleanup);
   fKeepDialog = 0;
   fRestoreNames = 1;
   fAsList = 0;
   fObject = obj;
   fObjName = obj->GetName();
   fOrigName = obj->GetName();
   if (obj->InheritsFrom("TList")) {
      fList = (TList*)obj;
      fObjName = "";
      // get names of objs in list
      TIter next(fList);
      TObject *o;
      while ( (o = next()) ){
			fNames.Add(new TObjString(o->GetName()));
			fTitles.Add(new TObjString(o->GetTitle()));
			fNamesSave.Add(new TObjString(o->GetName()));
		}
   } else {
      fList = NULL;
   }
	TRegexp vers(";[0-9]*");
	fObjName(vers) = "";

   TList *row_lab = new TList();
   row_lab->Add(new TObjString("StringValue_Output root file"));
   valp[ind++] = &fFileName;
   row_lab->Add(new TObjString("StringValue_Dir in root file\
&ROOT files may have directories.\n\
If this name is not blank\n\
object will be saved in this dir.\n\
If not existing it will be created"));
   valp[ind++] = &fDir;
   if (fList == NULL ) {
		TString lab("Save with name  ");
		if (fObject->InheritsFrom("TNamed")){
			lab.Prepend("StringValue_");
			lab+= "&Title: ";
			lab += obj->GetTitle();
			valp[ind++] = &fObjName;
		} else {
			lab = "CommentOnly_";
			lab += obj->ClassName();
			lab += ":Name: ";
			lab += fObjName;
			lab+= "&Title: ";
			lab += obj->GetTitle();
			valp[ind++] = &dummy;
		}
		row_lab->Add(new TObjString(lab));
	} else {
		TString lab;
		Int_t nent = fNames.GetEntries();
		for (Int_t ie = 0; ie<nent; ie++) {
			lab = "StringValue_Ent_";
			lab += ie;
			lab+= "&";
			lab += ((TObjString*)fTitles.At(ie))->String();
			row_lab->Add(new TObjString(lab));
			
			valp[ind++] = &(((TObjString*)fNames.At(ie))->String());
			
		}
	}
	if (fList != NULL || fObject->InheritsFrom("TNamed")){
		row_lab->Add(new TObjString("CheckButton_Restore original names"));
		valp[ind++] = &fRestoreNames;
	}
	/*		
   if (fList) {
      row_lab->Add(new TObjString("CheckButton_Write functions as list"));
      valp[ind++] = &fAsList;
		row_lab->Add(new TObjString("StringValue_Save list with name"));
		valp[ind++] = &fObjName;
	}
	*/
	Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();

//   valp[ind++] = &fCommand;
   TRootCanvas* window = win;
   if ( window == NULL)
      win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   TString text = obj->ClassName();
   text.Prepend ("Save ");
   text.Append(" to rootfile");
   Int_t retval = -2;   //wait for answer
   fDialog = new TGMrbValuesAndText(text, NULL,
                   &retval, itemwidth, window,
                   NULL, NULL, row_lab, valp,
//                   NULL, NULL, helpText);
                   NULL, NULL, helpText, this, this->ClassName());
//  cout << "retval " <<retval << endl;
   if (retval >= 0) {
      ExecuteSave();
      SaveDefaults();
   } else {
      cout << "Canceled " <<retval << endl;
      fCanceled = 1;
   }
};
//_________________________________________________________________________

Save2FileDialog::~Save2FileDialog()
{
	if (gDebug >0)
		cout << "dtor Save2FileDialog" << endl;
};
//_________________________________________________________________________

void Save2FileDialog::ExecuteSave()
{
   TFile * outfile;
   if (!gSystem->AccessPathName(fFileName, kFileExists))
      outfile = new TFile(fFileName, "UPDATE", "Output file");
   else
      outfile = new TFile(fFileName, "RECREATE", "Output file");
   if (fDir.Length() > 0) {
      TObjArray *oa;
      oa = fDir.Tokenize("/");
      TString ndir;
      for (Int_t i = 0; i < oa->GetEntries(); i++) {
         ndir = ((TObjString*)oa->At(i))->String();
         if ( (!gDirectory->cd(ndir)) ) {
            cout << "Ignore above message, creating dir: " << ndir << endl;
            TDirectory * dir = gDirectory->mkdir(ndir, ndir);
            if (dir) {
               dir->cd();
            } else {
               cout << "Cant create: " << fDir << endl;
               return;
            }
         }
      }
   }
	Int_t name_changed = 0;
   if (fList) {
//		TObject *obj;
		TString temp;
		TIter next((TList*)fList);
		for (Int_t ie=0; ie<fNames.GetEntries(); ie++) {
			if (fList->At(ie)->InheritsFrom("TNamed")) {
				if (((TObjString*)fNames.At(ie))->String() !=
					 ((TObjString*)fNamesSave.At(ie))->String() ) {
					name_changed++;
					((TNamed*)fList->At(ie))->SetName(((TObjString*)fNames.At(ie))->String());
				}
				cout << "Writing: \"" << ((TObjString*)fNamesSave.At(ie))->String()
				<< "\" with name: \"" << fList->At(ie)->GetName()<< "\"" << endl;
			} else {
				cout <<fList->At(ie)->ClassName() << " has no method SetName()" << endl;
			}
		}
      if (fAsList) {
			if (fObjName.Length() == 0) {
				fObjName = "func_list";
			}
			cout << endl 
			<< "Warning: List will not yet be displayed by HistPresent"
			<< endl << "Stored with name " << fObjName 
			<< endl;
         fList->Write(fObjName, 1);
      } else {
         fList->Write();
      }
      outfile->Close();
      if (name_changed && fRestoreNames) {
			for (Int_t ie=0; ie<fNames.GetEntries(); ie++) {
				if (fList->At(ie)->InheritsFrom("TNamed")) {
					((TNamed*)fList->At(ie))->SetName(((TObjString*)fNamesSave.At(ie))->String());
					cout << "Restoring: \"" << ((TObjString*)fNames.At(ie))->String()
					<< "\" with name orig \"" << fList->At(ie)->GetName()<<"\"" << endl;
				}
			}
		}
      return;
   }
	if ( fOrigName != fObjName) {
		if (fObject->InheritsFrom("TNamed")) {
			name_changed++;
			((TNamed*)fObject)->SetName(fObjName);
			cout << "fObject->SetName(fObjName) " << fObjName<< endl;
		} else {
			cout << " Cant change name of " <<fOrigName << " " 
			<< fObject->ClassName() << " has no method SetName()" << endl;
		}
	}
	fObject->Write();
	cout << "Write object with name: " << fObject->GetName() << endl;
	outfile->Close();
	if (fRestoreNames && name_changed) {
//		cout << "objorig->SetName(smame) " << sname << endl;
		((TNamed*)fObject)->SetName(fOrigName);
	}
};
//_________________________________________________________________________

void Save2FileDialog::SaveDefaults()
{
//   cout << "Save2FileDialog::SaveDefaults() " << endl;
   TEnv env(".hprrc");
	env.SetValue("Save2FileDialog.FileName", fFileName);
	env.SetValue("Save2FileDialog.fObjName", fObjName);
	env.SetValue("Save2FileDialog.Dir",      fDir);
	env.SetValue("Save2FileDialog.AsList",   fAsList);
	env.SetValue("Save2FileDialog.fRestoreNames",   fRestoreNames);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Save2FileDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
	fFileName = env.GetValue("Save2FileDialog.FileName", "workfile.root");
	fDir = env.GetValue("Save2FileDialog.Dir", "");
   fAsList = env.GetValue("Save2FileDialog.AsList", 0);
   fRestoreNames= env.GetValue("Save2FileDialog.fRestoreNames", 1);
}

//_________________________________________________________________________

void Save2FileDialog::CloseDown(Int_t /*wid*/)
{
// cout << "Save2FileDialog::CloseDown()" << endl;
//  SaveDefaults();
//  delete this;
}
//_______________________________________________________________________

void Save2FileDialog::CloseDialog()
{
 //  cout << "FitOneDimDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fDialog ) fDialog->CloseWindowExt();
   delete this;
}

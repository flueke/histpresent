#include "TNamedString.h"
#include <fstream>

ClassImp(TNamedString)
//_________________________________________________________________________
//
// This class wraps e TString into a TNamed object, this allows 
// the TString to written to a root file and retrieved by name
//_________________________________________________________________________

TNamedString::TNamedString(const TString & name, const TString & text):
   TNamed(name.Data(),"") {
   fString = text.Data(); 
}
Int_t TNamedString::FromFile(const Char_t * fname){
//_________________________________________________________________________
//
//   fill string from file
//
   ifstream inf(fname);
   if(inf.fail()){
      cerr << "Cant open " << fname << endl;
      return -1;
   }
   fString.ReadFile(inf);
   return fString.Length();
}
//_________________________________________________________________________

Int_t TNamedString::ToFile(const Char_t * fname){
//_________________________________________________________________________
//
//   write  string to file
//
   ofstream of(fname);
   if(of.fail()){
      cerr << "Cant open " << fname << endl;
      return -1;
   }
   of << fString.Data();
   return fString.Length();
}

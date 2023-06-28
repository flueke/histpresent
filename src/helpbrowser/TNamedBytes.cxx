
#include "TSystem.h"
#include <fstream>
#include <iostream>

#include "TNamedBytes.h"

namespace std {} using namespace std;

ClassImp(TNamedBytes)
//_________________________________________________________________________
//
// This class puts a file into a TNamed object, this allows 
// the contents of tke file to written to a root file and retrieved by name
//_________________________________________________________________________
TNamedBytes::TNamedBytes(const Char_t * fname):
   TNamed(gSystem->BaseName(fname), " ") {
   Long_t id, flags, modtime;
   Long64_t size;
   gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
   if(size <= 0){
//      cout << "File: " << fname << " does not exist of empty" << endl;
      fIsGood = kFALSE;
      fBuffer = 0;
   } else {
//      cout << "size " << size << endl;
      fBuffer = new TArrayC(size);
//         Char_t * b = (Char_t *)&fBuffer[0];
      ifstream inf(fname);
      inf.read(fBuffer->fArray,size);
//         inf.close();
      fIsGood = kTRUE;
   }
};
//_________________________________________________________________________

Int_t TNamedBytes::ToFile(const Char_t * fname){
//______________________________________________

// Write buffer contents to a file with name fname
   if(!fBuffer){
      cerr << "Buffer is empty"  << endl;
      return -1;
   }

   ofstream of(fname);
   if(of.fail()){
      cerr << "Cant open " << fname << endl;
      return -2;
   }
//      Char_t * b = (Char_t *)&fBuffer[0];
   of.write(fBuffer->fArray, fBuffer->GetSize());
   of.close();
   return fBuffer->GetSize();
}

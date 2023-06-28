#include "TROOT.h"
#include "TNamed.h"
#include "TString.h"
#include <iostream>

namespace std {} using namespace std;

class TNamedString : public TNamed {

public:
   TNamedString(){}
   TNamedString(const Char_t * name):TNamed(name,"") {}
   TNamedString(const Char_t * name, const Char_t * text):TNamed(name,"") {
     fString = text; 
}
   TNamedString(const TString & name, const TString & text);
   ~TNamedString(){}

   Int_t FromFile(const Char_t * fname);

   Int_t ToFile(const Char_t * fname);

   void SetText(Text_t * text){fString = text;}
   const Text_t * GetText(){return fString.Data();}
   Int_t Length(){return fString.Length();}
   void Print(Option_t * Option) const { TObject::Print(Option); };
//_________________________________________________________________________
   void Print() const {
      cout << fString.Data() << endl;
   }
private:   
   TString fString;           //  the wrapped TString

   ClassDef(TNamedString,1)   // A TNamed object containing a TString
};

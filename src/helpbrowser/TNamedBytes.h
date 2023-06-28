#include "TROOT.h"
#include "TNamed.h"
#include "TArrayC.h"

class TNamedBytes : public TNamed {
private:
   TArrayC * fBuffer;
   Bool_t fIsGood;

public:

   TNamedBytes(){};
   virtual ~TNamedBytes(){if(fBuffer)delete fBuffer;};
   TNamedBytes(const Char_t * fname);

   Bool_t IsGood(){return fIsGood;};
//_
   Int_t ToFile(const Char_t * fname);
   ClassDef(TNamedBytes,1)   // A TNamed object containing a sequence of bytes
};

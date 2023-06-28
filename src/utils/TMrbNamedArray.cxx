#include "TMrbNamedArray.h"

ClassImp(TMrbNamedArrayC)

ClassImp(TMrbNamedArrayI)

ClassImp(TMrbNamedArrayF)

ClassImp(TMrbNamedArrayD)
//__________________________________________________________________________

TMrbNamedArrayC::TMrbNamedArrayC(const char* name, const char* title, 
                const Int_t len, const char* val):
                TNamed(name, title) {if (len > 0 && val) Set(len, val);};

TMrbNamedArrayI::TMrbNamedArrayI(const char* name, const char* title,
                const Int_t len, const Int_t* val):
                TNamed(name, title) {if (len > 0 && val) Set(len, val);};

TMrbNamedArrayF::TMrbNamedArrayF(const char* name, const char* title, 
                const Int_t len, const Float_t * val):
                TNamed(name, title) {if (len > 0 && val) Set(len, val);};

TMrbNamedArrayD::TMrbNamedArrayD(const char* name, const char* title, 
                const Int_t len, const Double_t * val):
                TNamed(name, title) {if (len > 0 && val) Set(len, val);};

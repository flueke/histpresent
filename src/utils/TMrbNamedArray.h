#ifndef __TMrbNamedArray_h__
#define __TMrbNamedArray_h__

#include "TArrayC.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "TArrayD.h"
#include "TNamed.h"

//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedArrayC, I, F, D
// Description:    An array of char, int, float, double with name and title.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class  TMrbNamedArrayC: public TNamed, public TArrayC {
   public:
      TMrbNamedArrayC(){};
      TMrbNamedArrayC(const char* name, const char* title, 
                      const Int_t len = 0, const char* val = 0);
      ~TMrbNamedArrayC(){};

ClassDef(TMrbNamedArrayC, 1)		// [Utils] A named TArrayC
};


class  TMrbNamedArrayI: public TNamed, public TArrayI {
   public:
      TMrbNamedArrayI(){};
      TMrbNamedArrayI(const char* name, const char* title, 
                      const Int_t len = 0, const Int_t * val = 0);
      ~TMrbNamedArrayI(){};

ClassDef(TMrbNamedArrayI, 1)		// [Utils] A named TArrayI
};


class  TMrbNamedArrayF: public TNamed, public TArrayF {
   public:
      TMrbNamedArrayF(){};
      TMrbNamedArrayF(const char* name, const char* title, 
                      const Int_t len = 0, const Float_t * val = 0);
      ~TMrbNamedArrayF(){};

ClassDef(TMrbNamedArrayF, 1)		// [Utils] A named TArrayF
};



class  TMrbNamedArrayD: public TNamed, public TArrayD {
   public:
      TMrbNamedArrayD(){};
      TMrbNamedArrayD(const char* name, const char* title, 
                      const Int_t len = 0, const Double_t * val = 0);
      ~TMrbNamedArrayD(){};

ClassDef(TMrbNamedArrayD, 1)		// [Utils] A named TArrayD
};
#endif


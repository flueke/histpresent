#ifndef __FHCONTOUR_h__
#define __FHCONTOUR_h__

#include "TNamed.h"
#include "TGWindow.h"
#include "TArrayI.h"
#include "TArrayD.h"

namespace std {} using namespace std;


//////////////////////////////////////////////////////////////////////////////
// Name:           FhContour
// Description:    Store a user contour with name and title.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class  FhContour : public TNamed {
   protected:
      TArrayD fLevels;
      TArrayI fColors;
   public:
      FhContour(){};
      FhContour(const char* name, const char* title, const Int_t len = 0, 
                const Double_t * levels = 0, const Int_t * colors = 0);
      virtual ~FhContour(){};
      void Set(Int_t nlevels, const Double_t * levels, const Int_t * colors) {
         fLevels.Set(nlevels, levels); 
         fColors.Set(nlevels, colors);
      }
      TArrayI * GetColorArray() { return &fColors; }
      TArrayD * GetLevelArray() { return &fLevels; }
      void Print(Option_t * Option) const { TObject::Print(Option); }
      void Print();
      Int_t Edit(TGWindow * canvas = NULL);
//      void Draw(){};
    
ClassDef(FhContour,1)
};

#endif


#ifndef THPRARROW
#define THPRARROW
#include "HprElement.h"
#include "TArrow.h"
#include <iostream>

class THprArrow : public TArrow, public HprElement
{
public:
   THprArrow()
   {
      SetDaughter(this);
   };
   THprArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
             Float_t arrowsize=0.05 ,Option_t *option=">");
   ~THprArrow(){};
   void Paint(const Option_t *opt);
//____________________________________________________________________________

   Bool_t IsSortable() const {return kTRUE;}

   Int_t Compare( const TObject *obj) const
   {
      const HprElement* hpre = dynamic_cast<const HprElement*>(obj);
      if (!hpre) return 0;
//      std::cout << ClassName() <<GetPlane() << " " << hpre->GetPlane()<< std::endl;
      if (GetPlane()== hpre->GetPlane())
         return 0;
      if (GetPlane() > hpre->GetPlane())
         return 1;
      else
         return -1;
   }
// #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
//   void SavePrimitive(ostream &, Option_t *);
// #else
//   void SavePrimitive(ofstream &, Option_t *);
// #endif
ClassDef(THprArrow,1)
};
#endif

#ifndef THPRELLIPSE
#define THPRELLIPSE
#include "HprElement.h"
#include "TEllipse.h"

class THprEllipse : public TEllipse, public HprElement
{
public:
   THprEllipse(){ SetDaughter(this);};
   THprEllipse(Double_t x1, Double_t y1, Double_t r1, Double_t r2 =0,
             Double_t phimin=0, Double_t phimax=360,
             Double_t theta=0);
   ~THprEllipse(){};
   void Paint(const Option_t *opt);
   Bool_t IsSortable() const {return kTRUE;}
   Int_t Compare( const TObject *obj) const
   {
      const HprElement* hpre = dynamic_cast<const HprElement*>(obj);
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
ClassDef(THprEllipse,1)
};
#endif

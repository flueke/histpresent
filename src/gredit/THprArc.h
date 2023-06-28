#ifndef THPRARC
#define THPRARC
#include "HprElement.h"
#include "TArc.h"

class THprArc : public TArc, public HprElement
{
public:
   THprArc(){ SetDaughter(this);};
   THprArc(Double_t x1, Double_t y1, Double_t rad,
             Double_t phimin=0, Double_t phimax=360);
   ~THprArc(){};
   void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
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
ClassDef(THprArc,1)
};
#endif

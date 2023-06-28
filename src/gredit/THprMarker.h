#ifndef THPRMARKER
#define THPRMARKER
#include "HprElement.h"
#include "TMarker.h"

class THprMarker : public TMarker, public HprElement
{
public:
   THprMarker(){ SetDaughter(this);};
   THprMarker(Double_t x, Double_t y, Style_t style = 2);
   ~THprMarker(){};
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
ClassDef(THprMarker,1)
};
#endif

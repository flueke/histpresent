#ifndef THPRGTRAPH
#define THPRGTRAPH
#include "HprElement.h"
#include "TSplineX.h"

class THprGraph : public TGraph, public HprElement
{
public:
   THprGraph(){ SetDaughter(this);};
   THprGraph(Int_t npoints, Double_t *x, Double_t *y);
   ~THprGraph(){};
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
ClassDef(THprGraph,1)
};
#endif

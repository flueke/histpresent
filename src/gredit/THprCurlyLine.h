#ifndef THPRCURLYLINE
#define THPRCURLYLINE
#include "HprElement.h"
#include "TCurlyLine.h"

class THprCurlyLine : public TCurlyLine, public HprElement
{
public:
   THprCurlyLine(){ SetDaughter(this);};
   THprCurlyLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                  Double_t wl = .02, Double_t amp = .01);
   ~THprCurlyLine(){};
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
ClassDef(THprCurlyLine,1)
};
#endif

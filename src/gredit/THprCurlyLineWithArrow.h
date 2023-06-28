#ifndef THPRCURLYLINEWITHARROW
#define THPRCURLYLINEWITHARROW
#include "HprElement.h"
#include "TCurlyLineWithArrow.h"


class THprCurlyLineWithArrow : public  TCurlyLineWithArrow,
                               public  HprElement
{

public:
   THprCurlyLineWithArrow(){ SetDaughter(this);};
   THprCurlyLineWithArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
              Double_t wl = .02, Double_t amp = .01,
              Int_t where = 2, Float_t arrowsize=0);
   ~THprCurlyLineWithArrow(){};
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
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(std::ostream &, Option_t *);
 #else
   void SavePrimitive(std::ofstream &, Option_t *);
 #endif
ClassDef(THprCurlyLineWithArrow,1)
};
#endif

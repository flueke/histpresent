#ifndef THPRGaxis
#define THPRGaxis
#include "HprElement.h"
#include "TGaxis.h"

class THprGaxis : public TGaxis, public HprElement
{
public:
   THprGaxis(){ SetDaughter(this);};
   THprGaxis(Double_t xmin,Double_t ymin,Double_t xmax,Double_t ymax,
          Double_t wmin,Double_t wmax,Int_t ndiv=510, Option_t *chopt="",
          Double_t gridlength = 0);
   virtual ~THprGaxis(){};
   void Paint(const Option_t *opt);

   Int_t Compare( const TObject *obj) const
   {
      const HprElement* hpre = dynamic_cast<const HprElement*>(obj);
      std::cout << ClassName() <<GetPlane() << " " << hpre->GetPlane()<< std::endl;
      if (GetPlane()== hpre->GetPlane())
         return 0;
      if (GetPlane() > hpre->GetPlane())
         return 1;
      else
         return -1;
   }
ClassDef(THprGaxis,1)
};
#endif

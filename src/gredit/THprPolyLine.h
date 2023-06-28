#ifndef THPRPOLYLINE
#define THPRPOLYLINE
#include "HprElement.h"
#include "TPolyLine.h"

class THprPolyLine : public TPolyLine, public HprElement
{
public:
   THprPolyLine(){ SetDaughter(this);};
   THprPolyLine(Int_t np, Option_t* option = "");
   THprPolyLine(Int_t np, Double_t *x, Double_t* y, Option_t* option = "");
   ~THprPolyLine(){};
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
ClassDef(THprPolyLine,1)
};
#endif

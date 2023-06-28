#ifndef THPRLINE
#define THPRLINE
#include "HprElement.h"
#include "TLine.h"

class THprLine : public TLine, public HprElement
{
public:
   THprLine(){ SetDaughter(this);};
   THprLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   ~THprLine(){};
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
ClassDef(THprLine,1)
};
#endif

#ifndef THPRLATEX
#define THPRLATEX
#include "HprElement.h"
#include "TLatex.h"

class THprLatex : public TLatex, public HprElement
{
public:
      // TLatex status bits
    enum { kValignNoShift = BIT(16) };
   THprLatex(){ SetDaughter(this);};
   THprLatex(Double_t x, Double_t y, const Char_t * text);
   ~THprLatex(){};
//   THprLatex       *DrawLatex(Double_t x, Double_t y, const char *text);
//   virtual void    PaintLatex(Double_t x, Double_t y, Double_t angle, Double_t size, const char *text);
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
ClassDef(THprLatex,1)
};
#endif

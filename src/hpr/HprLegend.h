#ifndef THPRLEGEND
#define THPRLEGEND
#include "TLegend.h"
#include "TLegendEntry.h"

class HprLegend : public TLegend
{
public:
   HprLegend(Double_t x1, Double_t y1,Double_t x2, Double_t y2,
                  const char *header, Option_t *option);
   ~HprLegend(){};
   void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
ClassDef(HprLegend,1)
};
#endif

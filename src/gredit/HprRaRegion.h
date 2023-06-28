#ifndef HPRRAREGION
#define HPRRAREGION
#include "TBox.h"
#include "TCutG.h"

class HprRaRegion : public TBox
{
private:
   TCutG  *fCutG;
public:
   HprRaRegion(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   ~HprRaRegion();
   TCutG *CutG();
//   const char *GetName() { return "CUTG";};
ClassDef(HprRaRegion,0)
};
#endif

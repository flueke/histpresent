#include "TROOT.h"
#include "Riostream.h"
#include "THprArrow.h"
#include "GrCanvas.h"

ClassImp(THprArrow)

//________________________________________________________________________

THprArrow::THprArrow(
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           Float_t arrowsize ,Option_t *option)
           : TArrow(x1, y1, x2, y2, arrowsize, option)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprArrow::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetX2(htc->PutOnGridX(GetX2()));
      SetY1(htc->PutOnGridY(GetY1()));
      SetY2(htc->PutOnGridY(GetY2()));
//      cout << "THprArrow htc->PutOnGridY(GetY2() " <<  htc->PutOnGridY(GetY2())<< endl;
   }
   if (GetVisibility() == 1)
      TArrow::Paint(opt);
}

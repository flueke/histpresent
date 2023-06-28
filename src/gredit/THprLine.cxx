#include "TROOT.h"
#include "Riostream.h"
#include "THprLine.h"
#include "GrCanvas.h"

ClassImp(THprLine)

//________________________________________________________________________

THprLine::THprLine(
           Double_t x1, Double_t y1, Double_t x2, Double_t y2)
           : TLine(x1, y1, x2, y2)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprLine::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetX2(htc->PutOnGridX(GetX2()));
      SetY1(htc->PutOnGridY(GetY1()));
      SetY2(htc->PutOnGridY(GetY2()));
//      cout << "THprLine htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TLine::Paint(opt);
}

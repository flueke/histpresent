#include "TROOT.h"
#include "Riostream.h"
#include "THprCurlyLine.h"
#include "GrCanvas.h"

ClassImp(THprCurlyLine)

//________________________________________________________________________

THprCurlyLine::THprCurlyLine(
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           Double_t wl, Double_t amp)
           : TCurlyLine(x1, y1, x2, y2, wl, amp)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprCurlyLine::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetStartPoint(htc->PutOnGridX(GetStartX()),
      htc->PutOnGridY(GetStartY()));
      SetEndPoint(htc->PutOnGridX(GetEndX()),
      htc->PutOnGridY(GetEndY()));
      Build();
//      cout << "THprCurlyLine htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TCurlyLine::Paint(opt);
}

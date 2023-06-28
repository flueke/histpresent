#include "TROOT.h"
#include "Riostream.h"
#include "THprCurlyArc.h"
#include "GrCanvas.h"

ClassImp(THprCurlyArc)

//________________________________________________________________________

THprCurlyArc::THprCurlyArc(Double_t x1, Double_t y1, Double_t rad,
             Double_t phimin, Double_t phimax,
             Double_t wl, Double_t amp)
           : TCurlyArc(x1, y1, rad, phimin, phimax, wl, amp)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprCurlyArc::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetCenter(htc->PutOnGridX(GetStartX()),
      htc->PutOnGridY(GetStartY()));
      Build();
   }
   if (GetVisibility() == 1)
      TCurlyArc::Paint(opt);
}

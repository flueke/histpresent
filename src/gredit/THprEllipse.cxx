#include "TROOT.h"
#include "Riostream.h"
#include "THprEllipse.h"
#include "GrCanvas.h"

ClassImp(THprEllipse)

//________________________________________________________________________

THprEllipse::THprEllipse(Double_t x1, Double_t y1, Double_t r1, Double_t r2,
             Double_t phimin, Double_t phimax, Double_t theta)
           : TEllipse(x1, y1, r1, r2, phimin, phimax, theta)
{
}
//________________________________________________________________________

void THprEllipse::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetY1(htc->PutOnGridY(GetY1()));
   }
   if (GetVisibility() == 1)
      TEllipse::Paint(opt);
}

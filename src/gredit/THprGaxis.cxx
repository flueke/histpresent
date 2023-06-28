#include "TROOT.h"
#include "Riostream.h"
#include "THprGaxis.h"
#include "GrCanvas.h"

ClassImp(THprGaxis)

//________________________________________________________________________

THprGaxis::THprGaxis(
          Double_t xmin,Double_t ymin,Double_t xmax,Double_t ymax,
          Double_t wmin,Double_t wmax,Int_t ndiv, Option_t *chopt,
          Double_t gridlength) :
          TGaxis(xmin, ymin, xmax, ymax, wmin, wmax, ndiv, chopt, gridlength)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprGaxis::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetX2(htc->PutOnGridX(GetX2()));
      SetY1(htc->PutOnGridY(GetY1()));
      SetY2(htc->PutOnGridY(GetY2()));
//      cout << "THprGaxis htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TGaxis::Paint(opt);
}

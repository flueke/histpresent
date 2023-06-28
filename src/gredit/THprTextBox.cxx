#include "TROOT.h"
#include "Riostream.h"
#include "THprTextBox.h"
#include "GrCanvas.h"

ClassImp(THprTextBox)

//________________________________________________________________________

THprTextBox::THprTextBox(
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           const Char_t *name)
           : TextBox(x1, y1, x2, y2, name)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprTextBox::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetX2(htc->PutOnGridX(GetX2()));
      SetY1(htc->PutOnGridY(GetY1()));
      SetY2(htc->PutOnGridY(GetY2()));
		SetPaveNDC(this);
   }
   if (GetVisibility() == 1)
      TextBox::Paint(opt);
}

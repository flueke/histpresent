#include "TROOT.h"
#include "Riostream.h"
#include "THprPolyLine.h"
#include "GrCanvas.h"

ClassImp(THprPolyLine)

//________________________________________________________________________

THprPolyLine::THprPolyLine(Int_t np, Option_t* option)
           : TPolyLine(np, option)
{
   SetDaughter(this);
}
//________________________________________________________________________

THprPolyLine::THprPolyLine(Int_t np,
           Double_t *x, Double_t* y, Option_t* option)
           : TPolyLine(np, x, y, option)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprPolyLine::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
		Double_t *px = GetX();
		Double_t *py = GetY();
		for (Int_t i = 0; i < GetN(); i ++) {
			SetPoint(i, htc->PutOnGridX(px[i]), htc->PutOnGridY(py[i]));
		}
//      cout << "THprPolyLine htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TPolyLine::Paint(opt);
}

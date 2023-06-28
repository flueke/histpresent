#include "TROOT.h"
#include "Riostream.h"
#include "THprGraph.h"
#include "GrCanvas.h"

ClassImp(THprGraph)
//________________________________________________________________________

THprGraph::THprGraph(Int_t npoints, Double_t *x, Double_t *y)
           : TGraph(npoints, x, y)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprGraph::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      Double_t x, y;
      for (Int_t ip = 0; ip < GetN(); ip++) {
         GetPoint(ip, x, y);
         SetPoint(ip, htc->PutOnGridX(x), htc->PutOnGridY(y));
      }
//      cout << "THprGraph htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TGraph::Paint(opt);
}

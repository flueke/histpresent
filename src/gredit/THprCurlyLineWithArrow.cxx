#include "TROOT.h"
#include "Riostream.h"
#include "THprCurlyLineWithArrow.h"
#include "GrCanvas.h"

ClassImp(THprCurlyLineWithArrow)

//________________________________________________________________________

THprCurlyLineWithArrow::THprCurlyLineWithArrow(
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           Double_t wl, Double_t amp, Int_t where, Float_t arrowsize)
           : TCurlyLineWithArrow(x1, y1, x2, y2, wl, amp, where,arrowsize)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprCurlyLineWithArrow::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX1(htc->PutOnGridX(GetX1()));
      SetY1(htc->PutOnGridY(GetY1()));
      SetX2(htc->PutOnGridX(GetX2()));
      SetY2(htc->PutOnGridY(GetY2()));
//      cout << "htc->PutOnGridY(GetY2() " <<  htc->PutOnGridY(GetY2())<< endl;
   }
   if (GetVisibility() == 1)
      TCurlyLineWithArrow::Paint(opt);
}
//___________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void THprCurlyLineWithArrow::SavePrimitive(ostream &out, Option_t *)
#else
void THprCurlyLineWithArrow::SavePrimitive(ofstream &out, Option_t *)
#endif
{
   // Save primitive as a C++ statement(s) on output stream out

   cout << "THprCurlyLineWithArrow::SavePrimitive: " << this << endl;
   out<<"   "<<endl;
   if (gROOT->ClassSaved(THprCurlyLineWithArrow::Class())) {
      out<<"   ";
   } else {
      out<<"    THprCurlyLineWithArrow *";
   }

   out<<"clwa = new THprCurlyLineWithArrow("<< GetStartX()  <<","<< GetStartY() <<","
       << GetEndX() <<","<< GetEndY() <<"," << GetWaveLength()  <<","<< GetAmplitude()<<","
       << GetWhere()<<","<<GetDefaultArrowSize();
          out << ");"<<endl;

   SaveLineAttributes(out,"clwa",0,0,0);
   out<<"clwa->SetPlane(" << GetPlane() <<  ");"<<endl;
   out<<"clwa->Draw();"<<endl;
   if(GetArrowStart()) {
      out<<"clwa->GetArrowStart()->SetArrowSize(" <<
         GetArrowStart()->GetArrowSize()<< ");"<<endl;
      out<<"clwa->GetArrowStart()->SetAngle(" <<
         GetArrowStart()->GetAngle()<< ");"<<endl;
   }
   if(GetArrowEnd()) {
      out<<"clwa->GetArrowEnd()->SetArrowSize(" <<
         GetArrowEnd()->GetArrowSize()<< ");"<<endl;
      out<<"clwa->GetArrowEnd()->SetAngle(" <<
         GetArrowEnd()->GetAngle()<< ");"<<endl;
   }
}

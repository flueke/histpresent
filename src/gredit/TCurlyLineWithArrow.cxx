#include "Riostream.h"
#include "TCurlyLineWithArrow.h"
#include "TPad.h"
#include "TROOT.h"
#include "TMath.h"
#include "iostream"
ClassImp(TCurlyLineArrow)
ClassImp(TCurlyLineWithArrow)


TCurlyLineArrow::TCurlyLineArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
              Float_t as, Option_t *opt)
              : TArrow(x1, y1, x2, y2, as,opt )
{
   fParent = NULL;
}
//________________________________________________________________________

void TCurlyLineArrow::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//   static Double_t da, dc;
//   if (event == kButton1Down) {
//    remember distance of arrow head to end of curly
 //     da = fParent->GetDistToArrow(1);
//      std::cout << "TCurlyLineArrow::ExecuteEvent "<< std::endl;
//   }
   TCurlyLineArrow *aoe;
   TArrow::ExecuteEvent(event, px, py);
   if (event == kButton1Up) {
      if (fWhere == 2) {
//          std::cout << "TCurlyLineArrow::ExecuteEvent fY2 "<< fY2<< std::endl;
         fParent->SetX2(fX2);
         fParent->SetY2(fY2);
         fParent->SetEndPoint(fX2, fY2);

         aoe = fParent->GetArrowStart();
         if (aoe) {
            fParent->SetX1(aoe->GetX2());
            fParent->SetY1(aoe->GetY2());
            fParent->SetStartPoint(aoe->GetX2(),aoe->GetY2());
         }

         fParent->Paint();
      }
      if (fWhere == 1) {
         fParent->SetX1(fX2);
         fParent->SetY1(fY2);
         fParent->SetStartPoint(fX2, fY2);
         aoe = fParent->GetArrowEnd();
         if (aoe) {
            fParent->SetX2(aoe->GetX2());
            fParent->SetY2(aoe->GetY2());
            fParent->SetEndPoint(aoe->GetX2(),aoe->GetY2());
         }
         fParent->Paint();
      }
   }
}
//________________________________________________________________________

TCurlyLineWithArrow::TCurlyLineWithArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
              Double_t wl, Double_t amp,
              Int_t where, Float_t arrowsize)
              : TCurlyLine(x1, y1, x2, y2, wl, amp), fX1(x1), fY1(y1),
                           fX2(x2), fY2(y2), fWhere(where),
                           fDefaultArrowSize(arrowsize)
{
   fArrowStart = NULL;
   fArrowEnd   = NULL;
   if (fDefaultArrowSize <= 0) fDefaultArrowSize = 2 * wl;
   if (where & 1) {
      fArrowStart = new TCurlyLineArrow();
      fArrowStart->SetParent(this);
      fArrowStart->SetWhere(1);
      fArrowStart->SetArrowSize(fDefaultArrowSize);
      fArrowStart->Draw();
   }
   if (where & 2) {
      fArrowEnd   = new TCurlyLineArrow();
      fArrowEnd->SetParent(this);
      fArrowEnd->SetWhere(2);
      fArrowEnd->SetArrowSize(fDefaultArrowSize);
      fArrowEnd->Draw();
   }
   Paint();
}
//________________________________________________________________________

void TCurlyLineWithArrow::SetArrowSize(Int_t where, Float_t arrowsize)
{
   if (fArrowStart && where & 1)
      fArrowStart->SetArrowSize(arrowsize);
   if (fArrowEnd && where & 2)
      fArrowEnd->SetArrowSize(arrowsize);
}
//________________________________________________________________________

Double_t  TCurlyLineWithArrow::GetDistToArrow(Int_t where)
{
   Double_t da = 0;
   if (fArrowStart && where & 1)
       da = TMath::Sqrt((fX1 - GetStartX())* (fX1 - GetStartX())
                     + (fY1 - GetStartY())* (fY1 - GetStartY()));

   if (fArrowEnd && where & 2)
      da = TMath::Sqrt((fX2 - GetEndX())* (fX2 - GetEndX())
                     + (fY2 - GetEndY())* (fY2 - GetEndY()));
   return da;
}
//________________________________________________________________________

void TCurlyLineWithArrow::Paint(const Option_t */*opt*/)
{
//   TCurlyLine::Paint();
   Double_t x1, x2, y1, y2;
   Float_t as;
   Double_t dc, da;
   SetStartPoint(fX1, fY1);
   SetEndPoint(fX2, fY2);
   if (fArrowStart) {
//  at end
//   dist arrow head - start of line
      as = (gPad->GetX2() - gPad->GetX1())* fArrowStart->GetArrowSize();
      da = GetDistToArrow(fArrowStart->GetWhere());
      if (TMath::Abs(da - 0.75 * as) > 0.01) {
         dc = TMath::Sqrt((fX2 - fX1)* (fX2 - fX1)
                        + (fY2 - fY1)* (fY2 - fY1));
         SetStartPoint((fX2 - fX1) * (0.75*as) / dc + fX1,
                       (fY2 - fY1) * (0.75*as) / dc + fY1);
         Build();
      }
      x1 = GetStartX();
      x2 = fX1;
      y1 = GetStartY();
      y2 = fY1;
      fArrowStart->SetLineWidth(GetLineWidth());
      fArrowStart->SetLineColor(GetLineColor());
      fArrowStart->SetFillColor(GetLineColor());
      fArrowStart->SetFillStyle(1001);
      fArrowStart->SetX1(x1);
      fArrowStart->SetX2(x2);
      fArrowStart->SetY1(y1);
      fArrowStart->SetY2(y2);
      fArrowStart->Paint("|>");
//      std::cout << "TCurlyLineWithArrow::Paint fArrowStart"<< fY1<< std::endl;
   }
   if (fArrowEnd) {
//  at end
//   dist arrow head - start of line
      as = (gPad->GetX2() - gPad->GetX1())* fArrowEnd->GetArrowSize();
      da = GetDistToArrow(fArrowEnd->GetWhere());
      if (TMath::Abs(da - 0.75 * as) > 0.01) {
         dc = TMath::Sqrt((fX2 - fX1)* (fX2 - fX1)
                        + (fY2 - fY1)* (fY2 - fY1));
         SetEndPoint((fX2 - fX1) * (dc-0.75*as) / dc + fX1,
                     (fY2 - fY1) * (dc-0.75*as) / dc + fY1);
         Build();
      }
      x1 = GetEndX();
      x2 = fX2;
      y1 = GetEndY();
      y2 = fY2;
      fArrowEnd->SetLineWidth(GetLineWidth());
      fArrowEnd->SetLineColor(GetLineColor());
      fArrowEnd->SetFillColor(GetLineColor());
      fArrowEnd->SetFillStyle(1001);
      fArrowEnd->SetX1(x1);
      fArrowEnd->SetX2(x2);
      fArrowEnd->SetY1(y1);
      fArrowEnd->SetY2(y2);
      fArrowEnd->Paint("|>");
//      std::cout << "TCurlyLineWithArrow::Paint fArrowEnd"<< fY2<< std::endl;
   }
   TCurlyLine::Paint();
   gPad->Modified();
   gPad->Update();
}
//________________________________________________________________________

void TCurlyLineWithArrow::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   static Double_t daend, dastart,dc;
   if (event == kButton1Down) {
//    remember distance of arrow head to end of curly
//      std::cout << "TCurlyLineWithArrow::Enter "<< GetStartX()<< std::endl;
      daend = TMath::Sqrt((fX2 - GetEndX())* (fX2 - GetEndX())
                    + (fY2 - GetEndY())* (fY2 - GetEndY()));
      dastart = TMath::Sqrt((fX1 - GetStartX())* (fX1 - GetStartX())
                    + (fY1 - GetStartY())* (fY1 - GetStartY()));
   }
   TCurlyLine::ExecuteEvent(event, px, py);

   if (event == kButton1Up) {
      if (fArrowStart) {
         dc = TMath::Sqrt((GetEndX() - GetStartX())* (GetEndX() - GetStartX())
                     + (GetEndY() - GetStartY())* (GetEndY() - GetStartY()));
         fX1 = GetStartX() - (GetEndX() - GetStartX())*(dastart)/dc;
         fY1 = GetStartY() - (GetEndY() - GetStartY())*(dastart)/dc;

      } else {
          fX1 = GetStartX();
          fY1 = GetStartY();
      }
      if (fArrowEnd) {
         dc = TMath::Sqrt((GetEndX() - GetStartX())* (GetEndX() - GetStartX())
                     + (GetEndY() - GetStartY())* (GetEndY() - GetStartY()));
         fX2 = GetStartX() + (GetEndX() - GetStartX())*(dc + daend)/dc;
         fY2 = GetStartY() + (GetEndY() - GetStartY())*(dc + daend)/dc;
      } else {
          fX2 = GetEndX();
          fY2 = GetEndY();
      }

//      std::cout << "TCurlyLineWithArrow::Exit "<< GetStartX()<< std::endl;
      Paint();
   }
}
//___________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void TCurlyLineWithArrow::SavePrimitive(std::ostream &out, Option_t *)
#else
void TCurlyLineWithArrow::SavePrimitive(std::ofstream &out, Option_t *)
#endif
{
   // Save primitive as a C++ statement(s) on output stream out

   std::cout << "TCurlyLineWithArrow::SavePrimitive: " << this << std::endl;
   out<<"   "<<std::endl;
   if (gROOT->ClassSaved(TCurlyLineWithArrow::Class())) {
      out<<"   ";
   } else {
      out<<"    TCurlyLineWithArrow *";
   }

   out<<"clwa = new TCurlyLineWithArrow("<< fX1  <<","<< fY1 <<","
       << fX2 <<","<< fY2 <<"," << GetWaveLength()  <<","<< GetAmplitude()<<","
       << fWhere<<","<<fDefaultArrowSize;
          out << ");"<<std::endl;

   SaveLineAttributes(out,"clwa",0,0,0);

   out<<"clwa->Draw();"<<std::endl;
   if(fArrowStart) {
      out<<"clwa->GetArrowStart()->SetArrowSize(" <<
         fArrowStart->GetArrowSize()<< ");"<<std::endl;
      out<<"clwa->GetArrowStart()->SetAngle(" <<
         fArrowStart->GetAngle()<< ");"<<std::endl;
   }
   if(fArrowEnd) {
      out<<"clwa->GetArrowEnd()->SetArrowSize(" <<
         fArrowEnd->GetArrowSize()<< ");"<<std::endl;
      out<<"clwa->GetArrowEnd()->SetAngle(" <<
         fArrowEnd->GetAngle()<< ");"<<std::endl;
   }
}

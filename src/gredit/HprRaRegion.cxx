#include "Riostream.h"
#include "HprRaRegion.h"

ClassImp (HprRaRegion)

HprRaRegion::HprRaRegion(Double_t x1, Double_t y1, Double_t x2, Double_t y2) : TBox(x1, y1, x2, y2)
{
   Draw();
   SetLineWidth(1);
   SetLineStyle(2);
   SetLineColor(1);
   SetFillStyle(0);
   fCutG = NULL;
//   cout << "HprRaRegion ctor: " << this << endl;
}
//________________________________________________________________________
HprRaRegion::~HprRaRegion()
{
//   cout << "HprRaRegion dtor: " << this << endl;
   if (fCutG)
      delete fCutG;
}
//________________________________________________________________________
TCutG *HprRaRegion::CutG()
{
//   cout << "fCutG " << fCutG<< endl;
   if ( !fCutG )
      fCutG = new TCutG("HprRaRegionInt", 5);
//   cout << "fCutG "<< fCutG << endl;
   fCutG->SetPoint(0,  GetX1(), GetY1());
   fCutG->SetPoint(1,  GetX2(), GetY1());
   fCutG->SetPoint(2,  GetX2(), GetY2());
   fCutG->SetPoint(3,  GetX1(), GetY2());
   fCutG->SetPoint(4,  GetX1(), GetY1());
   return fCutG;
}

#include "TROOT.h"
#include "Riostream.h"
#include "TF1.h"
#include "TH1.h"
#include "HTPad.h"
#include "GrCanvas.h"

ClassImp(HTPad)

//________________________________________________________________________

HTPad::HTPad()
{
   TObject *obj;
   TIter next(gPad->GetListOfPrimitives());

   while ((obj = next())) {
      if (obj->InheritsFrom(TF1::Class())) {
         TF1* f = (TF1*)obj;
         TH1* h = f->GetHistogram();
         cout << "TH1* h = f->GetHistogram()" << h << endl;
      }
   }
}

//________________________________________________________________________

HTPad::HTPad(const char *name, const char *title,
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           Color_t color, Short_t bordersize, Short_t bordermode)
           : TPad(name, title, x1, y1, x2, y2, color, bordersize, bordermode)
           , HprElement((TObject*)this)
{
   Int_t  n = 0;
   fHiddenPrimitives = NULL;
   fCurrentPlane = 50;
   TObject *obj;
   TIter next(gPad->GetListOfPrimitives());

   while ((obj = next())) {
      if (obj->InheritsFrom(TPad::Class())) {
         n++;
      }
   }
//   std::cout << "ctor HTPad this: " << this  << "++++++++++++++++++++++++++++++++++++++++++" <<  std::endl;
}
HTPad::~HTPad()
{
   if ( !TestBit(kNotDeleted) )
      cout <<" dtor HTPad: " << this <<
             " is deleted !!!!" << endl;
//	cout << "dtor HTPad " << this << "++++++++++++++++++++++++++++++++++++++++++"  << endl;
}
;
void HTPad::Paint(Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);

   if (htc && htc->GetUseEditGrid()) {
		Double_t x1 =  htc->PutOnGridX_NDC(GetXlowNDC());
		Double_t y1 =  htc->PutOnGridY_NDC(GetYlowNDC());
		Double_t x2 =  htc->PutOnGridX_NDC(x1 + GetWNDC());
		Double_t y2 =  htc->PutOnGridY_NDC(y1 + GetHNDC());
      SetPad(x1, y1, x2, y2);
   }
   if (GetVisibility() == 1) {
//      cout << " TPad::Paint(opt); " << this << endl;
      TPad::Paint(opt);
   } else {
//      cout << " no  PaintModified" << this << endl;
   }
}
//________________________________________________________________

void HTPad::PaintModified()
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);

   if (htc && htc->GetUseEditGrid()) {
		Double_t x1 =  htc->PutOnGridX_NDC(GetXlowNDC());
		Double_t y1 =  htc->PutOnGridY_NDC(GetYlowNDC());
		Double_t x2 =  htc->PutOnGridX_NDC(x1 + GetWNDC());
		Double_t y2 =  htc->PutOnGridY_NDC(y1 + GetHNDC());
      SetPad(x1, y1, x2, y2);
   }
   if (GetVisibility() == 1) {
//      cout << " TPad::PaintModified(opt); " << this << endl;
      TPad::PaintModified();
   } else {
//      cout << " no  PaintModified" << this << endl;
   }
}
//______________________________________________________________________________

void HTPad::HideObject(TObject *obj)
{
   if (!obj || obj == this || GetListOfPrimitives()->GetEntries()  <= 0 ) return;
   if (!fHiddenPrimitives) fHiddenPrimitives = new TList();
   std::cout << "HideObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, GetListOfPrimitives(), fHiddenPrimitives);
}
//______________________________________________________________________________

void HTPad::ViewObject(TObject *obj)
{
   std::cout << "ViewObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, fHiddenPrimitives, GetListOfPrimitives());
}

//______________________________________________________________________________

void HTPad::ViewAllObjects()
{
   if (!fHiddenPrimitives || fHiddenPrimitives->GetEntries() <=0 ) return;
   HprElement::MoveAllObjects(fHiddenPrimitives, GetListOfPrimitives(), 0,1);
}
//______________________________________________________________________________

void HTPad::RemoveImage()
{
   TIter next(GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("HprImage")) {
         GetListOfPrimitives()->Remove(obj);;
         Modified();
         break;
      }
   }
}


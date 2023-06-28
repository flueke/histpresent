#include "TROOT.h"
#include "Riostream.h"
#include "THprSplineX.h"
#include "HprText.h"
#include "GrCanvas.h"

ClassImp(THprSplineX)
//________________________________________________________________________

THprSplineX::THprSplineX(Int_t npoints, Double_t *x, Double_t *y,
         Float_t *sf, Float_t prec, Bool_t closed)
           : TSplineX(npoints, x, y, sf, prec, closed)
{
   SetDaughter(this);
	fTextDialog = NULL;
}
//_____________________________________________________________________________________

void THprSplineX::AddTextDialog()
{
   Bool_t from_file = kFALSE;
   fTextDialog = new InsertTextDialog(from_file, this);
}
//_____________________________________________________________________________________

void THprSplineX::InsertTextExecute()
{
//   std::cout << "TSplineX::InsertTextExecute:" << std::endl;
   std::cout << *(fTextDialog->GetTextPointer()) << std::endl;
   HprText *ht = new HprText( fTextDialog->GetTextPointer()->Data());
	ht->SetTextAlign(fTextDialog->GetEditTextAlign());
	ht->SetTextFont(fTextDialog->GetEditTextFont() * 10 + fTextDialog->GetEditTextPrec());
	ht->SetTextSize(fTextDialog->GetEditTextSize());
	ht->SetTextColor(fTextDialog->GetEditTextColor());
	ht->SetTextAngle(fTextDialog->GetEditTextAngle());
   ht->SetOffset(fTextDialog->GetEditTextX0());

   AddText(ht);
   gPad->Modified();
   gPad->Update();
}
//_____________________________________________________________________________________

void THprSplineX::CloseDown(Int_t /*flag*/)
{
//   std::cout << "THprSplineX::CloseDown "<<  flag<< std::endl;
   if (fTextDialog) {
      fTextDialog->SaveDefaults();
      delete fTextDialog;
      fTextDialog = NULL;
   }
};
//________________________________________________________________________

void THprSplineX::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
		Double_t x, y, x1, y1;
      Float_t sf;
		Int_t changed = 0;
      for (Int_t ip = 0; ip < GetNofControlPoints(); ip++) {
         GetControlPoint(ip, &x, &y, &sf);
         SetControlPoint(ip, htc->PutOnGridX(x), htc->PutOnGridY(y),sf);
			GetControlPoint(ip, &x1, &y1, &sf);
			if (Length(x,y, x1,y1) > 0.001)
				changed++;
		}
		if (changed > 0)
			NeedReCompute();
//      cout << "THprSplineX htc->PutOnGridY(GetEndY() " <<  htc->PutOnGridY(GetEndY())<< endl;
   }
   if (GetVisibility() == 1)
      TSplineX::Paint(opt);
}

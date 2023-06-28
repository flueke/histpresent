#include "TROOT.h"
#include "Riostream.h"
#include "TVirtualPS.h"
#include "THprLatex.h"
#include "GrCanvas.h"

const Int_t kLatex      = BIT(10);

ClassImp(THprLatex)
//________________________________________________________________________

THprLatex::THprLatex(Double_t x, Double_t y, const Char_t * text)
           : TLatex(x, y, text)
{
//	cout << "THprLatex " << text << endl;
   SetDaughter(this);
}
//________________________________________________________________________

void THprLatex::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      SetX(htc->PutOnGridX(GetX()));
      SetY(htc->PutOnGridY(GetY()));
   }
   if (GetVisibility() == 1)
      TLatex::Paint(opt);
}
//______________________________________________________________________________
/*
THprLatex *THprLatex::DrawLatex(Double_t x, Double_t y, const char *text)
{
   // Make a copy of this object with the new parameters
   // And copy object attributes

   THprLatex *newtext = new THprLatex(x, y, text);
   TAttText::Copy(*newtext);
   newtext->SetBit(kCanDelete);
   if (TestBit(kTextNDC)) newtext->SetNDC();
	// hpr
   if (TestBit(kValignNoShift)) newtext->SetBit(kValignNoShift);
   newtext->AppendPad();
   return newtext;
}

//______________________________________________________________________________
void THprLatex::PaintLatex(Double_t x, Double_t y, Double_t angle, Double_t size, const Char_t *text1)
{
   // Main drawing function
   //
   // Warning: Unlike most others "XYZ::PaintXYZ" methods, PaintLatex modifies
   //          the TLatex data members.
	TString stext(text1);
	if (stext.Contains("\\")) {
		TLatex::PaintLatex(x,y,angle, size,text1);
		return;
	}
   TAttText::Modify();  //Change text attributes only if necessary

   // do not use Latex if font is low precision
   if (fTextFont%10 < 2) {
		if (gVirtualX) gVirtualX->SetTextAngle(angle);
		if (gVirtualPS) gVirtualPS->SetTextAngle(angle);
		gPad->PaintText(x,y,text1);
      return;
   }
   
   TString newText = text1;
	if( newText.Length() == 0) return;
	cout <<"PaintLatex: " << text1 << endl;
   Double_t saveSize = size;
   Int_t saveFont = fTextFont;
   if (fTextFont%10 > 2) {
      UInt_t w = TMath::Abs(gPad->XtoAbsPixel(gPad->GetX2()) -
                            gPad->XtoAbsPixel(gPad->GetX1()));
      UInt_t h = TMath::Abs(gPad->YtoAbsPixel(gPad->GetY2()) -
                            gPad->YtoAbsPixel(gPad->GetY1()));
      if (w < h)
         size = size/w;
      else
         size = size/h;
      SetTextFont(10*(saveFont/10) + 2);
   }
   if (gVirtualPS) gVirtualPS->SetBit(kLatex);


   fError = 0 ;
   if (CheckLatexSyntax(newText)) {
      cout<<"\n*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<text1<<endl;
      return ;
   }
   fError = 0 ;

   Int_t length = newText.Length() ;
   const Char_t *text = newText.Data() ;

   fX=x;
   fY=y;
   x = gPad->XtoAbsPixel(x);
   y = gPad->YtoAbsPixel(y);
   fShow = kFALSE ;
   TLatexFormSize fs = FirstParse(angle,size,text);

   fOriginSize = size;

   //get current line attributes
   Short_t lineW = GetLineWidth();
   Int_t lineC = GetLineColor();

   TextSpec_t spec;
   spec.fAngle = angle;
   spec.fSize  = size;
   spec.fColor = GetTextColor();
   spec.fFont  = GetTextFont();
   Short_t halign = fTextAlign/10;
   Short_t valign = fTextAlign - 10*halign;
   TextSpec_t newSpec = spec;
	//hpr
   Double_t cshift = 0;
   if ( this->TestBit(kValignNoShift) && valign == 2) {
      TLatexFormSize fs1 = FirstParse(angle,size,"a");
      cshift = (fs1.Over()-fs1.Under())/1.5;
   }
   // hpr end
   if (fError != 0) {
      cout<<"*ERROR<TLatex>: "<<fError<<endl;
      cout<<"==> "<<text<<endl;
   } else {
      fShow = kTRUE;
      Double_t mul = 1;
      newSpec.fSize = mul*size;

      switch (valign) {
         case 0: y -= fs.Under()*mul; break;
         case 1: break;
         case 2: 
            if ( this->TestBit(kValignNoShift) ) {
//               std::cout << "NoShift: " << std::endl;
               y += cshift;
            } else {
               y += (fs.Over()-fs.Under())*mul/1.5;
//               std::cout << " kValignNoShift " << std::endl;
            }
            break;
         case 3: y += fs.Over()*mul;  break;
      }
      switch (halign) {
         case 2: x -= fs.Width()*mul/2  ; break;
         case 3: x -= fs.Width()*mul    ;   break;
      }
      Analyse(x,y,newSpec,text,length);
   }

   SetTextSize(saveSize);
   SetTextAngle(angle);
   SetTextFont(saveFont);
   SetTextColor(spec.fColor);
   SetTextAlign(valign+10*halign);
   SetLineWidth(lineW);
   SetLineColor(lineC);
   delete[] fTabSize;

   if (gVirtualPS) gVirtualPS->ResetBit(kLatex);
}
*/

#include "TLatex.h"
#include "TLine.h"
#include "TROOT.h"
#include "TRegexp.h"
#include <iostream>
#include "TextOnArc.h"

ClassImp (TextOnArc)

Double_t pixtol(UInt_t w)
{
	return (Double_t)w * (gPad->GetUxmax()-gPad->GetUxmin()) / (Double_t)gPad->GetWw();
}
Double_t rad2deg(Double_t a)
{
	return a * 180 / TMath::Pi();
}
Double_t deg2rad(Double_t a)
{
	return TMath::Pi() * a / 180;
}
//_______________________________________________________________________________

TextOnArc::TextOnArc(Double_t x, Double_t y, Double_t r, const char * text,
						   Double_t stangle, Short_t align )
	: TArc(x, y, r)
{
   SetDaughter(this);
	fText = text;
	fAlign = align;
	fAngle = stangle;
	fSeperator = 0;
	SetTextAlign(21);
	SetTextSize(0.08);
	SetTextFont(62);
	SetTextColor(1);
	SetLineColor(0);
	SetLineWidth(0);
	SetFillStyle(0);
}
//_______________________________________________________________________________

TextOnArc::~TextOnArc(){};
//_______________________________________________________________________________

void TextOnArc::Paint(Option_t * option)
{
//	fPad = (TPad*)gPad;
//	if (fPad == 0) {
//		MakeDefaultCanvas();
//	}
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (GetMustAlign() && htc && htc->GetUseEditGrid()) {
      SetR1(htc->PutOnGridX(GetR1()));
      SetX1(htc->PutOnGridX(GetX1()));
      SetY1(htc->PutOnGridY(GetY1()));
   }
   if (GetVisibility() == 0)
      return;
	
	UInt_t w, h;
	Double_t cw, x, y, tang, r;
	TString ch;
	TLatex * lat;
//	TCanvas c;
	if (option) {};
// get length of painted text
	TText tt(GetX1(),GetY1(),fText.Data());
	tt.SetTextSize(GetTextSize());
	tt.GetTextExtent(w,h,fText.Data());
// 	std::cout << "Length: " << pixtol(w) << std::endl;
	tt.GetTextExtent(w,h,"a");
	Double_t cwa =  pixtol(w);
	tt.GetTextExtent(w,h,"A");
	Double_t cwA =  pixtol(w);
	Int_t font = GetTextFont() / 10;
	Int_t times = 13, times_bold = 2, helv = 4,
	      helv_bold = 6, courier = 8, courier_bold = 10;
	TRegexp uc("[A-Z]");
	Bool_t umlaut = kFALSE;
// Seperator length
//	tt.GetTextExtent(w,h,"i");
//	Double_t sep = pixtol(w);
	Double_t sep = fSeperator * cwa;
	
// find length of text
	Double_t tl = 0;
	umlaut = kFALSE;
	for (Int_t i = 0; i < fText.Length(); i++) {
		ch = fText[i];
		if (ch == "\"") {
			umlaut = kTRUE;
			continue;
		}
		tt.GetTextExtent(w,h,ch.Data());
		cw = pixtol(w);
//		cout << endl << "Ent " << ch << " " << cw << endl;
		if (font == helv)         cw *= 0.96;
		if (font == helv_bold)    cw *= 1.055;
		if (font == times)        cw *= 0.9;
		if (font == times_bold)   cw *= 0.95;
		if (font == courier_bold || font == courier) {
			if (ch.Index(uc) >= 0) {
				cw = cwA;
			} else {
				cw = cwa;
			}
		}
		if (( ch == "i" || ch == "I" )
		   && font != courier && font != courier_bold ){ 
		   cw *= 1.3;
		}
//		cout << "Cor " << ch << " " << cw << endl;
	   tl += 0.5 * cw ;
		if (( ch == "f") 
			&& font != courier && font != courier_bold ) {
			cw *= 0.6;
//		   cout << "Cof " << ch << " " << cw << endl;
 		}
	   tl += (0.5 * cw + sep);
	}
//
	Double_t phi = deg2rad(fAngle);
	r = GetR1();
	if ( fAlign == 2 ) {
		phi += 0.5 * tl / r;
	} else if ( fAlign == 3 ) {
		phi += tl / r;
	}
// 
	SetPhimin(rad2deg(phi));
	SetPhimax(rad2deg(phi-tl / r));
	umlaut = kFALSE;
	for (Int_t i = 0; i < fText.Length(); i++) {
		ch = fText[i];
		if (ch == "\"") {
			umlaut = kTRUE;
			continue;
		} 
		tt.GetTextExtent(w,h,ch.Data());
		cw = pixtol(w);
//		cout << endl << "Ent " << ch << " " << cw << endl;
		if (font == helv)         cw *= 0.96;
		if (font == helv_bold)    cw *= 1.055;
		if (font == times)        cw *= 0.9;
		if (font == times_bold)   cw *= 0.95;
		if (font == courier_bold || font == courier) {
			if (ch.Index(uc) >= 0) {
				cw = cwA;
			} else {
				cw = cwa;
			}
		}
		if (( ch == "i" || ch == "I" )
		   && font != courier && font != courier_bold ){ 
		   cw *= 1.3;
		}
//		cout << "Cor " << ch << " " << cw << endl;
	
		phi -= 0.5 * cw / r;
		x = GetX1() + r * TMath::Cos(phi);
		y = GetY1() + r * TMath::Sin(phi);
		tang = phi - 0.5 * TMath::Pi();
//		if (tang < 0) 
//			tang += 2 * TMath::Pi();
//		cout << ch << " cw " << cw << " phi " << rad2deg(phi) 
//		<< " tang " << rad2deg(tang) << endl;
		if (umlaut) {
			ch.Prepend("#ddot{");
			ch.Append("}");
			umlaut = kFALSE;
		}
		lat = new TLatex(x,y,ch);
		lat->SetTextAlign(21);
		lat->SetTextAngle(rad2deg(tang));
		lat->SetTextSize(GetTextSize());
		lat->SetTextColor(GetTextColor());
		lat->SetTextFont(GetTextFont());	
		lat->Paint();
//		TLine * ll = new TLine(GetX1(), GetY1() , x, y);
//		ll->Paint();
		if (( ch == "f") 
			&& font != courier && font != courier_bold ) {
			cw *= 0.6;
//		   cout << "Cof " << ch << " " << cw << endl;
 		}
		phi = phi - ( 0.5 * cw + sep ) / r;
	}
	gPad->Modified();
}
//_______________________________________________________________________________

Int_t TextOnArc::DistancetoPrimitive(Int_t px, Int_t py)
{
   // Compute distance from point px,py to an TextOnArc

   Double_t x = gPad->PadtoX(gPad->AbsPixeltoX(px));
   Double_t y = gPad->PadtoY(gPad->AbsPixeltoY(py));

   Double_t dxnr = x - GetX1();
   Double_t dynr = y - GetY1();
	Double_t r = TMath::Sqrt(dxnr*dxnr + dynr*dynr);
	Double_t phi1  = TMath::ATan2(dynr/r, dxnr/r);
	if(phi1 < 0) phi1 += 2 * TMath::Pi();
	phi1 = phi1 * 180 / TMath::Pi();
   Int_t dist = 9999;
	Double_t phi_start = GetPhimin();
	Double_t phi_end = GetPhimax();
	if (phi_end < phi_start) {
		Double_t t  = phi_end;
		phi_end = phi_start;
		phi_start = t;
	}
	
	if ( phi1 >= phi_start && phi1 <= phi_end ) {
		Double_t ch = GetTextSize() * (gPad->GetUymax() - gPad->GetUymin());
		if  ( r >= GetR1() && r <= GetR1() + ch ) {
			dist = 0;
		}
	}
   return dist;
}

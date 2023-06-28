#include "TROOT.h"
#include "TMath.h"
#include "Riostream.h"
#include "THprArc.h"
#include "GrCanvas.h"

const Double_t kPI = 3.14159265358979323846;
ClassImp(THprArc)

//________________________________________________________________________

THprArc::THprArc(Double_t x1, Double_t y1, Double_t rad,
             Double_t phimin, Double_t phimax)
           : TArc(x1, y1, rad, phimin, phimax)
{
   SetDaughter(this);
}
//________________________________________________________________________

void THprArc::Paint(const Option_t *opt)
{
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (GetMustAlign() && htc && htc->GetUseEditGrid()) {
      SetR1(htc->PutOnGridX(GetR1()));
      SetX1(htc->PutOnGridX(GetX1()));
      SetY1(htc->PutOnGridY(GetY1()));
   }
   if (GetVisibility() == 1)
      TArc::Paint(opt);
}
//________________________________________________________________________

void   THprArc::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   Int_t kMaxDiff = 10;
//   const Int_t kMinSize = 25;
   const Int_t np = 40;
   static Int_t x[np+2], y[np+2];
   static Int_t px1,py1,npe,r1;
   static Int_t pxold, pyold;
   static Int_t sig,impair;
   Int_t i, dpx, dpy;
   Double_t angle,dx,dy,dphi,fLx,fRx;
   static Bool_t pTop, pL, pR, pBot, pINSIDE;
   static Int_t pTx,pTy,pLx,pLy,pRx,pRy,pBx,pBy;

   if (!gPad->IsEditable()) return;

   switch (event) {

   case kButton1Down:
      gVirtualX->SetLineColor(-1);
      TAttLine::Modify();
      dphi = (fPhimax-fPhimin)*kPI/(180*np);
      for (i=0;i<np;i++) {
         angle = fPhimin*kPI/180 + Double_t(i)*dphi;
         dx    = fR1*TMath::Cos(angle);
         dy    = fR1*TMath::Sin(angle);
         x[i]  = gPad->XtoAbsPixel(fX1 + dx);
         y[i]  = gPad->YtoAbsPixel(fY1 + dy);
      }
      if (fPhimax-fPhimin >= 360 ) {
         x[np] = x[0];
         y[np] = y[0];
         npe = np;
      } else {
         x[np]   = gPad->XtoAbsPixel(fX1);
         y[np]   = gPad->YtoAbsPixel(fY1);
         x[np+1] = x[0];
         y[np+1] = y[0];
         npe = np + 1;
      }
      impair = 0;
      px1 = gPad->XtoAbsPixel(fX1);
      py1 = gPad->YtoAbsPixel(fY1);
      pTx = pBx = px1;
      pLy = pRy = py1;
      pTy = gPad->YtoAbsPixel(fR1+fY1);
      pBy = gPad->YtoAbsPixel(-fR1+fY1);
      pLx = gPad->XtoAbsPixel(-fR1+fX1);
      pRx = gPad->XtoAbsPixel(fR1+fX1);
//      r2 = (pBy-pTy)/2;
      r1 = (pRx-pLx)/2;
      gVirtualX->DrawLine(pRx+4, py1+4, pRx-4, py1+4);
      gVirtualX->DrawLine(pRx-4, py1+4, pRx-4, py1-4);
      gVirtualX->DrawLine(pRx-4, py1-4, pRx+4, py1-4);
      gVirtualX->DrawLine(pRx+4, py1-4, pRx+4, py1+4);
      gVirtualX->DrawLine(pLx+4, py1+4, pLx-4, py1+4);
      gVirtualX->DrawLine(pLx-4, py1+4, pLx-4, py1-4);
      gVirtualX->DrawLine(pLx-4, py1-4, pLx+4, py1-4);
      gVirtualX->DrawLine(pLx+4, py1-4, pLx+4, py1+4);
      gVirtualX->DrawLine(px1+4, pBy+4, px1-4, pBy+4);
      gVirtualX->DrawLine(px1-4, pBy+4, px1-4, pBy-4);
      gVirtualX->DrawLine(px1-4, pBy-4, px1+4, pBy-4);
      gVirtualX->DrawLine(px1+4, pBy-4, px1+4, pBy+4);
      gVirtualX->DrawLine(px1+4, pTy+4, px1-4, pTy+4);
      gVirtualX->DrawLine(px1-4, pTy+4, px1-4, pTy-4);
      gVirtualX->DrawLine(px1-4, pTy-4, px1+4, pTy-4);
      gVirtualX->DrawLine(px1+4, pTy-4, px1+4, pTy+4);
      // No break !!!

   case kMouseMotion:
      px1 = gPad->XtoAbsPixel(fX1);
      py1 = gPad->YtoAbsPixel(fY1);
      pTx = pBx = px1;
      pLy = pRy = py1;
      pTy = gPad->YtoAbsPixel(fR1+fY1);
      pBy = gPad->YtoAbsPixel(-fR1+fY1);
      pLx = gPad->XtoAbsPixel(-fR1+fX1);
      pRx = gPad->XtoAbsPixel(fR1+fX1);
//		cout << "kMouseMotion: px, py px1,py1: "<< px << " " <<py << " " <<  px1 <<" "  << py1 <<  endl;
      pTop = pL = pR = pBot = pINSIDE = kFALSE;
      if ((TMath::Abs(px - pTx) < kMaxDiff) &&
          (TMath::Abs(py - pTy) < kMaxDiff)) {             // top edge
         pTop = kTRUE;
         gPad->SetCursor(kTopSide);
      }
      else
      if ((TMath::Abs(px - pBx) < kMaxDiff) &&
          (TMath::Abs(py - pBy) < kMaxDiff)) {             // bottom edge
         pBot = kTRUE;
         gPad->SetCursor(kBottomSide);
      }
      else
      if ((TMath::Abs(py - pLy) < kMaxDiff) &&
          (TMath::Abs(px - pLx) < kMaxDiff)) {             // left edge
         pL = kTRUE;
         gPad->SetCursor(kLeftSide);
      }
      else
      if ((TMath::Abs(py - pRy) < kMaxDiff) &&
          (TMath::Abs(px - pRx) < kMaxDiff)) {             // right edge
         pR = kTRUE;
         gPad->SetCursor(kRightSide);
      }
      else {pINSIDE= kTRUE; gPad->SetCursor(kMove); }
      pxold = px;  pyold = py;

      break;

   case kButton1Motion:
      for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      if (pTop) {
         r1 -= (py - pyold);
         if (TMath::Abs(pyold-py)%2==1) impair++;
         if (py-pyold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; r1 -= sig;}
//         if (py1 > pBy-kMinSize) {py1 = sav1; r1 = sav2; py = pyold;}
//		cout << "kButton1Motion pTop: pRx, py1, pLx, px1: " <<
//		pRx <<" " <<  py1 <<" " <<  pLx << " " << px1 << " " << endl;
      }
      if (pBot) {
         r1 += (py - pyold);
         if (TMath::Abs(pyold-py)%2==1) impair++;
         if (py-pyold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; r1 += sig;}
 //        if (py1 < pTy+kMinSize) {py1 = sav1; r1 = sav2; py = pyold;}
      }
      if (pL) {
//         px1 += (px - pxold);
         r1 -= (px - pxold);
         if (TMath::Abs(pxold-px)%2==1) impair++;
         if (px-pxold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; r1 -= sig;}
//         if (px1 > pRx-kMinSize) {px1 = sav1; r1 = sav2; px = pxold;}
      }
      if (pR) {
//         px1 += (px - pxold);
         r1 += (px - pxold);
         if (TMath::Abs(pxold-px)%2==1) impair++;
         if (px-pxold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; px1 += sig; r1 += sig;}
//         if (px1 < pLx+kMinSize) {px1 = sav1; r1 = sav2; px = pxold;}
      }
      if (pTop || pBot || pL || pR) {
         gVirtualX->SetLineColor(-1);
         TAttLine::Modify();
         dphi = (fPhimax-fPhimin)*kPI/(180*np);
         for (i=0;i<np;i++) {
            angle = fPhimin*kPI/180 + Double_t(i)*dphi;
            dx    = r1*TMath::Cos(angle);
            dy    = r1*TMath::Sin(angle);
				x[i]  = gPad->XtoAbsPixel(fX1) + dx;
				y[i]  = gPad->YtoAbsPixel(fY1) + dy;
         }
         if (fPhimax-fPhimin >= 360 ) {
            x[np] = x[0];
            y[np] = y[0];
            npe = np;
         } else {
            x[np]   = px1;
            y[np]   = py1;
            x[np+1] = x[0];
            y[np+1] = y[0];
            npe = np + 1;
         }
         for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      }
      if (pINSIDE) {
         dpx  = px-pxold;  dpy = py-pyold;
         px1 += dpx; py1 += dpy;
         for (i=0;i<=npe;i++) { x[i] += dpx; y[i] += dpy;}
         for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      }
		
      pTx = pBx = px1;
      pRx = px1+r1;
      pLx = px1-r1;
      pRy = pLy = py1;
      pTy = py1-r1;
      pBy = py1+r1;
		
      pxold = px;
      pyold = py;
      break;

   case kButton1Up:
      if (gROOT->IsEscaped()) {
         gROOT->SetEscape(kFALSE);
         break;
      }

      fX1 = gPad->AbsPixeltoX(px1);
      fY1 = gPad->AbsPixeltoY(py1);
      fLx = gPad->AbsPixeltoX(px1+r1);
      fRx = gPad->AbsPixeltoX(px1-r1);
      fR1 = TMath::Abs(fRx-fLx)/2;
      fR2 = fR1;
      gPad->Modified(kTRUE);
      gVirtualX->SetLineColor(-1);
   }
}

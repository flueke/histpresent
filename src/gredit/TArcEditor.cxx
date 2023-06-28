// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TArcEditor.cxx
// Author: Ilka Antcheva, Otto Schaile 15/12/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TArcEditor                                                     //
//                                                                      //
//  Implements GUI for editing Arc attributes: radius, phi1, phi2. //                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/TArcEditor.gif">
*/
//End_Html


#include "TArcEditor.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "TArc.h"
#include "TVirtualPad.h"
#include "iostream"

ClassImp(TGedFrame)
ClassImp(TArcEditor)

enum {
   kCRLA_RAD,
   kCRLA_FMIN,
   kCRLA_FMAX,
   kCRLA_CX,
   kCRLA_CY,
   kCRLA_X1,
   kCRLA_Y1,
   kCRLA_X2,
   kCRLA_Y2,
   kCRLA_CPH,
   kCRLA_DRE,
   kCRLA_SEN,
   kCRLA_PCF
};

//______________________________________________________________________________
#if ROOTVERSION < 51304
TArcEditor::TArcEditor(const TGWindow *p, Int_t id, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, id, width, height, options | kVerticalFrame, back)
#else
TArcEditor::TArcEditor(const TGWindow *p, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, width, height, options | kVerticalFrame, back)
#endif
{
   // Constructor of Arc GUI.

   fArc = 0;
   
   MakeTitle("Arc");

   fDrawEdgesEntry = new TGCheckButton(this, "Draw Edges", kCRLA_DRE);
   fDrawEdgesEntry->SetState(kButtonUp);
   fDrawEdgesEntry->SetToolTipText("Connect circumference with center"); 
   AddFrame(fDrawEdgesEntry, new TGLayoutHints(kLHintsLeft, 5, 1, 5, 8));

   TGCompositeFrame *f3 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f3, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fRadiusLabel = new TGLabel(f3, "Radius:");
   f3->AddFrame(fRadiusLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fRadiusEntry = new TGNumberEntry(f3, 0.02, 7, kCRLA_RAD, 
                                    TGNumberFormat::kNESRealThree,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
   fRadiusEntry->GetNumberEntry()->SetToolTipText("Set radius of arc.");
   f3->AddFrame(fRadiusEntry, new TGLayoutHints(kLHintsLeft, 18, 1, 1, 1));
/*   
   fUseCenterPhiEntry = new TGRadioButton(this, "Use Center + Phi", kCRLA_CPH);
   fUseCenterPhi = kTRUE;
   fUseCenterPhiEntry->SetState(kButtonDown);
   fUseCenterPhiEntry->SetToolTipText("Use radius, center, phimin, phimax"); 
   AddFrame(fUseCenterPhiEntry, new TGLayoutHints(kLHintsLeft, 5, 1, 5, 8));
*/
   TGCompositeFrame *f4 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f4, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPhiminLabel = new TGLabel(f4, "Phimin:");
   f4->AddFrame(fPhiminLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fPhiminEntry = new TGNumberEntry(f4, 0, 7, kCRLA_FMIN, 
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEANonNegative,
                                    TGNumberFormat::kNELLimitMinMax, 0, 360);
   fPhiminEntry->GetNumberEntry()->SetToolTipText("Set Phimin in degrees.");
   f4->AddFrame(fPhiminEntry, new TGLayoutHints(kLHintsLeft, 19, 1, 1, 1));

   TGCompositeFrame *f5 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f5, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPhimaxLabel = new TGLabel(f5, "Phimax:");
   f5->AddFrame(fPhimaxLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fPhimaxEntry = new TGNumberEntry(f5, 0, 7, kCRLA_FMAX, 
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEANonNegative,
                                    TGNumberFormat::kNELLimitMinMax, 0, 360);
   fPhimaxEntry->GetNumberEntry()->SetToolTipText("Set Phimax in degrees.");
   f5->AddFrame(fPhimaxEntry, new TGLayoutHints(kLHintsLeft, 16, 1, 1, 1));
//--
   TGCompositeFrame *f6 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f6, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fCenterXLabel = new TGLabel(f6, "Center X:");
   f6->AddFrame(fCenterXLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fCenterXEntry = new TGNumberEntry(f6, 0.0, 7, kCRLA_CX,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fCenterXEntry->GetNumberEntry()->SetToolTipText("Set center X coordinate.");
   f6->AddFrame(fCenterXEntry, new TGLayoutHints(kLHintsLeft, 6, 1, 1, 1));
//--
   TGCompositeFrame *f7 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f7, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fCenterYLabel = new TGLabel(f7, "Y:");
   f7->AddFrame(fCenterYLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 48, 0, 1, 1));
   fCenterYEntry = new TGNumberEntry(f7, 0.0, 7, kCRLA_CY,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fCenterYEntry->GetNumberEntry()->SetToolTipText("Set center Y coordinate.");
   f7->AddFrame(fCenterYEntry, new TGLayoutHints(kLHintsLeft, 7, 1, 1, 1));
//-
/*
   fUsePoCfEntry = new TGRadioButton(this, "Use 2 Points on Cf", kCRLA_PCF);
   fUsePoCfEntry->SetState(kButtonUp);
   fUsePoCfEntry->SetToolTipText("Use 2 Points on Circumference"); 
   AddFrame(fUsePoCfEntry, new TGLayoutHints(kLHintsLeft, 5, 1, 5, 8));

   fSenseEntry = new TGCheckButton(this, "Sense counterclock", kCRLA_SEN);
   fSenseEntry->SetState(kButtonDown);
   fSenseEntry->SetToolTipText("Sense of arc, counterclock if pressed"); 
   AddFrame(fSenseEntry, new TGLayoutHints(kLHintsLeft, 5, 1, 5, 8));

   TGCompositeFrame *f8 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f8, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPoCfX1Label = new TGLabel(f8, "1.PoCf X:");
   f8->AddFrame(fPoCfX1Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fPoCfX1Entry = new TGNumberEntry(f8, 0.0, 7, kCRLA_X1,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fPoCfX1Entry->GetNumberEntry()->SetToolTipText("Set first point on circumference X");
   f8->AddFrame(fPoCfX1Entry, new TGLayoutHints(kLHintsLeft, 6, 1, 1, 1));
//--
   TGCompositeFrame *f9 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f9, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPoCfY1Label = new TGLabel(f9, "Y:");
   f9->AddFrame(fPoCfY1Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 48, 0, 1, 1));
   fPoCfY1Entry = new TGNumberEntry(f9, 0.0, 7, kCRLA_Y1,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fPoCfY1Entry->GetNumberEntry()->SetToolTipText("Set 1. point on circumference Y");
   f9->AddFrame(fPoCfY1Entry, new TGLayoutHints(kLHintsLeft, 7, 1, 1, 1));
//-
   TGCompositeFrame *f10 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f10, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPoCfX2Label = new TGLabel(f10, "2.PoCf X:");
   f10->AddFrame(fPoCfX2Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fPoCfX2Entry = new TGNumberEntry(f10, 0.0, 7, kCRLA_X2,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fPoCfX2Entry->GetNumberEntry()->SetToolTipText("Set 2. point on circumference X");
   f10->AddFrame(fPoCfX2Entry, new TGLayoutHints(kLHintsLeft, 6, 1, 1, 1));
//--
   TGCompositeFrame *f11 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f11, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPoCfY2Label = new TGLabel(f11, "Y:");
   f11->AddFrame(fPoCfY2Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 48, 0, 1, 1));
   fPoCfY2Entry = new TGNumberEntry(f11, 0.0, 7, kCRLA_Y2,
                                     TGNumberFormat::kNESRealThree,
                                     TGNumberFormat::kNEANonNegative, 
                                     TGNumberFormat::kNELNoLimits);
   fPoCfY2Entry->GetNumberEntry()->SetToolTipText("Set 2. point on circumference Y");
   f11->AddFrame(fPoCfY2Entry, new TGLayoutHints(kLHintsLeft, 7, 1, 1, 1));
//-
*/
#if ROOTVERSION < 51304
   TClass *cl = TArc::Class();
   TGedElement *ge = new TGedElement;
   ge->fGedFrame = this;
   ge->fCanvas = 0;
   cl->GetEditorList()->Add(ge);
#endif
}

//______________________________________________________________________________
TArcEditor::~TArcEditor()
{
   // Destructor of Arc editor.
#if ROOTVERSION < 51304
   TGFrameElement *el;
   TIter next(GetList());
   
   while ((el = (TGFrameElement *)next())) {
      if (!strcmp(el->fFrame->ClassName(), "TGCompositeFrame"))
         ((TGCompositeFrame *)el->fFrame)->Cleanup();
   }
   Cleanup();
#endif
}

//______________________________________________________________________________
void TArcEditor::ConnectSignals2Slots()
{
   // Connect signals to slots.

   fCenterXEntry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fCenterXEntry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fCenterYEntry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
//	std::cout << "DoRedraw()" << std::endl;
//   (fCenterYEntry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "DoRedraw()");
//	std::cout << "DoRedraw()" << std::endl;
   fRadiusEntry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fRadiusEntry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fPhiminEntry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPhiminEntry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fPhimaxEntry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPhimaxEntry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
/*
   fPoCfX1Entry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPoCfX1Entry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fPoCfX2Entry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPoCfX2Entry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fPoCfY1Entry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPoCfY1Entry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fPoCfY2Entry->Connect("ValueSet(Long_t)", "TArcEditor", this, "Redraw()");
   (fPoCfY2Entry->GetNumberEntry())->Connect("ReturnPressed()", "TArcEditor", this, "Redraw()");
   fUseCenterPhiEntry->Connect("Pressed()", "TArcEditor", this, "DoUseCenterPhi");
   fUsePoCfEntry->Connect("Pressed()", "TArcEditor", this, "DoUsePointsOnCf");
*/
   fInit = kFALSE;
}

//______________________________________________________________________________
#if ROOTVERSION < 51304
void TArcEditor::SetModel(TVirtualPad* pad, TObject* obj, Int_t)
{
   fModel = 0;
   fPad = 0;
   if (obj == 0 || !obj->InheritsFrom("TArc")) {
      SetActive(kFALSE);
      return;
   }

   fModel = obj;
   fPad = pad;
   
   fArc = (TArc *)fModel;
#else 

void TArcEditor::SetModel(TObject* obj)
{
   fArc = (TArc *)obj;
#endif
   // Pick up the used curly arc attributes.
//   std::cout << "TArcEditor::SetModel() " << std::endl;
   Double_t rad = fArc->GetR1();
   fRadiusEntry->SetNumber(rad);

   Double_t phimin = fArc->GetPhimin();
   fPhiminEntry->SetNumber(phimin);

   Double_t phimax = fArc->GetPhimax();
   fPhimaxEntry->SetNumber(phimax);

   Double_t xc = fArc->GetX1();
   fCenterXEntry->SetNumber(xc);

   Double_t yc = fArc->GetY1();
   fCenterYEntry->SetNumber(yc);
/*   
   Double_t x1 = rad * TMath::Cos(TMath::Pi() * phimin / 180) + xc; 
   fPoCfX1Entry->SetNumber(x1);
   Double_t y1 = rad * TMath::Sin(TMath::Pi() * phimin / 180) + yc; 
   fPoCfY1Entry->SetNumber(y1);
   Double_t x2 = rad * TMath::Cos(TMath::Pi() * phimax / 180) + xc; 
   fPoCfX2Entry->SetNumber(x2);
   Double_t y2 = rad * TMath::Sin(TMath::Pi() * phimax / 180) + yc; 
   fPoCfY2Entry->SetNumber(y2);
*/

   if (fInit) ConnectSignals2Slots();
//   SetActive();
}

#if ROOTVERSION > 51303
//______________________________________________________________________________

void TArcEditor::ActivateBaseClassEditors(TClass* cl)
{
   // Add editors to fGedFrame and exclude TLineEditor.

//   fGedEditor->ExcludeClassEditor(TAttFill::Class());
   TGedFrame::ActivateBaseClassEditors(cl);
}
#endif
//______________________________________________________________________________

void TArcEditor::Redraw()
{
   if (fArc && fArc->TestBit(TObject::kNotDeleted)) {
//      if (fUseCenterPhi) 
      DoCenterPhi();
//      else               DoPointsOnCf();
   } else {
      std::cout<< "Arc is deleted" << std::endl;
   }
}
//______________________________________________________________________________

void TArcEditor::DoRedraw()
{
//   std::cout << "TArcEditor::DoRedraw() " << std::endl;
   Double_t rad = fArc->GetR1();
   fRadiusEntry->SetNumber(rad);

   Double_t phimin = fArc->GetPhimin();
   fPhiminEntry->SetNumber(phimin);

   Double_t phimax = fArc->GetPhimax();
   fPhimaxEntry->SetNumber(phimax);

   Double_t xc = fArc->GetX1();
   fCenterXEntry->SetNumber(xc);

   Double_t yc = fArc->GetY1();
   fCenterYEntry->SetNumber(yc);
/*   
   Double_t x1 = rad * TMath::Cos(TMath::Pi() * phimin / 180) + xc; 
   fPoCfX1Entry->SetNumber(x1);
   Double_t y1 = rad * TMath::Sin(TMath::Pi() * phimin / 180) + yc; 
   fPoCfY1Entry->SetNumber(y1);
   Double_t x2 = rad * TMath::Cos(TMath::Pi() * phimax / 180) + xc; 
   fPoCfX2Entry->SetNumber(x2);
   Double_t y2 = rad * TMath::Sin(TMath::Pi() * phimax / 180) + yc; 
   fPoCfY2Entry->SetNumber(y2);
*/
   if (fArc && fArc->TestBit(TObject::kNotDeleted)) {
//      if (fUseCenterPhi) 
      DoCenterPhi();
//      else               DoPointsOnCf();
   } else {
      std::cout<< "Arc is deleted" << std::endl;
   }
}
//______________________________________________________________________________

void TArcEditor::DoCenterPhi()
{
   // Slot connected to set center .
//   std::cout << "TArcEditor::DoCenterPhi() " << std::endl;

   fArc->SetX1    ((Double_t)fCenterXEntry->GetNumber()); 
   fArc->SetY1    ((Double_t)fCenterYEntry->GetNumber());
   fArc->SetR1    ((Double_t)fRadiusEntry->GetNumber());
   fArc->SetR2    ((Double_t)fRadiusEntry->GetNumber());
   fArc->SetPhimin((Double_t)fPhiminEntry->GetNumber());
   fArc->SetPhimax((Double_t)fPhimaxEntry->GetNumber());
   fArc->SetNoEdges(fDrawEdgesEntry->GetState() == kButtonUp);
   fArc->Paint(fArc->GetDrawOption());
   Update();
/*
   Double_t rad = fArc->GetR1();
   Double_t xc = fArc->GetX1();
   Double_t yc = fArc->GetY1();
   Double_t phimin = fArc->GetPhimin();
   Double_t phimax = fArc->GetPhimax();

   if (!fUseCenterPhi) {
      std::cout << "TArcEditor:: set  PoCfX1" << std::endl;

      fPoCfX1Entry->SetNumber(rad * TMath::Cos(TMath::Pi() * phimin / 180) + xc);
      fPoCfY1Entry->SetNumber(rad * TMath::Sin(TMath::Pi() * phimin / 180) + yc);
      fPoCfX2Entry->SetNumber(rad * TMath::Cos(TMath::Pi() * phimax / 180) + xc);
      fPoCfY2Entry->SetNumber(rad * TMath::Sin(TMath::Pi() * phimax / 180) + yc);
   }
*/
}
/*
//______________________________________________________________________________
Bool_t PoCftoCenterPhi(Double_t x1, Double_t y1, 
                       Double_t x2, Double_t y2, Double_t r, Int_t sense,
                       Double_t *xcenter, Double_t *ycenter, 
                       Double_t *phimin, Double_t *phimax)
{
   // calculate center, given 2 points on circle and radius
   std::cout << "PoCftoCenterPhi: " << x1<<" " << y1<< " "<< x2 <<" " <<y2 << " " << r << std::endl; 
   Double_t xc, yc;
   Bool_t ok = kTRUE;
   if (x1 == x2) {
   	Double_t arg = r*r - (y2-y1)*(y2-y1)/4;
   	if (arg <= 0) {
      	std::cout << "No real solution" << std::endl;
      	xc = yc = 0;
         ok = kFALSE;
   	} else {
      	xc = x1 - sense * TMath::Sqrt(arg);
      	yc = 0.5*(y2+y1);
   	}
   } else {
   	Double_t a = 0.5 * ( x1*x1 - x2*x2 + y1*y1  - y2*y2) / (x1 - x2);
   	Double_t b =  (y1-y2) / (x2-x1);
   	Double_t A = 1 + b*b;
   	Double_t B = -2*y1 + 2*a*b - 2*b*x1;
   	Double_t C = a*a - 2*x1*a + x1*x1 + y1*y1 - r*r;
//      Double_t dist = TMath::Sqrt((x1 - x2)*(x1 - x2) + (y1-y2)*(y1-y2));
//   	std::cout << dist << " "  << A << " " << B << " " << C << std::endl;
      Double_t arg = B*B/4 - A*C;
      if (arg <=0 ) {
         std::cout << "No real solution" << std::endl;
         xc = yc = 0;
         ok = kFALSE;
      } else {
         yc = (-0.5*B  - sense * TMath::Sqrt(arg)) / A;
   	   xc = a + b * yc;
      }
   } 
   Double_t phi1 = 0;
   Double_t phi2 = 0;
   if (ok) {
      Double_t cosa = (x1 - xc)/r;
      Double_t sina = (y1 - yc)/r;
      phi1  = TMath::ATan2(sina,cosa);
      if(phi1 < 0) phi1 += 2 * TMath::Pi();
      phi1 = phi1 * 180 / TMath::Pi();
      cosa = (x2 - xc)/r;
      sina = (y2 - yc)/r;
      phi2  = TMath::ATan2(sina,cosa);
      if(phi2 < 0) phi2 += 2 * TMath::Pi();
      phi2 = phi2 * 180 / TMath::Pi();
      std::cout << xc << " " << yc<< " " << phi1<< " " << phi2 << std::endl;
   }
   *xcenter = xc;
   *ycenter = yc;
   *phimin = phi1;
   *phimax = phi2;
   return ok;
}
//______________________________________________________________________________
void TArcEditor::DoPointsOnCf()
{
   // Slot connected to points on circumference setting.

   std::cout << "TArcEditor::DoPointsOnCf() " << std::endl;
   Double_t x1 = (Double_t)fPoCfX1Entry->GetNumber();
   Double_t y1 = (Double_t)fPoCfY1Entry->GetNumber();
   Double_t x2 = (Double_t)fPoCfX2Entry->GetNumber();
   Double_t y2 = (Double_t)fPoCfY2Entry->GetNumber();
   Double_t r  = (Double_t)fRadiusEntry->GetNumber();
   Double_t xc, yc, phi1, phi2;
   Int_t sense = +1;
   if (fSenseEntry->GetState() == kButtonUp) sense = -1;
   Bool_t ok = PoCftoCenterPhi(x1, y1, x2, y2, r, sense, &xc, &yc, &phi1, &phi2);
   if (ok) {
      fArc->SetPhimin(phi1);
      fArc->SetPhimax(phi2);
      fArc->SetX1(xc); 
      fArc->SetY1(yc);
      fArc->SetR1(r);
      fArc->SetR2(r);
      fArc->SetNoEdges(fDrawEdgesEntry->GetState() == kButtonUp);
      fArc->Paint(fArc->GetDrawOption());
      Update();
		fCenterXEntry->SetNumber(xc);
		fCenterYEntry->SetNumber(yc);
//		fRadiusEntry->SetNumber(r);
//		fRadiusEntry->SetNumber(r);
		fPhiminEntry->SetNumber(phi1);
		fPhimaxEntry->SetNumber(phi2);
   }
//   DoCenterPhi();
}
//______________________________________________________________________________
void TArcEditor::DoUsePointsOnCf()
{
   fUseCenterPhi = kFALSE;
   fUseCenterPhiEntry->SetState(kButtonUp);
}
//______________________________________________________________________________
void TArcEditor::DoUseCenterPhi()
{
   fUseCenterPhi = kTRUE;
   fUsePoCfEntry->SetState(kButtonUp);
}
*/
//______________________________________________________________________________

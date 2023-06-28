// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TextBoxEditor.cxx
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
//  TextBoxEditor                                                     //
//                                                                      //
//  Implements GUI for editing Arc attributes: radius, phi1, phi2. //                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/TextBoxEditor.gif">
*/
//End_Html


#include "TextBoxEditor.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TRegexp.h"
#include "TGNumberEntry.h"
#include "TextBox.h"
#include "TVirtualPad.h"
#include "iostream"

ClassImp(TGedFrame)
ClassImp(TextBoxEditor)

enum {
   kCRLA_X1,
   kCRLA_Y1,
   kCRLA_X2,
   kCRLA_Y2
};

//______________________________________________________________________________
#if ROOTVERSION < 51304
TextBoxEditor::TextBoxEditor(const TGWindow *p, Int_t id, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, id, width, height, options | kVerticalFrame, back)
#else
TextBoxEditor::TextBoxEditor(const TGWindow *p, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, width, height, options | kVerticalFrame, back)
#endif
{
   // Constructor of Arc GUI.

   fTextBox = 0;
   
   MakeTitle("TextBox");

   TGCompositeFrame *f3 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f3, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
   TGLabel *fX1Label = new TGLabel(f3, "      X1:");
   f3->AddFrame(fX1Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fX1Entry = new TGNumberEntry(f3, 0.02, 7, kCRLA_X1, 
                                    TGNumberFormat::kNESRealThree,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
   fX1Entry->GetNumberEntry()->SetToolTipText("Set lower left  X of textbox.");
   f3->AddFrame(fX1Entry, new TGLayoutHints(kLHintsLeft, 18, 1, 1, 1));
	
   TGCompositeFrame *f4 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f4, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
   TGLabel *fY1Label = new TGLabel(f4, "      Y1:");
   f4->AddFrame(fY1Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fY1Entry = new TGNumberEntry(f4, 0.02, 7, kCRLA_Y1, 
                                    TGNumberFormat::kNESRealThree,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
   fY1Entry->GetNumberEntry()->SetToolTipText("Set lower left  Y of textbox.");
   f4->AddFrame(fY1Entry, new TGLayoutHints(kLHintsLeft, 18, 1, 1, 1));
	
   TGCompositeFrame *f5 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f5, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
   TGLabel *fX2Label = new TGLabel(f5, "      X2:");
   f5->AddFrame(fX2Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fX2Entry = new TGNumberEntry(f5, 0.02, 7, kCRLA_X2, 
                                    TGNumberFormat::kNESRealThree,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
   fX2Entry->GetNumberEntry()->SetToolTipText("Set upper right  X of textbox.");
   f5->AddFrame(fX2Entry, new TGLayoutHints(kLHintsLeft, 18, 1, 1, 1));

   TGCompositeFrame *f6 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   AddFrame(f6, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
   TGLabel *fY2Label = new TGLabel(f6, "      Y2:");
   f6->AddFrame(fY2Label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 8, 0, 1, 1));
   fY2Entry = new TGNumberEntry(f6, 0.02, 7, kCRLA_Y2, 
                                    TGNumberFormat::kNESRealThree,
                                    TGNumberFormat::kNEANonNegative, 
                                    TGNumberFormat::kNELNoLimits);
   fY2Entry->GetNumberEntry()->SetToolTipText("Set upper right Y of textbox.");
   f6->AddFrame(fY2Entry, new TGLayoutHints(kLHintsLeft, 18, 1, 1, 1));
	
	TGCompositeFrame *f7 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
	AddFrame(f7, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
	TGLabel *fCRadLabel = new TGLabel(f7, "Corn Rad:");
	f7->AddFrame(fCRadLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 0, 1, 1));
	fCRadEntry = new TGNumberEntry(f7, 0.02, 7, kCRLA_Y2, 
										  TGNumberFormat::kNESRealThree,
										  TGNumberFormat::kNEANonNegative, 
										  TGNumberFormat::kNELNoLimits);
										  fY2Entry->GetNumberEntry()->SetToolTipText("Set corner radius of textbox.");
										  f7->AddFrame(fCRadEntry, new TGLayoutHints(kLHintsLeft, 12, 1, 1, 1));
										  
#if ROOTVERSION < 51304
   TClass *cl = TextBox::Class();
   TGedElement *ge = new TGedElement;
   ge->fGedFrame = this;
   ge->fCanvas = 0;
   cl->GetEditorList()->Add(ge);
#endif
}

//______________________________________________________________________________
TextBoxEditor::~TextBoxEditor()
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
void TextBoxEditor::ConnectSignals2Slots()
{
   // Connect signals to slots.

   fX1Entry->Connect("ValueSet(Long_t)", "TextBoxEditor", this, "Redraw()");
   (fX1Entry->GetNumberEntry())->Connect("ReturnPressed()", "TextBoxEditor", this, "Redraw()");
   fY1Entry->Connect("ValueSet(Long_t)", "TextBoxEditor", this, "Redraw()");
   (fY1Entry->GetNumberEntry())->Connect("ReturnPressed()", "TextBoxEditor", this, "Redraw()");
   fX2Entry->Connect("ValueSet(Long_t)", "TextBoxEditor", this, "Redraw()");
   (fX2Entry->GetNumberEntry())->Connect("ReturnPressed()", "TextBoxEditor", this, "Redraw()");
   fY2Entry->Connect("ValueSet(Long_t)", "TextBoxEditor", this, "Redraw()");
   (fY2Entry->GetNumberEntry())->Connect("ReturnPressed()", "TextBoxEditor", this, "Redraw()");
	fCRadEntry->Connect("ValueSet(Long_t)", "TextBoxEditor", this, "SetCRadius()");
	(fCRadEntry->GetNumberEntry())->Connect("ReturnPressed()", "TextBoxEditor", this, "SetCRadius()");
	
   fInit = kFALSE;
}

//______________________________________________________________________________
#if ROOTVERSION < 51304
void TextBoxEditor::SetModel(TVirtualPad* pad, TObject* obj, Int_t)
{
   fModel = 0;
   fPad = 0;
   if (obj == 0 || !obj->InheritsFrom("TextBox")) {
      SetActive(kFALSE);
      return;
   }

   fModel = obj;
   fPad = pad;
   
   fTextBox = (TextBox *)fModel;
#else 

void TextBoxEditor::SetModel(TObject* obj)
{
   fTextBox = (TextBox *)obj;
#endif
   // Pick up the used curly arc attributes.
//   std::cout << "TextBoxEditor::SetModel() " << std::endl;
   Double_t x1 = fTextBox->GetX1();
   fX1Entry->SetNumber(x1);
   Double_t y1 = fTextBox->GetY1();
   fY1Entry->SetNumber(y1);
   Double_t x2 = fTextBox->GetX2();
   fX2Entry->SetNumber(x2);
   Double_t y2 = fTextBox->GetY2();
   fY2Entry->SetNumber(y2);
	Double_t cr = fTextBox->GetCornerRadius();
	fY2Entry->SetNumber(cr);
	if (fInit) ConnectSignals2Slots();
}

#if ROOTVERSION > 51303
//______________________________________________________________________________

void TextBoxEditor::ActivateBaseClassEditors(TClass* cl)
{
   // Add editors to fGedFrame and exclude TLineEditor.

   TGedFrame::ActivateBaseClassEditors(cl);
}
#endif

//______________________________________________________________________________

void TextBoxEditor::SetCRadius()
{
	if (!fTextBox || !fTextBox->TestBit(TObject::kNotDeleted))
		return;
	fTextBox->SetCornerRadius((Double_t)fCRadEntry->GetNumber());
	TString dopt(fTextBox->GetDrawOption());
	if ( fTextBox->GetCornerRadius() > 0) {
		if (!dopt.Contains("arc", TString::kIgnoreCase) ) {
			dopt += "arc";
			fTextBox->SetDrawOption(dopt);
		}
	} else {
		if (dopt.Contains("arc", TString::kIgnoreCase) ) {
			TRegexp ar("arc");
			dopt(ar) = "";
		}	
	}
	fTextBox->SetDrawOption(dopt);
	Update();
}
//______________________________________________________________________________

void TextBoxEditor::Redraw()
{
//	std::cout<< "TextBox Redraw" << std::endl;
   if (fTextBox && fTextBox->TestBit(TObject::kNotDeleted)) {
		fTextBox->SetX1    ((Double_t)fX1Entry->GetNumber()); 
		fTextBox->SetY1    ((Double_t)fY1Entry->GetNumber()); 
		fTextBox->SetX2    ((Double_t)fX2Entry->GetNumber()); 
		fTextBox->SetY2    ((Double_t)fY2Entry->GetNumber());
		
		fTextBox->SetPaveNDC(fTextBox);
		Update();
   } else {
      std::cout<< "TextBox is deleted" << std::endl;
   }
}

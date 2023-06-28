// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: HprElementEditor.cxx
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
//  HprElementEditor                                                     //
//                                                                      //
//  Implements GUI for editing Arc attributes: radius, phi1, phi2. //                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/HprElementEditor.gif">
*/
//End_Html


#include "HprElementEditor.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "HprElement.h"
#include "TVirtualPad.h"
#include "iostream"

ClassImp(TGedFrame)
ClassImp(HprElementEditor)

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
HprElementEditor::HprElementEditor(const TGWindow *p, Int_t id, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, id, width, height, options | kVerticalFrame, back)
#else
HprElementEditor::HprElementEditor(const TGWindow *p, Int_t width,
                           Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, width, height, options | kVerticalFrame, back)
#endif
{
   // Constructor of Arc GUI.
//   Pixel_t lblue;
//   fClient->GetColorByName("LightSteelBlue1", lblue);

   fHprElement = 0;

   TGGroupFrame *fg0 = new TGGroupFrame(this, "HprElement");

   TGCompositeFrame *f4 = new TGCompositeFrame(fg0, 80, 20, kHorizontalFrame);
   fg0->AddFrame(f4, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));

   TGLabel *fPlaneLabel = new TGLabel(f4, "Plane ");
   f4->AddFrame(fPlaneLabel, new TGLayoutHints(kLHintsNormal, 1, 0, 3, 5));
   fPlaneEntry = new TGNumberEntry(f4, 0, 7, kCRLA_FMIN,
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEANonNegative,
                                    TGNumberFormat::kNELLimitMinMax, 0, 100);
   fPlaneEntry->GetNumberEntry()->SetToolTipText("Set Plane number [0,100]");
   f4->AddFrame(fPlaneEntry, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));

   TGCompositeFrame *f5 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   fg0->AddFrame(f5, new TGLayoutHints(kLHintsTop, 1, 1, 3, 0));
   TGLabel *fVisLabel = new TGLabel(f5, "Visible");
   f5->AddFrame(fVisLabel, new TGLayoutHints(kLHintsNormal, 0, 0, 3, 5));
   fVisEntry = new TGNumberEntry(f5, 0, 7, kCRLA_FMIN,
                                    TGNumberFormat::kNESInteger);
   fVisEntry->GetNumberEntry()->SetToolTipText("Set Visiblity");
   f5->AddFrame(fVisEntry, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
//   fg0->SetBackgroundColor(lblue);
   this->AddFrame(fg0, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

#if ROOTVERSION < 51304
   TClass *cl = HprElement::Class();
   TGedElement *ge = new TGedElement;
   ge->fGedFrame = this;
   ge->fCanvas = 0;
   cl->GetEditorList()->Add(ge);
#endif
}

//______________________________________________________________________________
HprElementEditor::~HprElementEditor()
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
void HprElementEditor::ConnectSignals2Slots()
{
   // Connect signals to slots.

   fPlaneEntry->Connect("ValueSet(Long_t)", "HprElementEditor", this, "DoSetPlane()");
   (fPlaneEntry->GetNumberEntry())->Connect("ReturnPressed()", "HprElementEditor", this, "DoSetPlane()");
   fVisEntry->Connect("ValueSet(Long_t)", "HprElementEditor", this, "DoSetVisibility()");
   (fVisEntry->GetNumberEntry())->Connect("ReturnPressed()", "HprElementEditor", this, "DoSetVisibility()");
   fInit = kFALSE;
}

//______________________________________________________________________________
#if ROOTVERSION < 51304
void HprElementEditor::SetModel(TVirtualPad* pad, TObject* obj, Int_t)
{
   fModel = 0;
   fPad = 0;
   if (obj == 0 || !obj->InheritsFrom("HprElement")) {
      SetActive(kFALSE);
      return;
   }

   fModel = obj;
   fPad = pad;

   fHprElement = (HprElement *)fModel;
#else

void HprElementEditor::SetModel(TObject* obj)
{
   fHprElement = dynamic_cast<HprElement *>(obj);
#endif
   // Pick up the used curly arc attributes.
//   std::cout << "HprElementEditor::SetModel() "<< fHprElement << std::endl;
   fPlaneEntry->SetNumber(fHprElement->GetPlane());
   fVisEntry->SetNumber(fHprElement->GetVisibility());
   if (fInit) ConnectSignals2Slots();
//   SetActive();
}

#if ROOTVERSION > 51303
//______________________________________________________________________________

void HprElementEditor::ActivateBaseClassEditors(TClass* cl)
{
   // Add editors to fGedFrame and exclude TLineEditor.

//   fGedEditor->ExcludeClassEditor(TAttFill::Class());
   TGedFrame::ActivateBaseClassEditors(cl);
}
#endif
//____________________________________________________________________________

void HprElementEditor::DoSetPlane()
{
   fHprElement->SetPlane((Int_t)fPlaneEntry->GetNumber());
}
//___________________________________________________________________________

void HprElementEditor::DoSetVisibility()
{
   fHprElement->SetVisibility((Int_t)fVisEntry->GetNumber());
}

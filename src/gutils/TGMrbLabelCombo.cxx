//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLabelCombo.cxx
// Purpose:        MARaBOU graphic utilities: a labelled combo box
// Description:    Implements class methods to handle a labelled combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbLabelCombo.cxx,v 1.12 2011-09-28 12:22:02 Marabou Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbLabelCombo.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGMrbLabelCombo.h"

ClassImp(TGMrbLabelCombo)

TGMrbLabelCombo::TGMrbLabelCombo(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Entries,
												Int_t FrameId, Int_t Selected,
												Int_t Width, Int_t Height, Int_t ComboWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * ComboGC,
												TGMrbLayout * UpDownBtnGC,
												Bool_t BeginEndBtns,
												UInt_t FrameOptions,
												UInt_t ComboOptions) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo
// Purpose:        Define a label together with a combo box
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TMrbLofNamedX * Entries     -- combo box entries
//                 Int_t FrameId               -- id to be used with slignal/slot communication
//                 Int_t Selected              -- id of item to be displayed at startup
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t ComboWidth            -- combo width
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * ComboGC       -- ... (combo box)
//                 TGMrbLayout * UpDownBtnGC   -- ... (up/down btns) - adds buttons "<" and ">"
//                 Bool_t BeginEndBtns         -- if kTRUE add two additional buttons "<<" and ">>"
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t ComboOptions         -- options to configure the combo box
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;

	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	ComboGC = this->SetupGC(ComboGC, FrameOptions);

	fFrameId = FrameId;

	fUp = NULL;
	fDown = NULL;
	fBegin = NULL;
	fEnd = NULL;

	Int_t comboWidth = ComboWidth;

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	if (UpDownBtnGC) {
		TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) btnLayout);
		if (BeginEndBtns) {
			fEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbComboButtonEnd, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fEnd);
			fEnd->ChangeBackground(UpDownBtnGC->BG());
			fEnd->SetToolTipText("->End", 500);
			this->AddFrame(fEnd, btnLayout);
			fEnd->Connect("Clicked()", this->ClassName(), this, "EndButtonPressed()");
			comboWidth -= fEnd->GetWidth();
		}
		fUp = new TGPictureButton(this, fClient->GetPicture("arrow_right.xpm"), kGMrbComboButtonUp, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fUp);
		fUp->ChangeBackground(UpDownBtnGC->BG());
		fUp->SetToolTipText("Increment", 500);
		this->AddFrame(fUp, btnLayout);
		fUp->Connect("Clicked()", this->ClassName(), this, "UpButtonPressed()");
		comboWidth -= fUp->GetWidth();
		fDown = new TGPictureButton(this, fClient->GetPicture("arrow_left.xpm"), kGMrbComboButtonDown, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fDown);
		fDown->ChangeBackground(UpDownBtnGC->BG());
		fDown->SetToolTipText("Decrement", 500);
		this->AddFrame(fDown, btnLayout);
		fDown->Connect("Clicked()", this->ClassName(), this, "DownButtonPressed()");
		comboWidth -= fDown->GetWidth();
		if (BeginEndBtns) {
			fBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbComboButtonBegin, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fBegin);
			fBegin->ChangeBackground(UpDownBtnGC->BG());
			fBegin->SetToolTipText("->Start", 500);
			this->AddFrame(fBegin, btnLayout);
			fBegin->Connect("Clicked()", this->ClassName(), this, "BeginButtonPressed()");
			comboWidth -= fBegin->GetWidth();
		}
	}

	TGLayoutHints * comboLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
	fHeap.AddFirst((TObject *) comboLayout);
	fCombo = new TGComboBox(this, FrameId, ComboOptions, ComboGC->BG());
	fHeap.AddFirst((TObject *) fCombo);
	this->AddFrame(fCombo, comboLayout);
	fEntries.Delete();
	this->AddEntries(Entries);
	fCombo->Select(Selected, kFALSE);
	fCombo->Resize(comboWidth, Height);
	fCombo->Connect("Selected(Int_t)", this->ClassName(), this, "SelectionChanged(Int_t)");
	fCombo->Layout();
}

Bool_t TGMrbLabelCombo::AddEntries(TMrbLofNamedX * Entries) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::AddEntries
// Purpose:        Add entries to the combo box
// Arguments:      TMrbLofNamedX * Entries   -- list of entries to be added
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills the combo box.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * entry;
	entry = (TMrbNamedX *) Entries->First();
	while (entry) {
		fEntries.AddNamedX(entry);
		fCombo->AddEntry(entry->GetName(), entry->GetIndex());
		entry = (TMrbNamedX *) Entries->After(entry);
	}
	return(kTRUE);
}

const Char_t * TGMrbLabelCombo::GetText() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::GetText
// Purpose:        Get text from selected field
// Arguments:      --
// Results:        Char_t * Text   -- field text
// Exceptions:     
// Description:    Returns text of selected combo field
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGTextLBEntry * cf = (TGTextLBEntry *) fCombo->GetSelectedEntry();
	return (cf == NULL) ? "" : cf->GetText()->GetString();
}

void TGMrbLabelCombo::SetText(const Char_t * Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::SetText
// Purpose:        Set text of selected field
// Arguments:      Char_t * Text   -- field text
// Results:        --
// Exceptions:     
// Description:    Sets text of selected combo field
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGTextLBEntry * cf = (TGTextLBEntry *) fCombo->GetSelectedEntry();
	if (cf != NULL) {
		TGString * s = (TGString *) cf->GetText();
		s->SetString(Text);
	}
	return;
}

void TGMrbLabelCombo::BeginButtonPressed() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::BeginButtonPressed
// Purpose:        Slot method
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called upon ButtonPressed() events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fEntries.First();
	fCombo->Select(nx->GetIndex());
}

void TGMrbLabelCombo::EndButtonPressed() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::EndButtonPressed
// Purpose:        Slot method
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called upon ButtonPressed() events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fEntries.Last();
	fCombo->Select(nx->GetIndex());
}

void TGMrbLabelCombo::UpButtonPressed() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::UpButtonPressed
// Purpose:        Slot method
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called upon Clicked() events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = fCombo->GetSelected();
	TMrbNamedX * nx = fEntries.FindByIndex(idx);
	if (nx == fEntries.Last()) {
		nx = (TMrbNamedX *) fEntries.First();
	} else {
		nx = (TMrbNamedX *) fEntries.At(fEntries.IndexOf(nx) + 1);
	}
	fCombo->Select(nx->GetIndex());
}

void TGMrbLabelCombo::DownButtonPressed() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::DownButtonPressed
// Purpose:        Slot method
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called upon Clicked() events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = fCombo->GetSelected();
	TMrbNamedX * nx = fEntries.FindByIndex(idx);
	if (nx == fEntries.First()) {
		nx = (TMrbNamedX *) fEntries.Last();
	} else {
		nx = (TMrbNamedX *) fEntries.At(fEntries.IndexOf(nx) - 1);
	}
	fCombo->Select(nx->GetIndex());
}

void TGMrbLabelCombo::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables up/down buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fUp && fDown) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fUp->SetState(btnState);
		fDown->SetState(btnState);
	}
}

void TGMrbLabelCombo::SelectionChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::SelectionChanged
// Purpose:        Signal handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t Selection  -- selection index
// Results:        --
// Exceptions:     
// Description:    Emits signal on "selection changed" 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   Long_t args[2];

   args[0] = FrameId;
   args[1] = Selection;

   this->Emit("SelectionChanged(Int_t, Int_t)", args);
}

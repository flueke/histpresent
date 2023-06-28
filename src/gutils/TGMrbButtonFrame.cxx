//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbButtonFrame.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of buttons
// Description:    Implements basic class methods to handle a frame of buttons
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbButtonFrame.cxx,v 1.14 2008-10-14 17:27:06 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TGToolTip.h"
#include "TGMrbButtonFrame.h"

ClassImp(TGMrbButtonFrame)
ClassImp(TGMrbSpecialButton)

TGMrbButtonFrame::TGMrbButtonFrame(const TGWindow * Parent, const Char_t * Label, UInt_t ButtonType,
											TMrbLofNamedX * Buttons, Int_t FrameId, Int_t NofCL,
											Int_t Width, Int_t Height,
											TGMrbLayout * FrameGC, TGMrbLayout * LabelGC, TGMrbLayout * ButtonGC,
											UInt_t FrameOptions, UInt_t ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Arguments:      TGWindow * Parent             -- parent widget
//                 Char_t * Label                -- label text
//                 UInt_t ButtonType             -- type of button (radio/check, composite/group)
//                 TMrbLofNamedX Buttons         -- button names and their indices
//                 Int_t FrameId                 -- frame id (buttons will add their own id)
//                 Int_t NofCL                   -- button grid CxL: number of columns/lines
//                 Int_t Width                   -- frame width
//                 Int_t Height                  -- frame height
//                 TGMrbLayout * FrameLayout     -- frame layout
//                 TGMrbLayout * LabelLayout     -- label layout
//                 TGMrbLayout * ButtonLayout    -- button layout
//                 UInt_t FrameOptions           -- frame options
//                 UInt_t ButtonOptions          -- button options
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fParent = Parent;
	fLabel = Label ? Label : "";
	fType = ButtonType;
	fNofCL = NofCL;
	fWidth = Width;
	fHeight = Height;
	fFrameGC = FrameGC;
	fLabelGC = this->SetupGC(LabelGC, FrameOptions);
	fButtonGC = this->SetupGC(ButtonGC, FrameOptions);
	fFrameOptions = FrameOptions;
	fButtonOptions = ButtonOptions;
	fFrameId = FrameId;

	fButtons.Delete();

	if (Buttons != NULL) {
		TIterator * bIter = Buttons->MakeIterator();
		TMrbNamedX * namedX;
		while (namedX = (TMrbNamedX *) bIter->Next()) fButtons.AddNamedX(namedX);
	}
};

void TGMrbButtonFrame::PlaceButtons() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::PlaceButtons
// Purpose:        Place buttons
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Places buttons in the frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGLayoutHints * frameLayout;
	UInt_t btnOptions, subOptions;

	TGButton * btn = NULL;
	TGCheckButton * cbtn;
	TGRadioButton * rbtn;
	TGTextButton * tbtn;
	TGPictureButton * pbtn;
	TGMrbSpecialButton * sbtn1, * sbtn2;
	
	Int_t btnId;

	subOptions = fFrameOptions & (kVerticalFrame | kHorizontalFrame | kChildFrame);
 	if (fNofCL > 1) {
		btnOptions = (fFrameOptions & kVerticalFrame) ? kHorizontalFrame : kVerticalFrame;
	} else {
		btnOptions = fFrameOptions & (kVerticalFrame | kHorizontalFrame);
	}
	subOptions |= kChildFrame;

	if (fWidth == 0) fWidth = fFrame->GetDefaultWidth();
	if (fHeight == 0) fHeight = fFrame->GetDefaultHeight();

	if (fNofCL > 1) {
		if (fType & kGMrbTextButton)	frameLayout = new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 0, 0, 0);
		else							frameLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 0);
		fHeap.AddFirst((TObject *) frameLayout);
	} else {
		frameLayout = fFrameGC->LH();
	}
	
	TGCompositeFrame * btnFrame = new TGCompositeFrame(fFrame, fWidth, fHeight, btnOptions, fFrameGC->BG());
	fHeap.AddFirst((TObject *) btnFrame);
	fFrame->AddFrame(btnFrame, frameLayout);

	Int_t btnNo = 0;
	TGCompositeFrame * subFrame = NULL;
	TMrbNamedX * nx;
	TIterator * bIter = fButtons.MakeIterator();
	while (nx = (TMrbNamedX *) bIter->Next()) {
		if (fNofCL > 1) {
			if ((btnNo % fNofCL) == 0) {
				subFrame = new TGCompositeFrame(btnFrame, fWidth, fHeight, subOptions, fFrameGC->BG());
				fHeap.AddFirst((TObject *) subFrame);
				btnFrame->AddFrame(subFrame, frameLayout);
			}
		} else {
			subFrame = btnFrame;
		}
		btnId = nx->GetIndex();
		Int_t btnType = fType & (kGMrbCheckButton | kGMrbRadioButton | kGMrbTextButton | kGMrbPictureButton);
		switch (btnType) {
			case kGMrbCheckButton:
				cbtn = new TGCheckButton(subFrame,	nx->GetName(),
													btnId,
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) cbtn);
				btn = (TGButton *) cbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("CheckButtonClicked(Int_t=%d)", btnId));
				break;
			case kGMrbRadioButton:
				rbtn = new TGRadioButton(subFrame,	nx->GetName(),
													btnId,
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) rbtn);
				btn = (TGButton *) rbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("RadioButtonClicked(Int_t=%d)", btnId));
				break;
			case kGMrbTextButton:
				tbtn = new TGTextButton(subFrame,	nx->GetName(),
													btnId,
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) tbtn);
				btn = (TGButton *) tbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("TextButtonClicked(Int_t=%d)", btnId));
				break;
			case kGMrbPictureButton:
				pbtn = new TGPictureButton(subFrame, fParentClient->GetPicture(nx->GetName()),
													btnId,
													fButtonGC->GC(), fButtonOptions);
				fHeap.AddFirst((TObject *) pbtn);
				btn = (TGButton *) pbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("PictureButtonClicked(Int_t=%d)", btnId));
				break;
		}
		btn->ChangeBackground(fButtonGC->BG());
		subFrame->AddFrame(btn, fButtonGC->LH());
		nx->AssignObject(btn);
		if (nx->HasTitle()) btn->SetToolTipText(nx->GetTitle(), 500);
		btnNo++;
	}

	if (fType & kGMrbCheckButton && fLofSpecialButtons) {
		sbtn1 = (TGMrbSpecialButton *) fLofSpecialButtons->First();
		while (sbtn1) {
			sbtn2 = new TGMrbSpecialButton(	sbtn1->GetIndex(), sbtn1->GetName(), sbtn1->GetTitle(),
											sbtn1->GetPattern(), sbtn1->GetPicture());
			fButtons.Add(sbtn2);
			if (fNofCL > 1) {
				if ((btnNo % fNofCL) == 0) {
					subFrame = new TGCompositeFrame(btnFrame, fWidth, fHeight, subOptions, fFrameGC->BG());
					fHeap.AddFirst((TObject *) subFrame);
					btnFrame->AddFrame(subFrame, frameLayout);
				}
			} else {
				subFrame = btnFrame;
			}
			if (sbtn2->GetPicture() == NULL) {
				cbtn = new TGCheckButton(subFrame,	sbtn2->GetName(),
													sbtn2->GetIndex(),
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) cbtn);
				btn = (TGButton *) cbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("CheckButtonClicked(Int_t=%d)", sbtn2->GetIndex()));
			} else {
				pbtn = new TGPictureButton(subFrame, fParentClient->GetPicture(sbtn2->GetPicture()),
													sbtn2->GetIndex(),
													fButtonGC->GC(), fButtonOptions | kRaisedFrame);
				fHeap.AddFirst((TObject *) pbtn);
				btn = (TGButton *) pbtn;
				btn->Connect("Clicked()", "TGMrbButtonFrame", this, Form("PictureButtonClicked(Int_t=%d)", sbtn2->GetIndex()));
			}
			btn->ChangeBackground(fButtonGC->BG());
			subFrame->AddFrame(btn, new TGLayoutHints(kLHintsCenterY,	fButtonGC->LH()->GetPadLeft(),
															fButtonGC->LH()->GetPadRight(),
															fButtonGC->LH()->GetPadTop(),
															fButtonGC->LH()->GetPadBottom()));
			sbtn2->AssignObject(btn);
			if (sbtn2->HasTitle()) btn->SetToolTipText(sbtn2->GetTitle(), 500);
			btnNo++;
			sbtn1 = (TGMrbSpecialButton *) fLofSpecialButtons->After(sbtn1);
		}
	}
}
				
void TGMrbButtonFrame::ClearAll() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ClearAll
// Purpose:        Clear all buttons in the list
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Sets all buttons to inactive.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & kGMrbCheckButton) {
		TMrbNamedX * namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			if (button->GetState() == kButtonEngaged) button->SetState(kButtonUp);
		}
		this->ButtonPressed(0);
	}
}

void TGMrbButtonFrame::SetState(UInt_t Pattern, EButtonState State, Bool_t Emit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetState
// Purpose:        Mark button(s) as (in)active
// Arguments:      UInt_t Pattern       -- pattern describing button(s) to be set
//                 EButtonState State   -- button up or down
//                 Bool_t Emit          -- if signal is to be emitted
// Results:        
// Exceptions:     
// Description:    Sets button(s) given by pattern to (in)active.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & kGMrbRadioButton) {
		if (State == kButtonEngaged || State == kButtonDisabled) {
			TMrbNamedX *namedX;
			TIterator * bIter = fButtons.MakeIterator();
			while (namedX = (TMrbNamedX *) bIter->Next()) {
				TGButton * button = (TGButton *) namedX->GetAssignedObject();
				if ((namedX->GetIndex() & Pattern) == (UInt_t) namedX->GetIndex()) {
					button->SetState(State);
					if (Emit) this->ButtonPressed(namedX->GetIndex());
				}
			}
		} else {
			fRBState = 0;
			TMrbNamedX *namedX;
			TIterator * bIter = fButtons.MakeIterator();
			while (namedX = (TMrbNamedX *) bIter->Next()) {
				TGButton * button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) namedX->GetIndex() == Pattern) {
						button->SetState(State);
						if (Emit && State == kButtonDown) this->ButtonPressed(namedX->GetIndex());
						fRBState |= Pattern;
					} else {
						button->SetState(kButtonUp);
					}
				}
			}
		}
	} else if (fType & kGMrbCheckButton) {
		TGMrbSpecialButton * sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) Pattern = sbutton->GetPattern();
		if (State == kButtonEngaged || State == kButtonDisabled) {
			TMrbNamedX *namedX;
			TIterator * bIter = fButtons.MakeIterator();
			while (namedX = (TMrbNamedX *) bIter->Next()) {
				TGButton * button = (TGButton *) namedX->GetAssignedObject();
				if (namedX->GetIndex() & Pattern) button->SetState(State);
			}
		} else {
			TMrbNamedX *namedX;
			TIterator * bIter = fButtons.MakeIterator();
//			EButtonState invState = (State == kButtonUp) ? kButtonDown : kButtonUp;
			while (namedX = (TMrbNamedX *) bIter->Next()) {
				TGButton * button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if (namedX->GetIndex() & Pattern) {
						button->SetState(State);
					} else if (sbutton) {
						button->SetState(kButtonUp);
					}
				}
			}
		}
		if (Emit) this->ButtonPressed(this->GetActive());
	} else if (fType & kGMrbTextButton) {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			if ((UInt_t) namedX->GetIndex() == Pattern) {
				button->SetState(State);
				if (Emit) this->ButtonPressed(namedX->GetIndex());
			}
		}
	}
}

UInt_t TGMrbButtonFrame::GetActive() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::GetActive
// Purpose:        Get pattern of active button(s)
// Arguments:      --
// Results:        UInt_t Pattern    -- pattern of active buttons
// Exceptions:     
// Description:    Reads pattern of active buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t pattern = 0;
	if (fType & kGMrbRadioButton) {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			UInt_t bState = button->GetState();
			if (bState == kButtonDown && ((fRBState & namedX->GetIndex()) == 0)) pattern |= namedX->GetIndex();
		}
		if (pattern == 0) pattern = fRBState;
		this->SetState(pattern);
	} else {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			if (button->GetState() == kButtonDown) pattern |= namedX->GetIndex();
		}
	}
	return(pattern);
}

TMrbNamedX * TGMrbButtonFrame::GetActiveNx() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::GetActiveNx
// Purpose:        Get index and name of selected (radio) button
// Arguments:      --
// Results:        TMrbNamedX * SelBtn  -- selected button
// Exceptions:     
// Description:    Returns index and name of selected button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & kGMrbRadioButton) {
		UInt_t pattern = 0;
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			UInt_t bState = button->GetState();
			if (bState == kButtonDown && ((fRBState & namedX->GetIndex()) == 0)) pattern |= namedX->GetIndex();
		}
		if (pattern == 0) pattern = fRBState;
		this->SetState(pattern);
		return(fButtons.FindByIndex(pattern));
	} else {
		return(NULL);
	}
}

void TGMrbButtonFrame::FlipState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::FlipState
// Purpose:        Flip current check button state
// Arguments:      UInt_t Pattern       -- pattern describing button(s)
//                                         to be flipped
// Results:        
// Exceptions:     
// Description:    Flip state for given (check) button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & kGMrbCheckButton) {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			UInt_t idx;
			if ((idx = namedX->GetIndex()) & Pattern) {
				EButtonState state = button->GetState();
				if (state == kButtonDown) {
					button->SetState(kButtonUp);
				} else if (state == kButtonUp) {
					button->SetState(kButtonDown);
				}
			}
		}
		this->ButtonPressed(this->GetActive());
	}

}

void TGMrbButtonFrame::UpdateState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::UpdateState
// Purpose:        Update check button state
// Arguments:      UInt_t Pattern       -- pattern describing check button(s)
//                                         to be updated
// Results:        
// Exceptions:     
// Description:    Update state for given check button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & kGMrbTextButton) {
		this->ButtonPressed(Pattern);
	} else if (fType & kGMrbCheckButton) {
		TGMrbSpecialButton * sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) {
			UInt_t pat = sbutton->GetPattern();
			TMrbNamedX *namedX;
			TIterator * bIter = fButtons.MakeIterator();
			while (namedX = (TMrbNamedX *) bIter->Next()) {
				TGButton * button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) namedX->GetIndex() == Pattern) {
						button->SetState(kButtonDown);
					} else if (namedX->GetIndex() & pat) {
						button->SetState(kButtonDown);
					} else {
						button->SetState(kButtonUp);
					}
				}
			}
		}
		this->ButtonPressed(this->GetActive());
	} else if (fType & kGMrbRadioButton) {
		this->ButtonPressed(Pattern);
	} else if (fType & kGMrbPictureButton) {
		this->ButtonPressed(Pattern);
	}
}

TGButton * TGMrbButtonFrame::GetButton(Int_t ButtonIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::GetButton
// Purpose:        Get a button by its index
// Arguments:      Int_t ButtonIndex      -- button index
// Results:        TGButton * Button      -- addr of button or NULL
// Exceptions:     NULL if button not found
// Description:    Searches for a button by its index and return its address.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	if ((namedX = fButtons.FindByIndex(ButtonIndex)) == NULL) return(NULL);
	return((TGButton *) namedX->GetAssignedObject());
}

void TGMrbButtonFrame::SetButtonWidth(Int_t Width, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetButtonWidth
// Purpose:        Set label width of a button list
// Arguments:      Int_t Width          -- width
//                 Int_t ButtonIndex    -- button index
// Results:        
// Exceptions:     
// Description:    Sets width of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (ButtonIndex != 0) {
		TMrbNamedX *namedX;
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
		}
	} else {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
		}
	}
}

Int_t TGMrbButtonFrame::GetButtonWidth(Int_t ButtonIndex)  const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetButtonWidth
// Purpose:        Set label width of a button list
// Arguments:      Int_t ButtonIndex    -- button index
// Results:        Int_t Width          -- width
// Exceptions:     
// Description:    Gets width of specified button
//                 (if index = 0 -> max of all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t width = 0;
	if (ButtonIndex != 0) {
		TMrbNamedX *namedX;
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			width = button->GetWidth();
		}
	} else {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			Int_t wd = button->GetWidth();
			width = (wd > width) ? wd : width;
		}
	}
	return(width);
}

void TGMrbButtonFrame::ChangeButtonBackground(ULong_t Color, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ChangeButtonBackground
// Purpose:        Set background color of a button list
// Arguments:      ULong_t Color        -- color
//                 Int_t ButtonIndex   -- button index
// Results:        
// Exceptions:     
// Description:    Sets background color of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (ButtonIndex != 0) {
		TMrbNamedX *namedX;
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
		}
	} else {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGButton * button = (TGButton *) namedX->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
		}
	}
}

void TGMrbButtonFrame::JustifyButton(ETextJustification Justify, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::JustifyButton
// Purpose:        Justify button text
// Arguments:      ETextJustification Justify  -- how to justify the text
//                 Int_t ButtonIndex           -- button index
// Results:        
// Exceptions:     
// Description:    Justifies button text (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if ((fType & kGMrbTextButton) == 0) return;

	if (ButtonIndex != 0) {
		TMrbNamedX *namedX;
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGTextButton * button = (TGTextButton *) namedX->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
		}
	} else {
		TMrbNamedX *namedX;
		TIterator * bIter = fButtons.MakeIterator();
		while (namedX = (TMrbNamedX *) bIter->Next()) {
			TGTextButton * button = (TGTextButton *) namedX->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
		}
	}
}

TGMrbSpecialButton * TGMrbButtonFrame::FindSpecialButton(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::FindSpecialButton
// Purpose:        Search a special button
// Arguments:      Int_t Index      -- button index
// Results:        
// Exceptions:     
// Description:    Searches a special button by a given index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fLofSpecialButtons == NULL) return(NULL);
	
	TGMrbSpecialButton * sbtn;
	TIterator * sIter = fLofSpecialButtons->MakeIterator();
	while (sbtn = (TGMrbSpecialButton *) sIter->Next()) {
		if (sbtn->GetIndex() == Index) return(sbtn);
	}
	return(NULL);
}

void TGMrbButtonFrame::ButtonPressed(Int_t FrameId, Int_t Button) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ButtonPressed
// Purpose:        Signal handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t Button     -- button index
// Results:        --
// Exceptions:     
// Description:    Emits signal on "button pressed" 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Long_t args[2];

	args[0] = FrameId;
	args[1] = Button;

	this->Emit("ButtonPressed(Int_t, Int_t)", args);
}

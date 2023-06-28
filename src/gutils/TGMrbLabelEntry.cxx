//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLabelEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled entry
// Description:    Implements class methods to handle a labelled entry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbLabelEntry.cxx,v 1.25 2011-07-26 08:41:50 Marabou Exp $
// Date:
// Layout: A plain entry
//Begin_Html
/*
<img src=gutils/TGMrbLabelEntry1.gif>
<img src=gutils/TGMrbLabelEntry2.gif>
<img src=gutils/TGMrbLabelEntry3.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGMsgBox.h"
#include "TGButton.h"
#include "TGToolTip.h"
#include "TMrbLogger.h"
#include "TMrbString.h"
#include "TGMrbLabelEntry.h"

#include "SetColor.h"

ClassImp(TGMrbTextEntry)
ClassImp(TGMrbLabelEntry)

extern TMrbLogger * gMrbLog;		// access to message logging

TGMrbLabelEntry::TGMrbLabelEntry(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize, Int_t FrameId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * UpDownBtnGC,
												Bool_t BeginEndBtns,
												TMrbNamedX * Action, TGMrbLayout * ActionGC,
												TMrbLofNamedX * CheckBtns,
												TMrbLofNamedX * RadioBtns,
												UInt_t FrameOptions,
												UInt_t EntryOptions,
												Int_t NofEntries) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry
// Purpose:        Define a label together with an text entry field
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- size of text buffer in chars
//                 Int_t FrameId               -- id to be used with signals/slots
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- widht of the entry field
//                 Bool_t UpDownBtn            -- kTRUE: append up/down buttons
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * EntryGC       -- ... (entry)
//                 TGMrbLayout * UpDownBtnGC   -- ... (up/down btns) - adds buttons "<" and ">"
//                 Bool_t BeginEndBtns         -- if kTRUE add two additional buttons "<<" and ">>"
//                 TMrbNamedX * Action         -- adds an "action" button
//                 TGMrbLayout * ActionGC      -- graphic context & layout (action)
//                 TMrbLofNamedX * CheckBtns   -- adds a list of check buttons
//                 TMrbLofNamedX * RadioBtns   -- adds a list of radio buttons
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
//                 Int_t NofEntries            -- number of entries
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	EntryGC = this->SetupGC(EntryGC, FrameOptions);

	for (Int_t i = 0; i < kGMrbEntryNofEntries; i++) {
		fUp[i] = NULL;
		fDown[i] = NULL;
		fBegin[i] = NULL;
		fEnd[i] = NULL;
		fLowerLimit[i] = 0;
		fUpperLimit[i] = 0;
		fIncrement[i] = 1;
	}


	fFrameId = FrameId;

	fBase = 10;
	fWidth = EntryWidth/10;
	fPrecision = 2;
	fType = TGMrbLabelEntry::kGMrbEntryTypeChar;
	fLabel = NULL;

	fShowToolTip = kFALSE;
	fShowRange = kFALSE;

	Int_t entryWidth = EntryWidth;

	if (Label != NULL) {
		fLabel = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) fLabel);
		this->AddFrame(fLabel, LabelGC->LH());
		fLabel->SetTextJustify(kTextLeft);
	}

	if (RadioBtns) {
		TGLayoutHints * radioLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) radioLayout);
		fRadioBtns = new TGMrbRadioButtonList(this, NULL, RadioBtns, kGMrbEntryButtonRadio, 1, 0, Height, FrameGC);
		fHeap.AddFirst((TObject *) fRadioBtns);
		fRadioBtns->ChangeBackground(LabelGC->BG());
		this->AddFrame(fRadioBtns, radioLayout);
		entryWidth -= fRadioBtns->GetWidth();
	}

	if (CheckBtns) {
		TGLayoutHints * checkLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) checkLayout);
		fCheckBtns = new TGMrbCheckButtonList(this, NULL, CheckBtns, kGMrbEntryButtonCheck, 1, 0, Height, FrameGC);
		fHeap.AddFirst((TObject *) fCheckBtns);
		fCheckBtns->ChangeBackground(LabelGC->BG());
		this->AddFrame(fCheckBtns, checkLayout);
		entryWidth -= fCheckBtns->GetWidth();
	}

	if (Action && Action->GetAssignedObject()) {
		TGLayoutHints * actLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) actLayout);
		fAction = new TGTextButton(this, Action->GetName(), Action->GetIndex(), ActionGC->GC(), ActionGC->Font());
		fHeap.AddFirst((TObject *) fAction);
		fAction->ChangeBackground(ActionGC->BG());
		this->AddFrame(fAction, actLayout);
		TObject * assObj = Action->GetAssignedObject();
		TString slot = Action->GetTitle();
		if (!slot.EndsWith("(Int_t)")) {
			if (fLabel) gMrbLog->Err() << "[" << fLabel->GetTitle() << "] ";
			gMrbLog->Err() << "Wrong slot syntax - \"" << slot << "\" (arg \"(Int_t)\" missing)" << endl;
			gMrbLog->Flush(this->ClassName());
		} else {
			slot.ReplaceAll("(Int_t)", Form("(Int_t=%d)", Action->GetIndex()));
			fAction->Connect("Clicked()", assObj->ClassName(), assObj, slot.Data());
		}
		entryWidth -= fAction->GetWidth();
	}

	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
	fHeap.AddFirst((TObject *) btnLayout);

	fNofEntries = NofEntries;
	for (Int_t entryNo = NofEntries - 1; entryNo >= 0; entryNo--) {
		if (UpDownBtnGC) {
			if (BeginEndBtns) {
				fEnd[entryNo] = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbEntryButtonEnd, UpDownBtnGC->GC());
				fHeap.AddFirst((TObject *) fEnd[entryNo]);
				fEnd[entryNo]->ChangeBackground(UpDownBtnGC->BG());
				fEnd[entryNo]->SetToolTipText("->End", 500);
				this->AddFrame(fEnd[entryNo], btnLayout);
				fEnd[entryNo]->Connect("Clicked()", this->ClassName(), this, Form("EndButtonPressed(Int_t=%d)", entryNo));
				entryWidth -= fEnd[entryNo]->GetWidth();
			}
			fUp[entryNo] = new TGPictureButton(this, fClient->GetPicture("arrow_right.xpm"), kGMrbEntryButtonUp, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fUp[entryNo]);
			fUp[entryNo]->ChangeBackground(UpDownBtnGC->BG());
			fUp[entryNo]->SetToolTipText("Increment", 500);
			this->AddFrame(fUp[entryNo], btnLayout);
			fUp[entryNo]->Connect("Clicked()", this->ClassName(), this, Form("UpButtonPressed(Int_t=%d)", entryNo));
			entryWidth -= fUp[entryNo]->GetWidth();
			fDown[entryNo] = new TGPictureButton(this, fClient->GetPicture("arrow_left.xpm"), kGMrbEntryButtonDown, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fDown[entryNo]);
			fDown[entryNo]->ChangeBackground(UpDownBtnGC->BG());
			fDown[entryNo]->SetToolTipText("Decrement", 500);
			this->AddFrame(fDown[entryNo], btnLayout);
			fDown[entryNo]->Connect("Clicked()", this->ClassName(), this, Form("DownButtonPressed(Int_t=%d)", entryNo));
			entryWidth -= fDown[entryNo]->GetWidth();
			if (BeginEndBtns) {
				fBegin[entryNo] = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbEntryButtonBegin, UpDownBtnGC->GC());
				fHeap.AddFirst((TObject *) fBegin[entryNo]);
				fBegin[entryNo]->ChangeBackground(UpDownBtnGC->BG());
				fBegin[entryNo]->SetToolTipText("->Start", 500);
				this->AddFrame(fBegin[entryNo], btnLayout);
				fBegin[entryNo]->Connect("Clicked()", this->ClassName(), this, Form("BeginButtonPressed(Int_t=%d)", entryNo));
				entryWidth -= fBegin[entryNo]->GetWidth();
			}
			fType = TGMrbLabelEntry::kGMrbEntryTypeCharInt;
		}

		TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) entryLayout);
		fEntry[entryNo] = new TGMrbTextEntry(this, new TGTextBuffer(BufferSize), fFrameId,
											EntryGC->GC(), EntryGC->Font(), EntryOptions, EntryGC->BG());
		fHeap.AddFirst((TObject *) fEntry[entryNo]);
		this->AddFrame(fEntry[entryNo], entryLayout);
		fEntry[entryNo]->Resize(entryWidth, Height);
		fEntry[entryNo]->Connect("ReturnPressed()", this->ClassName(), this, Form("EntryChanged(Int_t=%d)", entryNo));
		fEntry[entryNo]->Connect("TabPressed()", this->ClassName(), this, Form("EntryChanged(Int_t=%d)", entryNo));
	}
}

void TGMrbLabelEntry::BeginButtonPressed(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::BeginButtonPressed
// Purpose:        Slot method
// Arguments:      Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Called upon ButtonPressed() events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s;
	if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		switch (fBase) {
			case 10:
				s = Form("%*d", fWidth, (Int_t) fLowerLimit[EntryNo]);
				break;
			case 16:
				s = Form("%*x", fWidth, (Int_t) fLowerLimit[EntryNo]);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
		s = this->GetText(EntryNo);
		for (Int_t i = s.Length() - 1; i >= 0; i--) {
			TString c = s(i,1);
			if (!c.IsDigit()) {
				s = s(0, s.Length() - i - 1);
				break;
			}
		}
		switch (fBase) {
			case 10:
				s += Form("%*d", fWidth, (Int_t) fLowerLimit[EntryNo]);
				break;
			case 16:
				s += Form("%*x", fWidth, (Int_t) fLowerLimit[EntryNo]);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		s = Form("%*.*f", fWidth, fPrecision, fLowerLimit[EntryNo]);
		this->SetText(s.Data(), EntryNo);
	}
	this->EntryChanged(EntryNo);
}

void TGMrbLabelEntry::EndButtonPressed(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::EndButtonPressed
// Purpose:        Slot method
// Arguments:      Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Called upon ButtonPressed() events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s;
	if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		switch (fBase) {
			case 10:
				s = Form("%*d", fWidth, (Int_t) fUpperLimit[EntryNo]);
				break;
			case 16:
				s = Form("%*x", fWidth, (Int_t) fUpperLimit[EntryNo]);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
		s = this->GetText(EntryNo);
		for (Int_t i = s.Length() - 1; i >= 0; i--) {
			TString c = s(i,1);
			if (!c.IsDigit()) {
				s = s(0, s.Length() - i - 1);
				break;
			}
		}
		switch (fBase) {
			case 10:
				s += Form("%*d", fWidth, (Int_t) fUpperLimit[EntryNo]);
				break;
			case 16:
				s += Form("%*x", fWidth, (Int_t) fUpperLimit[EntryNo]);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		s = Form("%*.*f", fWidth, fPrecision, fUpperLimit[EntryNo]);
		this->SetText(s.Data(), EntryNo);
	}
	this->EntryChanged(EntryNo);
}

void TGMrbLabelEntry::UpButtonPressed(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::UpButtonPressed
// Purpose:        Slot method
// Arguments:      Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Called upon ButtonPressed() events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s = this->GetText(EntryNo);
	if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		Int_t intVal = s.Atoi();
		intVal += (Int_t) fIncrement[EntryNo];
		if (!this->CheckRange((Double_t) intVal)) return;
		switch (fBase) {
			case 10:
				s = Form("%*d", fWidth, intVal);
				break;
			case 16:
				s = Form("%*x", fWidth, intVal);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
		TString ns;
		for (Int_t i = s.Length() - 1; i >= 0; i--) {
			TString c = s(i,1);
			if (!c.IsDigit()) {
				ns = s(i + 1, s.Length() - i - 1);
				s = s(0, s.Length() - i - 1);
				break;
			}
		}
		Int_t intVal = ns.Atoi();
		intVal += (Int_t) fIncrement[EntryNo];
		if (!this->CheckRange((Double_t) intVal)) return;
		switch (fBase) {
			case 10:
				s += Form("%*d", fWidth, intVal);
				break;
			case 16:
				s += Form("%*x", fWidth, intVal);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		Double_t dblVal = s.Atof();
		dblVal += fIncrement[EntryNo];
		if (!this->CheckRange(dblVal)) return;
		s = Form("%*.*f", fWidth, fPrecision, dblVal);
		this->SetText(s.Data(), EntryNo);
	}
	this->EntryChanged(EntryNo);
}

void TGMrbLabelEntry::DownButtonPressed(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::DownButtonPressed
// Purpose:        Slot method
// Arguments:      Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Called upon ButtonPressed() events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s = this->GetText(EntryNo);
	if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		Int_t intVal = s.Atoi();
		intVal -= (Int_t) fIncrement[EntryNo];
		if (!this->CheckRange((Double_t) intVal)) return;
		switch (fBase) {
			case 10:
				s = Form("%*d", fWidth, intVal);
				break;
			case 16:
				s = Form("%*x", fWidth, intVal);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
		TString ns;
		for (Int_t i = s.Length() - 1; i >= 0; i--) {
			TString c = s(i,1);
			if (!c.IsDigit()) {
				ns = s(i + 1, s.Length() - i - 1);
				s = s(0, s.Length() - i - 1);
				break;
			}
		}
		Int_t intVal = ns.Atoi();
		intVal -= (Int_t) fIncrement[EntryNo];
		if (!this->CheckRange((Double_t) intVal)) return;
		switch (fBase) {
			case 10:
				s += Form("%*d", fWidth, intVal);
				break;
			case 16:
				s += Form("%*x", fWidth, intVal);
				break;
		}
		this->SetText(s.Data(), EntryNo);
	} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		Double_t dblVal = s.Atof();
		dblVal -= fIncrement[EntryNo];
		if (!this->CheckRange(dblVal)) return;
		s = Form("%*.*f", fWidth, fPrecision, dblVal);
		this->SetText(s.Data(), EntryNo);
	}
	this->EntryChanged(EntryNo);
}

void TGMrbLabelEntry::SetType(EGMrbEntryType EntryType, Int_t Width, Int_t BaseOrPrec, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetType
// Purpose:        Define entry type
// Arguments:      EGMrbEntryType EntryType   -- type
//                 Int_t Width                -- number width
//                 Int_t BaseOrPrec           -- numerical base (int) or precision (double)
//                 Bool_t PadZero             -- pad with 0 if kTRUE
// Results:        --
// Exceptions:
// Description:    Defines entry type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fType = EntryType;
	fWidth = Width;
	fPadZero = PadZero;
	if (EntryType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		if (BaseOrPrec == -1) {
			fBase = 10;
		} else {
			fBase = BaseOrPrec;
		}
		this->SetTextAlignment(kTextRight);
	} else if (EntryType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		if (BaseOrPrec == -1) {
			fPrecision = 2;
		} else {
			fPrecision = BaseOrPrec;
		}
		this->SetTextAlignment(kTextRight);
	}
}

void TGMrbLabelEntry::SetText(const Char_t * Text, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Char_t * Text    -- text to be written
//                 Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Writes entry text and updates tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fEntry[EntryNo]->SetText(Text);
	this->CreateToolTip(EntryNo);
}

void TGMrbLabelEntry::SetText(Int_t Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Int_t Value    -- integer to be converted to text
//                 Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Writes entry text and updates tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s;
	switch (fBase) {
		case 10:
			s = Form("%*d", fWidth, Value);
			break;
		case 16:
			s = Form("%#*x", fWidth, Value);
			break;
	}
	fEntry[EntryNo]->SetText(s.Data());
	this->CreateToolTip(EntryNo);
}

void TGMrbLabelEntry::SetText(Double_t Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Double_t Value    -- double to be converted to text
//                 Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Writes entry text and updates tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s;
	s = Form("%*.*f", fWidth, fPrecision, Value);
	fEntry[EntryNo]->SetText(s.Data());
	this->CreateToolTip(EntryNo);
}

void TGMrbLabelEntry::SetTextAlignment(ETextJustification Align, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetTextAlignment
// Purpose:        Set text alignment
// Arguments:      ETextJustification Align    -- alignment (left, right)
//                 Int_t EntryNo               -- entry number
// Results:        --
// Exceptions:
// Description:    Writes entry text and updates tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (EntryNo == -1) {
		for (Int_t i = 0; i < fNofEntries; i++) this->SetTextAlignment(Align, i);
	} else {
		fEntry[EntryNo]->SetAlignment(Align);
	}
}

void TGMrbLabelEntry::SetState(Bool_t State, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetTextAlignment
// Purpose:        Set text alignment
// Arguments:      Bool_t State                -- kTRUE/kFALSE
//                 Int_t EntryNo               -- entry number
// Results:        --
// Exceptions:
// Description:    Enables/disables an entry and its buttons
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (EntryNo == -1) {
		for (Int_t i = 0; i < fNofEntries; i++) this->SetState(State, i);
	} else {
		fEntry[EntryNo]->SetState(State);
		if (fUp[EntryNo]) fUp[EntryNo]->SetEnabled(State);
		if (fDown[EntryNo]) fDown[EntryNo]->SetEnabled(State);
		if (fBegin[EntryNo]) fBegin[EntryNo]->SetEnabled(State);
		if (fEnd[EntryNo]) fEnd[EntryNo]->SetEnabled(State);
	}
}

TGMrbTextEntry * TGMrbLabelEntry::GetTextEntry(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetTextEntry
// Purpose:        Return address of specified text entry
// Arguments:      Int_t EntryNo            -- entry number
// Results:        TGMrbTextEntry * Addr    -- entry address
// Exceptions:
// Description:    Reads entry contents and updates tooltip
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(fEntry[EntryNo]);
}

const Char_t * TGMrbLabelEntry::GetText(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText
// Purpose:        Read text entry and update tooltip
// Arguments:      Int_t EntryNo    -- entry number
// Results:        Char_t * Text    -- text
// Exceptions:
// Description:    Reads entry contents and updates tooltip
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->CreateToolTip(EntryNo);
	return(fEntry[EntryNo]->GetText());
}

Int_t TGMrbLabelEntry::GetText2Int(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText2Int
// Purpose:        Read text entry, convert to integer, update tooltip
// Arguments:      Int_t EntryNo    -- entry number
// Results:        Int_t Value   -- integer value
// Exceptions:
// Description:    Reads entry contents and converts to integer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal = 0;
	if (fType == kGMrbEntryTypeInt) {
		TMrbString intStr = fEntry[EntryNo]->GetText();
		Int_t idx = intStr.Index("0x", 0);
		if (idx >= 0) {
			intStr = intStr(idx + 2, intStr.Length());
			intStr.ToInteger(intVal, 16);
		} else {
			intVal = intStr.Atoi();
		}
		this->CreateToolTip(EntryNo);
	} else {
		if (fLabel) gMrbLog->Err() << fLabel->GetTitle() << ": ";
		gMrbLog->Err() << "Not a INTEGER entry" << endl;
		gMrbLog->Flush(this->ClassName(), "GetText2Int");
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(intVal);
}

Double_t TGMrbLabelEntry::GetText2Double(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText2Double
// Purpose:        Read text entry, convert to double, update tooltip
// Arguments:      Int_t EntryNo    -- entry number
// Results:        Double_t Value   -- double value
// Exceptions:
// Description:    Reads entry contents and converts to double
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t dblVal = 0.0;
	if (fType == kGMrbEntryTypeDouble) {
		TString dblStr = fEntry[EntryNo]->GetText();
		dblVal = dblStr.Atof();
		this->CreateToolTip(EntryNo);
	} else {
		if (fLabel) gMrbLog->Err() << fLabel->GetTitle() << ": ";
		gMrbLog->Err() << "Not a DOUBLE entry" << endl;
		gMrbLog->Flush(this->ClassName(), "GetText2Double");
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(dblVal);
}

void TGMrbLabelEntry::ShowToolTip(Bool_t ShowFlag, Bool_t ShowRange) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::ShowToolTip
// Purpose:        Turn tooltips on/off
// Arguments:      Bool_t ShowFlag    -- kTRUE if tooltips have to be shown
//                 Bool_t ShowRange   -- kTRUE if range has to be shown
// Results:        --
// Exceptions:
// Description:    Shows number as binary, octal, dec, and hex in tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fShowToolTip = ShowFlag;
	fShowRange = ShowRange;
	for (Int_t i = 0; i < fNofEntries; i++) this->CreateToolTip(i);
}

void TGMrbLabelEntry::CreateToolTip(Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::CreateToolTip
// Purpose:        Create tooltip showing integer value in different formats
// Arguments:      Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:
// Description:    Shows number as binary, octal, dec, and hex in tooltip.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fShowToolTip) {
		Bool_t showRange = fShowRange;
		if (fIncrement[EntryNo] == 0) showRange = kFALSE;

		if (fType == kGMrbEntryTypeInt) {
			TMrbString v = fEntry[EntryNo]->GetText();
			Int_t value;
			v.ToInteger(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendInteger((Int_t) fLowerLimit[EntryNo], 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fIncrement[EntryNo], 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fUpperLimit[EntryNo], 0, 10);
				ttStr += "] ";
			}
			TMrbString m;
			if (value < 0) {
				value = -value;
				m = "-";
			} else {
				m = "";
			}
			ttStr += m;
			ttStr.AppendInteger(value, 0, 2);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 8);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 10);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 16);
			fEntry[EntryNo]->SetToolTipText(ttStr.Data());
		} else if (fType == kGMrbEntryTypeDouble) {
			TMrbString v = fEntry[EntryNo]->GetText();
			Double_t value;
			v.ToDouble(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendDouble(fLowerLimit[EntryNo]);
				ttStr += "...";
				ttStr.AppendDouble(fIncrement[EntryNo]);
				ttStr += "...";
				ttStr.AppendDouble(fUpperLimit[EntryNo]);
				ttStr += "] ";
			}
			ttStr.AppendDouble(value);
			fEntry[EntryNo]->SetToolTipText(ttStr.Data());
		}
	}
}

void TGMrbLabelEntry::SetIncrement(Double_t Increment, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetIncrement
// Purpose:        Define an increment
// Arguments:      Double_t Increment    -- increment
//                 Int_t EntryNo         -- entry number (-1 -> all entries)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines an increment value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (EntryNo == -1) {
		for (Int_t i = 0; i < fNofEntries; i++) this->SetIncrement(Increment, i);
	}
	fIncrement[EntryNo] = Increment;
}

Bool_t TGMrbLabelEntry::SetRange(Double_t LowerLimit, Double_t UpperLimit, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetRange
// Purpose:        Define a range for numerical input
// Arguments:      Double_t LowerLimit   -- lower limit
//                 Double_t UpperLimit   -- upper limit
//                 Int_t EntryNo         -- entry number (-1 -> all entries)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a numerical range.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (EntryNo == -1) {
		for (Int_t i = 0; i < fNofEntries; i++) {
			if (!this->SetRange(LowerLimit, UpperLimit, i)) return(kFALSE);
		}
		return(kTRUE);
	}

	if (LowerLimit == 0 && UpperLimit == 0) {
		fLowerLimit[EntryNo] = 0;
		fUpperLimit[EntryNo] = 0;
		return(kTRUE);
	}
	if (LowerLimit > UpperLimit) {
		gMrbLog->Err() << "Illegal range - [" << LowerLimit << "," << UpperLimit << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetRange");
		return(kFALSE);
	}
	fLowerLimit[EntryNo] = LowerLimit;
	fUpperLimit[EntryNo] = UpperLimit;
	return(kTRUE);
}

Bool_t TGMrbLabelEntry::WithinRange(Int_t EntryNo) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::WithinRange
// Purpose:        Check range
// Arguments:      Int_t EntryNo    -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if entry text is a numeric value within range.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;
	Double_t dblVal = 0.0;
	TMrbString numStr;
	TString prefix;

	if (!this->RangeToBeChecked()) return(kTRUE);

	switch (fType) {
		case TGMrbLabelEntry::kGMrbEntryTypeInt:		numStr = fEntry[EntryNo]->GetText();
														if (!numStr.ToInteger(intVal)) return(kFALSE);
														return(intVal >= fLowerLimit[EntryNo] && intVal <= fUpperLimit[EntryNo]);
		case TGMrbLabelEntry::kGMrbEntryTypeDouble:		numStr = fEntry[EntryNo]->GetText();
														if (!numStr.ToDouble(dblVal)) return(kFALSE);
														return(dblVal >= fLowerLimit[EntryNo] && dblVal <= fUpperLimit[EntryNo]);
		case TGMrbLabelEntry::kGMrbEntryTypeCharInt:	numStr = fEntry[EntryNo]->GetText();
														numStr.SplitOffInteger(prefix, intVal, fBase);
														return(intVal >= fLowerLimit[EntryNo] && intVal <= fUpperLimit[EntryNo]);
		case TGMrbLabelEntry::kGMrbEntryTypeCharDouble:	numStr = fEntry[EntryNo]->GetText();
														numStr.SplitOffDouble(prefix, dblVal);
														return(dblVal >= fLowerLimit[EntryNo] && dblVal <= fUpperLimit[EntryNo]);
	}
	return(kTRUE);
}

Bool_t TGMrbLabelEntry::CheckRange(Double_t Value, Int_t EntryNo, Bool_t Verbose, Bool_t Popup) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::CheckRange
// Purpose:        Check range
// Arguments:      Double_t Value    -- value to be tested
//                 Int_t EntryNo     -- entry number
//                 Bool_t Verbose    -- ouput message to stderr
//                 Bool_t Popup      -- pop up TGMsgBox
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if value is within range.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->RangeToBeChecked()) return(kTRUE);
	Bool_t withinRange = (Value >= fLowerLimit[EntryNo] && Value <= fUpperLimit[EntryNo]);
	if (withinRange) return(kTRUE);
	if (Verbose || Popup) {
		if (fLabel) gMrbLog->Err() << "[" << fLabel->GetTitle() << "] ";
		gMrbLog->Err() << "Value out of range - " << Value << " (should be in [" << fLowerLimit[EntryNo] << "," << fUpperLimit[EntryNo] << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckRange");
		if (Popup) new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(kFALSE);
}

Bool_t TGMrbLabelEntry::RangeToBeChecked(Int_t /*EntryNo*/) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::RangeToBeChecked
// Purpose:        Test if range to be checked
// Arguments:      Int_t EntryNo    -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if range has to be checked.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fType == TGMrbLabelEntry::kGMrbEntryTypeChar) return(kFALSE);
	return(kTRUE);
}

void TGMrbLabelEntry::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:
// Description:    Enables/disables up/down buttons.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = 0; i < fNofEntries; i++) {
		if (fUp[i] && fDown[i]) {
			EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
			fUp[i]->SetState(btnState);
			fDown[i]->SetState(btnState);
		}
	}
}

void TGMrbLabelEntry::ActionButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::ActionButtonEnable
// Purpose:        Enable/disable action button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:
// Description:    Enables/disables action button.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fAction) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fAction->SetState(btnState);
	}
}

void TGMrbLabelEntry::EntryChanged(Int_t /*FrameId*/, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::EntryChanged
// Purpose:        Signal handler
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t EntryNo     -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Emits signal on "text entry changed"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

   Long_t args[2];

   args[0] = fFrameId;
   args[1] = EntryNo;

   this->Emit("EntryChanged(Int_t, Int_t)", args);
}

Bool_t TGMrbTextEntry::HandleButton(Event_t * Event) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTextEntry::HandleButton
// Purpose:        Handle button events
// Arguments:      Event_t * Event   -- button event
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds private actions to button handling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsEnabled()) return(kTRUE);
	if (Event->fType == kButtonPress && Event->fCode == kButton3) {
		cerr	<< setred
				<< this->ClassName() << "::HandleButton(): Mouse button 3 not yet implemented"
				<< setblack << endl;
	} else {
		return(TGTextEntry::HandleButton(Event));
	}
	return(kTRUE);
}


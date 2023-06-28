#ifndef __TGMrbLabelEntry_h__
#define __TGMrbLabelEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLabelEntry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLabelEntry              -- a frame containing a label and
//                                                 an entry
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLabelEntry.h,v 1.18 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbCheckButton.h"
#include "TGMrbFocusList.h"
#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTextEntry
// Purpose:        
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbTextEntry: public TGTextEntry {

	public:
		TGMrbTextEntry(const TGWindow * Parent, TGTextBuffer * Text, Int_t Id,
								GContext_t /*Context*/, FontStruct_t Font, UInt_t /*Option*/, ULong_t Back) :
														TGTextEntry(Parent, Text, Id) {
			this->SetFont(Font);
			this->SetBackgroundColor(Back);
		};

		~TGMrbTextEntry() {};
		
		virtual Bool_t HandleButton(Event_t * Event);

	ClassDef(TGMrbTextEntry, 1) 	// [GraphUtils] A text entry
};
			
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry
// Purpose:        Define a labelled text entry
// Description:    Defines a label together with a text entry field.
//                 an Up/Down button may be appended to increment entry data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLabelEntry: public TGCompositeFrame, public TGMrbObject {

	public:
		enum EGMrbEntryButton	{	kGMrbEntryButtonUp,
									kGMrbEntryButtonDown,
									kGMrbEntryButtonBegin,
									kGMrbEntryButtonEnd,
									kGMrbEntryButtonCheck,
									kGMrbEntryButtonRadio
								};

		enum EGMrbEntryType 	{	kGMrbEntryTypeChar,
									kGMrbEntryTypeInt,
									kGMrbEntryTypeDouble,
									kGMrbEntryTypeCharInt,
									kGMrbEntryTypeCharDouble
								};

		enum					{	kGMrbEntryNofEntries = 10	};

	public:
		TGMrbLabelEntry(const TGWindow * Parent, const Char_t * Label,				// ctor
									Int_t BufferSize, Int_t FrameId,
									Int_t Width, Int_t Height, Int_t EntryWidth,
									TGMrbLayout * FrameGC,
									TGMrbLayout * LabelGC = NULL,
									TGMrbLayout * EntryGC = NULL,
									TGMrbLayout * UpDownBtnGC = NULL,
									Bool_t BeginEndBtns = kFALSE,
									TMrbNamedX * Action = NULL,
									TGMrbLayout * ActionGC = NULL,
									TMrbLofNamedX * CheckBtns = NULL,
									TMrbLofNamedX * RadioBtns = NULL,
									UInt_t FrameOptions = kHorizontalFrame,
									UInt_t EntryOptions = kSunkenFrame | kDoubleBorder,
									Int_t NofEntries = 1);

		~TGMrbLabelEntry() {};				// default dtor

		void SetType(EGMrbEntryType EntryType, Int_t Width = 0, Int_t BaseOrPrec = -1, Bool_t PadZero = kFALSE);
		void SetIncrement(Double_t Increment, Int_t EntryNo = -1);
		Bool_t SetRange(Double_t LowerLimit, Double_t UpperLimit, Int_t EntryNo = -1);
		Bool_t WithinRange(Int_t EntryNo = 0) const;
		Bool_t CheckRange(Double_t Value, Int_t EntryNo = 0, Bool_t Verbose = kFALSE, Bool_t Popup = kFALSE) const;
		Bool_t RangeToBeChecked(Int_t EntryNo = 0) const;

		inline void AddToFocusList(TGMrbFocusList * FocusList, Int_t EntryNo = 0) { fFocusList = FocusList; fFocusList->Add(fEntry[EntryNo]); };
		inline TGMrbFocusList * GetFocusList() const { return(fFocusList); };

		inline TGMrbTextEntry * GetEntry(Int_t EntryNo = 0) const { return(fEntry[EntryNo]); };
		inline TGPictureButton * GetUpButton(Int_t EntryNo = 0) const { return(fUp[EntryNo]); };
		inline TGPictureButton * GetDownButton(Int_t EntryNo = 0) const { return(fDown[EntryNo]); };
		inline TGPictureButton * GetBeginButton(Int_t EntryNo = 0) const { return(fBegin[EntryNo]); };
		inline TGPictureButton * GetEndButton(Int_t EntryNo = 0) const { return(fEnd[EntryNo]); };
		inline TGTextButton * GetActionButton() const { return(fAction); };
		inline TGMrbCheckButtonList * GetLofCheckButtons() const { return(fCheckBtns); };
		inline TGMrbRadioButtonList * GetLofRadioButtons() const { return(fRadioBtns); };

		inline TGMrbCheckButtonList * CheckBtns() { return(fCheckBtns); };
		inline TGMrbRadioButtonList * RadioBtns() { return(fRadioBtns); };
		inline TGTextButton * ActionBtn() { return(fAction); };

		inline void SetCheckButtonBits(UInt_t Bits) { if (fCheckBtns) fCheckBtns->SetState(Bits, kButtonDown); };
		inline void SetRadioButtonBits(UInt_t Bits) { if (fRadioBtns) fRadioBtns->SetState(Bits, kButtonDown); };
		inline UInt_t GetCheckButtonBits() { return(fCheckBtns ? fCheckBtns->GetActive() : 0); };
		inline UInt_t GetRadioButtonBits() { return(fRadioBtns ? fRadioBtns->GetActive() : 0); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down buttons
		void ActionButtonEnable(Bool_t Flag = kTRUE);			// enable/disable action button

		TGMrbTextEntry * GetTextEntry(Int_t EntryNo = 0);		// get text entry
		const Char_t * GetText(Int_t EntryNo = 0);				// get text, update tooltip if necessary
		Int_t GetText2Int(Int_t EntryNo = 0);					// get text, convert to integer
		Double_t GetText2Double(Int_t EntryNo = 0);				// ... convert to integer
		void SetText(const Char_t * Text, Int_t EntryNo = 0);	// set text, update tooltip if necessary
		void SetText(Int_t Value, Int_t EntryNo = 0);			// ... convert from integer
		void SetText(Double_t Value, Int_t EntryNo = 0);		// ... from double

		void SetTextAlignment(ETextJustification Align, Int_t EntryNo = -1);		// set text alignment

		void SetState(Bool_t Flag, Int_t EntryNo = -1); 		// enable/disable

		inline TGLabel * GetLabel() { return(fLabel); };

		void ShowToolTip(Bool_t Flag = kTRUE, Bool_t ShowRange = kFALSE);

		void BeginButtonPressed(Int_t EntryNo);		// slot methods called upon ButtonPressed() signals
		void EndButtonPressed(Int_t EntryNo);
		void UpButtonPressed(Int_t EntryNo);
		void DownButtonPressed(Int_t EntryNo);
		inline void EntryChanged(Int_t EntryNo) { this->EntryChanged(fFrameId, EntryNo); };

		void EntryChanged(Int_t FrameId, Int_t EntryNo);			// *SIGNAL*

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void CreateToolTip(Int_t EntryNo = 0);

	protected:
		Int_t fFrameId;										// id
		Int_t fNofEntries;									// number of entries
		TGLabel * fLabel;									//! label
		TGMrbTextEntry * fEntry[kGMrbEntryNofEntries];		//! entry widget(s)
		TGPictureButton * fUp[kGMrbEntryNofEntries]; 		//! button ">", increment
		TGPictureButton * fDown[kGMrbEntryNofEntries];		//! button "<", decrement
		TGPictureButton * fBegin[kGMrbEntryNofEntries]; 	//! button "<<", begin
		TGPictureButton * fEnd[kGMrbEntryNofEntries]; 		//! button ">>", end
		TGTextButton * fAction; 							//! textbutton
		TGMrbCheckButtonList * fCheckBtns;					//!checkbuttons
		TGMrbRadioButtonList * fRadioBtns;					//!radiobuttons
		EGMrbEntryType fType;								// entry type: char, char + int, ...
		Int_t fWidth;										// number of digits to be displayed
		Int_t fBase;										// numerical base if of type int or charint (2, 8, 10, 16)
		Int_t fPrecision;									// precision if type double
		Bool_t fPadZero; 									// should we pad with 0?
		Double_t fLowerLimit[kGMrbEntryNofEntries];			// lower limit
		Double_t fUpperLimit[kGMrbEntryNofEntries];			// upper limit
		Double_t fIncrement[kGMrbEntryNofEntries];			// increment

		Bool_t fShowToolTip;			// kTRUE if tooltip has to be shown
		Bool_t fShowRange;				// kTRUE if range has to be shown in tooltip

	TGMrbFocusList * fFocusList;	//! list of focusable entries

	ClassDef(TGMrbLabelEntry, 1)		// [GraphUtils] An entry field with a label
};

#endif

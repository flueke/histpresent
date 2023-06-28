#ifndef __TGMrbLabelCombo_h__
#define __TGMrbLabelCombo_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLabelCombo.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLabelCombo     -- a frame containing a label and
//                                        a combo box
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLabelCombo.h,v 1.11 2011-09-28 12:22:02 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGComboBox.h"

#include "TMrbLofNamedX.h"
#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo
// Purpose:        Define a labelled combo box
// Description:    Defines a label together with a combo box
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLabelCombo: public TGCompositeFrame, public TGMrbObject {

	public:
		enum EGMrbEntryButton	{	kGMrbComboButtonUp,
									kGMrbComboButtonDown,
									kGMrbComboButtonBegin,
									kGMrbComboButtonEnd
								};

	public:
		TGMrbLabelCombo(const TGWindow * Parent, const Char_t * Label,					// ctor
							TMrbLofNamedX * Entries,
							Int_t FrameId, Int_t Selected,
							Int_t Width, Int_t Height, Int_t ComboWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * ComboGC = NULL,
							TGMrbLayout * UpDownBtnGC = NULL,
							Bool_t BeginEndBtns = kFALSE,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t ComboOptions = kHorizontalFrame|kSunkenFrame|kDoubleBorder);

		~TGMrbLabelCombo() {};														// default dtor

		Bool_t AddEntries(TMrbLofNamedX * Entries); 								// add entries to the box
		
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		inline TGPictureButton * GetUpButton() const { return(fUp); };
		inline TGPictureButton * GetDownButton() const { return(fDown); };
		inline TGPictureButton * GetBeginButton() const { return(fBegin); };
		inline TGPictureButton * GetEndButton() const { return(fEnd); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down buttons

		const Char_t * GetText() const;
		void SetText(const Char_t * Text);

		inline void Select(Int_t ItemIdx) { fCombo->Select(ItemIdx, kFALSE); };
		inline Int_t GetSelected() { return(fCombo->GetSelected()); };
		inline TMrbNamedX * GetSelectedNx() { return(fEntries.FindByIndex(fCombo->GetSelected())); };

		inline TMrbNamedX * GetEntry(Int_t Index) { return((TMrbNamedX *) fEntries.FindByIndex(Index)); };

		void BeginButtonPressed();		// slot methods called upon ButtonPressed() signals
		void EndButtonPressed();
		void UpButtonPressed();
		void DownButtonPressed();

		inline void SelectionChanged(Int_t Selection) { this->SelectionChanged(fFrameId, Selection); };
		void SelectionChanged(Int_t FrameId, Int_t Selection); 			// *SIGNAL*

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fFrameId;
		TGComboBox * fCombo;			//!

		TMrbLofNamedX fEntries;			//! entries
		TGPictureButton * fUp; 			//! button ">", increment
		TGPictureButton * fDown;		//! button "<", decrement
		TGPictureButton * fBegin; 		//! button "<<", begin
		TGPictureButton * fEnd; 		//! button ">>", end

	ClassDef(TGMrbLabelCombo, 1)		// [GraphUtils] A labelled combo box
};

#endif

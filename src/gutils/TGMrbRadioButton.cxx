//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbRadioButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of radio buttons
// Description:    Implements class methods to handle a frame of
//                 radio buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbRadioButton.cxx,v 1.3 2005-04-28 10:25:49 rudi Exp $       
// Date:           
// Layout: A list of radio buttons
//Begin_Html
/*
<img src=gutils/TGMrbRadioButton1.gif>
<img src=gutils/TGMrbRadioButton2.gif>
<img src=gutils/TGMrbRadioButton3.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGMrbRadioButton.h"

ClassImp(TGMrbRadioButtonList)
ClassImp(TGMrbRadioButtonGroup)

TGMrbRadioButtonList::TGMrbRadioButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
												Int_t Width, Int_t Height,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * ButtonGC,
												UInt_t FrameOptions,
												UInt_t ButtonOptions) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()),
										TGMrbButtonFrame(Parent, Label, kGMrbRadioButton | kGMrbButtonList,
												Buttons, BtnId, NofCL, Width, Height,
												FrameGC, LabelGC, ButtonGC, FrameOptions, ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonList
// Purpose:        Define a list of radio buttons arranged in a composite frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Label                  -- label text (NULL means "no label")
//                 TMrbLofNamedX * Buttons         -- button names and indices
//                 Int_t BtnId                    -- (global) button id
//                 Int_t NofCL                     -- button grid CxL: number of columns/lines
//                 Int_t Width                     -- frame width
//                 Int_t Height                    -- frame height
//                 TGMrbLayout * FrameGC           -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC           -- ... (label)
//                 TGMrbLayout * ButtonGC          -- ... (button)
//                 UInt_t FrameOptions             -- frame options
//                 UInt_t ButtonOptions            -- options to configure buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;

	fFrame = this;

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), fLabelGC->GC(), fLabelGC->Font(), kChildFrame, fLabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, fLabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	this->PlaceButtons();
}

TGMrbRadioButtonGroup::TGMrbRadioButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
												TGMrbLayout * FrameGC,
												TGMrbLayout * ButtonGC,
												UInt_t FrameOptions,
												UInt_t ButtonOptions) :
								TGGroupFrame(Parent, Label, FrameOptions, FrameGC->GC(), FrameGC->Font(), FrameGC->BG()),
								TGMrbButtonFrame(Parent, Label, kGMrbRadioButton | kGMrbButtonGroup,
												Buttons, BtnId, NofCL, 0, 0,
												FrameGC, NULL, ButtonGC, FrameOptions, ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 Int_t BtnId                    -- (global) button id
//                 Int_t NofCL                    -- button grid CxL: number of columns/lines
//                 TGMrbLayout * FrameGC          -- graphic context & layout (frame)
//                 TGMrbLayout * ButtonGC         -- ... (button)
//                 UInt_t FrameOptions            -- frame options
//                 UInt_t ButtonOptions           -- options to configure buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;

	this->PlaceButtons();
}

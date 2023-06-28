#ifndef __TGMrbRadioButton_h__
#define __TGMrbRadioButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbRadioButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbRadioButtonList    -- a composite frame contaoning
//                                            radio buttons
//                 TGMrbRadioButtonGroup   -- a group frame containing
//                                            radio buttons
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbRadioButton.h,v 1.6 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"

#include "TMrbLofNamedX.h"
#include "TGMrbButtonFrame.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonList
// Purpose:        Define a list of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbRadioButtonList: public TGCompositeFrame, public TGMrbButtonFrame {

	public:
		TGMrbRadioButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
													Int_t Width, Int_t Height,
													TGMrbLayout * FrameGC,
													TGMrbLayout * LabelGC = NULL,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbRadioButtonList() {};				// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TGMrbRadioButtonList, 1)		// [GraphUtils] A list of radio buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbRadioButtonGroup: public TGGroupFrame, public TGMrbButtonFrame {

	public:
		TGMrbRadioButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
													TGMrbLayout * FrameGC,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbRadioButtonGroup() {};				// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TGMrbRadioButtonGroup, 1)		// [GraphUtils] A group of radio buttons
};

#endif

#ifndef __TGMrbPictureButton_h__
#define __TGMrbPictureButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbPictureButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbPictureButtonList   -- a composite frame containing
//                                             check buttons
//                 TGMrbPictureButtonGroup  -- check buttons arranged in
//                                             group frame
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbPictureButton.h,v 1.6 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
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
// Name:           TGMrbPictureButtonList
// Purpose:        Define a list of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbPictureButtonList: public TGCompositeFrame, public TGMrbButtonFrame {

	public:
		TGMrbPictureButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
													Int_t Width, Int_t Height,
													TGMrbLayout * FrameGC,
													TGMrbLayout * LabelGC = NULL,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbPictureButtonList() {};				// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TGMrbPictureButtonList, 1) 	// [GraphUtils] A list of picture buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbPictureButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbPictureButtonGroup: public TGGroupFrame, public TGMrbButtonFrame {

	public:
		TGMrbPictureButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t BtnId, Int_t NofCL,
													TGMrbLayout * FrameGC,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbPictureButtonGroup() {};				// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TGMrbPictureButtonGroup, 1) 	// [GraphUtils] A group of picture buttons
};

#endif

#ifndef __TGMrbLofKeyBindings_h__
#define __TGMrbLofKeyBindings_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLofKeyBindings.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLofKeyBindings          -- key bindings
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLofKeyBindings.h,v 1.6 2008-12-29 13:48:24 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"
#include "TGFrame.h"
#include "WidgetMessageTypes.h"
#include "TMrbLofNamedX.h"

#include "RQ_OBJECT.h"

enum				{	kC_KEY					= kC_USER + 100 	};	// to be used for ProcessMessage()
		
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofKeyBindings
// Purpose:        Maintain a list of key bindings
// Description:    Stores key bindings for a window
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLofKeyBindings : public TMrbLofNamedX {

	RQ_OBJECT("TGMrbLofKeyBindings")

	public:
		// global status bits
		enum EGMrbKeyAction {	kGMrbKeyActionAllDone	= 0,			// standard system codes
								kGMrbKeyActionExit,
								kGMrbKeyActionClose,
								kGMrbKeyActionSave,
								kGMrbKeyActionRestore,
								kGMrbKeyActionPrint,
								kGMrbKeyActionOpen,
								kGMrbKeyActionNew,
								kGMrbKeyActionFind,
								kGMrbKeyActionFindAgain,
								kGMrbKeyActionUndo,
								kGMrbKeyActionPaste,
								kGMrbKeyActionCopy,
								kGMrbKeyActionCut,
								kGMrbKeyActionUser		= 128,			// user may add his codes from here on
								kGMrbKeyActionAnyKey	= 256			// codes to handle other chars
							};
							
		enum				{	kGMrbKeyModifierMask	= kKeyShiftMask | kKeyLockMask | kKeyControlMask | kKeyMod1Mask };
		enum				{	kGMrbKeyIndexMask		= 0x0000FFFF		};	// index: keycode (0:7), modifier (8:15)
		enum				{	kGMrbKeyModifierBit		= 8					};	// therefore modifier shifted to bit 8
		enum				{	kGMrbKeyActionMask		= 0xFFFF0000		};	// action codes stored in high order bits
		enum				{	kGMrbKeyActionBit		= 16				};	// therefore action shifted to bit 16
		
	public:
		TGMrbLofKeyBindings(const TGWindow * Parent = NULL); 		// ctor
		~TGMrbLofKeyBindings() {};									// default dtor

		inline void SetParent(TGWindow * Parent) { fParent = Parent; };
		
		Bool_t BindKey(const Char_t * Key, EGMrbKeyAction Action = TGMrbLofKeyBindings::kGMrbKeyActionAllDone);
		Bool_t HandleKey(Event_t * Event);
		
		void KeyPressed(Int_t FrameId, Int_t Key);		 	//*SIGNAL*

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		const TGWindow * fParent;
		
	ClassDef(TGMrbLofKeyBindings, 0) 	// [GraphUtils] A list of key bindings
};

#endif

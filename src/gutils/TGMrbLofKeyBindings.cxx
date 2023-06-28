//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLofKeyBindings.cxx
// Purpose:        MARaBOU utilities: Maintain a list of key bindings
// Description:    Implements key handling
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbLofKeyBindings.cxx,v 1.3 2008-12-29 13:48:24 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TVirtualX.h"
#include "TMrbLogger.h"
#include "TGMrbLofKeyBindings.h"

extern TMrbLogger * gMrbLog;		// access to message logging

ClassImp(TGMrbLofKeyBindings)
		
TGMrbLofKeyBindings::TGMrbLofKeyBindings(const TGWindow * Parent) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofKeyBindings
// Purpose:        Define key bindings
// Arguments:      TGWindow * Parent           -- parent window
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fParent = Parent;
	this->SetName("Key bindings");
}

Bool_t TGMrbLofKeyBindings::BindKey(const Char_t * Key, EGMrbKeyAction Action) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofKeyBindings::BindKey
// Purpose:        Define a key binding
// Arguments:      Char_t Key       -- key code in ascii
//                 Int_t Modifier   -- modifier (ctrl, alt etc)
//                 EGMrbKeyAction   -- action to be taken
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a key binding.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t keyCode, keyIndex, modifier;
	TMrbNamedX * kb;
	TString key;
	TString prefix;
		
	if (fParent == NULL) {
		gMrbLog->Err()	<< "Can't call \"TGMainFrame::BindKey()\" - no parent frame defined" << endl;
		gMrbLog->Flush(this->ClassName(), "BindKey");
		return(kFALSE);
	}
	
	key = Key;
	if (key.Index("Ctrl-", 0) == 0 || key.Index("ctrl-", 0) == 0) {
		key = key(5, 1);
		prefix = "Ctrl-";
		modifier = kKeyControlMask;
	} else if (key.Index("Alt-", 0) == 0 || key.Index("alt-", 0) == 0) {
		key = key(4, 1);
		prefix = "Alt-";
		modifier = kKeyMod1Mask;
	} else {
		prefix = "";
		modifier = 0;
	}
	
	keyCode = gVirtualX->KeysymToKeycode(key(0));
	if (keyCode == 0) {
		gMrbLog->Err()	<< "Key \"" << Key << "\" - no key code found";
		gMrbLog->Flush(this->ClassName(), "BindKey");
		return(kFALSE);
	}
	
	keyIndex = (modifier << TGMrbLofKeyBindings::kGMrbKeyModifierBit) | keyCode;
	kb = this->FindByIndex(keyIndex, TGMrbLofKeyBindings::kGMrbKeyIndexMask);
	if (kb == NULL) {
		this->AddNamedX(keyIndex | (Action << TGMrbLofKeyBindings::kGMrbKeyActionBit), Key);
		((TGMainFrame *) fParent->GetMainFrame())->BindKey(fParent, keyCode, modifier);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Key \"" << Key << "\" - binding already defined" << endl;
		gMrbLog->Flush(this->ClassName(), "BindKey");
		return(kFALSE);
	}
}

Bool_t TGMrbLofKeyBindings::HandleKey(Event_t * Event) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofKeyBindings::HandleKey
// Purpose:        Handle a key
// Arguments:      Event_t * Event  -- key event
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Handles a key. Calls ProcessMessage(kC_KEY, ...) for this window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Event->fType != kGKeyPress) return(kFALSE);
	
	if (fParent == NULL) {
		gMrbLog->Err()	<< "No parent frame defined" << endl;
		gMrbLog->Flush(this->ClassName(), "HandleKey");
		return(kFALSE);
	}
	
	Int_t keyIndex = (Event->fState << TGMrbLofKeyBindings::kGMrbKeyModifierBit) | Event->fCode;
	TMrbNamedX * kb = this->FindByIndex(keyIndex, TGMrbLofKeyBindings::kGMrbKeyIndexMask);
	if (kb) {
		this->KeyPressed(	fParent->GetId(),
							(Long_t) ((kb->GetIndex() & TGMrbLofKeyBindings::kGMrbKeyActionMask) >> TGMrbLofKeyBindings::kGMrbKeyActionBit));
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Key code \"0x" << setbase(16) << Event->fCode << setbase(10) << "\" not bound" << endl;
		gMrbLog->Flush(this->ClassName(), "HandleKey");
		return(kFALSE);
	}
}


void TGMrbLofKeyBindings::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofKeyBindings::KeyPressed
// Purpose:        Signal handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t Kay        -- key code
// Results:        --
// Exceptions:     
// Description:    Emits signal on "key pressed" 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Long_t args[2];

	args[0] = FrameId;
	args[1] = Key;
	this->Emit("KeyPressed(Int_t, Int_t)", args);
}

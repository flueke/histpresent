//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbFocusList.cxx
// Purpose:        MARaBOU utilities: Handle focus for GUI objects
// Description:    Implements focus handling for a list of text entries
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbFocusList.cxx,v 1.2 2004-09-28 13:47:33 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TGMrbFocusList.h"

ClassImp(TGMrbFocusList)

void TGMrbFocusList::StartFocus(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::StartFocus
// Purpose:        Set focus to a given entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->FindObject(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->First();
	if (entry) entry->SetFocus();
}

void TGMrbFocusList::FocusForward(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::FocusForward
// Purpose:        Set focus to next entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->After(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->First();
	if (entry) entry->SetFocus();
}

void TGMrbFocusList::FocusBackward(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::FocusBackward
// Purpose:        Set focus to previous entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->Before(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->Last();
	if (entry) entry->SetFocus();
}

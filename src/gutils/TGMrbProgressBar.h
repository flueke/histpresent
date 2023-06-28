#ifndef __TGMrbProgressBar_h__
#define __TGMrbProgressBar_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbProgressBar.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbProgressBar       -- a transient frame
//                                             showing a progress bar
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbProgressBar.h,v 1.7 2009-08-19 12:52:49 Rudolf.Lutter Exp $
// Date:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGProgressBar.h"

#include "TGTextView.h"
#include "TMrbLogger.h"
#include "TMrbLofNamedX.h"
#include "TGMrbTextButton.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProgressBar
// Purpose:        Display messages in a scrollable listbox
// Description:    Extracts messages from MARaBOU's message loggin system
//                 and shows them in a scrollable listbox widget
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbProgressBar: public TGTransientFrame {

	public:
		enum {	kBarWidth = 400, kBarHeight = 50	};

	public:
		TGMrbProgressBar(const TGWindow * Parent, const TGWindow * Main,
												const Char_t * Title,
												UInt_t Width = kBarWidth,
												const Char_t * BarColor = "blue",
												const Char_t * BarText = NULL,
												Bool_t WithDetail = kFALSE);

		~TGMrbProgressBar() { fHeap.Delete(); };				// default dtor

//		TGMrbProgressBar(const TGMrbProgressBar & f) : TGTransientFrame(f) {};	// default copy ctor

		inline void SetRange(Float_t Min, Float_t Max) { fBar->SetRange(Min, Max); };
		inline void Increment(Float_t Incr, const Char_t * Detail = NULL, Int_t Sleep = 50) {
			fBar->Increment(Incr);
			if (Detail && fDetail)  fDetail->SetText(Detail);
			gSystem->Sleep(Sleep);
		};
		inline void SetBarColor(const Char_t * Color) { fBar->SetBarColor(Color); };
		inline void SetPosition(Int_t Position) { fBar->SetPosition(Position); };
		inline void Reset() { fBar->Reset(); };

		inline TGHProgressBar * Bar() { return fBar; };

		inline void DeleteWindow() { TGTransientFrame::DeleteWindow(); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;

		TGHProgressBar * fBar;			//! progress bar
		TGLabel * fDetail;				//! label to show details
		TGMrbTextButtonList * fAction;	//! action buttons

		TMrbLofNamedX fLofActions;

	ClassDef(TGMrbProgressBar, 1)		// [GraphUtils] MARaBOU's message viewer
};

#endif

#ifndef __TGMrbMessageViewer_h__
#define __TGMrbMessageViewer_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbMessageViewer.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbMessageViewer       -- a transient frame
//                                             displaying messages
// Description:        Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbMessageViewer.h,v 1.6 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"

#include "TGTextView.h"
#include "TMrbLogger.h"
#include "TMrbLofNamedX.h"
#include "TGMrbTextButton.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMessageViewer
// Purpose:        Display messages in a scrollable listbox
// Description:    Extracts messages from MARaBOU's message loggin system
//                 and shows them in a scrollable listbox widget
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbMessageViewer: public TGTransientFrame {

	public:
		enum EGMrbViewerId	{
								kGMrbViewerIdReset,
								kGMrbViewerIdAll,
								kGMrbViewerIdErrors,
								kGMrbViewerIdAdjust,
								kGMrbViewerIdClose
							};
										
	public:
		TGMrbMessageViewer(const TGWindow * Parent, const TGWindow * Main,
												const Char_t * Title,
												UInt_t Width, UInt_t Height,
												TMrbLogger * MsgLog = NULL,
												Bool_t Connect = kFALSE,
												const Char_t * File = NULL);

		~TGMrbMessageViewer() {};				// default dtor

//		TGMrbMessageViewer(const TGMrbMessageViewer & f) : TGTransientFrame(f) {};	// default copy ctor

		Int_t AddEntries(UInt_t Type = TMrbLogMessage::kMrbMsgAny, Int_t Start = 0, Bool_t WithDate = kTRUE);
		virtual Bool_t Notify();

		void ActionButtonPressed(Int_t FrameId, Int_t Button);		// slot method

		inline UInt_t Enable(UInt_t Bits) { fEnabled |= Bits; return(fEnabled); };
		inline UInt_t Disable(UInt_t Bits) { fEnabled &= ~Bits; return(fEnabled); };
		
		inline void SetWithDate(Bool_t WithDate) { fWithDate = WithDate; };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
	
		UInt_t fWidth;
		UInt_t fHeight;
		
		UInt_t fEnabled;				// one bit for each stream
		Bool_t fWithDate;				// kTRUE, if message date is to be shown
		TMrbLogger * fMsgLog;			//! connect to logging system
		TString fLogFile;				// log file

		TGTextView * fTextView; 		//! text viewer widget
		TGMrbTextButtonList * fAction; //! action buttons
								
		TMrbLofNamedX fLofActions;
		
	ClassDef(TGMrbMessageViewer, 1)		// [GraphUtils] MARaBOU's message viewer
};

#endif

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbMessageViewer.cxx
// Purpose:        MARaBOU graphic utilities: a message viewer
// Description:    Implements class methods to show messages
//                 in a scrollable listbox.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbMessageViewer.cxx,v 1.5 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
//
//Begin_Html
/*
<img src=gutils/TGMrbMessageViewer.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TGMrbLayout.h"
#include "TGMrbMessageViewer.h"
#include "TVirtualX.h"

const SMrbNamedX kGMrbMsgLofActions[] =
			{
				{TGMrbMessageViewer::kGMrbViewerIdReset, 	"Reset",		"Reset list of messages"				},
				{TGMrbMessageViewer::kGMrbViewerIdAll,		"All",	 	 	"Show all messages"						},
				{TGMrbMessageViewer::kGMrbViewerIdErrors,	"Errors",		"Show errors only"						},
				{TGMrbMessageViewer::kGMrbViewerIdAdjust,	"Adjust",		"Adjust width" 		 					},
				{TGMrbMessageViewer::kGMrbViewerIdClose,	"Close",		"Stop message display & close window"	},
				{0, 										NULL,			NULL									}
			};

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

//ClassImp(TGMrbMessageViewer)

TGMrbMessageViewer::TGMrbMessageViewer( const TGWindow * Parent,
										const TGWindow * Main,
										const Char_t * Title,
										UInt_t Width, UInt_t Height,
										TMrbLogger * MsgLog,
										Bool_t Connect,
										const Char_t * File) : TGTransientFrame(Parent, Main, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMessageViewer
// Purpose:        Display messages in scrollable listbox
// Arguments:      TGWindow * Parent             -- parent window
//                 TGWindow * Main               -- main window
//                 Char_t * Title                -- title
//                 UInt_t Width                  -- window width
//                 UInt_t Height                 -- window height
//                 TMrbLogger * MsgLog           -- where to take messages from
//                 Bool_t Connect                -- kTRUE if to be connected to logging system
//                 Char_t * File                 -- where to read messages from
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(font, black, gray);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);

//	Initialize several lists
	fLofActions.SetName("Action buttons");
	fLofActions.AddNamedX(kGMrbMsgLofActions);

	fTextView = new TGTextView(this, Width, Height);
	HEAP(fTextView);
	this->AddFrame(fTextView, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0));
	
	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);
	fAction = new TGMrbTextButtonList(this, NULL, &fLofActions, -1, 1, Width, 10, frameGC, labelGC, buttonGC);
	HEAP(fAction);
	this->AddFrame(fAction, frameGC->LH());
	((TGMrbButtonFrame *) fAction)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButtonPressed(Int_t, Int_t)");

	fWidth = Width;
	fHeight = Height;

	fEnabled = TMrbLogMessage::kMrbMsgAny;
	fWithDate = kTRUE;
	fMsgLog = MsgLog;
	fLogFile = File ? File : "";

	if (Connect && fMsgLog) fMsgLog->Connect("Flush()", "TGMrbMessageViewer", this, "Notify()");

	fTextView->Resize(Width, Height - fAction->GetDefaultHeight() - 10);

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() + 10), 0, ax, ay, wdum);
	this->Move(ax, ay);

	SetWindowName(Title);
	SetIconName(Title);
	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, Height);
	MapWindow();
}

Int_t TGMrbMessageViewer::AddEntries(UInt_t Type, Int_t Start, Bool_t WithDate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMessageViewer::AddEntries
// Purpose:        Add messages to the viewer widget
// Arguments:      UInt_t Type        -- message type
//                 Int_t Start        -- entry to start with
//                 Bool_t WithDate    -- show message date
// Results:        Int_t NofMessages  -- number of messages added
// Exceptions:     
// Description:    Displays a number of messages from the logging system.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * progName = fMsgLog->GetProgName();
	TObjArray lofMessages;
	fMsgLog->GetEntriesByType(lofMessages, Start, Type);
	Int_t nofMessages = 0;
	TIterator * iter = lofMessages.MakeIterator();
	TMrbLogMessage * msg;
	while (msg = (TMrbLogMessage *) iter->Next()) {
		TString msgText;
		msg->Get(msgText, progName, WithDate, kFALSE);
		msgText = msgText.Strip(TString::kBoth);
		msgText.Resize(msgText.Length() - 1);
		fTextView->AddLine(msgText);
		nofMessages++;
	}
	return(nofMessages);
}

Bool_t TGMrbMessageViewer::Notify() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMessageViewer::Notify
// Purpose:        Get notified if a new message is present
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Displays a number of messages from the logging system.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbLogMessage * msg;
	const Char_t * progName;
					
	progName = fMsgLog->GetProgName();
	msg = fMsgLog->GetLast(fEnabled);
	if (msg) {
		TString msgText;
		msg->Get(msgText, progName, fWithDate, kFALSE);
		msgText = msgText.Strip(TString::kBoth);
		msgText.Resize(msgText.Length() - 1);
		fTextView->AddLine(msgText);
		return(kTRUE);
	} else return(kFALSE);
}

void TGMrbMessageViewer::ActionButtonPressed(Int_t /*FrameId*/, Int_t Button) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMessageViewer::ActionButtonPressed
// Purpose:        Message handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t Button     -- button number
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMessageViewer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Button) {
		case TGMrbMessageViewer::kGMrbViewerIdReset:
			fTextView->Clear();
			break;
		case TGMrbMessageViewer::kGMrbViewerIdAll:
			fTextView->Clear();
			this->AddEntries();
			this->Enable(TMrbLogMessage::kMrbMsgAny);
			break;
		case TGMrbMessageViewer::kGMrbViewerIdErrors:
			fTextView->Clear();
			this->AddEntries(TMrbLogMessage::kMrbMsgError);
			this->Disable(TMrbLogMessage::kMrbMsgAny);
			this->Enable(TMrbLogMessage::kMrbMsgError);
			break;
		case TGMrbMessageViewer::kGMrbViewerIdAdjust:
			if (fTextView->ReturnLineCount() > 1) this->Resize(fTextView->ReturnLongestLineWidth(), fHeight);
			break;
		case TGMrbMessageViewer::kGMrbViewerIdClose:
			fMsgLog->Disconnect("Flush()", this, "Notify()");
			delete this;
			break;
	}
}


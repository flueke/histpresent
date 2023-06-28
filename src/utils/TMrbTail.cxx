//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbTail.cxx
// Purpose:        MARaBOU utilities:
//                 MARaBOU's tail utility
// Description:    Implements class methods to output file contents like tail(1)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbTail.cxx,v 1.6 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <sys/ioctl.h>

#include "TROOT.h"
#include "TObjString.h"
#include "SetColor.h"
#include "TMrbTail.h"

#include "TSysEvtHandler.h"

extern TMrbLogger * gMrbLog;

ClassImp(TMrbTail)

TMrbTail::TMrbTail(const Char_t * TailName, const Char_t * TailFile) : TMrbNamedX(0, TailName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        MARaBOU's tail utility
// Arguments:      Char_t * TailName   -- name of object
//                 Char_t * TailFile   -- file to be opened
// Description:    Initializes MARaBOU's tail utility.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fStrm = fopen(TailFile, "r");
	if (fStrm == NULL) {
		gMrbLog->Err() << "Can't open file - " << TailFile << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		TString title = TailName;
		title += " (file: ";
		title += TailFile;
		title += ")";
		this->SetTitle(title.Data());
		this->Setup();
	}
}
		
TMrbTail::TMrbTail(const Char_t * TailName, FILE * TailStrm) : TMrbNamedX(0, TailName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        MARaBOU's tail utility
// Arguments:      Char_t * TailName   -- name of object
//                 FILE * TailStrm     -- file stream
// Description:    Initializes MARaBOU's tail utility.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (TailStrm == NULL) {
		gMrbLog->Err() << "Can't connect to stream" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		this->SetTitle(TailName);
		fStrm = TailStrm;
		this->Setup();
	}
}
		
void TMrbTail::Setup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::Setup
// Purpose:        Common init part
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Inbbitializes tail mechanism.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t fd = fileno(fStrm);
	this->SetIndex(fd);

	fStopIt = kTRUE;
	fOutputMode = kMrbTailOutUndef;
	fLogger = NULL;
	fStdout = NULL;
	fStderr = NULL;
	fFH = NULL;
}

void TMrbTail::Start() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::Start
// Purpose:        Start output
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Starts timer
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fFH == NULL) {
		fFH = new TFileHandler(this->GetIndex(), TFileHandler::kRead);
		fFH->Add();
		if (kMrbTailOutSlot) {
			fFH->Connect("Notified()", fRecv.GetName(), fRecv.GetAssignedObject(), fRecv.GetTitle());
		} else {
			fFH->Connect("Notified()", this->ClassName(), this, "HandleInput()");
		}
	}
}

void TMrbTail::HandleInput() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::HandleInput
// Purpose:        Called on signal Notified()
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Handle input from selected stream
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t str[1024];

	if (fgets(str, 1024, fStrm)) {
		str[strlen(str) - 1] = '\0';
		TString xstr = str;
		switch (fOutputMode) {
			case kMrbTailOutLogger: 	this->ToLogger(xstr); break;
			case kMrbTailOutStdio:		this->ToStdio(xstr); break;
			case kMrbTailOutSlot:		break;
		}	
	}
}

Bool_t TMrbTail::SetOutput(TMrbLogger * Logger) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      TMrbLogger * Logger   -- output to logger
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fLogger = Logger;
		fOutputMode = kMrbTailOutLogger;
		return(kTRUE);
	}
}

Bool_t TMrbTail::SetOutput(ostream & Stdout, ostream & Stderr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      ostream & Stdout     -- output to stdio
//                 ostream & Stderr     -- ... stderr
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fStdout = &Stdout;
		fStderr = &Stderr;
		fOutputMode = kMrbTailOutStdio;
		return(kTRUE);
	}
}

Bool_t TMrbTail::SetOutput(const Char_t * RecvName, TObject * Recv, const Char_t * Slot) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      Char_t * RecvName      -- name of receiving class
//                 TObject * Recv         -- receiving class
//                 Char_t * Slot          -- slot method
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fRecv.SetName(RecvName);
		fRecv.SetTitle(Slot);
		fRecv.AssignObject(Recv);
		fOutputMode = kMrbTailOutSlot;
		return(kTRUE);
	}
}

void TMrbTail::ToLogger(TString & Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::ToLogger
// Purpose:        Output to logger
// Arguments:      TString & Text   -- line of text to be output
// Results:        --
// Exceptions:     
// Description:    Outputs text to logger
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Text(0) == '?') 		fLogger->Err() << Text << endl;
	else if (Text(0) == '%')	fLogger->Wrn() << Text << endl;
	else						fLogger->Out() << Text << endl;
	fLogger->Flush();
}

void TMrbTail::ToStdio(TString & Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::ToStdio
// Purpose:        Output to stdio
// Arguments:      TString & Text   -- line of text to be output
// Results:        --
// Exceptions:     
// Description:    Outputs text to cout/cerr
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Text(0) == '?') 		*fStderr << Text << endl;
	else if (Text(0) == '%')	*fStderr << Text << endl;
	else						*fStdout << Text << endl;
}

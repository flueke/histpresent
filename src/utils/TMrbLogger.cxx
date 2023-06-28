//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbLogger.cxx
// Purpose:        MARaBOU utilities:
//             MARaBOU's (error) message logger
// Description:        Implements class methods to output/store messages
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbLogger.cxx,v 1.14 2008-08-26 06:33:24 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include <sstream>
#include "TROOT.h"
#include "TFile.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "SetColor.h"
#include "TMrbLogger.h"

ClassImp(TMrbLogMessage)
ClassImp(TMrbLogger)

const SMrbNamedXShort kMrbMsgTypes[] =
							{
								{	TMrbLogMessage::kMrbMsgMessage, 			"Message" 		},
								{	TMrbLogMessage::kMrbMsgError,	 			"Error" 		},
								{	TMrbLogMessage::kMrbMsgWarning, 			"Warning" 		},
								{	TMrbLogMessage::kMrbMsgAny, 	 			"Any" 	 		},
								{	TMrbLogMessage::kMrbMsgAny, 	 			"*" 	 		},
								{	0, 											NULL			}
							};

const SMrbNamedXShort kMrbMsgOpenModes[] =
							{
								{	TMrbLogger::kMrbMsgFileNew,					"New"	 		},
								{	TMrbLogger::kMrbMsgFileAppend,				"Append"	 	},
								{	0, 											NULL			}
							};

TMrbLogger * gMrbLog = NULL;			// global access to logging system

TMrbLogMessage::TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
									const Char_t * ClassName, const Char_t * Method, const Char_t * Text, Bool_t Indent) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogMessage
// Purpose:        Store a message
// Arguments:      EMrbMsgType Type       -- message type
//                 Char_t * Color         -- color to be used for output
//                 Char_t * ClassName     -- calling class
//                 Char_t * Method        -- calling method
//                 Char_t * Text          -- message text
//                 Bool_t Indent          -- don't output class/method but indent text	
// Description:    Message book keeping.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fType = Type;
	if (Color == NULL || *Color == '\0') {
		switch (fType) {
			case kMrbMsgMessage:	fColor = setblack; break;
			case kMrbMsgError:		fColor = setred; break;
			case kMrbMsgWarning:	fColor = setmagenta; break;
			default:				fColor = setblack; break;
		}
	} else fColor = Color;
	fClassName = ClassName ? ClassName : "";
	fMethod = Method ? Method : "";
	fText = Text;
	fIndent = Indent;
};
		
const Char_t * TMrbLogMessage::Get(TString & FmtMsg,
						const Char_t * ProgName, Bool_t WithDate, Bool_t WithColors) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogMessage::Get
// Purpose:        Get message formatted
// Arguments:      TString FmtMsg          -- where to store the message
//                 Char_t * ProgName       -- name of program issuing this message
//                 Bool_t WithDate         -- output date if kTRUE
//                 Bool_t WithColors       -- add colors if kTRUE
// Results:        const Char_t * FmtMsg   -- pointer to stored string
// Exceptions:     
// Description:    Returns formatted message text.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FmtMsg = "";

	if (WithDate) {
		switch (fType) {
			case kMrbMsgMessage:	FmtMsg += "*-["; break;
			case kMrbMsgError:		FmtMsg += "?-["; break;
			case kMrbMsgWarning:	FmtMsg += "!-["; break;
			default:				FmtMsg += "  ["; break;
		}
		FmtMsg += fDatime.AsString();
		FmtMsg += "]- ";
	}

	if (*ProgName != '\0') {
		FmtMsg += ProgName;
		FmtMsg += " <> ";
	}
	if (WithColors) FmtMsg += fColor;

	TString fmt = FmtMsg;

	if (fClassName.Length() > 0) {
		FmtMsg += fClassName;
		FmtMsg += ":";
		if (fMethod.Length() > 0) {
			FmtMsg += ":";
			FmtMsg += fMethod;
			FmtMsg += "(): ";
		} else {
			FmtMsg += " ";
		}
	} else if (fMethod.Length() > 0) {
		FmtMsg += fMethod;
		FmtMsg += "(): ";
	}

	if (fIndent) {
		Int_t l = FmtMsg.Length() - fmt.Length();
		FmtMsg = Form("%s%*s", fmt.Data(), l, "");
 	}

	FmtMsg += fText;
	if (WithColors) FmtMsg += setblack;

	return(FmtMsg.Data());
}
	
TMrbLogger::TMrbLogger(const Char_t * LogFile, const Char_t * ProgName) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger
// Purpose:        MARaBOU's message logging
// Arguments:      Char_t LogFile   -- name of log file
// Description:    Initializes MARaBOU's message log system.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLogFile = LogFile;
	fLog = fLogFile.IsNull() ? NULL : new ofstream();

	fOut = new ostringstream();
	fErr = new ostringstream();
	fWrn = new ostringstream();
	fEnabled = TMrbLogger::kMrbMsgCout | TMrbLogger::kMrbMsgCerr;
	if (fLog) fEnabled |= TMrbLogger::kMrbMsgLog;
	fLofMessages.Delete();
	fIndexOfLastPrinted = 0;
//	gROOT->Append(this);
	this->SetProgName(ProgName);
	if (fLog) this->Open(fLogFile.Data());
	this->SetName(fProgName.IsNull() ? (fLogFile.IsNull() ? "TMrbLogger" : fLogFile.Data()) : fProgName.Data());
	this->SetTitle("MARaBOU's (error) message logger");
	gROOT->Append(this);
}

void TMrbLogger::SetProgName(const Char_t * ProgName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::SetProgName
// Purpose:        Define program name
// Arguments:      Char_t * ProgName      -- program name
// Results:        --
// Exceptions:     
// Description:    Defines the program name.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fProgName = ProgName;
	if (!fProgName.IsNull()) this->SetName(fProgName.Data());
}

Bool_t TMrbLogger::Open(const Char_t * LogFile, const Char_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Open
// Purpose:        Open log file
// Arguments:      Char_t * LogFile      -- name of log file
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens a file to store (error) messages.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	fLogFile = LogFile;

	opt.AddNamedX(kMrbMsgOpenModes);
	if ((nx = opt.FindByName(Option)) != NULL && nx->GetIndex() == TMrbLogger::kMrbMsgFileNew) {
			fLog->open(fLogFile.Data(), ios::out);
	} else {
			fLog->open(fLogFile.Data(), ios::app);
	}
	
	if (!fLog->good()) {
		cerr	<< setred
				<< this->ClassName() << "::Open(): "
				<< gSystem->GetError() << " - " << fLogFile
				<< setblack << endl;
		return(kFALSE);
	}
	cout	<< setblue
			<< this->ClassName() << "::Open(): Writing (error) messages to log file " << fLogFile
			<< setblack << endl;
	return(kTRUE);	
}

Bool_t TMrbLogger::Close() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Close
// Purpose:        Close log file
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Closes current log file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString logRoot;

	if (fLog && fLog->good()) {
		fLog->close();
		cout	<< setblue
				<< this->ClassName() << "::Close(): Closing log file " << fLogFile
				<< " (" << fLofMessages.GetLast() + 1 << " message(s))"
				<< setblack << endl;
		logRoot = fLogFile;
		logRoot += ".root";
		TFile * save = new TFile(logRoot.Data(), "NEW");
		if (save->IsOpen()) {
			fLofMessages.Write();
			save->Close();
			cout	<< setblue
					<< this->ClassName() << "::Close(): Save log messages to file " << logRoot
					<< setblack << endl;
		}
		delete save;
		fLofMessages.Delete();
		fIndexOfLastPrinted = 0;	
		return(kTRUE);
	} else return(kFALSE);
}
 
Bool_t TMrbLogger::Flush(const Char_t * ClassName, const Char_t * Method, const Char_t * Color, Bool_t Indent) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Flush
// Purpose:        Output message(s).
// Arguments:      Char_t * ClassName    -- calling class
//                 Char_t * Method       -- calling method
//                 Char_t * Color        -- text color to be used
//                 Bool_t Indent         -- don't output class/method but indent text	
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Outputs contents of out and err strings
//                 to log file, cout, cerr, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString str;
	TMrbLogMessage * msg;
		
	*fOut << ends;
	str = fOut->str().c_str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgMessage, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCout) cout << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName, kTRUE, kFALSE) << flush;
	}
	delete fOut;
	fOut = new ostringstream();
	
	*fErr << ends;
	str = fErr->str().c_str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgError, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	delete fErr;
	fErr = new ostringstream();

	*fWrn << ends;
	str = fWrn->str().c_str();
	if (str.Length() != 0) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgWarning, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	delete fWrn;
	fWrn = new ostringstream();

	this->Emit("Flush()");

	return(kTRUE);
}

Bool_t TMrbLogger::OutputMessage(TMrbLogMessage::EMrbMsgType MsgType, const Char_t * Msg,
						const Char_t * ClassName, const Char_t * Method, const Char_t * Color, Bool_t Indent) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::OutputMessage
// Purpose:        Output message(s).
// Arguments:      EMrbMsgType MsgType   -- message type (message, error, warning)
//                 Char_t * Msg          -- text
//                 Char_t * ClassName    -- calling class
//                 Char_t * Method       -- calling method
//                 Char_t * Color        -- text color to be used	
//                 Bool_t Indent         -- don't output class/method but indent text	
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Outputs message directly
//                 to log file, cout, cerr, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString str = Msg;
	if (str(str.Length()) != '\n') str += '\n';

	TMrbLogMessage * msg;
		
	if (MsgType == TMrbLogMessage::kMrbMsgMessage) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgMessage, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCout) cout << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName, kTRUE, kFALSE) << flush;
	} else if (MsgType == TMrbLogMessage::kMrbMsgError) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgError, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	} else if (MsgType == TMrbLogMessage::kMrbMsgWarning) {
		msg = new TMrbLogMessage(TMrbLogMessage::kMrbMsgWarning, Color, ClassName, Method, str, Indent);
		fLofMessages.Add(msg);
		if (fEnabled & TMrbLogger::kMrbMsgCerr) cerr << msg->Get(str, "", kFALSE, kTRUE) << flush;
		if (fEnabled & TMrbLogger::kMrbMsgLog && fLog && fLog->good()) *fLog << msg->Get(str, fProgName.Data(), kTRUE, kFALSE) << flush;
	}
	return(kTRUE);
}

Int_t TMrbLogger::GetNofEntries(UInt_t Type) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::GetNofEntries
// Purpose:        Return number of entries of given type
// Arguments:      UInt_t Type           -- message type
// Results:        Int_t NofEntries      -- number of entries found
// Exceptions:     
// Description:    Returns number of entries
// Keywords:
////////////////////////////////////////////////////////////////////////Lof//////

	Int_t nofEntries = 0;
	TMrbLogMessage * msg = (TMrbLogMessage *) fLofMessages.At(0);
	while (msg) {
		if (msg->GetType() & Type) nofEntries++;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
	return(nofEntries);
}

Int_t TMrbLogger::GetEntriesByType(TObjArray & MsgArr, Int_t Start, UInt_t Type) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::GetEntriesByType
// Purpose:        Extract entries of given type
// Arguments:      TObjArray & MsgArr    -- where to store messages
//                 Int_t Start           -- entry to start with
//                 UInt_t Type           -- message type
// Results:        Int_t NofEntries      -- number of entries found
// Exceptions:     
// Description:    Extracts messages of given type.
// Keywords:
////////////////////////////////////////////////////////////////////////Lof//////

	MsgArr.Clear();
	if (Start > fLofMessages.GetLast() + 1) return(0);
	if (Start < 0) Start = 0;
	Int_t nofEntries = 0;
	TMrbLogMessage * msg = (TMrbLogMessage *) fLofMessages.At(Start);
	while (msg) {
		if (msg->GetType() & Type) {
			MsgArr.Add(msg);
			nofEntries++;
		}
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	};
	return(nofEntries);
}

TMrbLogMessage * TMrbLogger::GetLast(const Char_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::GetLast
// Purpose:        Return last message
// Arguments:      const Char_t * Option      -- message type
// Results:        TMrbLogMessage * LastMsg   -- pointer to last message
//                                               of requested type
// Exceptions:     
// Description:    Returns last message. Mainly to be used by GUI.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Option)) != NULL) {
		return(this->GetLast(nx->GetIndex()));
	} else {
		cerr	<< setred
				<< this->ClassName() << "::GetLast(): Illegal message type - " << Option
				<< setblack << endl;
		return(NULL);
	}
}

TMrbLogMessage * TMrbLogger::GetLast(UInt_t Type) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::GetLast
// Purpose:        Return last message
// Arguments:      UInt_t Type                -- message type
// Results:        TMrbLogMessage * LastMsg   -- pointer to last message
//                                               of requested type
// Exceptions:     
// Description:    Returns last message. Mainly to be used by GUI.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	TMrbLogMessage * msg;
			
	msg = (TMrbLogMessage *) fLofMessages.At(fLofMessages.GetLast());
	if (msg->GetType() & Type) return(msg); else return(NULL);
}

void TMrbLogger::Print(Int_t Tail, const Char_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Print
// Purpose:        Print messages
// Arguments:      Int_t Tail            -- point last 'Tail' messages only
//                 const Char_t * Option -- message type
// Results:        --
// Exceptions:     
// Description:    Outputs (part of) message table.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Option)) != NULL) {
		this->Print(Tail, nx->GetIndex());
	} else {
		cerr	<< setred
				<< this->ClassName() << "::Print(): Illegal message type - " << Option
				<< setblack << endl;
	}
}

void TMrbLogger::Print(Int_t Tail, UInt_t Type) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Print
// Purpose:        Print messages
// Arguments:      Int_t Tail            -- point last 'Tail' messages only
//                 UInt_t Type           -- message type
// Results:        --
// Exceptions:     
// Description:    Outputs (part of) message table.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	TMrbLogMessage * msg;
	TString str;
			
	if (Tail >= 0) idx = 0; else idx = fLofMessages.GetLast() - Tail;
	if (idx < 0) idx = 0;
	
	msg = (TMrbLogMessage *) fLofMessages.At(idx);
	while (msg) {
		if (msg->GetType() & Type) cout << msg->Get(str, "", kTRUE, kTRUE) << flush;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
}

void TMrbLogger::PrintSinceLastCall(const Char_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::PrintSinceLastCall
// Purpose:        Print new messages since last printout
// Arguments:      const Char_t * Option -- message type
// Results:        --
// Exceptions:     
// Description:    Outputs recently created messages.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX opt;
	TMrbNamedX * nx;
		
	opt.AddNamedX(kMrbMsgTypes);
	if ((nx = opt.FindByName(Option)) != NULL) {
		this->PrintSinceLastCall(nx->GetIndex());
	} else {
		cerr	<< setred
				<< this->ClassName() << "::PrintSinceLastCall(): Illegal message type - " << Option
				<< setblack << endl;
	}
}

void TMrbLogger::PrintSinceLastCall(UInt_t Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::PrintSinceLastCall
// Purpose:        Print new messages since last printout
// Arguments:      UInt_t Type           -- message type
// Results:        --
// Exceptions:     
// Description:    Outputs recently created messages.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLogMessage * msg;
	TString str;
			
	msg = (TMrbLogMessage *) fLofMessages.At(fIndexOfLastPrinted);
	while (msg) {
		if (msg->GetType() & Type) cout << msg->Get(str, "", kTRUE, kTRUE) << flush;
		msg = (TMrbLogMessage *) fLofMessages.After(msg);
	}
	fIndexOfLastPrinted = fLofMessages.GetLast() + 1;
}

const Char_t * TMrbLogger::Prefix(const Char_t * Identifier, const Char_t * ProgName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger::Prefix
// Purpose:        Returns line prefix
// Arguments:      Char_t * Identifier  -- line identifier
//                 Char_t * ProgName    -- calling program
// Results:        Char_t * Prefix      -- line prefix
// Exceptions:     
// Description:    Formats a line prefix:
//                    "<Id>-[<Date & Time>]- <ProgName> <> "
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TDatime dt;
	fPrefix = Identifier;
	fPrefix += "-[";
	fPrefix += dt.AsString();
	fPrefix += "]- ";
	if (ProgName != NULL && *ProgName != '\0') {
		fPrefix += ProgName;
		fPrefix += " <> ";
	}
	return(fPrefix.Data());
}
	

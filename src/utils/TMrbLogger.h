#ifndef __TMrbLogger_h__
#define __TMrbLogger_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLogger.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbLogger    -- message/error logging
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLogger.h,v 1.13 2008-08-26 06:33:24 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TObject.h"
#include "TNamed.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TDatime.h"

#include "RQ_OBJECT.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogMessage
// Purpose:        (Error) Message with date, time, and calling info
// Description:    Message bookkeeping
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLogMessage: public TObject {

	public:
		enum EMrbMsgType	{	kMrbMsgMessage	= BIT(0), 		// normal message
								kMrbMsgError	= BIT(1),		// error message
								kMrbMsgWarning	= BIT(2),		// warning
								kMrbMsgAny		= kMrbMsgMessage | kMrbMsgError | kMrbMsgWarning
							};
					
	public:

		TMrbLogMessage() {};
		TMrbLogMessage(EMrbMsgType Type, const Char_t * Color,
										const Char_t * ClassName, const Char_t * Method, const Char_t * Text, Bool_t Indent = kFALSE);
		
		~TMrbLogMessage() {};
		
		inline UInt_t GetDatime() const { return(fDatime.Get()); };
		inline EMrbMsgType GetType() const { return(fType); };
		inline const Char_t * GetColor() const { return(fColor.Data()); };
		inline const Char_t * GetClassName() const { return(fClassName.Data()); };
		inline const Char_t * GetMethod() const { return(fMethod.Data()); };
		inline const Char_t * GetText() const { return(fText.Data()); };
		const Char_t * Get(TString & FmtMsg,
									const Char_t * ProgName,
									Bool_t WithDate = kFALSE,
									Bool_t WithColors = kTRUE) const;
				
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t fIndent;
		TDatime fDatime;
		EMrbMsgType fType;
		TString fColor;
		TString fClassName;
		TString fMethod;
		TString fText;
		
	ClassDef(TMrbLogMessage, 1) 	// [Utils] Single (error) message
};	
		
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLogger
// Purpose:        MARaBOU's message/error logger
// Description:    Methods to output (error) messages to different output channels.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLogger: public TNamed {

	RQ_OBJECT("TMrbLogger")

	public:
		enum EMrbMsgRoute		{	kMrbMsgCout 	= BIT(0), 		// stdout/cout
									kMrbMsgCerr 	= BIT(1), 		// stderr/cerr
									kMrbMsgLog		= BIT(2)		// log file
								};

		enum EMrbMsgFileMode	{	kMrbMsgFileNew,
									kMrbMsgFileAppend
								};

	public:
		TMrbLogger(const Char_t * LogFile = "marabou.log", const Char_t * ProgName = "");   // ctor

		~TMrbLogger() { Reset(); };						// dtor

		Bool_t Flush(const Char_t * ClassNameL = "", const Char_t * Method = "", const Char_t * Color = NULL, Bool_t Indent = kFALSE);	// *SIGNAL*
		Bool_t Open(const Char_t * LogFile = "marabou.log", const Char_t * Option = "APPEND");
		Bool_t Close();
				
		void SetProgName(const Char_t * ProgName);
		inline const Char_t * GetProgName() const { return(fProgName.Data()); };
		
		Int_t GetNofEntries(UInt_t Type = TMrbLogMessage::kMrbMsgAny) const; 	// number of entries
		inline Int_t GetEntries(TObjArray & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgAny)); };
		inline Int_t GetMessages(TObjArray & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgMessage)); };
		inline Int_t GetErrors(TObjArray & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgError)); };
		inline Int_t GetWarnings(TObjArray & MsgArr, Int_t Start = 0) const { return(GetEntriesByType(MsgArr, Start, TMrbLogMessage::kMrbMsgWarning)); };
		
		inline Int_t GetEntriesSinceLastCall(TObjArray & MsgArr) const { return(GetEntries(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetMessagesSinceLastCall(TObjArray & MsgArr) const { return(GetMessages(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetErrorsSinceLastCall(TObjArray & MsgArr) const { return(GetErrors(MsgArr, fIndexOfLastPrinted)); };
		inline Int_t GetWarningsSinceLastCall(TObjArray & MsgArr) const { return(GetWarnings(MsgArr, fIndexOfLastPrinted)); };
		
		Int_t GetEntriesByType(TObjArray & MsgArr, Int_t Start = 0, UInt_t Type = TMrbLogMessage::kMrbMsgAny) const;
	
		TMrbLogMessage * GetLast(const Char_t * Option = "*") const;
		TMrbLogMessage * GetLast(UInt_t Type) const;
		
		inline UInt_t Enable(UInt_t Bits) { fEnabled |= Bits; return(fEnabled); };
		inline UInt_t Disable(UInt_t Bits) { fEnabled &= ~Bits; return(fEnabled); };
		
		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(Int_t Tail = 0, const Char_t * Option = "*") const;
		void Print(Int_t Tail, UInt_t Type) const;
		
		void PrintSinceLastCall(const Char_t * Option = "Error");
		void PrintSinceLastCall(UInt_t Type);
				
		inline void Reset() {
//			fLofMessages.Delete();
			fIndexOfLastPrinted = 0;
		};
		
		inline Int_t GetIndexOfLastPrinted() const { return(fIndexOfLastPrinted); };
				
		const Char_t * Prefix(const Char_t * Identifier = "*", const Char_t * ProgName = NULL); 	// line prefix
		
		inline Bool_t Out(const Char_t * Msg, const Char_t * ClassNameL = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgMessage, Msg, ClassNameL, Method, Color));
		};
		inline Bool_t Err(const Char_t * Msg, const Char_t * ClassNameL = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgError, Msg, ClassNameL, Method, Color));
		};
		inline Bool_t Wrn(const Char_t * Msg, const Char_t * ClassNameL = "", const Char_t * Method = "", const Char_t * Color = NULL) {
				return(this->OutputMessage(TMrbLogMessage::kMrbMsgWarning, Msg, ClassNameL, Method, Color));
		};
	
		inline ostringstream & Out() { return(*fOut); };
		inline ostringstream & Err() { return(*fErr); };
		inline ostringstream & Wrn() { return(*fWrn); };
		inline ofstream & Log() { return(*fLog); };
				
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t OutputMessage(TMrbLogMessage::EMrbMsgType MsgType, const Char_t * Msg,
									const Char_t * ClassNameL = "", const Char_t * Method = "", const Char_t * Color = NULL, Bool_t Indent = kFALSE);

	protected:

		TString fLogFile;			// name of log file on disk
		TString fProgName;			// program issuing messages
		TString fPrefix;			// line prefix
				
		UInt_t fEnabled;			// one bit for each stream
		ofstream * fLog;			//!
		ostringstream * fOut;		//!
		ostringstream * fErr;		//!
		ostringstream * fWrn;		//!

		Int_t fIndexOfLastPrinted;	// index at last printout
		TObjArray fLofMessages; 	// list of messages
		
	ClassDef(TMrbLogger, 1) 		// [Utils] Message/error logger
};

#endif

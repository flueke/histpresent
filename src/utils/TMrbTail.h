#ifndef __TMrbTail_h__
#define __TMrbTail_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbTail.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbTail    -- tail utility
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbTail.h,v 1.11 2008-08-26 06:33:24 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbNamedX.h"
#include "TMrbLogger.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        Tail utility for MARaBOU
// Description:    Methods to implement a tail function in MARaBOU
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTail: public TMrbNamedX {

	public:
		enum EMrbTailOut		{	kMrbTailOutUndef = 0,
									kMrbTailOutLogger,
									kMrbTailOutStdio,
									kMrbTailOutSlot
								};

	public:
		TMrbTail() {};												 	// default ctor
		TMrbTail(const Char_t * TailName, const Char_t * TailFile); 	// explicit ctors
		TMrbTail(const Char_t * TailName, FILE * TailStrm);

		virtual ~TMrbTail() {};											// dtor

		Bool_t SetOutput(TMrbLogger * Logger);											// define output: logger
		Bool_t SetOutput(ostream & Stdout, ostream & StdErr);							// ... stdio
		Bool_t SetOutput(const Char_t * RecvName, TObject * Recv, const Char_t * Slot); // ... signal/slot

		inline TFileHandler * GetFH() { return(fFH); };
		void Start();
		void HandleInput();

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void Setup();
		void ToLogger(TString & Text);
		void ToStdio(TString & Text);

	protected:
		TString fTailFile;
		FILE * fStrm;					//! ... stream (fdopen(3))
		Bool_t fStopIt;					// kTRUE stops output

		TFileHandler * fFH; 			//!

		EMrbTailOut fOutputMode;		// output mode: logger, stdio, array
		TMrbLogger * fLogger;
		ostream * fStdout;
		ostream * fStderr;
		TMrbNamedX fRecv;

	ClassDef(TMrbTail, 1) 				// [Utils] Tail utility
};

#endif

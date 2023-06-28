//_________________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbHistory.cxx
// Purpose:        MARaBOU utilities:
//                 An (inverse) history buffer
// Description:    Implements a history buffer
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbHistory.cxx,v 1.5 2004-11-16 13:30:27 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TROOT.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TInterpreter.h"
#include "TMrbLogger.h"

#include "TMrbHistory.h"

#include "SetColor.h"

ClassImp(TMrbHistory)

extern TMrbLogger * gMrbLog;			// access to message logging


TObjString * TMrbHistory::AddEntry(TObject * Obj, const Char_t * Command) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::AddEntry
// Purpose:        Add a new entry
// Arguments:      TObject * Obj         -- object issuing this command
//                 Char_t * Command      -- command string
// Results:        TObjString * CmdObj   -- object wrapped around command string 
// Exceptions:
// Description:    Adds a new entry to the history buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ostringstream buf;
	buf << "((" << Obj->ClassName() << " *)"<< Obj << ")->"<< Command;
	TString st(buf.str());
	TObjString * objStr = new TObjString(st);
//	TObjString * objStr = new TObjString(Form("((%s *) %#lx)->%s", Obj->ClassName(), Obj, Command));
	this->Add(objStr);
//	this->Add(new TObjString(Form("((%s *) %#lx)->%s", Obj->ClassName(), Obj, Command)));
	return(objStr);
}

const Char_t * TMrbHistory::GetEntry(Int_t Index) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::GetEntry
// Purpose:        Return an existing entry
// Arguments:      Int_t Index       -- entry index
// Results:        Char_t * Command  -- entry contents
// Exceptions:
// Description:    Returns an entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Index < 0 || Index > this->GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << Index
						<< " (should be in [0, " << this->GetLast() << "])"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(NULL);
	}
	return(((TObjString *) this->At(Index))->GetString().Data());
}

Bool_t TMrbHistory::RemoveEntry(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::RemoveEntry
// Purpose:        Remove an existing entry
// Arguments:      Int_t Index       -- entry index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes an entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Index < 0 || Index > this->GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << Index
						<< " (should be in [0, " << this->GetLast() << "])"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "Remove");
		return(kFALSE);
	}
	this->RemoveAt(Index);
	return(kTRUE);
}

Bool_t TMrbHistory::Exec(Int_t Index) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::Exec
// Purpose:        Execute command of an existing entry
// Arguments:      Int_t Index       -- entry index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes command line.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Index < 0 || Index > this->GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << Index
						<< " (should be in [0, " << this->GetLast() << "])"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "Exec");
		return(kFALSE);
	}

	gROOT->ProcessLine(this->GetEntry(Index));
	return(kTRUE);
}

Bool_t TMrbHistory::Restore(Bool_t ReverseOrder) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::Restore
// Purpose:        Execute commands of all entries
// Arguments:      Bool_t ReverseOrder   -- execute in reverse order
// Results:        --
// Exceptions:
// Description:    Restores history by executing command by command.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetLast() < 0) {
		gMrbLog->Err()	<< "History buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "Restore");
		return(kFALSE);
	}
	
	if (ReverseOrder) {
		for (Int_t i = this->GetLast(); i >= 0; i--) {
			gROOT->ProcessLine(((TObjString *) this->At(i))->GetString().Data());
		}
	} else {
		for (Int_t i = 0; i <= this->GetLast(); i++) {
			gROOT->ProcessLine(((TObjString *) this->At(i))->GetString().Data());
		}
	}
	return(kTRUE);
}

void TMrbHistory::Print(ostream & Out) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory::Print
// Purpose:        Print contents of history buffer
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Prints history buffer to Out.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetLast() < 0) {
		gMrbLog->Err()	<< "History buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}
	
	Out << "Contents of history buffer:" << endl
		<< "------------------------------------------------------" << endl;
	for (Int_t i = 0; i <= this->GetLast(); i++) {
		Out << setw(3) << i << ":   " << this->GetEntry(i) << endl;
	}
}

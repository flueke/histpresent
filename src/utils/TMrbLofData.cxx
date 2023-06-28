//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbLofData.cxx
// Purpose:        MARaBOU utilities: A structured data list
// Description:    Implements class methods to manage a list of data
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbLofData.cxx,v 1.5 2004-11-16 13:30:27 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "SetColor.h"
#include "TMrbLofData.h"
#include "TMrbLogger.h"

ClassImp(TMrbLofData)

extern TMrbLogger * gMrbLog;			// access to message logging

TMrbLofData::TMrbLofData(const Char_t * Name, Int_t NofEntries, Int_t EntrySize)
																		: TNamed(Name, "List of structured data") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData
// Purpose:        Define a list of data
// Arguments:      Char_t * Name         -- name
//                 Int_t NofEntries      -- number of entries
//                 Int_t EntrySize       -- entry size in bytes
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fNofEntries = NofEntries;
	fEntrySize = EntrySize;
	fData = new char[NofEntries * EntrySize];
	if (fData == NULL) {
		gMrbLog->Err() << this->GetName() << ": Alloc of data section failed" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		this->Reset(kTRUE);
	}
}

Bool_t TMrbLofData::Reset(Bool_t ClearFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Reset
// Purpose:        Reset index and clear data
// Arguments:      Bool_t ClearFlag   -- clear data section if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Resets index to beginning. Clears all data if flag is set.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fData == NULL) return(kFALSE);
	
	fCurIndex = -1;
	if (ClearFlag) return(this->Clear());
	return(kTRUE);
}

Bool_t TMrbLofData::Clear(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Clear
// Purpose:        Clear data
// Arguments:      Int_t Index     -- data index (-1 -> clear list)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears data. If Index = -1 data will be cleared completely.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fData == NULL) return(kFALSE);
	
	if (Index == -1) {
		memset(fData, 0, fNofEntries * fEntrySize);
		return(kTRUE);
	} else if (Index < fNofEntries) {
		memset(fData + Index * fEntrySize, 0, fEntrySize);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< this->GetName() << ": Index out of range - " << Index
						<< " (should be in [0, " << fNofEntries - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Clear");
		return(kFALSE);
	}
}

Bool_t TMrbLofData::Clear(Char_t * Addr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Clear
// Purpose:        Clear data at given address
// Arguments:      Char_t * Addr     -- data address
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears data at given address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetIndex(Addr) == -1) return(kFALSE);
	memset(Addr, 0, fEntrySize);
	return(kTRUE);
}

Bool_t TMrbLofData::Fill(Int_t Index, const Char_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Fill
// Purpose:        Fill in data
// Arguments:      Int_t Index     -- data index (-1 -> fill list)
//                 Char_t Data     -- data to be filled in
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inserts data. If Index = -1 data will be filled completely.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fData == NULL) return(kFALSE);
	
	if (Index == -1) {
		memset(fData, Data, fNofEntries * fEntrySize);
		return(kTRUE);
	} else if (Index < fNofEntries) {
		memset(fData + Index * fEntrySize, Data, fEntrySize);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< this->GetName() << ": Index out of range - " << Index
						<< " (should be in [0, " << fNofEntries - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Fill");
		return(kFALSE);
	}
}

Char_t * TMrbLofData::At(Int_t Index) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::At
// Purpose:        Return pointer to entry
// Arguments:      Int_t Index        -- entry index
// Results:        Char_t * DataPtr   -- pointer to data entry
// Exceptions:     DataPtr = NULL on error
// Description:    Returns pointer to an entry given by its index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fData == NULL) return(NULL);

	if (Index < 0 || Index >= fNofEntries) {
		gMrbLog->Err()	<< this->GetName() << ": Index out of range - " << Index
						<< " (should be in [0, " << fNofEntries - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "At");
		return(NULL);
	} else {
		return(fData + Index * fEntrySize);
	}
}

Char_t * TMrbLofData::Next(Bool_t ClearFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Next
// Purpose:        Return pointer to next entry
// Arguments:      Bool_t ClearFlag   -- clear data if kTRUE
// Results:        Char_t * DataPtr   -- pointer to data entry
// Exceptions:     DataPtr = NULL on end of list
// Description:    Increments internal data pointer by 1 entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * dp;
	
	if (fData == NULL) return(NULL);

	fCurIndex++;
	if (fCurIndex >= fNofEntries) {
		fCurIndex = fNofEntries - 1;
		return(NULL);
	} else {
		dp = fData + fCurIndex * fEntrySize;
		if (ClearFlag) memset(dp, 0, fEntrySize);
		return(dp);
	}
}

Char_t * TMrbLofData::FindEmpty(Char_t * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::FindEmpty
// Purpose:        Return pointer to next empty slot
// Arguments:      Char_t * After     -- start after this entry
// Results:        Char_t * DataPtr   -- pointer to data entry
// Exceptions:     DataPtr = NULL on no empty slot found
// Description:    Searches for an empty slot.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * addr;
	Int_t idx;
		
	if (fData == NULL) return(NULL);

	if (After == NULL) {
		addr = fData;
		idx = 0;
	} else {
		if ((idx = this->GetIndex(After)) == -1) return(NULL);
		idx++;
		addr = After + fEntrySize;
	}
			
	for (Int_t i = idx; i < fNofEntries; i++) {
		if (this->IsEmpty(addr)) return(addr);
		addr += fEntrySize;
	}
	return(NULL);
}
	
Char_t * TMrbLofData::FindNonEmpty(Char_t * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::FindNonEmpty
// Purpose:        Return pointer to next active slot
// Arguments:      Char_t * After     -- start after this entry
// Results:        Char_t * DataPtr   -- pointer to data entry
// Exceptions:     DataPtr = NULL on no slot found
// Description:    Searches for an active slot.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * addr;
	Int_t idx;
		
	if (fData == NULL) return(NULL);

    if (After == NULL) {
        addr = fData;
        idx = 0;
	} else {
		if ((idx = this->GetIndex(After)) == -1) return(NULL);
		idx++;
		addr = After + fEntrySize;
	}
			
	for (Int_t i = idx; i < fNofEntries; i++) {
		if (!this->IsEmpty(addr)) return(addr);
		addr += fEntrySize;
	}
	return(NULL);
}
	
Bool_t TMrbLofData::IsEmpty(Char_t * Addr) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::IsEmpty
// Purpose:        Check if entry is empty
// Arguments:      Char_t * Addr      -- address of entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if current entry is empty.
//                 This method may be overwritten by a class inheriting
//                 from TMrbLofData.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetIndex(Addr) == -1) return(kFALSE);
	return(*((Int_t *) Addr) == 0);
}

Bool_t TMrbLofData::IsEmpty(Int_t Index) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::IsEmpty
// Purpose:        Check if entry is empty
// Arguments:      Int_t Index    -- entry index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if current entry is empty.
//                 This method may be overwritten by a class inheriting
//                 from TMrbLofData.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * addr;
	if ((addr = this->At(Index)) == NULL) return(kFALSE);
	return(*((Int_t *) addr) == 0);
}

void TMrbLofData::Print(ostream & Out, Char_t * Addr, Bool_t NonEmpty) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Print
// Purpose:        Print data
// Arguments:      ostream & Out      -- output stream
//                 Char_t * Addr      -- address of entry
//                 Bool_t NonEmpty    -- print non-empty entries only
// Results:        --
// Exceptions:
// Description:    Prints entry data. Assumes Int_t data.
//                 This method may be overwritten by a class inheriting
//                 from TMrbLofData.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
		
	if (fData == NULL) return;

    if (Addr == NULL) {
		this->Print(Out, -1, NonEmpty);
		return;
	} else if ((idx = this->GetIndex(Addr)) == -1) return;
	this->Print(Out, idx, NonEmpty);	
}

void TMrbLofData::Print(ostream & Out, Int_t Index, Bool_t NonEmpty) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::Print
// Purpose:        Print data
// Arguments:      ostream & Out      -- output stream
//                 Int_t Index        -- entry index
//                 Bool_t NonEmpty    -- print non-empty entries only
// Results:        --
// Exceptions:
// Description:    Prints entry data. Assumes Int_t data.
//                 This method may be overwritten by a class inheriting
//                 from TMrbLofData.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx1, idx2;
	Int_t nofWords;
	Int_t * addr;
	Bool_t needsNewLine = kFALSE;
	Bool_t printHeader = kFALSE;
					
	if (Index == -1) {
		idx1 = 0;
		idx2 = fNofEntries - 1;
	} else if (Index >= fNofEntries) {
		gMrbLog->Err()	<< this->GetName() << ": Index out of range - " << Index
						<< " (should be in [0, " << fNofEntries - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Clear");
		return;
	} else {
		idx1 = Index;
		idx2 = Index;
	}
	
	nofWords = fEntrySize / sizeof(Int_t);
	addr = (Int_t *) this->At(idx1);
	for (Int_t i = idx1; i <= idx2; i++) {
		if (NonEmpty && this->IsEmpty(i)) { addr += nofWords; continue; }
		if (!printHeader) {
			Out << (NonEmpty ? "Non-empty contents" : "Contents") << ends;
			Out << " of data list \"" << this->GetName() << "\":" << endl;
		}
		printHeader = kTRUE;
		Out << setw(5) << i << ": " << ends;
		for (Int_t j = 0; j < nofWords; j++) {
			needsNewLine = (j > 0 && (j % 10) == 0);
			if (needsNewLine) Out << endl << setw(7) << " " << ends;
			Out << setw(7) << *addr++ << ends;
		}
		if (!needsNewLine) Out << endl;
	}	
}

Int_t TMrbLofData::GetIndex(Char_t * Addr) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData::GetIndex
// Purpose:        Calculate entry index from address 
// Arguments:      Char_t * Addr      -- address of entry
// Results:        Int_t Index        -- derived index
// Exceptions:     Index == -1 on error
// Description:    Checks if address is a valid entry address and calculates
//                 entry index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	
	if ((Addr - fData) % fEntrySize) {
		gMrbLog->Err()  << this->GetName() << ": Addr not at entry boundary - "
						<< setbase(16) << (void *) Addr << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "GetIndex");
		return(-1);
	} else {
		idx = (Addr - fData) / fEntrySize;
		if (idx >= fNofEntries - 1) {
			gMrbLog->Err()  << this->GetName() << ": Address out of range - "
							<< setbase(16) << (void *) Addr << setbase(10) << endl;
			gMrbLog->Flush(this->ClassName(), "GetIndex");
			return(-1);
		}
	}                                                                                                                                 
	return(idx);
}

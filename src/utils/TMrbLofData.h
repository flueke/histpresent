#ifndef __TMrbLofData_h__
#define __TMrbLofData_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLofData.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbLofData    -- a list of (structured) data
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLofData.h,v 1.6 2006-01-25 12:21:27 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TString.h"
#include "TSystem.h"
#include "TNamed.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofData
// Purpose:        Define a list of data
// Description:    Class to manage a list of structured data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofData: public TNamed {

	public:
		TMrbLofData() {};													 	// default ctor
		TMrbLofData(const Char_t * Name, Int_t NofEntries, Int_t EntrySize); 	// ctor

		~TMrbLofData() { if (fData) delete [] fData; };							// dtor: delete data area

		Bool_t Reset(Bool_t ClearFlag = kFALSE);	// reset index (& clear data)
		Bool_t Fill(Int_t Index = -1, const Char_t Data = 0);	// fill entry
				
		Char_t * At(Int_t Index) const;		// return pointer to data item at given index
		Char_t * Next(Bool_t ClearFlag = kFALSE);		// return pointer to next item

		void Clear(const Option_t * Option) { TObject::Clear(Option); };
		Bool_t Clear(Int_t Index = -1); 	// clear data at given index
		Bool_t Clear(Char_t * Address); 	// clear data given by addr

		Char_t * FindEmpty(Char_t * After = NULL) const;		// find next empty slot
		Char_t * FindNonEmpty(Char_t * After = NULL) const; 	// find next active slot
				
		Int_t GetIndex(Char_t * Addr) const;					// calculate entry index from address
		
		virtual Bool_t IsEmpty(Char_t * Addr) const;			// check if slot is empty
		virtual Bool_t IsEmpty(Int_t Index) const;

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out, Char_t * Addr = NULL, Bool_t NonEmpty = kFALSE) const;			// print data
		void Print(ostream & Out, Int_t Index, Bool_t NonEmpty = kFALSE) const;
		inline void Print(Char_t * Addr = NULL, Bool_t NonEmpty = kFALSE) const { Print(cout, Addr, NonEmpty); };
		inline void Print(Int_t Index, Bool_t NonEmpty = kFALSE) const { Print(cout, Index, NonEmpty); };
		
		inline Int_t GetNofEntries() const { return(fNofEntries); };	// get number of entries
		inline Int_t GetEntrySize() const { return(fEntrySize); };	// get entry size
				
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
			
		Int_t fNofEntries;				// number of entries
		Int_t fEntrySize;				// entry size in bytes
		Char_t * fData; 				//! address of data

		Int_t fCurIndex;				// current index
		
	ClassDef(TMrbLofData, 1)			// [Utils] A list of structured data
};

#endif

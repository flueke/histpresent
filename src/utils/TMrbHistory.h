#ifndef __TMrbHistory_h__
#define __TMrbHistory_h__
#ifndef __CINT__
#ifndef USECLANG
	#define USECLANG = strcmp(CXX,"clang++")
#endif
#if USELANG == 1
	#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#endif

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbHistory.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbHistory    -- (inverse) history buffer
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbHistory.h,v 1.6 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include "TObjArray.h"
#include "TObjString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistory
// Purpose:        Keep track of actions performed so far
// Description:    Stores commands in an array of strings which may be
//                 accessed later on.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbHistory: public TObjArray {

	public:
		TMrbHistory() {};					// default ctor
		~TMrbHistory() {};					// default dtor

		TObjString * AddEntry(TObject * Obj, const Char_t * Command); 	// add entry
		const Char_t * GetEntry(Int_t Index) const;					// get history entry
		Bool_t RemoveEntry(Int_t Index);							// remove entry
		Bool_t Exec(Int_t Index) const; 									// execute command at entry(i)
		Bool_t Restore(Bool_t ReverseOrder = kTRUE) const; 				// exec all history entries
						
		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out) const;
		inline void Print() const { Print(cout); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:

	ClassDef(TMrbHistory, 1) 	// [Utils] Some extensions to TSystem class
};

#endif

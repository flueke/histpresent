#ifndef __TMrbNamedDouble_h__
#define __TMrbNamedDouble_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbNamedDouble.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbNamedDouble    -- a named double
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbNamedDouble.h,v 1.1 2005-11-21 09:34:00 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TObject.h"
#include "TSystem.h"

#include "TMrbString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedDouble
// Purpose:        A named double
// Description:    Defines a pair (name, value)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbNamedDouble: public TNamed {

	public:
		TMrbNamedDouble() {};				// default ctor

		TMrbNamedDouble(const Char_t * Name, Double_t Value) : TNamed(Name, "") { fValue = Value; };

		~TMrbNamedDouble() {};				// default dtor

		inline Double_t GetValue() { return(fValue); };
		inline void SetValue(Double_t Value) { fValue = Value; };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out) const; 	// print data
		inline void Print() const { Print(cout); };

		inline Bool_t IsSortable() const { return(kTRUE); };	// entries may be sorted
		Int_t Compare(const TObject * Dbl) const; 				// how to compare TMrbNamedDouble objects
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Double_t fValue;

	ClassDef(TMrbNamedDouble, 1) 	// [Utils] A named double
};

#endif

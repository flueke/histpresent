//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbNamedDouble.cxx
// Purpose:        MARaBOU utilities:
//                 A TNamed having an index and an opt. object assigned
// Description:    Implements class methods to manage indexed TNamed objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbNamedDouble.cxx,v 1.1 2005-11-21 09:34:00 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbNamedDouble.h"

ClassImp(TMrbNamedDouble)

Int_t TMrbNamedDouble::Compare(const TObject * Dbl) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedDouble::Compare
// Purpose:        Compare objects
// Arguments:      TObject * Dbl  -- TMrbNamedDouble object to be compared
// Results:        Int_t Restult  -- <0, 0, >0
// Exceptions:
// Description:    Compares TMrbNamedDouble objects.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t s;
	TMrbNamedDouble * dbl = (TMrbNamedDouble *) Dbl;
	s = (Int_t) (fValue - dbl->GetValue());
	return(s);
}

void TMrbNamedDouble::Print(ostream & Out) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNamedDouble::Print
// Purpose:        Print name & value
// Arguments:      ostream & Out   -- where to send it
// Results:        --
// Exceptions:
// Description:    Outputs name and value to stream Out
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << fName << ": " << fValue << endl;
}

//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbVariables.cxx
// Purpose:        MARaBOU utilities: Provide user-defined variables and windows
// Description:    Implements class methods to handle variables and 1-dim windows
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbVariables.cxx,v 1.5 2004-11-16 13:30:27 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbVar.h"
#include "TMrbLofUserVars.h"

#include "SetColor.h"

ClassImp(TMrbVariable)
ClassImp(TMrbVarI)
ClassImp(TMrbVarF)
ClassImp(TMrbVarS)
ClassImp(TMrbVarB)
ClassImp(TMrbVarArrayI)
ClassImp(TMrbVarArrayF)

extern TMrbLofUserVars * gMrbLofUserVars;		// a list of all vars and windows defined so far

TMrbVariable::TMrbVariable() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVariable
// Purpose:        Default constructor
// Description:    Creates list of vars/wdws.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
}

TMrbVariable::~TMrbVariable() {
//__________________________________________________________________[C++ DTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           ~TMrbVariable
// Purpose:        Destructor
// Description:    Remove variable from lists.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

//	gDirectory->GetList()->Remove(this);
	gMrbLofUserVars->Remove(this);
}

void TMrbVariable::Initialize() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVariable::Initialize
// Purpose:        Initialize variable
// Arguments:      
// Results:        
// Exceptions:
// Description:    Initializes variable.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsArray()) {
		switch (this->GetType()) {
			case kVarI:		((TMrbVarArrayI *) this)->Initialize(); break;
			case kVarF:		((TMrbVarArrayF *) this)->Initialize(); break;
		}
	} else {
		switch (this->GetType()) {
			case kVarI:		((TMrbVarI *) this)->Initialize(); break;
			case kVarF:		((TMrbVarF *) this)->Initialize(); break;
			case kVarS:		((TMrbVarS *) this)->Initialize(); break;
			case kVarB:		((TMrbVarB *) this)->Initialize(); break;
		}
	}
}

void TMrbVariable::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVariable::Print
// Purpose:        Print current value
// Arguments:      
// Results:        
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsArray()) {
		switch (this->GetType()) {
			case kVarI:		((TMrbVarArrayI *) this)->Print(Option); break;
			case kVarF:		((TMrbVarArrayF *) this)->Print(Option); break;
		}
	} else {
		switch (this->GetType()) {
			case kVarI:		((TMrbVarI *) this)->Print(Option); break;
			case kVarF:		((TMrbVarF *) this)->Print(Option); break;
			case kVarS:		((TMrbVarS *) this)->Print(Option); break;
			case kVarB:		((TMrbVarB *) this)->Print(Option); break;
		}
	}
}

void TMrbVariable::SetInitialType(UInt_t VarType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVariable::SetInitialType
// Purpose:        Set variable type
// Arguments:      UInt_t VarType    -- type bits
// Results:        
// Exceptions:
// Description:    Defines type bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetUniqueID() != 0) return;

	this->SetUniqueID(VarType);
	gMrbLofUserVars->Append(this);
//	if (gDirectory != NULL) gDirectory->Append(this);
}

TMrbVarI::TMrbVarI(const Char_t * Name, Int_t Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarI
// Purpose:        Define an integer variable
// Arguments:      Char_t * Name       -- name of variable
//                 Int_t Value         -- initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable with integer value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t hasInit;

	SetName(Name);
	fValue = Value;
	fInitValue = Value;
	hasInit = (Value != 0) ? kHasInitValues : 0; 
	fSize = 1;
	this->SetInitialType(hasInit | kVarI);
};

void TMrbVarI::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarI::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option    -- option
// Results:        
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = " << fValue << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "int"
				<< setw(19) << fValue
				<< setw(16) << fInitValue;
		if (IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " ... "
					<< setw(6) << fUpperRange;
		}
		cout	<< endl;
	}
}

TMrbVarF::TMrbVarF(const Char_t * Name, Double_t Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarF
// Purpose:        Define a double variable
// Arguments:      Char_t * Name           -- name of variable
//                 Double_t Value          -- initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable with a double value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t hasInit;

	SetName(Name);
	fValue = Value;
	fInitValue = Value;
	hasInit = (Value != 0.) ? kHasInitValues : 0; 
	fSize = 1;
	this->SetInitialType(hasInit | kVarF);
};

void TMrbVarF::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarF::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option     -- option
// Results:        
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = " << fValue << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "double"
				<< setw(19) << fValue
				<< setw(16) << fInitValue;
		if (IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " ... "
					<< setw(6) << fUpperRange;
		}
		cout	<< endl;
	}
}
TMrbVarS::TMrbVarS(const Char_t * Name, const Char_t * Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarS
// Purpose:        Define a string variable
// Arguments:      Char_t * Name           -- name of variable
//                 Char_t * Value          -- initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable with a string value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t hasInit;

	SetName(Name);
	fValue = Value;
	fInitValue = Value;
	hasInit = (*Value != '\0') ? kHasInitValues : 0; 
	fSize = 1;
	this->SetInitialType(hasInit | kVarS);
};

void TMrbVarS::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarS::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option   -- option
// Results:        
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = " << fValue << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(18) << GetName();
		cout	<< "  char ";
		cout	<< setw(20) << fValue
				<< setw(17) << fInitValue;
		cout	<< endl;
	}
}

TMrbVarB::TMrbVarB(const Char_t * Name, Bool_t Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarB
// Purpose:        Define a boolean variable
// Arguments:      Char_t * Name           -- name of variable
//                 Bool_t Value            -- initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable with a boolean value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SetName(Name);
	fValue = Value;
	fInitValue = Value;
	fSize = 1;
	this->SetInitialType(kVarB | kHasInitValues);
};

void TMrbVarB::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarB::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option   -- option
// Results:        
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = " << fValue << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "bool";
		if (fValue) cout << setw(19) << "kTRUE"; else cout << setw(19) << "kFALSE";
		cout	<< setw(16) << fInitValue << endl;
	}
}

TMrbVarArrayI::TMrbVarArrayI(const Char_t * Name, Int_t Size, Int_t Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayI
// Purpose:        Define an array of integer values
// Arguments:      Char_t * Name           -- name of variable
//                 Int_t Size              -- number of elements
//                 Int_t Value             -- common initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable containing an array of integers
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	UInt_t hasInit;

	SetName(Name);
	fSize = Size;
	fValue.Set(Size);
	for (i = 0; i < Size; i++) fValue[i] = Value;
	fInitValue.Set(Size);
	for (i = 0; i < Size; i++) fInitValue[i] = Value;
	hasInit = (Value != 0) ? kHasInitValues : 0;
	this->SetInitialType(hasInit | kVarI | kVarIsArray);
};

void TMrbVarArrayI::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayI::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option   -- option
// Results:        
// Exceptions:
// Description:    Outputs current values to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Char_t * t;
	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	t = "int";
	for (i = 0; i < fSize; i++) {
		if (i == 0) {
			cout	<< "   "
					<< setiosflags(ios::left) << setw(15) << GetName();
		} else {
			cout	<< "   "
					<< setiosflags(ios::left) << setw(15) << " " ;
		}
		cout	<< "[" << i << "]";
		cout	<< resetiosflags(ios::left)
				<< setw(6) << t
				<< setw(19) << fValue[i]
				<< setw(16) << fInitValue[i];
		t = "";
		if (i == 0 && IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " ... "
					<< setw(6) << fUpperRange;
		}
		cout	<< endl;
	}
}

TMrbVarArrayF::TMrbVarArrayF(const Char_t * Name, Int_t Size, Double_t Value) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayF
// Purpose:        Define an array of double values
// Arguments:      Char_t * Name           -- name of variable
//                 Int_t Size              -- number of elements
//                 Double_t Value          -- common initial value
// Results:        
// Exceptions:     
// Description:    Creates a variable containing an array of integers
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	UInt_t hasInit;

	SetName(Name);
	fSize = Size;
	fValue.Set(Size);
	for (i = 0; i < Size; i++) fValue[i] = Value;
	fInitValue.Set(Size);
	for (i = 0; i < Size; i++) fInitValue[i] = Value;
	hasInit = (Value != 0) ? kHasInitValues : 0;
	this->SetInitialType(hasInit | kVarF | kVarIsArray);
};

void TMrbVarArrayF::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayF::Print
// Purpose:        Print current values
// Arguments:      Char_t * Option   -- option
// Results:        
// Exceptions:
// Description:    Outputs current values to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Char_t * t;
	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	t = "double";
	for (i = 0; i < fSize; i++) {
		if (i == 0) {
			cout	<< "   "
					<< setiosflags(ios::left) << setw(15) << GetName();
		} else {
			cout	<< "   "
					<< setiosflags(ios::left) << setw(15) << " ";;
		}
		cout	<< "[" << i << "]";
		cout	<< resetiosflags(ios::left)
				<< setw(6) << t
				<< setw(19) << fValue[i]
				<< setw(16) << fInitValue[i];
		t = "";
		if (IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " ... "
					<< setw(6) << fUpperRange;
		}
		cout	<< endl;
	}
}

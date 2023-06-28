#ifndef __TMrbVar_h__
#define __TMrbVar_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbVar.h
// Purpose:        Define named variables
// Classes:        TMrbVariable             -- base class for variables
//                 TMrbVarI                 -- user-defined variables:     integer
//                 TMrbVarF                 -- ... float/double
//                 TMrbVarS                 -- ... string
//                 TMrbVarB                 -- ... boolean
//                 TMrbVarArrayI            -- a variable containing an array of ints
//                 TMrbVarArrayF            -- ... floats/double
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVar.h,v 1.5 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TArrayI.h"
#include "TArrayD.h"
#include "TString.h"
#include "TSystem.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVarWdwCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVariable
// Purpose:        Base class for user-defined variables
// Description:    Defines a variable or window and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVariable: public TNamed {

	public:
		TMrbVariable(); 			// default ctor
		~TMrbVariable();			// dtor

		inline UInt_t GetType() const { return(GetUniqueID() & kVarOrWindow); };
		inline UInt_t GetStatus() const { return(GetUniqueID() & kStatus); };
		inline Bool_t IsArray() const { return((GetUniqueID() & kVarIsArray) != 0); };
		inline Bool_t HasInitValues() const { return((GetUniqueID() & kHasInitValues) != 0); };
		inline Bool_t IsRangeChecked() const { return((GetUniqueID() & kIsRangeChecked) != 0); };
		inline Int_t GetSize() const { return(fSize); };

		void Initialize();								// initialize values
		virtual void Print(Option_t * Option = "") const;

		void SetRange(Bool_t Flag = kTRUE) {
			UInt_t varType = GetUniqueID();
			if (Flag) varType |= kIsRangeChecked; else varType &= ~kIsRangeChecked;
			SetUniqueID(varType);
		};

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void SetInitialType(UInt_t VarType);

	protected:
		Int_t fSize;				// number of elements if TMrbVarArrayX

	ClassDef(TMrbVariable, 1)		// [Utils] Base class for user-defined variables
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarI
// Purpose:        Base class for an integer variable
// Description:    Defines an integer variable and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarI: public TMrbVariable {

	public:
		TMrbVarI() {};											// default ctor
		TMrbVarI(const Char_t * Name, Int_t Value = 0);			// ctor
		~TMrbVarI() {}; 										// dtor

		inline Int_t Clip(Int_t Value) const {						// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		inline Int_t & operator[](UInt_t) { return(fValue); };

		inline void Set(Int_t Value) { fValue = Clip(Value); }; 		// set new value
		inline void SetInit(Int_t Value) { fInitValue = Clip(Value); };	// set init value

		inline Int_t Get() const { return(fValue); };

		inline Bool_t IsEQ(Int_t X) const { return(fValue == X); };	// compare vlues
		inline Bool_t IsNE(Int_t X) const { return(fValue != X); };
		inline Bool_t IsLT(Int_t X) const { return(fValue < X); };
		inline Bool_t IsLE(Int_t X) const { return(fValue <= X); };
		inline Bool_t IsGT(Int_t X) const { return(fValue > X); };
		inline Bool_t IsGE(Int_t X) const { return(fValue >= X); };

		inline void Initialize() { fValue = Clip(fInitValue); };	// reset to initial value

		void SetRange(Int_t Xlower, Int_t Xupper) { 			// define range
			SetUniqueID(GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		Int_t fValue;						// current value
		Int_t fInitValue; 					// initial value
		Int_t fLowerRange;					// lower range limit
		Int_t fUpperRange;					// upper range limit

	ClassDef(TMrbVarI, 1)		// [Utils] A used-defined integer variable
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarF
// Purpose:        Base class for an float variable
// Description:    Defines an float variable and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarF: public TMrbVariable {

	public:
		TMrbVarF() {};												// default ctor
		TMrbVarF(const Char_t * Name, Double_t Value = 0.); 		// ctor

		~TMrbVarF() {}; 											// dtor

		inline Double_t Clip(Double_t Value) const {			// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		inline Double_t & operator[](UInt_t) { return(fValue); };

		inline void Set(Double_t Value) { fValue = Clip(Value); };					// set new value

		inline Double_t Get() const { return(fValue); };

		inline Bool_t IsEQ(Double_t X) const { return(fValue == X); };	// compare vlues
		inline Bool_t IsNE(Double_t X) const { return(fValue != X); };
		inline Bool_t IsLT(Double_t X) const { return(fValue < X); };
		inline Bool_t IsLE(Double_t X) const { return(fValue <= X); };
		inline Bool_t IsGT(Double_t X) const { return(fValue > X); };
		inline Bool_t IsGE(Double_t X) const { return(fValue >= X); };

		inline void Initialize() { fValue = Clip(fInitValue); };	// reset to initial value

		void SetRange(Double_t Xlower, Double_t Xupper) {			// define range
			SetUniqueID(GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		Double_t fValue;							// current value
		Double_t fInitValue; 					// initial value
		Double_t fLowerRange;					// lower range limit
		Double_t fUpperRange;					// upper range limit

	ClassDef(TMrbVarF, 1)		// [Utils] A used-defined double/float variable
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarS
// Purpose:        Base class for a string variable
// Description:    Defines a string variable and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarS: public TMrbVariable {

	public:
		TMrbVarS() {};												// default ctor
		TMrbVarS(const Char_t * Name, const Char_t * Value = "");	// ctor

		~TMrbVarS() {}; 								// dtor

		inline void Set(const Char_t * Value) { fValue = Value; };	// set new value

		inline const Char_t * Get() const { return(fValue.Data()); };

		inline Bool_t IsEQ(Char_t * X) const { return(fValue.CompareTo(X) == 0); };	// compare values
		inline Bool_t IsNE(Char_t * X) const { return(fValue.CompareTo(X) != 0); };
		inline Bool_t IsLT(Char_t * X) const { return(fValue.CompareTo(X) < 0); };
		inline Bool_t IsLE(Char_t * X) const { return(fValue.CompareTo(X) <= 0); };
		inline Bool_t IsGT(Char_t * X) const { return(fValue.CompareTo(X) > 0); };
		inline Bool_t IsGE(Char_t * X) const { return(fValue.CompareTo(X) >= 0); };

		inline void Initialize() { fValue = fInitValue; };		// reset to initial value

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		TString fValue;						// current value
		TString fInitValue; 				// initial value

	ClassDef(TMrbVarS, 1)		// [Utils] A used-defined string variable
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarB
// Purpose:        Base class for a boolean variable
// Description:    Defines a boolean variable and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarB: public TMrbVariable {

	public:
		TMrbVarB() {};												// default ctor
		TMrbVarB(const Char_t * Name, Bool_t Value = kFALSE);		// ctor

		~TMrbVarB() {}; 											// dtor

		inline void Set(Bool_t Value) { fValue = Value; };	// set new value

		inline Bool_t Get() const { return(fValue); };

		inline Bool_t IsTrue() const { return(fValue); };

		inline void Initialize() { fValue = fInitValue; };		// reset to initial value

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		Bool_t fValue;						// current value
		Bool_t fInitValue;	 				// initial value

	ClassDef(TMrbVarB, 1)		// [Utils] A used-defined booleant variable
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayI
// Purpose:        Base class for an array of integer variables
// Description:    Defines an array of integer variables and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarArrayI: public TMrbVariable {

	public:
		TMrbVarArrayI() {};													// default ctor
		TMrbVarArrayI(const Char_t * Name, Int_t Size, Int_t Value = 0);	// explicit ctor
 
		~TMrbVarArrayI() {};												// dtor

		inline Int_t Clip(Int_t Value) const {				// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		inline Int_t & operator[](UInt_t i) { return(fValue[i]); };

		inline void Set(UInt_t Index, Int_t Value) { fValue[Index] = Clip(Value); };	// set new value
		inline void SetInit(UInt_t Index, Int_t Value) { fInitValue[Index] = Clip(Value); };	// set init val

		inline Int_t Get(UInt_t Index) const { return(fValue[Index]); };

		inline Bool_t IsEQ(UInt_t Index, Int_t X) const { return(fValue[Index] == X); };	// compare values
		inline Bool_t IsNE(UInt_t Index, Int_t X) const { return(fValue[Index] != X); };
		inline Bool_t IsLT(UInt_t Index, Int_t X) const { return(fValue[Index] < X); };
		inline Bool_t IsLE(UInt_t Index, Int_t X) const { return(fValue[Index] <= X); };
		inline Bool_t IsGT(UInt_t Index, Int_t X) const { return(fValue[Index] > X); };
		inline Bool_t IsGE(UInt_t Index, Int_t X) const { return(fValue[Index] >= X); };

		void SetRange(Int_t Xlower, Int_t Xupper) {		// define range
			SetUniqueID(GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		void Initialize() { for (Int_t i = 0; i < fSize; i++) fValue[i] = fInitValue[i]; };

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		TArrayI fValue;						// current values
		TArrayI fInitValue; 				// initial values
		Int_t fLowerRange;					// lower range limit
		Int_t fUpperRange;					// upper range limit

	ClassDef(TMrbVarArrayI, 1)		// [Utils] A used-defined array of integer variables
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVarArrayF
// Purpose:        Base class for an array of float variables
// Description:    Defines an array of integer variables and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbVarArrayF: public TMrbVariable {

	public:
		TMrbVarArrayF() {};										// default ctor
		TMrbVarArrayF(const Char_t * Name, Int_t Size, Double_t Value = 0.);	// explicit ctor
 
		~TMrbVarArrayF() {};									// dtor

		inline Double_t Clip(Double_t Value) const {				// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		inline Double_t & operator[](UInt_t i) { return(fValue[i]); };

		inline void Set(UInt_t Index, Double_t Value) { fValue[Index] = Clip(Value); };	// set new value
		inline void SetInit(UInt_t Index, Double_t Value) { fInitValue[Index] = Clip(Value); };	// set init val

		inline Double_t Get(UInt_t Index) const { return(fValue[Index]); };

		inline Bool_t IsEQ(UInt_t Index, Double_t X) const { return(fValue[Index] == X); };	// compare vlues
		inline Bool_t IsNE(UInt_t Index, Double_t X) const { return(fValue[Index] != X); };
		inline Bool_t IsLT(UInt_t Index, Double_t X) const { return(fValue[Index] < X); };
		inline Bool_t IsLE(UInt_t Index, Double_t X) const { return(fValue[Index] <= X); };
		inline Bool_t IsGT(UInt_t Index, Double_t X) const { return(fValue[Index] > X); };
		inline Bool_t IsGE(UInt_t Index, Double_t X) const { return(fValue[Index] >= X); };

		void SetRange(Double_t Xlower, Double_t Xupper) {		// define range
			SetUniqueID(GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		void Initialize() { for (Int_t i = 0; i < fSize; i++) fValue[i] = fInitValue[i]; };

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		TArrayD fValue;							// current values
		TArrayD fInitValue; 					// initial values
		Double_t fLowerRange;					// lower range limit
		Double_t fUpperRange;					// upper range limit

	ClassDef(TMrbVarArrayF, 1)		// [Utils] A used-defined array double/float variables
};

#endif

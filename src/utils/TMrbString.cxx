//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbString.cxx
// Purpose:        MARaBOU utilities: type conversion for strings
// Description:    Implements type conversion for TString objects
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbString.cxx,v 1.22 2011-05-18 11:04:49 Marabou Exp $
// Date:           $Date: 2011-05-18 11:04:49 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TObjString.h"
#include "TObjArray.h"

#include "TMrbString.h"
#include "TMrbLogger.h"
#include "SetColor.h"

ClassImp(TMrbString)

extern TMrbLogger * gMrbLog;			// access to message logger

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString
// Purpose:        Provide type conversion
// Description:    Type conversion for strings
//                 Based on original TString class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

TMrbString::TMrbString() {
	fBase = TMrbString::kDefaultBase;
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

TMrbString::TMrbString(const Char_t * Str) : TString(Str) {
	fBase = TMrbString::kDefaultBase;
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

TMrbString & TMrbString::FromInteger(Int_t IntVal,
							Int_t Width, Int_t Base, Bool_t PadZero, Bool_t AddBasePrefix, Bool_t LowerCaseHex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromInteger
// Purpose:        Convert integer to string
// Arguments:      Int_t IntVal          -- integer value
//                 Int_t Width           -- number of digits
//                 Int_t Base            -- conversion base (2, 8, 10, 16)
//                 Bool_t PadZero        -- pad leading blanks with zero
//                 Bool_t AddBasePrefix  -- prepend base identifier (0b, 0, 0x) if kTRUE
//                 Bool_t LowerCaseHex   -- use lc chars for hex representation
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t hex, prf;
	Char_t f[100], s[100];

	prf = AddBasePrefix ? '#' : ' ';
	hex = LowerCaseHex ? 'x' : 'X';

	this->Resize(0);

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "FromInteger")) return(*this);

	if (Base == 2) {
		while (IntVal) { this->Prepend((IntVal & 1) ? '1' : '0'); IntVal >>= 1; }
		if (AddBasePrefix) this->Prepend("0b");
	} else {
		switch (Base) {
			case 8:
				if (PadZero) {
					if (Width > 0)	{
						if (IntVal == 0 && prf == '#') {
							Width--;
							sprintf(f, "0%%0%do", Width);
						} else {
							sprintf(f, "%%%c0%do", prf, Width);
						}
					} else {
						if (IntVal == 0 && prf == '#') {
							sprintf(f, "0%%o");
						} else {
							sprintf(f, "%%%co", prf);
						}
					}
				} else {
					if (Width > 0)	{
						if (IntVal == 0 && prf == '#') {
							Width--;
							sprintf(f, "0%%o");
						} else {
							sprintf(f, "%%%c%do", prf, Width);
						}
					} else {
						if (IntVal == 0 && prf == '#') {
							sprintf(f, "0%%o");
						} else {
							sprintf(f, "%%%co", prf);
						}
					}
				}
				break;

			case 16:
				if (PadZero) {
					if (Width > 0)	{
						if (IntVal == 0 && prf == '#') {
							Width -= 2;
							sprintf(f, "0%c%%0%d%c", hex, Width, hex);
						} else {
							sprintf(f, "%%%c0%d%c", prf, Width, hex);
						}
					} else {
						if (IntVal == 0 && prf == '#') {
							sprintf(f, "0%c%%%c", hex, hex);
						} else {
							sprintf(f, "%%%c%c", prf, hex);
						}
					}
				} else {
					if (Width > 0)	{
						if (IntVal == 0 && prf == '#') {
							Width -= 2;
							sprintf(f, "0%c%%%c", hex, hex);
						} else {
							sprintf(f, "%%%c%d%c", prf, Width, hex);
						}
					} else {
						if (IntVal == 0 && prf == '#') {
							sprintf(f, "0%c%%%c", hex, hex);
						} else {
							sprintf(f, "%%%c%c", prf, hex);
						}
					}
				}
				break;

			case 10:
				if (PadZero) {
					if (Width > 0)	sprintf(f, "%%0%dd", Width);
					else			sprintf(f, "%%d");
				} else {
					if (Width > 0)	sprintf(f, "%%%dd", Width);
					else			sprintf(f, "%%d");
				}
				break;
		}
		sprintf(s, f, IntVal);
		this->Resize(0);
		this->Insert(0, s);
	}
	return(*this);
}

TMrbString & TMrbString::AppendInteger(Int_t IntVal,
								Int_t Width, Int_t Base, Bool_t PadZero, Bool_t AddBasePrefix, Bool_t LowerCaseHex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendInteger
// Purpose:        Append integer to string
// Arguments:      Int_t IntVal          -- integer value
//                 Int_t Width           -- number of digits
//                 Int_t Base            -- conversion base (2, 8, 10, 16)
//                 Bool_t PadZero        -- pad leading blanks with zero
//                 Bool_t AddBasePrefix  -- add base prefix if kTRUEs
//                 Bool_t LowerCaseHex   -- use lc chars for hex representation
// Results:        --
// Exceptions:
// Description:    Appends an integer value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "AppendInteger")) return(*this);

	TMrbString s(IntVal, Width, Base, PadZero, AddBasePrefix, LowerCaseHex);
	this->Append(s.Data());
	return(*this);
}

Bool_t TMrbString::ToInteger(Int_t & IntVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ToInteger
// Purpose:        Convert string to integer
// Arguments:      Int_t & IntVal   -- integer representation
//                 Int_t Base       -- force conversion to given base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Converts a string to its integer value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	IntVal = 0;

	if (!this->CheckBase(Base, "ToInteger")) return(kFALSE);

	Int_t idx = this->Index("0b", 0);
	if (idx != -1) {
		Base = 2;
		idx += 2;
	} else {
		idx = 0;
	}

	IntVal = (Int_t) strtoul(this->Data() + idx, &endptr, Base);

	if (*endptr != '\0') {
		gMrbLog->Err() << "Not a legal integer - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "ToInteger");
		IntVal = 0;
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbString::SplitOffInteger(TString & Prefix, Int_t & IntVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffInteger
// Purpose:        Return trailing integer
// Arguments:      TString & Prefix    -- leading substring
//                 Int_t IntVal        -- trailing integer part
//                 Int_t Base          -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "SplitOffInteger")) return(kFALSE);

	Int_t idx = this->CheckInteger(Base);
	if (idx == -1) {
		gMrbLog->Err() << "No trailing integer (base " << Base << ") - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "SplitOffInteger");
		Prefix = this->Data();
		return(kFALSE);
	}

	TMrbString s(this->Data() + idx);
	if (!s.ToInteger(intVal, Base)) {
		Prefix = this->Data();
		return(kFALSE);
	}

	Prefix = (*this)(0, idx);
	IntVal = intVal;
	return(kTRUE);
}

Bool_t TMrbString::Increment(Int_t Increment, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing integer
// Arguments:      Int_t Increment     -- increment
//                 Int_t Base          -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Increments the trailing integer of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;
	Bool_t basePrefix = kFALSE;

	if (Base == 0) Base = fBase;
	if (!this->CheckBase(Base, "Increment")) return(-1);

	if (!this->ToInteger(intVal, Base)) return(kFALSE);
	intVal += Increment;
	switch (Base) {
		case 2: basePrefix = (this->Index("0b", 0) == 0); break;
		case 8: basePrefix = (this->Index("0", 0) == 0); break;
		case 10: basePrefix = kFALSE; break;
		case 16: basePrefix = (this->Index("0x", 0) == 0); break;
	}
	return(this->FromInteger(intVal, 0, ' ', Base, basePrefix));
}

TMrbString & TMrbString::FromDouble(Double_t DblVal, Int_t Width, Int_t Precision, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::FromDouble
// Purpose:        Convert double to string
// Arguments:      Int_t DblVal     -- double value
//                 Int_t Width      -- number of digits
//                 Int_t Precision  -- precision
//                 Bool_t PadZero   -- pad leading blanks with zero
// Results:        --
// Exceptions:
// Description:    Converts an integer value to its ascii representation
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t f[100], s[100];

	if (PadZero) {
		if (Width > 0)	sprintf(f, "%%0%d.%df", Width, Precision);
		else			sprintf(f, "%%.%df", Precision);
	} else {
		if (Width > 0)	sprintf(f, "%%%d.%df", Width, Precision);
		else			sprintf(f, "%%.%df", Precision);
	}
	sprintf(s, f, DblVal);
	this->Resize(0);
	this->Insert(0, s);
    return(*this);
}

TMrbString & TMrbString::AppendDouble(Double_t DblVal, Int_t Width, Int_t Precision, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::AppendDouble
// Purpose:        Append double to string
// Arguments:      Double_t DblVal    -- integer value
//                 Int_t Width        -- number of digits
//                 Int_t Precision    -- precision
//                 Bool_t PadZero   -- pad leading blanks with zero
// Results:        --
// Exceptions:
// Description:    Appends a double value to an existing string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString s(DblVal, Width, Precision, PadZero);
	this->Append(s.Data());
	return(*this);
}

Bool_t TMrbString::ToDouble(Double_t & DblVal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ToDouble
// Purpose:        Convert string to double
// Arguments:      Double_t & DblVal   -- double representation
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Converts a string to its double value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	DblVal = 0;

	Int_t idx = this->CheckDouble();
	if (idx == -1) {
		gMrbLog->Err() << "Not a legal double - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		return(kFALSE);
	}

	TString pf = (*this)(0, idx);
	pf = pf.Strip(TString::kBoth);

	if (idx == -1 || pf.Length() != 0) {
		gMrbLog->Err() << "Not a legal double - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		DblVal = 0.;
		return(kFALSE);
	}

	DblVal = strtod(this->Data() + idx, &endptr);

	if (*endptr != '\0') {
		gMrbLog->Err() << "Not a legal double - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "ToDouble");
		DblVal = 0.;
		return(kFALSE);
	}
 	return(kTRUE);
}

Bool_t TMrbString::SplitOffDouble(TString & Prefix, Double_t DblVal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::SplitOffDouble
// Purpose:        Return trailing double
// Arguments:      TString & Prefix    -- leading substring
//                 Double_t DblVal     -- trailing double part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inspects a string for trailing digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	Double_t dblVal;

	idx = this->CheckDouble();
	if (idx == -1) {
		gMrbLog->Err() << "No trailing double - " << this->Data() << endl;
		gMrbLog->Flush("TMrbString", "SplitOffDouble");
		Prefix = this->Data();
		return(kFALSE);
	}

	TMrbString s = this->Data() + idx;
	if (!s.ToDouble(dblVal)) {
		Prefix = this->Data();
		return(kFALSE);
	}

	Prefix = (*this)(0, idx);
	DblVal = dblVal;
	return(kTRUE);
}

Bool_t TMrbString::Increment(Double_t Increment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Increment
// Purpose:        Increment trailing double
// Arguments:      Double_t Increment     -- increment
// Results:        Double_t IntVal        -- double value
// Exceptions:
// Description:    Increments the trailing double of a string (if present).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pf;
	Double_t dblVal;

	if (!this->ToDouble(dblVal)) return(kFALSE);
	dblVal += Increment;
	return(this->FromDouble(dblVal));
}

Int_t TMrbString::Split(TObjArray & LofSubStrings, const Char_t * Separator, Bool_t RemoveWhiteSpace) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Split
// Purpose:        Split string into substrings
// Arguments:      TObjArray & LofSubStrings   -- list of substrings
//                 Char_t * Separator          -- separator between substrings
// Results:        Int_t NofSubStrings         -- number of substrings
// Exceptions:
// Description:    Appends substrings to list.
//                 Substrings have to be converted to TObjString to get stored.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString sepStr;
	Int_t nstr, sepl, lng;
	TString subStr;
	TMrbString tmpStr;

	sepStr = Separator;
	sepl = sepStr.Length();

	Char_t whiteSpace = (sepStr.CompareTo("\t") == 0) ? '\t' : ' ';

	if (RemoveWhiteSpace) {
		tmpStr = this->Copy();
		tmpStr.ReplaceWhiteSpace(whiteSpace);
	} else {
		tmpStr = this->Data();
	}

	tmpStr = tmpStr.Strip(TString::kBoth);
	if (tmpStr.IsNull()) return(0);

	n1 = 0;
	nstr = 0;
	for (;;) {
		n2 = tmpStr.Index(sepStr.Data(), n1);
		lng = (n2 == -1) ? tmpStr.Length() - n1 : n2 - n1;
		subStr = tmpStr(n1, lng);
		if (RemoveWhiteSpace) subStr = subStr.Strip(TString::kBoth, whiteSpace);
		if (subStr.IsNull()) {
			LofSubStrings.Add(new TObjString(" "));
			nstr++;
		} else {
			LofSubStrings.Add(new TObjString(subStr.Data()));
			nstr++;
		}
		if (n2 == -1) break;
		n1 = n2 + sepl;
	}
	return(nstr);
}

Int_t TMrbString::ReplaceWhiteSpace(const Char_t WhiteSpace) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ReplaceWhiteSpace
// Purpose:        Replace white spaces
// Arguments:      Char_t WhiteSpace    -- white space to be used as replacement
// Results:        Int_t NofChars       -- resulting length
// Exceptions:
// Description:    Replaces multiple white spaces (blanks or tabs) with given character
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t out;
	Bool_t wasWhiteSpace;
	Char_t c;

	out = 0;
	wasWhiteSpace = kFALSE;
	for (Int_t i = 0; i < this->Length(); i++) {
		c = (*this)(i);
		if (c == ' ' || c == '\t') {
			if (!wasWhiteSpace) {
				wasWhiteSpace = kTRUE;
				(*this)(out++) = WhiteSpace;
			}
		} else {
			wasWhiteSpace = kFALSE;
			(*this)(out++) = c;
		}
	}
	this->Resize(out);
	return(out);
}


void TMrbString::Expand(const Char_t Escape) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Expand
// Purpose:        Expand escape sequences
// Arguments:      Char_t Escape    -- escape char
// Results:        --
// Exceptions:
// Description:    Looks for given escape char
//                 and calls method "ProcessEscapeSequence".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString esc;
	TString replStr;

	esc = Escape;

	n1 = 0;
	for (;;) {
		n2 = this->Index(esc, n1);
		if (n2 == -1) break;
		if (n2 == this->Length() - 1) {
			this->Replace(n2, 1, "");
			break;
		}
		this->ProcessEscapeSequence(*(this->Data() + n2 + 1), replStr);
		this->Replace(n2, 2, replStr);
		n1 = n2 + 1;
	}
}

void TMrbString::ProcessEscapeSequence(Char_t /*CharId*/, TString & Replacement) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::ProcessEscapeSequence
// Purpose:        Expand escape sequences
// Arguments:      Char_t CharId          -- escape sequence char
//                 TString & Replacement  -- replacement
// Results:        --
// Exceptions:
// Description:    User's method to replace escape sequences.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Replacement.Resize(0);		// nullify
}

Bool_t TMrbString::Encode(UInt_t & IntVal, TMrbLofNamedX * LofSubStrings, const Char_t * Separator, UInt_t Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert option string to number
// Arguments:      TMrbLofNamedX * LofSubStrings   -- possible substrings
//                 Char_t * Separator              -- separator
//                 UInt_t Mode                     -- how to identify substrings
// Results:        UInt_t & IntVal                 -- result
//                 kTRUE/kFALSE
// Exceptions:
// Description:    Encodes string to a number according to definitions in
//                 LofSubstrings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	IntVal = strtoul(this->Data(), &endptr, 0);
	if (*endptr == '\0') return(kTRUE);

	IntVal = 0;
	Bool_t isOk = kTRUE;
	TString subs;
	Int_t from = 0;
	while (this->Tokenize(subs, from, Separator)) {
		TMrbNamedX * nx = LofSubStrings->FindByName(subs.Data(), Mode);
		if (nx == NULL) {
			isOk = kFALSE;
		} else {
			IntVal |= nx->GetIndex();
		}
	}
	return(isOk);
}

Bool_t TMrbString::Encode(UInt_t & IntVal, SMrbNamedXShort * LofSubStrings, const Char_t * Separator, UInt_t Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert option string to number
// Arguments:      SMrbNamedXShort * LofSubStrings -- possible substrings
//                 Char_t * Separator              -- separator
//                 UInt_t Mode                     -- how to identify substrings
// Results:        UInt_t & IntVal                 -- result
//                 kTRUE/kFALSE
// Exceptions:
// Description:    Encodes string to a number according to definitions in
//                 LofSubstrings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	IntVal = strtoul(this->Data(), &endptr, 0);
	if (*endptr == '\0') return(kTRUE);

	TMrbLofNamedX lofSubStrings;
	lofSubStrings.AddNamedX(LofSubStrings);
	return(this->Encode(IntVal, &lofSubStrings, Separator, Mode));
}

Bool_t TMrbString::Encode(UInt_t & IntVal, const Char_t * LofSubStrings, const Char_t * Separator, UInt_t Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert option string to number
// Arguments:      Char_t * LofSubStrings          -- possible substrings
//                 Char_t * Separator              -- separator
//                 UInt_t Mode                     -- how to identify substrings
// Results:        UInt_t & IntVal                 -- result
//                 kTRUE/kFALSE
// Exceptions:
// Description:    Encodes string to a number according to definitions in
//                 LofSubstrings.
//                 Assigns BIT(0) to substring #1, BIT(1) to #2, and so on.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr;

	IntVal = strtoul(this->Data(), &endptr, 0);
	if (*endptr == '\0') return(kTRUE);

	TMrbLofNamedX lofSubStrings;
	lofSubStrings.AddNamedX(LofSubStrings, Separator, kTRUE);
	return(this->Encode(IntVal, &lofSubStrings, Separator, Mode));
}

Bool_t TMrbString::Decode(UInt_t IntVal, TMrbLofNamedX * LofSubStrings, const Char_t * /*Separator*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert number to option string
// Arguments:      UInt_t IntVal                   -- number to be decoded
//                 TMrbLofNamedX * LofSubStrings   -- possible substrings
//                 Char_t * Separator              -- separator
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes a number to an option string according to definitions in
//                 LofSubstrings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t isOk = kTRUE;
	this->Resize(0);
	TMrbNamedX * nx = (TMrbNamedX *) LofSubStrings->First();
	Bool_t first = kTRUE;
	while (nx) {
		if (IntVal & nx->GetIndex()) {
			if (!first) this->Append(":");
			this->Append(nx->GetName());
			first = kFALSE;
		}
		nx = (TMrbNamedX *) LofSubStrings->After(nx);
	}
	return(isOk);
}

Bool_t TMrbString::Decode(UInt_t IntVal, SMrbNamedXShort * LofSubStrings, const Char_t * Separator) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert number to option string
// Arguments:      UInt_t IntVal                   -- number to be decoded
//                 SMrbNamedXShort * LofSubStrings -- possible substrings
//                 Char_t * Separator              -- separator
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes a number to an option string according to definitions in
//                 LofSubstrings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX lofSubStrings;
	lofSubStrings.AddNamedX(LofSubStrings);
	return(this->Decode(IntVal, &lofSubStrings, Separator));
}

Bool_t TMrbString::Decode(UInt_t IntVal, const Char_t * LofSubStrings, const Char_t * Separator) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::Encode
// Purpose:        Convert number to option string
// Arguments:      UInt_t IntVal                   -- number to be decoded
//                 Char_t * LofSubStrings          -- possible substrings
//                 Char_t * Separator              -- separator
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes a number to an option string according to definitions in
//                 LofSubstrings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX lofSubStrings;
	lofSubStrings.AddNamedX(LofSubStrings, Separator, kTRUE);
	return(this->Decode(IntVal, &lofSubStrings, Separator));
}

Int_t TMrbString::CheckInteger(Int_t Base) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckInteger
// Purpose:        Check if string is valid integer
// Arguments:      Int_t Base             -- expected base
// Results:        Int_t Index            -- where integer number starts
// Exceptions:     returns -1 on error
// Description:    Test if string chars are legal integer digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = -1;

	if (this->Length() > 0) {
		switch (Base) {
			case 2:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = *(this->Data() + i);
					if (code == '0' && code == '1') idx = i; else break;
				}
				break;
			case 8:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = *(this->Data() + i);
					if (code == '-') { idx = i; break; }
					else if (code >= '0' && code <= '7') idx = i;
					else break;
				}
				break;
			case 10:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = *(this->Data() + i);
					if (code == '-') { idx = i; break; }
					else if (code >= '0' && code <= '9') idx = i;
					else break;
				}
				break;
			case 16:
				for (Int_t i = this->Length() - 1; i >= 0; i--) {
					Char_t code = *(this->Data() + i);
					if (code == '-') { idx = i; break; }
					else if (	(code >= '0' && code <= '9')
							||	(code >= 'a' && code <= 'f')
							||	(code >= 'A' && code <= 'F')) idx = i;
					else break;
				}
				break;
			default:
				gMrbLog->Err() << "Illegal base spec - " << Base << endl;
				gMrbLog->Flush("TMrbString", "CheckInteger");
				break;
		}
	}
	return(idx);
}

Int_t TMrbString::CheckDouble() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckDouble
// Purpose:        Check if string is valid integer
// Arguments:      --
// Results:        Int_t Index            -- where double number starts
// Exceptions:     returns -1 on error
// Description:    Test if string chars are legal double digits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx = -1;

	Int_t lng = this->Length();

	if (lng > 0) {
		Int_t n;
		if ((n = this->Index("e-", 0)) > 0) lng = n;
		else if ((n = this->Index("e+", 0)) > 0) lng = n;

		Bool_t dot = kFALSE;
		for (Int_t i = lng - 1; i >= 0; i--) {
			Char_t code = *(this->Data() + i);
			if (code == '-') { idx = i; break; }
			if (code >= '0' && code <= '9')	{
				idx = i;
			} else if (code == '.') {
				if (dot) break;
				dot = kTRUE;
				idx = i;
			} else break;
		}
	}
	return(idx);
}

Bool_t TMrbString::CheckBase(Int_t Base, Char_t * Method) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbString::CheckBase
// Purpose:        Check if numerical base is valid
// Arguments:      Int_t Base             -- base value ([0], 2, 8, 10, 16)
//                 Char_t * Method        -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Test if numerical base value is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Base) {
		case 0:
		case 2:
		case 8:
		case 10:
		case 16:	return(kTRUE);
	}
	gMrbLog->Err() << "Illegal base spec - " << Base << endl;
	gMrbLog->Flush("TMrbString", Method);
	return(kFALSE);
}


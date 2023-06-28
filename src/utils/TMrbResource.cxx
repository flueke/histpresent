//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbResource.cxx
// Purpose:        Handle resource files
// Description:    Implements class methods to handle resource files
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbResource.cxx,v 1.7 2010-12-14 14:18:04 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TSystem.h"
#include "TObjString.h"
#include "TIterator.h"

#include "TMrbResource.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbResource)

extern TMrbLogger * gMrbLog;					// MARaBOU's message logger

TMrbResource::TMrbResource(const Char_t * Prefix, const Char_t * ResourceFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource
// Purpose:        Access to ROOT environment
// Arguments:      Char_t * Prefix         -- prefix
//                 Char_t * ResourceFile   -- ROOT env file
// Results:        --
// Exceptions:
// Description:    Class constructorConvert
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (gSystem->AccessPathName(ResourceFile)) {
		gMrbLog->Wrn() << gSystem->GetError() << " - " << ResourceFile << endl;
		gMrbLog->Flush(this->ClassName());
	}
	fEnv = new TEnv(ResourceFile);
	fRcName = ResourceFile;
	fLofPrefixes.Delete();
	fLastPrefix = "";
	fVerbose = gEnv->GetValue("TMrbResource.VerboseMode", kFALSE);
	this->AddPrefix(Prefix);
}

TMrbResource::TMrbResource(const Char_t * Prefix, TEnv * Environment) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource
// Purpose:        Access to ROOT environment
// Arguments:      Char_t * Prefix         -- prefix
//                 TEnv * Environment      -- ROOT env
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (Environment == NULL) Environment = gEnv;
	fEnv = Environment;
	fRcName = fEnv->GetRcName();
	fLofPrefixes.Delete();
	fLastPrefix = "";
	fVerbose = gEnv->GetValue("TMrbResource.VerboseMode", kFALSE);
	this->AddPrefix(Prefix);
}

void TMrbResource::AddPrefix(const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::AddPrefix()
// Purpose:        Add prefix to list
// Arguments:      Char_t * Prefix   -- prefix or :-separated list of prefs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds one or more prefixes to list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString prefStr = Prefix;
	Int_t from = 0;
	TString prefix;
	while (prefStr.Tokenize(prefix, from, ":")) fLofPrefixes.Add(new TObjString(prefix.Data()));
}

const Char_t * TMrbResource::Get(TString & Result, const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      TString & Result   -- result
//                 Char_t * Res1      -- resource 1
//                 Char_t * Res2      -- [opt] resource 2
//                 Char_t * Res3      -- [opt] resource 3
//                 Char_t * Default   -- default value
// Results:        Char_t * Result    -- result
// Exceptions:
// Description:    Reads an env value of type string
//                 Full resource name will be built from
//                 <Prefix>.<Res1>.<Res2>.<Res3>
//                 Example: "DGFControl.Module.DGF11.Crate"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	Result = Default;
	if (r) this->ToString(r, Result); else r = Form("%s.%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"), (Res3 ? Res3 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << Result << "]" << endl;
	return(Result.Data());
}

const Char_t * TMrbResource::Get(TString & Result, const Char_t * Res1, const Char_t * Res2, const Char_t * Default) {

	const Char_t * r = this->Find(Res1, Res2);
	Result = Default;
	if (r) this->ToString(r, Result); else r = Form("%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << Result << "]" << endl;
	return(Result.Data());
}

const Char_t * TMrbResource::Get(TString & Result, const Char_t * Res, const Char_t * Default) {

	const Char_t * r = this->Find(Res);
	Result = Default;
	if (r) this->ToString(r, Result); else r = (Res ? Res : "?");
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << Result << "]" << endl;
	return(Result.Data());
}

Int_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1      -- resource 1
//                 Char_t * Res2      -- [opt] resource 2
//                 Char_t * Res3      -- [opt] resource 3
//                 Int_t Default      -- default value
// Results:        Int_t * Result     -- result
// Exceptions:
// Description:    Reads an env value of type integer
//                 Full resource name will be built from
//                 <Prefix>.<Res1>.<Res2>.<Res3>
//                 Example: "DGFControl.Module.DGF11.Crate"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	Int_t n = Default;
	if (r) this->ToInteger(r, n); else r = Form("%s.%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"), (Res3 ? Res3 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << n << "]" << endl;
	return(n);
}

Int_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, Int_t Default) {

	const Char_t * r = this->Find(Res1, Res2);
	Int_t n = Default;
	if (r) this->ToInteger(r, n); else r = Form("%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << n << "]" << endl;
	return(n);
}

Int_t TMrbResource::Get(const Char_t * Res, Int_t Default) {

	const Char_t * r = this->Find(Res);
	Int_t n = Default;
	if (r) this->ToInteger(r, n); else r = (Res ? Res : "?");
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << n << "]" << endl;
	return(n);
}

Bool_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1      -- resource 1
//                 Char_t * Res2      -- [opt] resource 2
//                 Char_t * Res3      -- [opt] resource 3
//                 Bool_t Default     -- default value
// Results:        Bool_t Result      -- result
// Exceptions:
// Description:    Reads an env value of type boolean
//                 Full resource name will be built from
//                 <Prefix>.<Res1>.<Res2>.<Res3>
//                 Example: "DGFControl.Module.DGF11.Crate"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	Bool_t result = r ? fEnv->GetValue(r, Default) : Default;
	if (r == NULL) r = Form("%s.%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"), (Res3 ? Res3 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << (result ? "TRUE" : "FALSE") << "]" << endl;
	return(result);
}

Bool_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, Bool_t Default) {

	const Char_t * r = this->Find(Res1, Res2);
	Bool_t result = r ? fEnv->GetValue(r, Default) : Default;
	if (r == NULL) r = Form("%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"));
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << (result ? "TRUE" : "FALSE") << "]" << endl;
	return(result);
}

Bool_t TMrbResource::Get(const Char_t * Res, Bool_t Default) {

	const Char_t * r = this->Find(Res);
	Bool_t result = r ? fEnv->GetValue(r, Default) : Default;
	if (r == NULL) r = (Res ? Res : "?");
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << (result ? "TRUE" : "FALSE") << "]" << endl;
	return(result);
}

Double_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Double_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1      -- resource 1
//                 Char_t * Res2      -- [opt] resource 2
//                 Char_t * Res3      -- [opt] resource 3
//                 Double_t Default   -- default value
// Results:        Double_t Result    -- result
// Exceptions:
// Description:    Reads an env value of type double
//                 Full resource name will be built from
//                 <Prefix>.<Res1>.<Res2>.<Res3>
//                 Example: "DGFControl.Module.DGF11.Crate"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * r = this->Find(Res1, Res2, Res3);
	Double_t d = Default;
	if (r) {
		Char_t * endptr;
		d = strtod((Char_t *) r, &endptr);
		if (*endptr != '\0') d = Default;
	} else {
		r = Form("%s.%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"), (Res3 ? Res3 : "?"));
	}
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << d << "]" << endl;
	return(d);
}

Double_t TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, Double_t Default) {

	const Char_t * r = this->Find(Res1, Res2);
	Double_t d = Default;
	if (r) {
		Char_t * endptr;
		d = strtod((Char_t *) r, &endptr);
		if (*endptr != '\0') d = Default;
	} else {
		r = Form("%s.%s", (Res1 ? Res1 : "?"), (Res2 ? Res2 : "?"));
	}
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << d << "]" << endl;
	return(d);
}

Double_t TMrbResource::Get(const Char_t * Res, Double_t Default) {

	const Char_t * r = this->Find(Res);
	Double_t d = Default;
	if (r) {
		Char_t * endptr;
		d = strtod((Char_t *) r, &endptr);
		if (*endptr != '\0') d = Default;
	} else {
		r = (Res ? Res : "?");
	}
	if (this->IsVerbose()) cout << "[" << this->ClassName() << "::Get(): " << r << "=" << d << "]" << endl;
	return(d);
}

TMrbNamedX * TMrbResource::Get(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbLofNamedX * List) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Get()
// Purpose:        Get a value from environment
// Arguments:      Char_t * Res1         -- resource 1
//                 Char_t * Res2         -- [opt] resource 2
//                 Char_t * Res3         -- [opt] resource 3
//                 TMrbLofNamedX * List  -- list of pairs N,X
// Results:        TMrbNamedX * Nx       -- result
// Exceptions:
// Description:    Reads an env value and searches for it in list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t intRes;
	TString strRes;
	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) return(NULL);
	if (!this->Convert(res, strRes, intRes)) return(NULL);
	TMrbNamedX * nx;
	if (intRes != (Int_t) 0xaffec0c0) nx = List->FindByIndex(intRes); else nx = List->FindByName(strRes);
	if (nx == NULL) return(NULL);
	if (intRes != (Int_t) 0xaffec0c0 && !strRes.IsNull()) {
		strRes.ToLower();
		TString x = nx->GetName();
		x.ToLower();
		if (!x.BeginsWith(strRes.Data())) {
			gMrbLog->Err()	<< "Mismatch - env=" << intRes << "/" << strRes
							<< " <--> list=" << nx->GetIndex() << "/" << nx->GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "Get");
			return(NULL);
		}
	}
	return(nx);
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, const Char_t * TrueFalse) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Set()
// Purpose:        Set (local) environment
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- [opt] resource part 2
//                 Char_t * Res3      -- [opt] resource part 3
//                 Char_t * TrueFalse -- TRUE or FALSE
// Results:        --
// Exceptions:
// Description:    Sets the local environment: <Prefix>.<Res1>.<Res2>.<Res3>: <TrueFalse>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
		if (Res3) { res += "."; res += Res3; }
	}
	fEnv->SetValue(res, TrueFalse);
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * TrueFalse) {
	TString res = this->Find(Res1, Res2);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
	}
	fEnv->SetValue(res, TrueFalse);
}

void TMrbResource::Set(const Char_t * Res, const Char_t * TrueFalse) {
	TString res = this->Find(Res);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res) { res += "."; res += Res; }
	}
	fEnv->SetValue(res, TrueFalse);
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Bool_t TrueFalse) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Set()
// Purpose:        Set (local) environment
// Arguments:      Char_t * Res1    -- resource part 1
//                 Char_t * Res2    -- resource part 2
//                 Char_t * Res3    -- resource part 3
//                 Bool_t TrueFalse -- TRUE or FALSE
// Results:        --
// Exceptions:
// Description:    Sets the local environment: <Prefix>.<Res1>.<Res2>.<Res3>: <TrueFalse>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
		if (Res3) { res += "."; res += Res3; }
	}
	fEnv->SetValue(res, TrueFalse ? "TRUE" : "FALSE");
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, Bool_t TrueFalse) {
	TString res = this->Find(Res1, Res2);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
	}
	fEnv->SetValue(res, TrueFalse ? "TRUE" : "FALSE");
}

void TMrbResource::Set(const Char_t * Res, Bool_t TrueFalse) {
	TString res = this->Find(Res);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res) { res += "."; res += Res; }
	}
	fEnv->SetValue(res, TrueFalse ? "TRUE" : "FALSE");
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Int_t IntVal, const Char_t * StrVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Set()
// Purpose:        Set (local) environment
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- resource part 2
//                 Char_t * Res3      -- resource part 3
//                 Int_t IntVal       -- resource value (integer)
//                 Int_t StrVal       -- ... (string)
//                 Int_t Base         -- numerical base
// Results:        --
// Exceptions:
// Description:    Sets the local environment: <Prefix>.<Res1>.<Res2>.<Res3>: <IntVal>(<StrVal>)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
		if (Res3) { res += "."; res += Res3; }
	}
	TString intVal;
	switch (Base) {
		case 8: 	intVal = Form("%#o", IntVal); break;
		case 16:	intVal = Form("%#x", IntVal); break;
		default:	intVal = Form("%d", IntVal); break;
	}
	if (StrVal != NULL && *StrVal != '\0') {
		fEnv->SetValue(res, Form("%s (%s)", intVal.Data(), StrVal));
	} else {
		fEnv->SetValue(res, intVal);
	}
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, Int_t IntVal, const Char_t * StrVal, Int_t Base) {
	TString res = this->Find(Res1, Res2);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
	}
	this->SetIntVal(res, IntVal, StrVal, Base);
}

void TMrbResource::Set(const Char_t * Res, Int_t IntVal, const Char_t * StrVal, Int_t Base) {
	TString res = this->Find(Res);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res) { res += "."; res += Res; }
	}
	this->SetIntVal(res, IntVal, StrVal, Base);
}

void TMrbResource::SetIntVal(const Char_t * Res, Int_t IntVal, const Char_t * StrVal, Int_t Base) {
	TString intVal;
	switch (Base) {
		case 8: 	intVal = Form("%#o", IntVal); break;
		case 16:	intVal = Form("%#x", IntVal); break;
		default:	intVal = Form("%d", IntVal); break;
	}
	if (StrVal != NULL && *StrVal != '\0') {
		fEnv->SetValue(Res, Form("%s (%s)", intVal.Data(), StrVal));
	} else {
		fEnv->SetValue(Res, intVal);
	}
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, Double_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Set()
// Purpose:        Set (local) environment
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- resource part 2
//                 Char_t * Res3      -- resource part 3
//                 Double_t Value     -- resource value
// Results:        --
// Exceptions:
// Description:    Sets the local environment: <Prefix>.<Res1>.<Res2>.<Res3>: <Value>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
		if (Res3) { res += "."; res += Res3; }
	}
	fEnv->SetValue(res, Value);
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, Double_t Value) {
	TString res = this->Find(Res1, Res2);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
	}
	fEnv->SetValue(res, Value);
}

void TMrbResource::Set(const Char_t * Res, Double_t Value) {
	TString res = this->Find(Res);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res) { res += "."; res += Res; }
	}
	fEnv->SetValue(res, Value);
}

void TMrbResource::Set(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3, TMrbNamedX * Nx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Set()
// Purpose:        Set (local) environment
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- resource part 2
//                 Char_t * Res3      -- resource part 3
//                 TMrbNamedX * Nx    -- resource value (int & str)
// Results:        --
// Exceptions:
// Description:    Sets the local environment: <Prefix>.<Res1>.<Res2>.<Res3>: <IntVal>(<StrVal>)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res = this->Find(Res1, Res2, Res3);
	if (res.IsNull()) {
		res = ((TObjString *) fLofPrefixes[0])->GetString();
		if (Res1) { res += "."; res += Res1; }
		if (Res2) { res += "."; res += Res2; }
		if (Res3) { res += "."; res += Res3; }
	}
	fEnv->SetValue(res, Form("%d (%s)", Nx->GetIndex(), Nx->GetName()));
}

const Char_t * TMrbResource::Find(const Char_t * Res1, const Char_t * Res2, const Char_t * Res3) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Find()
// Purpose:        Check if resource present
// Arguments:      Char_t * Res1      -- resource part 1
//                 Char_t * Res2      -- resource part 2
//                 Char_t * Res3      -- resource part 3
// Results:        Char_t * Resource  -- resulting resource
// Exceptions:
// Description:    Checks if environment contains a resource
//                 [<Prefix1>.]<Res1>.<Res2>.<Res3>
//                 If <Res1> starts with a dot <Prefix> will be omitted.
//                 Otherwise <Prefix1> will be taken from list of prefixes.
//                 <Res2> (containing the item name or number) may be multi-valued,
//                 separated by ":".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString res2 = Res2;
	if (!res2.IsNull() && res2.Contains(":")) {
		Int_t from = 0;
		TString r;
		while (res2.Tokenize(r, from, ":")) {
			const Char_t * rp = this->Find(Res1, r.Data(), Res3);
			if (rp) return(rp);
		}
		return(NULL);
	}

	TString r = "";
	Bool_t withDot = (Res1 && *Res1 == '.');
	if (withDot) r += Res1; else if (Res1) r += Form(".%s", Res1);
	if (Res2 && *Res2 != '\0') { if (*Res2 != '.') r += "."; r += Res2; }
	if (Res3 && *Res3 != '\0') { if (*Res3 != '.') r += "."; r += Res3; }
	if (r.IsNull()) return(NULL);

	if (!withDot) {
		fResource = r;
		if (fEnv->Lookup(fResource.Data())) return(fResource.Data());
	} else {
		if (!fLastPrefix.IsNull()) {
			fResource = Form("%s%s", fLastPrefix.Data(), r.Data());
			if (fEnv->Lookup(fResource.Data())) return(fResource.Data());
		}

		TIterator * iter = fLofPrefixes.MakeIterator();
		TObjString * p;
		while(p = (TObjString *) iter->Next()) {
			fResource = Form("%s%s", p->GetString().Data(), r.Data());
			if (fEnv->Lookup(fResource.Data())) {
				fLastPrefix = p->GetString();
				return(fResource.Data());
			}
		}
	}
	return(NULL);
}

Bool_t TMrbResource::Convert(const Char_t * Resource, TString & StrResult, Int_t & IntResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Convert()
// Purpose:        Convert resource
// Arguments:      Char_t * Resource    -- resource
//                 TString & StrResult  -- where to store ascii part
//                 Int_t & IntResult    -- where to store integer part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks for "x(n)" or "n(x)" and splits value into parts
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t * endptr = NULL;
	StrResult = "";
	IntResult = 0;

	TString r = fEnv->GetValue(Resource, "");
	if (r.IsNull()) return(kFALSE);
	StrResult = r;
	Int_t n1 = r.Index("(");
	Int_t n2 = r.Index(")");
	if (n1 < 0 && n2 < 0) {
		r = r.Strip(TString::kBoth);
		r.ToLower();
		if (r.IsDigit()) {
			IntResult = (Int_t) strtoul(r.Data(), &endptr, r.BeginsWith("0") ? 8 : 10);
		} else if (r.BeginsWith("0x")) {
			IntResult = (Int_t) strtoul(r.Data() + 2, &endptr, 16);
		} else if (r.BeginsWith("0b")) {
			IntResult = (Int_t) strtoul(r.Data() + 2, &endptr, 2);
		}
		if (endptr == NULL) {
			IntResult = 0xaffec0c0;
			return(kTRUE);
		}
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << r << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		StrResult = "";
		return(kTRUE);
	}
	if ((n1 > 0) && (n2 < 0 || n2 < n1)) return(kFALSE);
	if ((n2 > 0) && (n1 < 0)) return(kFALSE);
	TString s1 = r(0, n1); s1 = s1.Strip(TString::kBoth);
	TString s2 = r(n1 + 1, n2 - n1 - 1); s2 = s2.Strip(TString::kBoth);
	if (s1.IsDigit()) {
		IntResult = (Int_t) strtoul(s1.Data(), &endptr, s1.BeginsWith("0") ? 8 : 10);
	} else if (s1.BeginsWith("0x") || s1.BeginsWith("0X")) {
		IntResult = (Int_t) strtoul(s1.Data() + 2, &endptr, 16);
	} else if (s1.BeginsWith("0b") || s1.BeginsWith("0B")) {
		IntResult = (Int_t) strtoul(s1.Data() + 2, &endptr, 2);
	}
	if (endptr != NULL) {
		StrResult = s2;
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << s1 << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		return(kTRUE);
	}
	if (s2.IsDigit()) {
		IntResult = (Int_t) strtoul(s2.Data(), &endptr, s2.BeginsWith("0") ? 8 : 10);
	} else if (s2.BeginsWith("0x") || s2.BeginsWith("0X")) {
		IntResult = (Int_t) strtoul(s2.Data() + 2, &endptr, 16);
	} else if (s2.BeginsWith("0b") || s2.BeginsWith("0B")) {
		IntResult = (Int_t) strtoul(s2.Data() + 2, &endptr, 2);
	}
	if (endptr != NULL) {
		StrResult = s1;
		if (*endptr != '\0') {
			gMrbLog->Err() << "Not a legal integer - " << s2 << endl;
			gMrbLog->Flush(this->ClassName(), "Convert");
			return(kFALSE);
		}
		return(kTRUE);
	}
	StrResult = r;
	IntResult = 0xaffec0c0;
	gMrbLog->Wrn() << "Can't extract integer part - " << r << endl;
	gMrbLog->Flush(this->ClassName(), "Convert");
	return(kTRUE);
}

Bool_t TMrbResource::ToInteger(const Char_t * Resource, Int_t & IntResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::ToInteger()
// Purpose:        Convert to integer
// Arguments:      Char_t * Resource    -- resource
//                 Int_t & IntResult    -- where to store integer part
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to get the integer part of a resource
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	IntResult = 0;
	TString s;
	Int_t n = 0xaffec0c0;
	if (!this->Convert(Resource, s, n)) return(kFALSE);
	if (n == (Int_t) 0xaffec0c0) return(kFALSE);
	IntResult = n;
	return(kTRUE);
}

Bool_t TMrbResource::ToString(const Char_t * Resource, TString & StrResult) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::ToString()
// Purpose:        Convert to string
// Arguments:      Char_t * Resource      -- resource
//                 TString & StrResult    -- where to store string
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to get a resource as string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	StrResult = "";
	Int_t n;
	TString s = "\001";
	if (!this->Convert(Resource, s, n)) return(kFALSE);
	if (s(0) == '\001') return(kFALSE);
	StrResult = s;
	return(kTRUE);
}

const Char_t * TMrbResource::Replace(TString & String) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbResource::Replace
// Purpose:        Replace environment references
// Arguments:      TString String  -- string containing @{...}
// Results:        Char_t * String -- resulting string
// Exceptions:
// Description:    Expands @{<envname>:<default>}
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString replacement = "";
	Int_t idx1 = 0;
	Int_t idx2 = 0;
	while ((idx1 = String.Index("@{", idx2)) >= 0) {
		if (idx1 > idx2) replacement += String(idx2, idx1-idx2);
		idx2 = String.Index("}", idx1);
		if (idx2 == -1) {
			gMrbLog->Err() << "Syntax error in @{} - missing closing brace" << endl;
			gMrbLog->Flush(this->ClassName(), "Replace");
			return(NULL);
		}
		TString env = String(idx1+2, idx2-idx1-2);
		Int_t semi = env.Index(":", 0);
		if (semi == -1) {
			gMrbLog->Err() << "Syntax error in @{} - missing semicolon" << endl;
			gMrbLog->Flush(this->ClassName(), "Replace");
			return(NULL);
		}
		TString defval = env(semi+1,env.Length()-semi);
		env = env(0, semi);
		this->Get(env, env.Data(), defval.Data());
		replacement += env;
		idx2++;
		idx1 = idx2;
	}
	replacement += String(idx2, String.Length()-idx2);
	String = replacement;
	return(String.Data());
}


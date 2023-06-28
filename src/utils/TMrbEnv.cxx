//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbEnv.cxx
// Purpose:        Handle resource files
// Description:    Implements class methods to handle resource files
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbEnv.cxx,v 1.22 2008-03-05 09:34:13 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TObjString.h"
#include "TDatime.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbEnv.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbEnv)

extern TMrbLogger * gMrbLog;					// MARaBOU's message logger

TMrbEnv::TMrbEnv(const Char_t * ResourceFile, const Char_t * DefaultsFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv
// Purpose:        Open a resource file
// Arguments:      Char_t * ResourceFile   -- resource file
//                 Char_t * DefaultFile    -- file containing defaults
// Results:        --
// Exceptions:   
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fCurEnv = NULL;
	fDefaultsEnv = NULL;
	fIsSystemEnv = kFALSE;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (ResourceFile != NULL && *ResourceFile != '\0') {
		if (this->Open(ResourceFile)) { 							// try to open resource file
			if (DefaultsFile != NULL && *DefaultsFile != '\0') {
				if (!this->OpenDefaults(DefaultsFile)) {  			// try to open defaults
					gMrbLog->Err() << "Can't open defaults file - " << DefaultsFile << endl;
					gMrbLog->Flush(this->ClassName());
					this->MakeZombie();
				} else if (!this->HasDefaults()) {
					gMrbLog->Wrn() << "No defaults given" << endl;
					gMrbLog->Flush(this->ClassName());
					this->MakeZombie();
				}
			}
		} else {
			gMrbLog->Err() << "Can't open resource file - " << ResourceFile << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		}
	} else {
		fIsSystemEnv = kTRUE;
		fCurEnv = gEnv;
		fCurFile = ".rootrc";
		fIsModified = kFALSE;
		fResourceName = "";
		this->SetPrefix("");
	}				
}

Bool_t TMrbEnv::Open(const Char_t * ResourceFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Open
// Purpose:        Open resource file
// Arguments:      Char_t * ResourceFile   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a resource file
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t fileExists;
	TString resFile, rf;

	TDatime dt;
	ofstream f;

	resFile = gSystem->ExpandPathName(ResourceFile);
#if 0
	if (resFile(0) == '/') {
		gMrbLog->Err() << "Not a LOCAL file - " << resFile.Data() << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		return(kFALSE);
	}
	if (resFile.Index("./", 0) == 0) resFile = resFile(2, 1000);
	rf = gSystem->BaseName(resFile.Data());
	if (resFile.CompareTo(rf.Data()) != 0) {
		gMrbLog->Err() << "Not a LOCAL file - " << resFile.Data() << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		return(kFALSE);
	}
#endif
	
	fileExists = !gSystem->AccessPathName(resFile.Data(), (EAccessMode) F_OK);		// file existing?

	if (!fIsSystemEnv && fCurEnv != NULL) delete fCurEnv;
	fCurEnv = new TEnv(resFile.Data());
	fCurFile = resFile;
	this->Set("TMrbEnv.Info.File", gSystem->BaseName(resFile.Data()));
	this->Set("TMrbEnv.Info.Modified", dt.AsString());
	if (!fileExists) {
		this->Set("TMrbEnv.Info.Created", dt.AsString());
		gMrbLog->Out() << "Creating file " << resFile.Data() << endl;
		gMrbLog->Flush(this->ClassName(), "Open", setblue);
		this->Save(kFALSE);
	}

	fIsModified = kFALSE;
	fIsSystemEnv = kFALSE;
	fResourceName = "";
	this->SetPrefix("");
	return(kTRUE);
}

Bool_t TMrbEnv::OpenDefaults(const Char_t * DefaultsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::OpenDefaults
// Purpose:        Open defaults file
// Arguments:      Char_t * DefaultsFile   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens defaults file
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString defFile;

	defFile = gSystem->ExpandPathName(DefaultsFile);
	if (gSystem->AccessPathName(defFile.Data(), (EAccessMode) F_OK)) {		// file existing?
		gMrbLog->Err() << "No such file - " << defFile.Data() << endl;
		gMrbLog->Flush(this->ClassName(), "OpenDefaults");
		return(kFALSE);
	}

	if (fDefaultsEnv != NULL) delete fDefaultsEnv;
	fDefaultsEnv = new TEnv(defFile.Data());
	fDefaultsFile = defFile;
	return(kTRUE);
}

void TMrbEnv::Save(Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Save
// Purpose:        Save contents to file
// Arguments:      Bool_t Verbose   -- print message if kTRUE
// Results:        --
// Exceptions:
// Description:    Saves resources and closes file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (IsModified()) { 				// some modifications?
		TString pfs = fPrefix;
		this->SetPrefix("");
		if (!fIsSystemEnv) {
			TDatime dt;
			this->Set("TMrbEnv.Info.Modified", dt.AsString());
		}
		fCurEnv->SaveLevel(kEnvChange);
		fCurEnv->SaveLevel(kEnvLocal);
		fCurEnv->SaveLevel(kEnvUser);
		if (Verbose) {
			gMrbLog->Out()	<< "Resource data saved to file " << fCurFile << endl;
			gMrbLog->Flush(this->ClassName(), "Save", setblue);
		}
		fPrefix = pfs;
	}
	fIsModified = kFALSE;
}

void TMrbEnv::SetPrefix(const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::SetPrefix
// Purpose:        Define resource prefix
// Arguments:      Char_t * Prefix   -- resource prefix
// Results:        --
// Exceptions:
// Description:    Defines fixed header part of resource string.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fPrefix = Prefix;
	if (!fPrefix.IsNull()) {
		fPrefix += ".";
		fPrefix(0,1).ToUpper();	// first char must be upper case
	}
}

const Char_t * TMrbEnv::Resource(TString & R, const Char_t * Field1, Int_t Index1, const Char_t * Field2, Int_t Index2, const Char_t * Field3) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Resource
// Purpose:        Initialize resource string
// Arguments:      TString & R             -- where to store resulting name
//                 Char_t * Field1         -- resource field 1
//                 Int_t Index1            -- index field 1
//                 Char_t * Field2         -- resource field 2
//                 Int_t Index2            -- index field 2
//                 Char_t * Field3         -- resource field 3
// Results:        Char_t * ResourceName   -- resource name (same as R)
// Exceptions:
// Description:    Creates a resource string "Prefix.Field1Index1.Field2Index2.Field3"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	R = Field1;
	if (Index1 != -1) R += Index1;

	if (*Field2 != '\0') {
		R += ".";
		R += Field2;
		if (Index2 != -1) R += Index2;
		if (*Field3 != '\0') {
			R += ".";
			R += Field3;
		}
	}

	return(R.Data());
}

Bool_t TMrbEnv::Set(const Char_t * Resource, const Char_t * StrVal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Set
// Purpose:        Assign an ascii value
// Arguments:      Char_t * Resource    -- resource name
//                 Char_t * StrVal      -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a given resource.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString resValue, resString;

	fResourceName = fPrefix + Resource;

	fCurEnv->SetValue(fResourceName.Data(), StrVal);	// set resource
	fIsModified = kTRUE;
	return(kTRUE);
}

Bool_t TMrbEnv::Set(const Char_t * Resource, Int_t IntVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Set
// Purpose:        Assign an integer value
// Arguments:      Char_t * Resource         -- resource name
//                 Int_t IntVal              -- value to be set
//                 Int_t Base                -- base to be used
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a given resource.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString resString, resValue;

	fResourceName = fPrefix + Resource;

	resValue.FromInteger(IntVal, 0, Base, kFALSE, kTRUE);
	fCurEnv->SetValue(fResourceName.Data(), resValue.Data());	// set resource
	fIsModified = kTRUE;
	return(kTRUE);
}

Bool_t TMrbEnv::Set(const Char_t * Resource, Double_t DblVal, Int_t Precision) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Set
// Purpose:        Assign an integer value
// Arguments:      Char_t * Resource         -- resource name
//                 Double_t DblVal           -- value to be set
//                 Int_t Precision           -- precision
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a given resource.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString resValue;

	fResourceName = fPrefix + Resource;

	resValue.FromDouble(DblVal, 0, Precision);
	fCurEnv->SetValue(fResourceName.Data(), resValue.Data());	// set resource
	fIsModified = kTRUE;
	return(kTRUE);
}

Bool_t TMrbEnv::Set(const Char_t * Resource, TMrbNamedX * NamedVal, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Set
// Purpose:        Assign a pair of name + integer value
// Arguments:      Char_t * Resource         -- resource name
//                 TMrbNamedX * NamedVal     -- value to be set
//                 Int_t Base                -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a given resource.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fResourceName = fPrefix + Resource;

	TMrbString resValue = NamedVal->GetName();
	resValue += "(";
	resValue.AppendInteger(NamedVal->GetIndex(), 0, Base, kFALSE, kTRUE, kTRUE);
	resValue += ")";
	fCurEnv->SetValue(fResourceName.Data(), resValue.Data());	// set resource
	fIsModified = kTRUE;
	return(kTRUE);
}

	Int_t TMrbEnv::Get(const Char_t * Resource, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Get
// Purpose:        Read an int resource from database
// Arguments:      Char_t * Resource     -- resource name
//                 Int_t Default         -- default value
// Results:        Int_t Value           -- integer value
// Exceptions:
// Description:    Reads a given resource from database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString resValue;
	Int_t intVal;

	fResourceName = fPrefix + Resource;

	resValue = fCurEnv->GetValue(fResourceName, "<undef>");	// read as ascii
	if (resValue.CompareTo("<undef>") == 0) {
		if (this->HasDefaults()) {
			resValue = fDefaultsEnv->GetValue(fResourceName, "<undef>");
			if (resValue.CompareTo("<undef>") == 0) resValue = Default;
		} else {
			resValue = Default;
		}
	}
	resValue.Strip(TString::kBoth);
	resValue.ToInteger(intVal);
	return(intVal);
}

Double_t TMrbEnv::Get(const Char_t * Resource, Double_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Get
// Purpose:        Read an int resource from database
// Arguments:      Char_t * Resource     -- resource name
//                 Double_t Default      -- default value
// Results:        Double_t Value        -- integer value
// Exceptions:
// Description:    Reads a given resource from database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString resValue;
	Double_t dblVal;

	fResourceName = fPrefix + Resource;

	resValue = fCurEnv->GetValue(fResourceName, "<undef>");	// read as ascii
	if (resValue.CompareTo("<undef>") == 0) {
		if (this->HasDefaults()) {
			resValue = fDefaultsEnv->GetValue(fResourceName, "<undef>");
			if (resValue.CompareTo("<undef>") == 0) resValue = Default;
		} else {
			resValue = Default;
		}
	}
	resValue.Strip(TString::kBoth);
	resValue.ToDouble(dblVal);
	return(dblVal);
}

const Char_t * TMrbEnv::Get(TString & Result, const Char_t * Resource, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Get
// Purpose:        Read an ascii resource from database
// Arguments:      TString & Result      -- where to store resulting value
//                 Char_t * Resource     -- resource name
//                 Char_t * Default      -- default value
// Results:        Char_t * Value        -- ascii value (same as Result.Data())
// Exceptions:
// Description:    Reads a given resource from database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fResourceName = fPrefix + Resource;

	Result = fCurEnv->GetValue(fResourceName, "<undef>");	// read as ascii
	if (Result.CompareTo("<undef>") == 0) {
		if (this->HasDefaults()) {
			Result = fDefaultsEnv->GetValue(fResourceName, "<undef>");
			if (Result.CompareTo("<undef>") == 0) Result = Default;
		} else {
			Result = Default;
		}
	}
	return(Result.Data());
}

const Char_t * TMrbEnv::Get(TMrbNamedX & Result, const Char_t * Resource, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Get
// Purpose:        Read an ascii resource from database
// Arguments:      TMrbNamedX & Result   -- where to store resulting value
//                 Char_t * Resource     -- resource name
//                 Char_t * Default      -- default value
// Results:        Char_t * Value        -- ascii value (same as Result.Data())
// Exceptions:
// Description:    Reads a given resource from database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resVal;
	TMrbString resNum;
	Int_t intVal;
	Int_t base;

	fResourceName = fPrefix + Resource;

	resVal = fCurEnv->GetValue(fResourceName, "<undef>");	// read as ascii
	if (resVal.CompareTo("<undef>") == 0) {
		if (this->HasDefaults()) {
			resVal = fDefaultsEnv->GetValue(fResourceName, "<undef>");
			if (resVal.CompareTo("<undef>") == 0) resVal = Default;
		} else {
			resVal = Default;
		}
	}
	resVal.Strip(TString::kBoth);
	Result.Set(-1, resVal);
	Int_t idx1 = resVal.Index("(", 0);
	if (idx1 >= 0) {
		Int_t idx2 = resVal.Index(")", 0);
		if (idx2 > idx1 + 1) {
			resNum = resVal(idx1 + 1, idx2 - idx1 - 1);
			resNum.Strip(TString::kBoth);
			base = 10;
			if (resNum.Index("0b", 0) == 0)			base = 2;
			else if (resNum.Index("0x", 0) == 0)	base = 16;
			else if (resNum.Index("0", 0) == 0)		base = 8;
			if (base != 10) resNum = resNum(2, 1000);
			resNum.ToInteger(intVal, base);
			TString rv = resVal(0, idx1);
			Result.Set(intVal, rv.Data());
		}
	}

	return(Result.GetName());
}

Int_t TMrbEnv::Find(const Char_t * LofPrefixes, const Char_t * Resource, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Find
// Purpose:        Search for an int resource in database
// Arguments:      Char_t * LofPrefixes  -- prefixes to be tried out
//                 Char_t * Resource     -- resource name
//                 Int_t Default         -- default value
// Results:        Int_t Value           -- integer value
// Exceptions:
// Description:    Searches for a given resource trying different prefixes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString lofPrefixes = LofPrefixes;
	TObjArray * lPrefs = lofPrefixes.Tokenize(":");
	Int_t nPrefs = lPrefs->GetEntries();
	delete lPrefs;
	if (nPrefs <= 0) return(Default);

	TEnv * dEnvSave = fDefaultsEnv; 	// turn off defaults
	fDefaultsEnv = NULL;
	TString prefSave = fPrefix;
	Int_t intVal;
	Int_t from = 0;
	while (lofPrefixes.Tokenize(fPrefix, from, ":")) {
		fPrefix += ".";
		TString resValue;
		this->Get(resValue, Resource, "<undef>");
		if (resValue.CompareTo("<undef>") != 0) {
			intVal = this->Get(Resource, Default);
			fPrefix = prefSave;
			fDefaultsEnv = dEnvSave;
			return(intVal);
		}
	}
	fPrefix = "";
	fDefaultsEnv = dEnvSave;
	intVal = this->GetDefault(Resource, Default);
	fPrefix = prefSave;
	return(intVal);
}

const Char_t * TMrbEnv::Find(TString & Result, const Char_t * LofPrefixes, const Char_t * Resource, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Find
// Purpose:        Find an ascii resource in database
// Arguments:      TString & Result      -- where to store resulting value
//                 Char_t * LofPrefixes  -- prefixes to be tried out
//                 Char_t * Resource     -- resource name
//                 Char_t * Default      -- default value
// Results:        Char_t * Value        -- ascii value (same as Result.Data())
// Exceptions:
// Description:    Searches for a given resource  trying different prefixes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString lofPrefixes = LofPrefixes;
	TObjArray * lPrefs = lofPrefixes.Tokenize(":");
	Int_t nPrefs = lPrefs->GetEntries();
	delete lPrefs;
	if (nPrefs <= 0) return(Default);

	TEnv * dEnvSave = fDefaultsEnv; 	// turn off defaults
	fDefaultsEnv = NULL;
	TString prefSave = fPrefix;
	TString resValue;
	Int_t from = 0;
	while (lofPrefixes.Tokenize(fPrefix, from, ":")) {
		fPrefix += ".";
		this->Get(resValue, Resource, "<undef>");
		if (resValue.CompareTo("<undef>") != 0) {
			this->Get(Result, Resource, Default);
			fPrefix = prefSave;
			fDefaultsEnv = dEnvSave;
			return(Result.Data());
		}
	}
	fPrefix = "";
	fDefaultsEnv = dEnvSave;
	this->GetDefault(Result, Resource, Default);
	fPrefix = prefSave;
	return(Result.Data());
}

const Char_t * TMrbEnv::Find(TMrbNamedX & Result, const Char_t * LofPrefixes, const Char_t * Resource, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Find
// Purpose:        Find an ascii resource in database
// Arguments:      TMrbNamedX & Result   -- where to store resulting value
//                 Char_t * LofPrefixes  -- prefixes to be tried out
//                 Char_t * Resource     -- resource name
//                 Char_t * Default      -- default value
// Results:        Char_t * Value        -- ascii value (same as Result.Data())
// Exceptions:
// Description:    Searches for a given resource  trying different prefixes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString lofPrefixes = LofPrefixes;
	TObjArray * lPrefs = lofPrefixes.Tokenize(":");
	Int_t nPrefs = lPrefs->GetEntries();
	delete lPrefs;
	if (nPrefs <= 0) return(Default);

	TEnv * dEnvSave = fDefaultsEnv; 	// turn off defaults
	fDefaultsEnv = NULL;
	TString prefSave = fPrefix;
	TString resValue;
	Int_t from = 0;
	while (lofPrefixes.Tokenize(fPrefix, from, ":")) {
		fPrefix += ".";
		this->Get(resValue, Resource, "<undef>");
		if (resValue.CompareTo("<undef>") != 0) {
			this->Get(Result, Resource, Default);
			fPrefix = prefSave;
			fDefaultsEnv = dEnvSave;
			return(Result.GetName());
		}
	}
	fPrefix = "";
	fDefaultsEnv = dEnvSave;
	this->GetDefault(resValue, Resource, Default);
	fPrefix = prefSave;
	Result.Set(-1, resValue.Data());
	return(Result.GetName());
}

const Char_t * TMrbEnv::GetDefault(TString & Result, const Char_t * Resource, const Char_t * Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::GetDefault
// Purpose:        Read a resource from defaults
// Arguments:      TString & Result  -- where to store resulting value
//                 Char_t * Resource -- resource name
//                 Char_t * Default  -- default value
// Results:        Char_t * Value    -- resource value (same as Result.Data())
// Exceptions:
// Description:    Reads a given resource from defaults.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fResourceName = fPrefix + Resource;

	Result = this->HasDefaults() ? fDefaultsEnv->GetValue(fResourceName, Default) : Default;	// read as ascii
	return(Result.Data());
}

Int_t TMrbEnv::GetDefault(const Char_t * Resource, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::GetDefault
// Purpose:        Read a resource from defaults
// Arguments:      Char_t * Resource -- resource name
//                 Int_t Default     -- default value
// Results:        Int_t Value       -- integer representation
// Exceptions:
// Description:    Reads a given resource from defaults.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t resValue;

	fResourceName = fPrefix + Resource;

	resValue = this->HasDefaults() ? fDefaultsEnv->GetValue(fResourceName.Data(), Default) : Default;	// read integer
	return(resValue);
}

Int_t TMrbEnv::CopyDefaults(const Char_t * Resource, Bool_t ExactMatch, Bool_t OverWrite, TMrbLofNamedX * LofSubstitutions) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::CopyDefaults
// Purpose:        Copy a resource value from defaults
// Arguments:      Char_t * Resource                  -- resource name
//                 Bool_t ExactMatch                  -- how to match resource names?
//                 Bool_t OverWrite                   -- overwrite existing resources?
//                 TMrbLofNamedX * LofSubstitutions   -- list of substitutions to be performed
// Results:        Int_t NofRes                       -- number of resources copied
// Exceptions:
// Description:    Sets a resource to its default value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resValue, rv;
	TString resPattern;
	Int_t nofRes;

	if (!this->HasDefaults()) {
		gMrbLog->Err() << "No default resources present" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyDefaults");
		return(-1);
	}

	TString resName = Resource;
	fResourceName = fPrefix + resName;

	Bool_t wildCard = (fResourceName.Index("*", 0) >= 0);

	if (ExactMatch && !wildCard) {
		rv = fCurEnv->GetValue(fResourceName, "<undef>");
		if (OverWrite || rv.CompareTo("<undef>") == 0) {
			resValue = fDefaultsEnv->GetValue(fResourceName, "<undef>");	// read as ascii
			if (resValue.CompareTo("<undef>") == 0) {
				gMrbLog->Err() << "Resource " << fResourceName << ": No default given" << endl;
				gMrbLog->Flush(this->ClassName(), "CopyDefaults");
				return(-1);
			}
			if (LofSubstitutions != NULL) {
				TIterator * siter = LofSubstitutions->MakeIterator();
				TMrbNamedX * s;
				while (s = (TMrbNamedX *) siter->Next()) {
					resName.ReplaceAll(s->GetName(), s->GetTitle());
					resValue.ReplaceAll(s->GetName(), s->GetTitle());
				}
			}
			return(this->Set(resName.Data(), resValue.Data()) ? 1 : -1);
		} else {
			return(0);
		}
	} else {
		resPattern = fResourceName;
		if (!wildCard) resPattern += ".*";
		TRegexp rexp(resPattern.Data(), kTRUE);
		nofRes = this->CopyDefaults(rexp, OverWrite, LofSubstitutions);
		if (nofRes == -1) {
			gMrbLog->Err() << "No resource matching \"" << resPattern << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "CopyDefaults");
			return(-1);
		} else {
			if (nofRes > 0) {
				if (LofSubstitutions != NULL) {
					TIterator * siter = LofSubstitutions->MakeIterator();
					TMrbNamedX * s;
					while (s = (TMrbNamedX *) siter->Next()) resPattern.ReplaceAll(s->GetName(), s->GetTitle());
				}
				gMrbLog->Out()	<< "Copied " << nofRes
								<< " resource(s) matching \"" << resPattern << "\"" << endl;
				gMrbLog->Flush(this->ClassName(), "CopyDefaults", setblue);
			}
			return(nofRes);
		}
	}
	return(0);
}

Int_t TMrbEnv::CopyDefaults(const TRegexp & Regexp, Bool_t OverWrite, TMrbLofNamedX * LofSubstitutions) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::CopyDefaults
// Purpose:        Copy resource values within regular expression
// Arguments:      TRegexp Regexp                     -- regular expression
//                 Bool_t OverWrite                   -- overwrite existing resources?
//                 TMrbLofNamedX * LofSubstitutions   -- list of substitutions to be performed
// Results:        Int_t NofRes                       -- number of resources copied
// Exceptions:
// Description:    Sets resource(s) to default value(s).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->HasDefaults()) {
		gMrbLog->Err() << "No default resources present" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyDefaults");
		return(-1);
	}

	TRegexp infoRexp("*.Info.*", kTRUE);

	Int_t nofRes = 0;
	TString pfs = fPrefix;
	this->SetPrefix("");
	TList * lofDefaults = (TList *) fDefaultsEnv->GetTable();
	TIterator * iter = lofDefaults->MakeIterator();
	TEnvRec * r;
	while (r = (TEnvRec *) iter->Next()) {
		TString resName = r->GetName();
		TString resValue = r->GetValue();
		if (resName.Index(Regexp) == -1) continue;
		if (resName.Index(infoRexp) != -1) continue;
		if (LofSubstitutions != NULL) {
			TIterator * siter = LofSubstitutions->MakeIterator();
			TMrbNamedX * s;
			while (s = (TMrbNamedX *) siter->Next()) {
				resName.ReplaceAll(s->GetName(), s->GetTitle());
				resValue.ReplaceAll(s->GetName(), s->GetTitle());
			}
		}
		TString rv = fCurEnv->GetValue(resName.Data(), "<undef>");
		if (OverWrite || (rv.CompareTo("<undef>") == 0)) {
			if (resValue.CompareTo("<undef>") != 0) {
				if (this->Set(resName.Data(), resValue.Data())) nofRes++;
			}
		}
	}
	fPrefix = pfs;
	return(nofRes);
}

Int_t TMrbEnv::SetUndef(const Char_t * Resource, Bool_t ExactMatch) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::SetUndef
// Purpose:        Set resource value to "undefined"
// Arguments:      Char_t * Resource    -- resource name
//                 Bool_t ExactMatch    -- how to match resource names
// Results:        Int_t NofRes         -- number of resources found
// Exceptions:
// Description:    Sets a resource to <undef>.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resValue;
	TString resPattern;
	Int_t nofRes;

	fResourceName = fPrefix + Resource;

	if (ExactMatch) {
		resValue = fCurEnv->GetValue(fResourceName, "<undef>");	// read as ascii
		if (resValue.CompareTo("<undef>") == 0) {
			gMrbLog->Err() << "No such resource - " << fResourceName << endl;
			gMrbLog->Flush(this->ClassName(), "SetUndef");
			return(-1);
		} else {
			return(this->Set(Resource, "<undef>") ? 1 : -1);
		}
	} else {
		resPattern = fResourceName;
		resPattern += ".*";
		TRegexp rexp(resPattern.Data(), kTRUE);
		nofRes = this->SetUndef(rexp);
		if (nofRes <= 0) {
			gMrbLog->Err() << "No resource matching \"" << resPattern << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "SetUndef");
			return(-1);
		} else {
			return(nofRes);
		}
	}
}

Int_t TMrbEnv::SetUndef(const TRegexp & Regexp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::SetUndef
// Purpose:        Set resource value to "undefined"
// Arguments:      TRegexp Regexp      -- regular expression
// Results:        Int_t NofRes        -- number of entries found
// Exceptions:
// Description:    Sets a resource to <undef>.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resEntry;
	TString resName;
	TString resValue;
	TString pfs;
	ifstream cFile;
	Int_t n;
	Int_t nofRes;

	this->Save(kFALSE);

	cFile.open(fCurFile.Data(), ios::in);
	if (!cFile.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << fCurFile << endl;
		gMrbLog->Flush(this->ClassName(), "SetUndef");
		return(-1);
	}
	
	pfs = fPrefix;
	this->SetPrefix("");
	nofRes = 0;
	while (!cFile.eof()) {
		resEntry.ReadLine(cFile, kFALSE);
		n = resEntry.Index(":");
		resName = resEntry(0, n);
		if (resName.Index(Regexp) == -1) continue;
		if (this->Set(resName.Data(), "<undef>")) nofRes++;
	}
	cFile.close();
	fPrefix = pfs;
	return(nofRes);
}

Int_t TMrbEnv::Remove(const Char_t * Resource, Bool_t ExactMatch) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Remove
// Purpose:        Remove resource from file
// Arguments:      Char_t * Resource    -- resource name
//                 Bool_t ExactMatch    -- how to match resource names
// Results:        Int_t NofRes         -- number of resources removed
// Exceptions:
// Description:    Removes resource(s) from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofRes;

	this->SetUndef(Resource, ExactMatch);
	nofRes = this->Purge();
	if (nofRes > 0) {
		gMrbLog->Out()	<< nofRes << " resource(s) removed from file " << fCurFile << endl;
		gMrbLog->Flush(this->ClassName(), "Remove", setblue);
	}
	return(nofRes);
}

Int_t TMrbEnv::Remove(const TRegexp & Regexp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Remove
// Purpose:        Remove resource from file
// Arguments:      TRegexp Regexp      -- regular expression
// Results:        Int_t NofRes        -- number of entries removed
// Exceptions:
// Description:    Removes resource(s) from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofRes;

	this->SetUndef(Regexp);
	nofRes = this->Purge();
	if (nofRes > 0) {
		gMrbLog->Out()	<< nofRes << " resource(s) removed from file " << fCurFile << endl;
		gMrbLog->Flush(this->ClassName(), "Remove", setblue);
	}
	return(nofRes);
}

Int_t TMrbEnv::Purge(Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::Remove
// Purpose:        Remove resource from file
// Arguments:      Bool_t Verbose     -- output message if kTRUE
// Results:        Int_t NofRes       -- number of entries removed
// Exceptions:
// Description:    Removes undefined resources from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString resEntry;
	TString pfs;
	ifstream iFile;
	ofstream oFile;
	Int_t nofRes;
	TObjArray lofResources;
	TObjString *ostr;

	if (fIsSystemEnv) {
		gMrbLog->Err()	<< "Can't call method \"Purge()\" for ROOT's system environment" << endl;
		gMrbLog->Flush(this->ClassName(), "Remove");
		return(0);
	}

	this->Save(kFALSE);

	iFile.open(fCurFile.Data(), ios::in);
	if (!iFile.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << fCurFile << endl;
		gMrbLog->Flush(this->ClassName(), "Purge");
		return(-1);
	}
	
	pfs = fPrefix;
	this->SetPrefix("");
	nofRes = 0;
	TRegexp rexp(".*<undef>.*", kFALSE);
	lofResources.Delete();
	while (!iFile.eof()) {
		resEntry.ReadLine(iFile, kFALSE);
		resEntry.Strip(TString::kBoth);
		if ((resEntry.Length() == 0) || (resEntry.Index(rexp) >= 0)) { nofRes++; continue; }
		lofResources.Add(new TObjString(resEntry.Data()));
	}
	iFile.close();

	if (nofRes > 0) {
		oFile.open(fCurFile.Data(), ios::out);
		if (!oFile.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << fCurFile << endl;
			gMrbLog->Flush(this->ClassName(), "Purge");
			return(-1);
		}
		ostr = (TObjString *) lofResources.First();
		while (ostr) {
			oFile << ostr->String() << endl;
			ostr = (TObjString *) lofResources.After(ostr);
		}
		oFile.close();
		delete fCurEnv;
		fCurEnv = new TEnv(fCurFile.Data());
	}	
		
	if (Verbose && (nofRes > 0)) {
		gMrbLog->Out()	<< nofRes << " resource(s) removed from file " << fCurFile << endl;
		gMrbLog->Flush(this->ClassName(), "Purge", setblue);
	}

	fPrefix = pfs;
	return(nofRes);
}

void TMrbEnv::PrintInfo() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv::PrintInfo()
// Purpose:        Print file info
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs file info to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pfs;
	TString res;

	if (!fIsSystemEnv) {
		pfs = fPrefix;
		this->SetPrefix("");

		cout	<< "File          : " <<  this->Get(res, "TMrbEnv.Info.File") << endl;
		cout	<< "Created       : " <<  this->Get(res, "TMrbEnv.Info.Created") << endl;
		cout	<< "Modified      : " <<  this->Get(res, "TMrbEnv.Info.Modified") << endl;
		fPrefix = pfs;
	}
}

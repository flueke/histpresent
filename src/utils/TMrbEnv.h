#ifndef __TMrbEnv_h__
#define __TMrbEnv_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbEnv.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbEnv       -- provide an extended TEnv
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbEnv.h,v 1.8 2006-10-31 15:46:33 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TObject.h"
#include "TString.h"
#include "TList.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TRegexp.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEnv
// Purpose:        Handle resource files
// Description:    Provides a means to handle resource files (e.g. .rootrc)
//                 Based on original TEnv class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEnv : public TObject {

	public:
		TMrbEnv(const Char_t * ResourceFile = "", const Char_t * DefaultsFile = "");	// explicit ctor: names of resource files

		~TMrbEnv() {								// dtor: delete heap objects
			if (!fIsSystemEnv) delete fCurEnv;
			delete fDefaultsEnv;
		};

		TMrbEnv(const TMrbEnv &) {};				// default copy ctor

		Bool_t Open(const Char_t * ResourceFile);								// open resource file
		Bool_t OpenDefaults(const Char_t * DefaultsFile);						// open defaults file
		void Save(Bool_t Verbose = kTRUE);										// save contents 
		void SetPrefix(const Char_t * Prefix);									// define resource prefix

		Bool_t Set(const Char_t * Resource, const Char_t * StrVal); 			// set ascii value
		Bool_t Set(const Char_t * Resource, Int_t IntVal, Int_t Base = 10);		// set integer value
		Bool_t Set(const Char_t * Resource, Double_t DblVal, Int_t Precision = 4);	// set float value
		Bool_t Set(const Char_t * Resource, TMrbNamedX * NamedVal, Int_t Base = 10);	// set ascii(int)

																				// get resource value
		Int_t Get(const Char_t * Resource, Int_t Default);						// ... integer
		Double_t Get(const Char_t * Resource, Double_t Default);				// ... float
		const Char_t * Get(TString & Result, const Char_t * Resource, const Char_t * Default = ""); 	// ... ascii
		const Char_t * Get(TMrbNamedX & Result, const Char_t * Resource, const Char_t * Default = "");	// ... ascii(int)

																				// find resource value
		Int_t Find(const Char_t * LofPrefixes, const Char_t * Resource, Int_t Default);			// ... integer
		const Char_t * Find(TString & Result, const Char_t * LofPrefixes, const Char_t * Resource, const Char_t * Default = ""); 	// ... ascii
		const Char_t * Find(TMrbNamedX & Result, const Char_t * LofPrefixes, const Char_t * Resource, const Char_t * Default = "");	// ... ascii(int)

																				// get default resource value
		Int_t GetDefault(const Char_t * Resource, Int_t Default);				// ... integer
		const Char_t * GetDefault(TString & Result, const Char_t * Resource, const Char_t * Default = "");	// ... ascii

																				// set resource(s) from defaults
		Int_t CopyDefaults(const Char_t * Resource, Bool_t ExactMatch = kTRUE, Bool_t OverWrite = kFALSE, TMrbLofNamedX * LofSubstitutions = NULL);
		Int_t CopyDefaults(const TRegexp & Regexp, Bool_t OverWrite = kFALSE, TMrbLofNamedX * LofSubstitutions = NULL);

		Int_t SetUndef(const Char_t * Resource, Bool_t ExactMatch = kTRUE);		// deactivate resource(s)
		Int_t SetUndef(const TRegexp & Regexp);

		Int_t Remove(const Char_t * Resource, Bool_t ExactMatch = kTRUE);		// remove resource from file
		Int_t Remove(const TRegexp & Regexp);
		Int_t Purge(Bool_t Verbose = kFALSE);									// remove undef resources

		void Print(Option_t * Option) const { TObject::Print(Option); }
		inline void Print() const { fCurEnv->Print(); };								// print current settings
		inline void PrintDefaults() const { if (this->HasDefaults()) fDefaultsEnv->Print(); };	// print default settings												// print default settings
		void PrintInfo();														// print file info

		inline TEnv * GetEnv() { return(fCurEnv); };

		const Char_t * Resource(TString & R,	const Char_t * F1, Int_t X1 = -1,	// init resource name
												const Char_t * F2 = "", Int_t X2 = -1,
												const Char_t * F3 = "") const;

		inline Bool_t HasDefaults() const { return(fDefaultsEnv != NULL); };			// default file open?
		inline Bool_t IsModified() const { return(fIsModified); };					// modified?

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TString fCurFile;				// path to current resource file
		TEnv * fCurEnv;					// current resource data base 

		Bool_t fIsModified; 			// kTRUE, if at least 1 entry modified
		Bool_t fIsSystemEnv;			// if mapped to gEnv

		TString fDefaultsFile;			// path to default resource file
		TEnv * fDefaultsEnv;			// defaults data base

		TString fResourceName;			// resource name
		TString fPrefix;				// prefix

	ClassDef(TMrbEnv, 1)		// [Utils] Some extensions to TEnv class
};

#endif

#ifndef __TMrbLofUserVars_h__
#define __TMrbLofUserVars_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbLofUserVars.h
// Purpose:        Define named variables and windows
// Class:          TMrbLofUserVars -- a list of user-defined variables and windows
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLofUserVars.h,v 1.8 2010-11-22 11:41:07 Marabou Exp $
// Date:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TObject.h"
#include "TNamed.h"
#include "TDirectory.h"
#include "TObjArray.h"
#include "TIterator.h"
#include "TSystem.h"
#include "TFormula.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVar.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars
// Purpose:        Defines a list of user-defined variables and windows
// Description:    Defines a list of user variables.
//                 This class is mainly used to keep track of all vars and
//                 windows defined in the application (global gMrbLofUserVars).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofUserVars: public TNamed {

	public:
		TMrbLofUserVars() {};		// default ctor

		~TMrbLofUserVars() { 		// dtor
			fLofVars.Clear();			// empty list - but don't delete objects
			gDirectory->GetList()->Remove(this);
		};

		TMrbLofUserVars(const Char_t * Name);				// explicit ctor

		TMrbLofUserVars(const TMrbLofUserVars &) {};		// default copy ctor

		Bool_t Append(TObject * VarObj);					// add a new object
		inline void Remove(TObject * VarObj) {				// remove object from list
			fLofVars.Remove(VarObj);
			fLofVars.Compress();
		};
		Bool_t Initialize(const Char_t * Option = "Default");		// set all vars/windows to initial values
		Bool_t ReadFromFile(const Char_t * FileName);		// read definitions and settings from file

		virtual void Print(ostream & OutStrm, Option_t * Option = "Default") const;	// output settings to cout
		virtual void Print(Option_t * Option = "Default") const { this->Print(cout, Option); };

		TObject * Find(const Char_t * VarName, UInt_t VarType = kVarOrWindowOrFunction) const;	// find variable
		TObject * First(UInt_t  VarType = kVarOrWindowOrFunction) const;							// get first entry
		TObject * After(TObject * VarPtr, UInt_t VarType = kVarOrWindow) const; 		// get next entry

		void SetInitialFctType(TObject * Fct, UInt_t FctType);

		inline UInt_t GetEntries() const { return(fLofVars.GetLast() + 1); };		// get number of entries
		inline TObject * Get(UInt_t Index) const { return(fLofVars[Index]); };	// get object at position index
		inline UInt_t IndexOf(TObject * VarObj) const { return(fLofVars.IndexOf(VarObj)); };	// get index
		inline void Replace(TObject * OldVar, TObject * NewVar) { fLofVars.AddAt(NewVar, IndexOf(OldVar)); };

		inline TObjArray * GetArray() { return(&fLofVars); };

		void Draw(Option_t * Option);					// draw windows

		void SetGlobalAddress();						// set global address gMrbLofUserVars

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fNofVars;					// total number of vars/windows
		TObjArray fLofVars; 			// list of variables
		TIterator * fIter;				// iterator

	ClassDef(TMrbLofUserVars, 1)	// [Utils] A list of user-defined variables/windows
};

#endif

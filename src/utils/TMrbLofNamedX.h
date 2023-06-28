#ifndef __TMrbLofNamedX_h__
#define __TMrbLofNamedX_h__
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
// Name:           utils/inc/TMrbLofNamedX.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbLofNamedX    -- a list of named indices
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLofNamedX.h,v 1.11 2009-04-02 11:55:52 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TObjArray.h"
#include "TString.h"
#include "TSystem.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofNamedX
// Purpose:        Define a list named indices
// Description:    Class to manage a list of TMrbNamedX objects.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLofNamedX: public TObjArray {
// the following doesnt work with CINT, wait for ROOT 6 (no more CINT)
//	using TCollection::Print;
	public:
		enum EMrbFindMode		{	kFindExact		=	BIT(0), 	// needs exact matching of key names
									kFindUnique 	= 	BIT(1), 	// key names may be abbreviated uniquely
									kFindIgnoreCase =	BIT(2)		// ignore case while comparing key names
								};

		enum					{	kPatternBit 	=	BIT(31)	};
		enum					{	kIllIndexBit	=	BIT(30)	};

	public:
	
		TMrbLofNamedX(const Char_t * Name = NULL, Bool_t PatternMode = kFALSE); 	// default ctor

		TMrbLofNamedX(const SMrbNamedX * NamedX, Bool_t PatternMode = kFALSE); 		// explicit ctor
		TMrbLofNamedX(const SMrbNamedXShort * NamedX, Bool_t PatternMode = kFALSE);

		~TMrbLofNamedX() { this->Delete(); };										// dtor: delete heap objects

		inline void SetName(const Char_t * Name) { fName = Name; };
		inline const Char_t * GetName() const { return(fName); };
		inline void SetTitle(const Char_t * Title) { fTitle = Title; };
		inline const Char_t * GetTitle() const { return(fTitle); };
		inline void SetObject(const Char_t * Name, const Char_t * Title) { fName = Name; fTitle = Title; };

		inline void SetPatternMode(Bool_t Flag = kTRUE) { fPatternMode = Flag; };
		inline Bool_t IsPatternMode() const { return(fPatternMode); };

		void AddNamedX(const SMrbNamedX * NamedX);							// add one or more indices
		void AddNamedX(const SMrbNamedXShort * NamedX);					 	// ... (short names only)
		TMrbNamedX * AddNamedX(TMrbNamedX * NamedX); 						// add a single index to list (clone it)
		void AddNamedX(TMrbLofNamedX * LofNamedX); 	 						// append a list of indices
		TMrbNamedX * AddNamedX(Int_t Index, const Char_t * Name, const Char_t * Title = "", TObject * Object = NULL);
		void AddNamedX(const Char_t * NameString, const Char_t * Delim = ":", Bool_t PatternMode = kFALSE);
		void AddNamedX(const Char_t * NameString, Int_t StartIndex, Int_t Increment = 1, const Char_t * Delim = ":", Bool_t PatternMode = kFALSE);

																					// find an index by its name
		TMrbNamedX * FindByName(const Char_t * ShortName, UInt_t FindMode = TMrbLofNamedX::kFindExact) const;
		TMrbNamedX * FindByIndex(Int_t Index, Int_t Mask = 0xFFFFFFFF) const; 			// find an index
		
		UInt_t FindPattern(const Char_t * IndexString,								// find a compound index
							UInt_t FindMode = TMrbLofNamedX::kFindExact,
							const Char_t * Delim = ":") const;

		Bool_t FindByDialog(TString & Result, 									// dialog mode
							const TString & ShortName,
							UInt_t FindMode = TMrbLofNamedX::kFindExact,
							const Char_t * Delim = "") const;

		UInt_t CheckPattern(const Char_t * ClassNameL, const Char_t * Method, const Char_t * IndexString,	// check index string
							const SMrbNamedX * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);

		inline UInt_t CheckPatternShort(const Char_t * ClassNameL, const Char_t * Method, const Char_t * IndexString,
							const SMrbNamedXShort * NamedX = NULL,
							UInt_t Mode = TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase) {
			if (NamedX != NULL) this->AddNamedX(NamedX);
			return(this->CheckPattern(ClassNameL, Method, IndexString, NULL, Mode));
		};

		const Char_t * Pattern2String(TString & IndexString, UInt_t Pattern, const Char_t * Delim = ":") const;

		UInt_t GetMask() const;											// return mask of all bits in list

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(Option_t * Wildcard, Option_t * Option) const { TCollection::Print(Wildcard, Option); }
		void Print(ostream & Out, const Char_t * Prefix = "", UInt_t Mask = 0xffffffff) const;	// show list of indices
		inline void Print() const { Print(cout, "", 0xffffffff); };

		void PrintNames(ostream & Out = cout, 						// print names in one line
						const Char_t * Prefix = "",
						UInt_t Mask = 0xffffffff,
						Bool_t CrFlag = kTRUE) const;

		inline virtual void Sort(Int_t UpTo) { TObjArray::Sort(UpTo); };
		void Sort(Bool_t SortByName = kTRUE);						// sort entries by name or index
		inline Bool_t IsSorted() const { return(fIsSorted); };
		inline Bool_t IsSorted(Bool_t & ByName) { ByName = fIsSortedByName; return(fIsSorted); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t fPatternMode;
		Bool_t fIsSorted;
		Bool_t fIsSortedByName;
		TString fName;
		TString fTitle;

	ClassDef(TMrbLofNamedX, 1)		// [Utils] A list of named indices
};

#endif

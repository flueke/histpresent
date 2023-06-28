#ifndef __TMrbTemplate_h__
#define __TMrbTemplate_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbTemplate.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbTemplate     -- decode templates
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbTemplate.h,v 1.13 2007-07-27 11:17:23 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "Rtypes.h"

#include "TObject.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate
// Purpose:        Read & decode templates
// Description:    Provides a means to decode templates.
//                 Templates are considered to be pieces of program code
//                 marked with special tags. Code may be expanded at these tags
//                 according to the needs of the calling program.
//                 Template code may also be stored in memory to speed up access.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTemplate : public TObject {

	public:
		enum EMrbTagWordStatus	{	kTagError	 	=	BIT(0),	// some error occurred
									kNoTag			=	BIT(1),	// line has no tag
									kTagOK			=	BIT(2),	// tag is ok
									kTagNotFound	=	BIT(3),	// tag name not found
									kEOF			=	BIT(4)	// end of template file
								};

	public:
		TMrbTemplate(); 							// default ctor
		~TMrbTemplate() {						 	// dtor: delete heap objects
			this->Close();
			fCodeBuffer.Delete();
			fExpansionBuffer.Delete();
			fLofCodeSegments.Delete();
		};

		TMrbTemplate(const TMrbTemplate &) : TObject() {};		// default copy ctor

		Bool_t Open(const Char_t * TemplateFile, TMrbLofNamedX * LofTagWords);	// open template file
		inline void Close() { if (fIsActive)	{			// close
													fTemplStream.close();
													fTemplStream.clear();
													fIsActive = kFALSE;
												}
							};

		TMrbNamedX * Next(TString & Line, Bool_t ForceNextLine = kTRUE);	// decode next line
		TString & Encode(TString & Line, const Char_t * TagReplacement);	// encode line by replacing the tagword

		Bool_t InitializeCode(const Char_t * Prefix = NULL);				// initialize code for expansion

		Bool_t Substitute(const Char_t * ArgName, const Char_t * ArgValue);	// substitute arguments
		Bool_t Substitute(const Char_t * ArgName, Int_t ArgValue, Int_t ArgBase = 10);
		Bool_t Substitute(const Char_t * ArgName, Double_t ArgValue);

		Bool_t ExpandPathName();			// expand shell vars (like TSystem::ExpandPathName())

		Bool_t WriteCode(ostream & Out);									// write code buffer to output stream
		const Char_t * CopyCode(TString & CodeString, const Char_t * Seperator = " ");	// copy code to string
		inline Bool_t HasCode() const { return(fCodeBuffer.First() != NULL); };	// kTRUE if code exists
		inline Bool_t IsExpanded() const { return(fExpansionBuffer.First() != NULL); };	// kTRUE if expansion exists

		Bool_t ReadCodeFromFile(const Char_t * TemplateFile, TMrbLofNamedX * LofTagWords);	// read template code from file and store it in a TList
		Bool_t FindCode(Int_t TagIndex);			// find code in memory by its tag id and copy it to code buffer

		Bool_t PrintCode() const;							// output code to stdout

		inline EMrbTagWordStatus Status() const { return(fTagStatus); };			// tag word status
		inline Bool_t IsError() const { return((fTagStatus & (kTagError | kTagNotFound)) != 0); };
		inline Bool_t IsEof() const { return((fTagStatus & kEOF) != 0); };
		inline Bool_t IsOk() const { return((fTagStatus & (kNoTag | kTagOK)) != 0); };

		inline Bool_t HasOtherTags() const { return(fHasOtherTags); };
		
		inline TMrbLofNamedX * GetTagWords() { return fLofTagWords; };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t TestIfBeginOrEndOfCode();		// test if %%BeginOfCode%%/%%EndOfCode%%
		TObjString * TestIfInCodeSegment(TObjString * Code, Bool_t & InSegmentFlag, TString & Label, Bool_t & ErrorFlag);	// test if %%Begin()%%/%%End()%%

	protected:
		Bool_t fIsActive;			// flag to store open status
		Bool_t fHasOtherTags;		// kTRUE if line has some more tags pending
		Bool_t fInIfClause;         // kTRUE if inside %%IF_xxx%%, kFALSE if inside %%IFNOT_xxx%% or %%ELSE_xxx%%
		Bool_t fVerbose;			// kTRUE if verbose

		TString fTemplateFile; 		// name of template file
		ifstream fTemplStream;		// stream where to read data from
		Int_t fLineCount;			// line count

		TString fOrigLine;	 		// original data
		TString fExpandedLine;		// original data with tag words replaced
		Int_t fTagStart; 			// where possible tagword starts
		Int_t fTagEnd;				// where tagword ends
		EMrbTagWordStatus fTagStatus;	// current status bits
		TMrbNamedX fTag;				// current tag

		TMrbString fPrefix;			// prefix %XX%

		TList fCodeBuffer;			// buffer to store expandable code
		TList fExpansionBuffer;		// buffer to store code after expansion
		TList fLofCodeSegments;		// a list of codes copied to from file to memory

		TMrbLofNamedX * fLofTagWords;  // list of tag words

	ClassDef(TMrbTemplate, 0)		// [Utils] Template: a piece of text/code with embedded parameters
};

#endif

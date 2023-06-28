//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbTemplate.cxx
// Purpose:        MARaBOU utilities: Decode a template file
// Description:    Implements class methods to decode templates
//                 Templates are considered to be pieces of program code
//                 marked with special tags. Code may be expanded at these tags
//                 according to the needs of the calling program.
//                 Template code may also be stored in memory to speed up access.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbTemplate.cxx,v 1.14 2010-12-14 14:18:04 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TObjArray.h"
#include "TMrbTemplate.h"
#include "TMrbLogger.h"
#include "SetColor.h"

ClassImp(TMrbTemplate)

extern TMrbLogger * gMrbLog;			// access to logging system

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate
// Purpose:        Read & decode templates
// Arguments:      --
// Description:    Provides a means to decode templates.
//                 Templates are considered to be pieces of program code
//                 marked with special tags. Code may be expanded at these tags
//                 according to the needs of the calling program.
//                 Template code may also be stored in memory to speed up access.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

TMrbTemplate::TMrbTemplate() {
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fIsActive = kFALSE;
}

Bool_t TMrbTemplate::Open(const Char_t * TemplateFile, TMrbLofNamedX * LofTagWords) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Open
// Purpose:        Open a template file
// Arguments:      TString & TemplateFile        -- name of template file
//                 TMrbLofNamedX * LofTagWords  -- list of tag words
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a given template file.
//                 The list of tag words consists of pairs (key,value)
//                 describing possible tag word inserts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fIsActive) {
		gMrbLog->Err() << "Can't open " << TemplateFile << "- template already active" << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		return(kFALSE);
	}

//	fTemplStream.clear();
	fTemplStream.open(TemplateFile, ios::in);
	if (!fTemplStream.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << TemplateFile << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		fIsActive = kFALSE;
		return(fIsActive);
	}
	fIsActive = kTRUE;

	fTemplateFile = TemplateFile;			// save name of template file
	fLofTagWords = LofTagWords;
	fLineCount = 0;

	fVerbose = gEnv->GetValue("TMrbTemplate.VerboseMode", kFALSE);

	return(fIsActive);
}

TMrbNamedX * TMrbTemplate::Next(TString & Line, Bool_t ForceNextLine) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Next
// Purpose:        Read next line and decode tag word if present
// Arguments:      TString & Line           -- where to put original line
//                 Bool_t ForceNextLine     -- force read-in of a new line
// Results:        TMrbNamedX * TagWord        -- tag word
// Exceptions:
// Description:    Reads line by line and decodes tag words.
//                 Tag word format is %%TAG_WORD%%
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	Int_t lng;
	Int_t startIdx;

	if (ForceNextLine || !fHasOtherTags) {		// next line to be read
		Line.Remove(0);							// nothing read so far
		fTagStatus = TMrbTemplate::kNoTag;				// no tag word present

		fCodeBuffer.Delete();					// remove existing code segments
		fExpansionBuffer.Delete();				// and their expansion

		fHasOtherTags = kFALSE;

		if (!fIsActive) {
			gMrbLog->Err() << "Template not active" << endl;
			gMrbLog->Flush(this->ClassName(), "Next");
			fTagStatus = kTagError;		// error - template not open
			fTag.Set(-1, "NoTag");
			return(NULL);
		}
	
		fOrigLine.ReadLine(fTemplStream, kFALSE);
		if (fTemplStream.eof()) {
			this->Close();							// end of template file
			fTagStatus = TMrbTemplate::kEOF;
			fTag.Set(-1, "NoTag");
			return(NULL);
		}

		fTagStart = fTagEnd = -1;				// no tag word

		fLineCount++;							// count this line

		if (!TestIfBeginOrEndOfCode()) return(NULL); 	// test if begin/end of code

		Line = fOrigLine;						// pass original data to user
		fExpandedLine = fOrigLine;				// store a copy for further expansion
		startIdx = 0;
	} else {									// continue with current line
		startIdx = fTagEnd + 2;
	}

	fTagStart = fExpandedLine.Index("%%", startIdx); 		// search for %%TAG_WORD%%
	if (fTagStart == -1) {
		fTagStart = fTagEnd = -1;			// line doesn't contain tags
		fTagStatus = TMrbTemplate::kNoTag;
		fHasOtherTags = kFALSE;
		fTag.Set(-1, "NoTag");
		return(NULL);
	}

	fTagEnd = fExpandedLine.Index("%%", fTagStart + 2);	// search for end of tag
	if (fTagEnd == -1) {
		gMrbLog->Err() << fTemplateFile << " (line " << fLineCount << ") - tag delimiter missing" << endl;
		gMrbLog->Flush(this->ClassName(), "Next");
		fTagStart = fTagEnd = -1;	 		// error
		fTagStatus = kTagError;
		fHasOtherTags = kFALSE;
		fTag.Set(-1, "NoTag");
		return(NULL);
	}

	fTagStart += 2; 			// points now behind leading %%
	fTagEnd--;					// points before trailing %%
	lng = fTagEnd - fTagStart + 1;

	TString subString = fExpandedLine(fTagStart, lng);
	if ((kp = fLofTagWords->FindByName(subString.Data(), TMrbLofNamedX::kFindExact)) == NULL) {
		gMrbLog->Err()	<< fTemplateFile << " (line " << fLineCount << ") - tag word %%"
				<< fExpandedLine(fTagStart, lng) << "%% not found" << endl;
		gMrbLog->Flush(this->ClassName(), "Next");

		fTagStart = fTagEnd = -1;
		fTagStatus = kTagNotFound;
		fHasOtherTags = kFALSE;
		fTag.Set(-1, "NoTag");
		return(NULL);
	}

	fHasOtherTags = (fExpandedLine.Index("%%", fTagEnd + 2) != -1);		// check if more tags in this line

	fTagStatus = kTagOK;
	fTag.Set(kp);				// store current tag;
	return(kp); 	 			// tag word ok
}

Bool_t TMrbTemplate::TestIfBeginOrEndOfCode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::TestIfBeginOrEndOfCode
// Purpose:        Test if line is special
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if line starts with a special tag
//                      %%BeginOfCode%%        -- begin of expandable code
//                      %%EndOfCode%%          -- end of code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;

	TRegexp rx("%%BeginOfCode%%");
	if (fOrigLine.Index(rx, 0) == 0) {
		rx = TRegexp("%%EndOfCode%%");
		while (kTRUE) {
			fOrigLine.ReadLine(fTemplStream, kFALSE);
			if (fTemplStream.eof()) {
				Close();							// end of template file
				gMrbLog->Err() << "Unexpected EOF within code segment" << endl;
				gMrbLog->Flush(this->ClassName(), "TestIfBeginOrEndOfCode");
				fTagStatus = TMrbTemplate::kTagError;		// error - template not open
				return(kFALSE);
			}
			fLineCount++;							// count this line
			code = new TObjString();				
			if (fOrigLine.Index(rx, 0) != 0) {
				code->SetString((Char_t *) fOrigLine.Data());
				fCodeBuffer.Add((TObject *) code);				
			} else {
				delete code;
				break;
			}
		}
		fOrigLine.ReadLine(fTemplStream, kFALSE);
		if (!fTemplStream.eof()) fLineCount++;
	}
	return(kTRUE);
}

TString & TMrbTemplate::Encode(TString & Line, const Char_t * TagReplacement) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Encode
// Purpose:        Replace tag word by specified text
// Arguments:      TString & Line            -- where to put encoded line
//                 Char_t * TagReplacement   -- replacement
// Results:        TString & EncodedLine     -- original line with tag replaced
// Exceptions:
// Description:    Encode original text with some tag replacement.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fTagStart == -1) {
		Line = fExpandedLine;		// pass original data again
	} else {
		Line = fExpandedLine(0, fTagStart - 2)
					+ TagReplacement
					+ fExpandedLine(fTagEnd + 3, fExpandedLine.Length());	// return encoded data
	}
	return(Line);
}

Bool_t TMrbTemplate::Substitute(const Char_t * ArgName, const Char_t * ArgValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Substitute
// Purpose:        Substitute an argument in the code buffer
// Arguments:      Char_t * ArgName            -- argument name, must start with "$..."
//                 Char_t * ArgValue           -- replacement
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Replace an argument by its value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;

	if (!HasCode()) {
		gMrbLog->Err() << "%%" << fTag.GetName() << "%%: Code buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
		return(kFALSE);
	}

	if (fVerbose) {
		gMrbLog->Out()	<< "%%" << fTag.GetName() << "%%: " << ArgName << " = " << ArgValue << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
	}

	code = (TObjString *) fExpansionBuffer.First();
	while (code) {
		code->String().ReplaceAll(ArgName, ArgValue);
		code = (TObjString *) fExpansionBuffer.After((TObject *) code);
	}
	return(kTRUE);
}

Bool_t TMrbTemplate::Substitute(const Char_t * ArgName, Int_t ArgValue, Int_t ArgBase) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Substitute
// Purpose:        Substitute an argument in the code buffer
// Arguments:      Char_t * ArgName            -- argument name, must start with "$..."
//                 Int_t ArgValue              -- replacement
//                 Int_t ArgBase               -- numerical base
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Replace an argument by its value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * fmt;
	switch (ArgBase) {
		case 8: fmt = "%#o"; break;
		case 16: fmt = "%#x"; break;
		case 10:
		default: fmt = "%d"; break;
	}
	return(Substitute(ArgName, Form(fmt, ArgValue)));
}

Bool_t TMrbTemplate::Substitute(const Char_t * ArgName, Double_t ArgValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::Substitute
// Purpose:        Substitute an argument in the code buffer
// Arguments:      Char_t * ArgName            -- argument name, must start with "$..."
//                 Double_t ArgValue           -- replacement
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Replace an argument by its value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(Substitute(ArgName, Form("%.1f", ArgValue)));
}

Bool_t TMrbTemplate::ExpandPathName() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::ExpandPathName
// Purpose:        Expand evironment variables provided by shell
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Expands shell vars
//                 ATTENTION: has to be called *AFTER* any call to Substitute()!
//                            otherwise $-args will be catched by this method
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;

	if (!HasCode()) {
		gMrbLog->Err() << "%%" << fTag.GetName() << "%%: Code buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "ExpandPathName");
		return(kFALSE);
	}

	code = (TObjString *) fExpansionBuffer.First();
	while (code) {
		gSystem->ExpandPathName(code->String());
		code = (TObjString *) fExpansionBuffer.After((TObject *) code);
	}
	return(kTRUE);
}

Bool_t TMrbTemplate::WriteCode(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::WriteCode
// Purpose:        Output the code buffer
// Arguments:      ostream &n Out        -- output stream
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Write expanded code to output stream
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;

	if (!IsExpanded()) {
		gMrbLog->Err()	<< "%%" << fTag.GetName()
						<< "%%: Expansion buffer is empty (prefix = \"" << fPrefix << "\")" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteCode");
		return(kFALSE);
	} else if (fVerbose) {
		gMrbLog->Out()	<< "%%" << fTag.GetName() << "%%: prefix = \"" << fPrefix << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteCode");
	}
	code = (TObjString *) fExpansionBuffer.First();
	while (code) {
		Out 	<< code->String() << endl;
		code = (TObjString *) fExpansionBuffer.After((TObject *) code);
	}
	return(kTRUE);
}

const Char_t * TMrbTemplate::CopyCode(TString & CodeString, const Char_t * Separator) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::CopyCode
// Purpose:        Copy code buffer to string
// Arguments:      TString & CodeString        -- where to copy code
//                 const Char_t * Separator    -- separator between lines
// Results:        Char * CodeString           -- resulting string
// Exceptions:
// Description:    Copy expanded code to string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;

	if (!IsExpanded()) {
		gMrbLog->Err()	<< "%%" << fTag.GetName()
						<< "%%: Expansion buffer is empty (prefix = \"" << fPrefix << "\")" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyCode");
		return(NULL);
	} else if (fVerbose) {
		gMrbLog->Out()	<< "%%" << fTag.GetName() << "%%: prefix = \"" << fPrefix << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyCode");
	}

	CodeString.Resize(0);

	code = (TObjString *) fExpansionBuffer.First();
	while (code) {
		CodeString += code->String();
		CodeString += Separator;
		code = (TObjString *) fExpansionBuffer.After((TObject *) code);
	}
	return(CodeString.Data());
}

Bool_t TMrbTemplate::InitializeCode(const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::InitializeCode
// Purpose:        Fill expansion buffer with virgin code
// Arguments:      Char_t * Prefix   -- select only lines starting with prefix
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Copies specified lines from copy buffer to expansion buffer.
//                 How to mark code segments using prefixes:
//                 (1)        %XYZ% <line of code>      take this line
//                                                      if prefix %XYZ% is set
//                 (2)        %%Begin(XYZ)%%            take code segment
//                               <code segment>         within Begin/End
//                            %%End(XYZ)%%              if prefix %XYZ% has been set
//
//                 How to initialize code segments using prefixes:
//                            Initialize();             initialize any lines/segments
//                            Initialize("");           initialize any lines/segments
//                                                      WITHOUT prefixes
//                            Initialize("%XYZ%") ;     initialize lines/segments
//                                                      marked with %XYZ%
//                            Intialize("%XYZ%:%ABC%"); initialize lines/segments
//                                                      marked with %XYZ% OR %ABC%
//                            Initialize("~%XYZ%");     initialize any lines/segments
//                                                      NOT marked with %XYZ%                         
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;
	TObjString * xcode;
	TString lineOfCode;
	Bool_t hasPrefix;
	Bool_t lineIsValid;
	Bool_t errorFlag;

	TString prefix;
	TString pref;

	Bool_t inCodeSegment;
	TString label;
			
	if (!HasCode()) return(kFALSE);

	fExpansionBuffer.Delete();

	if (Prefix == NULL) {
		hasPrefix = kFALSE;
		prefix = "";
		fPrefix = "none";
	} else if (*Prefix == '\0') {
		hasPrefix = kTRUE;
		prefix = "~%";
		fPrefix = prefix;
	} else {
		hasPrefix = kTRUE;
		prefix = Prefix;
		fPrefix = prefix;
	}

	if (fVerbose) {
		gMrbLog->Out()	<< "%%" << fTag.GetName() << "%%: prefix = \"" << fPrefix << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeCode");
	}

	code = (TObjString *) fCodeBuffer.First();
	inCodeSegment = kFALSE;
	while (code) {
		if (hasPrefix) {
			lineIsValid = kFALSE;
			code = this->TestIfInCodeSegment(code, inCodeSegment, label, errorFlag);
			if (errorFlag) {
				gMrbLog->Err()	<< "Syntax error in Begin()/End() phrase - " << label << endl;
				gMrbLog->Flush(this->ClassName(), "InitializeCode");
				return(kFALSE);
			}
			if (code == NULL) break;
			lineOfCode = code->String();
			if (inCodeSegment) lineOfCode.Prepend(label.Data());
			Int_t from = 0;
			while (prefix.Tokenize(pref, from, ":")) {
				Bool_t takeIt = kTRUE;
				if (pref(0) == '~') {
					takeIt = kFALSE;
					pref = pref(1,1000);
				}
				pref.Prepend("^");
				TRegexp rx(pref.Data());
				if (lineOfCode.Index(rx) == 0) {
					lineOfCode(rx) = "";
					lineIsValid = takeIt;
				} else {
					lineIsValid = takeIt ? kFALSE : kTRUE;
				}
				if (lineIsValid) break;
			}
		} else {
			lineOfCode = code->String();
			lineIsValid = kTRUE;
		}
		if (lineIsValid) {
			TRegexp rx("^%.*%");
			if (lineOfCode.Index(rx) == 0) {
				lineOfCode(rx) = "";
				if (lineOfCode.Length() > 0) {
					xcode = new TObjString(lineOfCode);
					fExpansionBuffer.Add((TObject *) xcode);
				}
			} else {
				xcode = new TObjString(lineOfCode);
				fExpansionBuffer.Add((TObject *) xcode);
			}
		}
		code = (TObjString *) fCodeBuffer.After((TObject *) code);
	}
	if (inCodeSegment) {
		gMrbLog->Err()	<< "Syntax error in Begin()/End() phrase - " << label << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeCode");
		return(kFALSE);
	}
	return(kTRUE);
}

TObjString * TMrbTemplate::TestIfInCodeSegment(TObjString * Code, Bool_t & InSegmentFlag, TString & Label, Bool_t & ErrorFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::TestIfInCodeSegment
// Purpose:        Test if line is begin or end of a code segement
// Arguments:      TObjString * Code        -- pointer to current line of code
//                 Bool_t & InSegmentFlag   -- kTRUE if within code segment
//                 TString & Label          -- prefix line is labelled with
//                 Bool_t & ErrorFlag       -- kTRUE/kFALSE
// Results:        TObjString * Code        -- pointer to (next) line
// Exceptions:
// Description:    Tests if line is begin or end of a code segment
//                      %%Begin(name)%%        -- begin of segment <name>
//                      %%End(name)%%          -- end of code segment <name>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t isBegin;
	TString lb;
	TString lineOfCode;
	Int_t np1, np2;

	while (Code) {
		lineOfCode = Code->String();
		if (lineOfCode.Index("%%Begin(", 0) == 0) {
			isBegin = kTRUE;
			np1 = sizeof("%%Begin(") - 1;
		} else if (lineOfCode.Index("%%End(", 0) == 0) {
			isBegin = kFALSE;
			np1 = sizeof("%%End(") - 1;
		} else {
			ErrorFlag = kFALSE;
			return(Code);
		}

		np2 = lineOfCode.Index(")%%", 0);
		if (np2 == -1 || np2 <= np1) {
			Label = lineOfCode;
			ErrorFlag = kTRUE;
			return(Code);
		}
		lb = "%";
		lb += lineOfCode(np1, np2 - np1);
		lb += "%";
		if (isBegin) {
			if (InSegmentFlag) {
				Label = lineOfCode;
				ErrorFlag = kTRUE;
				return(Code);
			}
			Label = lb;
			InSegmentFlag = kTRUE;
			Code = (TObjString *) fCodeBuffer.After((TObject *) Code);
			if (Code == NULL) {
				Label = lineOfCode;
				ErrorFlag = kTRUE;
			}
		} else {
			if (!InSegmentFlag || lb.CompareTo(Label.Data()) != 0) {
				Label = lineOfCode;
				ErrorFlag = kTRUE;
				return(Code);
			}
			InSegmentFlag = kFALSE;
			Code = (TObjString *) fCodeBuffer.After((TObject *) Code);
			ErrorFlag = kFALSE;
		}
	}
	return(Code);
}

Bool_t TMrbTemplate::PrintCode() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::PrintCode
// Purpose:        Output code to stdout
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs both buffers - code as well as expansion - to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjString * code;
	Int_t lineNo;

	lineNo = 0;
	if (HasCode()) {
		cout	<< this->ClassName() << "::PrintCode():Contents of CODE buffer - " << endl;
		code = (TObjString *) fCodeBuffer.First();
		while (code) {
			lineNo++;
			cout	<< "\t" << lineNo << ":\t" << code->String() << endl;
			code = (TObjString *) fCodeBuffer.After((TObject *) code);
		}
	} else {
		gMrbLog->Err() << "%%" << fTag.GetName() << "%%: Code buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintCode");
	}

	lineNo = 0;
	if (IsExpanded()) {
		cout	<< this->ClassName() << "::PrintCode():Contents of EXPANSION buffer - " << endl;
		code = (TObjString *) fExpansionBuffer.First();
		while (code) {
			lineNo++;
			cout	<< "\t" << lineNo << ":\t" << code->String() << endl;
			code = (TObjString *) fExpansionBuffer.After((TObject *) code);
		}
	} else {
		gMrbLog->Err() << "%%" << fTag.GetName() << "%%: Expansion buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintCode");
	}
	return(kTRUE);
}

Bool_t TMrbTemplate::ReadCodeFromFile(const Char_t * TemplateFile, TMrbLofNamedX * TagWords) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::ReadCodeFromFile
// Purpose:        Read code from file and store it in memory
// Arguments:      TString & TemplateFile     -- name of template file
//                 TMrbLofNamedX * TagWords   -- list of tag words
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads code from given template file and stores it in
//                 list fLofCodeSegments.
//                 Tag words have to be preceeded by a code segment:
//                      %%BeginOfCode%% ... %%EndOfCode%%
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TList * codeList;
	TObjString * code;
	TObjString * xcode;
	TString line;
	TMrbNamedX * tagWord;

	if (!this->Open(TemplateFile, TagWords)) return(kFALSE);

	fLofCodeSegments.Delete();

	for (;;) {
		tagWord = this->Next(line);
		if (this->IsEof()) break;
		if (this->IsError()) continue;
		if (this->Status() & TMrbTemplate::kNoTag) continue;
		if (this->HasCode()) {
			codeList = new TList();
			codeList->SetUniqueID(tagWord->GetIndex());
			code = (TObjString *) fCodeBuffer.First();
			while (code) {
				xcode = new TObjString(code->String());
				codeList->Add((TObject *) xcode);
				code = (TObjString *) fCodeBuffer.After((TObject *) code);
			}
			fLofCodeSegments.Add((TObject *) codeList);
		} else {
			gMrbLog->Err() << "%%" << tagWord->GetName() << "%%: Tag without code (line " << fLineCount << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadCodeFromFile");
		}
	}
	this->Close();
	return(kTRUE);
}

Bool_t TMrbTemplate::FindCode(Int_t TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTemplate::FindCode
// Purpose:        Find code by its tag
// Arguments:      Int_t TagIndex       -- tag index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Searches for a given tag index in the list of code segments.
//                 If successful copies code to the code buffer for further
//                 expansion.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TList * codeList;
	TObjString * code;
	TObjString * xcode;

	fCodeBuffer.Delete();
	fExpansionBuffer.Delete();

	codeList = (TList *) fLofCodeSegments.First();
	while (codeList) {
		if (codeList->GetUniqueID() == (UInt_t) TagIndex) {
			code = (TObjString *) codeList->First();
			while (code) {
				xcode = new TObjString(code->String());
				fCodeBuffer.Add((TObject *) xcode);
				code = (TObjString *) codeList->After((TObject *) code);
			}
			return(kTRUE);
		}
		codeList = (TList *) fLofCodeSegments.After((TObject *) codeList);
	}
	return(kFALSE);
}

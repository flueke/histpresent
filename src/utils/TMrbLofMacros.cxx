//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbLofMacros.cxx
// Purpose:        MARaBOU utilities: A list of root macros
// Description:    Implements class methods to manage root macros
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbLofMacros.cxx,v 1.17 2008-08-18 08:18:57 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TMrbLofMacros.h"
#include "TMrbSystem.h"
#include "TMrbString.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbLofMacros)

const SMrbNamedXShort kMrbMacroLofEnvNames[] =
							{
								{TMrbLofMacros::kMrbMacroName,  			"Name"				},
								{TMrbLofMacros::kMrbMacroTitle, 			"Title" 			},
								{TMrbLofMacros::kMrbMacroAuthor,	 		"Author"	 		},
								{TMrbLofMacros::kMrbMacroKeyWords,	 		"KeyWords"	 		},
								{TMrbLofMacros::kMrbMacroPath, 				"Path"				},
								{TMrbLofMacros::kMrbMacroWidth, 			"Width" 			},
								{TMrbLofMacros::kMrbMacroAclic, 			"Aclic" 			},
								{TMrbLofMacros::kMrbMacroModify, 			"Modify" 			},
								{TMrbLofMacros::kMrbMacroGuiPtrMode,		"GuiPtrMode"	 	},
								{TMrbLofMacros::kMrbMacroUserStart,			"UserStart"		 	},
								{TMrbLofMacros::kMrbMacroRcFile, 			"RcFile" 			},
								{TMrbLofMacros::kMrbMacroNofArgs, 			"NofArgs"			},
								{TMrbLofMacros::kMrbMacroArgName, 			"Arg.Name"			},
								{TMrbLofMacros::kMrbMacroArgTitle,  		"Arg.Title"			},
								{TMrbLofMacros::kMrbMacroArgType, 			"Arg.Type"			},
								{TMrbLofMacros::kMrbMacroArgEntryType, 		"Arg.EntryType"		},
								{TMrbLofMacros::kMrbMacroArgNofEntryFields, "Arg.NofEntryFields"},
								{TMrbLofMacros::kMrbMacroArgEntryWidth, 	"Arg.Width" 		},
								{TMrbLofMacros::kMrbMacroArgDefaultValue, 	"Arg.Default"		},
								{TMrbLofMacros::kMrbMacroArgValues, 		"Arg.Values"		},
								{TMrbLofMacros::kMrbMacroArgAddLofValues, 	"Arg.AddLofValues"	},
								{TMrbLofMacros::kMrbMacroArgLowerLimit, 	"Arg.LowerLimit"	},
								{TMrbLofMacros::kMrbMacroArgUpperLimit, 	"Arg.UpperLimit"	},
								{TMrbLofMacros::kMrbMacroArgIncrement, 		"Arg.Increment" 	},
								{TMrbLofMacros::kMrbMacroArgBase, 			"Arg.Base"			},
								{TMrbLofMacros::kMrbMacroArgPrecision, 		"Arg.Precision"		},
								{TMrbLofMacros::kMrbMacroArgOrientation, 	"Arg.Orientation"	},
								{TMrbLofMacros::kMrbMacroArgNofCL, 			"Arg.NofCL" 		},
								{TMrbLofMacros::kMrbMacroArgExecFunct, 		"Arg.ExecFunct" 	},
								{TMrbLofMacros::kMrbMacroArgState,	 		"Arg.State" 		},
								{0, 										NULL				}
							};

extern TMrbLogger * gMrbLog;			// access to message logging

TMrbLofMacros::TMrbLofMacros() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacros
// Purpose:        Manage a list of ROOT macros
// Arguments:      
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fPath = gEnv->GetValue("Root.MacroPath", ".:$HOME/rootmacros:$MARABOU/macros");
	gSystem->ExpandPathName(fPath);

	fLofEnvNames.SetName("Environment Names");
	fLofEnvNames.AddNamedX(kMrbMacroLofEnvNames);
}

TMrbLofMacros::TMrbLofMacros(const Char_t * Path) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacros
// Purpose:        Manage a list of ROOT macros
// Arguments:      Char_t * Path     -- macro path
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fPath = Path;
	fPath = fPath.Strip(TString::kBoth);
	if (fPath.Length() == 0) fPath = gEnv->GetValue("Root.MacroPath", ".:$HOME/rootmacros:$MARABOU/macros");
	gSystem->ExpandPathName(fPath);

	fLofFilePatterns.Delete();
	fLofEnvNames.Delete();
	fLofEnvNames.SetName("Environment Names");
	fLofEnvNames.AddNamedX(kMrbMacroLofEnvNames);
}

Bool_t TMrbLofMacros::AddMacro(const Char_t * MacroName, const Char_t * Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacros::AddMacro
// Purpose:        Add a macro to the list
// Arguments:      Char_t * MacroName    -- macro name
//                 Char_t * Key          -- key word
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a new macro to the list. Macro name may be wildcarded.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString macroName;
	TString macroPath;
	TString mdfName;
	TString mdfPath;
	Bool_t searchIt;
	TMrbNamedX * macroIdx;

	TMrbSystem uxSys;
	TObjArray lofFiles;
	TObjArray lofMdefs;
	Int_t nofFiles;

	macroPath = MacroName;
	gSystem->ExpandPathName(macroPath);
	ifstream mf;

	uxSys.GetBaseName(macroName, macroPath.Data());
	if (macroName.CompareTo(macroPath.Data()) == 0) {
		searchIt = kTRUE;
	} else if (macroName.Index("./", 0) == 0) {
		searchIt = kFALSE;
		macroName = macroName(2, macroName.Length() - 2);
	} else {
		searchIt = kFALSE;
	}

	if (macroName.Index(".C", 0) == -1) {
		gMrbLog->Err() << macroName << ": Illegal extension - should be \".C\"" << endl;
		gMrbLog->Flush(this->ClassName(), "AddMacro");
		return(kFALSE);
	}

	if (macroPath.Index("?", 0) != -1) {
		gMrbLog->Err() << macroName << ": Illegal wild card - " << macroPath << endl;
		gMrbLog->Flush(this->ClassName(), "AddMacro");
		return(kFALSE);
	}

	if (macroPath.Index("*", 0) != -1) {
		lofFiles.Delete();
		nofFiles = uxSys.FindFile(lofFiles, macroName.Data(), fPath.Data());
		if (nofFiles == -1) {
			gMrbLog->Err() << "Can't add macro(s) - " << macroName << endl;
			gMrbLog->Flush(this->ClassName(), "AddMacro");
			return(kFALSE);
		}
		for (Int_t i = 0; i < nofFiles; i++) {
			lofMdefs.Delete();
			macroPath = ((TObjString *) lofFiles[i])->GetString();
			uxSys.GetBaseName(mdfName, macroPath.Data());
			mdfName.Resize(mdfName.Index(".C", 0));
			mdfName += ".mdf";
			if (uxSys.FindFile(lofMdefs, mdfName.Data(), fPath.Data()) == 1) {
				mdfPath = ((TObjString *) lofMdefs[0])->GetString();
				macroIdx = this->ProcessMacro(macroPath.Data(), mdfPath.Data(), kFALSE);
			} else {
				macroIdx = this->ProcessMacro(macroPath.Data(), NULL, kFALSE);
			}
			if (macroIdx == NULL) continue;
			if (this->CheckKey(macroIdx, Key)) this->AddNamedX(macroIdx);
		}
		fLofFilePatterns.Add(new TNamed(macroName.Data(), ""));
		return(kTRUE);
	} else {
		if (searchIt) {
			TString fileSpec;
			TMrbSystem ux;
			ux.Which(fileSpec, fPath, macroName.Data());
			if (!fileSpec.IsNull()) {
				macroPath = fileSpec;
			} else {
				gMrbLog->Err() << macroName << ": File not found, searched on " << fPath << endl;
				gMrbLog->Flush(this->ClassName(), "AddMacro");
				return(kFALSE);
			}
		} else if (gSystem->AccessPathName(macroPath.Data())) {
			gMrbLog->Err() << macroPath << ": File not found" << endl;
			gMrbLog->Flush(this->ClassName(), "AddMacro");
			return(kFALSE);
		}
		macroIdx = this->ProcessMacro(macroPath.Data(), NULL, kTRUE);
		if (macroIdx) {
			if (!this->CheckKey(macroIdx, Key)) return(kFALSE);
			this->AddNamedX(macroIdx);
			fLofFilePatterns.Add(new TNamed(macroName.Data(), ""));
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	}
}

void TMrbLofMacros::PrintMacro(const Char_t * MacroName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacro::PrintMacro
// Purpose:        Print macro data
// Arguments:      Char_t * MacroName  -- macro name
// Results:        --
// Exceptions:
// Description:    Outputs macro data to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;

	if (MacroName == NULL || *MacroName == '\0') {
		if (this->GetLast() >= 0) {
			macro = (TMrbNamedX *) this->First();
			while (macro) {
				this->PrintMacro(macro->GetName());
				macro = (TMrbNamedX *) this->After(macro);
			}
		} else {
			gMrbLog->Err() << "List is empty" << endl;
			gMrbLog->Flush(this->ClassName(), "PrintMacro");
		}
	} else {
		macro = this->FindByName(MacroName);
		if (macro) {
			macro->GetAssignedObject()->Print();
		} else {
			gMrbLog->Err() << "No such macro - " << MacroName << endl;
			gMrbLog->Flush(this->ClassName(), "PrintMacro");
		}
	}
}

TMrbNamedX * TMrbLofMacros::ProcessMacro(const Char_t * MacroPath, const Char_t * MdefPath, Bool_t TestFormat) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacro::ProcessMacro
// Purpose:        Process macro file
// Arguments:      Char_t * MacroPath     -- macro path
//                 Char_t * MdefPath      -- path of macro definition file
//                 Bool_t TestFormat      -- test for //+-Exec tags
// Results:        TMrbNamedX * MacroIdx  -- named index pointing to macro defs
// Exceptions:
// Description:    Decodes info data from macro file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;
	TString macroLine, envName, envVal;
	Bool_t envOk;
	Int_t lCnt;
	Bool_t insideExec;
	TMrbNamedX * macroIdx;
	TEnv * macroEnv = NULL;
	TMrbSystem uxSys;
	ifstream mf;

	TString macroName, macroPath, mdefName, mdefPath;

	macroPath = MacroPath;
	uxSys.GetBaseName(macroName, macroPath.Data());

	mf.open(macroPath.Data(), ios::in);
	if (!mf.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << macroName << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessMacro");
		return(NULL);
 	}

	insideExec = kFALSE;
	lCnt = 0;
	macroIdx = NULL;
	while (!mf.eof()) {
		lCnt++;
		macroLine.ReadLine(mf, kFALSE);
		macroLine = macroLine.Strip(TString::kBoth);
		if (macroLine.Index("//+Exec", 0) == 0) {
			if (insideExec) {
				gMrbLog->Err() << macroName << " (line " << lCnt << "): Out of phase - \"//+Exec\" unexpected" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessMacro");
				mf.close();
				if (macroIdx) delete macroIdx;
				return(NULL);
			} else {
				insideExec = kTRUE;
				macroIdx = new TMrbNamedX(this->GetLast() + 1, macroName.Data());
				macroEnv = new TEnv(".listOfMacros");
				macroEnv->SetValue("Path", macroPath.Data(), kEnvUser);
				macroIdx->AssignObject(macroEnv);
			}
		} else if (macroLine.Index("//-Exec", 0) == 0) {
			if (!insideExec) {
				gMrbLog->Err() << macroName << " (line " << lCnt << "): Out of phase - \"//-Exec\" unexpected" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessMacro");
				mf.close();
				if (macroIdx) delete macroIdx;
				return(NULL);
			} else {
				insideExec = kFALSE;
				break;
			}
		} else if (insideExec) {
			macroLine = macroLine(2, macroLine.Length() - 2);
			macroLine = macroLine.Strip(TString::kBoth);
			macroLine = macroLine.Strip(TString::kBoth, '\t');
			n = macroLine.Index(":", 0);
			if (n == -1) {
				gMrbLog->Err() << macroName << " (line " << lCnt << "): Syntax error - \":\" missing" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessMacro");
				mf.close();
				if (macroIdx) delete macroIdx;
				return(NULL);
			}
			envName = macroLine(0, n);
			envVal = macroLine(n + 1, macroLine.Length() - n - 1);
			envVal = envVal.Strip(TString::kBoth);
			envVal = envVal.Strip(TString::kBoth, '\t');
			if (envName.BeginsWith("Arg")) {
				TString en = envName(envName.Index(".", 0) + 1, 1000);
				envOk = this->CheckEnvName(en.Data(), kTRUE);
			} else {
				envOk = this->CheckEnvName(envName.Data(), kFALSE);
			}
			if (!envOk) {
				gMrbLog->Err() << macroName << " (line " << lCnt << "): Illegal name - " << envName << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessMacro");
				mf.close();
				if (macroIdx) delete macroIdx;
				return(NULL);
			}
			if (macroEnv->Lookup(envName.Data()) != NULL) {
				gMrbLog->Err() << macroName << " (line " << lCnt << "): \"" << envName << "\" multiply defined" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessMacro");
				mf.close();
				if (macroIdx) delete macroIdx;
				return(NULL);
			}
			macroEnv->SetValue(envName.Data(), envVal.Data(), kEnvUser);
		}
	}

	mf.close();
	if (insideExec) {
		gMrbLog->Err() << macroName << " (line " << lCnt << "): Out of phase - \"//-Exec\" missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessMacro");
		mf.close();
		if (macroIdx) delete macroIdx;
		return(NULL);
	}

	if (macroIdx == NULL && MdefPath != NULL) {
		mdefPath = MdefPath;
		macroIdx = new TMrbNamedX(this->GetLast() + 1, macroName.Data());
		macroEnv = new TEnv(mdefPath.Data());
		macroEnv->SetValue("Path", macroPath.Data(), kEnvUser);
		macroIdx->AssignObject(macroEnv);
	}

	if (TestFormat && macroIdx == NULL) {
		gMrbLog->Err() << macroName << ": No exec info found (\"//+Exec ... //-Exec\" missing)" << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessMacro");
		if (MdefPath != NULL) {
			gMrbLog->Err()	<< macroName << ": No macro definition file ("
							<< uxSys.GetBaseName(mdefName, mdefPath.Data()) << ") found" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMacro");
		}
		return(NULL);
	}

	if (macroIdx == NULL) return(NULL);
	if (!this->CheckMacro(macroIdx)) return(NULL);

	return(macroIdx);
}

Bool_t TMrbLofMacros::CheckKey(TMrbNamedX * Macro, const Char_t * Key) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacro::CheckKey
// Purpose:        Check if key words match
// Arguments:      TMrbNamedX * Macro     -- macro definition
//                 Char_t * Key           -- :-separated list of key words
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if key matches entries in macro header.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString key = Key;
	if (key.IsNull()) return(kTRUE);
	key.ToLower();
	TString macroKeys = ((TEnv *) Macro->GetAssignedObject())->GetValue("KeyWords", "");
	if (macroKeys.IsNull()) return(kFALSE);
	Int_t from = 0;
	TString mk;
	while (macroKeys.Tokenize(mk, from, ":")) {
		mk.ToLower();
		if (mk.CompareTo(key.Data()) == 0) return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbLofMacros::CheckMacro(TMrbNamedX * Macro) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacro::CheckMacro
// Purpose:        Check consistency
// Arguments:      TMrbNamedX * Macro     -- macro definition
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks macro arguments.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TEnv * macroEnv;
	TString argString;
	Int_t nofArgs;
	TMrbString argName0, argName;
	Int_t nofErrors;

	macroEnv = (TEnv *) Macro->GetAssignedObject();
	nofErrors = 0;

	argString = macroEnv->GetValue("Title", "");
	if (argString.Length() == 0) {
		gMrbLog->Err() << Macro->GetName() << ": Title missing" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckMacro");
		nofErrors++;
	}

	nofArgs = macroEnv->GetValue("NofArgs", -1);
	if (nofArgs == -1) {
		gMrbLog->Err() << Macro->GetName() << ": NofArgs missing" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckMacro");
		nofErrors++;
	}

	for (i = 1; i <= nofArgs; i++) {
		argName0 = "Arg";
		argName0 += i;
		argName = argName0 + ".Name";
		argString = macroEnv->GetValue(argName.Data(), "");
		if (argString.Length() == 0) {
			gMrbLog->Err() << Macro->GetName() << ": Arg"<< i << ".Name missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckMacro");
			nofErrors++;
		}
		argName = argName0 + ".Title";
		argString = macroEnv->GetValue(argName.Data(), "");
		if (argString.Length() == 0) {
			gMrbLog->Err() << Macro->GetName() << ": Arg" << i << ".Title missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckMacro");
			nofErrors++;
		}
		argName = argName0 + ".Type";
		argString = macroEnv->GetValue(argName.Data(), "");
		if (argString.Length() == 0) {
			gMrbLog->Err() << Macro->GetName() << ": Arg" << i << ".Type missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckMacro");
			nofErrors++;
		}
	}

	if (nofErrors > 0) {
		gMrbLog->Err() << Macro->GetName() << ": " << nofErrors << " error(s) detected" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckMacro");
		return(kFALSE);
	}

	return(kTRUE);
}

Bool_t TMrbLofMacros::CheckEnvName(const Char_t * EnvName, Bool_t IsArgEnv) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofMacro::CheckEnvName
// Purpose:        Check if environment name is legal
// Arguments:      Char_t * EnvName    -- environment name
//                 Bool_t IsArgEnv     -- kTRUE if used by arguments
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if a given environment name is legal.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString envName;

	if (IsArgEnv) {
		envName = "Arg.";
		envName += EnvName;
	} else {
		envName = EnvName;
	}
	return (fLofEnvNames.FindByName(envName.Data()) != NULL);
}

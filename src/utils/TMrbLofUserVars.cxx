//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbLofUserVars.cxx
// Purpose:        MARaBOU utilities: Provide user-defined variables and windows
// Description:    Implements class methods to handle variables and 1-dim windows
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbLofUserVars.cxx,v 1.11 2010-11-22 11:41:07 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TROOT.h"
#include "TRegexp.h"
#include "TFormula.h"
#include "TF1.h"
#include "TF2.h"
#include "TMrbLofUserVars.h"
#include "TMrbLogger.h"
#include "TMrbString.h"

#include "SetColor.h"

ClassImp(TMrbLofUserVars)

TMrbLofUserVars * gMrbLofUserVars = NULL;		// a list of all vars and windows defined so far
extern TMrbLogger * gMrbLog;					// access to loggin system

TMrbLofUserVars::TMrbLofUserVars(const Char_t * Name) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars
// Purpose:        Establish a list of user-defined variables/windows
// Arguments:      Char_t * Name           -- name of list
// Results:
// Exceptions:
// Description:    Instantiates a list to hold variables/windows defined
//                 by the user.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	SetName(Name);
	fNofVars = 0;
	fLofVars.Delete();
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
//	if (gROOT != NULL) gROOT->Append(this); 	// must be gROOT not gDirectory !!!
};

Bool_t TMrbLofUserVars::Append(TObject * VarObj) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::Append
// Purpose:        Add a variable or window to the list
// Arguments:      TObject * VarObj         -- address of var/wdw/fct object
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a now var/wdw/fct object to the list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString varName;

	if (fLofVars.FindObject(varName) != NULL) {
		gMrbLog->Err() << "Variable/window/function already defined - " << varName << endl;
		gMrbLog->Flush(this->ClassName(), "Append");
		return(kFALSE);
	}

	UInt_t id = VarObj->GetUniqueID();

	if (id & kIsVariable)			varName = ((TMrbVariable *) VarObj)->GetName();
	else if (id & kIsWindow2D)		varName = ((TMrbWindow2D *) VarObj)->GetName();
	else if (id & kIsWindow1D)		varName = ((TMrbWindow *) VarObj)->GetName();
	else if (id & kIsFunction)		varName = ((TFormula *) VarObj)->GetName();
	else return(kFALSE);

	fLofVars.Add(VarObj);
	fNofVars++;
	return(kTRUE);
}

Bool_t TMrbLofUserVars::Initialize(const Char_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::Initialize
// Purpose:        Reset all variables/windows to initial values
// Arguments:      Char_t * Option    -- option string: V(ariables), W(indows), or D(efault)
// Results:
// Exceptions:
// Description:    Reset to initial values. For vars and wdws only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX varOption;

	varOption.SetPatternMode();
	varOption.AddNamedX(kMrbVarWdwOptions);
	UInt_t vOpt = varOption.FindPattern(Option, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) {
		gMrbLog->Err() << this->GetName() << ": Illegal option(s) - " << Option << endl;
		gMrbLog->Flush(this->ClassName(), "Initialize");
		return(kFALSE);
	}

	TObject * varp = this->First(vOpt);
	while (varp) {
		if (varp->GetUniqueID() & kIsVariable)		((TMrbVariable *) varp)->Initialize();
		else if (varp->GetUniqueID() & kIsWindow)	((TMrbWindow *) varp)->Initialize();
		varp = this->After(varp, vOpt);
	}
	return(kTRUE);
}

void TMrbLofUserVars::Draw(const Char_t * /*Option*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::Draw
// Purpose:        Draw all windows in this list
// Arguments:      Char_t * Option (--ignored--)
// Results:
// Exceptions:
// Description:    Calls mehtod "Draw" for any window in the list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * varp = this->First(kIsWindow);
	while (varp) {
		((TMrbWindow *) varp)->Draw();
		varp = this->After(varp, kIsWindow);
	}
}

TObject * TMrbLofUserVars::First(UInt_t VarType) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::First
// Purpose:        Get first entry of var list
// Arguments:      UInt_t VarType       -- type
// Results:        TObject * varp       -- pointer to object
// Exceptions:     varp = NULL if end of list
// Description:    Returns first var object of given type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * varp = fLofVars.First();
	while (varp) {
		if (varp->GetUniqueID() & VarType) return(varp);
		varp = fLofVars.After(varp);
	}
	return(NULL);
}

TObject * TMrbLofUserVars::After(TObject * VarPtr, UInt_t VarType) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::After
// Purpose:        Get next entry of var list
// Arguments:      TObject * VarPtr       -- pointer to current var/wdw
//                 UInt_t VarType         -- type
// Results:        TObject * varp         -- pointer to object
// Exceptions:     varp = NULL if end of list
// Description:    Returns next var object of given type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * varp = fLofVars.After(VarPtr);
	while (varp) {
		if (varp->GetUniqueID() & VarType) return(varp);
		varp = fLofVars.After(varp);
	}
	return(NULL);
}

TObject * TMrbLofUserVars::Find(const Char_t * VarName, UInt_t VarType) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::Find
// Purpose:        Find a variable/window
// Arguments:      Char_t * VarName       -- name of variable/window
//                 UInt_t VarType         -- type
// Results:        TObject * varp         -- pointer to var object
// Exceptions:     varp = NULL if not found
// Description:    Searches for a var object of given name and type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString varName;

	TObject * varp = this->First(VarType);
	while (varp) {
		if (varp->GetUniqueID() & kIsVariable)		varName = ((TMrbVariable *) varp)->GetName();
		else if (varp->GetUniqueID() & kIsWindow)	varName = ((TMrbWindow *) varp)->GetName();
		else if (varp->GetUniqueID() & kIsFunction)	varName = ((TFormula *) varp)->GetName();
		if (varName.CompareTo(VarName) == 0) return(varp);
		varp = this->After(varp, VarType);
	}
	return(NULL);
}

void TMrbLofUserVars::Print(ostream & OutStrm, Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::Print
// Purpose:        Print a list of variables/windows to cout
// Arguments:      ostream & OutStrm    -- output stream
//                 Char_t * Option      -- option string: V(ariables), W(indows), or D(efault)
// Results:
// Exceptions:
// Description:    Outputs contents of the whole list to OutStrm (default cout).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObject * varp;
	UInt_t vOpt, vOpt2;
	Bool_t found;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOptions);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (fNofVars == 0) {
		gMrbLog->Err() << this->GetName() << ": No variables/windows defined" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	found = kFALSE;
	if (vOpt2 = vOpt & (kVarI | kVarF | kVarS | kVarB)) {
		varp = this->First(vOpt2);
		while (varp) {
			if (!found) {
				OutStrm	<< endl << GetName() << "::VARIABLES:" << endl;
				OutStrm	<< "   " << setiosflags(ios::left)
						<< setw(20) << "NAME"
						<< setw(18) << "TYPE"
						<< setw(17) << "VALUE"
						<< setw(11) << "INIT"
						<< setw(17) << "RANGE"
						<< endl;
			}
			found = kTRUE;
			((TMrbVariable *) varp)->Print();
			varp = this->After(varp, vOpt2);
		}
	}

	found = kFALSE;
	if (vOpt2 = vOpt & (kWindowI | kWindowF)) {
		varp = this->First(vOpt2);
		while (varp) {
			if (!found) {
				OutStrm	<< endl << GetName() << "::WINDOWS:" << endl;
				OutStrm	<< "   " << setiosflags(ios::left)
						<< setw(20) << "NAME"
						<< setw(17) << "TYPE"
						<< setw(18) << "LIMITS"
						<< setw(11) << "INIT"
						<< setw(17) << "RANGE"
						<< endl;
			}
			found = kTRUE;
			((TMrbWindow *) varp)->Print();
			varp = this->After(varp, vOpt2);
		}
	}

	found = kFALSE;
	if (vOpt2 = vOpt & kWindow2D) {
		varp = this->First(vOpt2);
		while (varp) {
			if (!found) {
				OutStrm	<< endl << GetName() << "::CUTS:" << endl;
				OutStrm	<< "   " << setiosflags(ios::left)
						<< setw(20) << "NAME"
						<< setw(15) << "TYPE"
						<< setw(18) << "POINTS X"
						<< setw(11) << "       Y"
						<< endl;
			}
			found = kTRUE;
			((TMrbWindow2D *) varp)->Print();
			varp = this->After(varp, vOpt2);
		}
	}

	found = kFALSE;
	if (vOpt2 = vOpt & (kFunction1D | kFunction2D)) {
		varp = this->First(vOpt2);
		while (varp) {
			if (!found) {
				OutStrm	<< endl << GetName() << "::FUNCTIONS:" << endl;
				OutStrm	<< "   " << setiosflags(ios::left)
						<< setw(20) << "NAME"
						<< setw(15) << "TYPE"
						<< endl;
			}
			Int_t ndim = ((TFormula *) varp)->GetNdim();
			OutStrm	<< "   " << setiosflags(ios::left)
					<< setw(20) << ((TFormula *) varp)->GetName()
					<< setw(15) << Form("TF%d", ndim)
					<< endl;
			found = kTRUE;
			if (ndim == 1) ((TF1 *) varp)->Print(); else ((TF2 *) varp)->Print();
			varp = this->After(varp, vOpt2);
		}
	}

	OutStrm << endl;
}

Bool_t TMrbLofUserVars::ReadFromFile(const Char_t * VarFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::ReadFromFile
// Purpose:        Read variables/windows from file
// Arguments:      Char_t * VarFile    -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads variable and window defs from given file.
//                 For vars and 1-dim windows only.
//                 Array mode for vars only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString varFile;
	ifstream vstream;
	TMrbString vline, vline1, vline2;
	TString varName;
	TString varIndex;
	TObject * varp;
	Int_t n;
	Int_t line;
	Int_t ilow, iup;
	Double_t flow, fup;
	TString str, boolStr;
	Bool_t boolVal = kFALSE;
	Int_t one;
	UInt_t vtype;
	UInt_t varType;

	Int_t vIdx;
	Bool_t isIndexed, isArray;

	varFile = VarFile;
	varFile = varFile.Strip(TString::kBoth);

	if (varFile[0] == '?') {
		if (varFile.Length() > 1) {
			cout << "Input var/wdw defs from file [" << varFile(1, varFile.Length() - 1) << "]: " << flush;
			varFile.ReadToDelim(cin);
			varFile = varFile.Strip(TString::kBoth);
			if (varFile.Length() == 0) varFile = (VarFile + 1);
		} else {
			cout << "Input var/wdw defs from file: " << flush;
			varFile.ReadToDelim(cin);
		}
	}

	if (gSystem->AccessPathName(varFile.Data())) {
		gMrbLog->Err() << "Can't open file - " << varFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadFromFile");
		return(kFALSE);
	}

	TRegexp rx(".C$");
	if (varFile.Index(rx, 0) >= 0) {
		gROOT->Macro(varFile.Data());
	} else {
		vstream.open(varFile.Data(), ios::in);
		if (!vstream.good()) {
			gMrbLog->Err() << gSystem->GetError() << " - " << varFile << endl;
			gMrbLog->Flush(this->ClassName(), "ReadFromFile");
			return(kFALSE);
		}

		line = 0;
		for (;;) {
			vline.ReadLine(vstream, kFALSE);
			if (vstream.eof()) {
				vstream.close();
				return(kTRUE);
			}
			line++;

			vline = vline.Strip();
			if (vline.Length() <= 0) continue;
			if (vline[0] == '#') continue;

			n = vline.Index(":");
			if (n < 0) {
				gMrbLog->Err()	<< "Illegal format - colon missing" << endl
								<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				continue;
			}

			varName = vline(0, n);
			varName = varName.Strip();
			vline1 = vline(n + 1, 1000);
			vline1.ReplaceWhiteSpace();
			vline1 = vline1.Strip(TString::kBoth);
			vline2 = vline1;
			vline2.ReplaceAll(" ", "@");	// prepare input to handle strings with white spaces

			isIndexed = kFALSE;
			n = varName.Index("[");
			if (n == 0) {
				gMrbLog->Err()	<< "Illegal format - spurious []" << endl
								<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				continue;
			} else if (n > 0) {
				varIndex = varName(n + 1, 1000);
				varName = varName(0, n);
				varName = varName.Strip();
				n = varIndex.Index("]");
				if (n < 0) {
					gMrbLog->Err()	<< "Illegal format - \"]\" missing" << endl
									<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadFromFile");
					continue;
				}
				isIndexed = kTRUE;
				varIndex = varIndex(0, n);
				varIndex = varIndex.Strip();
				varIndex += " 1";
				istringstream s(varIndex.Data());
				one = 0;
				s >> vIdx >> one;
				if (one != 1) {
					gMrbLog->Err()	<< "Illegal format - illegal array index" << endl
									<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadFromFile");
					continue;
				}
			}

			varp = this->Find(varName.Data());
			if (varp == NULL) {
				gMrbLog->Err()	<< "No such variable/window - " << varName << endl
								<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				continue;
			}

			varType = varp->GetUniqueID() & kVarOrWindow;
			isArray = (varp->GetUniqueID() & kVarIsArray) != 0;

			if (isIndexed & !isArray) {
				gMrbLog->Err()	<< "Not an array of vars - " << varName << endl
								<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				continue;
			} else if (isArray & !isIndexed) {
				gMrbLog->Err()	<< "Index missing - " << varName << endl
								<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadFromFile");
				continue;
			}

			if (isArray) {
				n = ((TMrbVariable *) varp)->GetSize();
				if (n <= vIdx) {
					gMrbLog->Err()	<< "Index out of range - " << varName
									<< "[" << n << "]" << endl
									<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadFromFile");
					continue;
				}
			}

			vline1 += " 1"; 				// append a known value to the string - so we may detect format errors
			vline2 += " 1";

			vtype = 0;
			istringstream s(vline1.Data());
			one = 0;
			s >> ilow >> iup >> one;
			if (one == 1) {
				vtype = kWindowI;
			} else {
				one = 0;
				istringstream s1(vline1.Data());
				s1 >> flow >> fup >> one;
				if (one == 1) {
					vtype = kWindowF;
				} else {
					one = 0;
					istringstream s2(vline1.Data());
					s2 >> ilow >> one;
					if (one == 1) {
						vtype = kVarI;
					} else {
						one = 0;
						istringstream s3(vline1.Data());
						s3 >> flow >> one;
						if (one == 1) {
							vtype = kVarF;
						} else {
							one = 0;
							istringstream s4(vline2.Data());
							s4 >> str >> one;
							if (one == 1) {
								boolStr = str;
								boolStr.ToLower();
								TString trueStr = "true yes y";
								TString falseStr = "false no n";
								if (trueStr.Index(boolStr.Data(), 0) != -1) {
									boolVal = kTRUE;
									vtype = kVarB;
								} else if (falseStr.Index(boolStr.Data(), 0) != -1) {
									boolVal = kFALSE;
									vtype = kVarB;
								} else {
									vtype = kVarS;
								}
							} else {
								gMrbLog->Err()	<< "Illegal format" << endl
												<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
								gMrbLog->Flush(this->ClassName(), "ReadFromFile");
								continue;
							}
						}
					}
				}
			}

			Bool_t ok = kFALSE;
			if (varType != vtype) {
				if (varType == kVarB && vtype == kVarI) {
					if (ilow == 1) {
						vtype = kVarB;
						boolVal = kTRUE;
						ok = kTRUE;
					} else if (ilow == 0) {
						vtype = kVarB;
						boolVal = kFALSE;
						ok = kTRUE;
					}
				} else if (varType == kVarS && vtype == kVarB) {
					ok = kTRUE;
					vtype = kVarS;
				}
				if (!ok) {
					gMrbLog->Err() << "Conflicting types - " << varName << ": ";
					switch (varType) {
						case kVarI:		gMrbLog->Err()	<< "VarI("; break;
						case kVarF:		gMrbLog->Err()	<< "VarF("; break;
						case kVarS:		gMrbLog->Err()	<< "VarS("; break;
						case kVarB:		gMrbLog->Err()	<< "VarB("; break;
						case kWindowI:	gMrbLog->Err()	<< "WindowI("; break;
						case kWindowF:	gMrbLog->Err()	<< "WindowF("; break;
						default:		gMrbLog->Err()	<< "Unknown("; break;
					}
					gMrbLog->Err()	<< varType << ") <> Input:";
					switch (vtype) {
						case kVarI:		gMrbLog->Err()	<< "VarI("; break;
						case kVarF:		gMrbLog->Err()	<< "VarF("; break;
						case kVarS:		gMrbLog->Err()	<< "VarS("; break;
						case kVarB:		gMrbLog->Err()	<< "VarB("; break;
						case kWindowI:	gMrbLog->Err()	<< "WindowI("; break;
						case kWindowF:	gMrbLog->Err()	<< "WindowF("; break;
						default:		gMrbLog->Err()	<< "Unknown("; break;
					}
					gMrbLog->Err()	<< vtype << ")" << endl
									<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadFromFile");
					continue;
				}
			}

			if (isArray) {
				switch (vtype) {
					case kVarI:	((TMrbVarArrayI *) varp)->Set(vIdx, ilow); break;
					case kVarF:	((TMrbVarArrayF *) varp)->Set(vIdx, flow); break;
				}
			} else {
				switch (vtype) {
					case kVarI:	((TMrbVarI *) varp)->Set(ilow); break;
					case kVarF:	((TMrbVarF *) varp)->Set(flow); break;
					case kVarB:	((TMrbVarB *) varp)->Set(boolVal); break;
					case kVarS:	str.ReplaceAll("@", " "); ((TMrbVarS *) varp)->Set(str.Data()); break;

					case kWindowI:
						if (ilow > iup) {
							gMrbLog->Err()	<< "Wrong limits - " << varName
											<< "[" << ilow << "," << iup << "]" << endl
											<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
							gMrbLog->Flush(this->ClassName(), "ReadFromFile");
							continue;
						}
						((TMrbWindowI *) varp)->Set(ilow, iup);
						break;

					case kWindowF:
						if (flow > fup) {
							gMrbLog->Err()	<< "Wrong limits - " << varName
									<< "[" << flow << "," << fup << "]" << endl
									<< setw(24) << varFile << "(" << line << ") >>" << vline << "<<" << endl;
							gMrbLog->Flush(this->ClassName(), "ReadFromFile");
							continue;
						}
						((TMrbWindowF *) varp)->Set(flow, fup);
						break;
				}
			}
		}
	}
	return(kTRUE);
}

void TMrbLofUserVars::SetGlobalAddress() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::SetGlobalAddress
// Purpose:        Set global address gMrbLofUserVars
// Arguments:
// Results:
// Exceptions:
// Description:    Sets global variable gMrbLofUserVars to `this'.
//                 May be used when reading a config from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMrbLofUserVars = this;
};

void TMrbLofUserVars::SetInitialFctType(TObject * Fct, UInt_t FctType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLofUserVars::SetInitialFctType
// Purpose:        Set function type
// Arguments:      TObject * Fct     -- ptr to function
//                 UInt_t VarType    -- type bits
// Results:
// Exceptions:
// Description:    Defines type bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Fct->GetUniqueID() != 0) return;

	Fct->SetUniqueID(FctType);
	gMrbLofUserVars->Append(Fct);
	if (gDirectory != NULL) gDirectory->Append(Fct);
}


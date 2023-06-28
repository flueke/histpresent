//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbProfile.cxx
// Purpose:        MARaBOU utilities: Graphics profiles
// Description:    Implements utilities to specify GC
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbProfile.cxx,v 1.4 2011-12-13 08:04:58 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "THashList.h"
#include "TEnv.h"
#include "TVirtualX.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGClient.h"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TGMrbProfile.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TGMrbGC)
ClassImp(TGMrbProfile)
ClassImp(TGMrbLofProfiles)

TGMrbLofProfiles * gMrbLofProfiles = NULL;	// system list of graph envs

static TString frameOptString = Form("SunkenFrame=%d:RaisedFrame=%d:DoubleBorder=%d", kSunkenFrame, kRaisedFrame, kDoubleBorder);

extern TMrbLogger * gMrbLog;			// access to message lgging

const SMrbNamedXShort kGMrbGCTypes[] =
							{
								{TGMrbGC::kGMrbGCFrame,			"Frame" 		},
								{TGMrbGC::kGMrbGCLabel,			"Label" 		},
								{TGMrbGC::kGMrbGCButton,		"Button"		},
								{TGMrbGC::kGMrbGCEntry,			"Entry"			},
								{TGMrbGC::kGMrbGCTextEntry,		"TextEntry"		},
								{TGMrbGC::kGMrbGCLBEntry,		"ListBoxEntry"	},
								{TGMrbGC::kGMrbGCTextButton,	"TextButton"	},
								{TGMrbGC::kGMrbGCRadioButton, 	"RadioButton"	},
								{TGMrbGC::kGMrbGCCheckButton, 	"CheckButton"	},
								{TGMrbGC::kGMrbGCPictureButton,	"PictureButton"	},
								{0, 							NULL			}
							};

TGMrbGC::TGMrbGC(const Char_t * Font, const Char_t * Foreground, const Char_t * Background, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC
// Purpose:        Graphics context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Char_t * Foreground   -- foreground color
//                 Char_t * Background   -- background color
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	TGFont * font;
	TString fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == NULL) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl
						<< "Falling back to \"NormalFont\"" << endl;
		gMrbLog->Flush("TGMrbGC");
		fontStr = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
		font = gClient->GetFont(fontStr.Data());
	}
	if (font == NULL) {
		gMrbLog->Err()	<< "Can't set font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbGC");
	}

	Pixel_t fg;
	if (!gClient->GetColorByName(Foreground, fg)) {
		gMrbLog->Wrn()	<< "No such color - " << Foreground << " (foreground)" << endl
						<< "Falling back to \"black\"" << endl;
		gMrbLog->Flush("TGMrbGC");
		gClient->GetColorByName("black", fg);
	}

	Pixel_t bg;
	if (!gClient->GetColorByName(Background, bg)) {
		gMrbLog->Wrn()	<< "No such color - " << Background << " (background)" << endl
						<< "Falling back to \"white\"" << endl;
		gMrbLog->Flush("TGMrbGC");
		gClient->GetColorByName("white", bg);
	}

	fFontName = fontStr;
	fForegroundName = Foreground;
	fBackgroundName = Background;

	fFont = font;
	fForeground = fg;
	fBackground = bg;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

TGMrbGC::TGMrbGC(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC
// Purpose:        Graphics context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Pixel_t Foreground    -- foreground color
//                 Pixel_t Background    -- background color
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	TString fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");

	TGFont * font;
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == NULL) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl
						<< "Falling back to \"NormalFont\"" << endl;
		gMrbLog->Flush("TGMrbGC");
		fontStr = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
		font = gClient->GetFont(fontStr);
	}
	if (font == NULL) {
		gMrbLog->Err()	<< "Can't set font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbGC");
	}

	fFontName = fontStr;
	fForegroundName = Form("#%0lx", Foreground);
	fForegroundName = Form("#%0lx", Background);

	fFont = font;
	fForeground = Foreground;
	fBackground = Background;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

TGMrbGC::TGMrbGC(TGMrbGC::EGMrbGCType Type, UInt_t Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC
// Purpose:        Graphics context within MARaBOU
// Arguments:      EGMrbGCType Type   -- grpahic object type
//                 UInt_t Options        -- option bits
// Description:    A graphics context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fFontName = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGFont * font = gClient->GetFont(fFontName.Data());
	if (font == NULL) {
		gMrbLog->Err()	<< "Can't set font - " << fFontName << endl;
		gMrbLog->Flush("TGMrbGC");
	}

	Pixel_t fg;
	fForegroundName = "black";
	gClient->GetColorByName(fForegroundName.Data(), fg);

	fBackgroundName = (Type >= TGMrbGC::kGMrbGCTextEntry) ? "white" : "gray";
	Pixel_t bg;
	gClient->GetColorByName(fBackgroundName.Data(), bg);

	fFont = font;
	fForeground = fg;
	fBackground = bg;

	fOptions = Options;

	fGC = new TGGC();
	if (fFont) fGC->SetFont(fFont->GetFontHandle());
	fGC->SetForeground(fForeground);
	fGC->SetBackground(fBackground);
}

Bool_t TGMrbGC::SetFont(const Char_t * Font) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC::SetFont
// Purpose:        Change font in a GC
// Arguments:      Char_t * Font      -- font name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a font.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;
	TGFont * font;

	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == NULL) {
		gMrbLog->Wrn()	<< "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbGC", "SetFont");
		return(kFALSE);
	}
	fFontName = fontStr; 
	fFont = font; 
	fGC->SetFont(fFont->GetFontHandle());
	return(kTRUE);
}

Bool_t TGMrbGC::SetFG(const Char_t * Foreground) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC::SetFG
// Purpose:        Change foreground
// Arguments:      Char_t * Foreground  -- foreground color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a foreground.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Foreground, color)) {
		gMrbLog->Wrn()	<< "No such color - " << Foreground << " (foreground)" << endl;
		gMrbLog->Flush("TGMrbGC", "SetFG");
		return(kFALSE);
	}

	fForegroundName = Foreground; 
	fForeground = color; 
	fGC->SetForeground(color);
	return(kTRUE);
}

Bool_t TGMrbGC::SetBG(const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC::SetBG
// Purpose:        Change background
// Arguments:      Char_t * Background  -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a background.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Background, color)) {
		gMrbLog->Wrn()	<< "No such color - " << Background << " (background)" << endl;
		gMrbLog->Flush("TGMrbGC", "SetFG");
		return(kFALSE);
	}

	fBackgroundName = Background; 
	fBackground = color; 
	fGC->SetBackground(color);
	return(kTRUE);
}

TGMrbProfile::TGMrbProfile(const Char_t * Name, const Char_t * Title, TEnv * Env) : TNamed(Name, Title) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile
// Purpose:        Define graphics profile
// Arguments:      Char_t * Name   -- name
//                 Char_t * Title  -- title
//                 TEnv * Env      -- env definitions
// Description:    Defines a graphics profile:
//                 a list of TGMrbGC objects together with some options
//                 and other global params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fLofGCs.Delete();
	fLofGCs.SetPatternMode();

	TMrbLofNamedX wtypes;
	wtypes.AddNamedX(kGMrbGCTypes);

	TMrbNamedX * bnx = wtypes.FindByIndex(TGMrbGC::kGMrbGCButton);
	TGMrbGC * bgc = this->AddGC(bnx, Env);

	TMrbNamedX * enx = wtypes.FindByIndex(TGMrbGC::kGMrbGCEntry);
	TGMrbGC * egc = this->AddGC(enx, Env);

	TMrbNamedX * nx = (TMrbNamedX *) wtypes.First();
	while (nx) {
		TGMrbGC * dfltGC = NULL;
		if (nx->GetIndex() & TGMrbGC::kGMrbGCButton)		dfltGC = bgc;
		else if (nx->GetIndex() & TGMrbGC::kGMrbGCEntry)	dfltGC = egc;
		this->AddGC(nx, Env, dfltGC);
		nx = (TMrbNamedX *) wtypes.After(nx);
	}
}
		
TGMrbGC * TGMrbProfile::AddGC(TMrbNamedX * GCSpec, TEnv * Env, TGMrbGC * DefaultGC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::AddGC
// Purpose:        Add a graphics context to list
// Arguments:      TMrbNamedX * GCSpec        -- graphics context specs
//                 TEnv * Env                 -- ROOT environment
//                 TGMrbGC * DefaultGC  -- default context
// Results:        TGMrbGC * GC         -- context
// Exceptions:
// Description:    Fills table 'fLofGCs'
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofGCs.FindByIndex(GCSpec->GetIndex());
	if (nx) return((TGMrbGC *) nx->GetAssignedObject());

	if (Env == NULL) Env = gEnv;

	TGMrbGC * gc = new TGMrbGC((TGMrbGC::EGMrbGCType) GCSpec->GetIndex());

	TString res = this->GetName();
	res.ToLower();
	this->SetName(res.Data());
	res(0,1).ToUpper();

	TString rName = res + ".";
	rName = rName + GCSpec->GetName();

	TString dflt = DefaultGC ? DefaultGC->GetFontName() : "";
	TString r = Env->GetValue(rName + ".Font", dflt.Data());
	if (!r.IsNull()) gc->SetFont(r.Data());

	dflt = DefaultGC ? DefaultGC->GetForegroundName() : "";
	r = Env->GetValue(rName + ".Foreground", dflt.Data());
	if (!r.IsNull()) gc->SetFG(r.Data());

	dflt = DefaultGC ? DefaultGC->GetBackgroundName() : "";
	r = Env->GetValue(rName + ".Background", dflt.Data());
	if (!r.IsNull()) gc->SetBG(r.Data());

	TString optStr = Env->GetValue(rName + ".Options", "");
	TMrbLofNamedX lofOptions; lofOptions.AddNamedX(frameOptString.Data());
	UInt_t optionBits = 0;
	Int_t from = 0;
	TString opt;
	while (optStr.Tokenize(opt, from, ":")) {
		TMrbNamedX * o = lofOptions.FindByName(opt.Data());
		if (o) optionBits |= o->GetIndex();
	}
	if (optionBits == 0) optionBits = DefaultGC ? DefaultGC->GetOptions() : 0;
	gc->SetOptions(optionBits);

	fLofGCs.AddNamedX(GCSpec->GetIndex(), GCSpec->GetName(), "", gc);

	return(gc);
}

Bool_t TGMrbProfile::SetGC(TGMrbGC::EGMrbGCType Type, TGMrbGC * GC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type        -- graphics context type
//                 TGMrbGC * GC      -- context
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByIndex(Type);
	if (nx == NULL) {
		gMrbLog->Err() << "No such graph context type - " << Type << endl;
		gMrbLog->Flush(this->ClassName(), "SetGC");
		return(kFALSE);
	}
	nx->AssignObject(GC);
	return(kTRUE);
}

Bool_t TGMrbProfile::SetGC(TGMrbGC::EGMrbGCType Type, const Char_t * Font, const Char_t * Foreground, const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type              -- gc type
//                 Char_t * Font                 -- font name
//                 Char_t * Foreground           -- foreground color
//                 Char_t * Background           -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbGC * gc = new TGMrbGC(Font, Foreground, Background);
	if (this->SetGC(Type, gc)) return(kTRUE);
	delete gc;
	return(kFALSE);
}
	
Bool_t TGMrbProfile::SetGC(TGMrbGC::EGMrbGCType Type, const Char_t * Font, Pixel_t Foreground, Pixel_t Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetGC
// Purpose:        Set graphics context
// Arguments:      EGMrbGCType Type              -- gc type
//                 Char_t * Font                 -- font name
//                 Pixel_t Foreground            -- foreground color
//                 Pixel_t Background            -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates table 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbGC * gc = new TGMrbGC(Font, Foreground, Background);
	if (this->SetGC(Type, gc)) return(kTRUE);
	delete gc;
	return(kFALSE);
}

TGMrbGC * TGMrbProfile::GetGC(TGMrbGC::EGMrbGCType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::GetGC
// Purpose:        Get graphics context
// Arguments:      EGMrbGCType Type              -- gc type
// Results:        TGMrbGC * GC            -- graphics context
// Exceptions:
// Description:    Table lookup in 'fLofGCs'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByIndex(Type);
	if (nx == NULL) {
		gMrbLog->Err() << "No such graph cont type - " << Type << endl;
		gMrbLog->Flush(this->ClassName(), "GetGC");
		return(NULL);
	}
	return((TGMrbGC *) nx->GetAssignedObject());
}

void TGMrbProfile::SetOptions(TGMrbGC::EGMrbGCType Type, UInt_t Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetOptions
// Purpose:        Set option bits
// Arguments:      EGMrbGCType Type              -- gc type
//                 UInt_t Options                -- option bits
// Results:        --
// Exceptions:
// Description:    Defines option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbGC * gc = this->GetGC(Type);
	if (gc != NULL) gc->SetOptions(Options);
}

UInt_t  TGMrbProfile::GetOptions(TGMrbGC::EGMrbGCType Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetOptions
// Purpose:        Get option bits
// Arguments:      EGMrbGCType Type              -- gc type
// Results:        UInt_t Options                -- option bits
// Exceptions:
// Description:    Returns option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbGC * gc = this->GetGC(Type);
	if (gc == NULL) return(0);
	else			return(gc->GetOptions());
}

void TGMrbProfile::SetOptions(const Char_t * Type, UInt_t Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetOptions
// Purpose:        Set option bits
// Arguments:      Char_t * Type                 -- gc type
//                 UInt_t Options                -- option bits
// Results:        --
// Exceptions:
// Description:    Defines option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByName(Type);
	if (nx) ((TGMrbGC *) nx->GetAssignedObject())->SetOptions(Options);
}

UInt_t  TGMrbProfile::GetOptions(const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::SetOptions
// Purpose:        Get option bits
// Arguments:      Char_t * Type                 -- gc type
// Results:        UInt_t Options                -- option bits
// Exceptions:
// Description:    Returns option bits for a graph context.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.FindByName(Type);
	if (nx == NULL) return(0);
	else			return(((TGMrbGC *) nx->GetAssignedObject())->GetOptions());
}

void TGMrbProfile::Print(ostream & Out, const Char_t * /*Type*/) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile::Print
// Purpose:        Print profile data
// Arguments:      ostream & Out        -- output stream
//                 Char_t * Type        -- graph context type
// Results:        --
// Exceptions:
// Description:    Output profile data to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << endl << "Graph profile \"" << this->GetName() << "\"";
	TString title = this->GetTitle();
	if (!title.IsNull()) Out << " (" << title << ")";
	Out << ":" << endl;
	TMrbNamedX * nx = (TMrbNamedX *) fLofGCs.First();
	while (nx) {
		Out << Form("%-15s", nx->GetName());
		TGMrbGC * gc = (TGMrbGC *) nx->GetAssignedObject();
		if (gc == NULL) {
			Out << "(empty)" << endl;
		} else {
			Out << Form("%15s", "[Font]: ") << gc->GetFontName() << endl;
			Out << Form("%15s%15s", "", "[Foreground]: ")
				<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
				<< gc->FG() << resetiosflags(ios::uppercase) << setbase(10)
				<< " = " << gc->GetForegroundName() << endl;
			Out << Form("%15s%15s", "", "[Background]: ")
				<< "#" << setbase(16) << setw(6) << setfill('0') << setiosflags(ios::uppercase)
				<< gc->BG() << resetiosflags(ios::uppercase) << setbase(10)
				<< " = " << gc->GetBackgroundName() << endl;
			TMrbLofNamedX lofOptions; lofOptions.AddNamedX(frameOptString.Data());
			TString opt;
			UInt_t optionBits = gc->GetOptions();
			TIterator * iter = lofOptions.MakeIterator();
			TMrbNamedX * nx1;
			TString sep = "";
			while (nx1 = (TMrbNamedX *) iter->Next()) {
				if (optionBits & nx1->GetIndex()) {
					opt += Form("%s%s", sep.Data(), nx1->GetName());
					sep = ":";
				}
			}
			Out << Form("%15s%15s", "", "[Options]: ")
				<< setbase(16) << setiosflags(ios::showbase) << gc->GetOptions() << resetiosflags(ios::showbase) << setbase(10);
			if (!opt.IsNull()) Out << " = " << opt;
			Out << endl;
		}
		nx = (TMrbNamedX *) fLofGCs.After(nx);
	}
}
 
TGMrbLofProfiles::TGMrbLofProfiles(const Char_t * Name, const Char_t * Title) : TMrbLofNamedX(Name, Title) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles
// Purpose:        A list of graphics profiles
// Arguments:      Char_t * Name   -- name
//                 Char_t * Title  -- title
// Description:    Defines a list of graphics profiles
//                 Tries to get profile specs from file given by resource <Name>.RcFile
//                 If this fails tries file .gprofrc
//                 If this fails looks at defs in .rootrc
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	TString name = this->GetName();
	TString title = this->GetTitle();
	if (title.IsNull() && name.CompareTo("LofGraphProfiles") == 0) this->SetTitle("MARaBOU's list of graphics profiles");

	TString res = this->GetName();
	res(0,1).ToUpper();
	TString envPath = res;
	envPath += ".RcFile";
	fRcFile = gEnv->GetValue(envPath.Data(), "");
	if (fRcFile.IsNull()) {
		envPath = this->ClassName();
		envPath += ".RcFile";
		fRcFile = gEnv->GetValue(envPath.Data(), "");
	}
	if (fRcFile.IsNull()) {
		TEnv * e = new TEnv(".gprofrc");
		if (e->GetTable()->GetSize() > 0) {
			fEnv = e;
		} else {
			fEnv = gEnv;
			delete e;
		}
	} else {
		fEnv = new TEnv(fRcFile.Data());
	}
	TString mstr = fEnv->GetValue(res + ".Members", "");
	if (mstr.IsNull()) {
		gMrbLog->Wrn() << "No graphics profiles defined (resource \"" << res << ".Members\" missing)" << endl;
		gMrbLog->Flush("TGMrbLofProfiles");
	} else {
		Int_t n = 0;
		Int_t from = 0;
		TString mbr;
		while (mstr.Tokenize(mbr, from, ":")) {
			n++;
			mbr.ToLower();
			res = mbr;
			res(0,1).ToUpper();
			title = fEnv->GetValue(res + ".Title", "");
			TGMrbProfile * gc = new TGMrbProfile(mbr.Data(), title.Data(), fEnv);
			this->AddNamedX(this->GetEntriesFast(), mbr.Data(), title.Data(), gc);
		}
		if (n == 0) {
			gMrbLog->Wrn() << "No graphics profiles defined (resource \"" << res << ".Members\" is empty)" << endl;
			gMrbLog->Flush("TGMrbLofProfiles");
		}
	}
	if (this->FindProfile("standard", kFALSE) == NULL) this->AddProfile("standard", "Standard graphics profile");
	TString defGC = fEnv->GetValue(res + ".Default", "");
	this->SetDefault(defGC.IsNull() ? "standard" : defGC.Data());
}

TGMrbProfile * TGMrbLofProfiles::AddProfile(const Char_t * Name, const Char_t * Title) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles::AddProfile
// Purpose:        Add a graphics profile to list
// Arguments:      Char_t * Name    -- name
//                 Char_t * Title   -- title
// Results:        --
// Exceptions:
// Description:    Adds a new graphics profile.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbProfile * gc;

	TString name = Name;
	name.ToLower();
	if ((gc = this->FindProfile(name.Data())) != NULL) {
		gMrbLog->Wrn() << "Graphics profile already defined - " << name << " (ignored)" << endl;
		gMrbLog->Flush(this->ClassName(), "AddProfile");
		return(gc);
	}
	gc = new TGMrbProfile(name.Data(), Title);
	this->AddNamedX(this->GetEntriesFast(), name.Data(), Title, gc);
	return(gc);
}

TGMrbProfile * TGMrbLofProfiles::FindProfile(const Char_t * Name, Bool_t FallBack) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles::FindProfile
// Purpose:        Find a given graph profile
// Arguments:      Char_t * Name          -- name
// Results:        TGMrbProfile * Profile -- profile
// Exceptions:
// Description:    Searches for a given profile
//                 FallBack = kTRUE: Returns "default" profile if search fails
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString name = Name;
	name.ToLower();
	TMrbNamedX * nx = this->FindByName(name.Data());
	if (nx) return((TGMrbProfile *) nx->GetAssignedObject());
	if (FallBack) return(fDefault);
	return(NULL);
}

TGMrbProfile * TGMrbLofProfiles::SetDefault(const Char_t * Name) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles::SetDefault
// Purpose:        Define a default graph profile
// Arguments:      Char_t * Name          -- name
// Results:        TGMrbProfile * Profile -- default profile
// Exceptions:
// Description:    Defines a graph profile to be default.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbProfile * gc = this->FindProfile(Name);
	if (gc == NULL) {
		gMrbLog->Err() << "Graphics profile not found - " << Name << endl;
		gMrbLog->Flush(this->ClassName(), "SetDefault");
		return(NULL);
	}
	fDefault = gc;
	return(gc);
}

void TGMrbLofProfiles::Print(ostream & Out, const Char_t * Name) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles::Print
// Purpose:        Print a list of graph envs
// Arguments:      ostream & Out        -- output stream
//                 Char_t * Name        -- env name
// Results:        --
// Exceptions:
// Description:    Output list of profiles to ostream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString name;
	TGMrbProfile * gc;

	TString defName = fDefault->GetName();

	Out << endl << "=========================================================================================" << endl;
	Out << "List of graphics profiles: " << this->GetName();
	TString title = this->GetTitle();
	if (!title.IsNull()) Out << " (" << title << ")";
	Out << endl << "-----------------------------------------------------------------------------------------" << endl;
	if (Name != NULL) {
		name = Name;
		name.ToLower();
	}
	TMrbNamedX * nx = (TMrbNamedX *) this->First();
	while (nx) {
		if (Name == NULL || name.CompareTo(nx->GetName()) == 0) {
			gc = (TGMrbProfile *) nx->GetAssignedObject();
			gc->Print(Out);
		}
		nx = (TMrbNamedX *) this->After(nx);
	}

	Out << endl << "[Default profile is \"" << fDefault->GetName() << "\"]" << endl;
	if (!fRcFile.IsNull()) Out << "[Settings as read from file \"" << fRcFile << "\"]" << endl;
}

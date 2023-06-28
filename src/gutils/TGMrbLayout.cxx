//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLayout.cxx
// Purpose:        MARaBOU utilities: Graphic context
// Description:    Implements utilities to specify GC
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbLayout.cxx,v 1.6 2008-06-24 08:36:37 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TGWindow.h"
#include "TGClient.h"
#include "TGMrbLayout.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TGMrbLayout)

extern TMrbLogger * gMrbLog;			// access to message lgging

TGMrbLayout::TGMrbLayout(const Char_t * Font, const Char_t * Foreground, const Char_t * Background, TGLayoutHints * Hints) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        Graphic context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 Char_t * Foreground   -- foreground color
//                 Char_t * Background   -- background color
// Description:    A graphic context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;

	TGFont * font;
	Pixel_t fg;
	Pixel_t bg;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout");
		font = gClient->GetFont(gEnv->GetValue("Gui.NormalFont",
													"-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	if (!gClient->GetColorByName(Foreground, fg)) {
		gMrbLog->Err() << "No such color - " << Foreground << " (foreground)" << endl;
		gMrbLog->Flush("TGMrbLayout");
		gClient->GetColorByName("black", fg);
	}

	if (!gClient->GetColorByName(Background, bg)) {
		gMrbLog->Err() << "No such color - " << Background << " (background)" << endl;
		gMrbLog->Flush("TGMrbLayout");
		gClient->GetColorByName("white", bg);
	}

	fLayoutHints = NULL;

	this->CreateGC(font, fg, bg, Hints);
}

TGMrbLayout::TGMrbLayout(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, TGLayoutHints * Hints) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        Graphic context within MARaBOU
// Arguments:      Char_t * Font         -- font
//                 ULong_t Foreground    -- foreground color
//                 ULOng_t Background    -- background color
// Description:    A graphic context within MARaBOU
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString fontStr;

	TGFont * font;

	fontStr = (*Font == '-') ? Font : gEnv->GetValue(Font, "");
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout");
		font = gClient->GetFont(gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}

	fLayoutHints = NULL;

	this->CreateGC(font, Foreground, Background, Hints);
}

Bool_t TGMrbLayout::SetFont(const Char_t * Font) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetFont
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
	if ((fontStr.Length() == 0) || (font = gClient->GetFont(fontStr.Data())) == 0) {
		gMrbLog->Err() << "No such font - " << fontStr << endl;
		gMrbLog->Flush("TGMrbLayout", "SetFont");
		font = gClient->GetFont(gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1"));
	}
	fFont = font;
	fGC.SetFont(font->GetFontHandle());
	return(kTRUE);
}

Bool_t TGMrbLayout::SetFG(const Char_t * Foreground) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetFG
// Purpose:        Change foreground
// Arguments:      Char_t * Foreground  -- foreground color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a foreground.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Foreground, color)) {
		gMrbLog->Err() << "No such color - " << Foreground << " (foreground)" << endl;
		gMrbLog->Flush("TGMrbLayout", "SetFG");
		gClient->GetColorByName("black", color);
	}
	fGC.SetForeground(color);
	return(kTRUE);
}

Bool_t TGMrbLayout::SetBG(const Char_t * Background) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::SetBG
// Purpose:        Change background
// Arguments:      Char_t * Background  -- background color
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a background.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Pixel_t color;

	if (!gClient->GetColorByName(Background, color)) {
		gMrbLog->Err() << "No such color - " << Background << " (background)" << endl;
		gMrbLog->Flush("TGMrbLayout", "SetBG");
		gClient->GetColorByName("white", color);
	}
	fGC.SetBackground(color);
	return(kTRUE);
}

void TGMrbLayout::CreateGC(TGFont * Font, Pixel_t Foreground, Pixel_t Background, TGLayoutHints * Hints) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout::CreateGC
// Purpose:        Create a new GC
// Arguments:      TGFont * Font         -- font
//                 Pixel_t Foreground    -- foreground
//                 Pixel_t Background    -- background
// Results:        
// Exceptions:
// Description:    Creates a new GC.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFont = Font;
	fGC = *gClient->GetResourcePool()->GetFrameGC();
	fGC.SetFont(Font->GetFontHandle());
	fGC.SetForeground(Foreground);
	fGC.SetBackground(Background);
	if (Hints) fLayoutHints = Hints;
}


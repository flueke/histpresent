//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbProgressBar.cxx
// Purpose:        MARaBOU graphic utilities: a progress bar
// Description:    Implements class methods to show messages
//                 in a scrollable listbox.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbProgressBar.cxx,v 1.3 2004-09-28 13:47:33 rudi Exp $       
// Date:           
//
//Begin_Html
/*
<img src=gutils/TGMrbProgressBar.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TVirtualX.h"
#include "TEnv.h"
#include "TGMrbLayout.h"
#include "TGMrbProgressBar.h"

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

ClassImp(TGMrbProgressBar)

TGMrbProgressBar::TGMrbProgressBar( const TGWindow * Parent,
										const TGWindow * Main,
										const Char_t * Title,
										UInt_t Width,
										const Char_t * BarColor,
										const Char_t * BarText,
										Bool_t WithDetail) : TGTransientFrame(Parent, Main, Width, 0) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProgressBar
// Purpose:        Display a progress bar
// Arguments:      TGWindow * Parent             -- parent window
//                 TGWindow * Main               -- main window
//                 Char_t * Title                -- title
//                 UInt_t Width                  -- window width
//                 Char_t * BarColor             -- fill color
//                 Char_t * BarText              -- text to be displayed
//                 Bool_t WithDetail             -- show detail
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(font, black, gray);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);

	if (BarText == NULL || *BarText == '\0') {
		fBar = new TGHProgressBar(this, TGProgressBar::kStandard, Width - 20);
		fBar->SetFillType(TGProgressBar::kBlockFill);
	} else {
		fBar = new TGHProgressBar(this, TGProgressBar::kFancy, Width - 20);
		fBar->ShowPosition(kTRUE, kFALSE, BarText);
	}
	fBar->SetBarColor(BarColor);
	HEAP(fBar);
	this->AddFrame(fBar, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0));
	
	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	fDetail = NULL;
	if (WithDetail) {
		fDetail = new TGLabel(this, new TGString(""));
		HEAP(fDetail);
		this->AddFrame(fDetail, frameGC->LH());
		fDetail->SetTextJustify(kTextCenterX);
	}

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() / 2), (((TGFrame *) Main)->GetHeight() / 2), ax, ay, wdum);
	this->Move(ax, ay);

	SetWindowName(Title);
	SetIconName(Title);
	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, 0);
	MapWindow();
}

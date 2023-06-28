//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbFileEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled file entry
// Description:    Implements class methods to handle a labelled file entry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbFileEntry.cxx,v 1.8 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbFileEntry.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TGLabel.h"
#include "TGToolTip.h"
#include "TMrbSystem.h"

#include "TGMrbFileEntry.h"

ClassImp(TGMrbFileEntry)

TGMrbFileEntry::TGMrbFileEntry(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize, Int_t FrameId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth,
												TGFileInfo * FileInfo, EFileDialogMode /*DialogMode*/,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * BrowseGC,
												UInt_t FrameOptions,
												UInt_t /*EntryOptions*/) :
								TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry
// Purpose:        Define a labelled entry suitable to browse files
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- size of text buffer in chars
//                 Int_t FrameId               -- id to be used with signal/slots
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- widht of the entry field
//                 TGFileInfo * FileInfo       -- file info
//                 EFileDialogMode DialogMode  -- mode: open or save
//                 TGMrbLayout * LabelGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * ButtonGC      -- ... (button)
//                 TGMrbLayout * BrowseGC      -- ... (browse)
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	EntryGC = this->SetupGC(EntryGC, FrameOptions);
	BrowseGC = this->SetupGC(BrowseGC, FrameOptions);

	Int_t entryWidth = EntryWidth;

	fFrameId = FrameId;

	if (Label != NULL) {
		TGLabel * label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	if (FileInfo) {
		fFileInfo.fFileTypes = FileInfo->fFileTypes;
		fFileInfo.fIniDir = StrDup(FileInfo->fIniDir);
		TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 0);
		fHeap.AddFirst((TObject *) btnLayout);
		fBrowse = new TGPictureButton(this, fClient->GetPicture("ofolder_t.xpm"), 0);
		fHeap.AddFirst((TObject *) fBrowse);
		this->AddFrame(fBrowse, btnLayout);
		fBrowse->ChangeBackground(BrowseGC->BG());
		fBrowse->SetToolTipText("Browse files", 500);
		fBrowse->Connect("Clicked()", this->ClassName(), this, "Browse()");
		entryWidth -= fBrowse->GetWidth();;
	}

	fEntry = new TGTextEntry(this, new TGTextBuffer(BufferSize), fFrameId);
	fEntry->SetFont(EntryGC->Font());
	fEntry->SetBackgroundColor(EntryGC->BG());
	fEntry->Connect("ReturnPressed()", this->ClassName(), this, Form("EntryChanged(Int_t=%d)", fFrameId));
	fEntry->Connect("TabPressed()", this->ClassName(), this, Form("EntryChanged(Int_t=%d)", fFrameId));

	fHeap.AddFirst((TObject *) fEntry);
	this->AddFrame(fEntry, EntryGC->LH());
	fEntry->Resize(entryWidth, Height);
}

void TGMrbFileEntry::Browse() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry::Browse
// Purpose:        Message handler for browse button
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Handle message received from browse button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	TGFileInfo fi;
	new TGFileDialog(fClient->GetRoot(), this, fDialogMode, &fFileInfo);
	if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') fEntry->SetText(fFileInfo.fFilename);
	this->EntryChanged(fFrameId);
}

void TGMrbFileEntry::FileButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:       TGMrbFileEntry::FileButtonEnableonEnable
// Purpose:        Enable/disable file button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables file button.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fBrowse) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fBrowse->SetState(btnState);
	}
}

void TGMrbFileEntry::EntryChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry::EntryChanged
// Purpose:        Signal handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t EntryNo    -- entry number
// Results:        --
// Exceptions:     
// Description:    Emits signal on "entry changed" 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   Long_t args[2];

   args[0] = FrameId;
   args[1] = EntryNo;

   this->Emit("EntryChanged(Int_t, Int_t)", args);
}

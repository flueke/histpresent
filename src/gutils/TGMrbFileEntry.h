#ifndef __TGMrbFileEntry_h__
#define __TGMrbFileEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbFileEntry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbFileEntry         -- a TGMrbLabelEntry suitable to
//                                           browse files
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbFileEntry.h,v 1.7 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGFileDialog.h"
#include "TGTextEntry.h"

#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry
// Purpose:        Define a labelled text entry to browse files
// Description:    Defines a label together with a text entry field.
//                 A BROWSE button on left side opens a file dialog.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileEntry: public TGCompositeFrame, public TGMrbObject {

	public:
		enum						{	kFrameIdShift 		=	16	};

	public:
		TGMrbFileEntry(const TGWindow * Parent, const Char_t * Label,				// ctor
							Int_t BufferSize, Int_t EntryId,
							Int_t Width, Int_t Height, Int_t EntryWidth,
							TGFileInfo * FileInfo,
							EFileDialogMode DialogMode,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * EntryGC = NULL,
							TGMrbLayout * BrowseGC = NULL,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t EntryOptions = kSunkenFrame | kDoubleBorder);

		~TGMrbFileEntry() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline const Char_t * GetText() { return(fEntry->GetText()); };
		inline void SetText(const Char_t * Text) { fEntry->SetText(Text); };

		void FileButtonEnable(Bool_t Flag = kTRUE);			// enable/disable file button

		void Browse();										// slot method called upon Clicked() signal

		void EntryChanged(Int_t FrameId, Int_t Signal = 0); //*SIGNAL*

		inline void Help() const { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); }

	protected:
		Int_t fFrameId;
		TGTextEntry * fEntry;				//!
		TGPictureButton * fBrowse;	 		//!
		TGFileInfo fFileInfo;	 			//!
		EFileDialogMode fDialogMode;

	ClassDef(TGMrbFileEntry, 1) 	// [GraphUtils] A labelled entry field to browse file names
};

#endif

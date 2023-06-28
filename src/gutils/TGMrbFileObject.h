#ifndef __TGMrbFileObject_h__
#define __TGMrbFileObject_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbFileObject.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbFileObjectCombo   -- a file entry together with
//                                           a combo box containing file objects
//                 TGMrbFileObjectList    -- a file entry together with
//                                           a list box containing file objects
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbFileObject.h,v 1.12 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "TGComboBox.h"

#include "TMrbLofNamedX.h"
#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo
// Purpose:        Define a labelled file entry and an associated combo box
// Description:    Defines a labelled file entry (TGMrbFileEntry) together
//                 with a combo box showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileObjectCombo: public TGCompositeFrame, public TGMrbObject {

	public:
		TGMrbFileObjectCombo(const TGWindow * Parent, const Char_t * Label,				// ctor
							Int_t BufferSize, Int_t FrameId,
							Int_t Width, Int_t Height,
							Int_t EntryWidth, Int_t ComboWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * EntryGC = NULL,
							TGMrbLayout * BrowseGC = NULL,
							TGMrbLayout * ComboGC = NULL,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t EntryOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
							UInt_t ComboOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder);

		~TGMrbFileObjectCombo() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		Bool_t OpenFile(const Char_t * FileName);

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,10,0)
		inline void ClearList() { fCombo->RemoveAll(); };
#else
		inline void ClearList() { fCombo->RemoveEntries(0, 1000); };
#endif

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		Int_t GetSelectionAsString(TString & SelString, Bool_t FullPath = kTRUE) const; // get selected file/object -- string
		Int_t GetSelection(TObjArray & SelArr, Bool_t FullPath = kTRUE) const;			// ... -- array
		void SetSelectionFromString(TString & SelString, Bool_t IsNewFile = kFALSE);  	// set items
		void SetSelection(TObjArray & SelArr, Bool_t IsNewFile = kFALSE); 				// ... -- array

		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		inline const Char_t * GetFileName() { return(fFileName.Data()); };

		void Browse();												// slot methods
		void EntryChanged();
		inline void SelectionChanged(Int_t Selection) { this->SelectionChanged(fFrameId, Selection); };

		void FileChanged(Int_t FrameId, Int_t Selection = 0); 		//*SIGNAL*
		void SelectionChanged(Int_t FrameId, Int_t Selection);		//*SIGNAL*

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fFrameId;

		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB;			//!
			
		TGTextEntry * fEntry;				//!
		TGComboBox * fCombo;				//!
		TGPictureButton * fBrowse;	 		//!

		TString fFileName;					// file name
		TString fText;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TGMrbFileObjectCombo, 1) 	// [GraphUtils] A file browser showing ROOT objects in a combo box
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox
// Purpose:        Define a labelled file entry and an associated listbox
// Description:    Defines a labelled file entry (TGMrbFileEntry) together
//                 with a listbox showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFileObjectListBox: public TGCompositeFrame, public TGMrbObject {

	public:
		enum	{	kBtnBrowse,
					kBtnSingle,
					kBtnRange,
					kBtnClear,
					kBtnApply
				};

	public:
		TGMrbFileObjectListBox(const TGWindow * Parent, const Char_t * Label,				// ctor
							Int_t BufferSize, Int_t FrameId,
							Int_t Width, Int_t Height,
							Int_t EntryWidth, Int_t ListBoxWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * EntryGC = NULL,
							TGMrbLayout * BrowseGC = NULL,
							TGMrbLayout * ListBoxGC = NULL,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t EntryOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
							UInt_t ListBoxOptions = kHorizontalFrame | kSunkenFrame | kDoubleBorder);

		~TGMrbFileObjectListBox() {};				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGListBox * GetListBox() const { return(fListBox); };

		Bool_t OpenFile(const Char_t * FileName);
		void SetList(TObjArray & LofEntries);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,10,0)
		inline void ClearList() { fListBox->RemoveAll(); };
#else
		inline void ClearList() { fListBox->RemoveEntries(0, 1000); };
#endif

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		Int_t GetSelectionAsString(TString & SelString, Bool_t FullPath = kTRUE) const;	// get selected file/object -- string
		Int_t GetSelection(TObjArray & SelArr, Bool_t FullPath = kTRUE) const;			// ... -- array
		void SetSelectionFromString(TString & SelString, Bool_t IsNewFile = kFALSE);  	// set items
		void SetSelection(TObjArray & SelArr, Bool_t IsNewFile = kFALSE); 				// ... -- array

		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		inline const Char_t * GetFileName() { return(fFileName.Data()); };

		void Browse();											// slot methods
		void EntryChanged();
		void ListBoxChanged(Int_t);
		void Single();
		void Range();
		void Clear();
		void Apply();
		inline void SelectionChanged(Int_t Selection) { this->SelectionChanged(fFrameId, Selection); };

		void FileChanged(Int_t FrameId, Int_t Selection = 0); 		//*SIGNAL*
		void SelectionChanged(Int_t FrameId, Int_t Selection);	//*SIGNAL*


		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fFrameId;

		TGVerticalFrame * fEC;				//!
		TGHorizontalFrame * fEB1;			//!
		TGHorizontalFrame * fEB2;			//!
			
		TGTextEntry * fEntry;				//!
		TGListBox * fListBox;				//!
		TGPictureButton * fBrowse;	 		//!

		TGRadioButton * fRBSingle;			//!
		TGRadioButton * fRBRange;			//!
		TGTextButton * fTBClear;			//!
		TGTextButton * fTBApply;			//!

		Int_t fStartIndex;
		TString fFileName;					// file name
		TMrbLofNamedX fLofListItems;		// entries

		TString fText;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TGMrbFileObjectListBox, 1) 	// [GraphUtils] A file browser showing ROOT objects in a list box
};

#endif

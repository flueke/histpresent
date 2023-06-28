#ifndef __TGMrbMacroBrowser_h__
#define __TGMrbMacroBrowser_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbMacroBrowser.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbMacroBrowserComposite   -- manage list of macros (composite frame)
//                 TGMrbMacroBrowserGroup       -- ... (group frame)
//                 TGMrbMacroBrowserPopup       -- ... (popup menu)
//                 TGMrbMacroBrowserTransient   -- ... (transient window)
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbMacroBrowser.h,v 1.28 2008-09-23 10:44:11 Rudolf.Lutter Exp $       
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
#include "TSystem.h"
#include "TROOT.h"
#include "TList.h"
#include "TObjArray.h"
#include "TEnv.h"
#include "TApplication.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGMenu.h"
#include "TGTab.h"
#include "TGFileDialog.h"

#include "RQ_OBJECT.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLofMacros.h"

#include "TGMrbLayout.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbFileObject.h"
#include "TGMrbRadioButton.h"
#include "TGMrbCheckButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbMessageViewer.h"
#include "TGMrbLofKeyBindings.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroArg
// Purpose:        Display a single macro argument
// Description:    Class to describe one singel macro argument
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroArg : public TMrbNamedX {

	friend class TGMrbMacroFrame;

	public:
		enum EGMrbMacroEntryType		{
											kGMrbMacroEntryBitEntry			= BIT(0),
											kGMrbMacroEntryBitMulti			= BIT(1),
											kGMrbMacroEntryBitChkBtn		= BIT(2),
											kGMrbMacroEntryBitRdoBtn		= BIT(3),
											kGMrbMacroEntryBitBeginEnd		= BIT(4),
											kGMrbMacroEntryBitUpDown		= BIT(5),

											kGMrbMacroEntryPlain			= kGMrbMacroEntryBitEntry,
											kGMrbMacroEntryPlain2			= kGMrbMacroEntryBitEntry | kGMrbMacroEntryBitMulti,
											kGMrbMacroEntryPlainC			= kGMrbMacroEntryBitEntry | kGMrbMacroEntryBitChkBtn,
											kGMrbMacroEntryPlainC2			= kGMrbMacroEntryPlainC | kGMrbMacroEntryBitMulti,

											kGMrbMacroEntryUpDown			= kGMrbMacroEntryBitEntry | kGMrbMacroEntryBitUpDown,
											kGMrbMacroEntryUpDown2			= kGMrbMacroEntryUpDown | kGMrbMacroEntryBitMulti,
											kGMrbMacroEntryUpDownC			= kGMrbMacroEntryUpDown | kGMrbMacroEntryBitChkBtn,
											kGMrbMacroEntryUpDownC2 		= kGMrbMacroEntryUpDownC | kGMrbMacroEntryBitMulti,

											kGMrbMacroEntryUpDownX			= kGMrbMacroEntryUpDown | kGMrbMacroEntryBitBeginEnd,
											kGMrbMacroEntryUpDownXC 		= kGMrbMacroEntryUpDownX | kGMrbMacroEntryBitChkBtn,
											kGMrbMacroEntryUpDownX2 		= kGMrbMacroEntryUpDownX | kGMrbMacroEntryBitMulti,
											kGMrbMacroEntryUpDownXC2		= kGMrbMacroEntryUpDownXC | kGMrbMacroEntryBitMulti,

											kGMrbMacroEntryYesNo			= BIT(6),
											kGMrbMacroEntryRadio			= BIT(7),
											kGMrbMacroEntryCheck			= BIT(8),
											kGMrbMacroEntryText 			= BIT(9),
											kGMrbMacroEntryCombo			= BIT(10),
											kGMrbMacroEntryFile 			= BIT(11),
											kGMrbMacroEntryFObjCombo		= BIT(12),
											kGMrbMacroEntryFObjListBox		= BIT(13),

											kGMrbMacroEntryComment			= BIT(14),
											kGMrbMacroEntrySection			= BIT(15),
											kGMrbMacroEntryGroup			= BIT(16),
											kGMrbMacroEntryPad 				= BIT(17),
											kGMrbMacroEntryTab				= BIT(18),
											kGMrbMacroEntryFrame			= BIT(20)
										};

		enum EGMrbMacroArgType		{
											kGMrbMacroArgInt,
											kGMrbMacroArgUInt,
											kGMrbMacroArgFloat,
											kGMrbMacroArgDouble,
											kGMrbMacroArgChar,
											kGMrbMacroArgBool,
											kGMrbMacroArgObjArr
										};

		enum EGMrbMacroArgBase			{
											kGMrbMacroArgBaseDec	=	BIT(0),
											kGMrbMacroArgBaseBin	=	BIT(1),
											kGMrbMacroArgBaseOct	=	BIT(2),
											kGMrbMacroArgBaseHex	=	BIT(3)
										};

		enum EGMrbMacroArgOrientation	{	kGMrbMacroEntryHorizontal	=	BIT(0),
											kGMrbMacroEntryVertical 	=	BIT(1)
										};

		enum							{	kGMrbMacroEntryNo		=	0	};
		enum							{	kGMrbMacroEntryYes		=	1	};
		enum							{	kGMrbMacroNofFileTypes	=	50	};

	public:
		TGMrbMacroArg() : TMrbNamedX(0, "Arg0") { 		// default ctor
			fFileInfo = new TGFileInfo();
		};
		TGMrbMacroArg(Int_t Number) : TMrbNamedX(Number, Form("Arg%d", Number)) {		// ctor
			fFileInfo = new TGFileInfo();
		};
		TGMrbMacroArg(Int_t Number, const Char_t * Name) : TMrbNamedX(Number, Name) {
			fFileInfo = new TGFileInfo();
		};
		virtual ~TGMrbMacroArg() { delete fFileInfo; };						// default dtor

		const Char_t * GetResource(TString & Resource, const Char_t * ResourceName) const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TMrbNamedX * fType;				// argument type
		TMrbNamedX * fEntryType; 		// entry type;

		TString fCurrentValue;		// current
		TString fDefaultValue;		// default

		Bool_t fAddLofValues;			// add list of values on exec?

		TGLabel * fComment; 			// gui representation
		TGLabel * fSection;
		TGLabel * fTab;
		TGGroupFrame * fGroup;
		TGVerticalFrame * fFrame;
		TGMrbLabelEntry * fEntry;
		TGMrbRadioButtonList * fRadio;
		TGMrbCheckButtonList * fCheck;
		TGMrbTextButtonList * fText;
		TGMrbLabelCombo * fCombo;
		TGMrbFileEntry * fFile;
		TGMrbFileObjectCombo * fFObjCombo;
		TGMrbFileObjectListBox * fFObjListBox;

		TMrbLofNamedX fButtons; 		// list of buttons

		Int_t fEntryWidth;				// dimensions
		Int_t fNofEntryFields;
		Double_t fLowerLimit;
		Double_t fUpperLimit;
		Double_t fIncrement;
		Int_t fOrientation;
		Int_t fNofCL;
		TString fExecFunct;

		TGFileInfo * fFileInfo;			// file info to be used with FileEntry
		TObjArray fFileTypes;
		const Char_t * fPtrFileTypes[2 * TGMrbMacroArg::kGMrbMacroNofFileTypes];

 	ClassDef(TGMrbMacroArg, 0)		// [GraphUtils] Display an argument of a ROOT macro
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame
// Purpose:        Dispay and exec a macro
// Description:    A transient frame to display macro args and exec macro
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroFrame : public TGTransientFrame {

	RQ_OBJECT("TGMrbMacroFrame")

	public:
		// ids to dispatch over X events
		enum EGMrbMacroId				{	kGMrbMacroIdExec,
											kGMrbMacroIdReset,
											kGMrbMacroIdClose,
											kGMrbMacroIdQuit,
											kGMrbMacroIdModifyHeader,
											kGMrbMacroIdModifySource,
											kGMrbMacroIdExecClose
										};

		enum							{	kUserButtonStart	=	BIT(16)	};

		enum							{	kFrameWidth 		=	1200 	};
		enum							{	kLineHeight 		=	20		};

		enum							{	kGMrbMacroNofFrames 	=	10	};
		enum							{	kGMrbMacroNofSubframes	=	10	};
		enum							{	kGMrbMacroNofTabs		=	10	};

	public:
		TGMrbMacroFrame(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro, UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		~TGMrbMacroFrame() { fHeap.Delete(); };

		Bool_t ResetMacroArgs();						// reset arguments
		Bool_t ExecMacro(Bool_t UserStart = kFALSE);	// exec macro
		Bool_t ModifyMacroHeader(); 					// modify header
		Bool_t ModifyMacroSource(); 					// modify source

		inline TMrbLofNamedX * GetLofArgTypes() { return(&fLofArgTypes); };
		inline TMrbLofNamedX * GetLofEntryTypes() { return(&fLofEntryTypes); };
		inline TObjArray * GetLofMacroArgs() { return(&fLofMacroArgs); };

		TGMrbMacroArg * FindArgByName(const Char_t * ArgName);
		Bool_t SetArgValue(const Char_t * ArgName, Int_t Value, Int_t EntryNo = 0);
		Bool_t SetArgValue(const Char_t * ArgName, Bool_t Value, Int_t EntryNo = 0);
		Bool_t SetArgValue(const Char_t * ArgName, Double_t Value, Int_t EntryNo = 0);
		Bool_t SetArgValue(const Char_t * ArgName, const Char_t * Value, Int_t EntryNo = 0);
		Bool_t SetArgValue(const Char_t * ArgName, TObjArray & Selection);
		Bool_t GetArgValue(const Char_t * ArgName, Int_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, Bool_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, Double_t & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, TString & Value, Int_t EntryNo = 0);
		Bool_t GetArgValue(const Char_t * ArgName, TObjArray & Selection);

		Bool_t SetArgCheck(const Char_t * ArgName, UInt_t Check);
		Bool_t GetArgCheck(const Char_t * ArgName, UInt_t & Check);
		Bool_t ArgIsChecked(const Char_t * ArgName);

		void ProcessSignal(Int_t FrameId, Int_t Signal); 			// slot methods
		void ActionButtonPressed(Int_t FrameId, Int_t Signal);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t LoadMacro();

	protected:
		TList fHeap;

		TMrbNamedX * fMacro;

		TGGroupFrame * fMacroInfo;
		TGLabel * fMacroName;
		TGLabel * fMacroPath;
		TGLabel * fMacroTitle;
		TGGroupFrame * fMacroArgs;
		TGTab * fMacroTab;
		TGCompositeFrame * fMacroLofTabs[kGMrbMacroNofTabs];
		TGVerticalFrame * fMacroLofTabFrames[kGMrbMacroNofTabs];
		TGHorizontalFrame * fMacroLofFrames[kGMrbMacroNofFrames];
		TGVerticalFrame * fMacroLofSubframes[kGMrbMacroNofSubframes];
		TGMrbTextButtonGroup * fAction;

		TMrbLofNamedX fLofArgTypes;
		TMrbLofNamedX fLofEntryTypes;

		TMrbLofNamedX fLofActions;
		TMrbLofNamedX fLofActionsM;
		TMrbLofNamedX fLofActionsX;

		TObjArray fLofMacroArgs;	// list of arguments

		Bool_t fMacroLoaded;

	ClassDef(TGMrbMacroFrame, 0)	// [GraphUtils] A frame containing definitions of a ROOT macro
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit
// Purpose:        Edit macro layout
// Description:    A transient frame to modify macro args
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroEdit : public TGTransientFrame {

	public:
		// button ids
		enum EGMrbMacroEditButtons		{	kGMrbMacroAclicPlus,
											kGMrbMacroAclicPlusPlus,
											kGMrbMacroAclicPlusG,
											kGMrbMacroAclicPlusPlusG,
											kGMrbMacroMayModify,
											kGMrbMacroDontModify,
											kGMrbMacroArglistOnly,
											kGMrbMacroGuiPtrOnly,
											kGMrbMacroArglistAndGui,
											kGMrbMacroUserStartOn,
											kGMrbMacroUserStartOff
										};

		// ids to dispatch over X events
		enum EGMrbMacroEditId			{	kGMrbMacroEditCurrentArg,
											kGMrbMacroEditIdFirstArg,
											kGMrbMacroEditIdLastArg,
											kGMrbMacroEditIdRemoveArg,
											kGMrbMacroEditIdResetArg,
											kGMrbMacroEditIdNewArgBefore,
											kGMrbMacroEditIdNewArgAfter,

											kGMrbMacroEditIdResetAll,
											kGMrbMacroEditIdResetHeader,
											kGMrbMacroEditIdResetAllArgs,
											kGMrbMacroEditIdSave,
											kGMrbMacroEditIdClose
										};

		enum							{	kEntryId		=	1000	};

		enum							{	kFrameWidth 	=	630 	};
		enum							{	kLineHeight 	=	20		};

		enum							{	kMaxNofArgs 	=	50		};

		enum EGMrbMacroTag	 			{	kMacroTagFile,
											kMacroTagName,
											kMacroTagTitle,
											kMacroTagProc,
											kMacroTagAuthor,
											kMacroTagMailAddr,
											kMacroTagUrl,
											kMacroTagCreationDate,
											kMacroTagSyntax,
											kMacroTagArguments,
											kMacroTagExec,
											kMacroTagIncludeHFiles,
											kMacroTagArgEnums,
											kMacroTagEnums,
											kMacroTagCode
										};

	public:
		TGMrbMacroEdit(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro, UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		~TGMrbMacroEdit() { fHeap.Delete(); };

		Bool_t SwitchToArg(Int_t FrameId, Int_t EntryNo = -1);				// slot methods
		void ActionButtonPressed(Int_t FrameId, Int_t Button);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t UpdateArg(Int_t ArgNo = -1);					// update GUI for a given argument
		Bool_t UpdateNofArgs(); 							// update number of arguments in GUI
		Bool_t StoreHeader();								// store header data
		Bool_t StoreArg(Int_t ArgNo = -1);					// store GUI settings for (current) arg
		Bool_t RemoveArg(Int_t ArgNo = -1);					// remove (current) argument and its env defs
		Bool_t InsertArg(Int_t ArgNo);				 		// insert a new argument
		Bool_t RestoreArg(Int_t ArgNo = -1);				// restore original settings for (current) argument
		Bool_t RestoreHeader(); 							// reset header env (Name, Title, Path ...) to original values
		Bool_t RestoreAllArgs();							// reset all args to original values
		Bool_t ChangeEnv(Int_t ArgNo, Bool_t Delete);		// change current environment by deleting/inserting this argument
		Bool_t SaveMacro(const Char_t * NewFile, const Char_t * OldFile);	// save settings to a file
		Int_t ExtractEnums(TMrbLofNamedX & LofEnums, Int_t ArgNo);	// extract enum defs

		Bool_t CopyUserCode(const Char_t * NewFile, const Char_t * OrgFile);		// copy user code fromn original to new file
		Bool_t CopyUserCode(ofstream & Out, const Char_t * OutFile, ifstream & In, const Char_t * InFile, const Char_t * Tag, Bool_t CopyFlag);

	protected:
		TList fHeap;

		TMrbNamedX * fMacro;

		TGGroupFrame * fMacroInfo;
		TGMrbLabelEntry * fMacroName;
		TGMrbLabelEntry * fMacroTitle;
		TGMrbLabelEntry * fMacroRcFile;
		TGMrbLabelEntry * fMacroProc;
		TGMrbLabelEntry * fMacroWidth;

		TGMrbRadioButtonList * fMacroAclic;
		TGMrbRadioButtonList * fMacroModify;
		TGMrbRadioButtonList * fMacroGuiPtrMode;
		TGMrbRadioButtonList * fMacroUserStart;

		TGMrbLabelEntry * fMacroNofArgs;

		TGGroupFrame * fMacroArg;
		TGMrbLabelEntry * fArgNumber;
		TGMrbTextButtonList * fArgAction;

		TGGroupFrame * fMacroLayout;
		TGMrbLabelEntry * fArgName;
		TGMrbLabelEntry * fArgTitle;
		TGMrbLabelCombo * fArgType;
		TGMrbLabelCombo * fArgEntryType;
		TGMrbLabelEntry * fArgNofEntryFields;
		TGMrbLabelEntry * fArgEntryWidth;
		TGMrbLabelEntry * fArgDefaultValue;
		TGMrbLabelEntry * fArgValues;
		TGMrbRadioButtonList * fArgAddLofValues;
		TGMrbRadioButtonList * fArgBase;
		TGMrbLabelEntry * fArgLimits;
		TGMrbRadioButtonList * fArgOrientation;
		TGMrbLabelEntry * fArgNofCL;
		TGMrbLabelEntry * fArgExecFunct;

		TGMrbTextButtonGroup * fAction;

		TMrbLofNamedX fLofAclicModes;
		TMrbLofNamedX fLofModifyModes;
		TMrbLofNamedX fLofArglistGuiModes;
		TMrbLofNamedX fLofUserStartModes;

		TMrbLofNamedX fLofArgTypes;
		TMrbLofNamedX fLofEntryTypes;
		TMrbLofNamedX fLofOrientations;
		TMrbLofNamedX fLofIntegerBases;
		TMrbLofNamedX fYesNo;

		TMrbLofNamedX fLofArgActions;
		TMrbLofNamedX fLofActions;

		Int_t fNofArgs; 					// actual number of arguments
		TGMrbMacroArg fCurrentArg;			// current argument number
		TEnv * fOriginalEnv;				// original settings
		TEnv * fCurrentEnv; 				// current settings
		TMrbLofNamedX fLofEnvNames; 		// list of legal env names

	ClassDef(TGMrbMacroEdit, 0)		// [GraphUtils] Edit macro args and layout
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList
// Purpose:        A list of buttons, one per macro
// Description:    A list of text buttons, one per macro,
//                 showing macro name & title
//                 Common subclass to be used by TGMrbBrowserVertical,
//                 TGMrbBrowserGroup, and TGMrbBrowserTransient
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroList : public TGVerticalFrame {

	public:
		TGMrbMacroList(const TGWindow * Parent, TMrbLofMacros * MacroList, UInt_t Width, UInt_t Height,
																TGMrbLayout * FrameGC, TGMrbLayout * ButtonGC,
																UInt_t FrameOptions = kChildFrame | kVerticalFrame,
																UInt_t ButtonOptions = kRaisedFrame | kDoubleBorder);
		~TGMrbMacroList() { fHeap.Delete(); };

		void SelectMacro(Int_t FrameId, Int_t Selection);	 	// slot method

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TGMrbTextButtonList * fButtonList;

		TMrbLofMacros * fLofMacros;
		TMrbLofNamedX fLofButtons;

	ClassDef(TGMrbMacroList, 0) 	// [GraphUtils] A list of ROOT macros
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain
// Purpose:        Manage a list of macros
// Description:    A main frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserMain : public TGMainFrame {

	public:
		enum EGMrbMacroMainId	{	kGMrbMacroMenuFileNew,
									kGMrbMacroMenuFileOpen,
									kGMrbMacroMenuFileExit,
									kGMrbMacroMenuViewErrors,
									kGMrbMacroMenuHelpContents,
									kGMrbMacroMenuHelpAbout
								};

	public:
		TGMrbMacroBrowserMain(const TGWindow * Parent, TMrbLofMacros * LofMacros,
												UInt_t Width, UInt_t Height);

		virtual ~TGMrbMacroBrowserMain() { fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
		void MenuSelect(Int_t Selection);			// slot method

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		inline void CloseWindow() { TGMainFrame::CloseWindow(); gApplication->Terminate(0); };

	protected:
		void PopupMessageViewer();
	
	protected:
		TList fHeap;
		TGPopupMenu * fMenuFile;
		TGPopupMenu * fMenuView;
		TGPopupMenu * fMenuHelp;
		TGMenuBar * fMenuBar;

		TGMrbLayout * fFrameGC;
		TGMrbLayout * fButtonGC;

		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

		TGMrbMessageViewer * fMsgViewer;

		TGMrbLofKeyBindings fKeyBindings;
		
	ClassDef(TGMrbMacroBrowserMain,0)		// [GraphUtils] Show ROOT macros in a main frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserVertical
// Purpose:        Manage a list of macros
// Description:    A vertical frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserVertical : public TGVerticalFrame {

	public:
		TGMrbMacroBrowserVertical(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height, UInt_t Options);

		virtual ~TGMrbMacroBrowserVertical() {};

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserVertical,0)	// [GraphUtils] Show ROOT macros in a vertical frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Manage a list of macros
// Description:    Group frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserGroup : public TGGroupFrame {

	public:
		TGMrbMacroBrowserGroup(const TGWindow * Parent, const Char_t * Title, TMrbLofMacros * LofMacros, TGMrbLayout * Layout);

		virtual ~TGMrbMacroBrowserGroup() {};

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserGroup,0)		// [GraphUtils] Show ROOT macros in a group frame
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup
// Purpose:        Manage a list of macros
// Description:    Popup menu showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserPopup : public TGPopupMenu {

	public:
		enum EGMrbMacroPopupId	{	kGMrbPopupNew	= 1000,
									kGMrbPopupOpen
								};

	public:
		TGMrbMacroBrowserPopup(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width = 10, UInt_t Height = 10);

		virtual ~TGMrbMacroBrowserPopup() { fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		void MenuSelect(Int_t Signal);			// slot method

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;

	ClassDef(TGMrbMacroBrowserPopup,0)		// [GraphUtils] Show ROOT macros in a popup menu
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserTransient
// Purpose:        Manage a list of macros
// Description:    Transient frame showing a list of macros
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbMacroBrowserTransient : public TGTransientFrame {

	public:
		TGMrbMacroBrowserTransient(const TGWindow * Parent, const TGWindow * Main, const Char_t * Title, TMrbLofMacros * LofMacros,
															UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);

		virtual ~TGMrbMacroBrowserTransient() {fHeap.Delete(); };

		inline TMrbLofMacros * GetLofMacros() const { return(fLofMacros); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TList fHeap;
		TMrbLofMacros * fLofMacros;
		TGMrbMacroList * fMacroList;

	ClassDef(TGMrbMacroBrowserTransient,0)		// [GraphUtils] Show ROOT macros in a transient frame
};

#endif

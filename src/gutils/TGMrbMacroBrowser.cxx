//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbMacroBrowser.cxx
// Purpose:        MARaBOU graphic utilities: browser to manage root macros
// Description:    Implements class methods to browse & execute root macros
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbMacroBrowser.cxx,v 1.53 2011-07-26 08:41:50 Marabou Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbMacroBrowser.gif>
*/
//End_Html
// Generated Code:
//Begin_Html
/*
<img src=gutils/TGMrbMacroBrowserCode.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TVirtualX.h"
#include "TEnv.h"
#include "TObjArray.h"
#include "TObjString.h"
//#include "Token.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"
#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "TGMsgBox.h"

#include "TGMrbMacroBrowser.h"

#include "SetColor.h"

ClassImp(TGMrbMacroBrowserMain)
ClassImp(TGMrbMacroBrowserVertical)
ClassImp(TGMrbMacroBrowserGroup)
ClassImp(TGMrbMacroBrowserPopup)
ClassImp(TGMrbMacroBrowserTransient)
ClassImp(TGMrbMacroFrame)
ClassImp(TGMrbMacroEdit)
ClassImp(TGMrbMacroArg)
ClassImp(TGMrbMacroList)

const SMrbNamedX kGMrbMacroLofAclicModes[] =
			{
				{TGMrbMacroEdit::kGMrbMacroAclicPlus, 		"+",		"Compile if needed"						},
				{TGMrbMacroEdit::kGMrbMacroAclicPlusPlus, 	"++",		"Force compiling"						},
				{TGMrbMacroEdit::kGMrbMacroAclicPlusG,		"+g",	 	"Compile if needed, include debug info"	},
				{TGMrbMacroEdit::kGMrbMacroAclicPlusPlusG,	"++g",	 	"Force compiling, include debug info"	},
				{0, 										NULL,		NULL									}
			};

const SMrbNamedX kGMrbMacroLofModifyModes[] =
			{
				{TGMrbMacroEdit::kGMrbMacroMayModify, 		"yes",		"User may modify header/source"	},
				{TGMrbMacroEdit::kGMrbMacroDontModify,	 	"no",		"User is not allowed to modify"	},
				{0, 										NULL,		NULL							}
			};

const SMrbNamedX kGMrbMacroLofArgListGuiModes[] =
			{
				{TGMrbMacroEdit::kGMrbMacroArglistOnly, 	"ArgList",			"Xfer arguments via arg list only"				},
				{TGMrbMacroEdit::kGMrbMacroGuiPtrOnly,	 	"GuiPtr",			"Xfer arguments via pointer to GUI only"		},
				{TGMrbMacroEdit::kGMrbMacroArglistAndGui,	"ArgList+GuiPtr",	"Add pointer to GUI at end of argument list"	},
				{0, 										NULL,				NULL											}
			};

const SMrbNamedX kGMrbMacroLofUserStartModes[] =
			{
				{TGMrbMacroEdit::kGMrbMacroUserStartOn,	 	"on",		"Macro will be started by user's start button"	},
				{TGMrbMacroEdit::kGMrbMacroUserStartOff, 	"off",		"User has to press \"Execute\" to start"		},
				{0, 										NULL,		NULL											}
			};

const SMrbNamedX kGMrbMacroLofActionsModify[] =
			{
				{TGMrbMacroFrame::kGMrbMacroIdModifyHeader, "Modify header",	"Modify macro layout"				},
				{TGMrbMacroFrame::kGMrbMacroIdModifySource, "Modify source",	"Modify macro source"				},
				{TGMrbMacroFrame::kGMrbMacroIdExec, 		"Execute",			"Execute macro"						},
				{TGMrbMacroFrame::kGMrbMacroIdExecClose, 	"Exec + Close", 	"Execute macro & close window"		},
				{TGMrbMacroFrame::kGMrbMacroIdReset,		"Reset",			"Reset arguments to default values"	},
				{TGMrbMacroFrame::kGMrbMacroIdClose,		"Close",			"Close window" 				 		},
				{TGMrbMacroFrame::kGMrbMacroIdQuit, 		"Quit",				"Exit from ROOT" 				 	},
				{0, 										NULL,				NULL								}
			};

const SMrbNamedX kGMrbMacroLofActionsUserStart[] =
			{
				{TGMrbMacroFrame::kGMrbMacroIdModifyHeader, "Modify header",	"Modify macro layout"				},
				{TGMrbMacroFrame::kGMrbMacroIdModifySource, "Modify source",	"Modify macro source"				},
				{TGMrbMacroFrame::kGMrbMacroIdReset,		"Reset",			"Reset arguments to default values"	},
				{TGMrbMacroFrame::kGMrbMacroIdClose,		"Close",			"Close window" 				 		},
				{TGMrbMacroFrame::kGMrbMacroIdQuit, 		"Quit",				"Exit from ROOT" 				 	},
				{0, 										NULL,				NULL								}
			};

const SMrbNamedX kGMrbMacroLofActions[] =
			{
				{TGMrbMacroFrame::kGMrbMacroIdExec, 		"Execute",			"Execute macro"						},
				{TGMrbMacroFrame::kGMrbMacroIdExecClose, 	"Exec + Close", 	"Execute macro & close window"		},
				{TGMrbMacroFrame::kGMrbMacroIdReset,		"Reset",			"Reset arguments to default values"	},
				{TGMrbMacroFrame::kGMrbMacroIdClose,		"Close",			"Close window" 				 		},
				{TGMrbMacroFrame::kGMrbMacroIdQuit,			"Quit",				"Exit from ROOT" 				 	},
				{0, 										NULL,				NULL								}
			};

const SMrbNamedX kGMrbMacroLofEditArgActions[] =
			{
				{TGMrbMacroEdit::kGMrbMacroEditIdRemoveArg, 	"Remove", 		"Remove argument"					},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetArg, 		"Reset",		"Reset argument defs to defaults" 	},
				{TGMrbMacroEdit::kGMrbMacroEditIdNewArgBefore,	"New before",	"Insert argument BEFORE current one"	},
				{TGMrbMacroEdit::kGMrbMacroEditIdNewArgAfter,	"New after",	"Insert argument AFTER current one"	},
				{0, 											NULL,			NULL								}
			};

const SMrbNamedX kGMrbMacroLofEditActions[] =
			{
				{TGMrbMacroEdit::kGMrbMacroEditIdResetAll, 		"Reset ALL",	"Reset header AND arguments"		},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetHeader, 	"Reset header",	"Reset header only" 				},
				{TGMrbMacroEdit::kGMrbMacroEditIdResetAllArgs, 	"Reset args",	"Reset arguments only"				},
				{TGMrbMacroEdit::kGMrbMacroEditIdSave, 			"Save macro",	"Save macro to file"				},
				{TGMrbMacroEdit::kGMrbMacroEditIdClose,			"Close",		"Close window"						},
				{0, 											NULL,			NULL								}
			};

const SMrbNamedX kGMrbMacroLofArgTypes[] =
				{
					{TGMrbMacroArg::kGMrbMacroArgInt, 		"Int_t",		"Integer value"					},
					{TGMrbMacroArg::kGMrbMacroArgUInt, 		"UInt_t",		"Unsigned integer value"		},
					{TGMrbMacroArg::kGMrbMacroArgFloat, 	"Float_t",		"Floating point value"			},
					{TGMrbMacroArg::kGMrbMacroArgDouble,	"Double_t", 	"Double precision value"		},
					{TGMrbMacroArg::kGMrbMacroArgChar,		"Char_t *",  	"String of characters"			},
					{TGMrbMacroArg::kGMrbMacroArgBool,		"Bool_t",		"Boolean value" 				},
					{TGMrbMacroArg::kGMrbMacroArgObjArr,	"TObjArray *",	"Array of ROOT objects" 		},
					{0, 									NULL,			NULL							}
				};

const SMrbNamedX kGMrbMacroLofEntryTypes[] =
				{
					{TGMrbMacroArg::kGMrbMacroEntryPlain, 	"Entry",		"Plain entry (text or numeric)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryPlain2, 	"Entry-M",		"Multiple entry (text or numeric)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryPlainC, 	"Entry-C",		"Plain entry (text or numeric) with check button" 		},
					{TGMrbMacroArg::kGMrbMacroEntryPlainC2, "Entry-MC",		"Multiple entry (text or numeric) with check button" 		},
					{TGMrbMacroArg::kGMrbMacroEntryUpDown,	"UpDown",		"Entry (numeric) with Up/Down buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDown2,	"UpDown-M",		"Multiple entry (numeric) with Up/Down buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownC,	"UpDown-C", 	"Entry (numeric) with Up/Down and check buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownC2,"UpDown-MC",	"Multiple entry (numeric) with Up/Down and check buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownX,	"UpDown-X", 	"Entry (numeric) with Begin/Up/Down/End buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownX2,"UpDown-MX",	"Multiple entry (numeric) with Begin/Up/Down/End buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryUpDownXC2,"UpDown-MXC",	"Multiple entry (numeric) with Begin/Up/Down/End and check buttons"	},
					{TGMrbMacroArg::kGMrbMacroEntryYesNo, 	"YesNo",		"Yes/No button" 							},
					{TGMrbMacroArg::kGMrbMacroEntryRadio,	"Radio",		"List of (exclusive) radio buttons" 				},
					{TGMrbMacroArg::kGMrbMacroEntryCheck,	"Check",		"List of (inclusive) check buttons" 				},
					{TGMrbMacroArg::kGMrbMacroEntryText,	"TextButton",	"List of text buttons"	 							},
					{TGMrbMacroArg::kGMrbMacroEntryCombo,	"Combo",		"Combo box" 							},
					{TGMrbMacroArg::kGMrbMacroEntryFile,	"File", 		"File dialog box"						},
					{TGMrbMacroArg::kGMrbMacroEntryFObjCombo,"FObjCombo",	"Combo box for ROOT objects"			},
					{TGMrbMacroArg::kGMrbMacroEntryFObjListBox,"FObjListBox","List box for ROOT objects"			},
					{TGMrbMacroArg::kGMrbMacroEntryComment, "Comment",		"Comment (no input)" 		},
					{TGMrbMacroArg::kGMrbMacroEntrySection, "Section",		"Section (no input)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryGroup,	"Group",		"Group frame (no input)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryPad, 	"Pad",			"New pad to place entries (no input)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryTab, 	"Tab",			"New tab to place entries (no input)" 		},
					{TGMrbMacroArg::kGMrbMacroEntryFrame, 	"Frame",		"New frame to place entries (no input)" 		},
					{0, 									NULL,			NULL									}
				};

const SMrbNamedXShort kGMrbMacroLofOrientations[] =
							{
								{TGMrbMacroArg::kGMrbMacroEntryHorizontal, 	"horizontal"	},
								{TGMrbMacroArg::kGMrbMacroEntryVertical,	"vertical"		},
								{0, 										NULL			}
							};

const SMrbNamedXShort kGMrbMacroLofIntegerBases[] =
							{
								{TGMrbMacroArg::kGMrbMacroArgBaseDec,	 	"dec"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseBin,		"bin"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseOct,		"oct"			},
								{TGMrbMacroArg::kGMrbMacroArgBaseHex,		"hex"			},
								{0, 										NULL			}
							};

const SMrbNamedXShort kGMrbMacroYesNoButtons[] =
							{
								{TGMrbMacroArg::kGMrbMacroEntryNo,		"No" 		},
								{TGMrbMacroArg::kGMrbMacroEntryYes, 	"Yes"		},
								{0, 									NULL		}
							};


const SMrbNamedXShort kGMrbMacroLofTags[] =
							{
								{TGMrbMacroEdit::kMacroTagFile,				"MACRO_FILE"				},
								{TGMrbMacroEdit::kMacroTagName,				"MACRO_NAME"				},
								{TGMrbMacroEdit::kMacroTagTitle, 			"MACRO_TITLE"				},
								{TGMrbMacroEdit::kMacroTagAuthor,			"AUTHOR"					},
								{TGMrbMacroEdit::kMacroTagMailAddr,			"MAILADDR"					},
								{TGMrbMacroEdit::kMacroTagUrl,				"URL"						},
								{TGMrbMacroEdit::kMacroTagCreationDate,		"CREATION_DATE" 			},
								{TGMrbMacroEdit::kMacroTagSyntax,			"MACRO_SYNTAX"				},
								{TGMrbMacroEdit::kMacroTagArguments, 		"MACRO_ARGUMENTS"			},
								{TGMrbMacroEdit::kMacroTagExec,				"MACRO_EXEC"				},
								{TGMrbMacroEdit::kMacroTagIncludeHFiles,	"MACRO_INCLUDE_H_FILES"		},
								{TGMrbMacroEdit::kMacroTagArgEnums,			"MACRO_ARG_ENUMS"			},
								{TGMrbMacroEdit::kMacroTagEnums,			"MACRO_ENUMS"				},
								{TGMrbMacroEdit::kMacroTagCode,				"MACRO_CODE"				},
								{0, 											NULL					}
							};

// file types
const char * macroFileTypes[] =	{	"ROOT macros", 	"*.C",
									"All files",	"*",
                            		NULL,			NULL
								};

extern TMrbLogger * gMrbLog;		// access to message logger 

TGMrbMacroBrowserMain::TGMrbMacroBrowserMain(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height) : TGMainFrame(Parent, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain
// Purpose:        Browse & execute root macros in a main frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Displays & executes root macros in a main frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fMsgViewer = NULL;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fButtonGC = new TGMrbLayout(font, black, gray);
	HEAP(fButtonGC);
	fFrameGC = new TGMrbLayout(font, black, gray);
	HEAP(fFrameGC);

	fLofMacros = LofMacros;

	Int_t nofMacros = fLofMacros->GetEntriesFast();

	if (nofMacros > 1) {
		fMenuFile = new TGPopupMenu(fClient->GetRoot());
		HEAP(fMenuFile);

		fMenuFile->AddEntry("New ... Ctrl-n", kGMrbMacroMenuFileNew);
		fMenuFile->AddEntry("Open ... Ctrl-o", kGMrbMacroMenuFileOpen);
		fMenuFile->AddSeparator();
		fMenuFile->AddEntry("Exit ... Ctrl-q", kGMrbMacroMenuFileExit);
		fMenuFile->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

		fMenuView = new TGPopupMenu(fClient->GetRoot());
		HEAP(fMenuView);

		fMenuView->AddEntry("&Errors", kGMrbMacroMenuViewErrors);
		fMenuView->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

		fMenuHelp = new TGPopupMenu(fClient->GetRoot());
		HEAP(fMenuHelp);

		fMenuHelp->AddEntry("&Contents", kGMrbMacroMenuHelpContents);
		fMenuHelp->AddEntry("&About", kGMrbMacroMenuHelpAbout);
		fMenuHelp->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

		fMenuBar = new TGMenuBar(this, 500, 20, kHorizontalFrame | kRaisedFrame);
		HEAP(fMenuBar);

		TGLayoutHints * menuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,0, 4, 0, 0);
		HEAP(menuBarItemLayout);
		fMenuBar->AddPopup("&File", fMenuFile, menuBarItemLayout);
		fMenuBar->AddPopup("&View", fMenuView, menuBarItemLayout);
		TGLayoutHints * menuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
		HEAP(menuBarHelpLayout);
		fMenuBar->AddPopup("&Help", fMenuHelp, menuBarHelpLayout);

		TGLayoutHints * menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
		HEAP(menuBarLayout);
		this->AddFrame(fMenuBar, menuBarLayout);

		fMenuBar->ChangeBackground(gray);

		TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
		HEAP(frameLayout);
		fFrameGC->SetLH(frameLayout);

		TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
		HEAP(buttonLayout);
		fButtonGC->SetLH(buttonLayout);

		fMacroList = new TGMrbMacroList(this, fLofMacros, Width - 10, Height - 10, fFrameGC, fButtonGC);
		HEAP(fMacroList);
		this->AddFrame(fMacroList, fFrameGC->LH());

		this->ChangeBackground(gray);

		fKeyBindings.SetParent(this);
		fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
		fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionExit);
		fKeyBindings.BindKey("Ctrl-n", TGMrbLofKeyBindings::kGMrbKeyActionNew);
		fKeyBindings.BindKey("Ctrl-o", TGMrbLofKeyBindings::kGMrbKeyActionOpen);
	
		if (gEnv->GetValue("MacroBrowser.ViewMessages", kFALSE) == 1
		||	gEnv->GetValue("TGMrbMessageViewer.Popup", kFALSE) == 1) this->PopupMessageViewer();

		SetWindowName("MacroBrowser: Display & exec ROOT macros");

		MapSubwindows();

		Resize(GetDefaultSize());
		Int_t w = fMenuBar->GetDefaultWidth();
		if (w < (Int_t) fMacroList->GetDefaultWidth()) w = fMacroList->GetDefaultWidth();
		Resize(w + 10, fMenuBar->GetDefaultHeight() + fMacroList->GetDefaultHeight() + 5);

		MapWindow();
	} else {
		TMrbNamedX * macro = fLofMacros->FirstMacro();
		if (macro) {
			TGMrbMacroFrame * mf = new TGMrbMacroFrame(fClient->GetRoot(), this, fLofMacros->FirstMacro(), 100, 100);
			if (mf->IsZombie()) delete mf;
		}
	}
}

void TGMrbMacroBrowserMain::MenuSelect(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain::MenuSelect
// Purpose:        Message handler
// Arguments:      Int_t Selection    -- item number
// Results:        =-
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroBrowserMain.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;
	TGFileInfo fi;
	TMrbLofMacros lofMacros;

	switch (Selection) {
		case TGMrbMacroBrowserMain::kGMrbMacroMenuFileNew:
			macro = new TMrbNamedX(0, "macro.C");
			macroEnv = new TEnv(".new");
			macroEnv->SetValue("NofArgs=1");
			macro->AssignObject(macroEnv);
			new TGMrbMacroEdit(fClient->GetRoot(), this, macro, 100, 100);
			break;
		case TGMrbMacroBrowserMain::kGMrbMacroMenuFileOpen:
			fi.fFileTypes = (const char **) macroFileTypes;
			new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
			if (fi.fFilename != NULL && *fi.fFilename != '\0') {
				if (lofMacros.AddMacro(fi.fFilename)) {
					macro = lofMacros.FirstMacro();
					if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
				}
			}
			break;
		case TGMrbMacroBrowserMain::kGMrbMacroMenuViewErrors:
			if (fMsgViewer != NULL) delete fMsgViewer;
			this->PopupMessageViewer();
			break;
		case TGMrbMacroBrowserMain::kGMrbMacroMenuFileExit:
			this->CloseWindow();
			break;
	}
}
				
void TGMrbMacroBrowserMain::PopupMessageViewer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserMain::PopupMessageViewer
// Purpose:        Create a message viewer window
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


	fMsgViewer = new TGMrbMessageViewer(fClient->GetRoot(), this,
									"MacroBrowser: View (error) messages", 800, 300, gMrbLog, kTRUE, NULL);
	fMsgViewer->Disable(TMrbLogMessage::kMrbMsgMessage);
	fMsgViewer->Enable(TMrbLogMessage::kMrbMsgError);
	fMsgViewer->AddEntries();
}

TGMrbMacroBrowserVertical::TGMrbMacroBrowserVertical(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height, UInt_t Options) :
															TGVerticalFrame(Parent, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserVertical
// Purpose:        Browse & execute root macros in a vertical frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
//                 UInt_t Options                  -- frame options
// Results:        
// Exceptions:     
// Description:    Displays & executes root macros in a vertical frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros, Width - 20, Height - 20, frameGC, buttonGC, Options);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroBrowserGroup::TGMrbMacroBrowserGroup(const TGWindow * Parent, const Char_t * Title, TMrbLofMacros * LofMacros, TGMrbLayout * Layout)
							: TGGroupFrame(Parent, Title, kVerticalFrame, Layout->GC(), Layout->Font(), Layout->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Browse & execute root macros in a group frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Title                  -- group title
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 TGMrbLayout * Layout            -- layout (GC, font, background)
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a group frame
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros,	this->GetDefaultWidth() - 20,
														this->GetDefaultHeight() - 20,
														frameGC, buttonGC);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroBrowserPopup::TGMrbMacroBrowserPopup(const TGWindow * Parent, TMrbLofMacros * LofMacros,
										UInt_t Width, UInt_t Height) : TGPopupMenu(Parent, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup
// Purpose:        Browse & execute root macros in a popup menu
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a popup menu
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofMacros = LofMacros;

	TMrbNamedX * macro = fLofMacros->FirstMacro();
	this->AddEntry("&New", TGMrbMacroBrowserPopup::kGMrbPopupNew);
	this->AddEntry("&Open", TGMrbMacroBrowserPopup::kGMrbPopupOpen);
	this->AddSeparator();
	this->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

	while (macro) {
		TEnv * macroEnv = (TEnv *) macro->GetAssignedObject();
		TString macroName = macro->GetName();
		macroName += ": ";
		TString macroTitle = macroEnv->GetValue("Title", "");
		macroName += macroTitle;
		this->AddEntry(macroName.Data(), macro->GetIndex());
		macro = fLofMacros->NextMacro(macro);
	}
}

void TGMrbMacroBrowserPopup::MenuSelect(Int_t Selected) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserPopup::ProcessMessage
// Purpose:        Message handler
// Arguments:      Int_t Selected    -- entry selection
// Results:        --
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroBrowserPopup.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;
	TGFileInfo fi;

	TMrbLofMacros lofMacros;

	switch (Selected) {
		case TGMrbMacroBrowserPopup::kGMrbPopupNew:
			macro = new TMrbNamedX(0, "macro.C");
			macroEnv = new TEnv(".new");
			macroEnv->SetValue("NofArgs=1");
			macro->AssignObject(macroEnv);
			new TGMrbMacroEdit(fClient->GetRoot(), this, macro, 100, 100);
			break;
		case TGMrbMacroBrowserPopup::kGMrbPopupOpen:
			fi.fFileTypes = (const char **) macroFileTypes;
			new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
			if (fi.fFilename != NULL && *fi.fFilename != '\0') {
				if (lofMacros.AddMacro(fi.fFilename)) {
					macro = lofMacros.FirstMacro();
					if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
				}
			}
			break;
	}
}

TGMrbMacroBrowserTransient::TGMrbMacroBrowserTransient(const TGWindow * Parent, const TGWindow * Main,
									const Char_t * /*Title*/, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height, UInt_t Options)
														: TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroBrowserGroup
// Purpose:        Browse & execute root macros in a group frame
// Arguments:      TGWindow * Parent               -- parent window
//                 TGWindow * Main                 -- main window
//                 Char_t * Title                  -- title bar
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
// Results:        
// Exceptions:     
// Description:    Manipulates root macros in a group frame
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);

	fLofMacros = LofMacros;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 1, 1);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroList = new TGMrbMacroList(this, fLofMacros,	this->GetDefaultWidth() - 20,
														this->GetDefaultHeight() - 20,
														frameGC, buttonGC, Options);
	HEAP(fMacroList);
	this->AddFrame(fMacroList, frameGC->LH());
}

TGMrbMacroList::TGMrbMacroList(const TGWindow * Parent, TMrbLofMacros * LofMacros, UInt_t Width, UInt_t Height,
														TGMrbLayout * FrameGC, TGMrbLayout * ButtonGC,
														UInt_t FrameOptions, UInt_t ButtonOptions) :
											TGVerticalFrame(Parent, Width, Height, FrameOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList
// Purpose:        A text button list containing one button per macro
// Arguments:      TGWindow * Parent               -- parent window
//                 TMrbLofMacros * LofMacros       -- list of macros to be managed
//                 UInt_t Width                    -- window width in pixels
//                 UInt_t Height                   -- window height in pixels
//                 TGMrbLayout * FrameGC           -- frame layout
//                 TGMrbLayout * ButtonGC          -- button layout
//                 UInt_t FrameOptions             -- frame options
//                 UInt_t ButtonOptions            -- button options
// Results:        
// Exceptions:     
// Description:    Displays a list of buttons, one per macro.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro;
	TEnv * macroEnv;
	TString macroTitle;

	fLofMacros = LofMacros;

	fLofButtons.Delete();
	macro = fLofMacros->FirstMacro();
	while (macro) {
		macroEnv = (TEnv *) macro->GetAssignedObject();
		macroTitle = macroEnv->GetValue("Title", "");
		fLofButtons.AddNamedX(new TMrbNamedX(macro->GetIndex(), macro->GetName(), macroTitle));
		macro = fLofMacros->NextMacro(macro);
	}
	fButtonList = new TGMrbTextButtonList(this, NULL, &fLofButtons, -1, 1, Width, Height,
														FrameGC, NULL, ButtonGC, FrameOptions, ButtonOptions);
	HEAP(fButtonList);
	this->AddFrame(fButtonList, FrameGC->LH());
	((TGMrbButtonFrame *) fButtonList)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectMacro(Int_t, Int_t)");

	this->ChangeBackground(FrameGC->BG());
}

void TGMrbMacroList::SelectMacro(Int_t /*FrameId*/, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroList::SelectMacro
// Purpose:        Slot method
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Selection   -- macro selection
// Results:        
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroList.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * macro = fLofMacros->FindByIndex(Selection);
	if (macro) new TGMrbMacroFrame(fClient->GetRoot(), this, macro, 100, 100);
}

TGMrbMacroFrame::TGMrbMacroFrame(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro,
										UInt_t Width, UInt_t Height, UInt_t Options) :
													TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame
// Purpose:        A fransient frame to display macro args 
// Arguments:      TGWindow * Parent     -- parent window
//                 TGWindow * Main       -- main window
//                 TMrbNamedX * Macro    -- macro specs
//                 UInt_t Width          -- width
//                 UInt_t Height         -- height
//                 UInt_t Options        -- options
// Results:        
// Exceptions:     
// Description:    Displays macro arguments and execs macro.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg * macroArg;

	Int_t frameWidth;

	Int_t nofArgs;

	TString macroName, macroPath, titleBar;
	TString argResource, argName, argTitle, argType, argEntryType, addLofValues;
	TString argString;
	TString argValue;
	TString prefix;
	TMrbNamedX * button;

	Int_t defaultEntryWidth;
	Int_t intBase = 10;
	TString currentValue;
	TString defaultValue;
	TString value;
	TString cintString;

	TGMrbLayout * updownGC;
	Bool_t xUpDown;

	TObjArray lofSubstrings;

	TObjArray * lofSpecialButtons;
	
	TObjArray lofValues;

	TString inputFile;
	Int_t nft;

	const Char_t * normalFont;
	const Char_t * boldFont;
	const Char_t * italicFont;
	ULong_t black, white, gray, blue, yellow, dgray;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("gray65", dgray);
	gClient->GetColorByName("white", white);
	gClient->GetColorByName("yellow", yellow);

//	Initialize graphic contexts
	normalFont = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	boldFont = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	italicFont = gEnv->GetValue("Gui.SlantedFont", "-adobe-helvetica-bold-o-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(normalFont, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * bbuttonGC = new TGMrbLayout(boldFont, black, gray);
	HEAP(bbuttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(boldFont, black, gray);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(normalFont, black, gray);
	HEAP(frameGC);
	TGMrbLayout * entryGC = new TGMrbLayout(normalFont, black, white);
	HEAP(entryGC);
	TGMrbLayout * comboGC = new TGMrbLayout(normalFont, black, white);
	HEAP(comboGC);
	TGMrbLayout * cmtGC = new TGMrbLayout(italicFont, black, gray);
	HEAP(cmtGC);
	TGLayoutHints * cmtLayout = new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 50, 1, 3, 3);
	HEAP(cmtLayout);
	cmtGC->SetLH(cmtLayout);
	TGMrbLayout * sectGC = new TGMrbLayout(boldFont, black, dgray);
	HEAP(sectGC);
	TGLayoutHints * sectLayout = new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 30, 1, 3, 3);
	HEAP(sectLayout);
	sectGC->SetLH(sectLayout);

//	Initialize several lists
	fLofArgTypes.SetName("Argument types");
	fLofArgTypes.AddNamedX(kGMrbMacroLofArgTypes);
	fLofEntryTypes.SetName("Entry types");
	fLofEntryTypes.AddNamedX(kGMrbMacroLofEntryTypes);
	fLofActions.SetName("Action buttons");
	fLofActions.AddNamedX(kGMrbMacroLofActions);
	fLofActionsM.SetName("Action buttons (+modify)");
	fLofActionsM.AddNamedX(kGMrbMacroLofActionsModify);
	fLofActionsX.SetName("Action buttons (auto exec)");
	fLofActionsX.AddNamedX(kGMrbMacroLofActionsUserStart);

	fMacro = Macro; 		// save pointer to macro

	fMacroLoaded = kFALSE;	// try to recompile

	macroName = Macro->GetName();
	TString curEnvName = macroName(0, macroName.Index(".C"));
	curEnvName = Form(".%s.env", curEnvName.Data());
	TEnv * macroEnv;
	if (!gSystem->AccessPathName(curEnvName.Data())) {
		macroEnv = new TEnv(curEnvName.Data());
	} else {
		macroEnv = (TEnv *) Macro->GetAssignedObject();
	}
	nofArgs = macroEnv->GetValue("NofArgs", 1);

	frameWidth = macroEnv->GetValue("Width", -1);
	if (frameWidth == -1) frameWidth = TGMrbMacroFrame::kFrameWidth;

	titleBar = "ROOT Macro Browser: ";
	titleBar += macroName;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 1, 1);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);
	fMacroInfo = new TGGroupFrame(this, "Info", kVerticalFrame, labelGC->GC(), labelGC->Font(), labelGC->BG());
	HEAP(fMacroInfo);
	this->AddFrame(fMacroInfo, frameGC->LH());

	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 50, 1, 3, 3);
	HEAP(labelLayout);
	labelGC->SetLH(labelLayout);
	fMacroName = new TGLabel(fMacroInfo, Macro->GetName(), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroName);
	fMacroName->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroName, labelGC->LH());

	fMacroTitle = new TGLabel(fMacroInfo, macroEnv->GetValue("Title", ""), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroTitle);
	fMacroTitle->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroTitle, labelGC->LH());

	macroPath = "loaded from ";
	macroPath += macroEnv->GetValue("Path", "");
	fMacroPath = new TGLabel(fMacroInfo, macroPath.Data(), labelGC->GC(), labelGC->Font(), kChildFrame, labelGC->BG());
	HEAP(fMacroPath);
	fMacroPath->SetTextJustify(kTextLeft);
	fMacroInfo->AddFrame(fMacroPath, labelGC->LH());

	fMacroArgs = new TGGroupFrame(this, "Arguments", kVerticalFrame, labelGC->GC(), labelGC->Font(), labelGC->BG());
	HEAP(fMacroArgs);
	this->AddFrame(fMacroArgs, frameGC->LH());

	TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 0, 1, 3, 3);
	HEAP(entryLayout);
	entryGC->SetLH(entryLayout);

	TGLayoutHints * comboLayout = new TGLayoutHints(kLHintsExpandY, 5, 1, 3, 3);
	HEAP(comboLayout);
	comboGC->SetLH(comboLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);
	bbuttonGC->SetLH(buttonLayout);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
	Int_t tabNo = -1;
	Int_t subframeNo = -1;
	Int_t frameNo = -1;
	TGCompositeFrame * parentFrame = NULL;
	TGCompositeFrame * curFrame = fMacroArgs;
	fMacroTab = NULL;
	Bool_t newPad = kTRUE;
	Bool_t newFrame = kTRUE;
	for (Int_t i = 0; i < nofArgs; i++) {
		Int_t argNo = i + 1;
		macroArg = new TGMrbMacroArg(argNo);
		fLofMacroArgs.Add(macroArg);

		argName = macroEnv->GetValue(macroArg->GetResource(argResource, "Name"), "");
		macroArg->SetName(argName.Data());

		argTitle = macroEnv->GetValue(macroArg->GetResource(argResource, "Title"), "");
		if (argTitle.BeginsWith("@")) argTitle = "";

		argEntryType = macroEnv->GetValue(macroArg->GetResource(argResource, "EntryType"), "Entry");
		if ((macroArg->fEntryType = fLofEntryTypes.FindByName(argEntryType.Data(), TMrbLofNamedX::kFindExact)) == NULL) {
			gMrbLog->Err() << Macro->GetName() << ": Illegal entry type - " << argEntryType	<< endl;
			gMrbLog->Flush(this->ClassName());
			macroArg->fEntryType = fLofEntryTypes.FindByIndex(TGMrbMacroArg::kGMrbMacroEntryPlain);
		}
		Int_t entryType = macroArg->fEntryType->GetIndex();

		if (newFrame || (entryType == TGMrbMacroArg::kGMrbMacroEntryFrame)) {
			frameNo++;
			newFrame = kFALSE;
			if (frameNo < TGMrbMacroFrame::kGMrbMacroNofFrames) {
				Int_t opt = (tabNo >= 0) ? kChildFrame | kRaisedFrame : kChildFrame;
				fMacroLofFrames[frameNo] = new TGHorizontalFrame(fMacroArgs, 1, 1, opt);
				HEAP(fMacroLofFrames[frameNo]);
				fMacroArgs->AddFrame(fMacroLofFrames[frameNo], frameGC->LH());
				curFrame = fMacroLofFrames[frameNo];
				parentFrame = fMacroLofFrames[frameNo];
				newPad = kTRUE;
			} else {
				gMrbLog->Wrn()	<< "[Arg " << argName << "(#" << macroArg->GetIndex() << ")]: Can't open new frame - max "
								<< TGMrbMacroFrame::kGMrbMacroNofFrames << " allowed" << endl;
				gMrbLog->Flush(this->ClassName());
				continue;
			}
		}

		if (entryType == TGMrbMacroArg::kGMrbMacroEntryTab) {
			if (fMacroTab == NULL) {
				if (i == 0) {
					fMacroTab = new TGTab(fMacroArgs, Width, Height);
					fMacroArgs->AddFrame(fMacroTab, frameGC->LH());
				} else {
					gMrbLog->Wrn()	<< "[Arg " << argName << "(#" << macroArg->GetIndex() << ")]: Can't open tab - tabs have to be initialized with arg#0" << endl;
					gMrbLog->Flush(this->ClassName());
					continue;
				}
			}
			tabNo++;
			if (tabNo < TGMrbMacroFrame::kGMrbMacroNofTabs) {
				fMacroLofTabs[tabNo] = fMacroTab->AddTab(argTitle.Data());
				fMacroLofTabFrames[tabNo] = new TGVerticalFrame(fMacroLofTabs[tabNo]);
				HEAP(fMacroLofTabs[tabNo]);
				fMacroLofTabs[tabNo]->AddFrame(fMacroLofTabFrames[tabNo], frameGC->LH());
				curFrame = fMacroLofTabFrames[tabNo];
				parentFrame = fMacroLofTabFrames[tabNo];
				newPad = kTRUE;
			} else {
				gMrbLog->Wrn()	<< "[Arg " << argName << "(#" << macroArg->GetIndex() << ")]: Can't open new tab - max "
								<< TGMrbMacroFrame::kGMrbMacroNofTabs << " allowed" << endl;
				gMrbLog->Flush(this->ClassName());
				continue;
			}
		}

		if (newPad || (entryType == TGMrbMacroArg::kGMrbMacroEntryPad)) {
			subframeNo++;
			if (subframeNo < TGMrbMacroFrame::kGMrbMacroNofSubframes) {
				fMacroLofSubframes[subframeNo] = new TGVerticalFrame(curFrame);
				HEAP(fMacroLofSubframes[subframeNo]);
				curFrame->AddFrame(fMacroLofSubframes[subframeNo], frameGC->LH());
				parentFrame = fMacroLofSubframes[subframeNo];
				newPad = kFALSE;
				if (i > 0) continue;
			} else {
				gMrbLog->Wrn()	<< "[Arg " << argName << "(#" << macroArg->GetIndex() << ")]: Can't open new vertical pad - max "
									<< TGMrbMacroFrame::kGMrbMacroNofSubframes << " allowed" << endl;
				gMrbLog->Flush(this->ClassName());
				continue;
			}
		}

		argType = macroEnv->GetValue(macroArg->GetResource(argResource, "Type"), "Char_t *");
		if ((macroArg->fType = fLofArgTypes.FindByName(argType.Data(), TMrbLofNamedX::kFindExact)) == NULL) {
			gMrbLog->Err() << "[Arg " << argName << "(#" << macroArg->GetIndex() << ")]: Illegal argument type - " << argType << endl;
			gMrbLog->Flush(this->ClassName());
			macroArg->fType = fLofArgTypes.FindByIndex(TGMrbMacroArg::kGMrbMacroArgChar);
		}

		addLofValues = macroEnv->GetValue(macroArg->GetResource(argResource, "AddLofValues"), "No");
		macroArg->fAddLofValues = (addLofValues.CompareTo("Yes") == 0);

		macroArg->fNofEntryFields = macroEnv->GetValue(macroArg->GetResource(argResource, "NofEntryFields"), 1);

		defaultEntryWidth = (macroArg->fType->GetIndex() == TGMrbMacroArg::kGMrbMacroArgChar) ? 250 : 150;
		if (macroArg->fEntryType->GetIndex() == TGMrbMacroArg::kGMrbMacroEntryFile) defaultEntryWidth = 300;
		macroArg->fEntryWidth = macroEnv->GetValue(macroArg->GetResource(argResource, "Width"), defaultEntryWidth);

		Int_t nofVals = macroEnv->GetValue(macroArg->GetResource(argResource, "Current.NofValues"), 0);
		lofValues.Delete();
		if (nofVals == 0) {
			currentValue = macroEnv->GetValue(macroArg->GetResource(argResource, "Current"), "");
			currentValue = currentValue.Strip(TString::kBoth);
			macroArg->fCurrentValue = currentValue;
		} else {
			currentValue = "";
			for (Int_t k = 0; k < nofVals; k++) {
				TString vStr = macroEnv->GetValue(macroArg->GetResource(argResource, Form("Current.%d", k)), "");
				if (k == 0) {
					currentValue = vStr;
				} else {
					lofValues.Add(new TObjString(vStr.Data()));
				}
			}
		}

		defaultValue = macroEnv->GetValue(macroArg->GetResource(argResource, "Default"), "");
		defaultValue = defaultValue.Strip(TString::kBoth);
		if (defaultValue(0) == '<') {
			defaultValue = defaultValue(1, defaultValue.Length() - 1);
			defaultValue = gEnv->GetValue(defaultValue.Data(), "");
		} else if (defaultValue(0) == '@') {
			defaultValue = defaultValue(1, defaultValue.Length() - 1);
			switch (macroArg->fType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroArgInt:		cintString = "macroArgInt = "; break;
				case TGMrbMacroArg::kGMrbMacroArgUInt:		cintString = "macroArgUInt = "; break;
				case TGMrbMacroArg::kGMrbMacroArgFloat:		cintString = "macroArgFloat = "; break;
				case TGMrbMacroArg::kGMrbMacroArgDouble:	cintString = "macroArgDouble = "; break;
				case TGMrbMacroArg::kGMrbMacroArgBool:		cintString = "macroArgBool = "; break;
				default:									cintString = "macroArgString = "; break;
			}
			cintString += defaultValue.Data();
			gROOT->ProcessLine(cintString.Data());
		}

		macroArg->fDefaultValue = defaultValue;
		gSystem->ExpandPathName(macroArg->fDefaultValue);

		if (entryType & TGMrbMacroArg::kGMrbMacroEntryComment) {
			macroArg->fComment = new TGLabel(parentFrame, argTitle.Data(), cmtGC->GC(), cmtGC->Font(), kChildFrame, cmtGC->BG());
			HEAP(macroArg->fComment);
			macroArg->fComment->SetTextJustify(kTextLeft);
			parentFrame->AddFrame(macroArg->fComment, cmtGC->LH());
		} else if (entryType & TGMrbMacroArg::kGMrbMacroEntrySection) {
			macroArg->fSection = new TGLabel(parentFrame, argTitle.Data(), sectGC->GC(), sectGC->Font(), kChildFrame, sectGC->BG());
			HEAP(macroArg->fSection);
			macroArg->fSection->SetTextJustify(kTextLeft);
			parentFrame->AddFrame(macroArg->fSection, sectGC->LH());
		} else if (entryType & TGMrbMacroArg::kGMrbMacroEntryGroup) {
			macroArg->fGroup = new TGGroupFrame(fMacroLofSubframes[subframeNo],  argTitle.Data(), kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
			HEAP(macroArg->fGroup);
			fMacroLofSubframes[subframeNo]->AddFrame(macroArg->fGroup, frameGC->LH());
			parentFrame = macroArg->fGroup;
		} else if (entryType & TGMrbMacroArg::kGMrbMacroEntryFrame) {
			macroArg->fFrame = new TGVerticalFrame(fMacroArgs);
			HEAP(macroArg->fFrame);
			fMacroArgs->AddFrame(macroArg->fFrame, frameGC->LH());
			parentFrame = macroArg->fFrame;
		} else if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitEntry) {

			TMrbLofNamedX * checkBtn;
			if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
				checkBtn = new TMrbLofNamedX();
				checkBtn->AddNamedX(1, "");
			} else {
				checkBtn = NULL;
			}

			xUpDown = (entryType & TGMrbMacroArg::kGMrbMacroEntryBitBeginEnd);
			updownGC = ((entryType & TGMrbMacroArg::kGMrbMacroEntryBitUpDown) || xUpDown) ? buttonGC : NULL;

			if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {

				macroArg->fEntry = new TGMrbLabelEntry(parentFrame, argTitle.Data(),
														40, argNo,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														macroArg->fEntryWidth,
														frameGC, labelGC, entryGC, updownGC, xUpDown,
														NULL, NULL, checkBtn, NULL, kHorizontalFrame, kSunkenFrame, macroArg->fNofEntryFields);
			} else {
				macroArg->fEntry = new TGMrbLabelEntry(parentFrame, argTitle.Data(),
														40, argNo,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														macroArg->fEntryWidth,
														frameGC, labelGC, entryGC, updownGC, xUpDown,
														NULL, NULL, checkBtn);
			}
			HEAP(macroArg->fEntry);
			macroArg->fEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
			parentFrame->AddFrame(macroArg->fEntry, frameGC->LH());

			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			Int_t nofEntryFields = macroArg->fNofEntryFields;

			TObjArray * lofTokens = value.Tokenize(":");
			Int_t nofTokens = lofTokens->GetEntries();
			Int_t nt = (entryType & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) ? (nofEntryFields + 1) : nofEntryFields;
			if (nt != nofTokens) {
				TString an = macroEnv->GetValue(macroArg->GetResource(argResource, "Name"), "??");
				gMrbLog->Wrn()	<< "[Arg " << an << " (#" << macroArg->GetIndex() << ")]: Number of (default) values not matching number of entries - "
								<< nofTokens << " (should be " << nt << ")" << endl;
				gMrbLog->Flush(this->ClassName());
			}
			TString v;
			TString aet = macroArg->fType->GetName();
			TString dv = aet.BeginsWith("Char_t") ? "" : "0";
			if (nofTokens >= 1) v = ((TObjString *) lofTokens->At(0))->GetString(); else v = dv;
			macroArg->fEntry->SetText(v, 0);
			if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
				for (Int_t ef = 1; ef < nofEntryFields; ef++) {
					if (nofTokens >= (ef + 1)) v = ((TObjString *) lofTokens->At(ef))->GetString(); else v = dv;
					macroArg->fEntry->SetText(v, ef);
				}
			}
			if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
				if (nofTokens == nt) v = ((TObjString *) lofTokens->At(nofTokens - 1))->GetString(); else v = "F";
				TGMrbCheckButtonList * lcb = macroArg->fEntry->GetLofCheckButtons();
				TGButton * btn = lcb->GetButton(1);
				btn->SetState(v.BeginsWith("T") ? kButtonDown : kButtonUp);
			}
			delete lofTokens;

			TGMrbLabelEntry::EGMrbEntryType at = TGMrbLabelEntry::kGMrbEntryTypeCharInt;
			ETextJustification jm = kTextLeft;
			switch (macroArg->fType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroArgInt:
				case TGMrbMacroArg::kGMrbMacroArgUInt:
				case TGMrbMacroArg::kGMrbMacroArgBool:
					{
						at = TGMrbLabelEntry::kGMrbEntryTypeInt;
						jm = kTextRight;
						intBase = macroEnv->GetValue(macroArg->GetResource(argResource, "Base"), 10);
					}
					break;
				case TGMrbMacroArg::kGMrbMacroArgFloat:
				case TGMrbMacroArg::kGMrbMacroArgDouble:
					{
						at = TGMrbLabelEntry::kGMrbEntryTypeDouble;
						jm = kTextRight;
						intBase = macroEnv->GetValue(macroArg->GetResource(argResource, "Precision"), 2);
					}
					break;
			}
			macroArg->fEntry->SetType(at, 0, intBase);
			macroArg->fEntry->SetTextAlignment(jm);

			if (entryType & TGMrbMacroArg::kGMrbMacroEntryUpDown) {
				if (entryType & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
					Int_t nofEntryFields1 = macroArg->fNofEntryFields;
					TString llvalue = macroEnv->GetValue(macroArg->GetResource(argResource, "LowerLimit"), "0.0");
					TString ulvalue = macroEnv->GetValue(macroArg->GetResource(argResource, "UpperLimit"), "1000000.0");
					TString ivalue = macroEnv->GetValue(macroArg->GetResource(argResource, "Increment"), "1.0");
					TObjArray * lltok = llvalue.Tokenize(":");
					Int_t nofTokens1 = lltok->GetEntries();
					if (nofTokens1 != nofEntryFields1) {
						gMrbLog->Wrn()	<< "[Arg #" << macroArg->GetIndex() << "]: Number of lower limits not matching number of entries - "
										<< nofTokens1 << " (should be " << nofEntryFields1 << ")" << endl;
						gMrbLog->Flush(this->ClassName());
					}
					TObjArray * ultok = ulvalue.Tokenize(":");
					nofTokens1 = ultok->GetEntries();
					if (nofTokens1 != nofEntryFields1) {
						gMrbLog->Wrn()	<< "[Arg #" << macroArg->GetIndex() << "]: Number of upper limits not matching number of entries - "
										<< nofTokens1 << " (should be " << nofEntryFields1 << ")" << endl;
						gMrbLog->Flush(this->ClassName());
					}
					TObjArray * itok = ivalue.Tokenize(":");
					nofTokens1 = itok->GetEntries();
					if (nofTokens1 != nofEntryFields1) {
						gMrbLog->Wrn()	<< "[Arg #" << macroArg->GetIndex() << "]: Number of increments not matching number of entries - "
										<< nofTokens1 << " (should be " << nofEntryFields1 << ")" << endl;
						gMrbLog->Flush(this->ClassName());
					}
					TString llv, ulv, iv;
					for (Int_t ef = 0; ef < nofEntryFields1; ef++) {
						if (nofTokens >= (ef + 1)) {
							llv = ((TObjString *) lltok->At(ef))->GetString();
							ulv = ((TObjString *) ultok->At(ef))->GetString();
							iv = ((TObjString *) itok->At(ef))->GetString();
							macroArg->fEntry->SetRange(llv.Atof(), ulv.Atof(), ef);
							macroArg->fEntry->SetIncrement(iv.Atof(), ef);
						} else {
							macroArg->fEntry->SetRange(0.0, 1000000.0, ef);
							macroArg->fEntry->SetIncrement(1.0, ef);
						}
					}
					delete lltok;
					delete ultok;
					delete itok;
				} else {
					macroArg->fLowerLimit = macroEnv->GetValue(macroArg->GetResource(argResource, "LowerLimit"), 0.0);
					macroArg->fUpperLimit = macroEnv->GetValue(macroArg->GetResource(argResource, "UpperLimit"), 1000000.0);
					macroArg->fIncrement = macroEnv->GetValue(macroArg->GetResource(argResource, "Increment"), 1.0);
					macroArg->fEntry->SetRange(macroArg->fLowerLimit, macroArg->fUpperLimit);
					macroArg->fEntry->SetIncrement(macroArg->fIncrement);
				}
			}
		} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			macroArg->fButtons.Delete();
			macroArg->fButtons.SetPatternMode();
			macroArg->fButtons.AddNamedX(kGMrbMacroYesNoButtons);
			macroArg->fRadio = new TGMrbRadioButtonList(parentFrame, argTitle.Data(),
														&macroArg->fButtons, argNo, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
			HEAP(macroArg->fRadio);
			parentFrame->AddFrame(macroArg->fRadio, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			if (value.CompareTo("Yes") == 0) {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryYes, kButtonDown);
			} else {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
			}
			((TGMrbButtonFrame *) macroArg->fRadio)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
		} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryRadio
						|| entryType == TGMrbMacroArg::kGMrbMacroEntryCheck
						|| entryType == TGMrbMacroArg::kGMrbMacroEntryText
						|| entryType == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			argString = macroEnv->GetValue(macroArg->GetResource(argResource, "Orientation"), "");
			if (argString.CompareTo("vertical") == 0)	macroArg->fOrientation = kVerticalFrame;
			else								macroArg->fOrientation = kHorizontalFrame;
			macroArg->fNofCL = macroEnv->GetValue(macroArg->GetResource(argResource, "NofCL"), 1);
			argString = macroEnv->GetValue(macroArg->GetResource(argResource, "Values"), "");
			argString = argString.Strip(TString::kBoth);
			if (argString.Length() == 0) {
				gMrbLog->Err()	<< Macro->GetName() << ": " << macroArg->fEntryType->GetName()
								<< ": Values missing" << endl;
				gMrbLog->Flush(this->ClassName());
			} else {
				Int_t m = 1;
				macroArg->fButtons.Delete();
				macroArg->fButtons.SetPatternMode();
				lofSubstrings.Delete();
				Int_t from = 0;
				TString str;
				while (argString.Tokenize(str, from, ":")) {
					intBase = macroEnv->GetValue(macroArg->GetResource(argResource, "Base"), 10);
					Int_t nsep = str.Index("=", 0);
					TString tip = "";
					if (nsep > 0) {
						UInt_t at = macroArg->fType->GetIndex();
						if (at == TGMrbMacroArg::kGMrbMacroArgInt || at == TGMrbMacroArg::kGMrbMacroArgUInt) {
							TString s = str(nsep + 1, 1000);
							m = strtol(s.Data(), NULL, intBase);
							str.Resize(nsep);
						}
					}
					Int_t ntip = str.Index("|", 0);
					if (ntip > 0) {
						tip = str(ntip + 1, 1000);
						str.Resize(ntip);
					}
					macroArg->fButtons.AddNamedX(new TMrbNamedX(m, str.Data(), (tip.Length() > 0) ? tip.Data() : NULL));
					m <<= 1;
				}
				if (entryType == TGMrbMacroArg::kGMrbMacroEntryRadio) {
					macroArg->fRadio = new TGMrbRadioButtonList(parentFrame, argTitle.Data(),
														&macroArg->fButtons, argNo, macroArg->fNofCL,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC, macroArg->fOrientation);
					HEAP(macroArg->fRadio);
					parentFrame->AddFrame(macroArg->fRadio, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					Int_t val = value.Atoi();
					if ((button = macroArg->fButtons.FindByIndex(val)) != NULL) {
						macroArg->fRadio->SetState(val, kButtonDown);
					}
					((TGMrbButtonFrame *) macroArg->fRadio)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
				} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryCheck) {
					macroArg->fCheck = new TGMrbCheckButtonList(parentFrame, argTitle.Data(),
														&macroArg->fButtons, argNo, macroArg->fNofCL,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC,
														lofSpecialButtons,
														macroArg->fOrientation);
					HEAP(macroArg->fCheck);
					parentFrame->AddFrame(macroArg->fCheck, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					Int_t pattern = value.Atoi();
					macroArg->fCheck->SetState(pattern, kButtonDown);
					((TGMrbButtonFrame *) macroArg->fCheck)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
				} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryText) {
					macroArg->fText = new TGMrbTextButtonList(parentFrame, argTitle.Data(),
														&macroArg->fButtons, argNo, macroArg->fNofCL,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC,
														macroArg->fOrientation);
					HEAP(macroArg->fText);
					parentFrame->AddFrame(macroArg->fText, frameGC->LH());
					((TGMrbButtonFrame *) macroArg->fText)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
				} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryCombo) {
					macroArg->fCombo = new TGMrbLabelCombo(parentFrame, argTitle.Data(), &macroArg->fButtons,
														argNo, -1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														macroArg->fEntryWidth,
														frameGC, labelGC, comboGC);
					HEAP(macroArg->fCombo);
					parentFrame->AddFrame(macroArg->fCombo, frameGC->LH());
					value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
					if ((button = macroArg->fButtons.FindByName(value.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
						macroArg->fCombo->GetComboBox()->Select(button->GetIndex());
						macroArg->fCombo->SetText(button->GetName());
					}
				}
			}
		} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryFile) {
			memset(macroArg->fPtrFileTypes, 0, 2 * TGMrbMacroArg::kGMrbMacroNofFileTypes * sizeof(Char_t *));
			argString = macroEnv->GetValue(macroArg->GetResource(argResource, "Values"), "");
			argString = argString.Strip(TString::kBoth);
			nft = 0;
			if (argString.Length() != 0) {
				macroArg->fFileTypes.Delete();
				TString ft;
				Int_t from = 0;
				while(argString.Tokenize(ft, from, ":")) {
					TObjString * oft = new TObjString(ft);
					macroArg->fPtrFileTypes[nft++] = oft->GetString();
					macroArg->fFileTypes.Add(oft);
				}
			}
			if (nft % 2) {
				gMrbLog->Err()	<< Macro->GetName() << ": " << macroArg->fEntryType->GetName()
								<< ": File types have to be pairs of strings" << endl;
				gMrbLog->Flush(this->ClassName());
				nft = 0;
			}
			macroArg->fPtrFileTypes[nft++] = "All files blabla";
			macroArg->fPtrFileTypes[nft] = "*";
			macroArg->fFileInfo->fIniDir = (Char_t *) gSystem->WorkingDirectory();
			macroArg->fFileInfo->fFileTypes = (const Char_t **) macroArg->fPtrFileTypes;
			macroArg->fFile = new TGMrbFileEntry(parentFrame, argTitle.Data(), 100,
												argNo,
												frameWidth - 20,
												TGMrbMacroFrame::kLineHeight,
												macroArg->fEntryWidth,
												macroArg->fFileInfo, kFDOpen,
												frameGC, labelGC, entryGC, labelGC);
			HEAP(macroArg->fFile);
			parentFrame->AddFrame(macroArg->fFile, frameGC->LH());
			value = (currentValue.Length() == 0) ? defaultValue.Data() : currentValue.Data();
			macroArg->fFile->GetEntry()->SetText(value.Data());
			macroArg->fFile->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "ProcessSignal(Int_t, Int_t)");
		} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			macroArg->fFObjCombo = new TGMrbFileObjectCombo(parentFrame, argTitle.Data(), 100,
												argNo,
												frameWidth - 20,
												TGMrbMacroFrame::kLineHeight,
												macroArg->fEntryWidth,
												macroArg->fEntryWidth,
												frameGC, labelGC, entryGC, labelGC, entryGC);
			HEAP(macroArg->fFObjCombo);
			parentFrame->AddFrame(macroArg->fFObjCombo, frameGC->LH());
			value = (currentValue.IsNull()) ? defaultValue.Data() : currentValue.Data();
			if (!value.IsNull()) {
				if (nofVals > 0) {
					macroArg->fFObjCombo->SetFileEntry(value.Data());
					if (!gSystem->AccessPathName(value.Data())) macroArg->fFObjCombo->OpenFile(value.Data());
				} else {
					macroArg->fFObjCombo->SetSelectionFromString(value, kTRUE);
				}
			}
		} else if (entryType == TGMrbMacroArg::kGMrbMacroEntryFObjListBox) {
			macroArg->fFObjListBox = new TGMrbFileObjectListBox(parentFrame, argTitle.Data(), 100,
												argNo,
												frameWidth - 20,
												TGMrbMacroFrame::kLineHeight,
												macroArg->fEntryWidth,
												macroArg->fEntryWidth,
												frameGC, labelGC, entryGC, labelGC, entryGC);
			HEAP(macroArg->fFObjListBox);
			parentFrame->AddFrame(macroArg->fFObjListBox, frameGC->LH());
			value = (currentValue.IsNull()) ? defaultValue.Data() : currentValue.Data();
			if (!value.IsNull()) {
				macroArg->fFObjListBox->SetFileEntry(value.Data());
				if (!gSystem->AccessPathName(value.Data())) macroArg->fFObjListBox->OpenFile(value.Data());
				if (nofVals > 0) {
					if (!gSystem->AccessPathName(value.Data())) macroArg->fFObjListBox->OpenFile(value.Data());
					macroArg->fFObjListBox->SetList(lofValues);
				} else {
					macroArg->fFObjListBox->SetSelectionFromString(value, kTRUE);
				}
			}
		}
	}

	TGLayoutHints * actionLayout = new TGLayoutHints(kLHintsExpandX, 5, 1, 3, 1);
	HEAP(actionLayout);
	bbuttonGC->SetLH(actionLayout);

	this->ChangeBackground(gray);
	this->SetWindowName(titleBar.Data());

	if (this->LoadMacro()) {

		Bool_t canModify = macroEnv->GetValue("Modify", kFALSE);
		Bool_t userStart = macroEnv->GetValue("UserStart", kFALSE);
		fAction = NULL;
		if (userStart) {
			TString gui = macroEnv->GetValue("GuiPtrMode", "GuiPtr");
			if (gui.CompareTo("GuiPtr") != 0) {
				gMrbLog->Err()	<< "User start not possible with gui ptr mode off" << endl;
				gMrbLog->Flush(this->ClassName());
				userStart = kFALSE;
			}
		}
		if (userStart) {
			if (canModify) fAction = new TGMrbTextButtonGroup(this, "Action", &fLofActionsX, -1, 2, labelGC, bbuttonGC);
			this->ExecMacro(kTRUE);
		} else {
			fAction = new TGMrbTextButtonGroup(this, "Action", canModify ? &fLofActionsM : &fLofActions, -1, 2, labelGC, bbuttonGC);
		}
		if (fAction) {
			HEAP(fAction);
			this->AddFrame(fAction, frameGC->LH());
			((TGMrbButtonFrame *) fAction)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButtonPressed(Int_t, Int_t)");
		}

		Window_t wdum;
		Int_t ax, ay;
		gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() + 10), 0, ax, ay, wdum);
		this->Move(ax, ay);

		this->MapSubwindows();
		this->Resize(this->GetDefaultSize());

		if (fAction) {
			this->Resize(frameWidth, fMacroInfo->GetDefaultHeight()
    							   + fMacroArgs->GetDefaultHeight()
    							   + fAction->GetDefaultHeight() + 20);
		} else {
			this->Resize(frameWidth, fMacroInfo->GetDefaultHeight()
    							   + fMacroArgs->GetDefaultHeight() + 20);
		}
		this->MapWindow();
	} else {
		this->MakeZombie();
	}
}

void TGMrbMacroFrame::ProcessSignal(Int_t FrameId, Int_t Signal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ProcessSignal
// Purpose:        Signal handler
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Signal      -- signal
// Results:        --
// Exceptions:     
// Description:    Pass signals to consumer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t id = Signal;
	Int_t argNo = FrameId;
	if (argNo > 0) {
		TEnv * macroEnv = (TEnv *) fMacro->GetAssignedObject();
		TString argName = macroEnv->GetValue(Form("Arg%d.Name", argNo), "???");
		TString temp = Form("ProcessSignal((TGMrbMacroFrame *) %lld, %d, \"%s\", %d);", this, argNo, argName.Data(), id);
		gROOT->ProcessLine(temp);
	}
}

void TGMrbMacroFrame::ActionButtonPressed(Int_t /*FrameId*/, Int_t Signal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ActionButtonPressed
// Purpose:        slot method
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Signal      -- signal
// Results:        --
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroFrame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Signal) {
		case TGMrbMacroFrame::kGMrbMacroIdModifyHeader:
			this->ModifyMacroHeader();
			break;
		case TGMrbMacroFrame::kGMrbMacroIdModifySource:
			this->ModifyMacroSource();
			break;
		case TGMrbMacroFrame::kGMrbMacroIdClose:
			this->CloseWindow();
			break;
		case TGMrbMacroFrame::kGMrbMacroIdReset:
			this->ResetMacroArgs();
			break;
		case TGMrbMacroFrame::kGMrbMacroIdExec:
			this->ExecMacro(kFALSE);
			break;
		case TGMrbMacroFrame::kGMrbMacroIdExecClose:
			this->ExecMacro(kFALSE);
			this->CloseWindow();
			break;
		case TGMrbMacroFrame::kGMrbMacroIdQuit:
			gSystem->Exit(0);
			break;
	}
}

Bool_t TGMrbMacroFrame::ResetMacroArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ResetMacroArgs
// Purpose:        Reset arguments
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Resets macro arguments
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX *button;

	TIterator * macroIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * macroArg;
	while (macroArg = (TGMrbMacroArg *) macroIter->Next()) {
		Int_t n = macroArg->fEntryType->GetIndex();
		if (n & (
					TGMrbMacroArg::kGMrbMacroEntryComment |
					TGMrbMacroArg::kGMrbMacroEntrySection |
					TGMrbMacroArg::kGMrbMacroEntryGroup |
					TGMrbMacroArg::kGMrbMacroEntryFrame |
					TGMrbMacroArg::kGMrbMacroEntryText |
					TGMrbMacroArg::kGMrbMacroEntryPad |
					TGMrbMacroArg::kGMrbMacroEntryTab)) {
			continue;
		} else if (n & TGMrbMacroArg::kGMrbMacroEntryBitMulti || n & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
			Int_t nofEntryFields = macroArg->fNofEntryFields;
			TString value = macroArg->fDefaultValue;
			TObjArray * lofTokens = value.Tokenize(":");
			Int_t nofTokens = lofTokens->GetEntries();
			Int_t nt = (n & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) ? (nofEntryFields + 1) : nofEntryFields;
			if (nt != nofTokens) {
				gMrbLog->Wrn()	<< "[Arg #" << macroArg->GetIndex() << "]: Number of (default) values not matching number of entries - "
								<< nofTokens << " (should be " << nt << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "ResetMacroArgs");
			}
			TString v;
			TString aet = macroArg->fType->GetName();
			TString dv = aet.BeginsWith("Char_t") ? "" : "0";
			if (n & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
				for (Int_t ef = 0; ef < nofEntryFields; ef++) {
					if (nofTokens >= (ef + 1)) v = ((TObjString *) lofTokens->At(ef))->GetString(); else v = dv;
					macroArg->fEntry->SetText(v, ef);
				}
			}
			if (n & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
				if (nofTokens == nt) v = ((TObjString *) lofTokens->At(nofTokens - 1))->GetString(); else v = "F";
				TGMrbCheckButtonList * lcb = macroArg->fEntry->GetLofCheckButtons();
				TGButton * btn = lcb->GetButton(1);
				btn->SetState(v.BeginsWith("T") ? kButtonDown : kButtonUp);
			}
			delete lofTokens;
		} else if (n & TGMrbMacroArg::kGMrbMacroEntryBitEntry) {
			macroArg->fEntry->SetText(macroArg->fDefaultValue.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			if (macroArg->fDefaultValue.CompareTo("Yes") == 0) {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryYes, kButtonDown);
			} else {
				macroArg->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryRadio) {
			if ((button = macroArg->fButtons.FindByName(macroArg->fDefaultValue.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
				macroArg->fRadio->SetState(button->GetIndex(), kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCheck) {
			UInt_t pattern = macroArg->fButtons.FindPattern(macroArg->fDefaultValue.Data());
			macroArg->fCheck->SetState(0xFFFFFFFF, kButtonUp);
			if (pattern != TMrbLofNamedX::kIllIndexBit) {
				macroArg->fCheck->SetState(pattern, kButtonDown);
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			if ((button = macroArg->fButtons.FindByName(macroArg->fDefaultValue.Data(), TMrbLofNamedX::kFindExact)) != NULL) {
				macroArg->fCombo->GetComboBox()->Select(button->GetIndex());
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFile) {
			macroArg->fFile->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			macroArg->fFObjCombo->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjListBox) {
			macroArg->fFObjListBox->GetEntry()->SetText(macroArg->fDefaultValue.Data());
		}
	}
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ExecMacro(Bool_t UserStart) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ExecMacro
// Purpose:        Execute macro
// Arguments:      Bool_t UserStart   -- kTRUE if to be started by user
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes macro with given arguments
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t buttonBits;
	TString argString[TGMrbLabelEntry::kGMrbEntryNofEntries];
	TString currentValue, cVal;
	UInt_t argType;

	if (!this->LoadMacro()) return(kFALSE);

	TEnv * macroEnv = (TEnv *) fMacro->GetAssignedObject();

	TObjArray * argArr = new TObjArray();

	TString cmd = fMacroName->GetText()->GetString();
	cmd = cmd(0, cmd.Index(".C"));
	TString delim = "(";

	TString checkBtn = "";

	Bool_t addGui = kFALSE;
	Bool_t guiOnly = kFALSE;
	TString gui = macroEnv->GetValue("GuiPtrMode", "ArgList");
	addGui = (gui.CompareTo("ArgList+GuiPtr") == 0);
	guiOnly = (gui.CompareTo("GuiPtr") == 0);

	TIterator * macroIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * macroArg;
	while (macroArg = (TGMrbMacroArg *) macroIter->Next()) {
		Int_t n = macroArg->fEntryType->GetIndex();
		if (n & (
					TGMrbMacroArg::kGMrbMacroEntryComment |
					TGMrbMacroArg::kGMrbMacroEntrySection |
					TGMrbMacroArg::kGMrbMacroEntryGroup |
					TGMrbMacroArg::kGMrbMacroEntryFrame |
					TGMrbMacroArg::kGMrbMacroEntryText |
					TGMrbMacroArg::kGMrbMacroEntryPad |
					TGMrbMacroArg::kGMrbMacroEntryTab)) {
			continue;
		} else if (n & TGMrbMacroArg::kGMrbMacroEntryBitEntry) {
			argString[0] = macroArg->fEntry->GetText();
			for (Int_t i = 1; i < TGMrbLabelEntry::kGMrbEntryNofEntries; i++) argString[i] = "";
			currentValue = argString[0];
			if (n & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
				Int_t nofEntryFields = macroArg->fNofEntryFields;
				for (Int_t ef = 1; ef < nofEntryFields; ef++) {
					currentValue += ":";
					argString[ef] = macroArg->fEntry->GetText(ef);
					currentValue += argString[ef];
				}
			}
			if (n & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
				TGMrbCheckButtonList * lcb = macroArg->fEntry->GetLofCheckButtons();
				if (lcb->GetActive()) {
					currentValue += ":T";
					checkBtn = "kTRUE";
				} else {
					currentValue += ":F";
					checkBtn = "kFALSE";
				}
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
			buttonBits = macroArg->fRadio->GetActive();
			TMrbNamedX * btn = macroArg->fButtons.FindByIndex(buttonBits);
			currentValue = macroArg->fButtons[0]->GetName();
			if (btn) {
				currentValue = btn->GetName();
				if (currentValue.CompareTo("Yes") == 0) {
					argString[0] = "kTRUE";
				} else {
					argString[0] = "kFALSE";
				}
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryRadio) {
			buttonBits = macroArg->fRadio->GetActive();
			argType = macroArg->fType->GetIndex();
			if (argType == TGMrbMacroArg::kGMrbMacroArgInt || argType == TGMrbMacroArg::kGMrbMacroArgUInt) {
				argString[0] = "";
				argString[0] += buttonBits;
			} else {
				TMrbNamedX * btn =  macroArg->fButtons.FindByIndex(buttonBits);
				argString[0] = btn ? btn->GetName() : macroArg->fButtons[0]->GetName();
			} 
			currentValue = argString[0];
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCheck) {
			buttonBits = macroArg->fCheck->GetActive();
			argString[0] = "";
			argString[0] += buttonBits;
			macroArg->fButtons.Pattern2String(currentValue, buttonBits);
			currentValue = argString[0];
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryCombo) {
			argString[0] = ((TGTextLBEntry *) macroArg->fCombo->GetComboBox()->GetSelectedEntry())->GetText()->GetString();
			currentValue = argString[0];
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFile) {
			argString[0] = macroArg->fFile->GetEntry()->GetText();
			currentValue = argString[0];
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			Int_t at = macroArg->fType->GetIndex();
			if (at == TGMrbMacroArg::kGMrbMacroArgObjArr) {
				macroArg->fFObjCombo->GetSelection(*argArr, kFALSE);
			} else {
				macroArg->fFObjCombo->GetSelectionAsString(argString[0], kFALSE);
				currentValue = argString[0];
			}
		} else if (n == TGMrbMacroArg::kGMrbMacroEntryFObjListBox) {
			Int_t at = macroArg->fType->GetIndex();
			if (at == TGMrbMacroArg::kGMrbMacroArgObjArr) {
				macroArg->fFObjListBox->GetSelection(*argArr, kFALSE);
			} else {
				macroArg->fFObjListBox->GetSelectionAsString(argString[0], kFALSE);
				currentValue = argString[0];
			}
		}
		if (macroArg->fType->GetIndex() == TGMrbMacroArg::kGMrbMacroArgChar) {
			argString[0].Prepend("\"");
			argString[0] += "\"";
		}

		macroArg->GetResource(cVal, "Current");
		if (n == TGMrbMacroArg::kGMrbMacroEntryFObjListBox || n == TGMrbMacroArg::kGMrbMacroEntryFObjCombo) {
			Int_t at = macroArg->fType->GetIndex();
			if (at == TGMrbMacroArg::kGMrbMacroArgObjArr) {
				Int_t nVal = argArr->GetEntriesFast();
				macroEnv->SetValue(Form("%s.NofValues", cVal.Data()), nVal);
				TIterator * iter = argArr->MakeIterator();
				TObjString * vStr;
				nVal = 0;
				while (vStr = (TObjString *) iter->Next()) {
					macroEnv->SetValue(Form("%s.%d", cVal.Data(), nVal), vStr->GetString().Data());
					nVal++;
				}
				argString[0] = Form("(TObjArray *) %#lld", argArr);
			} else {
				macroEnv->SetValue(cVal, currentValue);
			}
		} else {
			macroEnv->SetValue(cVal, currentValue);
		}

		if (!guiOnly) {
			cmd += Form("%s%s", delim.Data(), argString[0].Data());
			delim = ", ";
			if (n & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
				Int_t nofEntryFields = macroArg->fNofEntryFields;
				for (Int_t ef = 1; ef < nofEntryFields; ef++) cmd += Form("%s%s", delim.Data(), argString[ef].Data());
			}

			if (n & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) cmd += Form("%s%s", delim.Data(), checkBtn.Data());

			if (macroArg->fAddLofValues) {
				argString[0] = macroEnv->GetValue(macroArg->GetResource(argString[0], "Values"), "");
				cmd += Form("%s\"%s\"", delim.Data(), argString[0].Data());
			}
		}
	}
	if (addGui || guiOnly) cmd += Form("%s(TGMrbMacroFrame *) %lld", delim.Data(), this);

	cmd += ")";

	TString macroName = macroEnv->GetValue("Name", "macro.C");
	macroName = macroName(0, macroName.Index(".C"));
	macroEnv->WriteFile(Form(".%s.env", macroName.Data()), kEnvAll);

	if (!UserStart) {
		gROOT->ProcessLine(cmd.Data());
	}
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::LoadMacro() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::LoadMacro
// Purpose:        Execute macro
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes macro with given arguments
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fMacroLoaded) return(kTRUE);

	TEnv * macroEnv = (TEnv *) fMacro->GetAssignedObject();
	TString macroName = macroEnv->GetValue("Path", "UNKNOWN");

	TString macroPath = gEnv->GetValue("Root.MacroPath", ".:$HOME/rootmacros:$MARABOU/macros");
	gSystem->ExpandPathName(macroPath);

	TString fileSpec;
	TMrbSystem ux;
	ux.Which(fileSpec, macroPath.Data(), macroName.Data());
	if (fileSpec.IsNull()) {
		gMrbLog->Err()	<< "Macro not found -" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadMacro");
		gMrbLog->Err()	<< "            Searching on path " << macroPath << endl;
		gMrbLog->Flush();
		gMrbLog->Err()	<< "            for file " << macroName << endl;
		gMrbLog->Flush();
		return(kFALSE);
	}

	gSystem->AddIncludePath("-I$MARABOU/include");

	TString rcFile = macroEnv->GetValue("RcFile", "");
	if (!rcFile.IsNull() && rcFile.CompareTo("none") != 0) {
		TString rcSpec;
		ux.Which(rcSpec, macroPath.Data(), rcFile.Data());
		if (rcSpec.IsNull()) {
			gMrbLog->Wrn()	<< "RcFile not found - " << rcFile << endl;
			gMrbLog->Flush(this->ClassName(), "LoadMacro");
		} else {
			Int_t errCode;
			gROOT->Macro(rcFile.Data(), &errCode);
			if (errCode != 0) {
				gMrbLog->Err()  << "Executing rcFile failed - " << rcFile << endl;
				gMrbLog->Flush(this->ClassName(), "LoadMacro");
			}
		}
	}

	TString macroCmd = macroName;
	macroCmd.Remove(macroCmd.Index(".C"), 1000);

	TString aclicFlag = macroEnv->GetValue("Aclic", "+");
	aclicFlag = aclicFlag.Strip(TString::kBoth);
	if (!aclicFlag.BeginsWith("+")) aclicFlag = "+";
	TString aclic = fileSpec;
	aclic += aclicFlag;
	gMrbLog->Out()	<< "Compiling/loading macro " << gSystem->BaseName(macroName.Data()) << ", cmd=\".L " << aclic << "\" - wait ..." << endl;
	gMrbLog->Flush(this->ClassName(), "LoadMacro", setblue);
	Int_t errCode;
	Int_t sts = gROOT->LoadMacro(aclic.Data(), &errCode);
	if (sts == -1 || errCode != 0) {
		gMrbLog->Err()  << "Compile step failed for macro \"" << fileSpec << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadMacro");
		return(kFALSE);
	}
	fMacroLoaded = kTRUE;
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ModifyMacroSource() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ModifyMacroSource
// Purpose:        Modify macro source
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Calls "nedit" to modifiy macro source.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString cmd;
	TEnv * macroEnv;

	macroEnv = (TEnv *) fMacro->GetAssignedObject();
	cmd = "nedit ";
	cmd += macroEnv->GetValue("Path", "UNKNOWN");
	cmd += " &";
	gSystem->Exec(cmd.Data());
	return(kTRUE);
}

Bool_t TGMrbMacroFrame::ModifyMacroHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ModifyMacroHeader
// Purpose:        Modify macro args & layout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Modifies macro arguments and layout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	new TGMrbMacroEdit(fClient->GetRoot(), this, fMacro, 100, 100);
	this->CloseWindow();
	return(kTRUE);
}

TGMrbMacroArg * TGMrbMacroFrame::FindArgByName(const Char_t * ArgName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::FindArgByName
// Purpose:        Find an argument by its name
// Arguments:      Char_t * ArgName         -- name
// Results:        TGMrbMacroArg * ArgPtr   -- pointer to arg defs
// Exceptions:
// Description:    Finds an argument in the list of args
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) return(ap);
	}
	return(NULL);
}

Bool_t TGMrbMacroFrame::SetArgValue(const Char_t * ArgName, Int_t Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgValue
// Purpose:        Set argument value
// Arguments:      Char_t * ArgName         -- name
//                 Int_t Value              -- value
//                 Int_t EntryNo            -- entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets entry field of given argument to value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					ap->fEntry->SetText(Form("%d", Value), EntryNo);
					ap->fEntry->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool) return(kFALSE);
					ap->fRadio->SetState(((Bool_t) Value) ? (Int_t) TGMrbMacroArg::kGMrbMacroEntryYes : (Int_t) TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
					ap->fRadio->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					ap->fRadio->SetState(Value, kButtonDown);
					ap->fRadio->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					ap->fCheck->SetState(Value, kButtonDown);
					ap->fCheck->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::SetArgValue(const Char_t * ArgName, Bool_t Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgValue
// Purpose:        Set argument value
// Arguments:      Char_t * ArgName         -- name
//                 Bool_t Value             -- value
//                 Int_t EntryNo            -- entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets entry field of given argument to value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool) return(kFALSE);
					ap->fEntry->SetText(Value ? "TRUE" : "FALSE", EntryNo);
					ap->fEntry->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool) return(kFALSE);
					ap->fRadio->SetState(Value ? (Int_t) TGMrbMacroArg::kGMrbMacroEntryYes : (Int_t) TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
					ap->fRadio->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::SetArgValue(const Char_t * ArgName, Double_t Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgValue
// Purpose:        Set argument value
// Arguments:      Char_t * ArgName         -- name
//                 Double_t Value           -- value
//                 Int_t EntryNo            -- entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets entry field of given argument to value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgFloat && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgDouble) return(kFALSE);
					ap->fEntry->SetText(Form("%g", Value), EntryNo);
					ap->fEntry->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::SetArgValue(const Char_t * ArgName, const Char_t * Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgValue
// Purpose:        Set argument value
// Arguments:      Char_t * ArgName         -- name
//                 Char_t * Value           -- value
//                 Int_t EntryNo            -- entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets entry field of given argument to value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgChar) return(kFALSE);
					ap->fEntry->SetText(Value, EntryNo);
					ap->fEntry->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					{
						TString yn = Value;
						yn.ToLower();
						if (yn.CompareTo("y") == 0 || yn.CompareTo("yes") == 0) {
							ap->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryYes, kButtonDown);
							ap->fRadio->Layout();
							return(kTRUE);
						}
						if (yn.CompareTo("n") == 0 || yn.CompareTo("no") == 0) {
							ap->fRadio->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
							ap->fRadio->Layout();
							return(kTRUE);
						}
						return(kFALSE);
					}
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
					return(kFALSE);
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
					ap->fCombo->SetText(Value);
					ap->fCombo->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFile:
					ap->fFile->SetText(Value);
					ap->fFile->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
					ap->fFObjCombo->SetText(Value);
					ap->fFObjCombo->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
					ap->fFObjListBox->SetText(Value);
					ap->fFObjListBox->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::SetArgValue(const Char_t * ArgName, TObjArray & Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgValue
// Purpose:        Set argument value
// Arguments:      Char_t * ArgName         -- name
//                 TString & File           -- selected file name
//                 TObjArray & Selection    -- selected items
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets file name and list items for a combobox or listbox
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
					ap->fFObjCombo->SetSelection(Selection);
					ap->fFObjCombo->Layout();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
					ap->fFObjListBox->SetSelection(Selection);
					ap->fFObjListBox->Layout();
					return(kTRUE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgValue(const Char_t * ArgName, Int_t & Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgValue
// Purpose:        Get argument value
// Arguments:      Char_t * ArgName         -- name
//                 Int_t & Value            -- value to be returned
//                 Int_t EntryNo            -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads given argument and converts to int
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	TString argString;
	Value = 0;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					argString = ap->fEntry->GetText(EntryNo);
					Value = argString.Atoi();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool) return(kFALSE);
					Value = ap->fRadio->GetActive();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					Value = ap->fRadio->GetActive();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					Value = ap->fCheck->GetActive();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgValue(const Char_t * ArgName, Bool_t & Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgValue
// Purpose:        Get argument value
// Arguments:      Char_t * ArgName         -- name
//                 Bool_t & Value           -- value to be returned
//                 Int_t EntryNo            -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads given argument and converts to int
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	TString argString;
	Value = kFALSE;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool &&
						ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt &&
						ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					argString = ap->fEntry->GetText(EntryNo);
					argString.ToUpper();
					if (argString.CompareTo("TRUE") == 0) Value = kTRUE;
					else if (argString.CompareTo("FALSE") == 0) Value = kFALSE;
					else if (argString.IsDigit() && (argString.Atoi() != 0)) Value = kTRUE;
					else Value = kFALSE;
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgBool &&
						ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgInt &&
						ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgUInt) return(kFALSE);
					Value = (ap->fRadio->GetActive() != 0);
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgValue(const Char_t * ArgName, Double_t & Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgValue
// Purpose:        Get argument value
// Arguments:      Char_t * ArgName         -- name
//                 Double_t & Value         -- value to be returned
//                 Int_t EntryNo            -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads given argument and converts to double
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	TString argString;
	Value = 0;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgFloat && ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgDouble) return(kFALSE);
					argString = ap->fEntry->GetText(EntryNo);
					Value = argString.Atof();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgValue(const Char_t * ArgName, TString & Value, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgValue
// Purpose:        Get argument value
// Arguments:      Char_t * ArgName         -- name
//                 TString & Value          -- value to be returned
//                 Int_t EntryNo            -- entry number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads given argument and returns it as string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	Value = "";
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					if (ap->fType->GetIndex() != TGMrbMacroArg::kGMrbMacroArgChar) return(kFALSE);
					Value = ap->fEntry->GetText(EntryNo);
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryYesNo:
					{
						UInt_t sts = ap->fRadio->GetActive();
						Value = (sts & TGMrbMacroArg::kGMrbMacroEntryYes) ? "yes" : "no";
						return(kTRUE);
					}
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
					return(kFALSE);
				case TGMrbMacroArg::kGMrbMacroEntryFile:
					Value = ap->fFile->GetText();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
					ap->fCombo->GetText();
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
					ap->fFObjCombo->GetFileEntry(Value, kFALSE);
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
					ap->fFObjListBox->GetFileEntry(Value, kFALSE);
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgValue(const Char_t * ArgName, TObjArray & Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgValue
// Purpose:        Get argument value
// Arguments:      Char_t * ArgName         -- name
//                 TString & File           -- selected file name
//                 TObjArray & Selection    -- selected items
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads given argument (file combo or listbox) and returns
//                 file name as well as selected file members
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	Selection.Delete();
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlain:
				case TGMrbMacroArg::kGMrbMacroEntryPlain2:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown:
				case TGMrbMacroArg::kGMrbMacroEntryUpDown2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownX2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
				case TGMrbMacroArg::kGMrbMacroEntryRadio:
				case TGMrbMacroArg::kGMrbMacroEntryCheck:
				case TGMrbMacroArg::kGMrbMacroEntryText:
				case TGMrbMacroArg::kGMrbMacroEntryFile:
				case TGMrbMacroArg::kGMrbMacroEntryCombo:
				case TGMrbMacroArg::kGMrbMacroEntryComment:
				case TGMrbMacroArg::kGMrbMacroEntrySection:
				case TGMrbMacroArg::kGMrbMacroEntryGroup:
				case TGMrbMacroArg::kGMrbMacroEntryFrame:
				case TGMrbMacroArg::kGMrbMacroEntryPad:
				case TGMrbMacroArg::kGMrbMacroEntryTab:
					return(kFALSE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjCombo:
					ap->fFObjCombo->GetSelection(Selection, kFALSE);
					return(kTRUE);
				case TGMrbMacroArg::kGMrbMacroEntryFObjListBox:
					ap->fFObjListBox->GetSelection(Selection, kFALSE);
					return(kTRUE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::SetArgCheck(const Char_t * ArgName, UInt_t Check) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::SetArgCheck
// Purpose:        Set check button
// Arguments:      Char_t * ArgName         -- name
//                 UInt_t Check             -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets check button
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					ap->fEntry->SetCheckButtonBits(Check);
					ap->fEntry->Layout();
					return(kTRUE);
				default:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::GetArgCheck(const Char_t * ArgName, UInt_t & Check) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::GetArgCheck
// Purpose:        Read check button
// Arguments:      Char_t * ArgName         -- name
//                 UInt_t Check             -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads check button bit
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	Check = 0;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					Check = ap->fEntry->GetCheckButtonBits();
					return(kTRUE);
				default:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

Bool_t TGMrbMacroFrame::ArgIsChecked(const Char_t * ArgName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroFrame::ArgIsChecked
// Purpose:        Test if check button set
// Arguments:      Char_t * ArgName         -- name
//                 UInt_t Check             -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Returns true if check button is set
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * argIter = fLofMacroArgs.MakeIterator();
	TGMrbMacroArg * ap;
	while (ap = (TGMrbMacroArg *) argIter->Next()) {
		TString argName = ap->GetName();
		if (argName.CompareTo(ArgName) == 0) {
			switch (ap->fEntryType->GetIndex()) {
				case TGMrbMacroArg::kGMrbMacroEntryPlainC:
				case TGMrbMacroArg::kGMrbMacroEntryPlainC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownC2:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC:
				case TGMrbMacroArg::kGMrbMacroEntryUpDownXC2:
					return(ap->fEntry->GetCheckButtonBits() != 0);
				default:
					return(kFALSE);
			}
		}
	}
	return(kFALSE);
}

TGMrbMacroEdit::TGMrbMacroEdit(const TGWindow * Parent, const TGWindow * Main, TMrbNamedX * Macro,
										UInt_t Width, UInt_t Height, UInt_t Options) :
													TGTransientFrame(Parent, Main, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit
// Purpose:        A fransient frame to edit macro args 
// Arguments:      TGWindow * Parent     -- parent window
//                 TGWindow * Main       -- main window
//                 TMrbNamedX * Macro    -- macro specs
//                 UInt_t Width          -- width
//                 UInt_t Height         -- height
//                 UInt_t Options        -- options
// Results:        
// Exceptions:     
// Description:    Edits macro arguments and layout.
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=gif/TGMrbMacroEdit.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

	TString macroName, macroPath, titleBar;
	TString argValue;


	UInt_t frameWidth;

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * buttonGC = new TGMrbLayout(font, black, gray);
	HEAP(buttonGC);
	TGMrbLayout * labelGC = new TGMrbLayout(font, black, gray);
	HEAP(labelGC);
	TGMrbLayout * frameGC = new TGMrbLayout(font, black, gray);
	HEAP(frameGC);
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGMrbLayout * entryGC = new TGMrbLayout(font, black, white);
	HEAP(entryGC);
	TGMrbLayout * comboGC = new TGMrbLayout(font, black, white);
	HEAP(comboGC);

//	Initialize several lists
	fLofArgTypes.SetName("Argument types");
	fLofArgTypes.AddNamedX(kGMrbMacroLofArgTypes);
	fLofAclicModes.SetName("Aclic modes");
	fLofAclicModes.AddNamedX(kGMrbMacroLofAclicModes);
	fLofModifyModes.SetName("Modify modes");
	fLofModifyModes.AddNamedX(kGMrbMacroLofModifyModes);
	fLofArglistGuiModes.SetName("GUI modes");
	fLofArglistGuiModes.AddNamedX(kGMrbMacroLofArgListGuiModes);
	fLofUserStartModes.SetName("User start modes");
	fLofUserStartModes.AddNamedX(kGMrbMacroLofUserStartModes);
	fLofEntryTypes.SetName("Entry types");
	fLofEntryTypes.AddNamedX(kGMrbMacroLofEntryTypes);
	fLofArgActions.SetName("Action buttons");
	fLofArgActions.AddNamedX(kGMrbMacroLofEditArgActions);
	fLofActions.SetName("Action buttons");
	fLofActions.AddNamedX(kGMrbMacroLofEditActions);
	fLofOrientations.SetName("Entry orientations");
	fLofOrientations.AddNamedX(kGMrbMacroLofOrientations);
	fLofIntegerBases.SetName("Integer base values");
	fLofIntegerBases.AddNamedX(kGMrbMacroLofIntegerBases);
	fYesNo.SetName("Yes or No");
	fYesNo.AddNamedX(kGMrbMacroYesNoButtons);

	fMacro = Macro; 		// save pointer to macro

	TMrbLofMacros * m = new TMrbLofMacros();			// get list of env names
	fLofEnvNames.AddNamedX(m->GetLofEnvNames());
	delete m;

	fOriginalEnv = (TEnv *) fMacro->GetAssignedObject(); // original settings
	fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
	fCurrentEnv = new TEnv(".listOfArgs");
	this->RestoreHeader();								// copy from original to current
	this->RestoreAllArgs();

	frameWidth = TGMrbMacroEdit::kFrameWidth;

	macroName = Macro->GetName();
	titleBar = "Edit ROOT Macro: ";
	titleBar += macroName;

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 3);
	HEAP(frameLayout);
	frameGC->SetLH(frameLayout);
	fMacroInfo = new TGGroupFrame(this, "Info", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroInfo);
	this->AddFrame(fMacroInfo, frameGC->LH());

	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 50, 1, 3, 3);
	HEAP(labelLayout);
	labelGC->SetLH(labelLayout);

	TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 0, 1, 3, 3);
	HEAP(entryLayout);
	entryGC->SetLH(entryLayout);

	TGLayoutHints * comboLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 1, 3, 3);
	HEAP(comboLayout);
	comboGC->SetLH(comboLayout);

	TGLayoutHints * buttonLayout = new TGLayoutHints(0, 5, 1, 3, 3);
	HEAP(buttonLayout);
	buttonGC->SetLH(buttonLayout);

	fMacroName = new TGMrbLabelEntry(fMacroInfo, "Name", 50, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fMacroName);
	fMacroInfo->AddFrame(fMacroName, frameGC->LH());
	fMacroName->GetEntry()->SetText(Macro->GetName());

	fMacroTitle = new TGMrbLabelEntry(fMacroInfo, "Title", 50, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fMacroTitle);
	fMacroInfo->AddFrame(fMacroTitle, frameGC->LH());
	fMacroTitle->GetEntry()->SetText(fOriginalEnv->GetValue("Title", ""));

	fMacroWidth = new TGMrbLabelEntry(fMacroInfo, "Width", 50, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC, labelGC);
	HEAP(fMacroWidth);
	fMacroInfo->AddFrame(fMacroWidth, frameGC->LH());
	fMacroWidth->SetRange(0, 1000);
	fMacroWidth->SetIncrement(100);
	fMacroWidth->GetEntry()->SetText(fOriginalEnv->GetValue("Width", ""));

	fMacroAclic = new TGMrbRadioButtonList(fMacroInfo, "Aclic mode", &fLofAclicModes, -1, 1,
														frameWidth - 20,
														TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fMacroAclic);
	fMacroInfo->AddFrame(fMacroAclic, frameGC->LH());
	fMacroAclic->SetState(TGMrbMacroEdit::kGMrbMacroAclicPlus);

	fMacroRcFile = new TGMrbLabelEntry(fMacroInfo, "RcFile", 50, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fMacroRcFile);
	fMacroInfo->AddFrame(fMacroRcFile, frameGC->LH());
	fMacroRcFile->GetEntry()->SetText(fOriginalEnv->GetValue("RcFile", ""));

	fMacroModify = new TGMrbRadioButtonList(fMacroInfo, "Modify header and/or code", &fLofModifyModes, -1, 1,
														frameWidth - 20,
														TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fMacroModify);
	fMacroInfo->AddFrame(fMacroModify, frameGC->LH());
	fMacroModify->SetState(TGMrbMacroEdit::kGMrbMacroMayModify);

	fMacroGuiPtrMode = new TGMrbRadioButtonList(fMacroInfo, "Argument list and/or pointer to GUI", &fLofArglistGuiModes, -1, 1,
														frameWidth - 20,
														TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fMacroGuiPtrMode);
	fMacroInfo->AddFrame(fMacroGuiPtrMode, frameGC->LH());
	fMacroGuiPtrMode->SetState(TGMrbMacroEdit::kGMrbMacroArglistOnly);

	fMacroUserStart = new TGMrbRadioButtonList(fMacroInfo, "User start mode",
														&fLofUserStartModes, -1, 1,
														frameWidth - 20,
														TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fMacroUserStart);
	fMacroInfo->AddFrame(fMacroUserStart, frameGC->LH());
	fMacroUserStart->SetState(TGMrbMacroEdit::kGMrbMacroUserStartOff);

	fMacroNofArgs = new TGMrbLabelEntry(fMacroInfo, "Number of arguments", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fMacroNofArgs);
	fMacroInfo->AddFrame(fMacroNofArgs, frameGC->LH());

	fMacroArg = new TGGroupFrame(this, "Argument", kHorizontalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroArg);
	this->AddFrame(fMacroArg, frameGC->LH());

	TGLayoutHints * curargLayout = new TGLayoutHints(kLHintsLeft, 5, 1, 3, 3);
	HEAP(curargLayout);

	fArgNumber = new TGMrbLabelEntry(fMacroArg, "Current", 40, TGMrbMacroEdit::kGMrbMacroEditCurrentArg,
														(frameWidth - 20) / 5,
														TGMrbMacroEdit::kLineHeight, 120,
														frameGC, labelGC, entryGC, labelGC, kTRUE);
	HEAP(fArgNumber);
	fMacroArg->AddFrame(fArgNumber, curargLayout);
	fArgNumber->SetRange(1, TGMrbMacroEdit::kMaxNofArgs);
	fArgNumber->SetIncrement(1);
	fArgNumber->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "SwitchToArg(Int_t, Int_t)");

	fArgAction = new TGMrbTextButtonList(fMacroArg, NULL, &fLofArgActions, -1, 1, frameWidth / 2, TGMrbMacroEdit::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgAction);
	fMacroArg->AddFrame(fArgAction, frameGC->LH());
	((TGMrbButtonFrame *) fArgAction)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButtonPressed(Int_t, Int_t)");

	fMacroLayout = new TGGroupFrame(this, "Layout", kVerticalFrame, frameGC->GC(), frameGC->Font(), frameGC->BG());
	HEAP(fMacroLayout);
	this->AddFrame(fMacroLayout, frameGC->LH());

	fArgName = new TGMrbLabelEntry(fMacroLayout, "Name", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgName);
	fMacroLayout->AddFrame(fArgName, frameGC->LH());

	fArgTitle = new TGMrbLabelEntry(fMacroLayout, "Title", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgTitle);
	fMacroLayout->AddFrame(fArgTitle, frameGC->LH());

	fArgType = new TGMrbLabelCombo(fMacroLayout, "Type", &fLofArgTypes, -1, 0,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight, 100,
														frameGC, labelGC, buttonGC);
	HEAP(fArgType);
	fMacroLayout->AddFrame(fArgType, frameGC->LH());

	fArgEntryType = new TGMrbLabelCombo(fMacroLayout, "Entry type",
														&fLofEntryTypes, -1, 0,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight, 100,
														frameGC, labelGC, buttonGC);
	HEAP(fArgEntryType);
	fMacroLayout->AddFrame(fArgEntryType, frameGC->LH());

	fArgNofEntryFields = new TGMrbLabelEntry(fMacroLayout, "Number of entry fields", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC, labelGC);
	HEAP(fArgNofEntryFields);
	fMacroLayout->AddFrame(fArgNofEntryFields, frameGC->LH());
	fArgNofEntryFields->SetText(1);
	fArgNofEntryFields->SetRange(1, 5);
	fArgNofEntryFields->SetIncrement(1);

	fArgEntryWidth = new TGMrbLabelEntry(fMacroLayout, "Entry width", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC, labelGC);
	HEAP(fArgEntryWidth);
	fMacroLayout->AddFrame(fArgEntryWidth, frameGC->LH());
	fArgEntryWidth->SetRange(0, 1000);
	fArgEntryWidth->SetIncrement(100);

	fArgDefaultValue = new TGMrbLabelEntry(fMacroLayout, "Default value", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgDefaultValue);
	fMacroLayout->AddFrame(fArgDefaultValue, frameGC->LH());

	fArgValues = new TGMrbLabelEntry(fMacroLayout, "Values", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 300,
														frameGC, labelGC, entryGC);
	HEAP(fArgValues);
	fMacroLayout->AddFrame(fArgValues, frameGC->LH());

	fArgAddLofValues = new TGMrbRadioButtonList(fMacroLayout, "Add list of values",
														&fYesNo, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgAddLofValues);
	fMacroLayout->AddFrame(fArgAddLofValues, frameGC->LH());

	fArgLimits = new TGMrbLabelEntry(fMacroLayout, "Limits (lower, incr, upper)", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC,
														NULL, kFALSE, NULL, NULL, NULL, NULL,
														kHorizontalFrame, kSunkenFrame | kDoubleBorder, 3);
	HEAP(fArgLimits);
	fMacroLayout->AddFrame(fArgLimits, frameGC->LH());

	fArgBase = new TGMrbRadioButtonList(fMacroLayout, "Base",
														&fLofIntegerBases, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgBase);
	fMacroLayout->AddFrame(fArgBase, frameGC->LH());

	fArgOrientation = new TGMrbRadioButtonList(fMacroLayout, "Orientation",
														&fLofOrientations, -1, 1,
														frameWidth - 20,
														TGMrbMacroFrame::kLineHeight,
														frameGC, labelGC, buttonGC);
	HEAP(fArgOrientation);
	fMacroLayout->AddFrame(fArgOrientation, frameGC->LH());

	fArgExecFunct = new TGMrbLabelEntry(fMacroLayout, "Exec function", 40, -1, frameWidth - 20,
														TGMrbMacroEdit::kLineHeight, 100,
														frameGC, labelGC, entryGC);
	HEAP(fArgExecFunct);
	fMacroLayout->AddFrame(fArgExecFunct, frameGC->LH());

	fCurrentArg.SetIndex(1);
	argValue = 1;
	fArgNumber->GetEntry()->SetText(argValue.Data());
	this->UpdateArg();
	this->UpdateNofArgs();

	TGLayoutHints * actionLayout = new TGLayoutHints(kLHintsExpandX, 5, 1, 3, 1);
	HEAP(actionLayout);
	buttonGC->SetLH(actionLayout);

	fAction = new TGMrbTextButtonGroup(this, "Macro action", &fLofActions, -1, 1, frameGC, buttonGC);
	HEAP(fAction);
	this->AddFrame(fAction, frameGC->LH());
	((TGMrbButtonFrame *) fAction)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ActionButtonPressed(Int_t, Int_t)");

	this->ChangeBackground(gray);
	this->SetWindowName(titleBar.Data());

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(), (((TGFrame *) Main)->GetWidth() + 10), 0, ax, ay, wdum);
	this->Move(ax, ay);

	this->MapSubwindows();
	this->Resize(this->GetDefaultSize());

	this->Resize(frameWidth, fMacroInfo->GetDefaultHeight()
								+ fMacroArg->GetDefaultHeight()
								+ fMacroLayout->GetDefaultHeight()
								+ fAction->GetDefaultHeight()
								+ 20);

	this->MapWindow();

	gClient->WaitFor(this);
}

void TGMrbMacroEdit::ActionButtonPressed(Int_t /*FrameId*/, Int_t Button) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::ActionButtonPressed
// Purpose:        Slot method
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Button      -- button number
// Results:        --
// Exceptions:     
// Description:    Handle messages sent to TGMrbMacroEdit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue;

	TGFileInfo fo;

	TMrbLofMacros lofMacros;
	TMrbNamedX * macro;

	switch (Button) {
		case kGMrbMacroEditIdRemoveArg:
			this->RemoveArg();
			this->UpdateNofArgs();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdResetArg:
			this->RestoreArg();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdNewArgBefore:
			this->StoreArg();
			this->InsertArg(fCurrentArg.GetIndex());
			this->UpdateNofArgs();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdNewArgAfter:
			this->StoreArg();
			this->InsertArg(fCurrentArg.GetIndex() + 1);
			this->UpdateNofArgs();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdResetAll:
			this->RestoreHeader();
			fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
			this->UpdateNofArgs();
			this->RestoreAllArgs();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdResetHeader:
			this->RestoreHeader();
			break;
		case kGMrbMacroEditIdResetAllArgs:
			this->RestoreAllArgs();
			fNofArgs = fOriginalEnv->GetValue("NofArgs", 1);
			this->UpdateNofArgs();
			this->UpdateArg();
			break;
		case kGMrbMacroEditIdSave:
			fo.fFileTypes = (const char **) macroFileTypes;
			new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fo);
			if (fo.fFilename != NULL && *fo.fFilename != '\0') {
				if (this->SaveMacro(fo.fFilename, fMacro->GetName())) {
					if (lofMacros.AddMacro(fo.fFilename)) {
						macro = lofMacros.FirstMacro();
						fMacro->GetAssignedObject()->Delete();
						fMacro->AssignObject(macro->GetAssignedObject());
						this->CloseWindow();
					}
				}
			}
			break;
		case kGMrbMacroEditIdClose:
			this->CloseWindow();
			break;
	}
}

Bool_t TGMrbMacroEdit::SwitchToArg(Int_t /*FrameId*/, Int_t /*EntryNo*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::SwitchToArg
// Purpose:        Switch to next argument
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry number (ignored)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Switches to argument given by text field
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue = fArgNumber->GetEntry()->GetText();
	Int_t argNo = argValue.Atoi();
	argValue = "";
	if (argNo < 1) {
		this->StoreArg();
		fCurrentArg.SetIndex(1); 
		argValue += 1;
		fArgNumber->GetEntry()->SetText(argValue.Data());
		this->UpdateArg();
	} else if (argNo > fNofArgs) {
		this->StoreArg();
		fCurrentArg.SetIndex(fNofArgs); 
		argValue += fNofArgs;
		fArgNumber->GetEntry()->SetText(argValue.Data());
		this->UpdateArg();
	} else {
		this->StoreArg();
		fCurrentArg.SetIndex(argNo); 
		this->UpdateArg();
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::UpdateArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::UpdateArg
// Purpose:        Update GUI for an argument
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Updates GUI fields for a given argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue, argType, argEntryType, argBase, argOrientation;

	TMrbNamedX * nx;
	Int_t idx;

	TGMrbMacroArg thisArg;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetIndex();
	thisArg.SetIndex(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateArg");
		return(kFALSE);
	}


	fArgName->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Name"), ""));
	thisArg.SetName(thisArg.GetResource(argValue, "Name"));
	fArgTitle->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Title"), ""));

	argType = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Type"), "");
	nx = fLofArgTypes.FindByName(argType.Data());
	idx = nx ? nx->GetIndex() : 0;
	fArgType->Select(idx);

	argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "EntryType"), "");
	nx = fLofEntryTypes.FindByName(argEntryType.Data());
	idx = nx ? nx->GetIndex() : 0;
	fArgEntryType->Select(idx);

	fArgNofEntryFields->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "NofEntryFields"), ""));
	fArgEntryWidth->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Width"), ""));
	fArgDefaultValue->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Default"), ""));
	fArgValues->GetEntry()->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Values"), ""));
	argValue = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "AddLofValues"), "No");
	nx = fYesNo.FindByName(argValue.Data());
	if (nx) fArgAddLofValues->SetState(nx->GetIndex(), kButtonDown);
	else	fArgAddLofValues->SetState(TGMrbMacroArg::kGMrbMacroEntryNo, kButtonDown);
	fArgLimits->GetEntry(0)->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "LowerLimit"), ""));
	fArgLimits->GetEntry(1)->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Increment"), ""));
	fArgLimits->GetEntry(2)->SetText(fCurrentEnv->GetValue(thisArg.GetResource(argValue, "UpperLimit"), ""));

	argBase = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Base"), "");
	nx = fLofEntryTypes.FindByName(argBase.Data());
	idx = nx ? nx->GetIndex() : 1;
	fArgBase->SetState(idx, kButtonDown);

	argOrientation = fCurrentEnv->GetValue(thisArg.GetResource(argValue, "Orientation"), "");
	nx = fLofEntryTypes.FindByName(argOrientation.Data());
	idx = nx ? nx->GetIndex() : 1;
	fArgOrientation->SetState(idx, kButtonDown);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::StoreHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::StoreHeader
// Purpose:        Xfer entry data from GUI to environment
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stores GUI data to environment.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue;

	argValue = fMacroName->GetEntry()->GetText();
	fCurrentEnv->SetValue("Name", argValue.Data(), kEnvChange);
	argValue = fMacroTitle->GetEntry()->GetText();
	fCurrentEnv->SetValue("Title", argValue.Data(), kEnvChange);
	argValue = fMacroWidth->GetEntry()->GetText();
	fCurrentEnv->SetValue("Width", argValue.Data(), kEnvChange);
	Int_t idx = fMacroAclic->GetActive();
	TMrbNamedX * nx = fLofAclicModes.FindByIndex(idx);
	argValue = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue("Aclic", argValue.Data(), kEnvChange);
	idx = fMacroModify->GetActive();
	nx = fLofModifyModes.FindByIndex(idx);
	argValue = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue("Modify", argValue.Data(), kEnvChange);
	idx = fMacroGuiPtrMode->GetActive();
	nx = fLofArglistGuiModes.FindByIndex(idx);
	argValue = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue("GuiPtrMode", argValue.Data(), kEnvChange);
	argValue = fMacroRcFile->GetEntry()->GetText();
	fCurrentEnv->SetValue("RcFile", argValue.Data(), kEnvChange);
	idx = fMacroUserStart->GetActive();
	nx = fLofUserStartModes.FindByIndex(idx);
	argValue = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue("UserStart", argValue.Data(), kEnvChange);
	argValue = fMacroNofArgs->GetEntry()->GetText();
	fCurrentEnv->SetValue("NofArgs", argValue.Data(), kEnvChange);
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::StoreArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::StoreArg
// Purpose:        Xfer entry data from GUI to environment
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Stores GUI data to environment.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argName, argValue, argType, argEntryType, argBase, argOrientation;

	TMrbNamedX * nx;
	Int_t idx;

	TGMrbMacroArg thisArg;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetIndex();
	thisArg.SetIndex(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "StoreArg");
		return(kFALSE);
	}

	argValue = fArgName->GetEntry()->GetText();
	thisArg.SetName(argValue.Data());
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Name"), argValue.Data(), kEnvChange);
	argValue = fArgTitle->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Title"), argValue.Data(), kEnvChange);

	idx = fArgType->GetSelected();
	nx  = fLofArgTypes.FindByIndex(idx);
	argType = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Type"), argType.Data(), kEnvChange);

	idx = fArgEntryType->GetSelected();
	nx = fLofEntryTypes.FindByIndex(idx);
	argEntryType = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "EntryType"), argEntryType.Data(), kEnvChange);

	argValue = fArgNofEntryFields->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "NofEntryFields"), argValue.Data(), kEnvChange);
	argValue = fArgEntryWidth->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Width"), argValue.Data(), kEnvChange);
	argValue = fArgDefaultValue->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Default"), argValue.Data(), kEnvChange);
	argValue = fArgValues->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Values"), argValue.Data(), kEnvChange);
	nx = fYesNo.FindByIndex(fArgAddLofValues->GetActive());
	if (nx) argValue = nx->GetName(); else argValue = "No";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "AddLofValues"), argValue.Data(), kEnvChange);
	argValue = fArgLimits->GetEntry(0)->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "LowerLimit"), argValue.Data(), kEnvChange);
	argValue = fArgLimits->GetEntry(1)->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Increment"), argValue.Data(), kEnvChange);
	argValue = fArgLimits->GetEntry(2)->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "UpperLimit"), argValue.Data(), kEnvChange);

	idx = fArgBase->GetActive();
	nx = fLofIntegerBases.FindByIndex(idx);
	argBase = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Base"), argBase.Data(), kEnvChange);

	idx = fArgOrientation->GetActive();
	nx = fLofOrientations.FindByIndex(idx);
	argOrientation = nx ? nx->GetName() : "";
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "Orientation"), argOrientation.Data(), kEnvChange);

	argValue = fArgExecFunct->GetEntry()->GetText();
	fCurrentEnv->SetValue(thisArg.GetResource(argName, "ExecFunct"), argValue.Data(), kEnvChange);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RemoveArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RemoveArg
// Purpose:        Remove an argument and its environment definitions
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Removes a given argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetIndex();

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "RemoveArg");
		return(kFALSE);
	}

	if (this->ChangeEnv(ArgNo, kTRUE)) {
		fNofArgs--;
		if (fNofArgs <= 0) {
			this->InsertArg(1);
			fCurrentArg.SetIndex(1);
			argValue = fNofArgs;
			fArgNumber->GetEntry()->SetText(argValue.Data());
		} else if (fCurrentArg.GetIndex() > fNofArgs) {
			fCurrentArg.SetIndex(fNofArgs);
			argValue = fNofArgs;
			fArgNumber->GetEntry()->SetText(argValue.Data());
		}
		return(kTRUE);
	} else return(kFALSE);
}

Bool_t TGMrbMacroEdit::InsertArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::InsertArg
// Purpose:        Insert an argument and create empty environment definitions
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Inserts a new argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetIndex();

	if (ArgNo < 1 || ArgNo > fNofArgs + 1) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs + 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "InsertArg");
		return(kFALSE);
	}

	if (this->ChangeEnv(ArgNo, kFALSE)) {
		fNofArgs++;
		fCurrentArg.SetIndex(ArgNo);
		argValue = ArgNo;
		fArgNumber->GetEntry()->SetText(argValue.Data());
		return(kTRUE);
	} else return(kFALSE);
}

Bool_t TGMrbMacroEdit::RestoreArg(Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreArg
// Purpose:        Restore argument settings from original environment
// Arguments:      Int_t ArgNo   -- argument number
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores an argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TGMrbMacroArg thisArg;
	TString envName, envVal, argEnv;

	if (ArgNo == -1) ArgNo = fCurrentArg.GetIndex();
	thisArg.SetIndex(ArgNo);

	if (ArgNo < 1 || ArgNo > fNofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << fNofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreArg");
		return(kFALSE);
	}

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == 0) {
			envName = envName(4, envName.Length() - 4);
			thisArg.GetResource(argEnv, envName.Data());
			envVal = fOriginalEnv->GetValue(argEnv.Data(), "");
			fCurrentEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RestoreHeader() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreHeader
// Purpose:        Restore header settings from original environment
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores header data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TString envName, envVal;

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == -1) {
			envVal = fOriginalEnv->GetValue(envName.Data(), "");
			fCurrentEnv->SetValue(envName.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::RestoreAllArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::RestoreAllArgs
// Purpose:        Restore argument settings from original environment
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores all arguments.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = 1; i <= fNofArgs; i++) this->RestoreArg(i);
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::UpdateNofArgs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::UpdateNofArgs
// Purpose:        Update GUI entry "number of arguments"
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores all arguments.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString argValue = fNofArgs;
	fMacroNofArgs->GetEntry()->SetText(argValue.Data());
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::ChangeEnv(Int_t ArgNo, Bool_t Delete) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::ChangeEnv
// Purpose:        Change environment settings
// Arguments:      Int_t ArgNo      -- argument number
//                 Bool_t Delete    -- if kTRUE -> delete argument
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Change environment: delete / insert an argument.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg thisArg, nextArg;
	TString argEnv, envName, envVal;
	TString argName;
	TEnv * newEnv;
	TMrbNamedX * nx;

	Int_t nofArgs = (Delete) ? fNofArgs : fNofArgs + 1;

	if (ArgNo < 1 || ArgNo > nofArgs) {
		gMrbLog->Err() << "Arg number " << ArgNo << " out of range (should be in [1, " << nofArgs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ChangeEnv");
		return(kFALSE);
	}

	newEnv = new TEnv(".listOfArgs");

	nx = (TMrbNamedX *) fLofEnvNames.First();
	while (nx) {
		envName = nx->GetName();
		if (envName.Index("Arg.", 0) == -1) {
			envVal = fCurrentEnv->GetValue(envName.Data(), "");
			newEnv->SetValue(envName.Data(), envVal.Data(), kEnvChange);
		}
		nx = (TMrbNamedX *) fLofEnvNames.After(nx);
	}

	if (Delete) {
		for (Int_t i = 1; i < ArgNo; i++) {
			thisArg.SetIndex(i);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
		for (Int_t i = ArgNo + 1; i <= fNofArgs; i++) {
			thisArg.SetIndex(i);
			nextArg.SetIndex(i - 1);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(nextArg.GetResource(argEnv, envName.Data()), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
	} else {
		for (Int_t i = 1; i < ArgNo; i++) {
			thisArg.SetIndex(i);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(argEnv.Data(), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
		thisArg.SetIndex(ArgNo);
		nx = (TMrbNamedX *) fLofEnvNames.First();
		while (nx) {
			envName = nx->GetName();
			if (envName.Index("Arg.", 0) == 0) {
				envName = envName(4, envName.Length() - 4);
				thisArg.GetResource(argEnv, envName.Data());
				newEnv->SetValue(argEnv.Data(), "", kEnvChange);
			}
			nx = (TMrbNamedX *) fLofEnvNames.After(nx);
		}
		for (Int_t i = ArgNo; i <= fNofArgs; i++) {
			thisArg.SetIndex(i);
			nextArg.SetIndex(i + 1);
			nx = (TMrbNamedX *) fLofEnvNames.First();
			while (nx) {
				envName = nx->GetName();
				if (envName.Index("Arg.", 0) == 0) {
					envName = envName(4, envName.Length() - 4);
					envVal = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, envName.Data()), "");
					newEnv->SetValue(nextArg.GetResource(argEnv, envName.Data()), envVal.Data(), kEnvChange);
				}
				nx = (TMrbNamedX *) fLofEnvNames.After(nx);
			}
		}
	}

	delete fCurrentEnv;
	fCurrentEnv = newEnv;
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::SaveMacro(const Char_t * NewFile, const Char_t * OldFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::SaveMacro
// Purpose:        Save macro settings
// Arguments:      Char_t * NewFile   -- new file name (.C)
//                 Char_t * OldFile   -- old file name
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Write macro definitions to a file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString macroTemplateFile;
	ofstream macroStrm;
	ifstream orgStrm;
	TMrbTemplate macroTmpl;
	TMrbNamedX * macroTag;
	TMrbLofNamedX lofMacroTags;
	TString line;
	TGMrbMacroEdit::EGMrbMacroTag tagIdx;
	TString orgFile, macroName, macroType;
	TDatime dt;
	TGMrbMacroArg thisArg;
	TMrbNamedX * nx;

	Int_t yesno;

	TString argEnv, argName, argTitle, argType, argEntryType, argDefault, argDel, addLofValues;
	TString envName, envVal;
	TString argSpace, envSpace;

	TGMrbMacroArg::EGMrbMacroEntryType argEntryTypeBits = (TGMrbMacroArg::EGMrbMacroEntryType)0;

	TString templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", "");
	if (templatePath.Length() == 0) templatePath = ".:$(MARABOU)/templates/macros";
	gSystem->ExpandPathName(templatePath);

	Bool_t fileIsNew = kTRUE;
	Bool_t sameFile = kFALSE;
	TMrbSystem ux;
	TString newFilePath = NewFile;
	TString newFile; ux.GetBaseName(newFile, NewFile);
	TString oldFilePath = OldFile;
	TString oldFile; ux.GetBaseName(oldFile, OldFile);

	if (!gSystem->AccessPathName(newFilePath.Data())) {
		TString msg = this->ClassName();
		msg += "::SaveMacro()";
		new TGMsgBox(fClient->GetRoot(), this, msg, Form("%s:\nFile exists. Overwrite?", newFilePath.Data()), kMBIconQuestion, kMBYes | kMBNo, &yesno);
		if (yesno != 1) return(kFALSE);
		fileIsNew = kFALSE;
	}

	if (!fileIsNew) sameFile = ux.IsSameFile(newFile.Data(), oldFilePath.Data());

	if (sameFile) {
		orgFile = newFile;
		orgFile += ".org";
		gSystem->Unlink(orgFile.Data());
		gSystem->Rename(newFile.Data(), orgFile.Data());
	} else {
		orgFile = oldFile;
	}

	macroName = newFile(0, newFile.Index("."));

	macroStrm.open(newFile.Data(), ios::out);
	if (!macroStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << newFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveMacro");
		return(kFALSE);
	}

	TString fileSpec;
	ux.Which(fileSpec, templatePath.Data(), "MacroBrowserTemplate.C");
	if (fileSpec.IsNull()) {
		gMrbLog->Err() << "No such file - " << templatePath << "/" << "MacroBrowserTemplate.C"	<< endl;
		gMrbLog->Flush(this->ClassName(), "Savemacro");
		return(kFALSE);
	}

	this->StoreHeader();
	this->StoreArg();

	macroTemplateFile = fileSpec;

	Bool_t addGui = kFALSE;
	Bool_t guiOnly = kFALSE;
	Int_t idx = fMacroGuiPtrMode->GetActive();
	TMrbNamedX * guiMode = fLofArglistGuiModes.FindByIndex(idx);
	if (guiMode) {
		addGui = (guiMode->GetIndex() == TGMrbMacroEdit::kGMrbMacroArglistAndGui);
		guiOnly = (guiMode->GetIndex() == TGMrbMacroEdit::kGMrbMacroGuiPtrOnly);
	}

	Int_t lastRealArg = 0;
	for (Int_t na = 1; na <= fNofArgs; na++) {
		thisArg.SetIndex(na);
		argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
		TMrbNamedX * argx = fLofEntryTypes.FindByName(argEntryType.Data());
		if (argx) argEntryTypeBits = (TGMrbMacroArg::EGMrbMacroEntryType) argx->GetIndex();
		if (argEntryTypeBits & (
									TGMrbMacroArg::kGMrbMacroEntryComment |
									TGMrbMacroArg::kGMrbMacroEntrySection |
									TGMrbMacroArg::kGMrbMacroEntryGroup |
									TGMrbMacroArg::kGMrbMacroEntryFrame |
									TGMrbMacroArg::kGMrbMacroEntryText |
									TGMrbMacroArg::kGMrbMacroEntryPad |
									TGMrbMacroArg::kGMrbMacroEntryTab
								)) continue;
		lastRealArg = na;
	}

	lofMacroTags.AddNamedX(kGMrbMacroLofTags);
	if (!macroTmpl.Open(macroTemplateFile, &lofMacroTags)) return(kFALSE);

	for (;;) {
		macroTag = macroTmpl.Next(line);
		if (macroTmpl.IsEof()) break;
		if (macroTmpl.IsError()) continue;
		if (macroTmpl.Status() & TMrbTemplate::kNoTag) {
			if (line.Index("//-") != 0) macroStrm << line << endl;
		} else {
			switch (tagIdx = (TGMrbMacroEdit::EGMrbMacroTag) macroTag->GetIndex()) {
				case TGMrbMacroEdit::kMacroTagFile:
					macroStrm << macroTmpl.Encode(line, newFile) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagName:
					macroStrm << macroTmpl.Encode(line, macroName) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagTitle:
					macroStrm << macroTmpl.Encode(line, fCurrentEnv->GetValue("Title", "")) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagAuthor:
					macroStrm << macroTmpl.Encode(line, gSystem->Getenv("USER")) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagMailAddr:
					{
						TString mailAddr = gEnv->GetValue("User.MailAddr", gSystem->Getenv("MAILADDR"));
						macroStrm << macroTmpl.Encode(line, mailAddr.Data()) << endl;
					}
					break;
				case TGMrbMacroEdit::kMacroTagUrl:
					{
						TString url = gEnv->GetValue("User.URL", gSystem->Getenv("URL"));
						macroStrm << macroTmpl.Encode(line, url.Data()) << endl;
					}
					break;
				case TGMrbMacroEdit::kMacroTagCreationDate:
					macroStrm << macroTmpl.Encode(line, dt.AsString()) << endl;
					break;
				case TGMrbMacroEdit::kMacroTagSyntax:
					if (guiOnly) {
						macroTmpl.InitializeCode("%ARGGUI%");
						macroTmpl.Substitute("$macroFile", newFile);
						macroTmpl.Substitute("$macroName", macroName);
						macroTmpl.WriteCode(macroStrm);
					} else {
						Bool_t firstArg = kTRUE;
						for (Int_t na = 1; na <= fNofArgs; na++) {
							thisArg.SetIndex(na);
							argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
							TMrbNamedX * argx = fLofEntryTypes.FindByName(argEntryType.Data());
							if (argx) argEntryTypeBits = (TGMrbMacroArg::EGMrbMacroEntryType) argx->GetIndex();
							if (argEntryTypeBits & (
														TGMrbMacroArg::kGMrbMacroEntryComment |
														TGMrbMacroArg::kGMrbMacroEntrySection |
														TGMrbMacroArg::kGMrbMacroEntryGroup |
														TGMrbMacroArg::kGMrbMacroEntryFrame |
														TGMrbMacroArg::kGMrbMacroEntryText |
														TGMrbMacroArg::kGMrbMacroEntryPad |
														TGMrbMacroArg::kGMrbMacroEntryTab
													)) continue;
							if (firstArg) macroTmpl.InitializeCode("%ARG1%");
							else		macroTmpl.InitializeCode("%ARGN%");
							argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
							thisArg.SetName(argName.Data());
							argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
							argSpace.Resize(0); Int_t n = newFile.Length() + 5; for (; n--;) argSpace.Insert(0, " ");
							macroTmpl.Substitute("$macroFile", newFile);
							macroTmpl.Substitute("$macroName", macroName);
							macroTmpl.Substitute("$argName", argName);
							macroTmpl.Substitute("$argSpace", argSpace);
							if (argType.BeginsWith("Char_t")) argType.Prepend("const ");
							macroTmpl.Substitute("$argType", argType);
							argDel = (na == lastRealArg) ? ")" : ",";
							macroTmpl.Substitute("$argDel", argDel);
							macroTmpl.WriteCode(macroStrm);
							firstArg = kFALSE;
						}
					}
					break;
				case TGMrbMacroEdit::kMacroTagArguments:
					if (guiOnly) {
						macroTmpl.InitializeCode("%ARGGUI%");
						macroTmpl.WriteCode(macroStrm);
					} else {
						Bool_t firstArg = kTRUE;
						for (Int_t na = 1; na <= fNofArgs; na++) {
						thisArg.SetIndex(na);
						argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
						TMrbNamedX * argx = fLofEntryTypes.FindByName(argEntryType.Data());
						if (argx) argEntryTypeBits = (TGMrbMacroArg::EGMrbMacroEntryType) argx->GetIndex();
						if (argEntryTypeBits & (
													TGMrbMacroArg::kGMrbMacroEntryComment |
													TGMrbMacroArg::kGMrbMacroEntrySection |
													TGMrbMacroArg::kGMrbMacroEntryGroup |
													TGMrbMacroArg::kGMrbMacroEntryFrame |
													TGMrbMacroArg::kGMrbMacroEntryText |
													TGMrbMacroArg::kGMrbMacroEntryPad |
													TGMrbMacroArg::kGMrbMacroEntryTab
												)) continue;
						if (firstArg) macroTmpl.InitializeCode("%ARG1%");
						else		macroTmpl.InitializeCode("%ARGN%");
						argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
						thisArg.SetName(argName.Data());
						argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
						argTitle = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Title"), "");
						argSpace.Resize(0); Int_t n = 25 - (argType.Length() + argName.Length() + 1); for (; n--;) argSpace.Insert(0, " ");
						macroTmpl.Substitute("$argName", argName);
						macroTmpl.Substitute("$argType", argType);
						macroTmpl.Substitute("$argTitle", argTitle);
						macroTmpl.Substitute("$argSpace", argSpace);
						macroTmpl.WriteCode(macroStrm);
						firstArg = kFALSE;
					}
				}
				break;
				case TGMrbMacroEdit::kMacroTagExec:
					macroStrm << "//+Exec __________________________________________________[ROOT MACRO BROWSER]" << endl;
					nx = (TMrbNamedX *) fLofEnvNames.First();
					while (nx) {
						envName = nx->GetName();
						if (envName.Index("Arg.", 0) == -1) {
							if (envName.CompareTo("Path") != 0) {
								envVal = fCurrentEnv->GetValue(envName.Data(), "");
								envSpace.Resize(0); Int_t n = 20 - envName.Length(); for (; n--;) envSpace.Insert(0, " ");
								macroTmpl.InitializeCode();
								macroTmpl.Substitute("$envName", envName);
								macroTmpl.Substitute("$envVal", envVal);
								macroTmpl.Substitute("$envSpace", envSpace);
								macroTmpl.WriteCode(macroStrm);
							}
						}
						nx = (TMrbNamedX *) fLofEnvNames.After(nx);
					}
					for (Int_t i = 1; i <= fNofArgs; i++) {
						thisArg.SetIndex(i);
						nx = (TMrbNamedX *) fLofEnvNames.First();
						while (nx) {
							envName = nx->GetName();
							if (envName.Index("Arg.", 0) == 0) {
								envName = envName(4, envName.Length() - 4);
								thisArg.GetResource(argEnv, envName.Data());
								envVal = fCurrentEnv->GetValue(argEnv.Data(), "");
								if (envVal.Length() > 0) {
									envSpace.Resize(0); Int_t n = 20 - argEnv.Length(); for (; n--;) envSpace.Insert(0, " ");
									macroTmpl.InitializeCode();
									macroTmpl.Substitute("$envName", argEnv);
									macroTmpl.Substitute("$envVal", envVal);
									macroTmpl.Substitute("$envSpace", envSpace);
									macroTmpl.WriteCode(macroStrm);
								}
							}
							nx = (TMrbNamedX *) fLofEnvNames.After(nx);
						}
					}
					macroStrm << "//-Exec" << endl;
					break;
				case TGMrbMacroEdit::kMacroTagIncludeHFiles:
					macroTmpl.InitializeCode("%B%");
					macroTmpl.WriteCode(macroStrm);
					if (fileIsNew) {
						macroTmpl.InitializeCode("%NEW%");
						macroTmpl.WriteCode(macroStrm);
						if (addGui || guiOnly) {
							macroTmpl.InitializeCode("%GUI%");
							macroTmpl.WriteCode(macroStrm);
						}
					} else {
						orgStrm.open(orgFile.Data(), ios::in);
						if (!orgStrm.good()) {	
							gMrbLog->Err() << gSystem->GetError() << " - " << orgFile << endl;
							gMrbLog->Flush(this->ClassName(), "SaveMacro");
						} else {
							for (;;) {
								line.ReadLine(orgStrm, kFALSE);
								if (orgStrm.eof()) break;
								if (line.Index("#include", 0) == 0) macroStrm << line << endl;
							}
							orgStrm.close();
						}
					}
					macroTmpl.InitializeCode("%E%");
					macroTmpl.WriteCode(macroStrm);
					break;
				case TGMrbMacroEdit::kMacroTagArgEnums:
					{
						TString mName = macroName;
						mName(0,1).ToUpper();
						macroTmpl.InitializeCode("%B%");
						macroTmpl.Substitute("$macroName", mName);
						macroTmpl.WriteCode(macroStrm);
						for (Int_t i = 1; i <= fNofArgs; i++) {
								thisArg.SetIndex(i);
								argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
								thisArg.SetName(argName.Data());
								macroTmpl.InitializeCode("%N%");
								macroTmpl.Substitute("$enumName", Form("kArg%s", argName.Data()));
								macroTmpl.Substitute("$enumVal", i);
								macroTmpl.WriteCode(macroStrm);
						}
						macroTmpl.InitializeCode("%E%");
						macroTmpl.WriteCode(macroStrm);
					}
					break;
				case TGMrbMacroEdit::kMacroTagEnums:
					{
						TString mName = macroName;
						mName(0,1).ToUpper();
						macroTmpl.InitializeCode("%B%");
						macroTmpl.Substitute("$macroName", mName);
						macroTmpl.WriteCode(macroStrm);
						for (Int_t i = 1; i <= fNofArgs; i++) {
							TMrbLofNamedX lofEnums;
							Int_t nofEnums = this->ExtractEnums(lofEnums, i);
							if (nofEnums > 0) {
								TIterator * enumIter = lofEnums.MakeIterator();
								TMrbNamedX * en;
								while (en = (TMrbNamedX *) enumIter->Next()) {
									macroTmpl.InitializeCode("%N%");
									macroTmpl.Substitute("$enumName", en->GetName());
									macroTmpl.Substitute("$enumVal", en->GetIndex());
									macroTmpl.WriteCode(macroStrm);
								}
							}
						}
						macroTmpl.InitializeCode("%E%");
						macroTmpl.WriteCode(macroStrm);
					}
					break;
				case TGMrbMacroEdit::kMacroTagCode:
					macroTmpl.InitializeCode("%B%");
					macroTmpl.WriteCode(macroStrm);
					if (guiOnly) {
						macroType = "void";
						macroTmpl.InitializeCode("%ARGGUI%");
						macroTmpl.Substitute("$macroType", macroType);
						macroTmpl.Substitute("$macroName", macroName);
						macroTmpl.WriteCode(macroStrm);
						macroTmpl.InitializeCode("%E%");
						macroTmpl.WriteCode(macroStrm);
						if (fileIsNew) {
							macroTmpl.InitializeCode("%NEW%");
							macroTmpl.WriteCode(macroStrm);
						}
					} else {
						macroType = "void";
						Bool_t hasDefaults = kFALSE;
						Bool_t firstArg = kTRUE;
						for (Int_t na = 1; na <= fNofArgs; na++) {
							thisArg.SetIndex(na);
							argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
							thisArg.SetName(argName.Data());
							argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
							TMrbNamedX * argx = fLofEntryTypes.FindByName(argEntryType.Data());
							if (argx) argEntryTypeBits = (TGMrbMacroArg::EGMrbMacroEntryType) argx->GetIndex();
							if (argEntryTypeBits & (
														TGMrbMacroArg::kGMrbMacroEntryComment |
														TGMrbMacroArg::kGMrbMacroEntrySection |
														TGMrbMacroArg::kGMrbMacroEntryGroup |
														TGMrbMacroArg::kGMrbMacroEntryFrame |
														TGMrbMacroArg::kGMrbMacroEntryText |
														TGMrbMacroArg::kGMrbMacroEntryPad |
														TGMrbMacroArg::kGMrbMacroEntryTab
													)) continue;
							argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
							argDefault = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Default"), "");
							if (argDefault.Length() == 0 && hasDefaults) {
								TString spor0 = argType.BeginsWith("Char_t") ? "<space>" : "0";
								gMrbLog->Wrn()	<< "Default value missing - argument \""
												<< argName << "\" (#" << na << ", " << argEntryType
												<< ", " << argType << "); substituting " << spor0 << endl;
								gMrbLog->Flush(this->ClassName(), "SaveMacro");
								if (argType.BeginsWith("Char_t")) argDefault = " "; else argDefault = "0";
							}
							if (firstArg) {
								argSpace.Resize(0);
								TString iniStr;
								if (argDefault.Length() > 0) {
									hasDefaults = kTRUE;
									if (argType.BeginsWith("Char_t"))	iniStr = "%ARG1DC%";
									else								iniStr = "%ARG1D%";
								} else									iniStr = "%ARG1%";
								macroTmpl.InitializeCode(iniStr.Data());
							} else {
								argSpace.Resize(0); Int_t n = macroType.Length() + macroName.Length() + 2; for (; n--;) argSpace.Insert(0, " ");
								if (argDefault.Length() > 0) {
									hasDefaults = kTRUE;
									if (argType.BeginsWith("Char_t")) {
										macroTmpl.InitializeCode("%ARGNDC%");
									} else {
										macroTmpl.InitializeCode("%ARGND%");
										if (argEntryType.CompareTo("Radio") == 0 || argEntryType.CompareTo("Check") == 0) {
											TMrbLofNamedX lofEnums;
											Int_t nofEnums = this->ExtractEnums(lofEnums, na);
											if (nofEnums > 0) {
												Int_t intBase = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Base"), 10);
												Int_t n1 = strtol(argDefault.Data(), NULL, intBase);
												TMrbNamedX * en = lofEnums.FindByIndex(n1);
												if (en) argDefault = en->GetName();
											}
										}
									}
								} else {
									macroTmpl.InitializeCode("%ARGN%");
								}
							}
							macroTmpl.Substitute("$macroType", macroType);
							macroTmpl.Substitute("$macroName", macroName);
							if (argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
								TString an = argName;
								an += "0";
								macroTmpl.Substitute("$argName", an);
							} else {
								macroTmpl.Substitute("$argName", argName);
							}
							if (argType.BeginsWith("Char_t")) argType.Prepend("const ");
							macroTmpl.Substitute("$argType", argType);
							if (argEntryTypeBits == TGMrbMacroArg::kGMrbMacroEntryYesNo) {
								if (argDefault.CompareTo("Yes") == 0)	macroTmpl.Substitute("$argDefault", "kTRUE");
								else									macroTmpl.Substitute("$argDefault", "kFALSE");
							} else if (argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
								TObjArray * lofTokens = argDefault.Tokenize(":");
								Int_t nofTokens = lofTokens->GetEntries();
								TString v;
								if (nofTokens >= 1) v = ((TObjString *) lofTokens->At(0))->GetString(); else v = argType.BeginsWith("Char_t") ? "" : "0";
								macroTmpl.Substitute("$argDefault", v);
								delete lofTokens;
							} else if (argType.BeginsWith("TObjArray")) {
								macroTmpl.Substitute("$argDefault", "NULL");
							} else {
								macroTmpl.Substitute("$argDefault", argDefault);
							}
							addLofValues = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "AddLofValues"), "No");
							macroTmpl.Substitute("$argSpace", argSpace);
							argDel = ((na == lastRealArg) && !addGui) ? ")" : ",";
							if ((argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) ||
								(argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitMulti) ||
								addLofValues.CompareTo("Yes") == 0) argDel = ",";
							macroTmpl.Substitute("$argDel", argDel);
							macroTmpl.WriteCode(macroStrm);
							if (argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitMulti) {
								Int_t nofEntryFields = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "NofEntryFields"), 1);
								TObjArray * lofTokens = argDefault.Tokenize(":");
								Int_t nofTokens = lofTokens->GetEntries();
								TString dv = argType.BeginsWith("Char_t") ? "" : "0";
								for (Int_t ef = 1; ef < nofEntryFields; ef++) {
									if (hasDefaults) {
										if (argType.BeginsWith("Char_t")) {
											macroTmpl.InitializeCode("%ARGNDC%");
										} else {
											macroTmpl.InitializeCode("%ARGND%");
										}
									} else {
										macroTmpl.InitializeCode("%ARGN%");
									}
									macroTmpl.Substitute("$macroType", macroType);
									macroTmpl.Substitute("$macroName", macroName);
									macroTmpl.Substitute("$argType", argType);
									TString an = argName;
									an += ef;
									macroTmpl.Substitute("$argName", an);
									TString v;
									if (nofTokens >= ef + 1) v = ((TObjString *) lofTokens->At(ef))->GetString(); else v = dv;
									macroTmpl.Substitute("$argDefault", v);
									argDel = ((na == lastRealArg) && !addGui && !(argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn)) ? ")" : ",";
									macroTmpl.Substitute("$argDel", argDel);
									argSpace.Resize(0); Int_t n = macroType.Length() + macroName.Length() + 2; for (; n--;) argSpace.Insert(0, " ");
									macroTmpl.Substitute("$argSpace", argSpace);
									macroTmpl.WriteCode(macroStrm);
								}
								delete lofTokens;
							}
							if (argEntryTypeBits & TGMrbMacroArg::kGMrbMacroEntryBitChkBtn) {
								macroTmpl.InitializeCode(hasDefaults ? "%ARGNCHKD%" : "%ARGNCHK%");
								macroTmpl.Substitute("$argName", argName);
								argDel = ((na == lastRealArg) && !addGui) ? ")" : ",";
								macroTmpl.Substitute("$argDel", argDel);
								argSpace.Resize(0); Int_t n = macroType.Length() + macroName.Length() + 2; for (; n--;) argSpace.Insert(0, " ");
								macroTmpl.Substitute("$argSpace", argSpace);
								macroTmpl.WriteCode(macroStrm);
							}
							if (addLofValues.CompareTo("Yes") == 0) {
								argSpace.Resize(0); Int_t n = macroType.Length() + macroName.Length() + 2; for (; n--;) argSpace.Insert(0, " ");
								macroTmpl.InitializeCode("%ARGNDC%");
								macroTmpl.Substitute("$argSpace", argSpace);
								macroTmpl.Substitute("$argType", "const Char_t *");
								macroTmpl.Substitute("$argName", argName + "Values");
								macroTmpl.Substitute("$argDefault",
										fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Values"), ""));
								argDel = ((na == lastRealArg) && !addGui) ? ")" : ",";
								macroTmpl.Substitute("$argDel", argDel);
								macroTmpl.WriteCode(macroStrm);
							}
							firstArg = kFALSE;
						}
						if (addGui) {
							argSpace.Resize(0); Int_t n = macroType.Length() + macroName.Length() + 2; for (; n--;) argSpace.Insert(0, " ");
							macroTmpl.InitializeCode("%ARGND%");
								macroTmpl.Substitute("$argSpace", argSpace);
								macroTmpl.Substitute("$argType", "TGMrbMacroFrame *");
								macroTmpl.Substitute("$argName", "GuiPtr");
								macroTmpl.Substitute("$argDefault", "NULL");
								macroTmpl.Substitute("$argDel", ")");
								macroTmpl.WriteCode(macroStrm);
						}
						macroTmpl.InitializeCode("%E%");
						macroTmpl.WriteCode(macroStrm);
						if (fileIsNew) {
							macroTmpl.InitializeCode("%NEW%");
							macroTmpl.WriteCode(macroStrm);
						}
					}
					break;
			}
		}
	}	
	macroStrm.close();
	gMrbLog->Out() << "Definitions saved to file " << newFile << endl;
	gMrbLog->Flush(this->ClassName(), "SaveMacro", setblue);
	if (!fileIsNew) {
		if (sameFile) {
			gMrbLog->Out() << "Original version saved to " << orgFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveMacro", setblue);
		}
		if (this->CopyUserCode(newFile.Data(), orgFile.Data())) {
			gMrbLog->Out() << "User code copied from " << orgFile << " to " << newFile << endl;
			gMrbLog->Flush(this->ClassName(), "SaveMacro", setblue);
		} else {
			gMrbLog->Err()	<< "Can't copy USER CODE from " << orgFile << " to " << newFile
							<< " - copy code MANUALLY please ..." << endl;
			gMrbLog->Flush(this->ClassName(), "SaveMacro");
		}
	}
	return(kTRUE);
}

Bool_t TGMrbMacroEdit::CopyUserCode(const Char_t * NewFile, const Char_t * OrgFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::CopyUserCode
// Purpose:        Copy user code
// Arguments:      Char_t * NewFile      -- new macro file
//                 Char_t * OrgFile      -- original (old) macro file
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Copies user's code from original to new file.
//                 !ATTENTION!
//                 Depends *strongly* on a given sequence of code parts:
//                     (1)  //+Exec ... //-Exec (taken from new file)
//                     (2)  [user code from original file]
//                     (3)  //+IncludeFiles ... //-IncludeFiles (taken from new file)
//                     (4)  [user code from original file]
//                     (5)  //+ArgNums ... //-ArgNums (taken from new file)
//                     (6)  [user code from original file]
//                     (7)  //+Enums ... //-Enums (taken from new file)
//                     (8)  [user code from original file]
//                     (9)  //+CallingSequence ... //-CallingSequence (taken from new file)
//                     (10) [user code from original file]
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString newnew = NewFile;
	newnew += ".new";

	ifstream tagStrm(OrgFile);
	if (!tagStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << OrgFile << " (input)" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kFALSE);
	}

	Bool_t hasTags = kFALSE;
	Bool_t hasExecTag = kFALSE;
	Bool_t hasIncludeTag = kFALSE;
	Bool_t hasArgNumTag = kFALSE;
	Bool_t hasEnumTag = kFALSE;
	Bool_t hasCallSeqTag = kFALSE;
	TString line;
	for (;;) {
		line.ReadLine(tagStrm, kFALSE);
		if (tagStrm.eof()) break;
		line = line.Strip(TString::kBoth);
		if (line.BeginsWith("//+Exec") == 0) { hasExecTag = kTRUE; }
		if (line.BeginsWith("//+IncludeFiles") == 0) { hasIncludeTag = kTRUE; hasTags = kTRUE; }
		if (line.BeginsWith("//+ArgNums") == 0) { hasArgNumTag = kTRUE; hasTags = kTRUE; }
		if (line.BeginsWith("//+Enums") == 0) { hasEnumTag = kTRUE; hasTags = kTRUE; }
		if (line.BeginsWith("//+CallingSequence") == 0) { hasCallSeqTag = kTRUE; hasTags = kTRUE; }
	}
	tagStrm.close();

	if (!hasExecTag) {
		gMrbLog->Err() << "No //+Exec tag in file " << OrgFile << " - format error?" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kFALSE);
	}

	if (!hasTags) {
		gMrbLog->Wrn() << "No tags in file " << OrgFile << " - nothing to copy" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kTRUE);
	}

	ifstream orgStrm(OrgFile);
	if (!orgStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << OrgFile << " (input)" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kFALSE);
	}

	ifstream newStrm(NewFile);
	if (!newStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << NewFile << " (input)" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kFALSE);
	}
	ofstream outStrm(newnew.Data());
	if (!outStrm.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << newnew << " (output)" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyUserCode");
		return(kFALSE);
	}

// (1)  //+Exec ... //-Exec (taken from new file)
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//-Exec", kTRUE)) return(kFALSE);
	if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//-Exec", kFALSE)) return(kFALSE);
// (2)  [user code from original file]
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//+IncludeFiles", kFALSE)) return(kFALSE);
	if (hasIncludeTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//+IncludeFiles", kTRUE)) return(kFALSE);
	}
// (3)  //+IncludeFiles ... //-IncludeFiles (taken from new file)
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//-IncludeFiles", kTRUE)) return(kFALSE);
	if (hasIncludeTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//-IncludeFiles", kFALSE)) return(kFALSE);
	}
// (4)  [user code from original file]
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//+ArgNums", kFALSE)) return(kFALSE);
	if (hasArgNumTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//+ArgNums", kTRUE)) return(kFALSE);
	}
// (5)  //+ArgNums ... //-ArgNums (taken from new file)
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//-ArgNums", kTRUE)) return(kFALSE);
	if (hasArgNumTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//-ArgNums", kFALSE)) return(kFALSE);
	}
// (6)  [user code from original file]
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//+Enums", kFALSE)) return(kFALSE);
	if (hasEnumTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//+Enums", kTRUE)) return(kFALSE);
	}
// (7)  //+Enums ... //-Enums (taken from new file)
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//-Enums", kTRUE)) return(kFALSE);
	if (hasEnumTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//-Enums", kFALSE)) return(kFALSE);
	}
// (8)  [user code from original file]
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//+CallingSequence", kFALSE)) return(kFALSE);
	if (hasCallSeqTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//+CallingSequence", kTRUE)) return(kFALSE);
	}
// (9)  //+CallingSequence ... //-CallingSequence (taken from new file)
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "//-CallingSequence", kTRUE)) return(kFALSE);
	if (hasCallSeqTag) {
		if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "//-CallingSequence", kFALSE)) return(kFALSE);
	}
// (10) [user code from original file]
	if (!CopyUserCode(outStrm, newnew.Data(), newStrm, NewFile, "", kFALSE)) return(kFALSE);
	if (!CopyUserCode(outStrm, newnew.Data(), orgStrm, OrgFile, "", kTRUE)) return(kFALSE);

	orgStrm.close();
	newStrm.close();
	outStrm.close();

	gSystem->Unlink(NewFile);
	gSystem->Rename(newnew.Data(), NewFile);

	return(kTRUE);
}

Bool_t TGMrbMacroEdit::CopyUserCode(ofstream & Out, const Char_t * /*OutFile*/, ifstream & In, const Char_t * InFile, const Char_t * Tag, Bool_t CopyFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::CopyUserCode
// Purpose:        Copy user code
// Arguments:      ofstream & Out      -- output stream
//                 Char_t * OutFile    -- name of output file
//                 ifstream & In       -- input stream
//                 Char_t * InFile     -- name of input file
//                 Char_t * Tag        -- stop copying on this tag
//                 Bool_t CopyFlag     -- copy code / skip code
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Copies user's code from original to new file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString line, stripLine;
	for (;;) {
		line.ReadLine(In, kFALSE);
		if (In.eof()) {
			if (*Tag == '\0') return(kTRUE);
			gMrbLog->Err() << InFile << " out of phase - \"" << Tag << "\" missing" << endl;
			gMrbLog->Flush(this->ClassName(), "CopyUserCode");
			In.close();
			Out.close();
			return(kFALSE);
		}
		stripLine = line.Strip(TString::kBoth);
		if (CopyFlag) Out << line << endl;
		if (*Tag != '\0' && stripLine.BeginsWith(Tag)) return(kTRUE);
	}
	return(kFALSE);
}

Int_t TGMrbMacroEdit::ExtractEnums(TMrbLofNamedX & LofEnums, Int_t ArgNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroEdit::ExtractEnums
// Purpose:        Extract enum names and values
// Arguments:      TMrbLofNamedX & LofEnums    -- where to store enum defs
//                 Int_t ArgNo                 -- argument number
// Results:        Int_t NofEnums              -- number of enums
// Exceptions:
// Description:    Extracts a list of enum defs from arg body.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbMacroArg thisArg;
	thisArg.SetIndex(ArgNo);

	Int_t nofEnums = 0;
	LofEnums.Delete();

	TString argEnv;
	TString argName = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Name"), "");
	thisArg.SetName(argName.Data());
	TString argType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Type"), "");
	TString argEntryType = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "EntryType"), "");
	
	if (argEntryType.CompareTo("Radio") != 0 && argEntryType.CompareTo("Check") != 0) return(0);
	if (argType.CompareTo("Int_t") != 0) return(0);

	TString argVals = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Values"), "");
	TString valName;
	TString val = "";
	Int_t from = 0;
	while (argVals.Tokenize(valName, from, ":")) {
		Int_t nsep = valName.Index("=", 0);
		if (nsep > 0) {
			val = valName(nsep + 1, 1000);
			valName.Resize(nsep);
		}
		Int_t ntip = valName.Index("|", 0);
		if (ntip > 0) valName.Resize(ntip);
		if (!val.IsNull()) {
			TString enumName;
			for (Int_t i = 0; i < valName.Length(); i++) {
				Char_t c = valName(i);
				if (isalnum(c)) {
					enumName += c;
				} else if (i < valName.Length() - 1) {
					valName(i+1,1).ToUpper();
				}
			}
			enumName(0,1).ToUpper();
			enumName.Prepend(argName.Data());
			enumName(0,1).ToUpper();
			enumName.Prepend("k");
			Int_t intBase = fCurrentEnv->GetValue(thisArg.GetResource(argEnv, "Base"), 10);
			LofEnums.AddNamedX(strtol(val.Data(), NULL, intBase), enumName.Data());
			nofEnums++;
		}
	}
	return (nofEnums);
}

const Char_t * TGMrbMacroArg::GetResource(TString & Resource, const Char_t * ResourceName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbMacroArg::GetResource
// Purpose:        Build resource name
// Arguments:      TString & Resource      -- where to put results
//                 Char_t * ResourceName   -- resource name (trailing part)
// Results:        Char_t * Resource       -- same as arg #1
// Exceptions:
// Description:    Builds resource names "ArgN.ResourceName"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString arg;

	arg = "Arg";
	arg += this->GetIndex();
	arg += ".";
	arg += ResourceName;
	Resource = arg;
	return(Resource.Data());
}

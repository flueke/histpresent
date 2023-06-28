namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TVirtualX.h>
#include "TRootHelpDialog.h"
#include "TRootCanvas.h"
#include <TList.h>
#include <TFile.h>
#include <TEnv.h>
#include <TKey.h>
#include <TColor.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRegexp.h>

#include <TGResourcePool.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGColorSelect.h>

#include "TGToolTip.h"
#include "TGButton.h"
#include "TGLabel.h"

#include "TGMrbValuesAndText.h"
#include "TMrbHelpBrowser.h"
//#include "TGedAlignSelect.h"
//#include "SetColor.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// General purpose dialog widget to request input of a text string       //
// and any number of ints, floats, colors etc. arranged in a table       //
//                                                                       //
// The string input part implements a history and text completion        //
// facility, for this to work a file containing the history and          //
// a TList containing TObjStrings must  
//                                                                       //
// The meaning of the requested values are stored in the first 11        //
// characters terminated by _, +, - of the row labels as follows:        //
// The termination char _, +, - determine if the dialog entry starts     //
// on new row: _: start new row,                                         //
//              +: continue on same row, equalize  distances             //
//              -: continue on same row, pack as close as possible       //
// The remainder of the text is the label of the button        			 //
//                                                                       //
// StringValue : Text string	(this has no history/text completion 	    //
// PlainShtVal : Short_t																 //
// PlainIntVal : Int_t  																 //
// DoubleValue : Double_t																 //
// Float_Value : Float_t																 //
// Min,max values may be given after the label field separated by ;      //
// (semicolon)                                                           //
// ArrowSelect : a arrow selection (Style_t) 									 //
// LineSSelect : Line Style  (Style_t) 											 //
// CfontSelect : Character Font (Font_t)											 //
// AlignSelect : Text alignment (Style_t) 										 //
// Fill_Select : Fill style															 //
// Mark_Select : Marker style 														 //
// ColorSelect : Color_t																 //
// ComboSelect : a Combo box															 //
// RadioButton : radio button 														 //
// CheckButton : check button 														 //
// DoubleV+CbF : a double + a CheckButton label "Fix"							 //
// DoubleV+CbU : a double + a CheckButton label "Use"							 //
// Exec_Button : only emit signal button clicked								 //
// FileRequest : file name (invokes file dialog)								 //
// FileContReq : invoke file dialog and present listbox with content		 //
//             : arg: filename | classname | object name                 //
// Pressing one of the above buttons emits a signal which is connected   //
// to a slot:  CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)       //
// which must be implemented in the "calling_class"                     //
//                                                                       //
// CommandButt : Command button implemented as follows:						 //
//               assume: CommandButt_DrawArrow()								 //
//               when pressed will emit:											 //
//               ("Clicked()", Char_t * cname,  								 //
//                 TObject * calling_class,"DrawArrow()")					 //
// Note: A CommandButt can be connected to any slot (method) whereas     //
//       an Exec_Button is connected to a fixed slot                     //
//                                                                       //
// 																							 //
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const char *Prompt: the Prompt string                                //
//  TString * text:     the value of the string                          //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //
//  Int_t   win_width : Width (pixels) of the dialog  widget             //
//                                                                       //
//  optional arguments:                                                  //
//                                                                       //
// TGWindow *win        pointer to parent window								 //
// Char_t * History     a file containing lines from which a selection   //
//                      may be made, a TGListBox is used in this case	 //
//                      This is used to store newly type text lines not  //
//                      yet in the list											 //
// TList * Complist:    list containing TObjStrings used for				 //
//                      text completion (tab)									 //
// TList * rowlabs:     List of TObjStrings of row labels					 //
// void ** valpointers  array with pointers to the requested values  	 //
// TArrayI * Flags      pointer to integer array of flags, a checkbutton //
//                      is added to each value  								 //
// Char_t * flaglabel   label of the (opotional)flags / checkbuttons 	 //
//                      column														 //
// Char_t * helptext    helptext displayed when pressing (optional)  	 //
//                      Help button 												 //
// TObject * class      pointer to the calling class used in the emitted //
//                      signal triggered by a command button				 //
// Char_t * classname   name of the calling class  							 //
//                                                                       //
//                                                                       //
// This class can be accessed via the following utility function:        //
//                                                                       //
//                                                                       //
// Bool_t GetStringExt(const char *Prompt, TString  *text , 				 //
// 							  Int_t win_width, TGWindow *Win,   				 //
// 							  const char *History, TList * Complist, 		 //
// 							  TList * rowlabs, void **val_pointers,			 //
// 							  TArrayI * Flags, const char * Flagslabel,		 //
// 							  const char *helptext, TObject * calling_class, //
// 							  const char * cname)									 //
//  																							 //
//  																							 //
///////////////////////////////////////////////////////////////////////////
//
//extern Ssiz_t IndexOfLastSpecial(TString &str);
//extern Ssiz_t IndexOfLast(TString &str, char &c);
//extern Int_t Matches(TList * list, const char * s, Int_t * matchlength);
//________________________________________________________________________________________

Ssiz_t IndexOfLastSpecial(TString &str)
{
	// return index  of not (letter, number or _ ) in str searching backwards
	// Skip ::
	TRegexp spc("[^a-zA-Z0-9_]", kFALSE);
	if (str.Index(spc) < 0) return -1;    // no occurence
	Int_t len = str.Length();
	Int_t ind = len - 1;
	Int_t indspc;
	TString onec;
	const char *sp = str.Data();
	char c = {':'};
	while (ind >= 0) {
//      cout << ind << " " << str.Index(spc, ind) << endl;
		onec = str(ind, 1);
		indspc = onec.Index(spc);
		if (indspc == 0) {
			if (sp[ind] != c) return ind;
			if (sp[ind] == c && (sp[ind -1] != c || ind == 0)) {
				if (ind == 0)
					cout << "Warning: Expression begins with ':' " << endl;
				return ind;
			}
			ind--;
		}
		ind--;
	}
	return -1;
}
//________________________________________________________________________________________

Ssiz_t IndexOfLast(TString &str, char &c)
{
	// return index of detached char c in str searching backwards
	if (str.Index(c) < 0) return -1;    // no occurence
	Int_t len = str.Length();
	const char *sp = str.Data();
//   cout << sp << endl;
	Int_t ind = len - 1;
	while (ind > 0) {
		if (sp[ind] == c && sp[ind -1] != c) return ind;
		ind--;
		if (sp[ind+1] == c)ind--;
	}
	return -1;
}
//________________________________________________________________________________________

Int_t Matches(TList * list, const char * s, Int_t * matchlength)
{
	Int_t pos = -1;
	Bool_t unique = kTRUE;
	TObjString * obs;
	Int_t lmax = 1000000;
	for (Int_t i = 0; i < list->GetSize(); i++) {
		obs = (TObjString *)list->At(i);
		TString var = obs->GetString();
		if (var.Index(s, 0) == 0) {
			cout << "Input: " << s << " matches: " << var << endl;
			if (pos >=0) unique = kFALSE;
			pos = i;
			if (var.Length() < lmax) lmax = var.Length();
			*matchlength = var.Length();
		}
	}
	if (unique) {
		return pos;
	} else {
		cout << "Not unique, find  longest match: lmax = " << lmax << endl;
		Int_t lm = 0;
		for (Int_t l = 0; l < lmax; l++) {
			Bool_t fm = kTRUE;
			char cm ={' '};
			for (Int_t i = 0; i < list->GetSize(); i++) {
				obs = (TObjString *)list->At(i);
				TString var = obs->GetString();
				if (var.Index(s, 0) != 0) continue;
				if (fm) {
					cm = var[l];
					fm = kFALSE;
				} else {
					if (var[l] != cm) {
						*matchlength = lm;
//                  cout << "pso, lm: " << pos << " " << lm << endl;
						return pos;
					}
				}
			}
			lm++;
		}
	}
	*matchlength = lmax;
	return pos;
}
;

//#include "TGResourcePool.h"
//#include "TGPicture.h"
#include "TGToolTip.h"
//#include "TGButton.h"
//#include "Riostream.h"

ClassImp(TGedAlignSelect)
ClassImp(TGedAlignPopup)

EWidgetMessageTypes kC_ALIGNSEL = (EWidgetMessageTypes)1001,
						  kALI_SELCHANGED = (EWidgetMessageTypes)2;


struct alignDescription_t {
//   const char* filename;
	const char* pictname;
	const char* name;
	int number;
	int seqnr;
};

static alignDescription_t  Aligns[] = {
/*
	{"align13.gif","13 Top, Left",      13},
	{"align23.gif","23 Top, Middle",    23},
	{"align33.gif","33 Top, Right",     33},
	{"align12.gif","12 Middle, Left",   12},
	{"align22.gif","22 Middle, Middle", 22},
	{"align32.gif","32 Middle, Right",  32},
	{"align11.gif","11 Bottom, Left",   11},
	{"align21.gif","21 Bottom, Middle", 21},
	{"align31.gif","31 Bottom, Right",  31},
*/
	{"align13","13 Top, Left",      13, 0},
	{"align23","23 Top, Middle",    23, 1},
	{"align33","33 Top, Right",     33, 2},
	{"align12","12 Middle, Left",   12, 3},
	{"align22","22 Middle, Middle", 22, 4},
	{"align32","32 Middle, Right",  32, 5},
	{"align11","11 Bottom, Left",   11, 6},
	{"align21","21 Bottom, Middle", 21, 7},
	{"align31","31 Bottom, Right",  31, 8},
	{0, 0, 0, 0}
};
	/* XPM */
	static const char *align11[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align12[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align13[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%      %%%%%%%%%%%%%",
	"%%    %%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align21[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align22[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align23[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%      %%%%%%%",
	"%%%%%%%%    %%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align31[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align32[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char *align33[] = {
	/* columns rows colors chars-per-pixel */
	"20 20 2 1",
	"  c black",
	"% c #FDFDFE",
	/* pixels */
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%      %",
	"%%%%%%%%%%%%%%    %%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%"
	};
	static const char **pixarr[9] = {
		&align13[0],
		&align23[0],
		&align33[0],
		&align12[0],
		&align22[0],
		&align32[0],
		&align11[0],
		&align21[0],
		&align31[0]
	};


static alignDescription_t* GetAlignByNumber(int number)
{
	for (int i = 0; Aligns[i].number != 0; i++) {
		if (Aligns[i].number == number)
			 return &Aligns[i];
	}
	return 0;
}

static const char *filetypes[] = {
					"All files",     "*",
					"ROOTfile",   "*.root",
					"TextFile",   "*.[t|d|a][x|a|s][t|t|c]*",
					"Imagefiles", "*.[p|j|g][n|p|i][g|g|f]", 
					NULL,				NULL};

//______________________________________________________________________________
TGedAlignPopup::TGedAlignPopup(const TGWindow *p, const TGWindow *m, Style_t alignStyle)
	: TGedPopup(p, m, 30, 30, kDoubleBorder | kRaisedFrame | kOwnBackground,
					GetDefaultFrameBackground())
{
	TGButton *b;
	fCurrentStyle = alignStyle;

	Pixel_t white;
	gClient->GetColorByName("white", white); // white background
	SetBackgroundColor(white);

	SetLayoutManager(new TGTileLayout(this, 1));
	for (int i = 0; Aligns[i].number != 0; i++) {
		const TGPicture * pict = gClient->GetPicturePool()->GetPicture(Aligns[i].pictname, (char**)pixarr[i]);
		AddFrame(b = new TGPictureButton(this, pict,
					Aligns[i].number, TGButton::GetDefaultGC()(), kSunkenFrame),
					new TGLayoutHints(kLHintsLeft, 14, 14, 14, 14));
		b->SetToolTipText(Aligns[i].name);
//      cout << Aligns[i].filename << endl;
	}

	Resize(76, 76);
	MapSubwindows();
}

//______________________________________________________________________________
TGedAlignPopup::~TGedAlignPopup()
{
	Cleanup();
}

//______________________________________________________________________________
Bool_t TGedAlignPopup::ProcessMessage(Long_t msg, Long_t parm1, Long_t /*parm2*/)
{
//   cout << "TGedAlignPopup::ProcessMessage " << GET_MSG(msg) << endl;
	if (GET_MSG(msg) == kC_COMMAND && GET_SUBMSG(msg) == kCM_BUTTON) {
		SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED), 0, parm1);
		EndPopup();
	}
	return kTRUE;
}

//______________________________________________________________________________
TGedAlignSelect::TGedAlignSelect(const TGWindow *p, Style_t alignStyle, Int_t id)
	: TGedSelect(p, id)
{
//   cout << "TGedAlignSelect::ctor id = " << id << endl;
	fPicture = 0;
	SetPopup(new TGedAlignPopup(gClient->GetDefaultRoot(), this, alignStyle));
	SetAlignStyle(alignStyle);

}

//_____________________________________________________________________________
Bool_t TGedAlignSelect::ProcessMessage(Long_t msg, Long_t /*parm1*/, Long_t parm2)
{
//   std::cout << "TGedAlignSelect::ProcessMessage " <<fWidgetId << std::endl;
	if (GET_MSG(msg) == kC_ALIGNSEL && GET_SUBMSG(msg) == kALI_SELCHANGED) {
		SetAlignStyle(parm2);
		SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED),
						fWidgetId, parm2);
	}
	return kTRUE;
}

//_____________________________________________________________________________

void TGedAlignSelect::DoRedraw()
{
	TGedSelect::DoRedraw();

	Int_t  x, y;
	UInt_t w, h;

	if (IsEnabled()) {
		// pattern rectangle
//      cout << "TGedAlignSelect::DoRedraw() " << fPicture<< endl;
		x = fBorderWidth + 2;
		y = fBorderWidth + 2;  // 1;
		h = fHeight - (fBorderWidth * 2) - 4;  // -3;  // 14
		w = h;
		if (fState == kButtonDown) {
			++x; ++y;
		}
		gVirtualX->DrawRectangle(fId, GetShadowGC()(), x, y, w - 1, h - 1);

		if (fPicture != 0) fPicture->Draw(fId, fDrawGC->GetGC(), x + 1, y + 1);
	} else { // sunken rectangle
		x = fBorderWidth + 2;
		y = fBorderWidth + 2;  // 1;
		w = 42;
		h = fHeight - (fBorderWidth * 2) - 4;  // 3;
		Draw3dRectangle(kSunkenFrame, x, y, w, h);
	}
}

//_____________________________________________________________________________
void TGedAlignSelect::SetAlignStyle(Style_t alignStyle)
{
	// Set align.

	fAlignStyle = alignStyle;
//   gClient->NeedRedraw(this);

	if (fPicture) {
		gClient->FreePicture(fPicture);
		fPicture = 0;
	}

	alignDescription_t *md = GetAlignByNumber(fAlignStyle);

//   cout << "TGedAlignSelect::SetAlignStyle " << md->filename << endl;
	if (md) {
//   cout << "TGedAlignSelect::SetAlignStyle " << fAlignStyle << " " << md->pictname << endl;
//      fPicture = gClient->GetPicture(md->filename);
		fPicture = gClient->GetPicturePool()->GetPicture(md->pictname, (char**)pixarr[md->seqnr]);
//      fPicture = gClient->GetPicture(md->filename);
		SetToolTipText(md->name);
	}
//   std::cout << "AlignSelected(fAlignStyle); " << fAlignStyle << endl;
	gClient->NeedRedraw(this);
	AlignSelected(fAlignStyle);
}

//______________________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void TGedAlignSelect::SavePrimitive(ostream &out, Option_t *)
#else
void TGedAlignSelect::SavePrimitive(ofstream &out, Option_t *)
#endif
{
	// Save the pattern select widget as a C++ statement(s) on output stream out

	out <<"   TGedAlignSelect *";
	out << GetName() << " = new TGedAlignSelect(" << fParent->GetName()
		 << "," << fAlignStyle << "," << WidgetId() << ");" << endl;
}
//________________________________________________________________________________________

ClassImp(TTLabel)

TTLabel::TTLabel(const TGWindow *p, TGString *text,
           GContext_t norm,
           FontStruct_t font,
           UInt_t options,
           Pixel_t back):
           TGLabel(p, text, norm, font, options, back)
{
	fTip = NULL;
	AddInput(kPointerMotionMask | kEnterWindowMask | kLeaveWindowMask);
}

//______________________________________________________________________________
Bool_t TTLabel::HandleButton(Event_t *event)
{
   // Handle mouse button event.
//printf("HandleButton %d\n", event->fType);
   
   if (fTip) fTip->Hide();

//   DoRedraw();
   return kTRUE;
}


//______________________________________________________________________________
Bool_t TTLabel::HandleCrossing(Event_t *event)
{
   // Handle mouse crossing event.
//printf("HandleCrossing %d\n", event->fType);

   if (fTip) {
      if (event->fType == kEnterNotify)
         fTip->Reset();
      else
         fTip->Hide();
   }
   
//   DoRedraw();
   return kTRUE;
}

//______________________________________________________________________________

void TTLabel::SetToolTipText(const char *text, Long_t delayms)
{
   // Set tool tip text associated with this button. The delay is in
   // milliseconds (minimum 250). To remove tool tip call method with
   // text = 0.
//   DoRedraw();

   if (fTip) {
      delete fTip;
      fTip = 0;
   }

   if (text && strlen(text)) {
      fTip = new TGToolTip(gClient->GetRoot(), this, text, delayms);
 //     printf("SetToolTipText: %s\n", text);
   }
}

//________________________________________________________________________________________

enum buttonId {kIdOk = 101, kIdCancel = 102, kIdHelp = 103, kIdClearHist = 104,
					kIdText = 201, kIdFileName = 202, kIdTextValue = 301,
					kIdTextSelect, kIdListBoxReq = 401,
					kIdFileDialog = 4, kIdFileDialogCont = 5, kIdFontS = 6, kIdCommand = 7,
					kIdExec, kIdLineS, kIdArrowS, kIdComboS,kIdAlignS, kIdMarkS, kIdFillS, kIdColorS};
enum {
	kIsAEditorPage  = BIT(23)
};

ClassImp(TGMrbValuesAndText)

	const Int_t MAXROWS = 500;

//________________________________________________________________________________________

TGMrbValuesAndText::TGMrbValuesAndText(const char *Prompt, TString * text,
						Int_t * ok, Int_t win_width, const TGWindow *Win,
						const char * History, TList * complist,
						TList * RowLabels, void **val_pointers,
						TArrayI * Flags, const char * Flagslabel,
						const char *helptext, TObject * calling_class,
						const char * cname, Int_t id):
								 TGTransientFrame(gClient->GetRoot(), Win, 10, 10)
{
	// Create  input dialog.
	SetCleanup(kDeepCleanup);

	Pixel_t red, blue, lblue, brown, grey, lgrey, wheat;
	fClient->GetColorByName("firebrick", brown);
	fClient->GetColorByName("red", red);
	fClient->GetColorByName("blue", blue);
	fClient->GetColorByName("LightSteelBlue1", lblue);
//   fBlueTextGC.SetForeground(blue);
	fClient->GetColorByName("grey", grey);
//   fLightGreyTextGC.SetForeground(lgrey);
	fClient->GetColorByName("grey90", lgrey);
//   fLightGreyTextGC.SetForeground(lgrey);
	fClient->GetColorByName("wheat", wheat);

//   TGGC myGC = *fClient->GetResourcePool()->GetFrameGC();
//   TGFont *myfont = fClient->GetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
//   if (myfont) myGC.SetFont(myfont->GetFontHandle());
	fWidgetId = id;
	fButtonId = 0;
 //  fButtonList = new void [Rowlabels->GetSize()];
	fWindowWidth = win_width;
//   fWidgets = new TList;
	fFlagButtons = new TList;
//   fFinis = 0;
	SetBit(kMustCleanup);
	fStartDir = gSystem->Getenv("PWD");
	fMustRestoreDir = kFALSE;
	fEmitClose = kTRUE;
	fListBox = NULL;
	fLabels = new TList;
	fValPointers = val_pointers;
	fFlags = Flags;
	fText = text;
	fPrompt = Prompt;
	fHelpText = helptext;
	fCancelButton = NULL;
	fEntries = NULL;
	const TGWindow * main = gClient->GetRoot();
	if(Win != 0) {
		fMyWindow = Win;
		TRootCanvas * cimp = (TRootCanvas*)Win;
		fCallingCanvas = cimp->Canvas();

	} else {
		fMyWindow = main;
		fCallingCanvas = NULL;
	}
	if ( gDebug > 0 && Win) {
		cout << "ctor TGMrbValuesAndText: Id, fId(Main) " << this->GetId() 
		<< " " << Win->GetId() << endl
		<< "TGTransientFrame * trf=(TGTransientFrame*)"<< this << endl;
		cout << "fLabels " << fLabels<< endl;
	}
	fCompList = complist;
	fCloseFlag = *ok;
	Bool_t mustwait = kFALSE;
	if (*ok <= -2)
		mustwait = kTRUE;
	fReturn = ok;
	*ok = -1;    // if closed by cancel
	TString CancelCmd;
	Bool_t has_commands = kFALSE;
	fCallClose = kTRUE;
	if (calling_class != NULL && win_width > 0) {
		this->Connect("CloseDown(Int_t)",cname, calling_class, "CloseDown(Int_t)");
		this->Connect("CRButtonPressed(Int_t, Int_t, TObject*)", cname, calling_class, "CRButtonPressed(Int_t, Int_t, TObject*)");
	}
	if (win_width < 0) {
		fCallClose = kFALSE;
		win_width *= -1;
	}

//   cout << "win_width " <<win_width << endl;

	TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX|kLHintsCenterY , 2, 2, 2, 2);
	TGLayoutHints * lol = new TGLayoutHints( kLHintsLeft|kLHintsCenterY , 2, 2, 2, 2);
	TGLayoutHints * lor = new TGLayoutHints( kLHintsRight|kLHintsExpandX, 2, 2, 2, 2);
	TGLayoutHints * lorne = new TGLayoutHints(kLHintsRight, 2, 2, 2, 2);
	TGLayoutHints * lone = new TGLayoutHints(0, 2, 2, 2, 2);
	TGLayoutHints * locy = new TGLayoutHints( kLHintsCenterY , 2, 2, 2, 2);
	TGLayoutHints * locx = new TGLayoutHints( kLHintsCenterX , 2, 2, 2, 2);
	TGLayoutHints * l1 = new TGLayoutHints(kLHintsExpandX|kLHintsLeft | kLHintsCenterY, 2, 2, 2, 2);
	TGLayoutHints * l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2);
	// find label font and use as length norm fixed font courier
	TString label_fontname(gEnv->GetValue("Gui.MenuHiFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
	TGFont *label_font = gClient->GetFont(label_fontname);
	TGFont *ref_font = NULL;
	FontMetrics_t font_m;;
	label_font->GetFontMetrics(&font_m);
	if (!font_m.fFixed) {
		TObjArray * oo = label_fontname.Tokenize("-");
		TString sh = ((TObjString*)oo->At(6))->String();
		if ( sh.IsDec() ) {
			TString ref_fontname("-adobe-courier-bold-r-*-*-");
			ref_fontname += sh;
			ref_fontname += "-*-*-*-*-*-iso8859-1";
			ref_font = gClient->GetFont(ref_fontname);
	//		cout << "ref_font " << ref_fontname << endl;
		}
	}
//	cout << "label_font " << label_fontname << " sh " << sh << endl;
	
//  table part

	TGCompositeFrame *hframe = NULL , *hframe1 = NULL, *hButtonFrame = NULL;
	if (RowLabels) {
		TTLabel * label = NULL;
		TGTextEntry * tentry;
		TGNumberEntry * tnentry;
		TGButton * cbutton;
		TGTextBuffer * tbuf;
//      TString s;
		TString l;
		TString nl;
		fNrows = RowLabels->GetSize();
		if (fFlags && Flagslabel) {
			hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);
//         fWidgets->AddFirst(hframe);
			hframe1 = new TGCompositeFrame(hframe, win_width*5/6, 20, kFixedWidth);
//         fWidgets->AddFirst(hframe1);
			hframe->AddFrame(hframe1, l2);
			hframe1 = new TGCompositeFrame(hframe, win_width*1/6, 20, kFixedWidth);
//         fWidgets->AddFirst(hframe1);
			label = new TTLabel(hframe1, new TGString(Flagslabel));
//			fWidgets->AddFirst(label);
			hframe1->AddFrame(label, lor);
			hframe->AddFrame(hframe1, lor);

			this->AddFrame(hframe, lor);
		}
		// find longest label in pixel for sel font
		Int_t longest_label = 0;
		if (!font_m.fFixed) {
			for (Int_t indrows= 0; indrows < fNrows; indrows++) {
				l = ((TObjString *)RowLabels->At(indrows))->String();
				// chop off args or tool tip
				Int_t isem = l.Index(";");
				if (isem > 0)
					l.Resize(isem);
				isem = l.Index("&");
				if (isem > 0)
					l.Resize(isem);
				// chop off command part
				l.Remove(0,12);
				Int_t ll = label_font->TextWidth(l);
				if (ll > longest_label)
					longest_label = ll;
			}
		}
		fEntries = new TList();
		hframe = NULL;
		fClassName = new TString*[MAXROWS];
		fFileType = new Int_t[MAXROWS];
		fFileDialogContTextEntry = new TGTextEntry*[MAXROWS];
		for (Int_t j = 0; j < MAXROWS; j++) {
			fFileType[j] = 0;
			fClassName[j] = (TString*)NULL;
			fFileDialogContTextEntry[j] = NULL;
		}
		Int_t i = 0;  // index on ValPointers
//      cout << "fNrows " << fNrows << endl;
		for (Int_t indrows= 0; indrows < fNrows; indrows++) {
			if ( i >= MAXROWS ) {
				cout << "Too many entries: " << i << endl;
				break;
			}
			fLabels->Add(RowLabels->At(indrows));
			l = ((TObjString *)RowLabels->At(indrows))->String();
//			cout << "i: " << i << " indrows: " << indrows 
//			<< "text: " << l << endl;
//       in multi column row save space by avoiding expandx
			TGLayoutHints * loc = l1;
			if (l[11] == '-') loc = locy;
			if (indrows < fNrows-1) {
				nl =((TObjString *)RowLabels->At(indrows+1))->String();
				if (nl[11] == '-') loc = locy;
			}
//       continue in same row if +
			if (l[11] == '_' && hframe != NULL) {
				 this->AddFrame(hframe, lo1);
				 hframe = NULL;
			}
			if (hframe == NULL) {
				hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);
//            fWidgets->AddFirst(hframe);
			}
			Double_t lowlim = 0;
			Double_t  uplim = 0;
			Bool_t has_lowlim = kFALSE;
			Bool_t has_uplim = kFALSE;
			TGNumberFormat::ELimit     limsw = TGNumberFormat::kNELNoLimits;
			TGNumberFormat::EAttribute attr  = TGNumberFormat::kNEAAnyNumber;
			TGNumberFormat::EStyle     style = TGNumberFormat::kNESReal;
			
			if (!l.BeginsWith("CommandButt") && !l.BeginsWith("Exec_Button")) {
// label
				TString lab(l);
				TString tt_text;
				if (lab.Length() > 11) {
					lab.Remove(0,12);
					// care about tool tip
					Int_t tt_ind = lab.Index("&");
					if (tt_ind > 1) {
						tt_text = lab(tt_ind+1, lab.Length());
						lab.Resize(tt_ind);
					}
					
// look for min / max for number entries
					TObjArray * tokens;
					tokens = lab.Tokenize(";");
					Int_t nt = tokens->GetEntries();
					if (nt > 1) {
						Int_t ft = lab.Index(";");
						lab.Resize(ft);
						if (!l.BeginsWith("ComboSelect")) {
							TString s = ((TObjString*)tokens->At(1))->String();
							if (s.Length() > 0) {
								lowlim = s.Atof();
								has_lowlim = kTRUE;
							}
							if (nt > 2) {
								s = ((TObjString*)tokens->At(2))->String();
								if (s.Length() > 0) {
									uplim = s.Atof();
									has_uplim = kTRUE;
								}
							}
						}
					}
					delete tokens;
					if ( has_lowlim && has_uplim ) {
						limsw = TGNumberFormat::kNELLimitMinMax;
						if ( uplim - lowlim <= 10 )
							style = TGNumberFormat::kNESRealOne;
						if ( uplim - lowlim <= 2 )
							style = TGNumberFormat::kNESRealTwo;
					} else if ( has_lowlim ) {
						limsw = TGNumberFormat::kNELLimitMin;
						if ( lowlim >= 0 )
							attr = TGNumberFormat::kNEANonNegative;
					} else if  ( has_uplim ) {
						limsw = TGNumberFormat::kNELLimitMax;
					}
					if (l.BeginsWith("Comment")) loc = l1;
					if (l.BeginsWith("CommentRigh")) loc = lor;
					hButtonFrame = new TGCompositeFrame(hframe, win_width, 20, kHorizontalFrame);
					if ( lab.Length() > 0 ) {
						// adjust length assuming fixed font
//						lolab->SetPadLeft(2);
//						lolab->SetPadRight(2);
						if (label_font && ref_font) {
							Int_t lleng = label_font->TextWidth(lab);
							Int_t rleng = ref_font->TextWidth(lab);
							if (rleng > longest_label) rleng = longest_label;
//							cout << lab << " "  << lleng << " " << rleng << endl;
							if (rleng > lleng) {
								Int_t space_leng = label_font->TextWidth(" ");
								Int_t extra = (rleng-lleng)/space_leng;
								if (extra > 0) {
									for (Int_t is = 0; is<extra; is++) {
										if (lab.BeginsWith(" "))
											lab.Prepend(" ");
										else 
											lab.Append(" ");
									}
								}
							}
						}
						label = new TTLabel(hButtonFrame, new TGString(lab));
						label->SetTextFont(label_fontname);
						hButtonFrame->AddFrame(label,lol);	
						if (tt_text.Length() > 0) {
							label->SetToolTipText(tt_text.Data(),250);
						}
						label->ChangeBackground(lgrey);
						if (l.BeginsWith("Comment")) fEntries->Add(label);
					}
				}
			}

			if (l.BeginsWith("Comment")) {
				if (label)
					label->ChangeBackground(wheat);
				hframe->AddFrame(hButtonFrame, locx);
			} else if (l.BeginsWith("CommandButt")) {
				TString *sr = (TString*)fValPointers[i];
//				cout << "i, sr " << i << " " << sr << endl;
//				cout << "i, sr " << i << " " << sr->Data() << endl;
				TString lab(l);
				lab.Remove(0,12);
				cbutton = new TGTextButton(hframe, new TGHotString(lab.Data()), i + 1000 * kIdCommand);
				cbutton->Connect("Clicked()", cname, calling_class,sr->Data());
				cbutton->SetForegroundColor(blue);
				((TGTextButton*)cbutton)->SetFont(gEnv->GetValue("Gui.MenuHiFont",
				"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
//            ((TGTextButton*)cbutton)->SetFont("-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
				cbutton->SetBackgroundColor(lblue);
				cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
				
				fEntries->Add(cbutton);
				hframe->AddFrame(cbutton, loc);
				has_commands = kTRUE;
				cbutton->Associate(this);

			} else if (l.BeginsWith("Exec_Button")) {
				TString lab(l);
				lab.Remove(0,12);
				cbutton = new TGTextButton(hframe, new TGHotString(lab.Data()), i + 1000*kIdExec);
				cbutton->SetForegroundColor(blue);
				((TGTextButton*)cbutton)->SetFont(gEnv->GetValue("Gui.MenuHiFont",
				"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
 //           ((TGTextButton*)cbutton)->SetFont("-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
				cbutton->SetBackgroundColor(lblue);
				cbutton->Resize(win_width / 6, cbutton->GetDefaultHeight());
				fEntries->Add(cbutton);
				cbutton->Associate(this);
				hframe->AddFrame(cbutton, lor);

			} else if (l.BeginsWith("CheckButton")) {
				cbutton = new TGCheckButton(hButtonFrame, new TGHotString(""), i + 1000);
				Int_t state = *(Int_t*)fValPointers[i];
				if (state != 0) cbutton->SetState(kButtonDown);
				else                    cbutton->SetState(kButtonUp);
				cbutton->Resize(win_width / 8, cbutton->GetDefaultHeight());
				fEntries->Add(cbutton);
				cbutton->Associate(this);
				hButtonFrame->AddFrame(cbutton, lor);
				hframe->AddFrame(hButtonFrame, loc);
			} else if (l.BeginsWith("RadioButton")) {
				cbutton = new TGRadioButton(hButtonFrame, new TGHotString(""), i + 1000);
				Int_t state = *(Int_t*)fValPointers[i];
				if (state != 0) cbutton->SetState(kButtonDown);
				else                    cbutton->SetState(kButtonUp);
				cbutton->Resize(win_width / 6, cbutton->GetDefaultHeight());
				fEntries->Add(cbutton);
				cbutton->Associate(this);
				hButtonFrame->AddFrame(cbutton, lor);
				hframe->AddFrame(hButtonFrame, loc);
		 } else if (l.BeginsWith("ColorSelect")) {
				Int_t col = GetColorPixelByInd(*(Color_t*)fValPointers[i]);
				cbutton = new TGColorSelect(hButtonFrame, col, i + 1000*kIdColorS);
				fEntries->Add(cbutton);
				cbutton->Associate(this);
				hButtonFrame->AddFrame(cbutton, lor);
				hframe->AddFrame(hButtonFrame, loc);
				fLastColorSelect = i;          // TGColor doesnt set Id correctly
			} else if (l.BeginsWith("Mark_Select")) {
				fMarker = *(Style_t*)fValPointers[i];
				fMarkerSelect = new TGedMarkerSelect(hButtonFrame, fMarker, i + 1000*kIdMarkS);
				fMarkerSelect->Associate(this);
				fEntries->Add(fMarkerSelect);
				hButtonFrame->AddFrame(fMarkerSelect, lor);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("Fill_Select")) {
				fPattern = *(Style_t*)fValPointers[i];
				fPatternSelect = new TGedPatternSelect(hButtonFrame, fPattern,i +  1000*kIdFillS);
				fPatternSelect->Associate(this);
				fEntries->Add(fPatternSelect);
				hButtonFrame->AddFrame(fPatternSelect, lor);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("AlignSelect")) {
				fAlign = *(Short_t*)fValPointers[i];
				fAlignSelect = new TGedAlignSelect(hButtonFrame, fAlign, i + 1000*kIdAlignS);
				fAlignSelect->Associate(this);
				fEntries->Add(fAlignSelect);
				hButtonFrame->AddFrame(fAlignSelect, lor);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("CfontSelect")) {
				fFont = *(Font_t*)fValPointers[i];
				fFontComboBox = new TGFontTypeComboBox(hButtonFrame, i + 1000*kIdFontS);
				fFontComboBox->Resize(fFontComboBox->GetDefaultWidth(), 20);
				Font_t tf = fFont;
				if (tf > 12) tf /= 10;
				if (tf < 1 || tf > 13) tf = 6; // helvetica
				fFontComboBox->Select(tf);
				fEntries->Add(fFontComboBox);
				fFontComboBox->Associate(this);
				hButtonFrame->AddFrame(fFontComboBox, loc);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("LineSSelect")) {
				fLineStyle = *(Style_t*)fValPointers[i];
				fLineStyleComboBox = new TGLineStyleComboBox(hButtonFrame, i + 1000*kIdLineS);
				fLineStyleComboBox->Resize(fLineStyleComboBox->GetDefaultWidth(), 20);
				fLineStyleComboBox->Select(fLineStyle);
				fEntries->Add(fLineStyleComboBox);
				fLineStyleComboBox->Associate(this);
				hButtonFrame->AddFrame(fLineStyleComboBox, loc);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("ArrowSelect")) {
				fArrowShape = *(Int_t*)fValPointers[i];
				fArrowComboBox = new TGComboBox(hButtonFrame, i + 1000*kIdArrowS);
				fArrowComboBox->AddEntry(" ----|>",1);
				fArrowComboBox->AddEntry(" <|----",2);
				fArrowComboBox->AddEntry(" ----->",3);
				fArrowComboBox->AddEntry(" <-----",4);
				fArrowComboBox->AddEntry(" --->--",5);
				fArrowComboBox->AddEntry(" ---<--",6);
				fArrowComboBox->AddEntry(" --|>--",7);
				fArrowComboBox->AddEntry(" --<|--",8);
				fArrowComboBox->AddEntry(" <--->", 9);
				fArrowComboBox->AddEntry(" <|--|>",10);
				fArrowComboBox->Resize(fArrowComboBox->GetDefaultWidth(), 20);
				fArrowComboBox->Select(fArrowShape);
				fEntries->Add(fArrowComboBox);
				fArrowComboBox->Associate(this);
				hButtonFrame->AddFrame(fArrowComboBox, loc);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("ComboSelect")) {
				fComboSelect = (TString*)fValPointers[i];
				fComboBox = new TGComboBox(hButtonFrame, i + 1000*kIdComboS);
				TString lab(l);
				Int_t tt_ind = lab.Index("&");  // Tool tip
				if (tt_ind > 1) {
					lab.Resize(tt_ind);
				}
				Int_t sel = 0;
				Int_t maxcha = 0;
				if (lab.Length() > 12) {
					lab.Remove(0,12);
					TObjArray * tokens;
					tokens = lab.Tokenize(";");
					Int_t nt = tokens->GetEntries();
					if (nt > 1) {
						Int_t ft = lab.Index(";");
						lab.Resize(ft);
						for (Int_t iv = 1; iv < nt; iv++) {
							TString s = ((TObjString*)tokens->At(iv))->String();
							fComboBox->AddEntry(s,iv-1);
							if (s.Length() > maxcha)
								maxcha = s.Length();
							if (s == *fComboSelect)
								sel = iv-1;
						}
					}
					delete tokens;
				}
				fComboBox->Select(sel);
				fEntries->Add(fComboBox);
				fComboBox->Associate(this);
				fComboBox->Resize((maxcha+1) * 12, 20);
				hButtonFrame->AddFrame(fComboBox, lor);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("Float_Value")) {
				Double_t neval = *(Float_t*)fValPointers[i];
				tnentry = new TGNumberEntry(hButtonFrame,neval, 5, i + 1000, style, attr, limsw, lowlim, uplim);
				hButtonFrame->AddFrame(tnentry, loc);
				hframe->AddFrame(hButtonFrame, loc);
				tnentry->Associate(this);
				fEntries->Add(tnentry);
			} else if (l.BeginsWith("DoubleValue")) {
				Double_t neval = *(Double_t*)fValPointers[i];
				tnentry = new TGNumberEntry(hButtonFrame,neval, 5, i + 1000, style, attr, limsw, lowlim, uplim);
				hButtonFrame->AddFrame(tnentry, loc);
				hframe->AddFrame(hButtonFrame, loc);
				tnentry->Associate(this);
				fEntries->Add(tnentry);

			} else if (l.BeginsWith("DoubleV+Cb")) {
				Double_t neval = *(Double_t*)fValPointers[i];
				tnentry = new TGNumberEntry(hButtonFrame,neval, 5, i + 1000, style, attr, limsw, lowlim, uplim);
				tnentry->Associate(this);
				fEntries->Add(tnentry);
				hButtonFrame->AddFrame(tnentry, l1);
				if (l.BeginsWith("DoubleV+CbF")) {
					label = new TTLabel(hButtonFrame, new TGString("Fix"));
				} else if (l.BeginsWith("DoubleV+CbU")) {
					label = new TTLabel(hButtonFrame, new TGString("Use"));
				} else {
					label = new TTLabel(hButtonFrame, new TGString(" "));
				}
				hButtonFrame->AddFrame(label, lone);
				label->ChangeBackground(lgrey);
				i += 1;
				cbutton = new TGCheckButton(hButtonFrame, new TGHotString(""), i + 1000);
				Int_t state = *(Int_t*)fValPointers[i];
				if (state != 0) cbutton->SetState(kButtonDown);
				else            cbutton->SetState(kButtonUp);
				cbutton->Resize(win_width / 8, cbutton->GetDefaultHeight());
				fLabels->Add(new TObjString("CheckButton_CbFU"));
				fEntries->Add(cbutton);
				cbutton->Associate(this);
				hButtonFrame->AddFrame(cbutton, lorne);
				hframe->AddFrame(hButtonFrame, loc);

			} else if (l.BeginsWith("PlainIntVal")) {
				Double_t neval = *(Int_t*)fValPointers[i];
				tnentry = new TGNumberEntry(hButtonFrame,neval, 5, i + 1000, 
				TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, limsw, lowlim, uplim );
				hButtonFrame->AddFrame(tnentry, loc);
				hframe->AddFrame(hButtonFrame, loc);
				tnentry->Associate(this);
				fEntries->Add(tnentry);
			} else if (l.BeginsWith("PlainShtVal")) {
				Double_t neval = *(Short_t*)fValPointers[i];
				tnentry = new TGNumberEntry(hButtonFrame,neval, 5, i + 1000, 
				TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, limsw, lowlim, uplim );
				hButtonFrame->AddFrame(tnentry, loc);
				hframe->AddFrame(hButtonFrame, loc);
				tnentry->Associate(this);
				fEntries->Add(tnentry);

			} else if (l.BeginsWith("StringValue")) {
				TString scol;
				scol = *(TString*)fValPointers[i];
				tentry = new TGTextEntry(hButtonFrame, tbuf = new TGTextBuffer(15), i + 1000*kIdText);
				tentry->GetBuffer()->AddText(0, (const char *)scol);
				hButtonFrame->AddFrame(tentry, loc);
				hframe->AddFrame(hButtonFrame, loc);
				tentry->Associate(this);
				fEntries->Add(tentry);
			} else if (l.BeginsWith("FileRequest")) {
				if ( l.Contains("rootfile", TString::kIgnoreCase) ) {
					fFileType[i] = 2;
				} else if ( l.Contains("datafile", TString::kIgnoreCase) ) {
					fFileType[i] = 4;
				} else if ( l.Contains("imagefile", TString::kIgnoreCase) ) {
					fFileType[i] = 6;
				} else {
					fFileType[i] = 0;
				}
				TString scol;
				scol = *(TString*)fValPointers[i];
				tentry = new TGTextEntry(hButtonFrame, tbuf = new TGTextBuffer(100), i + 1000*kIdText);
				fFileDialogContTextEntry[i]  = tentry;
				tentry->GetBuffer()->AddText(0, (const char *)scol);
				hButtonFrame->AddFrame(tentry, loc);
				tentry->Associate(this);
				fEntries->Add(tentry);
				TGPictureButton * tb = new TGPictureButton(hButtonFrame,
								 fClient->GetPicture("arrow_down.xpm"), i + 1000 * kIdFileDialog);
				tb->Resize(20, 20);
				tb->SetToolTipText("Browse");

				hButtonFrame->AddFrame(tb, lorne);
				hframe->AddFrame(hButtonFrame, loc);
				tb->Associate(this);

			} else if (l.BeginsWith("FileContReq")) {
				if ( l.Contains("rootfile", TString::kIgnoreCase) ) {
					fFileType[i] = 2;
				} else if ( l.Contains("datafile", TString::kIgnoreCase) ) {
					fFileType[i] = 4;
				} else if ( l.Contains("imagefile", TString::kIgnoreCase) ) {
					fFileType[i] = 6;
				} else {
					fFileType[i] = 0;
				}
				TString scol;
				TString fname("none");
				fClassName[i] = new TString("TF1");
				TString ename("none");

				scol = *(TString*)fValPointers[i];
				TObjArray * oa = scol.Tokenize("|");
				Int_t nent = oa->GetEntries();
				if (nent > 0) fname =((TObjString*)oa->At(0))->String();
				if (nent > 1) *(fClassName[i]) =((TObjString*)oa->At(1))->String();
				if (nent > 2) ename =((TObjString*)oa->At(2))->String();
				fFileNameEntry =
					new TGTextEntry(hButtonFrame, tbuf = new TGTextBuffer(100),  i + 1000*kIdFileName);
				fFileNameEntry->GetBuffer()->AddText(0, (const char *)fname);
				hButtonFrame->AddFrame(fFileNameEntry, loc);
				fFileNameEntry->Associate(this);
				fFileDialogContTextEntry[i] = fFileNameEntry;
				TGPictureButton * tb = new TGPictureButton(hButtonFrame,
								 fClient->GetPicture("arrow_down.xpm"), i + 1000 * kIdFileDialogCont);
				tb->Resize(20, 20);
				tb->SetToolTipText("Browse");

				hButtonFrame->AddFrame(tb, lorne);
				hframe->AddFrame(hButtonFrame, loc);
				this->AddFrame(hframe, lo1);
				hframe = NULL;
				tb->Associate(this);
//
				fListBoxReq = new TGListBox(this,  i + 1000*kIdListBoxReq);
				fListBoxReq->AddEntry(ename.Data(), 0);
				fListBoxReq->Resize(win_width , 150);
				fListBoxReq->Layout();
				fListBoxReq->Associate(this);
				this->AddFrame(fListBoxReq, l2);
				fEntries->Add(fListBoxReq);
				UpdateRequestBox(fFileNameEntry->GetBuffer()->GetString(),  kTRUE, i);
			}
			if (fFlags) {
				hframe1 = new TGCompositeFrame(hframe, win_width*1/6, 20, kFixedWidth);
				cbutton = new TGCheckButton(hframe1, new TGHotString(""), i + 1000*300);
				fFlagButtons->Add(cbutton);
				if (fFlags->At(i) == 1) cbutton->SetState(kButtonDown);
				else                cbutton->SetState(kButtonUp);
				hframe1->AddFrame(cbutton, lor);
				hframe->AddFrame(hframe1, lor);
			}
			i += 1;
		}
	}
	
	if (hframe != NULL) {
		this->AddFrame(hframe, lo1);
	}
	TGTextButton *b;
// create Prompt label and textentry widget

	if (fText) {
		hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);
		TGLabel *label = new TGLabel(hframe, Prompt);
//      fWidgets->AddFirst(label);
		hframe->AddFrame(label, lo1);
		if (History != NULL) {
			b = new TGTextButton(hframe, "Clear history",  1000*kIdClearHist);
 //        fWidgets->AddFirst(b);
			b->Associate(this);
			hframe->AddFrame(b, lor);
		}
		this->AddFrame(hframe, l2);

		TGTextBuffer *tbuf = new TGTextBuffer(256);  // will be deleted by TGtextEntry
		tbuf->AddText(0, text->Data());

		fTE = new TGTextEntry(this, tbuf,  1000*kIdTextValue);
		fTE->Resize(win_width, fTE->GetDefaultHeight());
		fTE->Associate(this);
//      fWidgets->AddFirst(fTE);

		this->AddFrame(fTE, l2);

		// optionally create a ListBox from which items can be selected
		Int_t wid;
		fHistory = History;         // remember
		if (History != NULL) {
			if (gSystem->AccessPathName(History)) {
				cout << "Warning: File with selections not found: " << History << endl;
			} else {
				ifstream selections(History);
				TString line;
				Int_t idl = 0;
				fListBox = new TGListBox(this,  1000*kIdTextSelect);
				while (1) {
					line.ReadLine(selections);
					if (selections.eof()) break;
					fListBox->AddEntry(line.Data(), idl);
					if (idl == 00 && (Int_t)fTE->GetBuffer()->GetTextLength() == 0) {
						fTE->GetBuffer()->AddText(0, line.Data());
						if (fText) *fText = fTE->GetBuffer()->GetString();
						gClient->NeedRedraw(fTE);
					}
					idl++;
				}
				selections.close();
 //           cout << "Entries: " << id << endl;
				wid = 8 * fTE->GetBuffer()->GetTextLength();
				fListBox->Resize(TMath::Max(wid,320), TMath::Min(200, idl*20));
				fListBox->Layout();
				fListBox->Associate(this);
//            fWidgets->AddFirst(fListBox);
				this->AddFrame(fListBox, l2);
			}
		}
		wid = 8 * fTE->GetBuffer()->GetTextLength();
		fTE->Resize(TMath::Max(wid,320), fTE->GetDefaultHeight());
	}
	// create frame and layout hints for Ok and Cancel buttons

	TGHorizontalFrame *hf = new TGHorizontalFrame(this, win_width, 20, kFixedWidth);
//   fWidgets->AddFirst(hf);
	// put hf as last in list to be deleted

	// create OK and Cancel buttons in their own frame (hf)
	TGLayoutHints * ll = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY , 2, 2, 0, 0);
	TGLayoutHints * lc = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY , 2, 2, 0, 0);
	TGLayoutHints * lr = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY , 2, 2, 0, 0);

	UInt_t  nb = 0, width = 0, height = 0;
	b = NULL;
	if ( fCloseFlag >= -2 ) {
		if (!has_commands) {
			b = new TGTextButton(hf, "Apply",  1000*kIdOk);
			b->SetToolTipText("Apply action and close dialog");
	
		} else {
			b = new TGTextButton(hf, "Save-Quit",  1000*kIdOk);
			b->SetToolTipText("Save current parameters and close dialog");
		}
	}
//		fWidgets->AddFirst(b);
	if ( b ) {
		b->Associate(this);
		hf->AddFrame(b, ll);
		height = b->GetDefaultHeight();
		width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
	}
	if ( fCloseFlag >= -2) 
		fCancelButton = new TGTextButton(hf, "Cancel",  1000*kIdCancel);
	else
		fCancelButton = new TGTextButton(hf, "Close Dialog",  1000*kIdCancel);
	fCancelButton->SetToolTipText("Close dialog");
//   fWidgets->AddFirst(fCancelButton);
	fCancelButton->Associate(this);
	hf->AddFrame(fCancelButton, lc);
	height = fCancelButton->GetDefaultHeight();
	width  = TMath::Max(width, fCancelButton->GetDefaultWidth()); ++nb;
	if(helptext){
		b = new TGTextButton(hf, "Help", 1000 * kIdHelp);
//      fWidgets->AddFirst(b);
		b->Associate(this);
		hf->AddFrame(b, lr);
		height = b->GetDefaultHeight();
		width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
	}
	// place button frame (hf) at the bottom
	TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
//   fWidgets->AddFirst(l4);

	this->AddFrame(hf, l4);

	// keep buttons centered and with the same width
	hf->Resize((width + 20) * nb, height);

	// set dialog title
	TString wn("");
	wn += Prompt;
	this->SetWindowName(wn.Data());

	// map all widgets and calculate size of dialog
	this->MapSubwindows();

	width  = this->GetDefaultWidth();
	height = this->GetDefaultHeight();
//   cout << "width " << width<< endl;
	this->Resize(win_width, height);

	// position relative to the parent window (which is the root window)
	Window_t wdum;
	int      ax, ay;

	gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
			0, ((TGFrame *) fMyWindow)->GetHeight(),
								  ax, ay, wdum);

	ax = ax - (int)( win_width );
	if (fCallingCanvas && fCallingCanvas->TestBit(kIsAEditorPage) ) {
		ax += 180;
	}

//   ax = ax - (int)( win_width >> 1);
	ay = ay - (int)( height) - 30;

	UInt_t ww, wh;
	Int_t  wwi,whi;
	Int_t screen_x, screen_y;
//  screen size in pixels
	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
				  screen_x, screen_y, ww, wh);
	wwi = ww;
	whi = wh;
	Int_t wi = win_width;
	Int_t hi = height;
//  make sure its inside physical screen
	if      (ax < 0)    ax = 10;
	else if (ax + wi > wwi)  ax = wwi - wi - 5;
	if      (ay + hi/2 < 0) ay =  hi/2 + 5;
	else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

	this->Move(ax, ay);
	this->SetWMPosition(ax, ay);
	fPosX = ax;
	fPosY = ay;
	fWidX = win_width;
	fWidY = height;
	
	// popup dialog and wait till user replies
	this->MapWindow();
	this->ChangeBackground(brown);
	hf->ChangeBackground(brown);
//   if (!has_commands)
	if (calling_class == NULL || mustwait)
		gClient->WaitFor(this);
};
//________________________________________________________________________________________
Bool_t TGMrbValuesAndText::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
	// Handle button and text enter events
//  cout << "TGMrbValuesAndText::ProcessMessage " << GET_MSG(msg) << " "
//      << GET_SUBMSG(msg) << " parm1 " << parm1 << " parm2 " << parm2 << endl;

	if (!fEmitClose) return kTRUE;
	Int_t idButton = 0, idCmd = 0;
	if (parm1 >= 1000) {
		idCmd = parm1 / 1000;
		idButton = parm1%1000 ;
	}
	switch (GET_MSG(msg)) {
		case kC_COMMAND:
			switch (GET_SUBMSG(msg)) {
				case kCM_BUTTON:
					{
					if (idCmd == kIdOk) {
						// here copy the string from text buffer to return variable
						if (fText) *fText = fTE->GetBuffer()->GetString();
						if (fHistory && fHistory.Length() > 0) this->SaveList();
						StoreValues();
						idButton = -1;
						*fReturn = 0;
						CloseDown(idButton);
					} else if (idCmd == kIdClearHist) {
						if (fHistory && fHistory.Length() > 0) {
							TString cmd(fHistory);
							cmd.Prepend("rm ");
							gSystem->Exec(cmd);
#if ROOT_VERSION_CODE>= ROOT_VERSION(5,10,0)
							fListBox->RemoveAll();
#else
							fListBox->RemoveEntries(0, 1000);
#endif
							gClient->NeedRedraw(fListBox);
							idButton = -1;
						}
					} else if (idCmd == kIdCancel) {
						idButton = -2;
						CloseDown(idButton);
						return kTRUE;
					} else if (idCmd == kIdHelp) {
						TString temp(fHelpText);
//						cout << temp << endl;
						if (temp.BeginsWith("<!DOCTYPE HTML") ){
							TMrbHelpBrowser * helpbr = (TMrbHelpBrowser*)gROOT->FindObject("TMrbHelpBrowser");
							if (helpbr) {
								TString anchor;
								Int_t ind_title_start=temp.Index("<TITLE>");
								Int_t ind_title_end=temp.Index("</TITLE>");
								if (ind_title_start > 0 && ind_title_end > 0) {
									anchor = temp(ind_title_start+7, ind_title_end-(ind_title_start+7));
								} else {
									anchor = "Help_",
									anchor += temp.Length();
								}
								TString dir;
								if ( !helpbr->GetHelpList()->FindObject(anchor)) {
									helpbr->AddHtmlAsString(temp, anchor, dir);
								}
								helpbr->DrawText(anchor, GetPosX()+GetWidX(), GetPosY()+GetWidY());
								return kTRUE;
							}
						}
						Int_t nl = temp.CountChar('\n');
						nl *= 15;
						TRootHelpDialog * hd = new TRootHelpDialog(this, fPrompt, 500, nl+25);
						hd->SetText(fHelpText);
						hd->Popup();
						idButton = -1;
					} else {
						if (idCmd == kIdFileDialog || idCmd == kIdFileDialogCont) {
//                      Int_t entryNr = idButton;
							TString fn;
							TGFileInfo* fi = new TGFileInfo();
							TGTextEntry *te = fFileDialogContTextEntry[idButton];
							fn = te->GetText();
//							cout << "fFileDialogContTextEntry " << fn<< endl;
							if ( fn.BeginsWith("/") ) {
								 TString bn = gSystem->BaseName(fn);
								 Size_t bnl = bn.Length();
								 if ( bnl > 0 ) {
									 fn.Resize(fn.Length() - bnl);
								 }
								 cout << "fInidir " << fn<< endl;
								 char * inidir = new char[fn.Length()+1];
								 strncpy(inidir, fn.Data(), fn.Length());
								 inidir[fn.Length()] = 0;
								 fi->fIniDir = inidir;
//								 fi->fIniDir = (char*)fn.Data();
							}
 //                    const char * filter[] = {"data files", "*", 0, 0};
							fi->fFileTypes = filetypes;
							fi->fFileTypeIdx = fFileType[idButton];
// 							 cout << "fFileType " << fFileType[idButton]<< endl;
							new  TGFileDialog(gClient->GetRoot(), this, kFDOpen, fi);
  							if (fi->fFilename) {
								fn = fi->fFilename;
//                        TString cwd(gSystem->pwd());
//                        if (fn.BeginsWith(cwd.Data()))fn.Remove(0,cwd.Length()+1);
								te->SetText(fn.Data());
								fMustRestoreDir = kTRUE;
								gClient->NeedRedraw(te);
								gClient->NeedRedraw(this);
							}
							delete fi;
							if (idCmd == kIdFileDialogCont) {
								UpdateRequestBox(fn.Data(), kTRUE, idButton);
							}

						}
					 }
					 break;
					 }
				case kCM_RADIOBUTTON:
					{
//                   cout << "toggle TGRadioButtons " << parm1 << endl;
						TIter nextent(fEntries);
						TObject * obj;
						Int_t i = 0;
						while ( (obj = nextent()) ) {
							if (obj->InheritsFrom("TGRadioButton"))  {
								if (i == idButton)
									((TGRadioButton*)obj)->SetState(kButtonDown);
								 else
									((TGRadioButton*)obj)->SetState(kButtonUp);
							}
							i++;
						}
						break;
					}
				case kCM_COMBOBOX:
					{
						break;
					}

				case kCM_LISTBOX:
					{
					switch (idCmd) {
						case kIdTextSelect:
							TGTextLBEntry * tge = (TGTextLBEntry *)fListBox->GetEntry(parm2);
							TString txt = tge->GetText()->GetString();
							fTE->SetText(txt.Data());
							gClient->NeedRedraw(fTE);
							if (fText) *fText = txt.Data();
							break;
						 }
					}
				default:
					break;
			 }
			 break;

		 case kC_TEXTENTRY:
			switch (GET_SUBMSG(msg)) {
				 case kTE_TAB:
//               cout << "Tab " << idCmd << " " << fCompList<< endl;
//                if (idCmd != kIdText) break;
					 if (fCompList) {
						 TString temp = fTE->GetBuffer()->GetString();
						 Int_t indcol = IndexOfLastSpecial(temp);
						 TString var = temp(indcol+1, temp.Length() - indcol - 1);
//                   cout << "var: " << var << endl;
						 Int_t matchlength;
						 Int_t pos = Matches(fCompList, var.Data(), &matchlength);
						 if (pos >= 0) {
							 TObjString * obs = (TObjString *)fCompList->At(pos);
							 TString result = (obs->GetString()).Data();
							 if (matchlength != result.Length())result = result(0,matchlength);
							 temp.Replace(indcol + 1, temp.Length() - indcol - 1, result.Data());
							 fTE->SetText(temp.Data());
						 }
					 }
					 break;
				 case kTE_ENTER:
//                cout << "kTE_ENTER parm1 " << idCmd << "  kIdText " << kIdText << " " << fText << endl;
					 if (idCmd == kIdFileName) {
						 TString fn = fFileNameEntry->GetBuffer()->GetString();
						 UpdateRequestBox(fn, kTRUE, idButton);
					 }
                break;
				 default:
					 if (fText && idCmd == kIdTextValue) {
						 *fText = fTE->GetBuffer()->GetString();
//                    cout << "kC_TEXTENTRY: " << GET_SUBMSG(msg) << " "
//                    << idCmd << " " << parm2 << endl;
					 }
					 break;
			 }
			 break;

		 default:
//      	 if (fValPointers != NULL)
//         	 StoreValues();
			 break;
	}
//cout << "ProcessMessage  " << fValPointers  << " "
//     << idButton << " " << kIdExec << endl;
//  only if a command is executed
	if (  GET_MSG(msg) == kC_COMMAND
		&& GET_SUBMSG(msg) == kCM_BUTTON
		&& idCmd== kIdCommand) {
//
		if (fText) *fText = fTE->GetBuffer()->GetString();
		if (fHistory && fHistory.Length() > 0) this->SaveList();
		ReloadValues();
//      StoreValues();
//   } else if (fValPointers != NULL && idButton >= 0  && idButton != kIdExec ) {
	} else if (fValPointers != NULL && idButton >= 0) {
		StoreValues();
	}
	if (idButton >= 0) {
		CRButtonPressed(fWidgetId, idButton, fCallingCanvas);
	}
	return kTRUE;
}

//_____________________________________________________________________________

void TGMrbValuesAndText::UpdateRequestBox(const char *fname, Bool_t store, Int_t iseq)
{
	if (gSystem->AccessPathName(fname)) {
		if ( fname )cout << "Cant find: " << fname << endl;
		return;
	}
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,10,0)
	fListBoxReq->RemoveAll();
#else
	fListBoxReq->RemoveEntries(0, 1000);
#endif
	TFile f(fname);
	TIter next(f.GetListOfKeys());
	TKey* key;
	TObject* obj;
	Int_t id = 0;
	while( (key = (TKey*)next()) ){
		obj = (TObject*)key->ReadObj();
		if ( !obj )
			continue;
//      cout << "AddEntry " << fClassName<< " " <<  obj->ClassName()<< endl;
		TString tn(obj->ClassName());
		if ( tn.BeginsWith(*(fClassName[iseq])) ) {
			TString s(obj->GetName());
			s += ";";
			s+= key->GetCycle();
			s += " | ";
			s += obj->GetTitle();
			fListBoxReq->AddEntry(s.Data(), id);
//         cout << "AddEntry |" << s.Data() << "|"<< endl;
			id++;
		}
	}
	if ( id > 0 )
		fListBoxReq->Select(0, kTRUE);
//   cout << "fListBoxReq->GetDefaultWidth()" << fListBoxReq->GetDefaultWidth() << endl;
	fListBoxReq->Resize(fListBoxReq->GetDefaultWidth(), TMath::Min(200, (id+1)*20));
	fListBoxReq->Layout();
	gClient->NeedRedraw(fListBoxReq);
	this->Resize((UInt_t) TMath::Abs(fWindowWidth), this->GetDefaultHeight());
	gClient->NeedRedraw(this);
	if (store)
		StoreValues();
}
//________________________________________________________________[C++ METHOD]
//_____________________________________________________________________________

void TGMrbValuesAndText::StoreValues(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbValuesAndText::StoreValues()
// Purpose:        Store return values
// Arguments:
//
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

 //  cout << "StoreValues()" << endl;

//   if (fFinis != 0) return;
	if (fEntries == NULL) return;

	TIter nextent(fEntries);
	TIter next(fLabels);
	TObjString *objs;

	TObject * obj;
	TGCheckButton * cbutton;
	TGColorSelect * colsel;
	Int_t i=0;

	while((objs = (TObjString*)next())){
		 obj=nextent();
		 if ( !obj ) continue;
		 fButtonId = i;
//       cout << "fValue: " << objs->String() << " " << obj->ClassName()  << endl;
		 if (obj->InheritsFrom("TGColorSelect")){
			 colsel = (TGColorSelect*)obj;
			 Int_t col = colsel->GetColor();
			 Color_t val =  TColor::GetColor((Int_t)col);
			 *(Color_t*)fValPointers[i] = val;

		 } else if (obj->InheritsFrom("TGedAlignSelect")) {
			 Short_t align =((TGedAlignSelect*)obj)->GetAlignStyle();
			 *(Short_t*)fValPointers[i] = align;

		 } else if (obj->InheritsFrom("TGedMarkerSelect")) {
			 fMarker = ((TGedMarkerSelect*)obj)->GetMarkerStyle();
			 *(Style_t*)fValPointers[i] = fMarker;

		 } else if (obj->InheritsFrom("TGedPatternSelect")) {
			 Style_t sel = ((TGedPatternSelect*)obj)->GetPattern();
			 *(Style_t*)fValPointers[i] = sel;

		 } else if (obj->InheritsFrom("TGComboBox")) {
			 Short_t sel = -1;
			 if (((TGComboBox*)obj)->WidgetId() / 1000 == kIdArrowS) {
				 sel = ((TGComboBox*)obj)->GetSelected();
				 *(Short_t*)fValPointers[i] = sel;
			 } else if (((TGComboBox*)obj)->WidgetId() / 1000 == kIdFontS) {
				 sel = ((TGComboBox*)obj)->GetSelected();
				 Font_t tf = *(Short_t*)fValPointers[i];
				 Int_t prec = tf % 10;
				 if ( prec > 3 ) prec = 2;
				 sel = sel * 10 + prec;
				 *(Short_t*)fValPointers[i] = sel;

			 } else if (((TGComboBox*)obj)->WidgetId()  / 1000== kIdLineS) {
				 sel = ((TGComboBox*)obj)->GetSelected();
				 *(Short_t*)fValPointers[i] = sel;

			 } else if (((TGComboBox*)obj)->WidgetId()  / 1000== kIdComboS) {
				 TGTextLBEntry *sent = (TGTextLBEntry*)((TGComboBox*)obj)->GetSelectedEntry();
				 *(TString*)fValPointers[i]  = sent->GetText()->GetString();
			 }

		 } else if (obj->InheritsFrom("TGCheckButton")) {
			 cbutton = (TGCheckButton*)obj;
			 Int_t state = 0;
			 if (cbutton->GetState() == kButtonDown) state = 1;
			 *(Int_t*)fValPointers[i] = state;
//          cout << "TGCheckButton:STORE; " << i << " " << state << endl;

		 } else if (obj->InheritsFrom("TGRadioButton")) {
			 cbutton = (TGCheckButton*)obj;
			 Int_t state = 0;
			 if (cbutton->GetState() == kButtonDown) state = 1;
			 *(Int_t*)fValPointers[i] = state;

		 } else if (obj->InheritsFrom("TGNumberEntry")) {

			 TGNumberEntry * tnentry = (TGNumberEntry*)obj;
			 TString tmp(objs->String());
			 if (tmp.BeginsWith("DoubleV")) {
//              cout << "TGNumberEntry " << tmp << " " << tnentry->GetNumber()<< endl;
				  *(Double_t*)fValPointers[i] = tnentry->GetNumber();

			 } else if (tmp.BeginsWith("Float_Value")) {
				  *(Float_t*)fValPointers[i] = (Float_t)tnentry->GetNumber();

			 } else if (tmp.BeginsWith("PlainIntVal")) {
				 *(Int_t*)fValPointers[i] = tnentry->GetIntNumber();
			 } else if (tmp.BeginsWith("PlainShtVal")) {
				 *(Short_t*)fValPointers[i] = (Short_t)tnentry->GetIntNumber();
			 }
		 } else if (obj->InheritsFrom("TGTextEntry") || obj->InheritsFrom("TGListBox")) {
			 TGTextEntry   * tentry;
			 if (obj->InheritsFrom("TGListBox"))
				 tentry = fFileDialogContTextEntry[i];
			 else
				 tentry = (TGTextEntry*)obj;
			 TString tmp(objs->String());
//          cout << "TGTextEntry " << tmp << endl;
			 if (tmp.BeginsWith("StringValue") || tmp.BeginsWith("FileRequest")
				 || tmp.BeginsWith("FileContReq") ) {
				 TString * sr = (TString*)fValPointers[i];
//             cout << " " << tmp << " " << *sr << endl;
				 if (tmp.BeginsWith("FileContReq")) {
					 TString retstr(tentry->GetBuffer()->GetString());
					 retstr += "|";
					 retstr += *(fClassName[i]);
					 retstr += "|";
					 TGLBEntry * tle = fListBoxReq->GetSelectedEntry();
					 if (tle == NULL) {
						 retstr += "noselection";
					 } else {
						 TGTextLBEntry * tge = (TGTextLBEntry *)tle;
						 TString temp(tge->GetText()->GetString());
						 Int_t ip = temp.Index("|");
						 if (ip > 0) temp.Resize(ip);
						 retstr += temp.Data();
						 retstr = retstr.Strip(TString::kBoth);
					 }
					 *sr = retstr.Data();
				 } else {
//                cout << "TGTextEntry *sr... " << tentry->GetBuffer()->GetString()<< endl;
					 *sr = tentry->GetBuffer()->GetString();
				 }
			 }
		 }

		 if (fFlags) {
			 cbutton = (TGCheckButton*)fFlagButtons->At(i);
			 if (cbutton->GetState() == kButtonDown) (*fFlags)[i]= 1;
			 else                                    (*fFlags)[i]= 0;
		 }
		 i++;
	}
}
//_____________________________________________________________________________

void TGMrbValuesAndText::ReloadValues(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbValuesAndText::ReloadValues()
// Purpose:        Reload values which might been modified
//                 during execution of a command
// Arguments:
//
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!fEntries) return;

	TIter nextent(fEntries);
	TIter next(fLabels);
	TObjString *objs;

	TObject * obj;
	Int_t i=0;

	while((objs = (TObjString*)next())){
		 obj=nextent();
		 ReloadValue(obj, objs, i);
		 i++;
	}
	if (fText && fTE) fTE->SetText(fText->Data());
}
//_____________________________________________________________________________

void TGMrbValuesAndText::ReloadValue(TObject * obj, TObjString *objs, Int_t ipos)
{
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Purpose:        Reload value which might been modified
//                 during execution of a command
// Arguments:
//
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////
//       cout << "fValue: " << objs->String() << endl;
	if (obj->InheritsFrom("TGColorSelect")){
		TGColorSelect *colsel = (TGColorSelect*)obj;
		Int_t col = GetColorPixelByInd(*(Color_t*)fValPointers[ipos]);
		colsel->SetColor(col);

	} else if (obj->InheritsFrom("TGedAlignSelect")) {
		TGedAlignSelect * as = (TGedAlignSelect*)obj;
		as->SetAlignStyle(*(Short_t*)fValPointers[ipos]);

	} else if (obj->InheritsFrom("TGedMarkerSelect")) {
		TGedMarkerSelect * ms = (TGedMarkerSelect*)obj;
		ms->SetMarkerStyle(*(Short_t*)fValPointers[ipos]);

	} else if (obj->InheritsFrom("TGedPatternSelect")) {
		TGedPatternSelect * ms = (TGedPatternSelect*)obj;
		ms->SetPattern(*(Short_t*)fValPointers[ipos]);
	} else if (obj->InheritsFrom("TGCheckButton")) {
		TGCheckButton *cbutton = (TGCheckButton*)obj;
		Int_t state = *(Int_t*)fValPointers[ipos];
		if (state != 0) cbutton->SetState(kButtonDown);
		else                    cbutton->SetState(kButtonUp);
//		cout << "TGCheckButton:reload; " << ipos << " " << state << endl;

	} else if (obj->InheritsFrom("TGRadioButton")) {
		TGRadioButton *cbutton = (TGRadioButton*)obj;
		Int_t state = *(Int_t*)fValPointers[ipos];
		if (state != 0) cbutton->SetState(kButtonDown);
		else                    cbutton->SetState(kButtonUp);
	} else if (obj->InheritsFrom("TGNumberEntry")) {

		TGNumberEntry * tnentry = (TGNumberEntry*)obj;
		TString tmp(objs->String());
		if (tmp.BeginsWith("DoubleV")) {
//         cout << "ReloadValue: " << tmp << " " << ipos << "  " << *(Double_t*)fValPointers[ipos] << endl;
			tnentry->SetNumber(*(Double_t*)fValPointers[ipos]);
		} else if (tmp.BeginsWith("Float_Value")) {
			*(Float_t*)fValPointers[ipos] = (Float_t)tnentry->GetNumber();
			Double_t va = *(Float_t*)fValPointers[ipos];
			tnentry->SetNumber(va);

		} else if (tmp.BeginsWith("PlainIntVal")) {
			Int_t va = *(Int_t*)fValPointers[ipos];
			tnentry->SetIntNumber(va);

		} else if (tmp.BeginsWith("PlainShtVal")) {
			Int_t va = *(Short_t*)fValPointers[ipos];
			tnentry->SetIntNumber(va);
		}
	} else if (obj->InheritsFrom("TGTextEntry")) {
		TGTextEntry   * tentry  = (TGTextEntry*)obj;
		TString tmp(objs->String());
		if (tmp.BeginsWith("StringValue") || tmp.BeginsWith("FileRequest")) {
			TString * sr = (TString*)fValPointers[ipos];
			tentry->SetText(sr->Data());
		}
	}
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::SaveList()
{
//   cout << "enter SaveList() " << endl;
	if (fListBox == NULL) {
		fListBox = new TGListBox(this, kIdTextSelect);
		fListBox->AddEntry(fTE->GetBuffer()->GetString(), 0);
//      fWidgets->AddFirst(fListBox);
		gClient->NeedRedraw(this);
	}
	if (!(gSystem->AccessPathName(fHistory.Data()))) {
		TString bak(fHistory);
		bak += ".bak";
		gSystem->Rename(fHistory.Data(), bak.Data());
	}
	ofstream outfile(fHistory.Data());
	TString sel(fTE->GetBuffer()->GetString());
	Int_t ne = fListBox->GetNumberOfEntries();
//   cout << "SaveList() Selected: |" << sel <<"|" << endl;
	outfile << sel << endl;
	Bool_t in = kFALSE;
	if (ne > 0) {
		for (Int_t i = 0; i < ne; i++) {
			TGLBEntry * tle = fListBox->GetEntry(i);
			if (tle == NULL) continue;
			TGTextLBEntry * tge = (TGTextLBEntry *)tle;
			TString text = tge->GetText()->GetString();

 //        cout << "Ent: |" <<  text << "|"  << endl;
			if (text != sel) {
				outfile <<text.Data() << endl;
			} else {
				in = kTRUE;
			}
		}
	}
	if (!in) {
//      cout << "Put: " << sel.Data() << endl;
		fListBox->AddEntry(sel.Data(), ne);
		Int_t wid = 8 * fTE->GetBuffer()->GetTextLength();
		fListBox->Resize(TMath::Max(wid,320), TMath::Min(200, (ne+1)*20));
		fListBox->Layout();
		this->Resize(this->GetDefaultWidth(),this->GetDefaultHeight());
		gClient->NeedRedraw(fListBox);
		gClient->NeedRedraw(this);
	}
	outfile.close();
}

//_______________________________________________________________________________________

TGMrbValuesAndText::~TGMrbValuesAndText()
{
// Cleanup dialog.
		if (gDebug > 0)
			cout << "enter dtor: TGMrbValuesAndText " << endl;
		if ( fMustRestoreDir ) {
			gSystem->cd(fStartDir);
//			cout << gSystem->Getenv("PWD") << endl;
		}
		if (fFlagButtons) delete fFlagButtons;
		if ( fLabels->GetSize() <=0 ) return;
		if (fFileType) delete [] fFileType;
		if (fFileDialogContTextEntry) delete [] fFileDialogContTextEntry;
		for (Int_t i = 0; i < MAXROWS; i++) {
			if ( fClassName[i] != NULL) 
				delete fClassName[i];
		}
		delete  [] fClassName;
		if (gDebug > 0)
			cout << "exit dtor: TGMrbValuesAndText " << endl;
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::CloseWindow()
{
// come here when deleted by Windowmanager (X)
//	cout <<"TGMrbValuesAndText::CloseWindow() " << endl;
	CloseDown(-2);
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::CloseWindowExt()
{
	fEmitClose = kFALSE;
	CloseDown(0);
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::CRButtonPressed(Int_t /*i*/, Int_t bid, TObject */*obj*/)
{
	if (gDebug > 0)
		cout << "TGMrbValuesAndText::Emit: CRButtonPressed(" << fWidgetId   <<"," << bid<< ")"
		<< endl << flush;
	Long_t args[3];
	args[0] = (Long_t)fWidgetId;
	args[1] = (Long_t)bid;
	args[2] = (Long_t)fCallingCanvas;
	Emit("CRButtonPressed(Int_t, Int_t, TObject*)", args);
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::CloseDown(Int_t wid)
{
	Long_t args[1];
	args[0] = (Long_t)wid;
	if (fEmitClose) {
		Emit("CloseDown(Int_t)", args);
//      cout << "TGMrbValuesAndText::CloseDown(" << wid << ") " << endl << flush;;
	}
	DeleteWindow();
}
//_____________________________________________________________________________

Int_t TGMrbValuesAndText::GetColorPixelByInd(Int_t index)
{
	Int_t pixels = 0;
	TIter next(gROOT->GetListOfColors());
	while (TColor * c = (TColor *)next()) {
		if (c->GetNumber() == index) {
			pixels = c->GetPixel();;
		}
	}
	return pixels;
}
//______________________________________________________________________________
void TGMrbValuesAndText::EnableButton(Int_t id)
{
//   TGButton *b = (TGButton *)fEntries->At(id);
	TObjString *objs = (TObjString*)fLabels->At(id);
	if (fEntries->At(id)->InheritsFrom("TGButton")) {
		TGButton *b = (TGButton *)fEntries->At(id);
		if (b) {
			 b->SetEnabled(kTRUE);
			 ReloadValue(b, objs, id);
			 gClient->NeedRedraw(this);
			 cout << "EnableButton " << id << endl;
		}
	} else if (fEntries->At(id) ->InheritsFrom("TGNumberEntry")) {
		TGNumberEntry *b = (TGNumberEntry *)fEntries->At(id);
		if (b) {
			b->SetState(kTRUE);
			ReloadValue(b, objs, id);
			gClient->NeedRedraw(this);
		}
	}
};
//______________________________________________________________________________

void TGMrbValuesAndText::DisableButton(Int_t id)
{
	if (fEntries->At(id) ->InheritsFrom("TGButton")) {
		TGButton *b = (TGButton *)fEntries->At(id);
		if (b) {
			b->SetEnabled(kFALSE);
			gClient->NeedRedraw(this);
		}
	} else if (fEntries->At(id) ->InheritsFrom("TGNumberEntry")) {
		TGNumberEntry *b = (TGNumberEntry *)fEntries->At(id);
		if (b) {
			b->SetState(kFALSE);
			gClient->NeedRedraw(this);
		}
	}

};
//______________________________________________________________________________

void TGMrbValuesAndText::SetCheckButton(Int_t id, Int_t state)
{
	if (fEntries->At(id) ->InheritsFrom("TGCheckButton")) {
		TGCheckButton *b = (TGCheckButton *)fEntries->At(id);
		if (b) {
			if ( state )
				b->SetState(kButtonDown);
			else
				b->SetState(kButtonUp);
		}
	}
}
//______________________________________________________________________________

void TGMrbValuesAndText::SetComboSelect(Int_t id, Int_t sel)
{
	if (fEntries->At(id) ->InheritsFrom("TGComboBox")) {
		TGComboBox *b = (TGComboBox *)fEntries->At(id);
		if (b) {
			if ( sel >= 0 && sel < b->GetNumberOfEntries() )
				b->Select(sel, kFALSE);
		}
	}
}

Bool_t GetStringExt(const char *Prompt, TString  *text ,
								Int_t win_width, TGWindow *Win,
								const char *History, TList * Complist,
								TList * rowlabs, void **val_pointers,
								TArrayI * Flags, const char * Flagslabel,
								const char *helptext, TObject * calling_class,
								const char * cname)
{
	Int_t ret = 0;

	new TGMrbValuesAndText(Prompt, text, &ret, win_width,  Win,
		 History, Complist, rowlabs, val_pointers,Flags, Flagslabel, helptext,
		 calling_class, cname);
	if(ret == 0) return kTRUE;
	else         return kFALSE;
}
//_____________________________________________________________________

Int_t TGMrbValuesAndText::LabelLetterWidth()
{
	TGFont *label_font = gClient->GetFont(gEnv->GetValue("Gui.MenuHiFont",
						"-adobe-courier-bold-r-*-*-13-*-*-*-*-*-iso8859-1"));
	if (gDebug > 0) {
		label_font->Print();
		cout << "LabelLetterWidth: " << label_font->TextWidth("A")<< endl;
	}
   return label_font->TextWidth("A");
}

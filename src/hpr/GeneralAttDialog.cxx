#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"
#include "TKey.h"
#include "TStyle.h"
#include "TSystem.h"
#include "GeneralAttDialog.h"
#include "HistPresent.h"
#include "SetColor.h"
#include "hprbase.h"
#include <iostream>


namespace std {} using namespace std;

Int_t GeneralAttDialog::fPrependFilenameToName = 1;
Int_t GeneralAttDialog::fPrependFilenameToTitle = 0;
Int_t GeneralAttDialog::fAppendTimestampToHistname = 0;
Int_t GeneralAttDialog::fForceStyle = 1;
Int_t GeneralAttDialog::fShowPSFile = 0;
Int_t GeneralAttDialog::fSuppressWarnings = 1;
Int_t GeneralAttDialog::fUseRegexp = 0;
Int_t GeneralAttDialog::fShowListsOnly = 0;
Int_t GeneralAttDialog::fRememberLastSet = 1;
Int_t GeneralAttDialog::fRememberZoom = 1;
Int_t GeneralAttDialog::fRememberTitle = 0;
Int_t GeneralAttDialog::fRememberStatBox = 0;
Int_t GeneralAttDialog::fRememberLegendBox = 1;
Int_t GeneralAttDialog::fUseAttributeMacro = 0;
Int_t GeneralAttDialog::fMaxListEntries = 333;
Int_t GeneralAttDialog::fMaxFileListEntries = 333;
Int_t GeneralAttDialog::fSkipFirst = 0;
Int_t GeneralAttDialog::fContentLowLimit = 0;
Int_t GeneralAttDialog::fVertAdjustLimit =0;
Int_t GeneralAttDialog::fAdjustMinY      =0;
Int_t GeneralAttDialog::fStackedReally = 1;
Int_t GeneralAttDialog::fScaleStack = 1;
Int_t GeneralAttDialog::fStackedNostack = 0;
Int_t GeneralAttDialog::fStackedPads = 0;
Int_t GeneralAttDialog::fLegoSuppressZero = 1;
TString GeneralAttDialog::fGlobalStyle = "Plain";

//_______________________________________________________________________

GeneralAttDialog::GeneralAttDialog(TGWindow * win)
{
static const char helptext[] =
"\n\
___________________________________________________________\n\
Force current style\n\
\n\
Use graphics option currently in use instead of options\n\
stored with the histogram on file\n\
___________________________________________________________\n\
Global style\n\
\n\
Activate roots style options\n\
\n\
___________________________________________________________\n\
Append timestamp to histo names\n\
\n\
With this option histograms get unique names when displayed\n\
coming from different files\n\
\n\
___________________________________________________________\n\
Append filename to histo names\n\
\n\
With this option histograms get unique names when displayed\n\
In this way one may display the same histograms residing in\n\
memory several times\n\
\n\
____________________________________________________________\n\
Append Timestamp to histo names\n\
\n\
With this option histograms get unique names when displayed\n\
In this way one may display the same histograms residing in\n\
memory several times\n\
\n\
__________________________________________________________\n\
Prepend filename to histo titles\n\
\n\
This is useful when superimposing, stacking or showing histograms\n\
in one canvas with same titles from files e.g. from different runs\n\
since the title is used in legends.\n\
\n\
____________________________________________________________\n\
Remember Expand settings (Marks) \n\
\n\
Using marks expanded parts of hists may be shown. This\n\
option allows to remember these settings in later sessions.\n\
____________________________________________________________\n\
Remember Zoomings (by left mouse)\n\
\n\
Pressing the left mouse button in the scale of a histogram\n\
and dragging to the required limit allows to zoom\n\
in the picture. This  option allows to remember these\n\
settings in later sessions.\n\
\n\
____________________________________________________________\n\
Remember position of StatBox, LegendBox\n\
\n\
Position / size of StatBox, LegendBox can be adjusted from the\n\
menu \"Graphic_defaults\". Using there the command \n\
\"Save as global default\" make these settings persistent\n\
across sessions.\n\
The size will be adjusted according to the number of lines\n\
in the box, e.g. if only name and number of entries of a\n\
histogram should be displayed the box is smaller than\n\
if in addition mean and sigma are to be displayed.\n\
Movements of the boxes by the mouse are not remembered.\n\
\n\
If this option is activated position of the boxes\n\
are remembered when moved with the mouse\n\
___________________________________________________________\n\
fMaxListEntries: Max Entries in Lists\n\
\n\
Too many entries in histlists (default 333)  may crash X \n\
on some systems\n\
With this option one can limit this number\n\
Only the first fMaxListEntries are shown\n\
\n\
____________________________________________________________\n\
fMaxFileListEntries: Max Entries in File Lists\n\
\n\
Too many entries in filelists (default 333)  may crash X \n\
on some systems\n\
With this option one can limit this number\n\
Only the first fMaxFileListEntries are shown\n\
Setting fSkipFirst allows to access any part of the\n\
complete list.\n\
Sorting is done b e f o r e the cut of the list is applied.\n\
\n\
__________________________________________________________\n\
fSkipFirst: Skip first Entries in Lists\n\
\n\
If the above fMaxFileListEntries are exceeded this value \n\
allows to skip the first fSkipFirst entires.\n\
___________________________________________________________\n\
\n\
Show lists only\n\
\n\
If very many histograms are used this option allows\n\
to display user defined lists of histograms only.\n\
\n\
___________________________________________________________\n\
\n\
Show PS file after creation\n\
\n\
Automatically invoke ghostview when a PostScript file is\n\
generated.\n\
\n\
___________________________________________________________\n\
Use Attribute Macro \n\
\n\
With this option activ each time after a histogram is\n\
displayed a macro (FH_setdefaults.C) is executed.\n\
____________________________________________________________\n\
";
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
   fRow_lab = new TList();

   Int_t ind = 0;
   static Int_t dummy;
	GetCustomStyles();
	TObject *obj;
   TString style_menu ("ComboSelect_         Set / Apply global style");
   TIter next(gROOT->GetListOfStyles());
	while ( (obj = next()) ) {
	   style_menu += ";";
	   style_menu += ((TStyle*)obj)->GetName();
	}
	TString rescmd("RestoreSavedSettings()");
	TString mskcmd("SetSelectionMask()");
   RestoreDefaults();
   fRow_lab->Add(new TObjString("CheckButton_              Force current style"));
   fValp[ind++] = &fForceStyle;
   fRow_lab->Add(new TObjString(style_menu));
   fGlobalStyleButton = ind;
   fValp[ind++] = &fGlobalStyle;
	fRow_lab->Add(new TObjString("CommandButt_Apply saved settings"));
   fValp[ind++] = &rescmd;

   fRow_lab->Add(new TObjString("CommentOnly_    How to modify names in display    "));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("CheckButton_  Append Timestamp to histo names"));
   fValp[ind++] = &fAppendTimestampToHistname;
   fRow_lab->Add(new TObjString("CheckButton_  Prepend filename to histo names"));
   fValp[ind++] = &fPrependFilenameToName;
   fRow_lab->Add(new TObjString("CheckButton_ Prepend filename to histo titles"));
   fValp[ind++] = &fPrependFilenameToTitle;
   
   fRow_lab->Add(new TObjString("CommentOnly_    What to remember across sessions   "));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("CheckButton_ Remember Expand settings (Marks)"));
   fValp[ind++] = &fRememberLastSet;
   fRow_lab->Add(new TObjString("CheckButton_Remember Zoomings (by left mouse)"));
   fValp[ind++] = &fRememberZoom;
   fRow_lab->Add(new TObjString("CheckButton_             Remember axis titles"));
   fValp[ind++] = &fRememberTitle;
   fRow_lab->Add(new TObjString("CheckButton_     Remember position of StatBox"));
   fValp[ind++] = &fRememberStatBox;
	fRow_lab->Add(new TObjString("CheckButton_   Remember position of LegendBox"));
	fValp[ind++] = &fRememberLegendBox;
   fRow_lab->Add(new TObjString("CommentOnly_     How to limit length of lists     "));
   fValp[ind++] = &dummy;
	fRow_lab->Add(new TObjString("CheckButton_              Show histlists only"));
	fValp[ind++] = &fShowListsOnly;
//   fRow_lab->Add(new TObjString("CheckButton_    Use Regular expression syntax"));
//   fValp[ind++] = &fUseRegexp;
   fRow_lab->Add(new TObjString("PlainIntVal_        Max Entries in Hist Lists"));
   fValp[ind++] = &fMaxListEntries;
   fRow_lab->Add(new TObjString("PlainIntVal_        Max Entries in File Lists"));
   fValp[ind++] = &fMaxFileListEntries;
   fRow_lab->Add(new TObjString("PlainIntVal_ Skip first Entries in File Lists"));
   fValp[ind++] = &fSkipFirst;
	fRow_lab->Add(new TObjString("CommandButt_Set File/Histgram selection masks"));
   fValp[ind++] = &mskcmd;
/*
	fRow_lab->Add(new TObjString("CheckButton_ Adjust min Y to min cont, (1dim)"));
   fAdjustMinYButton = ind;
	fValp[ind++] = &fAdjustMinY;
*/
   fRow_lab->Add(new TObjString("CommentOnly_   Parameters for 2-dim histograms"));
   fValp[ind++] = &dummy;
	fRow_lab->Add(new TObjString("PlainIntVal_            Fit2Dim Content Limit"));
	fValp[ind++] = &fContentLowLimit;
	fRow_lab->Add(new TObjString("PlainIntVal_    Fit2Dim Vertical Adjust Limit"));
	fValp[ind++] = &fVertAdjustLimit;
	fRow_lab->Add(new TObjString("CheckButton_Dont draw cells with 0 cont(LEGO)"));
	fValp[ind++] = &fLegoSuppressZero;

   fRow_lab->Add(new TObjString("CommentOnly_       Miscellaneous       "));
   fValp[ind++] = &dummy;
	fRow_lab->Add(new TObjString("CheckButton_        Suppress warning messages"));
   fValp[ind++] = &fSuppressWarnings;
   fRow_lab->Add(new TObjString("CheckButton_      Show PS file after creation"));
   fValp[ind++] = &fShowPSFile;
	fRow_lab->Add(new TObjString("CheckButton_              Use Attribute Macro"));
   fValp[ind++] = &fUseAttributeMacro;
/*
   fRow_lab->Add(new TObjString("CommentOnly_Option for stacked hists"));
   fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("CheckButton_     Apply scale to stacked hists"));
	fValp[ind++] = &fScaleStack;
	fRow_lab->Add(new TObjString("RadioButton_ Really stack"));
	fValp[ind++] = &fStackedReally;
   fRow_lab->Add(new TObjString("RadioButton+  Superimpose"));
   fValp[ind++] = &fStackedNostack;
   fRow_lab->Add(new TObjString("RadioButton+Separate Pads"));
   fValp[ind++] = &fStackedPads;
*/
   static Int_t ok;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

GeneralAttDialog::~GeneralAttDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________
void GeneralAttDialog::GetCustomStyles()
{
	if (!gSystem->AccessPathName("hpr_custom_styles.root", kFileExists)) {
	   TFile *cstyle = new TFile("hpr_custom_styles.root");
		TIter n1(gDirectory->GetListOfKeys());
		TKey* key;
		while( (key = (TKey*)n1()) ){
		   if(!strncmp(key->GetClassName(),"TStyle",6)) {
		      TStyle *sty = (TStyle*)cstyle->Get(key->GetName());
//				cout << "key->GetName() " << key->GetName()<< endl;
				if (!gROOT->GetListOfStyles()->FindObject(key->GetName()))
			      gROOT->GetListOfStyles()->Add(sty);
		   }
		}
		cstyle->Close();
	}
}
//_______________________________________________________________________

void GeneralAttDialog::RestoreSavedSettings()
{
	fGlobalStyle = "Plain";
	gROOT->SetStyle(fGlobalStyle);
   TEnv env(".hprrc");
   env.SetValue("GeneralAttDialog.fGlobalStyle",       fGlobalStyle);
	env.SaveLevel(kEnvLocal);
	fDialog->SetComboSelect(1,0);
	gHpr->RestoreOptions();
	cout << setmagenta << "Saved settings have been restored. "
		<< "If you want to keep this mode for later sessions "
		<< "change style to \"Plain\"" << setblack<< endl;
}
//_______________________________________________________________________

void GeneralAttDialog::CloseDialog()
{
//   cout << "GeneralAttDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab ) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void GeneralAttDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("GeneralAttDialog.fGlobalStyle",       fGlobalStyle);
   env.SetValue("GeneralAttDialog.fForceStyle",        fForceStyle);
   env.SetValue("GeneralAttDialog.fShowPSFile",        fShowPSFile);
   env.SetValue("GeneralAttDialog.fSuppressWarnings",  fSuppressWarnings);
   env.SetValue("GeneralAttDialog.fUseRegexp",         fUseRegexp);
   env.SetValue("GeneralAttDialog.fShowListsOnly",     fShowListsOnly);
   env.SetValue("GeneralAttDialog.fRememberLastSet",   fRememberLastSet);
   env.SetValue("GeneralAttDialog.fRememberZoom",      fRememberZoom);
   env.SetValue("GeneralAttDialog.fRememberTitle",     fRememberTitle);
	env.SetValue("GeneralAttDialog.fRememberStatBox",   fRememberStatBox);
	env.SetValue("GeneralAttDialog.fRememberLegendBox", fRememberLegendBox);
	env.SetValue("GeneralAttDialog.fUseAttributeMacro", fUseAttributeMacro);
   env.SetValue("GeneralAttDialog.fMaxListEntries",    fMaxListEntries);
   env.SetValue("GeneralAttDialog.fMaxFileListEntries",fMaxFileListEntries);
   env.SetValue("GeneralAttDialog.fSkipFirst",         fSkipFirst);
   env.SetValue("GeneralAttDialog.fVertAdjustLimit",   fVertAdjustLimit);
   env.SetValue("GeneralAttDialog.fAdjustMinY",        fAdjustMinY);
   env.SetValue("GeneralAttDialog.fContentLowLimit",   fContentLowLimit);
   env.SetValue("GeneralAttDialog.fStackedReally" ,    fStackedReally );
	env.SetValue("GeneralAttDialog.fScaleStack" ,       fScaleStack );
	env.SetValue("GeneralAttDialog.fStackedNostack",    fStackedNostack);
   env.SetValue("GeneralAttDialog.fStackedPads"   ,    fStackedPads   );
	env.SetValue("GeneralAttDialog.fPrependFilenameToTitle", fPrependFilenameToTitle);
	env.SetValue("GeneralAttDialog.fPrependFilenameToName",  fPrependFilenameToName);
	env.SetValue("GeneralAttDialog.fAppendTimestampToHistname", fAppendTimestampToHistname );
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void GeneralAttDialog::RestoreDefaults()
{
	GetCustomStyles();
   TEnv env(".hprrc");
   fGlobalStyle = env.GetValue("GeneralAttDialog.fGlobalStyle", "Plain");
   fForceStyle = env.GetValue("GeneralAttDialog.fForceStyle", 1);
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
   fShowPSFile = env.GetValue("GeneralAttDialog.fShowPSFile", 1);
   fSuppressWarnings = env.GetValue("GeneralAttDialog.fSuppressWarnings", 1);
   if (fSuppressWarnings) gErrorIgnoreLevel=kError;
   else                   gErrorIgnoreLevel=kWarning;
   fUseRegexp = env.GetValue("GeneralAttDialog.fUseRegexp", 0);
   fShowListsOnly = env.GetValue("GeneralAttDialog.fShowListsOnly", 0);
   fRememberLastSet = env.GetValue("GeneralAttDialog.fRememberLastSet", 1);
   fRememberZoom = env.GetValue("GeneralAttDialog.fRememberZoom", 1);
   fRememberTitle = env.GetValue("GeneralAttDialog.fRememberTitle", 0);
	fRememberStatBox = env.GetValue("GeneralAttDialog.fRememberStatBox", (Int_t)0);
	fRememberLegendBox = env.GetValue("GeneralAttDialog.fRememberLegendBox", 1);
	fUseAttributeMacro = env.GetValue("GeneralAttDialog.fUseAttributeMacro", 0);
	fMaxListEntries = env.GetValue("GeneralAttDialog.fMaxListEntries", 333);
	if ( fMaxListEntries <= 0 )
		fMaxListEntries = 333;
   fSkipFirst = env.GetValue("GeneralAttDialog.fSkipFirst", 0);
	if ( fSkipFirst <= 0 )
		fSkipFirst = 0;
	fMaxFileListEntries = env.GetValue("GeneralAttDialog.fMaxFileListEntries", 333);
	if ( fMaxFileListEntries <= 0 )
		fMaxFileListEntries = 333;
   fVertAdjustLimit = env.GetValue("GeneralAttDialog.fVertAdjustLimit", 0);
   fAdjustMinY      = env.GetValue("GeneralAttDialog.fAdjustMinY",      1);
   fContentLowLimit = env.GetValue("GeneralAttDialog.fContentLowLimit", 0);
   fScaleStack      = env.GetValue("GeneralAttDialog.fScaleStack" ,     0);
	fStackedReally   = env.GetValue("GeneralAttDialog.fStackedReally" , 1);
	fStackedNostack  = env.GetValue("GeneralAttDialog.fStackedNostack", 0);
   fStackedPads     = env.GetValue("GeneralAttDialog.fStackedPads"   , 0);
	fPrependFilenameToTitle = env.GetValue("GeneralAttDialog.fPrependFilenameToTitle"   , 0);
	fPrependFilenameToName = env.GetValue("GeneralAttDialog.fPrependFilenameToName", 1);
	fAppendTimestampToHistname = env.GetValue("GeneralAttDialog.fAppendTimestampToHistname", 0);

	if (fAdjustMinY == 0 ) {
		cout << setred
		<< "Warning: Y scale of 1dim hists is forced to 0 (or smaller)"<< endl
		<< "         Be careful when setting log Y scale,"<< endl
		<< "         dont use popup menu with right mouse in canvas"<< endl
		<< "         to set log Y" << setblack << endl;
	}
	// if it is user defined style apply it now
	// it overwrites saved settings
	if (fGlobalStyle != "Plain" && fGlobalStyle != "Default" ) {
		cout << setmagenta << "Set style to: " << fGlobalStyle << endl;
		cout << "Saved settings are overwritten" << setblack<< endl;
		gROOT->SetStyle(fGlobalStyle);
	}
	gStyle->SetPalette(1,NULL);
	// make sure text precission is 2 (not 3)
	Int_t tp = gStyle->GetTextFont();
	if ( tp % 10 != 2 ) {
		cout << "Resetting text font from " << tp;
		tp = tp % 10 + 2;
		gStyle->SetTextFont(tp);
		cout << " to  " << tp << endl;
	}
}
//______________________________________________________________________

void GeneralAttDialog::SetSelectionMask()
{
	gHpr->GetFileSelMask();
}

//______________________________________________________________________

void GeneralAttDialog::CloseDown(Int_t wid)
{
   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void GeneralAttDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
   if (bid == fGlobalStyleButton) {
	   cout << "Set style to: " << fGlobalStyle << endl;
		gROOT->SetStyle(fGlobalStyle);
		gStyle->SetPalette(1,NULL);
		Int_t tp = gStyle->GetTextFont();
		if ( tp % 10 != 2 ) {
			cout << "Resetting text font from " << tp;
			tp = tp % 10 + 2;
			gStyle->SetTextFont(tp);
			cout << " to  " << tp << endl;
		}
   }
   if (bid == fAdjustMinYButton && fAdjustMinY == 0 ) {
		cout << setred
		<< "Warning: Y scale of 1dim hists is forced to 0 (or smaller)"<< endl
		<< "         Be careful when setting log Y scale,"<< endl
		<< "         dont use popup menu with right mouse in canvas"<< endl
		<< "         to set log Y" << setblack << endl;
	}
   if (fForceStyle) gROOT->ForceStyle(kTRUE);
   else             gROOT->ForceStyle(kFALSE);
   if (fSuppressWarnings) gErrorIgnoreLevel=kError;
   else                   gErrorIgnoreLevel=kWarning;
}


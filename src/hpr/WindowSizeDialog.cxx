#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "WindowSizeDialog.h"
#include "hprbase.h"
#include <iostream>

namespace std {} using namespace std;
Int_t WindowSizeDialog::fMainWidth;
Int_t WindowSizeDialog::fNwindows;
Int_t WindowSizeDialog::fWintopx;     // origin of window
Int_t WindowSizeDialog::fWintopy;
Int_t WindowSizeDialog::fWincury;
Int_t WindowSizeDialog::fWincurx;
Int_t WindowSizeDialog::fWinwidx_2dim;
Int_t WindowSizeDialog::fWinwidy_2dim;
Int_t WindowSizeDialog::fWinwidx_1dim;
Int_t WindowSizeDialog::fWinwidy_1dim;
Int_t WindowSizeDialog::fWinshiftx;
Int_t WindowSizeDialog::fWinshifty;
Int_t WindowSizeDialog::fWinwidx_hlist;
Int_t WindowSizeDialog::fEntryHeight;
Double_t WindowSizeDialog::fProjectBothRatio;
//___________________________________________________________________

WindowSizeDialog::WindowSizeDialog(TGWindow * win)
{
// *INDENT-OFF*
   static const char helptext[] =
"\n\
This menu controls default window sizes:\n\
Window sizes and offsets are measured in pixels\n\
Top left is (0, 0)\n\
\n\
Main Window Width:\n\
This defines the width of the main control window \n\
and windows for file and hist lists, default 250.\n\
\n\
Lists Width:\n\
This gives additional control for lists:\n\
> 0: Take value as absolute width (pixels)\n\
= 0: Take default width (see above)\n\
< 0: Adjust width to fit names without scrollbars\n\
\n\
Text size in lists:\n\
Height of text fields in file and hist lists\n\
\n\
Window_X_Width_1dim etc.:\n\
Widths for 1 and 2-dim Histograms\n\
\n\
Window_Shift_X, Y:\n\
Each new window is staggered ny theses values\n\
\n\
Window_Top_X, Y\n\
Position of first window (after Close Windows)\n\
\n\
Project_Both_Ratio\n\
When projectecting 2-dim histograms on both axis\n\
the 2-dim histogram takes this amount of the space\n\
available\n\
\n\
X- Y Margin in ShowSelected\n\
This controls the space between the pictures when\n\
multiple histograms are shown in one canvas.\n\
_____________________________________________________\n\
\n\
";
// *INDENT-ON*

	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
   fRow_lab = new TList();
   RestoreDefaults();

   Int_t ind = 0;
//   static Int_t dummy;
//   static TString stycmd("SetWindowSizePermLocal()");
   fRow_lab->Add(new TObjString("PlainIntVal_Main Window Width"));
   fRow_lab->Add(new TObjString("PlainIntVal+Lists Width"));
   fRow_lab->Add(new TObjString("PlainIntVal_Text size in lists"));
//   fRow_lab->Add(new TObjString("CheckButton+Fit text"));
//   fRow_lab->Add(new TObjString("CheckButton+Fixed width"));
   fRow_lab->Add(new TObjString("PlainIntVal_X_Width_1dim"));
   fRow_lab->Add(new TObjString("PlainIntVal+Y_Width_1dim"));
   fRow_lab->Add(new TObjString("PlainIntVal_X_Width_2dim"));
   fRow_lab->Add(new TObjString("PlainIntVal+Y_Width_2dim"));
   fRow_lab->Add(new TObjString("PlainIntVal_Shift_X     "));
   fRow_lab->Add(new TObjString("PlainIntVal+Shift_Y     "));
   fRow_lab->Add(new TObjString("PlainIntVal_Start Top_X "));
   fRow_lab->Add(new TObjString("PlainIntVal+Start Top_Y "));
   fRow_lab->Add(new TObjString("DoubleValue_Project_Both_Ratio"));

   fValp[ind++] = &fMainWidth;
   fValp[ind++] = &fWinwidx_hlist;
   fValp[ind++] = &fEntryHeight;
//   fValp[ind++] = &fFittext;
//   fValp[ind++] = &fUsecustom;
   fValp[ind++] = &fWinwidx_1dim;
   fValp[ind++] = &fWinwidy_1dim;
   fValp[ind++] = &fWinwidx_2dim;
   fValp[ind++] = &fWinwidy_2dim;
   fValp[ind++] = &fWinshiftx;
   fValp[ind++] = &fWinshifty;
   fValp[ind++] = &fWintopx;
   fValp[ind++] = &fWintopy;
   fValp[ind++] = &fProjectBothRatio;

   static Int_t ok;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

WindowSizeDialog::~WindowSizeDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//___________________________________________________________________

void WindowSizeDialog::CloseDialog()
{
//   cout << "WindowSizeDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//_______________________________________________________________________

void WindowSizeDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("WindowSizeDialog.fMainWidth", fMainWidth);
   env.SetValue("WindowSizeDialog.fWintopx", fWintopx);
   env.SetValue("WindowSizeDialog.fWintopy", fWintopy);
   env.SetValue("WindowSizeDialog.fWinwidx_2dim", fWinwidx_2dim);
   env.SetValue("WindowSizeDialog.fWinwidy_2dim", fWinwidy_2dim);
   env.SetValue("WindowSizeDialog.fWinwidx_1dim", fWinwidx_1dim);
   env.SetValue("WindowSizeDialog.fWinwidy_1dim", fWinwidy_1dim);
   env.SetValue("WindowSizeDialog.fWinshiftx", fWinshiftx);
   env.SetValue("WindowSizeDialog.fWinshifty", fWinshifty);
   env.SetValue("WindowSizeDialog.fWinwidx_hlist", fWinwidx_hlist);
   env.SetValue("WindowSizeDialog.fEntryHeight", fEntryHeight);
//   env.SetValue("WindowSizeDialog.fFittext", fFittext);
   env.SetValue("WindowSizeDialog.fProjectBothRatio", fProjectBothRatio);
   env.SaveLevel(kEnvLocal);
}
//___________________________________________________________________

void WindowSizeDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fMainWidth = env.GetValue("WindowSizeDialog.fMainWidth", 250);
   fWintopx = env.GetValue("WindowSizeDialog.fWintopx", 515);
   fWintopy = env.GetValue("WindowSizeDialog.fWintopy", 5);
   fWincurx = fWintopx;
   fWincury = fWintopy;
   fWinwidx_2dim = env.GetValue("WindowSizeDialog.fWinwidx_2dim", 750);
   fWinwidy_2dim = env.GetValue("WindowSizeDialog.fWinwidy_2dim", 750);
   fWinwidx_1dim = env.GetValue("WindowSizeDialog.fWinwidx_1dim", 750);
   fWinwidy_1dim = env.GetValue("WindowSizeDialog.fWinwidy_1dim", 550);
   fWinshiftx = env.GetValue("WindowSizeDialog.fWinshiftx", 30);
   fWinshifty = env.GetValue("WindowSizeDialog.fWinshifty", 30);
   fWinwidx_hlist = env.GetValue("WindowSizeDialog.fWinwidx_hlist", 0);
   fEntryHeight  = env.GetValue("WindowSizeDialog.fEntryHeight", 24);
//   fFittext = env.GetValue("WindowSizeDialog.fFittext", 1);
   fProjectBothRatio = env.GetValue("WindowSizeDialog.fProjectBothRatio", 0.5);
}
//___________________________________________________________________

void WindowSizeDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void WindowSizeDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
 //  SetWindowSizeNow(canvas);
}


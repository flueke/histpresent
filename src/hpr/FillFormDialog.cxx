
#include "TROOT.h"
#include "TEnv.h"
#include "TLatex.h"
#include "TRootCanvas.h"
#include "TSystem.h"
#include "TGMsgBox.h"
#include "TGClient.h"
#include <TVirtualX.h>
#include "GEdit.h"
#include "HTCanvas.h"
#include "HistPresent.h"
#include "FillFormDialog.h"
#include "TGMrbInputDialog.h"
#include "hprbase.h"
#include <fstream>
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(FillFormDialog)//	fWindow = NULL;


FillFormDialog::FillFormDialog()
{
static const Char_t helptext[] =
"Fill in a form given as  gif, jpg, png pictures.\n\
The real drawing is done by ImageMagick. This program\n\
just prepares commands for ImageMagick to insert the text\n\
into the input file typically generated from a scan.\n\
Widths, offsets are in units (pixels) of the inserted\n\
picture. To match positions in the form and input by\n\
the mouse pointer the canvas used has the dimensions\n\
in pixels (or an integer fraction) of the input file.\n\
\n\
The generated script may be edited to adjust text colors,\n\
fonts and sizes according to ImageMagicks capabilities.\n\
This method is used to use the full resolution of the\n\
scanned picture and not only the screen resolution\n\
when generating printed output.\n\
";
	fWindow = gHpr->GetRootCanvas();
	Int_t ind = 0;
	fBidOpen = -1;
	fOutputWritten = 0;
	RestoreDefaults();
	ComposeFileNames();
	static TString open_cmd("OpenInput()");
	static TString writepdf_cmd("WriteOutput()");
	static TString edit_cmd("EditCommandFile()");

	fRow_lab = new TList();
	fRow_lab->Add(new TObjString("FileRequest_Input file (.png, .jpg)"));
	fValp[ind++] = &fInputName;
	fRow_lab->Add(new TObjString("FileRequest_Output file (.pdf)"));
	fValp[ind++] = &fOutputName;
	fRow_lab->Add(new TObjString("FileRequest_Command file (.sh)"));
	fValp[ind++] = &fCommandName;
	fRow_lab->Add(new TObjString("CommandButt_Open Input"));
	fBidOpen = ind;
	fValp[ind++] = &open_cmd;
	fRow_lab->Add(new TObjString("CommandButt_Show generated Commands"));
	fValp[ind++] = &edit_cmd;
	 fRow_lab->Add(new TObjString("CommandButt_Write Output"));
	fValp[ind++] = &writepdf_cmd;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();

	static Int_t ok;
	fDialog =
		new TGMrbValuesAndText("Fill out a form", NULL, &ok,
			 itemwidth, fWindow, "history.txt", NULL,
			 fRow_lab, fValp, NULL, NULL,
			 helptext, this, this->ClassName());
}
//____________________________________________________________________________

void FillFormDialog::OpenInput()
{
	
	fImage = TImage::Open(fInputName.Data());
	if (!fImage->IsValid()) {
		cout << "Could not create an image... exit" << endl;
		return;
	}
	fImageWidth = (Double_t )fImage->GetWidth();
	fImageHeight= (Double_t )fImage->GetHeight();
	Int_t displayWidth  = fImageWidth;
	Int_t displayHeight = fImageHeight;
	UInt_t ww, wh;
	Int_t screen_x, screen_y;
//  screen size in pixels
	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
				  screen_x, screen_y, ww, wh);
	cout << "Sceen Size " << ww  << "x" << wh;
	fRedFactor = 1;
	while (displayHeight+26 > (Int_t)wh ) {
		displayWidth  = fImageWidth;
		displayHeight = fImageHeight;
		fRedFactor += 1;
		displayHeight /= fRedFactor;
		displayWidth /= fRedFactor;
	}
	// add for top bar and widget decoration
	displayHeight += 26;
	displayWidth += 2;
	cout << " Pict Size: " << fImageWidth << "x" << fImageHeight
		<< " Display: " << displayWidth << "x" << displayHeight<<  endl;
	fHCanvas = new HTCanvas(fBaseName, fInputName, 400, 0, displayWidth, displayHeight);
	fHCanvas->GetCanvasImp()->ShowEditor(kTRUE);
	new GEdit(fHCanvas);
	fHCanvas->Draw();
	fHCanvas->SetRightMargin(0);
	fHCanvas->SetLeftMargin(0);
	fHCanvas->SetTopMargin(0);
	fHCanvas->SetBottomMargin(0);
	fHCanvas->Range(0, 0, fImageWidth, fImageHeight); 
	fImage->Draw();
	gPad->Modified();
	gPad->Update();
};
//____________________________________________________________________________

void FillFormDialog::WriteOutput()
{
	// search canvas for Latex objects and generate commands for Imagemagic
	ofstream cmd_file(fCommandName);
	cmd_file << "#!/bin/bash" << endl;
	cmd_file << "convert -font Helvetica  -pointsize 40 \\" << endl;
	cmd_file << "-stroke blue -strokewidth 1 \\" << endl;
	TObject *obj = NULL; 
	TText *l;
	TIter next(fHCanvas->GetListOfPrimitives());
	while ( (obj = next()) ) {
//		cout << "obj " << obj->ClassName() << endl;
		if (obj->InheritsFrom("TLatex") ) {
			l = (TLatex*)obj;
			Int_t ix = (Int_t)(l->GetX() * fImageWidth);
			Int_t iy = (Int_t)((1.-l->GetY()) * fImageHeight);
			cout << ix << ", " << iy << " \"" << l->GetTitle() << "\"" << endl;
			cmd_file << "-draw \'text " << ix << ", " << iy << " \""
						<< l->GetTitle() << "\"\' \\" << endl;
			cout << "Font " <<l->GetTextFont()  << " Size " <<l->GetTextSize()  
			<< " Color " << l->GetTextColor() << endl;
		}
	}
	cmd_file << fInputName << " " << fOutputName << endl;
	fOutputWritten++;
}
//____________________________________________________________________________

void FillFormDialog::EditCommandFile()
{
	if ( fOutputWritten <= 0) {
		cout << "No output written yet" << endl;
		return;
	}
	TString cmd("cat ");
	cmd += fCommandName;
	gSystem->Exec(cmd);
}
//____________________________________________________________________________

void FillFormDialog::ComposeFileNames()
{
	fBaseName = fInputName;
	if ( fBaseName.Index('.') > 0 ) {
		fBaseName.Resize(fBaseName.Index('.'));
	}
//	cout << "ComposeFileNames() " << fBaseName << endl;
	fOutputName = fBaseName;
	fOutputName += ".pdf";
	fCommandName = fBaseName;
	fCommandName += "_cmd.sh";
}
//____________________________________________________________________________

void FillFormDialog::SaveDefaults()
{
//   cout << "FillFormDialog::InsertFunction::SaveDefaults()" << endl;
	TEnv env(".hprrc");
	env.SetValue("FillFormDialog.fInputName",     fInputName);
//	env.SetValue("FillFormDialog.fOutputName",   fOutputName);
//	env.SetValue("FillFormDialog.fCommandName", fCommandName);
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void FillFormDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fInputName   = env.GetValue("FillFormDialog.fInputName",   "os1.png");
//	fOutputName  = env.GetValue("FillFormDialog.fOutputName",  "");
//	fCommandName = env.GetValue("FillFormDialog.fCommandName", "");
}
//_________________________________________________________________________

FillFormDialog::~FillFormDialog()
{
	if (fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void FillFormDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
//	cout << "CRButtonPressed: " << bid <<"  " << fBidOpen<< endl;
	if (bid == fBidOpen) {
		ComposeFileNames(); 
		fDialog->ReloadValues();
		fDialog->DoNeedRedraw();
	}
};
//_______________________________________________________________________

void FillFormDialog::CloseDialog()
{
//   cout << "FillFormDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void FillFormDialog::CloseDown(Int_t wid)
{
//   cout << "FillFormDialog::CloseDown()" << endl;
	if (wid != -2) SaveDefaults();
	delete this;
}


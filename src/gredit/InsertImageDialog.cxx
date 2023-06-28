
#include "TROOT.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TGMsgBox.h"
#include "GEdit.h"
#include "HTPad.h"
#include "InsertImageDialog.h"
#include "HprImage.h"
#include "TGMrbInputDialog.h"
#include "HprRaRegion.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertImageDialog)

InsertImageDialog::InsertImageDialog()
{
static const Char_t helptext[] =
"Insert and manipulate gif, jpg, png pictures\n\
Widths, offsets are in units (pixels) of the inserted picture\n\
Use middle mouse button to select a pad\n\
\n\
Merge Modes:\n\
add            - color addition with saturation \n\
alphablend     - alpha-blending\n\
allanon        - color values averaging\n\
colorize       - hue and saturate bottom image same as top image\n\
darken         - use lowest color value from both images\n\
diff           - use absolute value of the color difference between two images\n\
dissipate      - randomly alpha-blend images\n\
hue            - hue bottom image same as top image\n\
lighten        - use highest color value from both images\n\
overlay        - some wierd image overlaying(see GIMP)\n\
saturate       - saturate bottom image same as top image\n\
screen         - another wierd image overlaying(see GIMP)\n\
sub            - color substraction with saturation\n\
tint           - tinting image with image\n\
value          - value bottom image same as top image\n\
";
   fCanvas = gPad->GetCanvas();
   fWindow = NULL;
   if (fCanvas) {
      fWindow = (TRootCanvas*)fCanvas->GetCanvasImp();
		fWindow->Connect("CloseWindow()", "InsertImageDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
   fRow_lab->Add(new TObjString("PlainIntVal_Offset X"));
   fValp[ind++] = &fOffset_x;
   fRow_lab->Add(new TObjString("PlainIntVal+Offset Y"));
   fValp[ind++] = &fOffset_y;
   fRow_lab->Add(new TObjString("PlainIntVal_Width X"));
   fValp[ind++] = &fWidth_x;
   fRow_lab->Add(new TObjString("PlainIntVal+Width Y"));
   fValp[ind++] = &fWidth_y;
   fRow_lab->Add(new TObjString("DoubleValue_Blur X"));
   fValp[ind++] = &fBlur_x;
   fRow_lab->Add(new TObjString("DoubleValue+Blur Y"));
   fValp[ind++] = &fBlur_y;
   fRow_lab->Add(new TObjString("RadioButton_Keep pad height"));
   fValp[ind++] = &fFix_h;
   fRow_lab->Add(new TObjString("RadioButton+Keep pad width"));
   fValp[ind++] = &fFix_w;
   fRow_lab->Add(new TObjString("RadioButton_Keep pad wi/he "));
   fValp[ind++] = &fFix_wh;
   fRow_lab->Add(new TObjString("RadioButton+Use Image size"));
   fValp[ind++] = &fFix_no;
   fRow_lab->Add(new TObjString("CheckButton_Use entire pad"));
   fValp[ind++] = &fEntirePad;
   fRow_lab->Add(new TObjString("ComboSelect+Merge Mode;add;allanon;colorize;darken;diff;dissipate;hue;lighten;overlay;saturate;screen;sub;tint;value"));
   fValp[ind++] = &fMergeMode;
   fMergeMode = "allanon";
   fRow_lab->Add(new TObjString("FileRequest_Imagefile"));
   fValp[ind++] = &fPname;
//   TImage *img = TImage::Open(name.Data());
   static TString execute_cmd("ExecuteInsert()");
   fRow_lab->Add(new TObjString("CommandButt_Insert into Pad"));
   fValp[ind++] = &execute_cmd;
   static TString merge_cmd("ExecuteMerge()");
   fRow_lab->Add(new TObjString("CommandButt+Merge into existing"));
   fValp[ind++] = &merge_cmd;
   static TString crop_cmd("ExecuteCrop()");
   fRow_lab->Add(new TObjString("CommandButt_Crop"));
   fValp[ind++] = &crop_cmd;
   static TString blur_cmd("ExecuteBlur()");
   fRow_lab->Add(new TObjString("CommandButt+Blur"));
   fValp[ind++] = &blur_cmd;
   static TString imwrite_cmd("ImageToFile()");
   fRow_lab->Add(new TObjString("CommandButt_Write Image to File"));
   fValp[ind++] = &imwrite_cmd;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Select image file", NULL, &ok,
          itemwidth, fWindow, NULL, NULL,
          fRow_lab, fValp, NULL, NULL,
          helptext, this, this->ClassName());
}
//____________________________________________________________________________

void InsertImageDialog::ExecuteInsert()
{
   HTPad *pad = NULL;
   HprImage * hprimg = new HprImage(fPname.Data(), pad);
   fImage = hprimg->GetImage();
   if (!fImage) {
      cout << "Could not create an image... exit" << endl;
      return;
   }
   fImageWidth = (Double_t )fImage->GetWidth();
   fImageHeight= (Double_t )fImage->GetHeight();

   if (!fEntirePad) {
      fCanvas->cd();
      pad = GEdit::GetEmptyPad();
//      pad = (HTPad*)gPad;
      if (pad) {
         gROOT->SetSelectedPad(pad);
         Double_t aspect_ratio = 1.;
         if ( fImageWidth > 0 )
   		   aspect_ratio = fImageHeight * fCanvas->GetXsizeReal()
                        		/ (fImageWidth* fCanvas->GetYsizeReal());
         cout << fImageHeight << " " <<  fCanvas->GetXsizeReal() << " "
              << fImageWidth  << " " << fCanvas->GetYsizeReal() << endl;
   		if (fFix_w) {
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetWNDC(),
                  		pad->GetYlowNDC() + pad->GetWNDC() * aspect_ratio);
             cout << "fFix_w " << aspect_ratio << endl;
   		} else if (fFix_h) {
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + pad->GetHNDC() / aspect_ratio,
                  		pad->GetYlowNDC() + pad->GetHNDC());
             cout << "fFix_h " << aspect_ratio << endl;

   		} else if (fFix_no) {
      		pad->SetPad(pad->GetXlowNDC(), pad->GetYlowNDC(),
                  		pad->GetXlowNDC() + fImageWidth / fCanvas->GetWw(),
                  		pad->GetYlowNDC() + fImageHeight / fCanvas->GetWh());
             cout << "fFix_no " << aspect_ratio << endl;
   		}

      } else {
         Int_t retval = 0;
         new TGMsgBox(gClient->GetRoot(), fWindow, "Warning",
             "Please create a new Pad in this Canvas",
             kMBIconExclamation, kMBDismiss, &retval);
         return;
      }
   }
   if (gPad == gPad->GetMother()) {
		Int_t retval = 0;
		new TGMsgBox(gClient->GetRoot(), fWindow, "Warning",
				"Do you really want to use entire canvas?",
				kMBIconQuestion,  kMBYes | kMBNo, &retval);
		if ( retval == kMBNo )return;
   }

   cout << "InsertImage(): " <<  gPad->GetXlowNDC() << " " << gPad->GetYlowNDC() << " "
        <<  gPad->GetWNDC()    << " " << gPad->GetHNDC()    << endl;
   cout << "Image size, X,Y: " << fImageWidth
                        << " " << fImageHeight << endl;
   fImage->SetConstRatio(kTRUE);
   fImage->SetEditable(kTRUE);
   fImage->SetImageQuality(TAttImage::kImgBest);
   TString drawopt;
   if (fOffset_x == 0 && fOffset_y == 0) {
      drawopt= "xxx";
   } else {
      drawopt = "T";
      drawopt += fOffset_x;
      drawopt += ",";
      drawopt += fOffset_x;
      drawopt += ",#ffffff";
   }
//   cout << drawopt << endl;
   hprimg->Draw(drawopt);
   gPad->Modified();
   gPad->Update();
};
//____________________________________________________________________________

TImage * InsertImageDialog::FindActiveImage()
{
   TImage *img = NULL;
   TIter next(gPad->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("HprImage")) {
         img = ((HprImage*)obj)->GetImage();
      }
   }
   if (!img) {
      printf("No base image... exit\n");
      return NULL;
   } else {
      return img;
   }
}
//____________________________________________________________________________

void InsertImageDialog::ImageToFile()
{
   fImage = FindActiveImage();
   if (!fImage) {
      printf("No image in active pad");
      return;
   }
   TString fname = fImage->GetName();
   Int_t last_us = fname.First('.');	// chop off extension

   if (last_us > 0)
      fname.Remove(last_us);
   fname += "_save.gif";
   Bool_t ok;
   fname =
       GetString("Write Image with name", fname.Data(), &ok, fWindow);
   if (!ok) {
      cout << " Cancelled " << endl;
      return;
   }
   if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
//      cout << fname << " exists" << endl;
      int buttons = kMBOk | kMBDismiss, retval = 0;
      EMsgBoxIcon icontype = kMBIconQuestion;
      new TGMsgBox(gClient->GetRoot(), fWindow,
                   "Question", "File exists, overwrite?",
                   icontype, buttons, &retval);
      if (retval == kMBDismiss)
         return;
   }
   fImage->WriteImage(fname);
}
//____________________________________________________________________________

void InsertImageDialog::ExecuteMerge()
{
   fImage = FindActiveImage();
   if (!fImage) {
      printf("No base image");
      return;
   }
   TImage *img = TImage::Open(fPname);
   if (!img) {
      printf("Could not create an image");
      return;
   }

   HprRaRegion *ra = (HprRaRegion*)gPad->GetListOfPrimitives()->FindObject("HprRaRegion");
   if ( ra ) {
      Double_t xr = 1 / gPad->GetX2() - gPad->GetX1();
      Double_t yr = 1 / gPad->GetY2() - gPad->GetY1();
      fOffset_x = (Int_t)(fImageWidth * xr * ra->GetX1());
      fOffset_y = (Int_t)(fImageHeight* yr * (gPad->GetY2() - ra->GetY2()));
      fWidth_x  = (Int_t)(fImageWidth * xr * (ra->GetX2() - ra->GetX1())) ;
      fWidth_y  = (Int_t)(fImageHeight* yr * (ra->GetY2() - ra->GetY1()));
      Double_t aspect_ratio = fImageHeight * fCanvas->GetYsizeReal()
                             / ( fImageWidth * fCanvas->GetXsizeReal());
      if        (fFix_w) {
         img->Scale(fWidth_x, (UInt_t)(fWidth_x * aspect_ratio));
   	} else if (fFix_h) {
         img->Scale((UInt_t)(fWidth_y / aspect_ratio), fWidth_y);
   	} else  if(fFix_wh) {
         img->Scale(fWidth_x, fWidth_y);
      } else {
         fWidth_x  = (Int_t)fImageWidth;
         fWidth_y  = (Int_t)fImageHeight;
      }
      cout << "Merge: " << fMergeMode.Data()<<" fImage->GetWidth() " << fImage->GetWidth() << " fWidth_x " << fWidth_x << " fWidth_y " << fWidth_y << endl;
   }
   fImage->Merge(img, fMergeMode.Data(),fOffset_x, fOffset_y);
   gPad->Modified();
   gPad->Update();
}
//____________________________________________________________________________

void InsertImageDialog::ExecuteBlur()
{
   fImage = FindActiveImage();
   if (!fImage) {
      printf("No image");
      return;
   }
   cout << "Blur: " << fBlur_x << " " <<  fBlur_y<< endl;
   fImage->Blur(fBlur_x, fBlur_y);
   gPad->Modified();
   gPad->Update();
}
//____________________________________________________________________________

void InsertImageDialog::ExecuteCrop()
{
   fImage = FindActiveImage();
   if (!fImage) {
      printf("No image");
      return;
   }
   cout << "Crop: " << fOffset_x<< " " <<  fOffset_y<< " " <<  fWidth_x<< " " <<  fWidth_y << endl;
   fImage->Crop(fOffset_x, fOffset_y, fWidth_x, fWidth_y);
   gPad->Modified();
   gPad->Update();
}
//____________________________________________________________________________

void InsertImageDialog::SaveDefaults()
{
//   cout << "InsertImageDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertImageDialog.fEntirePad", fEntirePad);
   env.SetValue("InsertImageDialog.fFix_h", fFix_h);
   env.SetValue("InsertImageDialog.fFix_w", fFix_w);
   env.SetValue("InsertImageDialog.fFix_wh", fFix_wh);
   env.SetValue("InsertImageDialog.fFix_no", fFix_no);
   env.SetValue("InsertImageDialog.fOffset_x", fOffset_x);
   env.SetValue("InsertImageDialog.fOffset_y", fOffset_y);
   env.SetValue("InsertImageDialog.fWidth_x", fWidth_x);
   env.SetValue("InsertImageDialog.fWidth_y", fWidth_y);
   env.SetValue("InsertImageDialog.fBlur_x", fBlur_x);
   env.SetValue("InsertImageDialog.fBlur_y", fBlur_y);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertImageDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fEntirePad= env.GetValue("InsertImageDialog.fEntirePad", 0);
   fFix_h    = env.GetValue("InsertImageDialog.fFix_h", 0);
   fFix_w    = env.GetValue("InsertImageDialog.fFix_w", 1);
   fFix_wh   = env.GetValue("InsertImageDialog.fFix_wh", 0);
   fFix_no   = env.GetValue("InsertImageDialog.fFix_no", 0);
   fOffset_x = env.GetValue("InsertImageDialog.fOffset_x", 0);
   fOffset_y = env.GetValue("InsertImageDialog.fOffset_y", 0);
   fWidth_x  = env.GetValue("InsertImageDialog.fWidth_x", 100);
   fWidth_y  = env.GetValue("InsertImageDialog.fWidth_y", 100);
   fBlur_x   = env.GetValue("InsertImageDialog.fBlur_x",  3.);
   fBlur_y   = env.GetValue("InsertImageDialog.fBlur_y",  3.);
}
//_________________________________________________________________________

InsertImageDialog::~InsertImageDialog()
{
	if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
}
//_______________________________________________________________________

void InsertImageDialog::CloseDialog()
{
//   cout << "InsertImageDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertImageDialog::CloseDown(Int_t wid)
{
//   cout << "InsertImageDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}


#include "Ascii2HistDialog.h"
#include "TGMrbValuesAndText.h"
#include "HistPresent.h"
#include "FitHist.h"
#include "hprbase.h"

#include "TMath.h"
#include "TEnv.h"
#include "TGFont.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TObjString.h"
#include "TSystem.h"
#include <fstream>

extern Int_t gHprDebug;

extern TString gHprLocalEnv;

//Double_t LocMin(Int_t n, Double_t *x)
//{
//   Double xmin =

ClassImp(Ascii2HistDialog)

Ascii2HistDialog::Ascii2HistDialog( TGWindow * win)
{

static const Char_t helptext[] =
"Read values from ASCII file and fill histogram\n\
Input data can have the formats:\n\
W:          1 dim: Spectrum, Channel Contents only\n\
                   no X values\n\
X:          1 dim: X values to be filled\n\
X, W:       1 dim: X, Weight\n\
X, Y:       2 dim: X, Y values to be filled\n\
X, Y, W:    2 dim: X, Y , Weight \n\
X, Ym Z:    3 dim: X, Y, Z values to be filled\n\
X, Y, Z, W: 3 dim: X, Y ,Z, Weight \n\
\n\
Lines not starting with a number are skipped as comment\n\
(To be exact: not TString::IsFloat())\n\
\n\
For convenience the first N columns of the input data\n\
may be skipped typically if the first value is just a\n\
a serial number.\n\
\n\
Note when filling with weights:\n\
The bin error is set to sqrt(content) as was the\n\
default with root version 5\n\
The default behavior in root 6 is now\n\
bin error = sqrt(sum(weigth**2))\n\
A common error may be applied to all channels\n\
if this value is > 0\n\
\n\
Before the histogram is build the values must be read\n\
by \"Read_Input()\". At this moment the format\n\
should have been selected so meaningful values\n\
for the axis can be calculated if the option\n\
\"Keep limits\" is off.\n\
";

   static void *valp[50];
   Int_t ind = 0;
   fNvalues = 0;
   fCommand = "Draw_The_Hist()";
   fSaveCommand = "WriteoutHist()";
   fReadCommand = "Read_Input()";
   fCommandHead = "Show_Head_of_File()";
   RestoreDefaults();
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("RadioButton_1 Dim,Spectrum, channel cont,no X"));
   row_lab->Add(new TObjString("RadioButton_1 Dim,X values to be filled      "));
   row_lab->Add(new TObjString("RadioButton_1 Dim,X,Weight                   "));
	row_lab->Add(new TObjString("RadioButton_1 Dim,X,Weight, Error            "));
	row_lab->Add(new TObjString("RadioButton_2 Dim,X,Y values to be filled    "));
   row_lab->Add(new TObjString("RadioButton_2 Dim,X,Y, Weight                "));
   row_lab->Add(new TObjString("RadioButton_3 Dim,X,Y,Z values to be filled  "));
   row_lab->Add(new TObjString("RadioButton_3 Dim,X,Y,Z, Weight              "));
	row_lab->Add(new TObjString("PlainIntVal_Skip first n columns, n:         "));
	row_lab->Add(new TObjString("FileRequest_Input datafile"));
   row_lab->Add(new TObjString("StringValue_Name"));
   row_lab->Add(new TObjString("StringValue_Title"));
   row_lab->Add(new TObjString("PlainIntVal_NbX"));
   row_lab->Add(new TObjString("DoubleValue+Xl"));
   row_lab->Add(new TObjString("DoubleValue+Xu"));
   row_lab->Add(new TObjString("PlainIntVal_NbY"));
   row_lab->Add(new TObjString("DoubleValue+Yl"));
   row_lab->Add(new TObjString("DoubleValue+Yu"));
   row_lab->Add(new TObjString("PlainIntVal_NbZ"));
   row_lab->Add(new TObjString("DoubleValue+Zl"));
   row_lab->Add(new TObjString("DoubleValue+Zu"));
   row_lab->Add(new TObjString("CheckButton_Keep limits"));
   row_lab->Add(new TObjString("DoubleValue+Common error"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt+Read_Input"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Histogram"));
   row_lab->Add(new TObjString("CommandButt+Save_Hist_to_File"));

   valp[ind++] = &fSpectrum;
   valp[ind++] = &f1Dim;
   valp[ind++] = &f1DimWithWeight;
	valp[ind++] = &f1DimWithErrors;
	valp[ind++] = &f2Dim;
   valp[ind++] = &f2DimWithWeight;
   valp[ind++] = &f3Dim;
   valp[ind++] = &f3DimWithWeight;
   fBidSelection = ind;
   valp[ind++] = &fNskip;
	valp[ind++] = &fHistFileName;
	valp[ind++] = &fHistName;
   valp[ind++] = &fHistTitle;
   valp[ind++] = &fNbinsX;
   valp[ind++] = &fXlow;
   valp[ind++] = &fXup;
   valp[ind++] = &fNbinsY;
   valp[ind++] = &fYlow;
   valp[ind++] = &fYup;
   valp[ind++] = &fNbinsZ;
   valp[ind++] = &fZlow;
   valp[ind++] = &fZup;
   valp[ind++] = &fKeepLimits;
   valp[ind++] = &fError;
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fReadCommand;
   valp[ind++] = &fCommand;
   valp[ind++] = &fSaveCommand;
   Int_t ok;
//	TGFont *label_font = gClient->GetFont(gEnv->GetValue("Gui.MenuHiFont",
//						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText("Hists parameters", NULL, &ok,itemwidth, win,
                      NULL, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
   fDialog->Move(100,100);

//   Int_t itemwidth = 380;
//   ok = GetStringExt("Hists parameters", NULL, itemwidth, win,
//                   NULL, NULL, row_lab, valp,
//                   NULL, NULL, &helpText[0], this, this->ClassName());
};
//_________________________________________________________________________

Ascii2HistDialog::~Ascii2HistDialog()
{
   SaveDefaults();
};
//______________________________________________________________________

void Ascii2HistDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
	if (bid  < fBidSelection ) {
		GetDim();
		SetDim();
		fDialog->ReloadValues();
	}
}
//_________________________________________________________________________

void Ascii2HistDialog::Read_Input()
{
   fNvalues = 0;
   cout << endl;
   ifstream infile;
   infile.open(fHistFileName.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "Ascii2HistDialog: "
			<< gSystem->GetError() << " - " << fHistFileName.Data()
			<< endl;
		return;
	}
	fXval.Set(100);
	fYval.Set(100);
	fZval.Set(100);
	fWval.Set(100);
	
	Bool_t ok = kTRUE;
   Int_t nval;
   if      (fSpectrum || f1Dim)       nval = 1;
   else if (f1DimWithWeight || f2Dim) nval = 2;
   else if (f2DimWithWeight || f3Dim || f1DimWithErrors) nval = 3;
   else if (f3DimWithWeight)          nval = 4;
   else {
      cout << "Invalid mode: " << endl;
      return;
   }

   TString line;
	TString del(" ,\t, \r");
	TObjArray * oa;
	Int_t nn = 0;
	TString val;
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
		// check for DOS format
		if ( line.EndsWith("\r") ) {
			line.Resize(line.Length() - 1);
		}
		if ( line.BeginsWith("#") || line.BeginsWith("*")
			|| line.BeginsWith("!")|| line.BeginsWith("C") ){
			cout << "Comment: " << line << endl;
			continue;
		} 

		oa = line.Tokenize(del);
		Int_t nent = oa->GetEntries();
		if (nn < 50 && gHprDebug > 0) {
			cout << line << " " << nent << endl;
			nn++;
		}
		if ( nent < 1 )
			continue;
//		val = ((TObjString*)oa->At(0))->String();
//		if ( !val.IsFloat() ) {
//			cout << "Comment: " << line << endl;
//			continue;
//		} 
		if (nval == 1 && nent > 1 )
			continue;
		if (nent < nval+fNskip) {
			cout << "Not enough entries at: " << fNvalues+1 << endl;
			ok = kFALSE;
			continue;
		}
// 		if (nn < 50) cout << " val: ";
		for (Int_t i = fNskip; i < nval+fNskip; i++) {
			val = ((TObjString*)oa->At(i))->String();
			if (nn < 50 && gHprDebug > 0) 
				cout << val << " " ; 
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << fNvalues+1 << endl;
				ok = kFALSE;
				continue;
			}
			
			if      ( i == 0+fNskip ) fXval.AddAt(val.Atof(), fNvalues);
			else if ( i == 1+fNskip ) fYval.AddAt(val.Atof(), fNvalues);
			else if ( i == 2+fNskip ) fZval.AddAt(val.Atof(), fNvalues);
			else if ( i == 3+fNskip ) fWval.AddAt(val.Atof(), fNvalues);
		}
		fNvalues++;
		if (nn < 50 && gHprDebug > 0) 
			cout << endl;
		if (fNvalues >= fXval.GetSize() - 1){
			fXval.Set(fNvalues+100);
			fYval.Set(fNvalues+100);
			fZval.Set(fNvalues+100);
			fWval.Set(fNvalues+100);
		}
	}
	infile.close();
	if ( !ok ) {
		cout << "Illegal input data found" << endl;
//		return;
	}
   cout << "entries " << fNvalues << endl;
	if (fNvalues <= 0)
		return;
   if (fSpectrum) {
      fNbinsX = fNvalues;
      fXlow = 0;
      fXup  = fNvalues;
   } else {
      if (fKeepLimits <= 0) {
//   	   if (f1DimWithWeight || f1DimWithErrors ) fNbinsX = fNvalues;
         fXlow = fXval[TMath::LocMin(fNvalues, fXval.GetArray())];
         fXup  = fXval[TMath::LocMax(fNvalues, fXval.GetArray())];
         Double_t binw2 = 0.5 * (fXup - fXlow) / (Double_t) fNbinsX;
         fXlow -= binw2;
         fXup  += binw2;
         cout << " fXlow " << fXlow  << " fXup " << fXup;
      }
   	if (f2Dim ||  f2DimWithWeight) {
         if (fKeepLimits <= 0) {
      	   fYlow = fYval[TMath::LocMin(fNvalues, fYval.GetArray())];
      	   fYup  = fYval[TMath::LocMax(fNvalues, fYval.GetArray())];
      	   Double_t binw2 = 0.5 * (fYup - fYlow) / (Double_t) fNbinsY;
      	   fYlow -= binw2;
      	   fYup  += binw2;
            cout << " fYlow " << fYlow  << " fYup " << fYup;
         }
    	}
      if (f3Dim || f3DimWithWeight) {
         if (fKeepLimits <= 0) {
      	   fYlow = fYval[TMath::LocMin(fNvalues, fYval.GetArray())];
      	   fYup  = fYval[TMath::LocMax(fNvalues, fYval.GetArray())];
      	   Double_t binw2 = 0.5 * (fYup - fYlow) / (Double_t) fNbinsY;
      	   fYlow -= binw2;
      	   fYup  += binw2;
            cout << " fYlow " << fYlow  << " fYup " << fYup;
      	   fZlow = fZval[TMath::LocMin(fNvalues, fZval.GetArray())];
      	   fZup  = fZval[TMath::LocMax(fNvalues, fZval.GetArray())];
      	   Double_t binw3 = 0.5 * (fZup - fZlow) / (Double_t) fNbinsZ;
      	   fZlow -= binw3;
      	   fZup  += binw3;
            cout << " fZlow " << fZlow  << " fZup " << fZup;
   	   }
   	   if (f3DimWithWeight) {
				cout << "Min val, max val: " << fWval[TMath::LocMin(fNvalues, fWval.GetArray())]
				<< " " << fWval[TMath::LocMax(fNvalues, fWval.GetArray())] << endl;
			}
		}
   }

   cout << endl;
   return;
}
//_________________________________________________________________________

void Ascii2HistDialog::Draw_The_Hist()
{
   if (fNvalues <= 0) {
      cout << "No values, do: Read_Input first" << endl;
      return;
   }
   fHist = NULL;
	if (fSpectrum ||f1Dim || f1DimWithWeight || f1DimWithErrors) {
      TH1D * hist1 = new TH1D(fHistName, fHistTitle, fNbinsX, fXlow, fXup);
      hist1->Sumw2(kTRUE);
      fHist = hist1;
      if (fSpectrum) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->SetBinContent(i+1, fXval[i]);
         }
      } else if (f1Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->Fill(fXval[i]);
         }
      } else if (f1DimWithWeight || f1DimWithErrors) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist1->Fill(fXval[i], fYval[i]);
				if (f1DimWithErrors) {
					Int_t ib = hist1->FindBin(fXval[i]);
					hist1->SetBinError(ib, fZval[i]);
				}
         }
		}
      // case of common error
		if (fError > 0 || f1DimWithWeight) {
         for (Int_t i = 0; i < fNbinsX; i++) {
            if (hist1->GetBinContent(i+1) > 0) {
					if (fError > 0)
						hist1->SetBinError(i+1, fError);
					else
						hist1->SetBinError(i+1, TMath::Sqrt(hist1->GetBinContent(i+1)));
				}
			}
		}
	}
   if (f2Dim || f2DimWithWeight) {
      TH2F * hist2 = new TH2F(fHistName, fHistTitle, fNbinsX, fXlow, fXup,
                                            fNbinsY, fYlow, fYup);
      fHist = hist2;
      if (f2Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist2->Fill(fXval[i], fYval[i]);
         }
      } else if (f2DimWithWeight) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist2->Fill(fXval[i], fYval[i], fZval[i]);
         }
      }
      // case of common error
      if (fError > 0) {
         for (Int_t i = 0; i < fNbinsX; i++) {
            for (Int_t k = 0; k < fNbinsY; k++) {
               if (hist2->GetBinError(i+1, k+1) != 0)
                  hist2->SetBinError(i+1, k+1, fError);
            }
         }
      }
   }
   if (f3Dim || f3DimWithWeight) {
		
      TH3F * hist3 = new TH3F(fHistName, fHistTitle, fNbinsX, fXlow, fXup,
                             fNbinsY, fYlow, fYup, fNbinsZ, fZlow, fZup);
		hist3->Sumw2();
      fHist = hist3;
      if (f3Dim) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist3->Fill(fXval[i], fYval[i], fZval[i]);
         }
      } else if (f3DimWithWeight) {
         for (Int_t i = 0; i < fNvalues; i++) {
            hist3->Fill(fXval[i], fYval[i], fZval[i], fWval[i]);
//				if (fWval[i] != 0)
//					cout << fXval[i]<< " " << fYval[i]  <<" " << fZval[i]<<  "  " <<  fWval[i] << endl;
         }
      }
   }
   if (gHpr) {
		 FitHist *fh = gHpr->ShowHist(fHist);
		 if (fh ) fCanvas = fh->GetCanvas();
   } else {
		fHist->Draw();
	}
   SaveDefaults();
   return;

}
//_________________________________________________________________________

void Ascii2HistDialog::Show_Head_of_File()
{
   TString cmd(fHistFileName.Data());
   cmd.Prepend("head ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2HistDialog::SaveDefaults()
{
	if ( gHprDebug > 0 )
		cout << "Ascii2HistDialog::SaveDefaults() " << endl;
   TEnv env(gHprLocalEnv);
   env.SetValue("Ascii2HistDialog.fHistFileName",   fHistFileName);
   env.SetValue("Ascii2HistDialog.fHistName",		 fHistName);
   env.SetValue("Ascii2HistDialog.fHistTitle",		 fHistTitle);
   env.SetValue("Ascii2HistDialog.fSpectrum",		 fSpectrum);
   env.SetValue("Ascii2HistDialog.f1Dim", 			 f1Dim);
   env.SetValue("Ascii2HistDialog.f1DimWithWeight", f1DimWithWeight);
	env.SetValue("Ascii2HistDialog.f1DimWithErrors", f1DimWithErrors);
	env.SetValue("Ascii2HistDialog.f2Dim", 			 f2Dim);
   env.SetValue("Ascii2HistDialog.f2DimWithWeight", f2DimWithWeight);
   env.SetValue("Ascii2HistDialog.f3Dim", 			 f3Dim);
   env.SetValue("Ascii2HistDialog.f3DimWithWeight", f3DimWithWeight);
	env.SetValue("Ascii2HistDialog.fNskip",  			  fNskip);
	env.SetValue("Ascii2HistDialog.fError",  		     fError);
   env.SetValue("Ascii2HistDialog.fNbinsX",  		  fNbinsX);
   env.SetValue("Ascii2HistDialog.fXlow",  			  fXlow);
   env.SetValue("Ascii2HistDialog.fXup",   			  fXup);
   env.SetValue("Ascii2HistDialog.fNbinsY",  		  fNbinsY);
   env.SetValue("Ascii2HistDialog.fYlow",  			  fYlow);
   env.SetValue("Ascii2HistDialog.fYup",   			  fYup);
   env.SetValue("Ascii2HistDialog.fNbinsZ",  		  fNbinsZ);
   env.SetValue("Ascii2HistDialog.fZlow",  			  fZlow);
   env.SetValue("Ascii2HistDialog.fZup",   			  fZup);
   GetDim();
   if (fDim == 1) {
		env.SetValue("Ascii2HistDialog.f1DimNbinsX",			fNbinsX); 
	} else if (fDim == 2) {
		env.SetValue("Ascii2HistDialog.f2DimNbinsX",			fNbinsX); 
		env.SetValue("Ascii2HistDialog.f2DimNbinsY",			fNbinsY); 
   } else if (fDim == 3) {
		env.SetValue("Ascii2HistDialog.f3DimNbinsX",			fNbinsX); 
		env.SetValue("Ascii2HistDialog.f3DimNbinsY",			fNbinsY);
		env.SetValue("Ascii2HistDialog.fNbinsZ",  			fNbinsZ);
	}

   env.SetValue("Ascii2HistDialog.fKeepLimits",		  fKeepLimits);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Ascii2HistDialog::GetDim()
{
	fDim = 1;
	if (f2Dim != 0 || f2DimWithWeight != 0) {
		fDim = 2;
	} else if ( f3Dim != 0 || f3DimWithWeight != 0)	{
		fDim = 3;
	}
}
//_________________________________________________________________________

void Ascii2HistDialog::SetDim()
{
  if (fDim == 1) {
		fNbinsX = f1DimNbinsX; 
	} else if (fDim == 2) {
		fNbinsX = f2DimNbinsX; 
		fNbinsY = f2DimNbinsY; 
   } else if (fDim == 3) {
		fNbinsX = f3DimNbinsX; 
		fNbinsY = f3DimNbinsY;
	}
}
//_________________________________________________________________________

void Ascii2HistDialog::RestoreDefaults()
{
   TEnv env(gHprLocalEnv);
   fHistFileName    = env.GetValue("Ascii2HistDialog.fHistFileName",  "values.dat");
   fHistName 		  = env.GetValue("Ascii2HistDialog.fHistName",		  "h1");
   fHistTitle  	  = env.GetValue("Ascii2HistDialog.fHistTitle",  	  "h1");
   fSpectrum        = env.GetValue("Ascii2HistDialog.fSpectrum",		   0);
   f1Dim            = env.GetValue("Ascii2HistDialog.f1Dim", 			   0);
   f1DimWithWeight  = env.GetValue("Ascii2HistDialog.f1DimWithWeight",  1);
	f1DimWithErrors  = env.GetValue("Ascii2HistDialog.f1DimWithErrors",  0);
	f2Dim            = env.GetValue("Ascii2HistDialog.f2Dim", 			   0);
   f2DimWithWeight  = env.GetValue("Ascii2HistDialog.f2DimWithWeight",  0);
   f3Dim            = env.GetValue("Ascii2HistDialog.f3Dim", 			   0);
   f3DimWithWeight  = env.GetValue("Ascii2HistDialog.f3DimWithWeight",  0);
	fNskip	  		  = env.GetValue("Ascii2HistDialog.fNskip",  	      0);
	fError           = env.GetValue("Ascii2HistDialog.fError",  		  0.0);
   fNbinsX  		  = env.GetValue("Ascii2HistDialog.fNbinsX",  	    100);
   fXlow    		  = env.GetValue("Ascii2HistDialog.fXlow", 			   0.);
   fXup     		  = env.GetValue("Ascii2HistDialog.fXup",  			 100.);
   fNbinsY  		  = env.GetValue("Ascii2HistDialog.fNbinsY",  		 100);
   fYlow    		  = env.GetValue("Ascii2HistDialog.fYlow", 			   0.);
   fYup     		  = env.GetValue("Ascii2HistDialog.fYup",  			 100.);
   fNbinsZ  		  = env.GetValue("Ascii2HistDialog.fNbinsZ",  		 100);
   fZlow    		  = env.GetValue("Ascii2HistDialog.fZlow", 			   0.);
   fZup     		  = env.GetValue("Ascii2HistDialog.fZup",  			 100.);
   fKeepLimits      = env.GetValue("Ascii2HistDialog.fKeepLimits",  		0 );
	f1DimNbinsX =	env.GetValue("Ascii2HistDialog.f1DimNbinsX",		1000); 
	f2DimNbinsX =	env.GetValue("Ascii2HistDialog.f2DimNbinsX",		100); 
	f2DimNbinsY =	env.GetValue("Ascii2HistDialog.f2DimNbinsY",		100); 
	f3DimNbinsX =	env.GetValue("Ascii2HistDialog.f3DimNbinsX",		50); 
	f3DimNbinsY =	env.GetValue("Ascii2HistDialog.f3DimNbinsX",		50); 
 	fNbinsZ 		=	env.GetValue("Ascii2HistDialog.f3NbinsZ",			50); 
 	GetDim();
 	SetDim();
//   cout << "fXup " << fXup<< endl;
}
//_________________________________________________________________________
void Ascii2HistDialog::WriteoutHist()
{
	TRootCanvas *cimp = NULL;
	if (fCanvas != NULL)
		cimp = (TRootCanvas *)fCanvas->GetCanvasImp();
	if (fHist) {
		Save2FileDialog sfd(fHist, NULL, cimp);
	}
}
//_________________________________________________________________________

void Ascii2HistDialog::CloseDown(Int_t /*wid*/)
{
   cout << "Ascii2HistDialog::CloseDown() " << endl;
   delete this;
}

#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TList.h"
#include "TObjString.h"
#include "TPolyMarker.h"
#include "SetColor.h"
#include <iostream>
#include "FhPeak.h"
#include "PeakFinder.h"
#include "FindPeakDialog.h"
#include "support.h"
#include "hprbase.h"
#include <fstream>

using std::cout;
using std::endl;
//____________________________________________________________________________

FindPeakDialog::FindPeakDialog()
{
	fInteractive = 0;
	fSelPad = NULL;
	fDialog = NULL;
}
//____________________________________________________________________________
FindPeakDialog::FindPeakDialog(TH1 * hist, Int_t interactive)
{

static const Char_t helptext[] =
"Find peaks either using TSpectrum implemented in root (default)\n\
or a simple method based on a shifting a square wave across\n\
the histogram and calculating the convolution integral.\n\
Peaks are added to list associated with the histogram for later\n\
use in fitting procedures.\n\
If peak heights vary strongly several different ranges may be used\n\
Parameters:\n\
  - fInteractive: = 0 (unchecked): dont present dialog\n\
  - From, To  : range for peaks to be stored for later fit\n\
Paramemeter for TSpectrum method:\n\
  - Sigma     : Assumumd Sigma of the peaks\n\
  - Threshold : Relative height of peak compared to maximum\n\
                in displayed range taken into account\n\
  - 2 Peak Resolution: Minimum separation of peaks in units\n\
                       of sigma, default 3, this the maximum\n\
                       for bigger values sigma must be increased\n\
                !!!!!!!!!  not yet implemented  !!!!!!!!!!!!!!!!!\n\
Parameters for square wave convolution:\n\
  - ThresholdSigma: Corresponds rougly to the significance of the\n\
                    peak (Sigma) above background\n\
  - PeakMWidth:     Should be about the full width of the peak\n\
                    e.g. 2 Sigma correspond to 68 % of the content.\n\
\n\
\"Print / Read Peak List\" allows to write to and read a peaklist\n\
from an ASCII- file containing 1 line / peak:\n\
Mean Width Content MeanError Chi2oNdf\n\
Only Mean is obligatory the others are optional\n\
 \n\
";
	fInteractive = interactive;
	fFindPeakDone = 0;
	fSelHist = hist;
	fName = hist->GetName();
	fParentWindow = NULL; 
	SetBit(kMustCleanup);
	fSelPad = gPad;
	gROOT->GetListOfCleanups()->Add(this);
	TIter next(gROOT->GetListOfCanvases());
	TCanvas *c;
	while ( (c = (TCanvas*)next()) ) {
		if (c->GetListOfPrimitives()->FindObject(fName)) {
			fSelPad = c;
			fSelPad->cd();
//         cout << "fSelPad " << fSelPad << " name: " << fName<< endl;
			break;
		}
	}
  
	if (fSelPad == NULL) {
	  cout << "fSelPad = 0!!" <<  endl;
	} else {
		
		fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
	}
	TAxis *ax = fSelHist->GetXaxis();
	fFrom = fSelHist->GetBinCenter(ax->GetFirst());
	fTo = fSelHist->GetBinCenter(ax->GetLast());
	RestoreDefaults();
//   cout << "fSigma " <<fSigma << endl;
	if ( interactive > 0 ) {
		TList *row_lab = new TList(); 
		static void *valp[50];
		Int_t ind = 0;
		static Int_t dummy = 0;
		static TString exgcmd("ExecuteFindPeak()");
		static TString clfcmd("ClearList()");
		static TString expcmd("PrintList()");
		static TString exrcmd("ReadList()");
		
		row_lab->Add(new TObjString("DoubleValue_From"));
		valp[ind++] = &fFrom;
		row_lab->Add(new TObjString("DoubleValue+To"));
		valp[ind++] = &fTo;
		row_lab->Add(new TObjString("RadioButton_TSpectrum"));
		fUseTSpectrumEntry = ind;
		valp[ind++] = &fUseTSpectrum;
		row_lab->Add(new TObjString("RadioButton+SQWaveConv"));
		fUseSQWaveFoldEntry = ind;
		valp[ind++] = &fUseSQWaveFold;
		row_lab->Add(new TObjString("CommentOnly_Parameters for TSpectrum"));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("DoubleValue_Sigma;0.;"));
		fSigmaEntry = ind;
		valp[ind++] = &fSigma;
//		row_lab->Add(new TObjString("DoubleValue+2 Peak Sep[sig]"));
//		valp[ind++] = &fTwoPeakSeparation;
		row_lab->Add(new TObjString("DoubleValue+Thresh;0.0;0.5"));
		fThresholdEntry = ind;
		valp[ind++] = &fThreshold;
		row_lab->Add(new TObjString("CheckButton_Markow Alg"));
		fMarkowEntry = ind;
		valp[ind++] = &fMarkow;
		row_lab->Add(new TObjString("CheckButton+Remove BG"));
		fRemoveBGEntry = ind;
		valp[ind++] = &fRemoveBG;
 		row_lab->Add(new TObjString("CommentOnly_Parameters for Square Wave Convolution"));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("DoubleValue_ThresholdSigma"));
		fThresholdSigmaEntry = ind;
		valp[ind++] = &fThresholdSigma;
//		row_lab->Add(new TObjString("DoubleValue+2 Peak Sep[sig]"));
//		valp[ind++] = &fTwoPeakSeparation;
		row_lab->Add(new TObjString("DoubleValue+PeakMWidth"));
		fPeakMwidthEntry = ind;
		valp[ind++] = &fPeakMwidth;
		row_lab->Add(new TObjString("CommandButt_Execute Find"));
		valp[ind++] = &exgcmd;
		row_lab->Add(new TObjString("CommandButt+Clear List"));
		valp[ind++] = &clfcmd;
		row_lab->Add(new TObjString("CommandButt_Print Peak List"));
		valp[ind++] = &expcmd;
		row_lab->Add(new TObjString("CommandButt+Read Peak List"));
		valp[ind++] = &exrcmd;
		row_lab->Add(new TObjString("FileRequest_Filename"));
		valp[ind++] = &fPeakListName;
		Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
		Int_t ok = 0;
		fDialog =
		new TGMrbValuesAndText ("FindPeaks", NULL, &ok, itemwidth,
								fParentWindow, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
		CRButtonPressed(0, fUseTSpectrumEntry, NULL);
	}
}
//__________________________________________________________________________

FindPeakDialog::~FindPeakDialog()
{ 
// cout << "dtor FindPeakDialog: " << this << endl;
	gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void FindPeakDialog::RecursiveRemove(TObject * obj)
{
//   cout << "FindPeakDialog: RecursiveRemove " << obj << endl;
	if (obj == fSelPad) { 
//      cout << "FindPeakDialog: CloseDialog "  << endl;
		CloseDialog(); 
	}
}
//__________________________________________________________________________

void FindPeakDialog::ClearList()
{
	cout << " FindPeakDialog::ClearList()" << endl;
	TList * p
	= (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
	if (p) {
		fSelHist->GetListOfFunctions()->Remove(p);
		p->Delete();
		delete p;
	}
	TPolyMarker *pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
		if (pm) {
			fSelHist->GetListOfFunctions()->Remove(pm);
			pm->Delete();
		}
	if (gPad) {
		gPad->Modified();
		gPad->Update();
	}
}
//__________________________________________________________________________

void FindPeakDialog::PrintList()
{
	cout << " FindPeakDialog::PrintList()" << endl;
	TList * pl
	= (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
	if (pl) {
		TIter next(pl);
		FhPeak *p;
		ofstream wstream;
		Bool_t tofile=kFALSE;
		if ( fPeakListName.Length() > 0 ) {
			if (!gSystem->AccessPathName((const char *)fPeakListName , kFileExists)) {
				TString question = fPeakListName;
				question += " exists, Overwrite?";
//				if (!QuestionBox(question.Data(), fParentWindow)) return;
				int buttons = kMBOk | kMBDismiss, retval = 0;
				EMsgBoxIcon icontype = kMBIconQuestion;
				new TGMsgBox(gClient->GetRoot(), fParentWindow,
								 "Question", question,
								 icontype, buttons, &retval);
								 if (retval == kMBDismiss)
									 return;
								 
			}
			wstream.open(fPeakListName, ios::out);
			if (!wstream.good()) {
				cout	<< "Error in open file: "
				<< gSystem->GetError() << " - " << fPeakListName<< endl;
				return;
			}
			tofile = kTRUE;
		}
		while ( p = (FhPeak *)next() ) {
			if ( tofile )
				p->PrintPeak(wstream);
			else
				p->PrintPeak(cout);
		}
	}
}
//__________________________________________________________________________

void FindPeakDialog::ReadList()
{
	cout << " FindPeakDialog::ReadList()" << endl;
	TList * p
	= (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
	if ( p && p->GetSize()  > 0 ) {
//		if (!QuestionBox("Peak list exists, add anyway?", fParentWindow)) return;
		int buttons = kMBOk | kMBDismiss, retval = 0;
		EMsgBoxIcon icontype = kMBIconQuestion;
		new TGMsgBox(gClient->GetRoot(),fParentWindow ,
						 "Question", "Peak list exists, add anyway?",
						 icontype, buttons, &retval);
						 if (retval == kMBDismiss)
							 return;
	}
	ifstream infile;
	if ( fPeakListName.Length() > 0 ) {
		if (gSystem->AccessPathName((const char *)fPeakListName , kFileExists)) {
//			TString question = fPeakListName;
//			question += " not found";
//			WarnBox(question, fParentWindow);
			cout << setred << fPeakListName << " not found" << setblack << endl;
			return;
		}
		infile.open(fPeakListName, ios::in);
		if (!infile.good()) {
			cout	<< "Error in open file: "
			<< gSystem->GetError() << " - " << fPeakListName<< endl;
			return;
		}
	}
	if (!p) {
		p = new TList();
		p->SetName("spectrum_peaklist");
		fSelHist->GetListOfFunctions()->Add(p);
	}
	TPolyMarker * pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
	if (!pm) {
		pm = new TPolyMarker(100);
		pm->SetMarkerColor(6);
		pm->SetMarkerSize(1);
		pm->SetMarkerStyle(23);
		fSelHist->GetListOfFunctions()->Add(pm);
	}
	TString line;
	TString del(" ,\t");
	TObjArray * oa;
	TString val; 
	Int_t n = 0;
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
		oa = line.Tokenize(del);
		Int_t nent = oa->GetEntries();
		if (nent < 1) {
			cout << "Not enough entries at: " << n+1 << endl;
			break;
		}
		for (Int_t i = 0; i < nent; i++) {
			val = ((TObjString*)oa->At(i))->String();
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << n+1 << endl;
				return;
			}
		}
		val = ((TObjString*)oa->At(0))->String();
		Double_t x = val.Atof();
		Int_t bin = fSelHist->GetBin(x);
		Double_t maxy = fSelHist->GetMaximum();
		if (bin < 1 || bin >fSelHist->GetNbinsX() ) {
			cout << "Value: " << x << " outside histogram" << endl;
		} else {
			Double_t y = fSelHist->GetBinContent(bin);
			if (fSelHist->GetBinContent(bin-1) > y )
				y = fSelHist->GetBinContent(bin-1);
			if (fSelHist->GetBinContent(bin+1) > y )
				y = fSelHist->GetBinContent(bin+1);
			pm->SetPoint(n, x, y + 0.005 * maxy);
			cout << " pm " << n << " " << x << " " <<  y + 0.005 * maxy<< endl;
		}
		if ( n >= pm->Size() -1 )
			pm->SetPolyMarker(pm->Size() + 10);
		FhPeak *pe = new FhPeak(x);
		
		if (nent >= 1) {
			val = ((TObjString*)oa->At(1))->String();
			pe->SetWidth(val.Atof());
		}
		if (nent >= 2) {
			val = ((TObjString*)oa->At(2))->String();
			pe->SetContent(val.Atof());
		}
		if (nent >= 3) {
			val = ((TObjString*)oa->At(3))->String();
			pe->SetMeanError(val.Atof());
		}
		if (nent >= 4) {
			val = ((TObjString*)oa->At(4))->String();
			pe->SetChi2oNdf(val.Atof());
		}
		p->Add(pe);
		n++;
	}
	pm->SetPolyMarker(n);
	pm->Draw();
	gPad->Modified();
	gPad->Update();
}
//__________________________________________________________________________

void FindPeakDialog::ExecuteFindPeak()
{
	PeakFinder * pf = NULL;
	TSpectrum *s    = NULL;
	TPolyMarker poly; 
	TPolyMarker * pm;
	TAxis *xa = fSelHist->GetXaxis();
	Int_t fbin = 1;
	Int_t lbin = fSelHist->GetNbinsX();
	if (fFrom > 0) 
		fbin = fSelHist->FindBin(fFrom);
	if (fTo > 0) 
		lbin = fSelHist->FindBin(fTo);
	xa->SetRange(fbin, lbin);
//   if (fClearList) {
//      ClearList();
//   } else {
		pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
		if (pm) {
			cout << "copy pm -> poly" << endl;
			poly.SetPolyMarker(0);
			pm->Copy(poly);
		}
//   }
	TList *lof = fSelHist->GetListOfFunctions();
	TList *p = NULL;
	p = (TList*)lof->FindObject("spectrum_peaklist");
	if (!p) {
		p = new TList();
		p->SetName("spectrum_peaklist");
		fSelHist->GetListOfFunctions()->Add(p);
	}
	Float_t *xpeaks;
	Int_t nfound = 0;
	if (fUseTSpectrum) {
		Int_t npeaks = 100;
		s = new TSpectrum(2*npeaks, 1);
	//   if (fTwoPeakSeparation <= 0) fTwoPeakSeparation = 3.;
	//   s->SetResolution(3. / fTwoPeakSeparation);
	//   cout << " SetResolution " << 3. / fTwoPeakSeparation << endl;
		TString opt("");
		if (!fMarkow) opt += "noMarkov";
		if (!fRemoveBG) opt += "nobackground";
		if (!fShowMarkers) opt += "goff";
		nfound = s->Search(fSelHist,fSigma, opt,fThreshold);
		xpeaks = (Float_t*)s->GetPositionX();
	} else {
		pf = new PeakFinder(fSelHist, fPeakMwidth, fThresholdSigma);
		xpeaks = (Float_t*)pf->GetPositionX();
		nfound = pf->GetNpeaks();
	}
	if (nfound > 100) nfound = 100;
	for (Int_t i=0;i<nfound;i++) {
		Float_t xp = xpeaks[i];
		if (xp < fFrom || xp > fTo) continue;
		Int_t bin = fSelHist->GetXaxis()->FindBin(xp);
		Float_t yp = fSelHist->GetBinContent(bin);
		if (fInteractive) 
			printf("Pos: %8.1f Cont@Pos:  %8.1f\n", xp, yp);
		FhPeak *pe = new FhPeak(xp);
		pe->SetWidth(fSigma);
		pm = new TPolyMarker();
		pe->SetContent(yp);
		p->Add(pe);
	}
//   if (!fClearList) {
		pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
		if (!pm) {
			pm = new TPolyMarker();
			cout << "pm = new TPolyMarker() " << endl;
			fSelHist->GetListOfFunctions()->Add(pm);
		}
		pm->SetMarkerColor(2);
		pm->SetMarkerSize(2);
		pm->SetMarkerStyle(23);
		// copy possibly existing marks
		Double_t * xp = poly.GetX();
		Double_t * yp = poly.GetY();
		Int_t np = pm->Size();
		for (Int_t i = 0; i < poly.GetN(); i++) {
			cout << "pm" << i << " " << *xp << " " << *yp << endl;
			pm->SetPoint(np + i, *xp++, *yp++);
		}
		pm->Draw();
//   }
	gPad->Modified();
	gPad->Update();
	SaveDefaults();
	fFindPeakDone++;
}
//_______________________________________________________________________

void FindPeakDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fFrom      = env.GetValue("FindPeakDialog.fFrom", 0.);
	fTo        = env.GetValue("FindPeakDialog.fTo", 2000.);
	fThreshold = env.GetValue("FindPeakDialog.fThreshold", 0.001);
	fSigma     = env.GetValue("FindPeakDialog.fSigma", 2.);
	fTwoPeakSeparation = env.GetValue("FindPeakDialog.fTwoPeakSeparation", 3.);
	fMarkow    = env.GetValue("FindPeakDialog.fMarkow", 1);
	fRemoveBG  = env.GetValue("FindPeakDialog.fRemoveBG", 1);
	fShowMarkers = env.GetValue("FindPeakDialog.fShowMarkers", 1);
	fUseTSpectrum   = env.GetValue("FindPeakDialog.fUseTSpectrum", 1);
	fUseSQWaveFold  = env.GetValue("FindPeakDialog.fUseSQWaveFold", 0);
	fThresholdSigma = env.GetValue("FindPeakDialog.fThresholdSigma", 2.5);
	fPeakMwidth     = env.GetValue("FindPeakDialog.fPeakMwidth", 5);
}
//_______________________________________________________________________

void FindPeakDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("FindPeakDialog.fFrom",      fFrom     );
	env.SetValue("FindPeakDialog.fTo",        fTo       );
	env.SetValue("FindPeakDialog.fThreshold", fThreshold);
	env.SetValue("FindPeakDialog.fSigma",     fSigma    );
	env.SetValue("FindPeakDialog.fTwoPeakSeparation",fTwoPeakSeparation);
	env.SetValue("FindPeakDialog.fMarkow", fMarkow);
	env.SetValue("FindPeakDialog.fRemoveBG", fRemoveBG);
	env.SetValue("FindPeakDialog.fShowMarkers", fShowMarkers);
	env.SetValue("FindPeakDialog.fUseTSpectrum", fUseTSpectrum);
	env.SetValue("FindPeakDialog.fUseSQWaveFold", fUseSQWaveFold);
	env.SetValue("FindPeakDialog.fThresholdSigma", fThresholdSigma);
	env.SetValue("FindPeakDialog.fPeakMwidth",fPeakMwidth) ;
	env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FindPeakDialog::CloseDialog()
{
//   cout << "FindPeakDialog::CloseDialog() " << endl;
	gROOT->GetListOfCleanups()->Remove(this);
	if (fInteractive >  0) fDialog->CloseWindowExt();
	delete this;
}
//_______________________________________________________________________

void FindPeakDialog::CloseDown(Int_t /*wid*/)
{
//   cout << "FindPeakDialog::CloseDown() " << wid<< endl;
	SaveDefaults();
	delete this;
}
//_______________________________________________________________________

void FindPeakDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/) 
{
	if (bid == fUseTSpectrumEntry || bid == fUseSQWaveFoldEntry) {
		if (fUseTSpectrum) {
			fDialog->DisableButton(fThresholdSigmaEntry);
			fDialog->DisableButton(fPeakMwidthEntry    );
			fDialog->EnableButton(fThresholdEntry);
			fDialog->EnableButton(fSigmaEntry    );
			fDialog->EnableButton(fMarkowEntry   );
			fDialog->EnableButton(fRemoveBGEntry );
		} else {
			fDialog->DisableButton(fThresholdEntry);
			fDialog->DisableButton(fSigmaEntry    );
			fDialog->DisableButton(fMarkowEntry   );
			fDialog->DisableButton(fRemoveBGEntry );
			fDialog->EnableButton(fThresholdSigmaEntry);
			fDialog->EnableButton(fPeakMwidthEntry    );
		}
	}
//   cout << "FindPeakDialog::Btp " << wid << " " << bid << endl;
}


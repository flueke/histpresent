#include "TROOT.h"
//#include "TStyle.h"
//#include "TSystem.h"
#include "TSpectrum2.h"
#include "TCanvas.h"
#include "TEnv.h"
#include "TLatex.h"
#include "TList.h"
#include "TMath.h"
#include "TString.h"
//#include "TObjString.h"
#include "TPolyMarker.h"
#include "SetColor.h"
// #include "WindowSizeDialog.h"
#include <iostream>
//#include "FhPeak.h"
//#include "PeakFinder.h"
#include "FindPeakDialog2D.h"
// #include "support.h"
// #include "hprbase.h"
#include <fstream>

using std::cout;
using std::endl;
//____________________________________________________________________________

FindPeakDialog2D::FindPeakDialog2D()
{
	fInteractive = 0;
	fSelPad = NULL;
	fDialog = NULL;
	fDeconvHist = NULL;
	fDeconvCanvas = NULL;
}
//____________________________________________________________________________
FindPeakDialog2D::FindPeakDialog2D(TH2 * hist, Int_t interactive)
{

static const Char_t helptext[] =
"Find peaks  using TSpectrum implemented in root \n\
Peaks are added to list associated with the histogram for later\n\
use in fitting procedures.\n\
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
                    e.g. 2 Sigma correspond to 68 % of the content.\n\
\n\
\"Print / Read Peak List\" allows to write to and read a peaklist\n\
from an ASCII- file containing 1 line / peak:\n\
Mean Width Content MeanError Chi2oNdf\n\
Only Mean is obligatory the others are optional\n\
 \n\
";
	fInteractive = interactive;
	fNpeaks = 50;
	fFindPeakDone = 0;
	fDeconvHist = NULL;
	fDeconvCanvas = NULL;
	fSelHist = hist;
	fName = hist->GetName();
	fParentWindow = NULL; 
	SetBit(kMustCleanup );
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
	fFromX = ax->GetBinCenter(ax->GetFirst());
	fToX = ax->GetBinCenter(ax->GetLast());
	TAxis *ay = fSelHist->GetYaxis();
	fFromY = ay->GetBinCenter(ay->GetFirst());
	fToY = ay->GetBinCenter(ay->GetLast());
	RestoreDefaults();
//   cout << "fSigma " <<fSigma << endl;
	if ( interactive > 0 ) {
		TList *row_lab = new TList(); 
		row_lab->SetName("row_lab");
		static void *valp[50];
		Int_t ind = 0;
//		static Int_t dummy = 0;
		static TString exgcmd("ExecuteFindPeak()");
		static TString clfcmd("ClearList()");
		static TString expcmd("PrintList()");
		static TString exrcmd("ReadList()");
		
		row_lab->Add(new TObjString("DoubleValue_FromX"));
		valp[ind++] = &fFromX;
		row_lab->Add(new TObjString("DoubleValue+ToX"));
		valp[ind++] = &fToX;
		row_lab->Add(new TObjString("DoubleValue_FromY"));
		valp[ind++] = &fFromY;
		row_lab->Add(new TObjString("DoubleValue+ToY"));
		valp[ind++] = &fToY;
		row_lab->Add(new TObjString("CheckButton_HighRes"));
		valp[ind++] = &fHighRes;
		row_lab->Add(new TObjString("DoubleValue+Sigma;0.1;"));
		valp[ind++] = &fSigma;
		row_lab->Add(new TObjString("DoubleValue+Thresh;0.0001;1."));
		valp[ind++] = &fThreshold;
		row_lab->Add(new TObjString("CheckButton_Markow Alg"));
		valp[ind++] = &fMarkov;
		row_lab->Add(new TObjString("CheckButton+Remove BG"));
		valp[ind++] = &fRemoveBG;
		row_lab->Add(new TObjString("PlainIntVal+AvgeWindow"));
		valp[ind++] = &fAverageWindow;
		
		row_lab->Add(new TObjString("ColorSelect_MColor"));
		valp[ind++] = &fMarkerColor;
		row_lab->Add(new TObjString("Mark_Select+MStyle"));
		valp[ind++] = &fMarkerStyle;
		row_lab->Add(new TObjString("Float_Value+MSiz;0;5"));
		valp[ind++] = &fMarkerSize;

		row_lab->Add(new TObjString("CommandButt_Execute Find"));
		valp[ind++] = &exgcmd;
		row_lab->Add(new TObjString("CommandButt+Clear List"));
		valp[ind++] = &clfcmd;
//		row_lab->Add(new TObjString("CommandButt_Print Peak List"));
//		valp[ind++] = &expcmd;
//		row_lab->Add(new TObjString("CommandButt+Read Peak List"));
//		valp[ind++] = &exrcmd;
//		row_lab->Add(new TObjString("FileRequest_Filename"));
//		valp[ind++] = &fPeakListName;
		Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
		Int_t ok = 0;
		fDialog =
		new TGMrbValuesAndText ("FindPeaks", NULL, &ok, itemwidth,
								fParentWindow, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
	}
}
//__________________________________________________________________________
FindPeakDialog2D::~FindPeakDialog2D()
{ 
// cout << "dtor FindPeakDialog2D: " << this << endl;
	gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void FindPeakDialog2D::RecursiveRemove(TObject * obj)
{
//	if (gHprDebug > 1) 
 //  cout << "FindPeakDialog2D: RecursiveRemove " << obj << endl;
	if (obj == fDeconvHist) {
//		if (gHprDebug > 0) 
//		cout << "FindPeakDialog2D: RecursiveRemove fDeconvHist " << obj << endl;
		fDeconvHist = NULL;
	}
	if (obj == fDeconvCanvas) {
//		if (gHprDebug > 0) 
//		cout << "FindPeakDialog2D: RecursiveRemove fDeconvCanvas " << obj << endl;
		fDeconvCanvas = NULL;
	}
	if (obj == fSelPad) { 
//		if (gHprDebug > 0) 
//		cout << "FindPeakDialog2D: CloseDialog "  << endl;
		CloseDialog(); 
	}
}
//__________________________________________________________________________

void FindPeakDialog2D::ClearList()
{
//	cout << " FindPeakDialog2D::ClearList()" << endl;
	/*
	TList * p
	= (TList*)fSelHist->GetListOfFunctions()->FindObject("spectrum_peaklist");
	if (p) {
		fSelHist->GetListOfFunctions()->Remove(p);
		p->Delete();
		delete p;
	}
	*/
	if (fDeconvHist) {
		fDeconvHist->GetListOfFunctions()->Clear("nodelete");
		delete fDeconvHist;
		fDeconvHist = NULL;
	}
	if (fDeconvCanvas) {
		delete fDeconvCanvas;
		fDeconvCanvas = NULL;
	}
	fSelPad->cd();
	TObject *pm;
	TList *temp = new TList();
	temp->SetName("temp");
	TIter next1(fSelHist->GetListOfFunctions());
	while (pm  = next1()) {
		if (pm->InheritsFrom("TPolyMarker"))
			temp->Add(pm);
	}
	if (temp->GetSize() > 0) {
	//	cout<< "temp.GetSize() " << temp->GetSize() << endl;
		TIter next(temp);
		while ( pm = next() ){
		//	cout<< "pm " << pm << endl;
			fSelHist->GetListOfFunctions()->Remove(pm);
//			pm->Delete();
		}
	}
	temp->Delete();
	
	TIter next2(fSelPad->GetListOfPrimitives());
	while (pm  = next2()) {
		if (pm->InheritsFrom("TLatex"))
			temp->Add(pm);
	}
	temp->Delete();
	delete temp;
	if (gPad) {
		gPad->Modified();
		gPad->Update();
	}
}
//__________________________________________________________________________
/*
void FindPeakDialog2D::PrintList()
{
	cout << " FindPeakDialog2D::PrintList()" << endl;
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

void FindPeakDialog2D::ReadList()
{
	cout << " FindPeakDialog2D::ReadList()" << endl;
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
*/
//__________________________________________________________________________

void FindPeakDialog2D::ExecuteFindPeak()
{
	TAxis *xa = fSelHist->GetXaxis();
	TAxis *ya = fSelHist->GetYaxis();
	Int_t nbinsx = fSelHist->GetNbinsX();
 	Int_t nbinsy = fSelHist->GetNbinsY();
	TSpectrum2 *s = new TSpectrum2(fNpeaks);
	Double_t** source = NULL;
	Double_t** dest= NULL;
	if (fHighRes > 0) {
		source = new Double_t*[nbinsx];
		for (int i=0;i<nbinsx;i++) source[i]=new Double_t[nbinsy];
		dest = new Double_t*[nbinsx];
		for (int i=0;i<nbinsx;i++) dest[i]=new Double_t[nbinsy];
		
		for (int i=0;i<nbinsx;i++) {
			for (int j=0;j<nbinsy;j++) {
				if (  xa->GetBinCenter(i+1) >= fFromX && xa->GetBinCenter(i+1) <= fToX
					&& ya->GetBinCenter(j+1) >= fFromY && ya->GetBinCenter(j+1) <= fToY )
					source[i][j] = fSelHist->GetBinContent(i+1, j+1);
				else 
					source[i][j] = 0.;
				dest[i][j] = 0.;
			}
		}
	}
   //now the real stuff: Finding the peaks
   // Note: search is done in normalized histogram
   Double_t sigma_norm = fSigma*nbinsx / (xa->GetXmax()- xa->GetXmin());
   Int_t avgwin_norm = TMath::Nint(fAverageWindow*nbinsx / (xa->GetXmax()- xa->GetXmin()));
   avgwin_norm = TMath::Min(1,avgwin_norm);
//  cout << "sigma_norm " << sigma_norm <<  " avgwin_norm " <<avgwin_norm <<  endl;
//  cout << "xa->GetXmin() " <<xa->GetXmin() <<  " xa->GetXmax() " << xa->GetXmax()<<  endl;
//   cout << "ya->GetXmin() " <<ya->GetXmin() <<  " ya->GetXmax() " << ya->GetXmax()<<  endl;
	Int_t nfound = 0;
   if (fHighRes > 0) {
		Int_t dconiterations = 1;
		nfound = s->SearchHighRes(source, dest, nbinsx, nbinsy,
		sigma_norm, fThreshold, fRemoveBG, dconiterations, fMarkov, avgwin_norm);
	} else {
		TString opt("goff");
		if (fMarkov == 0) opt+="noMarkov";
		if (fRemoveBG == 0) opt+="nobackground";
		nfound = s->Search(fSelHist, sigma_norm, opt, fThreshold);
	} 
//   s->Print();
//   Int_t nfound = s->Search(myhist2,sigma,"col",threshold);

   TPolyMarker * pm = (TPolyMarker*)fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
   if (pm && fAutoClear) {
      fSelHist->GetListOfFunctions()->Remove(pm);
      delete pm;
		gPad->Modified();
		gPad->Update();
   }
   // 
   pm = new TPolyMarker(nfound, s->GetPositionX(), s->GetPositionY());
   // convert to histogram scale
   Double_t x0 = xa->GetXmin();
   Double_t xs = (xa->GetXmax()- x0) / (Double_t)nbinsx;
   Double_t y0 = ya->GetXmin();
   Double_t ys = (ya->GetXmax()- y0) / (Double_t)nbinsy;
   Double_t *xp = s->GetPositionX();
   Double_t *yp = s->GetPositionY();
   Double_t *xm = pm->GetX();
   Double_t *ym = pm->GetY();
   for (Int_t i = 0; i<nfound; i++) {
		if (fHighRes>0) {
			xm[i] = x0 + xs*xp[i];
			ym[i] = y0 + ys*yp[i];
		}else {
			xm[i] = xp[i];
			ym[i] = yp[i];
		}		
		cout << "x[" << i << "]=" << xm[i] <<  
				", y[" << i << "]=" << ym[i] << endl;
	}
   fSelHist->GetListOfFunctions()->Add(pm);
   pm->SetMarkerStyle(fMarkerStyle);
   pm->SetMarkerColor(fMarkerColor);
   pm->SetMarkerSize(fMarkerSize);
	TLatex lat;
	lat.SetTextSize(0.06);
	lat.SetTextAlign(22);
	lat.SetTextAlign(kRed);
	for (Int_t i=0; i<pm->GetN(); i++){
		lat.DrawLatex(xm[i], ym[i], Form("%d",i));
	}
	gPad->Modified();
	gPad->Update();
   if (fHighRes > 0  && fShowDeconv > 0) {
		// fill hist with resulting convoluted spectrum
		TString title = fSelHist->GetTitle();
		title += "_deconv";
		
		fDeconvHist = new TH2F("deconv", title, 
										nbinsx,xa->GetXmin() ,xa->GetXmax(),
										nbinsy, ya->GetXmin(),ya->GetXmax());
		for (int i=0;i<nbinsx;i++) {
			for (int j=0;j<nbinsy;j++) {
				fDeconvHist->SetBinContent(i+1,j+1,dest[i][j]);
			}
		}
		fDeconvHist->GetListOfFunctions()->Add(pm);
		fDeconvCanvas = new TCanvas("deconv", "deconvoluted",1200, 200, 700, 700);
		fDeconvCanvas->ToggleEventStatus();
//		WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
//		WindowSizeDialog::fWinwidx_2dim, WindowSizeDialog::fWinwidy_2dim);
		fDeconvCanvas->Draw();
		fDeconvHist->Draw("colz");
		for (Int_t i=0; i<pm->GetN(); i++){
			lat.DrawLatex(xm[i], ym[i], Form("%d",i));
		}
		gPad->Modified();
		gPad->Update();
	}
	SaveDefaults();
	fFindPeakDone++;
}
//_______________________________________________________________________

void FindPeakDialog2D::RestoreDefaults()
{
	TEnv env(".hprrc");
	fFromX      = env.GetValue("FindPeakDialog2D.fFromX", 0.);
	fToX        = env.GetValue("FindPeakDialog2D.fToX", 4000.);
	fFromY      = env.GetValue("FindPeakDialog2D.fFromY", 0.);
	fToY        = env.GetValue("FindPeakDialog2D.fToY", 4000.);
	fThreshold = env.GetValue("FindPeakDialog2D.fThreshold", 0.01);
	fSigma     = env.GetValue("FindPeakDialog2D.fSigma", 2.);
	fSigma     = env.GetValue("FindPeakDialog2D.fSigma", 2.);
	fHighRes   = env.GetValue("FindPeakDialog2D.fHighRes", 1);
//	fTwoPeakSeparation = env.GetValue("FindPeakDialog2D.fTwoPeakSeparation", 3.);
	fMarkov    = env.GetValue("FindPeakDialog2D.fMarkov", 1);
	fRemoveBG  = env.GetValue("FindPeakDialog2D.fRemoveBG", 1);
	fAverageWindow = env.GetValue("FindPeakDialog2D.fAverageWindow", 1);
	fShowDeconv = env.GetValue("FindPeakDialog2D.fShowDeconv", 1);
	fAutoClear   = env.GetValue("FindPeakDialog2D.fAutoClear", 0);
	fMarkerColor = env.GetValue("FindPeakDialog2D.fMarkerColor", kBlue);
	fMarkerStyle = env.GetValue("FindPeakDialog2D.fMarkerStyle", 23);
	fMarkerSize   = env.GetValue("FindPeakDialog2D.fMarkerSize", 1.6);

//	fThresholdSigma = env.GetValue("FindPeakDialog2D.fThresholdSigma", 2.5);
//	fPeakMwidth     = env.GetValue("FindPeakDialog2D.fPeakMwidth", 5);
}
//_______________________________________________________________________

void FindPeakDialog2D::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("FindPeakDialog2D.fFromX",      fFromX     );
	env.SetValue("FindPeakDialog2D.fToX",        fToX       );
	env.SetValue("FindPeakDialog2D.fFromY",      fFromY     );
	env.SetValue("FindPeakDialog2D.fToY",        fToY       );
	env.SetValue("FindPeakDialog2D.fThreshold", fThreshold);
	env.SetValue("FindPeakDialog2D.fSigma",     fSigma    );
	env.SetValue("FindPeakDialog2D.fHighRes", fHighRes);
//	env.SetValue("FindPeakDialog2D.fTwoPeakSeparation",fTwoPeakSeparation);
	env.SetValue("FindPeakDialog2D.fMarkov", fMarkov);
	env.SetValue("FindPeakDialog2D.fRemoveBG", fRemoveBG);
	env.SetValue("FindPeakDialog2D.fAverageWindow", fAverageWindow);
	env.SetValue("FindPeakDialog2D.fShowDeconv", fShowDeconv);
	env.SetValue("FindPeakDialog2D.fAutoClear", fAutoClear);
	env.SetValue("FindPeakDialog2D.fMarkerColor", fMarkerColor);
	env.SetValue("FindPeakDialog2D.fMarkerStyle", fMarkerStyle);
	env.SetValue("FindPeakDialog2D.fMarkerSize", fMarkerSize);
	
//	env.SetValue("FindPeakDialog2D.fUseSQWaveFold", fUseSQWaveFold);
//	env.SetValue("FindPeakDialog2D.fThresholdSigma", fThresholdSigma);
//	env.SetValue("FindPeakDialog2D.fPeakMwidth",fPeakMwidth) ;
	env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FindPeakDialog2D::CloseDialog()
{
//   cout << "FindPeakDialog2D::CloseDialog() " << endl;
	gROOT->GetListOfCleanups()->Remove(this);
	if (fInteractive >  0) fDialog->CloseWindowExt();
	delete this;
}
//_______________________________________________________________________

void FindPeakDialog2D::CloseDown(Int_t /*wid*/)
{
//   cout << "FindPeakDialog2D::CloseDown() " << wid<< endl;
	SaveDefaults();
	delete this;
}
//_______________________________________________________________________

void FindPeakDialog2D::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/) 
{
//   cout << "FindPeakDialog2D::Btp " << wid << " " << bid << endl;
}


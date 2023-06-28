#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "TROOT.h"
#include "TQObject.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TCutG.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TObject.h"
#include "TGObject.h"
#include "TObjectTable.h"
#include "TTimer.h"
#include "TProfile.h"
#include "TPad.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TPolyLine.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TPaveLabel.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "Buttons.h"
#include "TButton.h"
#include "TDiamond.h"
#include "TContextMenu.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TRint.h"
#include "TList.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TArrayC.h"
#include "TArrayI.h"
#include "TExec.h"
#include "TLegendEntry.h"
#include "THistPainter.h"
#include "TView3D.h"
#include "TGWidget.h"
#include "HprGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
//#include "FitHist_Help.h"
#include "HTCanvas.h"
#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FhContour.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbNamedArray.h"
#include "TMrbString.h"
#include "TMrbArrayD.h"
#include "TGMrbSliders.h"
#include "TGMrbInputDialog.h"
#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "FitOneDimDialog.h"
#include "Fit2DimDialog.h"
#include "Set3DimOptDialog.h"
#include "SetHistOptDialog.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "HandleMenus.h"
#include "TText3D.h"

#include "hprbase.h"

extern HistPresent *gHpr;
extern Int_t gHprClosing;
extern Int_t gHprDebug;
extern Int_t nHists;
extern Double_t gTranspThresh;
extern Double_t gTranspAbove;
extern Double_t gTranspBelow;

extern Double_t my_transfer_function(const Double_t *x, const Double_t * /*param*/);

enum dowhat { expand, projectx, projecty, statonly, projectf,
		 projectboth , profilex, profiley, projectx_func};

ClassImp(FitHist)

//_______________________________________________________________________________
// def constructor
FitHist::FitHist()
{
	if (gDebug > 0)
		cout << "FitHist default ctor: " << this << endl;
	fSelHist = NULL;
	fCanvas = NULL;
}
//_______________________________________________________________________________
// constructor

FitHist::FitHist(const Text_t * name, const Text_t * title, TH1 * hist,
					const Text_t * hname, Int_t win_topx, Int_t win_topy,
					Int_t win_widx, Int_t win_widy, TButton *cmdb)
		  :TNamed(name, title), fCmdButton(cmdb)
{
	if (gHprDebug > 0)
		cout << "FitHist ctor: " << this << " name: " << name << " title: " << name
		<< endl << " hname: " << fHname.Data()<< "HistAddr " << hist
		<< endl << flush;
	if (!hist) {
		cout << "NULL pointer in: " << name << endl;
		return;
	};
	// look if object exists 
	FitHist *hold = (FitHist *) gROOT->GetList()->FindObject(GetName());
	if (hold) {
		if (gHprDebug>0)
			cout << "FitHist ctor: this " << this << " Old FitHist: " << hold 
			<<" Old Hist " << hold->GetSelHist() << endl;
		gROOT->GetList()->Remove(hold);
//		gROOT->GetListOfCleanups()->Remove(hold);
		TCanvas *cc = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(hold->GetCanvas());
		if (cc) {
			cc->GetListOfPrimitives()->Remove(hold->GetSelHist());
			if ( hold->GetSelHist() == hist )
				hold->SetSelHist(NULL);
			hold->SetCanvasIsAlive(kFALSE);
			delete cc;
		}
		// FitHist object will be deleted after signal from delete TCanvas
//		delete hold;
	}
//   hp = (HistPresent *) gROOT->FindObject("mypres");
	if (!gHpr)
		cout << "running without HistPresent" << endl;
	fSelHist = hist;
	fHname = hname;
//   Int_t pp = fHname.Index(".");
//   if(pp) fHname.Remove(0,pp+1);
	if (gHprDebug > 0) {
		cout << "FitHist ctor: " << this << " name: " << name << " title: " << name
		<< endl << " hname: " << fHname.Data()<< "HistAddr " << hist
		<< endl << flush;
		PrintMarks();
	}
	fCutPanel = NULL;
//   fDialog  = NULL;
	fSetRange = kFALSE;
	fRangeLowX = 0;
	fRangeUpX = 0;
	fRangeLowY = 0;
	fRangeUpY = 0;
	fOrigLowX = hist->GetXaxis()->GetXmin();
	fOrigUpX  = hist->GetXaxis()->GetXmax();
	fOrigLowY = hist->GetYaxis()->GetXmin();
	fOrigUpY =  hist->GetYaxis()->GetXmax();
	fBinX_1 = fBinX_2 = fBinY_1 = fBinY_2 = 0;
	fBinlx  = fBinux = fBinly = fBinuy = 0;     
	fExplx  = fExpux = fExply = fExpuy = 0;    
	fX_1    =   fX_2 =   fY_1 =   fY_2 = 0;    
	fRangeLowX = fRangeUpX = fRangeLowY = fRangeUpY = 0;
//	fOrigLowX  = fOrigUpX  = fOrigLowY  = fOrigUpY  = 0;
	fFrameX1   = fFrameX2  = fFrameY1   = fFrameY2  = 0;
	fBinX_1 = fBinX_2 = -1;
	fLogx = 0;
	fLogy = 0;
	fLogz = 0;
	fUserContourLevels = 0;
//   datebox = 0;
	fExpInd = 0;
	fLiveConstBG = 0;
	fFit1DimD = 0;
	fFit2DimD = 0;
	fCalHist = NULL;
	fCalFitHist = NULL;
	fCalFunc = NULL;
	fSelPad = NULL;
	fCanvas   = NULL;
	fExpHist = NULL;
	fDateText = NULL;
	fProjHistX = NULL;
	fProjHistY = NULL;
	fTofLabels = NULL;
	fMarkers = NULL;
	fDialog = NULL;
	fSerialRot = 0;
	fFirstUse = 0;
	fSetColors = kFALSE;
	fSetLevels = kFALSE;
	
	fSelInside = kTRUE;
	fAutoDisplayS2FD = kFALSE;
	fDeleteCalFlag = kFALSE;

	fActiveWindows = new TList();fActiveWindows ->SetName("fActiveWindows") ;
	fPeaks = new TList();  fPeaks->SetName("fPeaks") ;
	fActiveFunctions = new TList();  fActiveFunctions->SetName("fActiveFunctions") ;
	fActiveCuts = new TList();  fActiveCuts->SetName("fActiveCuts") ;
	fCmdLine = new TList();  fCmdLine->SetName("fCmdLine") ;

	if (gHpr) {
		fAllFunctions = gHpr->GetFunctionList();
		fAllWindows = gHpr->GetWindowList();
		fAllCuts = gHpr->GetCutList();
		CheckList(fActiveCuts);
		CheckList(fActiveWindows);
		CheckList(fAllWindows);
	}
	fTimer.SetTimerID(0);
	fTimer.Connect("Timeout()", "FitHist", this, "DoSaveLimits()");

	peaks = new TObjArray(0);
	fDimension = hist->GetDimension();
//  look for cuts and windows



	TList *lof = hist->GetListOfFunctions();
	if (lof) {
		TObject *p;
		TIter next(lof);
		while ( (p = (TObject *) next()) ) {
//         cout << "$$$$$$$$$$$$   "  << p << "  $$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
//         p->Dump();
 //        cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
			if (p->InheritsFrom(TMrbWindow::Class())) {
				TMrbWindow *w = dynamic_cast<TMrbWindow*>(p);
				if (fDimension == 2)
					fActiveCuts->Add(w);

				if (fDimension == 1) {
					if (fAllWindows->FindObject(w->GetName()));
	//               cout << p->GetName() << " already existing" <<endl;
					else
						fAllWindows->Add(w);
					if (!fActiveWindows->FindObject(w->GetName()))
						fActiveWindows->Add(w);
				}
			}
		}
	}

	TEnv env(".hprrc");         // inspect ROOT's environment
	fFitMacroName =
		 env.GetValue("HistPresent.FitMacroName", "fit_user_function.C");

//   RestoreDefaultRanges();
//   cout << "FitMacroName " << fFitMacroName.Data()<< endl;

	fTemplateMacro = "TwoGaus";
	fAdjustMinY    = env.GetValue("GeneralAttDialog.fAdjustMinY",1);
	fFill1Dim      = env.GetValue("Set1DimOptDialog.fFill1Dim",  0);
	fFillColor     = env.GetValue("Set1DimOptDialog.fFillColor", 1);
	fLineColor     = env.GetValue("Set1DimOptDialog.fLineColor", 1);
	fLineWidth     = env.GetValue("Set1DimOptDialog.fLineWidth", 1);
	fFillStyle     = env.GetValue("Set1DimOptDialog.fFillStyle", 3001);
	fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
	fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
	fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
	fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
	fDrawAxisAtTop = env.GetValue("Set1DimOptDialog.fDrawAxisAtTop", 0);
	fShowContour   = env.GetValue("Set1DimOptDialog.fShowContour", 0);
	fErrorMode     = env.GetValue("Set1DimOptDialog.fErrorMode", "none");
	fMarkerSize    = env.GetValue("Set1DimOptDialog.fMarkerSize", 1);
	fShowMarkers    = env.GetValue("Set1DimOptDialog.fShowMarkers", 0);
	gStyle->SetErrorX(env.GetValue("Set1DimOptDialog.fErrorX", 0.));
	fSmoothLine  = env.GetValue("Set1DimOptDialog.fSmoothLine",    0);
	fSimpleLine  = env.GetValue("Set1DimOptDialog.fSimpleLine",    0);
	fBarChart    = env.GetValue("Set1DimOptDialog.fBarChart",      0);
	fBarChart3D  = env.GetValue("Set1DimOptDialog.fBarChart3D",    0);
	fBarChartH   = env.GetValue("Set1DimOptDialog.fBarChartH",     0);
	fPieChart    = env.GetValue("Set1DimOptDialog.fPieChart",      0);
	fText        = env.GetValue("Set1DimOptDialog.fText",          0);
	fTextAngle   = env.GetValue("Set1DimOptDialog.fTextAngle",     0);
	fLabelsTopX  = env.GetValue("Set1DimOptDialog.fLabelsTopX",    0);
	fLabelsRightY= env.GetValue("Set1DimOptDialog.fLabelsRightY",  0);

	fDrawOpt2Dim   = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
	f2DimBackgroundColor = env.GetValue("Set2DimOptDialog.f2DimBackgroundColor", 0);
	fHistFillColor2Dim = env.GetValue("Set2DimOptDialog.fHistFillColor2Dim", 1);
	fHistFillStyle2Dim = env.GetValue("Set2DimOptDialog.fHistFillStyle2Dim", 0);
	fHistLineColor2Dim = env.GetValue("Set2DimOptDialog.fHistLineColor2Dim", 1);
	fHistLineStyle2Dim = env.GetValue("Set2DimOptDialog.fHistLineStyle2Dim", 1);
	fHistLineWidth2Dim = env.GetValue("Set2DimOptDialog.fHistLineWidth2Dim", 1);
	fMarkerColor2Dim   = env.GetValue("Set2DimOptDialog.fMarkerColor2Dim",   1);
	fMarkerStyle2Dim   = env.GetValue("Set2DimOptDialog.fMarkerStyle2Dim",   1);
	fMarkerSize2Dim    = env.GetValue("Set2DimOptDialog.fMarkerSize2Dim",    1);
	
	fShowZScale        = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
	if ( fShowZScale != 0 && !fDrawOpt2Dim.Contains("GL") )fDrawOpt2Dim += "Z";
	if ( fSelHist->GetDimension() == 1 ) {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat1Dim", 11111);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox1Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox1Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox1Dim",1);
	} else if ( fSelHist->GetDimension() == 2 ){
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat2Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox2Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox2Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", 1);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle2Dim", 1);
		fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox2Dim",1);
	} else {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat3Dim", 0);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox3Dim", 0);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox3Dim", 0);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay3Dim", 0);
		fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle3Dim", 1);
	}
	fTitleCenterX  = env.GetValue("SetHistOptDialog.fTitleCenterX", 0);
	fTitleCenterY  = env.GetValue("SetHistOptDialog.fTitleCenterY", 0);
	fTitleCenterZ  = env.GetValue("SetHistOptDialog.fTitleCenterZ", 0);
	fOneDimLogX = env.GetValue("Set1DimOptDialog.fOneDimLogX", 0);
	fOneDimLogY = env.GetValue("Set1DimOptDialog.fOneDimLogY", 0);
	fTwoDimLogX = env.GetValue("Set2DimOptDialog.fTwoDimLogX", 0);
	fTwoDimLogY = env.GetValue("Set2DimOptDialog.fTwoDimLogY", 0);
	fTwoDimLogZ = env.GetValue("Set2DimOptDialog.fTwoDimLogZ", 0);
	fRoad2      = env.GetValue("Set2DimOptDialog.fRoad2", 50);
	
	fDrawOpt3Dim   = env.GetValue("Set3DimOptDialog.fDrawOpt3Dim", "");
	fShowZScale3Dim        = env.GetValue("Set3DimOptDialog.fShowZScale", 1);
	if ( fShowZScale3Dim != 0 && !fDrawOpt3Dim.Contains("Z",TString::kIgnoreCase) )fDrawOpt3Dim += "Z";
	fApplyTranspCut    = env.GetValue("Set3DimOptDialog.fApplyTranspCut",    0);
	f3DimBackgroundColor = env.GetValue("Set3DimOptDialog.f3DimBackgroundColor", 0);
	fHistFillColor3Dim = env.GetValue("Set3DimOptDialog.fHistFillColor3Dim", 1);
	fHistLineColor3Dim = env.GetValue("Set3DimOptDialog.fHistLineColor3Dim", 1);
	fMarkerColor3Dim   = env.GetValue("Set3DimOptDialog.fMarkerColor3Dim",   1);
	fMarkerStyle3Dim   = env.GetValue("Set3DimOptDialog.fMarkerStyle3Dim",   7);
	fMarkerSize3Dim    = env.GetValue("Set3DimOptDialog.fMarkerSize3Dim",    1.);
	f3DimPolyMarker    = env.GetValue("Set3DimOptDialog.f3DimPolyMarker",    0);

	fSerialPx = 0;
	fSerialPy = 0;
	fSerialPf = 0;
	fFuncNumb = 0;
	fCutNumber = 0;
	fProjectedBoth = 0;
	wdw_numb = 0;
	if (env.GetValue("HistPresent.FitOptMinos", 0) != 0) fCallMinos = kTRUE;
	else                                                 fCallMinos = kFALSE;
	if (env.GetValue("HistPresent.FitOptKeepPar", 0) != 0)fKeepParameters  = kTRUE;
	else                                                  fKeepParameters = kFALSE;
	fOldMode = -1;
	fColSuperimpose = 1;
	fMax = hist->GetMaximum();
	fMin = hist->GetMinimum();
	fXmin = hist->GetXaxis()->GetXmin();
	fXmax = hist->GetXaxis()->GetXmax();
	fYmax = fYmin = fLogz = 0;
	if (fDimension == 2) {
		fYmin = hist->GetYaxis()->GetXmin();
		fYmax = hist->GetYaxis()->GetXmax();
		fLogx = fTwoDimLogX;
		fLogy = fTwoDimLogY;
		fLogz = fTwoDimLogZ;
	}
	if (fDimension == 1) {
		fLogx = fOneDimLogX;
		fLogy = fOneDimLogY;
		if ( fAdjustMinY == 0 && fLogy == 0 ) 
			fSelHist->SetMinimum(TMath::Min(0.0, fSelHist->GetMinimum()));
	}
	if (fDimension == 3) {
		gStyle->SetFrameFillColor(0); // make sure we can superimpose
	}
	TObject * obj = fSelHist->GetListOfFunctions()->FindObject("palette");
	if (obj) {
		if (gHprDebug > 0)
			cout << " Removing obj TPaletteAxis" << endl;
		fSelHist->GetListOfFunctions()->Remove(obj);
	}
	RestoreDefaultRanges();
	DisplayHist(hist, win_topx, win_topy, win_widx, win_widy);
//	RestoreDefaultRanges();
	ClearMarks();
	fLogx = fCanvas->GetLogx();
	fLogy = fCanvas->GetLogy();
	fLogz = fCanvas->GetLogz();
	fHasExtraAxis = kFALSE;
	if ( !gStyle->GetCanvasPreferGL() )
		TQObject::Connect((TPad*)fCanvas, "Modified()",
							"FitHist", this, "HandlePadModified()");
	gROOT->GetListOfCleanups()->Add(this);
	gROOT->GetList()->Add(this);
	
};
//------------------------------------------------------
// destructor
FitHist::~FitHist()
{
	if (gHprClosing ){
		if ( gHprDebug > 0 )
			cout<< " gHprClosing = 1"<<endl;
		return;
	}
	if ( gHprDebug > 0 ) {
		cout << " ~FitHist(): " << this<< " fSelHist " <<fSelHist
		<< " fCanvas " <<fCanvas << endl;
	}
	gROOT->GetList()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
	if (fSelHist == NULL) {
		return;
	}
   TRootCanvas *rc = (TRootCanvas *)fCanvas->GetCanvasImp();
	rc->DontCallClose();
	fTimer.Stop();
	fCanvas->Disconnect("HTCanvasClosed()", this, "CloseFitHist()");
	if (fTofLabels)
		fTofLabels->Disconnect("Destroyed()", this, "ClearTofl()");
	TQObject::Disconnect((TPad*)fCanvas,this->ClassName(), this, "CloseFitHist()");
	DisconnectFromPadModified();
	if (!fExpHist && gHpr && GeneralAttDialog::fRememberZoom) SaveDefaults(kTRUE);
	if ( fFit1DimD ) fFit1DimD->CloseDialog();
	if ( fFit2DimD ) fFit2DimD->CloseDialog();
	if ( WindowSizeDialog::fNwindows > 0 ) 
		WindowSizeDialog::fNwindows -= 1;
	
	if ( fExpHist == fSelHist) {
      cout << "fExpHist == fSelHist return " << endl;
      return;
	}
	if ( fExpHist ) {
//      cout << "fExpHist " << fExpHist->GetName() << endl;
//      dont delete possible windows
//		fExpHist->GetListOfFunctions()->Clear("nodelete");
		gROOT->GetList()->Remove(fExpHist);
		delete fExpHist;
		fExpHist = 0;
	}
	if (fTofLabels) { delete fTofLabels; fTofLabels=NULL;}
	if (fCalFunc) delete fCalFunc;
//   if (fDateText) delete fDateText;
	if (fCanvas && !gROOT->GetListOfCanvases()->FindObject(fCanvas)) {
		cout << "~FitHist: " << this << " Canvas : " << fCanvas << " is deleted" << endl;
		fCanvas = NULL;
	}

	if (fCanvas) {
		fCanvas->SetFitHist(NULL);
		if (fCanvasIsAlive) {
         cout << " deleting " << fCanvas->GetName() << endl;
			delete fCanvas;
			fCanvas = NULL;
		}
	}
	if (fCutPanel && fCutPanel->TestBit(TObject::kNotDeleted))
		delete fCutPanel;
//   if (fMarkers)
//      ClearMarks();
//      delete fMarkers;
	if (fActiveCuts)
		delete fActiveCuts;
	if (peaks)
		delete peaks;
	if (fCmdLine)
		delete fCmdLine;
	if (gHprDebug > 0)
		cout << setred << "delete fSelHist; " << fSelHist <<setblack << endl;
	delete fSelHist;
};

//_______________________________________________________________________________
void FitHist::CloseFitHist()
{
	if (gHprDebug>0)cout << "CloseFitHist() " << this << endl<< flush;
	fCanvasIsAlive = kFALSE;
	delete this;
}
//_______________________________________________________________________________

void FitHist::DisconnectFromPadModified()
{
	TQObject::Disconnect((TPad*)fCanvas, "Modified()", this, "HandlePadModified()");
	if (gHprDebug > 0)
		cout << " DisconnectFromPadModified this : " << this << " fCanvas " <<fCanvas << endl;
}
//_______________________________________________________________________________

void FitHist::HandlePadModified()
{
//	DoSaveLimits();
	if (gHprDebug>0)cout << "FitHist::HandlePadModified() " << this << endl;
	fTimer.Start(20, kTRUE);   // 2 seconds single-shot
//	TTimer::SingleShot(20, "FitHist", this, "DoSaveLimits()");
}
//_______________________________________________________________________________

void FitHist::DoSaveLimits()
{
	if ( gHprDebug > 1 ) {
		cout << "FitHist::DoSaveLimits: " << gPad 
				<< " fCanvas: " << fCanvas 
				<< " gPad  "  << gPad << " fSelPad->GetLogy() " <<  fSelPad->GetLogy()
				<< " fExpHist " << fExpHist<< endl;
		cout << "x1, x2, y1, y2: "<<fCanvas->GetFrame()->GetX1()<<" "
		<<fCanvas->GetFrame()->GetX2()<< " " <<fCanvas->GetFrame()->GetY1()<<" "
		<<fCanvas->GetFrame()->GetY2()<<endl;
//		fCanvas->Dump();
	}
	if (fSelHist->GetDimension() > 2)
		return;
//	gSystem->Sleep(1000);
//	if (gPad == fCanvas) {
	if ( fLogy != fSelPad->GetLogy() ){
		fLogy = fSelPad->GetLogy();
		if ( fSelHist->GetDimension() == 1 ) {
			fCanvas->GetHandleMenus()->SetLog(fLogy);
		}	if (fSelHist == NULL) {
		return;
	}

	}
	if ( fLogx != fSelPad->GetLogx() ){
		fLogx = fSelPad->GetLogx();
		if ( fSelHist->GetDimension() == 1 ) {
		}
	}
	if ( fSelHist->GetDimension() == 2) {
		if ( fLogz != fSelPad->GetLogz() ){
			fLogz = fSelPad->GetLogz();
			fCanvas->GetHandleMenus()->SetLog(fLogz);
		}
	}
	fXtitle = fSelHist->GetXaxis()->GetTitle();
	fYtitle = fSelHist->GetYaxis()->GetTitle();
	if ( fSelHist->GetDimension() == 3) {
		fYtitle = fSelHist->GetZaxis()->GetTitle();
	}
	if ( fExpHist == NULL ) {
		fBinlx = fSelHist->GetXaxis()->GetFirst();
		fBinux = fSelHist->GetXaxis()->GetLast();
		if ( gHprDebug > 1 ) {
			cout << "fBinlx,ux " <<fBinlx << " " <<fBinux << endl;
		}
		if (fDimension == 2) {
			fBinly = fSelHist->GetYaxis()->GetFirst();
			fBinuy = fSelHist->GetYaxis()->GetLast();
			if ( gHprDebug > 1 ) {
				cout << "fBinly,uy " <<fBinly << " " <<fBinuy << endl;
			}
		}
	}
}
//________________________________________________________________

void FitHist::RecursiveRemove(TObject * obj)
{
	if (gHprDebug > 1){
		cout << "FitHist:: " << this << " fSelHist " <<  fSelHist << " RecursiveRemove: " ;
		if (obj) cout << obj << " name  " <<obj->GetName() << " class " << obj->ClassName();
		cout << endl;
	}
	if (fSelHist == NULL) {
		return;
	}
	//fSelHist->Print();
	if (gHprClosing != 0) {
		if (gHprDebug > 0)
			cout << "FitHist::RecursiveRemove: gHprClosing " << gHprClosing << endl;
		return;
	}
	if (fSelHist && obj != fSelHist) {
		if (gHprDebug > 2) {
			cout << "fSelHist->GetListOfFunctions()->GetSize() " 
				<< fSelHist->GetListOfFunctions()->GetSize() << endl;
//			if (fSelHist->GetListOfFunctions()->GetSize() > 0) 
//				fSelHist->GetListOfFunctions()->Print();
		}
		if (fSelHist->GetListOfFunctions()->GetSize() > 0)
			fSelHist->GetListOfFunctions()->Remove(obj);
//		else
//			fSelHist = NULL;
	}
	if (fActiveCuts) fActiveCuts->Remove(obj);
	if (fActiveWindows) fActiveWindows->Remove(obj);
	if (fActiveFunctions) fActiveFunctions->Remove(obj);
	if (obj == fFit1DimD) fFit1DimD = NULL;
	if (obj == fFit2DimD) fFit2DimD = NULL;
}

//________________________________________________________________

void FitHist::SaveDefaults(Bool_t /*recalculate*/)
{
	if (!gHpr) return;      // not called from  Histpresenter
	if (!GeneralAttDialog::fRememberLastSet &&  !GeneralAttDialog::fRememberZoom) return;
		
//   if (!GeneralAttDialog::fRememberLastSet &&  !GeneralAttDialog::fRememberZoom) return;
//   cout << "Enter SaveDefaults " << endl;

	Bool_t checkonly = kFALSE;
	if ( (!CreateDefaultsDir(mycanvas, checkonly)) ) return;

	TEnv * env = GetDefaults(fHname, kFALSE, kFALSE); // fresh values
	if (!env) return;
	
	env->SetValue("FitMacroName", fFitMacroName);
	if (fDimension == 1) {
		if ( fLogx != fOneDimLogX ) 
			env->SetValue("LogX", fLogx);
		if ( fLogy != fOneDimLogY ) 
			env->SetValue("LogY", fLogy);
	}
	if (fDimension == 2) {
		if ( fLogx != fTwoDimLogX ) 
			env->SetValue("LogX", fLogx);
		if ( fLogy != fTwoDimLogY ) 
			env->SetValue("LogY", fLogy);
		if ( fLogz != fTwoDimLogZ ) 
			env->SetValue("LogZ", fLogz);
		env->SetValue("fBinly", fBinly);
		env->SetValue("fBinuy", fBinuy);
	}
	env->SetValue("fBinlx", fBinlx);
	env->SetValue("fBinux", fBinux);
	if (fSetRange) {
		env->SetValue("fRangeLowX", fRangeLowX);
		env->SetValue("fRangeUpX",  fRangeUpX);
		if (fDimension == 2) {
			env->SetValue("fRangeLowY", fRangeLowY);
			env->SetValue("fRangeUpY",  fRangeUpY);
		}
	}
	if (GeneralAttDialog::fRememberTitle && fSelHist && fSelHist->TestBit(TObject::kNotDeleted)) {
		if (fXtitle.Length() > 0)
			env->SetValue("fXtitle", fXtitle);
		if (fYtitle.Length() > 0)
			env->SetValue("fYtitle", fYtitle);
		if (fZtitle.Length() > 0)
			env->SetValue("fZtitle", fZtitle);
	}
	if (gHprDebug > 1)
		cout << "env->SaveLevel(kEnvLocal): " << fBinlx << " " 
		<< fBinux << " " << fLogy << endl;
	env->SaveLevel(kEnvLocal);
	delete env;
	return;
}
//________________________________________________________________

void FitHist::RestoreDefaultRanges()
{
	if (fSelHist->GetDimension() > 2) 
		return;
	TEnv * lastset = GetDefaults(fHname);
	if (!lastset) 
		return;
	fFitMacroName = lastset->GetValue("FitMacroName",fFitMacroName.Data());
	if (gHpr && (GeneralAttDialog::fRememberLastSet || GeneralAttDialog::fRememberZoom)) {
		fLogx = lastset->GetValue("LogX", fOneDimLogX);
		fLogy = lastset->GetValue("LogY", fOneDimLogY);
		fRangeLowX = fSelHist->GetXaxis()->GetXmin();
		fRangeUpX = fSelHist->GetXaxis()->GetXmax();
		if (lastset->Lookup("fRangeLowX")) {
			fRangeLowX = lastset->GetValue("fRangeLowX", fRangeLowX);
			fRangeUpX = lastset->GetValue("fRangeUpX", fRangeUpX);
			fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(), fRangeLowX, fRangeUpX);
			fSetRange = kTRUE;
//         cout << "fRangeLowX " << fRangeLowX << endl;
		}
//		cout << "lastset->Lookup " << fHname << endl;
		if (fSelHist->GetDimension() == 1 && lastset->Lookup("fRangeLowY")) {
//			cout << "lastset->Lookup " << endl;
			fSelHist->SetMinimum((Double_t)lastset->GetValue("fRangeLowY",(Double_t)fSelHist->GetMinimum()));
			fSelHist->SetMaximum((Double_t)lastset->GetValue("fRangeUpY", (Double_t)fSelHist->GetMaximum()));
		}
		fBinlx = fSelHist->GetXaxis()->GetFirst();
		fBinux = fSelHist->GetXaxis()->GetLast();
		if (lastset->Lookup("fBinlx")) {
			fBinlx = lastset->GetValue("fBinlx", fBinlx);
			fBinux = lastset->GetValue("fBinux", fBinux);
			fSelHist->GetXaxis()->SetRange(fBinlx, fBinux);
		}

		if (GeneralAttDialog::fRememberTitle && lastset->Lookup("fXtitle"))
			fSelHist->GetXaxis()->SetTitle(lastset->GetValue("fXtitle", ""));
		if (GeneralAttDialog::fRememberTitle && lastset->Lookup("fYtitle"))
			fSelHist->GetYaxis()->SetTitle(lastset->GetValue("fYtitle", ""));

		if (is2dim(fSelHist)) {
			fLogx = lastset->GetValue("LogX", fTwoDimLogX);
			fLogy = lastset->GetValue("LogY", fTwoDimLogY);
			fLogz = lastset->GetValue("LogZ", fTwoDimLogZ);
			fRangeLowY = fSelHist->GetYaxis()->GetXmin();
			fRangeUpY = fSelHist->GetYaxis()->GetXmax();
			if (lastset->Lookup("fRangeLowY")) {
				fRangeLowY = lastset->GetValue("fRangeLowY", fRangeLowY);
				fRangeUpY = lastset->GetValue("fRangeUpY", fRangeUpY);
				fSelHist->GetYaxis()->Set(fSelHist->GetNbinsY(), fRangeLowY,
											 fRangeUpY);
				fSetRange = kTRUE;
			}
			fBinly = fSelHist->GetYaxis()->GetFirst();
			fBinuy = fSelHist->GetYaxis()->GetLast();
			if (lastset->Lookup("fBinly")) {
				fBinly = lastset->GetValue("fBinly", fBinly);
				fBinuy = lastset->GetValue("fBinuy", fBinuy);
				fSelHist->GetYaxis()->SetRange(fBinly, fBinuy);
			}
			if (lastset->Lookup("Contours")) {
				fUserContourLevels = lastset->GetValue("Contours", 0);
				if (fUserContourLevels > 0) {
					TString temp;
					Double_t * uc = new Double_t[fUserContourLevels];
			 		TMrbNamedArrayI * colors = new TMrbNamedArrayI("Pixel", fHname.Data());
					colors->Set(fUserContourLevels);
					Double_t all_levels = 0;
					for (Int_t i = 0; i < fUserContourLevels; i++) {
						temp = "Cont";
						temp += i;
						uc[i] = lastset->GetValue(temp.Data(), (Double_t)0);
						all_levels += uc[i];
						temp = "Pix";
						temp += i;
						(*colors)[i] = lastset->GetValue(temp.Data(), (Int_t)0);
					}
					if (all_levels != 0) fSelHist->SetContour(fUserContourLevels, uc);

//   				Int_t allcolors = 0;
//   				for (Int_t i=0; i< fUserContourLevels; i++) allcolors += (*colors)[i];
					if (colors->GetSum() != 0) {
				 //  set new Palette
	  					fSelHist->GetListOfFunctions()->Add(colors);
//                  colors->Dump();
						Hpr::SetUserPalette(1001, colors);
					}
					if (uc) delete [] uc;
				}
			}
		}
	}
	if (lastset)
		delete lastset;
//	cout << "RestoreDefaultRanges() fLogz " << fLogz<< endl;
}
//______________________________________________________________________________________

void FitHist::handle_mouse()
{
	Double_t fChi2Limit = 10;
	TString likelyhood = "";
	TString fitopt;
	static TF1 * gFitGauss = NULL;
	static TF1 * gFitBG = NULL;
	static Bool_t gFitGaussNotYetDrawn = kTRUE;
	static Bool_t gFitBGNotYetDrawn = kTRUE;
	Double_t cont, sum, mean;
	static Double_t gconst, center, sigma, bwidth,  esigma;
	static Double_t startbinX_lowedge, startbinY_lowedge;
	static Double_t chi2 = 0;
	static Int_t startbinX, startbinY;
	static Double_t ratio1, ratio2;
	static Int_t px1old, py1old, px2old, py2old;

	static Bool_t is2dim = kFALSE;
	static Bool_t first_time = kTRUE;
	static Bool_t skip_after_TCUTG = kFALSE;
	static Bool_t TCUTG_moved = kFALSE;
	static TCutG * selected_cut = 0;
	static TH1 * hist = 0;
	static Int_t npar = 0;
	static TLine * lowedge = 0;
	static TLine * upedge = 0;
	static TBox * box = 0;
	static Int_t nrows = 4;
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi());

	static Double_t sum_in_cut = 0;
	static Double_t meanx_in_cut = 0;
	static Double_t sigmax_in_cut = 0;
	static Double_t meany_in_cut = 0;
	static Double_t sigmay_in_cut = 0;
	static Int_t cut_stat_filled = 0;

//	static Int_t InitSetSP = 0;
	Int_t px, py;
	
	if (gROOT->GetEditorMode() != 0) return;
	if (gStyle->GetCanvasPreferGL()) return;
	TEnv env(".hprrc");
	Int_t event = gPad->GetEvent();
//	cout << "px: "  << (char)gPad->GetEventX() << endl;
	if (event ==  kKeyPress) {
//		cout << "px: "  << (char)gPad->GetEventX() << endl;
		char ch = (char)gPad->GetEventX();
		if ( ch == 'M' ||ch == 'm') {
			if (cut_stat_filled > 0 ) {
				cut_stat_filled = 0;
				FhMarker * mm = new FhMarker(meanx_in_cut, meany_in_cut,34);
				mm->SetErrX(sigmax_in_cut / TMath::Sqrt(sum_in_cut));
				mm->SetErrY(sigmay_in_cut / TMath::Sqrt(sum_in_cut));
				fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
				if (fMarkers == NULL) {
					fMarkers = new  FhMarkerList();
					fSelHist->GetListOfFunctions()->Add(fMarkers);
				}
				fMarkers->Add(mm);
				mm->SetMarkerColor(2);
				mm->SetMarkerSize(2);
				mm->Draw();
				gPad->Modified();
				gPad->Update();
				cout << "cut_stat: Sum " << sum_in_cut <<
				" MeanX: " << meanx_in_cut << " SigmaX " << sigmax_in_cut <<
				" MeanY: " << meany_in_cut << " SigmaY " << sigmay_in_cut
				<< endl;
			}
		}
		if ( ch == 'Q' ||ch == 'q') {
			TCanvas * canpro = Hpr::FindCanvas("_projection_");
			if ( canpro ) {
				TString cname = canpro->GetName();
				// hide canvas from root
				TRegexp pr("_projection_");
				cname(pr) = "_Projection_";
				canpro->SetName(cname);
				env.SetValue("Set2DimOptDialog.fNbinLiveSliceX",0);
				env.SetValue("Set2DimOptDialog.fNbinLiveSliceY",0);
				if (fCanvas->GetHandleMenus() ) {
					fCanvas->GetHandleMenus()->SetLiveSliceX(0);
					fCanvas->GetHandleMenus()->SetLiveSliceY(0);
				}
				env.SaveLevel(kEnvLocal);
			}
			gSystem->ProcessEvents();
			fCanvas->Modified();
			fCanvas->Update();
			return;
		}
		if ( ch == 'C' ||ch == 'c') {
			TRootCanvas *rc =  (TRootCanvas*)fCanvas->GetCanvasImp();
//        if (fCanvas->GetAutoExec()) fCanvas->ToggleAutoExec();
//         gSystem->Sleep(200);
			rc->ShowEditor(kFALSE);
			rc->SendCloseMessage();
			return;
		}
	}
	if ( TCUTG_moved && event == kButton1Up ) {
		if  (fProjectedBoth == 1 ) {
			if ( gHprDebug > 0 )
				cout << " ProjectBoth " << endl;
			ProjectBoth();
		} else if ( fDimension == 2 && selected_cut !=NULL) {
			sum_in_cut = 0;
			Double_t sumx_in_cut = 0, sumx2_in_cut = 0, sumy_in_cut = 0, sumy2_in_cut = 0;
			TAxis *xa = fSelHist->GetXaxis();
			TAxis *ya = fSelHist->GetYaxis();
			for (Int_t ix = 1; ix < xa->GetNbins(); ix++) {
				for (Int_t iy = 1; iy < ya->GetNbins(); iy++) {
					Double_t bcx = xa->GetBinCenter(ix);
					Double_t bcy = ya->GetBinCenter(iy);
					if(selected_cut->IsInside(bcx, bcy )) {
						Double_t bc = fSelHist->GetBinContent(ix,iy);
						sum_in_cut += bc;
						sumx_in_cut  += bc * bcx;
						sumx2_in_cut += bc * bcx * bcx;
						sumy_in_cut  += bc * bcy;
						sumy2_in_cut += bc * bcy * bcy;
					}
				}
			}
			if (sum_in_cut > 0.) {
				meanx_in_cut = sumx_in_cut / sum_in_cut;
				sigmax_in_cut = TMath::Sqrt(sumx2_in_cut /
													sum_in_cut - meanx_in_cut * meanx_in_cut);
				meany_in_cut = sumy_in_cut / sum_in_cut;
				sigmay_in_cut = TMath::Sqrt(sumy2_in_cut /
													sum_in_cut - meany_in_cut * meany_in_cut);
				cut_stat_filled = 1;
				cout << "Sum in cut: " << sum_in_cut <<
				" MeanX: " << meanx_in_cut << " SigmaX " << sigmax_in_cut <<
				" MeanY: " << meany_in_cut << " SigmaY " << sigmay_in_cut
				<< endl;
			}
		}
		TCUTG_moved = kFALSE;
		selected_cut = NULL;
	}
	TObject *select = gPad->GetSelected();
	if (!select) return;
//	cout << select->ClassName() << " event " << event << endl;
	if (event == kButton1Up && skip_after_TCUTG) {
		skip_after_TCUTG = kFALSE;
		return;
	}
	if (event == kButton1Down) {
		if (gROOT->FindObject("CUTG")) {
			cout << "handle_mouse(): UpdateCut();" << endl;
			UpdateCut();
			skip_after_TCUTG = kTRUE;
			return;
		}
	}
	if (select->InheritsFrom("TPave")) return;
	if (event == kButton1Motion && select->InheritsFrom("TCutG")) {
		selected_cut = (TCutG*)select;
		TCUTG_moved = kTRUE;
		if ( gHprDebug > 0 )
			cout << select->ClassName() << " event " << event << endl;
		return;
	}
			
	px = gPad->GetEventX();
	py = gPad->GetEventY();
	if (event == kButton1Down) {
//      cout << "handle_mouse select " << select->ClassName() << endl;
		if (select->InheritsFrom("TH2") && !gSystem->AccessPathName("fh_user_macro_2d.C")) {
			TString cmd("fh_user_macro_2d.C(");
			cmd += gPad->AbsPixeltoX(px);
			cmd += ",";
			cmd += gPad->AbsPixeltoY(py);
			cmd += ")";
//			cout << cmd  << endl;
			gROOT->Macro(cmd);
			return;
		}
		if (select->InheritsFrom("TF1")) {
			TF1 *f = (TF1*)select;
			cout  << endl << "Function " << f->GetName() << ": " << f->GetTitle()<< endl;
			cout << "Chi2 / NDF " << f->GetChisquare() << "/" << f->GetNDF() << endl;
			for (Int_t i = 0; i < f->GetNpar(); i++) {
				cout << f->GetParName(i) << "  " << f->GetParameter(i)
				<< " +- " <<  f->GetParError(i)<< endl;
			}
			cout  << endl << "----------------------------" << endl;
			TH1 * h = Hpr::FindHistOfTF1(gPad, f->GetName(), 1);
			if (h) {
				cout << "popped " << f << endl;
				TIter next(h->GetListOfFunctions());
				TObject *objf;
				while (objf = next()) {
					if (objf->InheritsFrom("TF1")){
						objf->ResetBit(kSelected);
					}
				}
			}
			f->SetBit(kSelected);
			return;
		}
	}
//   if ( (!fLiveStat1Dim && fDimension == 1) || (!fLiveStat2Dim && fDimension == 2) ||
//         fDimension == 3 )  return;
	if ( fDimension == 3 )  return;
//	TEnv env(".hprrc");
	if ( fDimension == 1) { 
		fLiveStat1Dim = (Int_t)env.GetValue("Set1DimOptDialog.fLiveStat1Dim",0);
		fLiveGauss    = (Int_t)env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
		if (fLiveGauss > 0)		// force stat box when fitting gauss
			fLiveStat1Dim = 1;
		if (fLiveStat1Dim == 0)
			return;
	}
	if ( fDimension == 2) {
		Int_t NbinSliceX = env.GetValue("Set2DimOptDialog.fNbinLiveSliceX",0);
		Int_t NbinSliceY = env.GetValue("Set2DimOptDialog.fNbinLiveSliceY",0);
		if (NbinSliceX  > 0 ) {
			THistPainter * hip= (THistPainter*)fSelHist->GetPainter();
			if (hip) {
				TCanvas * canpro = Hpr::FindCanvas("_projection_");
				if ( canpro == NULL ) {
					hip->SetShowProjection("x", NbinSliceX);
					
					canpro = Hpr::FindCanvas("_projection_");
					if (canpro == NULL) {
						cout << "No canvas created, strange" << endl;
						return;
					}
					UInt_t ww, wh;
					Int_t wx, wy;
					wx = fCanvas->GetWindowTopX();
					wy = fCanvas->GetWindowTopY();
					ww = fCanvas->GetWindowWidth();
					wh = fCanvas->GetWindowHeight();
					canpro->SetWindowSize(500,wh/2-20);
					canpro->SetWindowPosition(wx+ww+4, wy-20);
					cout<< setblue << "To terminate press: \"q\"" << setblack<< endl;
				}
				hip->ShowProjectionX(px,py);
				return;
			}
		} else if (NbinSliceY  > 0 ) {
			THistPainter * hip= (THistPainter*)fSelHist->GetPainter();
			if (hip) {
				TCanvas * canpro = Hpr::FindCanvas("_projection_");
				if ( canpro == NULL ) {
					hip->SetShowProjection("y", NbinSliceY);
					
					canpro = Hpr::FindCanvas("_projection_");
					if (canpro == NULL) {
						cout << "No canvas created, strange" << endl;
						return;
					}
					UInt_t ww, wh;
					Int_t wx, wy;
					wx = fCanvas->GetWindowTopX();
					wy = fCanvas->GetWindowTopY();
					ww = fCanvas->GetWindowWidth();
					wh = fCanvas->GetWindowHeight();
					canpro->SetWindowSize(500,wh/2-20);
					canpro->SetWindowPosition(wx+ww+4, wy + wh/2);
					cout<< setblue << "To terminate press: \"q\"" << setblack<< endl;
				}
				hip->ShowProjectionY(px,py);
				return;
			}
		} else {
			fLiveStat2Dim = (Int_t)env.GetValue("Set2DimOptDialog.fLiveStat2Dim",0);
			if (fLiveStat2Dim == 0)
			return;
		}
//		return;
	}
	
	if (event == kButton1Down) {
		if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
			||  (select->InheritsFrom("TCanvas") && IsInsideFrame(fCanvas, px, py))) {
			TList * lofp = gPad->GetListOfPrimitives();
			if (!lofp) return;
			TIter next(lofp);
			TObject * obj = 0;
			while ( (obj = next()) ) {
				if (obj->InheritsFrom("TH3")) return;
				if (obj->InheritsFrom("TH1")) hist = (TH1*)obj;
			}
			if (!hist) {
				cout << "No histogram found" << endl;
				return;
			}
			TString sopt = hist->GetOption();
//         cout << " sopt " << sopt << endl;
			if (sopt.Contains("SURF", TString::kIgnoreCase) ||
				 sopt.Contains("LEGO", TString::kIgnoreCase)) {
				 return;
			}

			if (hist->GetDimension() == 3) {
//            cout << "3d histogram not supported" << endl;
				return;
			} else if (hist->GetDimension() == 2){
				is2dim = kTRUE;
			} else {
				is2dim = kFALSE;
			}

			if (lowedge) {
				delete lowedge; lowedge = 0;
				delete upedge; upedge = 0;
			}

			if (box) {delete box; box = 0;};
			if (gFitGauss) { delete gFitGauss; gFitGauss = 0;}
			if (gFitBG)   { delete gFitBG; gFitBG = 0;}
			first_time = kTRUE;
			Axis_t xx = gPad->AbsPixeltoX(px);
			startbinX = hist->GetXaxis()->FindBin(xx);
			startbinX_lowedge = hist->GetXaxis()->GetBinLowEdge(startbinX);

			if (is2dim) {
				Axis_t yy = gPad->AbsPixeltoY(py);
				startbinY = hist->GetYaxis()->FindBin(yy);
				startbinY_lowedge = hist->GetYaxis()->GetBinLowEdge(startbinY);
				cont = hist->GetBinContent(startbinX, startbinY);
				nrows = 4;
			} else {
				bwidth = hist->GetBinWidth(startbinX);
				esigma = hist->GetBinWidth(startbinX) * TMath::Sqrt(12.);
				cont = hist->GetBinContent(startbinX);
				gconst = cont * bwidth / sqrt2pi * esigma;
				center = hist->GetBinCenter(startbinX);
				fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
				fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
				fLiveConstBG   = env.GetValue("Set1DimOptDialog.fLiveConstBG", 0);

				if (fLiveGauss) {
					if (fLiveBG) {
						gFitGauss = new TF1("fitfunc", "pol1+gaus(2)");
						gFitBG = new TF1("bgfunc", "pol1");
						npar = 5;
						if (nrows != 6) {delete fTofLabels; fTofLabels = 0;}
						nrows = 6;
					} else if (fLiveConstBG) {
						gFitGauss = new TF1("fitfunc", "pol0+gaus(1)");
						gFitGauss->SetParLimits(0, hist->GetMinimum(), hist->GetMaximum());
						gFitBG = new TF1("bgfunc", "pol0");
					  npar = 5;
						if (nrows != 6) {delete fTofLabels; fTofLabels = 0;}
						nrows = 6;
					} else {
						gFitGauss = new TF1("fitfunc", "gaus");
 //                 gFitGauss ->SetParameters(gconst, center, esigma);
					  npar = 3;
					  if (nrows != 5) {delete fTofLabels; fTofLabels = 0;}
					  nrows = 5;
					}
					gFitGaussNotYetDrawn = kTRUE;
					gFitBGNotYetDrawn = kTRUE;
					gFitGauss->SetLineColor(kRed);
					gFitGauss->SetLineWidth(3);
					if ( gFitBG ) {
						gFitBG->SetLineColor(kCyan);
						gFitBG->SetLineWidth(2);
					}
				} else {
					if (nrows != 4) {delete fTofLabels; fTofLabels = 0;}
					nrows = 4;
				}
			}
//         if ( kTRUE ) {
/*         if ( !is2dim )
				fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
			if ( is2dim )
				fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
*/
			if ( (fLiveStat1Dim && !is2dim ) || (fLiveStat2Dim && is2dim) ) {
				if (!fTofLabels) {
					TOrdCollection rowlabels;
					if (is2dim) {
						rowlabels.Add(new TObjString("Low left Bin: binx,biny, x,y"));
						rowlabels.Add(new TObjString("Up right Bin: binx,biny),x,y"));
					} else {
						rowlabels.Add(new TObjString("Low left Bin: Number, Low edge"));
						rowlabels.Add(new TObjString("Up right Bin: Number, Up edge"));
					}
					rowlabels.Add(new TObjString("Current Bin: Content "));
					if (is2dim)
						rowlabels.Add(new TObjString("Integral, Mean Z"));
					else
						rowlabels.Add(new TObjString("Integral, Mean"));
					TOrdCollection values;
					if (is2dim) {
						values.Add(new TObjString(Form("%d, %d, %lg, %lg",
									 startbinX, startbinY, startbinX_lowedge , startbinY_lowedge)));
						values.Add(new TObjString(Form("%d, %d, %lg, %lg",
									 startbinX, startbinY,
									 startbinX_lowedge + hist->GetBinWidth(startbinX),
									 startbinY_lowedge + hist->GetYaxis()->GetBinWidth(startbinY))));
						values.Add(new TObjString(Form("%lg", cont)));
						values.Add(new TObjString(Form("%lg", cont)));

					} else {
						TString timestring;
						if (hist->GetXaxis()->GetTimeDisplay()) {
							ConvertTimeToString((time_t)startbinX_lowedge, hist->GetXaxis(), &timestring);
							timestring.Prepend(Form("%d,", startbinX));
							values.Add(new TObjString(timestring.Data()));
							ConvertTimeToString((time_t)(startbinX_lowedge + hist->GetBinWidth(startbinX)),
																  hist->GetXaxis(), &timestring);
							timestring.Prepend(Form("%d,", startbinX));
							values.Add(new TObjString(timestring.Data()));

						} else {
							values.Add(new TObjString(Form("%d, %lg", startbinX, startbinX_lowedge )));
							values.Add(new TObjString(Form("%d, %lg", startbinX, startbinX_lowedge
								  + hist->GetBinWidth(startbinX))));
						}
						values.Add(new TObjString(Form("%lg", cont)));
						values.Add(new TObjString(Form("%lg", cont)));
						 if (fLiveGauss) {
							 if (fLiveBG) {
								 rowlabels.Add(new TObjString("Backgd: content, const, slope"));
								 values.Add(new TObjString("no fit done yet"));
							 }  else if (fLiveConstBG) {
								 rowlabels.Add(new TObjString("Backgd: content, const"));
								 values.Add(new TObjString("no fit done yet"));
							 }

							 rowlabels.Add(new TObjString("Gaus: Cont, Mean, Sig, Xi2"));
							 values.Add(new TObjString("no fit done yet"));
						}
					}
					TString title("Stat. in sel. region of: ");
					title += fHname.Data();
					Int_t xw = gPad->GetCanvas()->GetWindowTopX();
					Int_t yw = gPad->GetCanvas()->GetWindowTopY() + gPad->GetCanvas()->GetWindowHeight() + 28;
					fTofLabels = new TableOfLabels(0, &title, 1, nrows, &values,
									  0, &rowlabels, xw, yw);
					fTofLabels->Connect("Destroyed()", "FitHist", this, "ClearTofl()");
	//            pressed = kTRUE;
				} else {
					fTofLabels->SetLabelText(0,0,Form("%d", startbinX));
					fTofLabels->SetLabelText(0,1,Form("%d", startbinX));
					fTofLabels->SetLabelText(0,2,Form("%lg", cont));
					fTofLabels->SetLabelText(0,3,Form("%lg", cont));
				}
			}
			ratio1 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
			px1old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio1*(gPad->GetUxmax() - gPad->GetUxmin()));
			if (is2dim) {
				px2old = gPad->XtoAbsPixel(gPad->GetUxmax());
				Double_t yru = gPad->GetUymax() - gPad->GetUymin();
				ratio1 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru ;
				Double_t yu = gPad->GetUymin() + ratio1 * yru;
				py1old = gPad->YtoAbsPixel(yu);
				py2old = gPad->YtoAbsPixel(gPad->GetUymax());
			} else {
				px2old = px1old;
				py1old = gPad->YtoAbsPixel(gPad->GetUymin());
				py2old = gPad->YtoAbsPixel(gPad->GetUymax());
			}
			gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
			gVirtualX->SetLineColor(-1);
		}
	}  else if (event == kButton1Motion || event == kButton1Up) {
//      if(!hist || !fTofLabels) return;
		if(!hist) return;
		if (select->InheritsFrom("TH1")) {
			TString sopt = hist->GetOption();
			if (sopt.Contains("SURF", TString::kIgnoreCase) ||
				 sopt.Contains("LEGO", TString::kIgnoreCase)) {
				 return;
			}
		}

		if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
			||  (select->InheritsFrom("TCanvas") && IsInsideFrame(fCanvas, px, py))){
			if (select->InheritsFrom("TH3")) return;
			px = gPad->GetEventX();
			Axis_t xx = gPad->AbsPixeltoX(px);
			Axis_t yy = gPad->AbsPixeltoY(py);
			Int_t binX = hist->GetXaxis()->FindBin(xx);
			Int_t binY;
			Int_t binXlow, binXup, binYlow, binYup;
			Axis_t XlowEdge, XupEdge, YlowEdge = 0, YupEdge = 0;
			binXlow = TMath::Min(startbinX, binX);
			binXup  = TMath::Max(startbinX, binX);
			XlowEdge = hist->GetXaxis()->GetBinLowEdge(binXlow);
			XupEdge  = hist->GetXaxis()->GetBinLowEdge(binXup) + hist->GetXaxis()->GetBinWidth(binXup);

			if (is2dim) {
				binY = hist->GetYaxis()->FindBin(yy);
				binYlow = TMath::Min(startbinY, binY);
				binYup  = TMath::Max(startbinY, binY);
				YlowEdge = hist->GetYaxis()->GetBinLowEdge(binYlow);
				YupEdge  = hist->GetYaxis()->GetBinLowEdge(binYup) + hist->GetYaxis()->GetBinWidth(binYup);
				if (fTofLabels) {
//            if (fLiveStat2Dim && fTofLabels) {
				 	cont = hist->GetBinContent(binX, binY);
					sum = ((TH2*)hist)->Integral(binXlow, binXup, binYlow, binYup);
					Int_t totbins = (binYup - binYlow +1) * (binXup - binXlow +1);
					if (totbins > 0) mean = sum / (Double_t)totbins;
					else             mean = 0;

					fTofLabels->SetLabelText(0,0,Form("%d, %d, %lg, %lg", binXlow, binYlow, XlowEdge, XupEdge));
					fTofLabels->SetLabelText(0,1,Form("%d, %d,  %lg, %lg", binXup,  binYup,  YlowEdge, YupEdge));
					fTofLabels->SetLabelText(0,2,Form("%lg", cont));
					fTofLabels->SetLabelText(0,3,Form("%lg, %lg", sum, mean));
				}
			} else {
				if (fTofLabels) {
//              if (fLiveStat1Dim && fTofLabels) {
					Double_t sumx = 0;
					cont = hist->GetBinContent(binX);
					sum = hist->Integral(binXlow, binXup);
//           	Int_t totbins = (binXup - binXlow +1);
					for (Int_t ibin = binXlow; ibin <= binXup; ibin++)
						sumx = sumx + hist->GetBinContent(ibin) * hist->GetBinCenter(ibin);
					if (sum > 0) mean = sumx / sum;
					else             mean = 0;
					TString timestring;
					if (hist->GetXaxis()->GetTimeDisplay()) {
						ConvertTimeToString((time_t)XlowEdge, hist->GetXaxis(), &timestring);
						timestring.Prepend(Form("%d,", binXlow));
						fTofLabels->SetLabelText(0, 0, timestring.Data());
						ConvertTimeToString((time_t)XupEdge, hist->GetXaxis(), &timestring);
						timestring.Prepend(Form("%d,", binXup));
						fTofLabels->SetLabelText(0, 1, timestring.Data());
					} else {
						fTofLabels->SetLabelText(0,0,Form("%d,%lg", binXlow, XlowEdge));
						fTofLabels->SetLabelText(0,1,Form("%d,%lg", binXup, XupEdge ));
					}
					fTofLabels->SetLabelText(0,2,Form("%lg", cont));
					fTofLabels->SetLabelText(0,3,Form("%lg,%lg", sum, mean));
					// set initial parameters
					if (fLiveGauss &&  (binXup - binXlow - npar) > 0) {
						Int_t ndf = binXup - binXlow - npar;
						if (!first_time) chi2 = gFitGauss->GetChisquare();
						
						Int_t paroff = 0;
						if ( fLiveBG )
							paroff = 2;
						else if ( fLiveConstBG )
							paroff = 1;
						if (first_time                    ||
						gFitGauss->GetParameter(0 + paroff) < 0 ||
						gFitGauss->GetParameter(2 + paroff) < 0 ||
						chi2 / ndf > fChi2Limit) {
//                     	 cout << " old par at bin " << binX << ": ";
//                     	 for (Int_t i = 0; i < gFitGauss->GetNpar(); i++)
//                        	cout  << gFitGauss->GetParameter(i) << " ";
//                     	 cout << chi2 << " " << ndf << endl;
							if ( paroff > 0 ) gFitGauss->SetParameter(0, cont);
							if ( paroff > 1 ) gFitGauss->SetParameter(1, 0);
							gFitGauss->SetParameter(0 + paroff, sum * bwidth);
							gFitGauss->SetParameter(1 + paroff, hist->GetMean() );
							gFitGauss->SetParameter(2 + paroff, hist->GetRMS() );
/*                    	 cout << " new par: bg cont " << cont << " gaus cont "  << sum * bwidth << " mean "
									  << hist->GetMean()<< "  sig "
									  << hist->GetRMS() << endl;*/
						}
						fitopt = "RNQ";
						fitopt += likelyhood;
						hist->Fit(gFitGauss, fitopt, "",XlowEdge ,XupEdge );
						first_time = kFALSE;
						gFitGauss->SetRange(XlowEdge ,XupEdge);
						if ( gFitGaussNotYetDrawn ) {
							gFitGauss->Draw("same");
							gFitGaussNotYetDrawn = kFALSE;
						}
						Double_t bcont = 0;
						if (fLiveBG || fLiveConstBG) {
							gFitBG->SetParameter(0, gFitGauss->GetParameter(0));
							if ( fLiveBG )
								gFitBG->SetParameter(1, gFitGauss->GetParameter(1));
							gFitBG->SetRange(XlowEdge ,XupEdge);
							if ( gFitBGNotYetDrawn ) {
								gFitBG->Draw("same");
								gFitBGNotYetDrawn = kFALSE;
							}
						}
						gPad->Modified();
						gPad->Update();
						gPad->GetCanvas()->GetFrame()->SetBit(TBox::kCannotMove);
						chi2 = gFitGauss->GetChisquare() / (Double_t)TMath::Max(gFitGauss->GetNDF(),1);

						if ( fLiveBG ) {
							Double_t a = gFitGauss->GetParameter(0);
							Double_t b = gFitGauss->GetParameter(1);
							bcont = (a + 0.5 * b * (XupEdge+XlowEdge))*(XupEdge-XlowEdge) / bwidth;
							fTofLabels->SetLabelText(0,4,Form("%lg,%lg,%lg", bcont, a, b));
						}		
						else if (fLiveConstBG) {
							Double_t a = gFitGauss->GetParameter(0);
							bcont = a * (XupEdge-XlowEdge) / bwidth;
							fTofLabels->SetLabelText(0,4,Form("%lg,%lg", bcont, a));
						}		
						sigma = gFitGauss->GetParameter(paroff + 2);
						gconst = gFitGauss->GetParameter(paroff + 0) * sqrt2pi * sigma / bwidth;
						center = gFitGauss->GetParameter(paroff + 1);
						Int_t laboff = 0;
						if (paroff > 0) laboff =1;
						fTofLabels ->SetLabelText(0,laboff + 4, Form("%lg,%lg,%lg,%lg",
											 gconst, center, sigma, chi2));
					}
				}
			}
			if ( event == kButton1Motion) {
				gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
				ratio2 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
				px2old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio2*(gPad->GetUxmax() - gPad->GetUxmin()));
				if (is2dim) {
					Double_t yru = gPad->GetUymax() - gPad->GetUymin();
					ratio2 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru;
					py2old = gPad->YtoAbsPixel(gPad->GetUymin() + ratio2 * yru );
				}
				gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
				gVirtualX->SetLineColor(-1);
			} else {
				ClearMarks();
				fMarkers = new FhMarkerList();
				fSelHist->GetListOfFunctions()->Add(fMarkers);
				if (is2dim) {
					box = new TBox(XlowEdge, YlowEdge, XupEdge, YupEdge);
					box->SetLineColor(-1);
					box->SetFillStyle(0);
					box->Draw();
					fMarkers->Add(new FhMarker(XlowEdge, YlowEdge, 28));
					fMarkers->Add(new FhMarker(XupEdge,  YupEdge, 28));
					PaintMarks();
				} else {
					lowedge = new TLine(XlowEdge, hist->GetYaxis()->GetXmin(),
											  XlowEdge, hist->GetYaxis()->GetXmax());
					upedge  = new TLine(XupEdge, hist->GetYaxis()->GetXmin(),
											  XupEdge, hist->GetYaxis()->GetXmax());
					lowedge->SetLineColor(4); lowedge->Draw();
					upedge->SetLineColor(4); upedge->Draw();
					fMarkers->Add(new FhMarker(XlowEdge, yy, 28));
					fMarkers->Add(new FhMarker(XupEdge,  yy, 28));
					PaintMarks();
				}
			}
			if (fTofLabels)fTofLabels->RaiseWindow();
		}
	}
}
//_____________________________________________________________________________________

void FitHist::DrawTopAxis() {
 //  cout << "  add axis (channels) on top" << endl;
	TGaxis *naxis;
	Axis_t x1, y1, x2, y2;
//      TString side("-");
	Float_t loff;
	Float_t lsize;
	Int_t ndiv;

	x1 = fCanvas->GetUxmin();
	x2 = fCanvas->GetUxmax();
	if ( fCanvas->GetLogx() ) {
		x1 = TMath::Power(10, x1);
		x2 = TMath::Power(10, x2);
	}
	y1 = fCanvas->GetUymax();
	y2 = y1;
	loff = 0.0;
	ndiv = fSelHist->GetXaxis()->GetNdivisions();
	lsize = fSelHist->GetXaxis()->GetLabelSize();
	fCanvas->cd();
	Axis_t blow = (Axis_t)(fSelHist->GetXaxis()->GetFirst() - 1);
	Axis_t bup  = (Axis_t)(fSelHist->GetXaxis()->GetLast());
	naxis = new TGaxis(x1, y1, x2, y2,blow, bup, ndiv, "-W");
	naxis->SetGridLength(-0.8);
	naxis->SetLabelOffset(loff);
	naxis->SetLabelSize(lsize);
	naxis->Paint();
}
//_____________________________________________________________________________________
// Show histograms

void FitHist::DisplayHist(TH1 * hist, Int_t win_topx, Int_t win_topy,
								  Int_t win_widx, Int_t win_widy)
{
	if (hist != NULL) {
		fSelHist = hist;
	} else {
		cout << "Null pointer in DisplayHist" << endl;
		return;
	}
	fOrigHist = hist;

//  construct name of canvas and pad
	fCname = "C_";
	fCname += GetName();
	fEname = "E_";
	fEname += GetName();
	fCtitle = fSelHist->GetName();
	fCtitle += ": ";
	fCtitle += fSelHist->GetTitle();
	if ( fSelHist->GetDimension() == 2 ) {
		if (fDrawOpt2Dim.BeginsWith("GLLEGO") || fDrawOpt2Dim.BeginsWith("GLSURF")) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}
	}
	if ( fSelHist->GetDimension() == 3 ) {
		if ( fDrawOpt3Dim.BeginsWith("GL") ) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}
	}
	if ( fSelHist->GetDimension() == 1 ) {
		TEnv ee(".rootrc");
		if ( ee.GetValue("OpenGL.CanvasPreferGL",0) ) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}
	}
	fCanvas = new HTCanvas(fCname.Data(), fCtitle.Data(),
								win_topx, win_topy, win_widx, win_widy, gHpr, this);
	fCanvasIsAlive = kTRUE;
	fCanvas->SetEditable(kTRUE);
	fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseFitHist()");
	hist->SetDirectory(gROOT);
//   if ( (is2dim(hist) && fLiveStat2Dim) || (!is2dim(hist) && fLiveStat1Dim) ){
	if (gStyle->GetCanvasPreferGL() == kFALSE) {
// 		TString cmd("((FitHist*)gROOT->FindObject(\"");
//       cmd += GetName();
//       cmd += "\"))->handle_mouse()";
		TString cmd("((FitHist*)");
		ostringstream  buf;
		buf << this;
		cmd += buf.str();
		cmd += ")->handle_mouse()";
		if (gHprDebug > 0)
			cout << "FitHist::DisplayHist cmd: " << cmd << endl;
		fCanvas->AddExec("handle_mouse", cmd.Data());
		fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
	}
	gDirectory = gROOT;
	fSelPad = fCanvas;
	fSelPad->cd();
	fCanvas->ToggleEventStatus();
	
	if (is3dim(hist)) {
		Draw3Dim();
	} else if (is2dim(hist)) {
		fCanvas->SetLogx(fLogx);
		fCanvas->SetLogy(fLogy);
		fCanvas->SetLogz(fLogz);
		Draw2Dim();
	} else {
		fCanvas->SetLogx(fLogx);
//		fCanvas->SetLogy(fLogy);
		this->SetLogy(fLogy);
		if (gHprDebug > 1) {
			cout<< "fCanvas->GetLogy(): " ;
			if (fCanvas->GetLogy()) 
				cout << "true";
			else
				cout << "false";
			cout << endl;
		}
		fCanvas->GetHandleMenus()->SetLog(fLogy);
		Draw1Dim();
	}
	if (gHprDebug != 0) {
		cout << hist->ClassName() << " *" <<  hist->GetName()
		<< " = (" << hist->ClassName() << "*)" 
		<< hist << ";" << endl;
		cout << "yaxis " << hist->GetYaxis() << endl;
		cout << "stats " << hist->GetListOfFunctions()->FindObject("stats") << endl;
	}
	fFrameX1 = fCanvas->GetFrame()->GetX1();
	fFrameX2 = fCanvas->GetFrame()->GetX2();
	fFrameY1 = fCanvas->GetFrame()->GetY1();
	fFrameY2 = fCanvas->GetFrame()->GetY2();
	gStyle->SetCanvasPreferGL(kFALSE);
};

//------------------------------------------------------
// Magnify

void FitHist::Magnify()
{
	if (is2dim(fSelHist)) {
		Hpr::WarnBox("Magnify 2 dim not yet supported ");
		return;
	}
	Int_t newx = (Int_t) (2.1 * fSelHist->GetNbinsX());
	Int_t wx = fCanvas->GetWw();
	if (newx <= wx) {
		cout << "Screen resolution already good enough" << endl;
		return;
	} else {
		Float_t fac = (Float_t) newx / (Float_t) wx;
//		TAxis *xaxis = fSelHist->GetXaxis();
		TAxis *yaxis = fSelHist->GetYaxis();

//		xaxis->SetNdivisions(2080);
		yaxis->SetTickLength(0.03 / fac);
		fCanvas->SetLeftMargin(0.1 / fac);
		fCanvas->SetRightMargin(0.1 / fac);
		TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
		if (st) {
		  st->SetX1NDC(1 - 0.4 / fac);
		  st->SetX2NDC(0.99);
		}
//      gStyle->SetStatW(0.19 / fac);
//      gStyle->SetTitleXSize(0.02 / fac);
//      gStyle->SetLabelOffset(0.001 / fac, "y");

	}
	Int_t newy = (Int_t) (0.95 * fCanvas->GetWh());
//   fCanvas->SetWindowSize(wx, fCanvas->GetWh());
//   cout << "newx, newy " << newx << " " << newy<< endl;
	fCanvas->SetCanvasSize(newx, newy);
	fCanvas->Modified(kTRUE);
	fCanvas->Update();
	fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
}
//_______________________________________________________________________________________

void FitHist::Entire()
{
//   fSelHist->GetListOfFunctions()->Print();
//   fOrigHist->GetListOfFunctions()->Print();

	if (fExpHist) {
		if (gHprDebug > 0)
			cout << " Entire() fExpHist->Delete()" <<endl;
		fExpHist->GetListOfFunctions()->Clear("nodelete");
		TF1 *ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("backf");
		if (ff) delete ff;
			  ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("tailf");
		if (ff) delete ff;
			  ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("gausf");
		if (ff) delete ff;
		if (fExpHist == fSelHist )
			fSelHist = NULL;
		fExpHist->Delete();
		fExpHist = NULL;
	}
	fSelHist = fOrigHist;
	fSelHist->SetMinimum(-1111);
	fSelHist->SetMaximum(-1111);
	fSelHist->GetXaxis()->SetRange(1, fSelHist->GetNbinsX());
	fBinlx = 1;
	fBinux =  fSelHist->GetNbinsX();
	if (is2dim(fSelHist)) {
		fBinly = 1;
		fBinuy =  fSelHist->GetNbinsY();
	}
	SaveDefaults(kTRUE);

	ClearMarks();
	fSelPad->cd();
	if (is2dim(fSelHist)) {
		fSelHist->GetYaxis()->SetRange(1, fSelHist->GetNbinsY());
		Draw2Dim();
	} else if (is3dim(fSelHist)){
		Draw3Dim();
	} else {
		Draw1Dim();
		TIter next(fSelHist->GetListOfFunctions());
		while (TObject *o = next()) {
			if (o->InheritsFrom("TF1")) {
			  ((TF1*)o)->Draw("same");
			}
		}
	}
//   fCanvas->Modified(kTRUE);
//   fCanvas->Update();
	fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
};
//_______________________________________________________________________________________

void FitHist::RebinOne()
{
	if (fSelHist == NULL) return;

	TString title(fSelHist->GetTitle());
	title += "_rebin";
	TString name(fSelHist->GetName());
	name += "_rebin";

	static Int_t ngroupX = 2;
	static Int_t ngroupY = 2;
	Bool_t ok;
	TH1 *newhist = NULL;
	TList *row_lab = new TList(); row_lab->SetName("row_lab");
	static void *valp[25];
	Int_t ind = 0;
	row_lab->Add(new TObjString("StringValue_Name of rebinned hist"));
	row_lab->Add(new TObjString("StringValue_Title of rebinned hist"));
	row_lab->Add(new TObjString("PlainIntVal_Rebin value X"));
	if (is2dim(fSelHist))
		row_lab->Add(new TObjString("PlainIntVal_Rebin value Y"));

	valp[ind++] = &name;
	valp[ind++] = &title;
	valp[ind++] = &ngroupX;
	if (is2dim(fSelHist))
		valp[ind++] = &ngroupY;

	Int_t itemwidth = 400;
	ok = GetStringExt("Rebin parameters", NULL, itemwidth, mycanvas,
							 NULL, NULL, row_lab, valp);
	if (!ok) return;
	title += ngroupX;
	name  += ngroupX;
	if (is2dim(fSelHist)) {
//      cout << "nyi" << endl;
		title += "_";
		name  += "_";
		title += ngroupY;
		name  += ngroupY;

		Int_t nbinsXnew = fSelHist->GetNbinsX() / ngroupX;
		Int_t nbinsYnew = fSelHist->GetNbinsY() / ngroupY;
		if (nbinsXnew * ngroupX != fSelHist->GetNbinsX()) {
			nbinsXnew += 1;
			TString warn("Warning: Number of bins X not multiple of ");
			warn += ngroupX;
			Hpr::WarnBox(warn.Data());
		}
		if (nbinsYnew * ngroupY != fSelHist->GetNbinsY()) {
			nbinsYnew += 1;
			TString warn("Warning: Number of bins Y not multiple of ");
			warn += ngroupY;
			Hpr::WarnBox(warn.Data());
		}
		TH2* h2 = (TH2*)fSelHist;
		newhist = h2->Rebin2D(ngroupX, ngroupY, name.Data());
	} else {
		newhist = (TH1*)fSelHist->Clone();
		newhist->GetListOfFunctions()->Clear();
		Int_t first = fSelHist->GetXaxis()->GetFirst();
		Int_t last = fSelHist->GetXaxis()->GetLast();
		newhist->Rebin(ngroupX);
		first /= ngroupX;
		last /= ngroupY;
		newhist->GetXaxis()->SetRange(first, last);
		newhist->SetName(name);
	}
	if (newhist) {
		newhist->SetTitle(title);
		if (gHpr) gHpr->ShowHist(newhist);
		else    newhist->Draw();
	}
}
//_______________________________________________________________________________________

void FitHist::Enlarge()
{
	static const Char_t helptext[] =
"Add extra bins to histogram\n\
";
	if (fSelHist == NULL) return;

	TString title(fSelHist->GetTitle());
	title += "_ext";
	TString name(fSelHist->GetName());
	name += "_ext";

	static Int_t ext = 4;
//	static Int_t exty = 4;
	TList *row_lab = new TList(); row_lab->SetName("row_lab");
	static void *valp[25];
	Int_t ind = 0;
	row_lab->Add(new TObjString("StringValue_Name of enlarged hist"));
	row_lab->Add(new TObjString("StringValue_Title of enlarged hist"));
	row_lab->Add(new TObjString("PlainIntVal_Extra bins"));
//	if (is2dim(fSelHist))
//		row_lab->Add(new TObjString("PlainIntVal_Extra bins Y"));

	valp[ind++] = &name;
	valp[ind++] = &title;
	valp[ind++] = &ext;
//	if (is2dim(fSelHist))
//		valp[ind++] = &ngroupY;

//	ok = GetStringExt("Rebin parameters", NULL, itemwidth, mycanvas,
//							 NULL, NULL, row_lab, valp);
	Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
	Int_t ok = 0;
//	fDialog =
		new TGMrbValuesAndText ("Enlarge hist", NULL, &ok, itemwidth,
								mycanvas, NULL, NULL, row_lab, valp,
								NULL, NULL, helptext);
//	cout << "ok " << ok << endl;
	if (ok < 0) return;
	title += ext;
	name  += ext;
   TAxis *xa = fSelHist->GetXaxis();
	Double_t bwx = xa->GetBinWidth(1);
	if (is2dim(fSelHist)) {
//		cout << "nyi" << endl;
		TAxis *ya = fSelHist->GetYaxis();
		Double_t bwy = ya->GetBinWidth(1);
		TH2F *hext = new TH2F(name, title, 
				fSelHist->GetNbinsX()+2*ext, xa->GetXmin()-ext*bwx, xa->GetXmax()+ext*bwx,
				fSelHist->GetNbinsY()+2*ext, ya->GetXmin()-ext*bwy, ya->GetXmax()+ext*bwy);
		for (Int_t ix=1; ix<=fSelHist->GetNbinsX(); ix++){
			for (Int_t iy=1; iy<=fSelHist->GetNbinsY(); iy++){
				hext->SetBinContent(ix+ext, iy+ext, fSelHist->GetBinContent(ix, iy));
			}
		}
		if (gHpr) gHpr->ShowHist(hext);
		else    hext->Draw();
	
	} else {
		TH1F *hext = new TH1F(name, title, 
				fSelHist->GetNbinsX()+2*ext, xa->GetXmin()-ext*bwx, xa->GetXmax()+ext*bwx);
		for (Int_t ix=1; ix<=fSelHist->GetNbinsX(); ix++){
				hext->SetBinContent(ix+ext,  fSelHist->GetBinContent(ix));
		}
		if (gHpr) gHpr->ShowHist(hext);
		else    hext->Draw();
	}
}
//_______________________________________________________________________________________

void FitHist::RedefineAxis()
{
	if (fExpHist) {
		Hpr::WarnBox("RedefineAxis not implemented \
for expanded Histogram");
		return;
	}
	Int_t n;
	if (is2dim(fSelHist))
		n = 2;
	else
		n = 1;
	TArrayD xyvals(2 * n);
	//   Double_t *xyvals = new Double_t[2*n];
//  TString title("Set new axis limits");
	TOrdCollection *row_lab = new TOrdCollection();
	row_lab->Add(new TObjString("new xlow, xup"));
	xyvals[0] = fSelHist->GetXaxis()->GetXmin();
	if (!is2dim(fSelHist))
		xyvals[1] = fSelHist->GetXaxis()->GetXmax();
	else {
		xyvals[1] = fSelHist->GetYaxis()->GetXmin();
		xyvals[2] = fSelHist->GetXaxis()->GetXmax();
		xyvals[3] = fSelHist->GetYaxis()->GetXmax();
		row_lab->Add(new TObjString("new ylow, yup"));
	}
// show values to caller and let edit
	Int_t ret = 0, ncols = 2, itemwidth = 120, precission = 5;
	TGMrbTableOfDoubles(mycanvas, &ret, "Set new axis limits", itemwidth,
							  ncols, n, xyvals, precission, NULL, row_lab);
//   cout << ret << endl;
	if (ret >= 0) {
		if (!is2dim(fSelHist)) {
			fRangeLowX = xyvals[0];
			fRangeUpX = xyvals[1];
		} else {
			fRangeLowX = xyvals[0];
			fRangeUpX = xyvals[2];
			fRangeLowY = xyvals[1];
			fRangeUpY = xyvals[3];
		}
		TIter next(fCanvas->GetListOfPrimitives());
		TObject *obj;
		while ( obj = next() ) {
			if (obj->InheritsFrom("TH1") ){
				TH1* h = ((TH1*)obj);
				h->GetXaxis()->Set(h->GetNbinsX(), fRangeLowX, fRangeUpX);
				if (h->GetDimension() == 2)
					h->GetYaxis()->Set(h->GetNbinsY(), fRangeLowY, fRangeUpY);
			}
		}
		fSetRange = kTRUE;
		fCanvas->Modified(kTRUE);
		fCanvas->Update();
	}
//   if (xyvals) delete [] xyvals;
	if (row_lab) {
		row_lab->Delete();
		delete row_lab;
	}
};
//_______________________________________________________________________________________

void FitHist::SetRangeAxis()
{
	if (fExpHist) {
		Hpr::WarnBox("SetRangeAxis not implemented \
for expanded Histogram");
		return;
	}
	
	Int_t n = fSelHist->GetDimension();
	if (n > 2) {
				Hpr::WarnBox("SetRangeAxis not implemented \
for 3d Histogram");
		return;
	}
	TArrayD xyvals(4);
	//   Double_t *xyvals = new Double_t[2*n];
//  TString title("Set new axis limits");
	TOrdCollection *row_lab = new TOrdCollection();
	row_lab->Add(new TObjString("xlow, xup"));
	row_lab->Add(new TObjString("ylow, yup"));
	xyvals[0] = fSelHist->GetXaxis()->GetXmin();
	xyvals[2] = fSelHist->GetXaxis()->GetXmax();
	if (n == 1) {
		xyvals[1] = fSelHist->GetMinimum();
		xyvals[3] = fSelHist->GetMaximum();
	} else {
		xyvals[1] = fSelHist->GetYaxis()->GetXmin();
		xyvals[3] = fSelHist->GetYaxis()->GetXmax();
	}		
	fRangeLowX = xyvals[0];
	fRangeUpX  = xyvals[2];
	fRangeLowY = xyvals[1];
	fRangeUpY  = xyvals[3];
	Bool_t modX = kFALSE;
	Bool_t modY = kFALSE;
// show values to caller and let edit
	Int_t ret = 0, ncols = 2, itemwidth = 120, precission = 5;
	TGMrbTableOfDoubles(mycanvas, &ret, "Set new axis limits for display", itemwidth,
							  ncols, 2, xyvals, precission, NULL, row_lab);
//   cout << ret << endl;
	if (ret >= 0) {
		if(fRangeLowX != xyvals[0]) {
			fRangeLowX = xyvals[0];
			modX =kTRUE;
		}
		if(fRangeUpX  != xyvals[2]) {
			fRangeUpX  = xyvals[2];
			modX =kTRUE;
		}
		if(fRangeLowY != xyvals[1]) {
			fRangeLowY = xyvals[1];
			modY =kTRUE;
		}
		if(fRangeUpY  != xyvals[3]) {
			fRangeUpY  = xyvals[3];
			modY =kTRUE;
		}
		TIter next(fCanvas->GetListOfPrimitives());
		TObject *obj;
		while ( obj = next() ) {
			if (obj->InheritsFrom("TH1") ){
				TH1* h = ((TH1*)obj);
				h->GetXaxis()->SetRangeUser(fRangeLowX, fRangeUpX);
				h->GetYaxis()->SetRangeUser(fRangeLowY, fRangeUpY);
			}
		}
		fSetRange = kTRUE;
		fCanvas->Modified(kTRUE);
		fCanvas->Update();
	}
//   if (xyvals) delete [] xyvals;
	if (row_lab) {
		row_lab->Delete();
		delete row_lab;
	}
	if (modX || modY ) {
		TEnv * env = GetDefaults(fHname, kFALSE, kFALSE); // fresh values
		if (!env) return;
		if (modX) {
			env->SetValue("fRangeLowX", fRangeLowX);
			env->SetValue("fRangeUpX",  fRangeUpX);
		}
		if (modY) {
			env->SetValue("fRangeLowY", fRangeLowY);
			env->SetValue("fRangeUpY",  fRangeUpY);
		}
		env->SaveLevel(kEnvLocal);
	}
};
//_______________________________________________________________________________________

void FitHist::RestoreAxis()
{
	if (fExpHist) {
		Hpr::WarnBox("RedefineAxis not implemented \
for expanded Histogram");
		return;
	}

	TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
	while ( obj = next() ) {
		if (obj->InheritsFrom("TH1") ){
			TH1* h = ((TH1*)obj);
			h->GetXaxis()->Set(h->GetNbinsX(), fOrigLowX, fOrigUpX);
			if (h->GetDimension() == 2)
				h->GetYaxis()->Set(h->GetNbinsY(), fOrigLowY, fOrigUpY);
		}
	}
	fSetRange = kFALSE;
	fCanvas->Modified(kTRUE);
	fCanvas->Update();
}
//_______________________________________________________________________________________

void FitHist::AddAxis(Int_t where)
{
//   Double_t *xyvals = new Double_t[2];
	TArrayD xyvals(2);
//   TString title("Define axis limits");
	TOrdCollection *row_lab = new TOrdCollection();
	row_lab->Add(new TObjString("Lower, Upper"));
	if (where == 1) {            // xaxis
		xyvals[0] = fCanvas->GetFrame()->GetX1();
		xyvals[1] = fCanvas->GetFrame()->GetX2();
		if (fCanvas->GetLogx()) {
			xyvals[0] = TMath::Power(10, xyvals[0]);
			xyvals[1] = TMath::Power(10, xyvals[1]);
		}
	} else {
		xyvals[0] = fCanvas->GetFrame()->GetY1();
		xyvals[1] = fCanvas->GetFrame()->GetY2();
		if (fCanvas->GetLogy()) {
			xyvals[0] = TMath::Power(10, xyvals[0]);
			xyvals[1] = TMath::Power(10, xyvals[1]);
		}
	}
// show values to caller and let edit
	Int_t ret = 0, ncols = 2, nrows = 1, itemwidth = 120, precission = 5;
	TGMrbTableOfDoubles(mycanvas, &ret, "Define axis limits", itemwidth,
							  ncols, nrows, xyvals, precission, NULL, row_lab);
	
	if (ret >= 0) {
		Hpr::DoAddAxis(fCanvas, fSelHist, where, xyvals[0],xyvals[1]);
		fHasExtraAxis = kTRUE;
	}
	if (row_lab) {
		row_lab->Delete();
		delete row_lab;
	}
}
//_______________________________________________________________________________________

void FitHist::ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject *obj)
{
	if ( gHprDebug > 0 )
		cout << "ObjMoved: " << obj->ClassName() <<endl;
	if (obj->IsA() == TAxis::Class()) {
	}
}
//_______________________________________________________________________________________

//_______________________________________________________________________________________

void FitHist::HandleLinLogChanged(TObject *obj)
{
	if ( gHprDebug > 0 ) {
		cout << "FitHist::HandleLinLogChanged: " << obj 
				<< " fCanvas: " << fCanvas <<endl;
	}
}
//_______________________________________________________________________________________

void FitHist::PictToPSFile(Int_t plain_flag)
{
	if (fSelHist) {
		TString hname = fSelHist->GetName();
		hname += ".ps";
		Bool_t ok;
		hname =
			 GetString("Write PS-file with name", hname.Data(), &ok,
						  mycanvas);
		if (!ok) {
			cout << " cancelled " << endl;
			return;
		}

		if (plain_flag > 0) {
			fCanvas->SetFillStyle(0);
			fCanvas->SetBorderMode(0);
			fCanvas->GetFrame()->SetFillStyle(0);
			fCanvas->GetFrame()->SetBorderMode(0);

			TPaveText *tpt = (TPaveText *) fCanvas->GetPrimitive("stats");
			if (tpt)
				tpt->SetFillStyle(0);
			tpt = (TPaveText *) fCanvas->GetPrimitive("title");
			if (tpt)
				tpt->SetFillStyle(0);
		}
		gStyle->SetPaperSize(19., 28.);
		fCanvas->SaveAs(hname.Data());
	}
};
//_______________________________________________________________________________________

void FitHist::PictToLP()
{
	if (fSelHist) {
		TString cmd = "lpr -P";
		cmd += gSystem->Getenv("PRINTER");
		TEnv env(".hprrc");      // inspect ROOT's environment
		const char *ccmd =
			 env.GetValue("HistPresent.PrintCommand", cmd.Data());
		cmd = ccmd;
		Bool_t ok;
		cmd = GetString("Printer command: ", cmd.Data(), &ok, mycanvas);
		if (!ok) {
			//      cout << "canceled" << endl;
			return;
		}
		fCanvas->SetFillStyle(0);
		fCanvas->SetBorderMode(0);
		gStyle->SetPaperSize(19., 28.);
		fCanvas->SaveAs("temp_pict.ps");
		TString prcmd = cmd;
		prcmd += " ";
		prcmd += "temp_pict.ps";
		gSystem->Exec(prcmd.Data());
		if (cmd.Contains("lpr ")) {
			cmd(2, 1) = "q";
			gSystem->Exec(cmd.Data());
		}
	}
};
//_______________________________________________________________________________________

void FitHist::WriteFunctions()
{
	if (fSelHist) {
		ClearMarks();
		TList *temp = new TList();
		TIter next(fSelHist->GetListOfFunctions());
		Int_t nf = 0;
		TObject *obj;
		while ( (obj = next() ) ){
			if (obj->InheritsFrom("TF1")) {
				temp->Add(obj);
//				cout << "Adding: " << (TF1*)obj->GetName() << endl;
				nf++;
			}
		}
		cout << nf << " functions found" << endl;
		if (nf > 0) {
			Save2FileDialog sfd((TObject*)temp, NULL, GetMyCanvas());
		}
		delete temp;
//		Save2FileDialog sfd(fSelHist->GetListOfFunctions(), NULL, GetMyCanvas());
	}
};
//_______________________________________________________________________________________

void FitHist::WriteOutCanvas()
{
	if (fSelHist) {
		TString hname = fCanvas->GetName();
		hname += ".canvas";
		Bool_t ok;
		TObject *obj;
		hname =
			 GetString("Save canvas with name", hname.Data(), &ok, mycanvas);
		if (!ok)
			return;
		if ((obj = gROOT->FindObject(hname.Data()))) {
			Hpr::WarnBox("Object with this name already exists");
			cout << setred << "Object with this name already exists: "
				 << obj->ClassName() << setblack << endl;
			return;
		}
//      if (OpenWorkFile()) {
			if (fCanvas->GetAutoExec())
				fCanvas->ToggleAutoExec();
//         fCanvas->SetName(hname.Data());
			TCanvas *nc =
				 new TCanvas(hname.Data(), fCanvas->GetTitle(), 50, 500, 720,
								 500);
//         TCanvas * nc = new TCanvas(hname.Data(), "xxx" ,720, 500);
			TIter next(fCanvas->GetListOfPrimitives());
			while ((obj = next())) {
				if (is_a_hist(obj)) {
					TH1 *hi = (TH1 *) obj;
					if (is2dim(hi)) {
						hi->SetOption(fDrawOpt2Dim);
					} else {
						TString drawopt;
						if (fShowContour)
							drawopt = "hist";
						if ( fErrorMode != "none")
								drawopt += "e1";
						if (fFill1Dim) {
							if (fFillStyle == 0) fFillStyle = 1001;
							hi->SetFillStyle(fFillStyle);
							hi->SetFillColor(fFillColor);
						} else
							hi->SetFillStyle(0);
					}
				}
				gROOT->SetSelectedPad(nc);
				nc->GetListOfPrimitives()->Add(obj->Clone());
			}
			if (nc->GetAutoExec())
				nc->ToggleAutoExec();
//         nc->Write();
//         CloseWorkFile();
			Save2FileDialog sfd(nc, NULL, GetMyCanvas());
			delete nc;
//      }
	}
};
//_______________________________________________________________________________________

void FitHist::WriteOutHist()
{
	if (fSelHist) {
		Save2FileDialog sfd(fSelHist, NULL, GetMyCanvas());
	}
};
//_______________________________________________________________________________________

#include "time.h"

void AddAsString(Double_t x, TAxis * a, Int_t  prec, TOrdCollection *entries)
{
	struct tm * utctis;
	char CHLABEL[256];
	char *LABEL;
	LABEL  = &CHLABEL[0];
	TObjString * os;
	if (a == NULL || !(a->GetTimeDisplay())) {
	  	TMrbString s(x, prec);		      // convert double to string
		os = new TObjString(s.Data());   // wrap TObject around it
	} else {
		time_t t = (time_t)x;
		TString tf(a->GetTimeFormat());
		Int_t itoff = tf.Index("%F");
		if (itoff > 0){
			TString tos = tf;
			tos.Remove(0,itoff + 2);
			tf.Resize(itoff);
		}
		utctis = gmtime(&t);
//      cout << "time " << t <<  " tf.Data() " <<  tf.Data() << endl;
		Int_t nchar = strftime(LABEL, 256, tf.Data(), utctis);
		if (nchar <= 0)os = new TObjString("xxx");
		else           os = new TObjString(LABEL);   // wrap TObject around it
	}
	entries->Add((TObject *) os);	   // add to collection of string
}
//_______________________________________________________________________________________

void FitHist::SetXaxisRange()
{
	SetAxisHistX(fCanvas, fSelHist->GetXaxis());
}
//_______________________________________________________________________________________

void FitHist::SetYaxisRange()
{
	SetAxisHistX(fCanvas, fSelHist->GetYaxis());
}
//_______________________________________________________________________________________

void FitHist::SetZaxisRange()
{
	SetAxisHistX(fCanvas, fSelHist->GetZaxis());
}
//_______________________________________________________________________________________

void FitHist::Set2Marks()
{
	Int_t n = 2;
	static Float_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
//   Double_t *xyvals = new Double_t[n*2];
	TArrayD xyvals(2 * n);
	TOrdCollection *row_lab = new TOrdCollection();

	xyvals[0] = x1;
	xyvals[1] = x2;
	xyvals[2] = y1;
	xyvals[3] = y2;
	row_lab->Add(new TObjString("x,y value"));
	row_lab->Add(new TObjString("x,y value"));
// show values to caller and let edit
	Int_t ret = 0, ncols = 2, nrows = n, itemwidth = 120, precission = 5;
	TGMrbTableOfDoubles(mycanvas, &ret, "Marks", itemwidth,
							  ncols, nrows, xyvals, precission, NULL, row_lab);
	if (ret >= 0) {
		x1 = xyvals[0];
		x2 = xyvals[1];
		y1 = xyvals[2];
		y2 = xyvals[3];
		fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
		if (fMarkers == NULL) {
			fMarkers = new  FhMarkerList();
			fSelHist->GetListOfFunctions()->Add(fMarkers);
		}
		fMarkers->Add(new FhMarker(x1, y1, 28));
		fMarkers->Add(new FhMarker(x2, y2, 28));
	}
//   if (xyvals) delete [] xyvals;
	if (row_lab) {
		row_lab->Delete();
		delete row_lab;
	}
	PaintMarks();
};
//______________________________________________________________________

void FitHist::AddMark(TPad * pad, Int_t px, Int_t py)
{
	Float_t x, y;
	if (pad != (TVirtualPad *) fCanvas) {
		cout << " its not us " << endl;
		return;
	}
//   cout << " FitHist::AddMark " << endl;
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL) {
 //     cout << "new  FhMarkerList()  " << endl;
		fMarkers = new  FhMarkerList();
		fSelHist->GetListOfFunctions()->Add(fMarkers);
	}
	x = pad->AbsPixeltoX(px);
	y = pad->AbsPixeltoY(py);
	if (pad->GetLogx()) x = TMath::Power(10, x);
	if (pad->GetLogy()) y = TMath::Power(10, y);
	FhMarker *m = new FhMarker(x, y, 28);
	m->SetMarkerColor(6);
	m->SetMarkerSize(1);
	fMarkers->Add(m);
	PaintMarks();

}
//_______________________________________________________________________________________

void FitHist::ClearMarks()
{
//
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL ) return;
	fSelHist->GetListOfFunctions()->Remove(fMarkers);
	TIter next(fMarkers);
	FhMarker * ti;
	while ( (ti = (FhMarker *) next()) ) {
		fCanvas->GetListOfPrimitives()->Remove(ti);
	}

	fMarkers->Delete();
//   fMarkers->Clear();
	fCanvas->Modified(kTRUE);
	fCanvas->Update();
};
//_______________________________________________________________________________________

Int_t FitHist::GetMarks(TH1 * hist)
{
	Int_t nval = 0;
	Axis_t xmin, xmax, ymin = 0, ymax = 0;
	if (hist) {
		TAxis *xaxis = hist->GetXaxis();
		Int_t first = xaxis->GetFirst();
		Int_t last = xaxis->GetLast();
//      xmin = xaxis->GetBinLowEdge(first);
//      xmax = xaxis->GetBinUpEdge(last);
		xmin = xaxis->GetBinLowEdge(first) + 0.5 * xaxis->GetBinWidth(first);
		xmax = xaxis->GetBinUpEdge(last)   - 0.5 * xaxis->GetBinWidth(last);
		if (is2dim(hist)) {
			TAxis *yaxis = hist->GetYaxis();
			first = yaxis->GetFirst();
			last = yaxis->GetLast();
//         ymin = yaxis->GetBinLowEdge(first);
 //        ymax = yaxis->GetBinUpEdge(last);
			ymin = yaxis->GetBinLowEdge(first) + 0.5 * yaxis->GetBinWidth(first);
			ymax = yaxis->GetBinUpEdge(last)   - 0.5 * yaxis->GetBinWidth(last);
		}
	} else {
		cout << "Null pointer to hist" << endl;
		return -1;
	}
	fMarkers = (FhMarkerList*)hist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL) {
		fMarkers = new  FhMarkerList();
		hist->GetListOfFunctions()->Add(fMarkers);
	}

	nval = fMarkers->GetEntries();
	if (nval == 1) {
		if (fSelInside)
			fMarkers->Add(new FhMarker(xmin, ymin, 30));
		else
			fMarkers->Add(new FhMarker(xmax, ymax, 30));
		nval = 2;
	}
	if (nval == 0) {
		fMarkers->Add(new FhMarker(xmin, ymin, 30));
		fMarkers->Add(new FhMarker(xmax, ymax, 30));
		nval = 2;
	}
	fMarkers->Sort();
	return nval;
};
//_______________________________________________________________________________________

void FitHist::PrintMarks()
{
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
		cout << "--- Current Markers --------------" << endl;
		FhMarker *ti;
		TIter next(fMarkers);
		while ( (ti = (FhMarker *) next()) ) {
//			cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
			ti->Print();
		}
	} else {
		cout << "--- No marks set ---" << endl;
	}
};
//_______________________________________________________________________________________

void FitHist::DistBetween2Marks()
{
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers != NULL && fMarkers->GetEntries() != 2) {
		cout << "Please set exactly 2 marks" << endl;
		return;
	}
	Double_t x1 = ((FhMarker *)fMarkers->At(0))->GetX();
	Double_t y1 = ((FhMarker *)fMarkers->At(0))->GetY();
	Double_t x2 = ((FhMarker *)fMarkers->At(1))->GetX();
	Double_t y2 = ((FhMarker *)fMarkers->At(1))->GetY();
	cout << "Dist " << TMath::Sqrt((x2-x1)*(x2-x1) +(y2-y1)*(y2-y1))  << endl;
};
//_______________________________________________________________________________________

void FitHist::PaintMarks()
{
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
//      cout << "--- Current fMarkers --------------" << endl;
		FhMarker *ti;
		TIter next(fMarkers);
		fCanvas->cd();
		while ( (ti = (FhMarker *) next()) ) {
			ti->Draw();
		}
		fCanvas->Modified(kTRUE);
		fCanvas->Update();
	}
};

//____________________________________________________________________________________

Bool_t FitHist::IsThereAnyHist()
{
	TList *tl = gDirectory->GetList();
	TIter next(tl);
	while (TObject * obj = next()) {
		if (is_a_hist(obj))
			return kTRUE;
	}
	return kFALSE;
}


//____________________________________________________________________________________

void FitHist::GetRange()
{
	TH1 *hist = Hpr::GetOneHist(fSelHist);
	if (!hist)
		return;

	fSelHist->GetXaxis()->SetRange(hist->GetXaxis()->GetFirst(),
											 hist->GetXaxis()->GetLast());
	if ( fSelHist->GetDimension() > 1 )
		fSelHist->GetYaxis()->SetRange(hist->GetYaxis()->GetFirst(),
												 hist->GetYaxis()->GetLast());
	if ( fSelHist->GetDimension() > 2 )
		fSelHist->GetZaxis()->SetRange(hist->GetZaxis()->GetFirst(),
												 hist->GetZaxis()->GetLast());
	fCanvas->Modified();
	fCanvas->Update();
}

//____________________________________________________________________________________

void FitHist::KolmogorovTest()
{
	TH1 *hist;
	if (gHpr->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
		if (gHpr->GetSelectedHist()->GetSize() > 1) {
			Hpr::WarnBox("More than 1 selection");
			return;
		}
		hist = gHpr->GetSelHistAt(0);
	} else
		hist = Hpr::GetOneHist(fSelHist);      // look in memory
	if (hist) {
		fSelHist->KolmogorovTest(hist, "D");
	}
}
//____________________________________________________________________________________

void FitHist::SuperImpose(Int_t mode)
{
	Hpr::SuperImpose(fCanvas, fSelHist, mode);
}
//____________________________________________________________________________________
//  find limits

void FitHist::GetLimits()
{
	Int_t inp;
	fBinX_1 = fBinX_2 = fBinY_1 = fBinY_2 = 0;
	FhMarker *p;
	fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
	if (fMarkers == NULL) {
		fBinX_1 = fSelHist->GetXaxis()->GetFirst();
		fBinX_2 = fSelHist->GetXaxis()->GetLast();
		if (fSelHist->GetDimension() == 2) {
			fBinY_1 = fSelHist->GetYaxis()->GetFirst();
			fBinY_2 = fSelHist->GetYaxis()->GetLast();
		}
		return;
	}
	TIter next(fMarkers);
//   fMarkers->Sort();
	while ( (p = (FhMarker *) next()) ) {
		inp = fSelHist->GetXaxis()->FindBin(p->GetX());
//      inp = XBinNumber(fSelHist, p->GetX());
		if (inp > 0) {
//         cout << "p->GetX() " << p->GetX()<<  " xbin " << inp << endl;
			if (fBinX_1 <= 0) {
				fBinX_1 = inp;
				fX_1 = p->GetX();
			};
			if (fBinX_1 > 0 && inp < fBinX_1) {
				fBinX_1 = inp;
				fX_1 = p->GetX();
			}
			if (inp > fBinX_2) {
				fBinX_2 = inp;
				fX_2 = p->GetX();
			}
		}
		if (is2dim(fSelHist)) inp = fSelHist->GetYaxis()->FindBin(p->GetY());
		else                  inp = 1;

//      inp = YBinNumber(fSelHist, p->GetY());
//      inp = (Int_t)fYbins->At(i);
		if (inp > 0) {
//         cout << "ybin " << inp << endl;
			if (fBinY_1 <= 0) {
				fBinY_1 = inp;
				fY_1 = p->GetY();
			}
			if (fBinY_1 > 0 && inp < fBinY_1) {
				fBinY_1 = inp;
				fY_1 = p->GetY();
			}
			if (inp > fBinY_2) {
				fBinY_2 = inp;
				fY_2 = p->GetY();
			}
		}
	}
	if (fBinX_1 > fBinX_2) {
		int bin = fBinX_1;
		fBinX_1 = fBinX_2;
		fBinX_2 = bin;
		Axis_t val = fX_1;
		fX_1 = fX_2;
		fX_2 = val;
	}
	if (fBinY_1 > fBinY_2) {
		int bin = fBinY_1;
		fBinY_1 = fBinY_2;
		fBinY_2 = bin;
		Axis_t val = fY_1;
		fY_1 = fY_2;
		fY_2 = val;
	}
}

//____________________________________________________________________________________
//

void FitHist::OutputStat(Int_t fill_hist)
{
//   enum dowhat {expand, projectx, projecty, statonly};

//   ExpandProject(statonly);
	cout <<
		 "============================================================="
		 << endl;

	if (!fSelPad) {
		cout << "Cant find pad, call Debugger" << endl;
		return;
	}
	if (GetMarks(fSelHist) <= 0 && !Nwindows()) {
		cout << "No marks, no windows" << endl;
		return;
	}
	TH1F* h_bincont = NULL;
	Double_t h_min = 1E20;
	Double_t h_max = -1E20;
	
	
//  1-dim case
	if (!is2dim(fSelHist)) {
		Double_t x = 0, cont = 0;
		Double_t sum   = 0;
		Double_t sumx  = 0;
		Double_t sumx2 = 0;
		Double_t mean  = 0;
		Double_t sigma = 0;
		Float_t xlow, xup;
		if (Nwindows() == 0) {
			if (GetMarks(fSelHist) != 2) {
				Hpr::WarnBox("Please select 1 window or exactly 2 marks");
				return;
			}
			GetLimits();
			xlow = fX_1;
			xup = fX_2;
			cout << "Using marks " << endl;
		} else {
			if (Nwindows() > 1) {
				Hpr::WarnBox("Please select one window");
				return;
			}
			TMrbWindow *wdw = (TMrbWindow *) fActiveWindows->First();
			xlow = wdw->GetX1();
			xup = wdw->GetX2();
			cout << "Using window " << wdw->GetName() << endl;
		}
		for (Int_t i = 1; i <= fSelHist->GetNbinsX(); i++) {
			x = fSelHist->GetBinCenter(i);
			if (x >= xlow && x < xup) {
				cont = fSelHist->GetBinContent(i);
				if (cont < h_min) h_min = cont;
				if (cont > h_max) h_max = cont;
				sum += cont;
				sumx += x * cont;
				sumx2 += x * x * cont;
			}
		}
		if ( fill_hist == 1 ) {
			TString pname(fOrigHist->GetName());
			pname += "_BinCont_";
			pname += fSerialPx;
			h_max += 1;
			Int_t nbins = (Int_t)(h_max - h_min);
			h_bincont = new TH1F(pname, fOrigHist->GetTitle(), nbins, h_min, h_max);
			fSerialPx++;
			for (Int_t i = 1; i <= fSelHist->GetNbinsX(); i++) {
				x = fSelHist->GetBinCenter(i);
				if (x >= xlow && x < xup) {
					cont = fSelHist->GetBinContent(i);
					h_bincont->Fill(cont);
				}
			}
		}
		if (sum > 0.) {
			mean = sumx / sum;
			sigma = sqrt(sumx2 / sum - mean * mean);
		}
		cout << "Statistics for Histogram:" << fSelHist->GetName()
			 << " from " << xlow << " to " << xup << endl;
		cout <<
			 "-------------------------------------------------------------"
			 << endl;
		cout << "Content    " << sum << endl;
		cout << "Mean       " << mean << endl;
		cout << "Sigma      " << sigma << endl;
		cout << "2.35*Sigma " << 2.35 * sigma << endl;	// 2*sqrt(2ln2)
		cout <<
			 "-------------------------------------------------------------"
			 << endl;
	} else {
		GetLimits();
		double sum = 0;
		UpdateCut();
//      CheckList(fActiveCuts);
		int nc = Ncuts();
		TAxis *xaxis = fSelHist->GetXaxis();
		TAxis *yaxis = fSelHist->GetYaxis();
		for (int i = 1; i <= fSelHist->GetNbinsX(); i++) {
			Axis_t xcent = xaxis->GetBinCenter(i);
			for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
				Axis_t ycent = yaxis->GetBinCenter(j);
				if (nc <= 0 ||(nc > 0 && InsideCut((float) xcent, (float) ycent))) {
					Double_t cont = fSelHist->GetBinContent(i, j);
					sum += cont;
					if (cont < h_min) h_min = cont;
					if (cont > h_max) h_max = cont;
				}
			}
		}
		if ( fill_hist == 1 ) {
			TString pname(fOrigHist->GetName());
			pname += "_BinCont_";
			pname += fSerialPx;
			h_max += 1;
			Int_t nbins = (Int_t)(h_max - h_min);
			h_bincont = new TH1F(pname, fOrigHist->GetTitle(), nbins, h_min, h_max);
			fSerialPx++;
			for (int i = 1; i <= fSelHist->GetNbinsX(); i++) {
				Axis_t xcent = xaxis->GetBinCenter(i);
				for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
					Axis_t ycent = yaxis->GetBinCenter(j);
					if (nc <= 0 ||(nc > 0 && InsideCut((float) xcent, (float) ycent))) {
						sum += fSelHist->GetBinContent(i, j);
						if ( fill_hist == 1 ) {
							h_bincont->Fill(fSelHist->GetBinContent(i, j));
						}
					}
				}
			}
		}
		
		cout << "Statistics for 2 dim Histogram:" << fSelHist->
			 GetName() << endl;

//      cout << " X from " << fX_1 << " to " << fX_2 << endl;
//      cout << " Y from " << fY_1 << " to " << fY_2 << endl;
		cout <<
			 "-------------------------------------------------------------"
			 << endl;
		if (nc) {
			cout << "Cuts applied: " << endl;
			TIter next(fActiveCuts);
			while (TMrbWindow2D * cut = (TMrbWindow2D *) next())
				PrintOneCut(cut);
			cout <<
				 "-------------------------------------------------------------"
				 << endl;
		} else {
			cout << "No cuts applied!!!" << endl;
		}
		cout << "Content ";
		if (fSelInside) cout << "inside: ";
		else            cout << "outside: ";
		cout << sum << endl;
		cout <<
			 "-------------------------------------------------------------"
			 << endl;
	}
	if ( h_bincont != NULL ) {
		gHpr->ShowHist(h_bincont);
	}
}

//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::Expand()
{
//   enum dowhat {expand, projectx, projecty, statonly};
	ExpandProject(expand);
	SaveDefaults(kFALSE);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectBoth()
{
//   enum dowhat {expand, projectx, projecty, statonly,projectboth};
	TString opt = fSelHist->GetOption();
	if ( opt.Contains("LEGO", TString::kIgnoreCase) ||  
		  opt.Contains("SURF", TString::kIgnoreCase)) {
		cout << "Cant do that with LEGO or SURF" << endl;
		return;
	}
	ExpandProject(projectboth);
	fProjectedBoth = 1;
}

//____________________________________________________________________________________

// Show X projection of histograms

void FitHist::ProjectX()
{
	ExpandProject(projectx);
}
//____________________________________________________________________________________

// Show X projection of histograms within a window in y along function

void FitHist::ProjectX_Func()
{
	ExpandProject(projectx_func);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectY()
{
	ExpandProject(projecty);
}

//____________________________________________________________________________________

// Show projection of histograms along a function

void FitHist::ProjectF()
{
	ExpandProject(projectf);
}

//____________________________________________________________________________________

// Fast Fourier Transform

void FitHist::FastFT()
{
#if ROOTVERSION < 51100
	cout << "FFT not yet available" << endl;
#else
	const char helpText[] =
" This function allows to do discrete Fourier transforms of TH1 and TH2.\n\
 Available transform types and flags are described below.\n\
\n\
 To extract more information about the transform, use the function\n\
  TVirtualFFT::GetCurrentTransform() to get a pointer to the current\n\
  transform object.\n\
\n\
 Parameters:\n\
  1st - histogram for the output. If a null pointer is passed, a new histogram is created\n\
  and returned, otherwise, the provided histogram is used and should be big enough\n\
\n\
  Options: option parameters consists of 3 parts:\n\
	 - option on what to return\n\
	\"RE\" - returns a histogram of the real part of the output\n\
	\"IM\" - returns a histogram of the imaginary part of the output\n\
	\"MAG\"- returns a histogram of the magnitude of the output\n\
	\"PH\" - returns a histogram of the phase of the output\n\
\n\
	 - option of transform type\n\
	\"R2C\"  - real to complex transforms - default\n\
	\"R2HC\" - real to halfcomplex (special format of storing output data,\n\
			 results the same as for R2C)\n\
	\"DHT\" - discrete Hartley transform\n\
			real to real transforms (sine and cosine):\n\
	\"R2R_0\", \"R2R_1\", \"R2R_2\", \"R2R_3\" - discrete cosine transforms of types I-IV\n\
	\"R2R_4\", \"R2R_5\", \"R2R_6\", \"R2R_7\" - discrete sine transforms of types I-IV\n\
	 To specify the type of each dimension of a 2-dimensional real to real\n\
	 transform, use options of form \"R2R_XX\", for example, \"R2R_02\" for a transform,\n\
	 which is of type \"R2R_0\" in 1st dimension and  \"R2R_2\" in the 2nd.\n\
\n\
	 - option of transform flag\n\
	 \"ES\" (from \"estimate\") - no time in preparing the transform, but probably sub-optimal\n\
		 performance\n\
	 \"M\" (from \"measure\")   - some time spend in finding the optimal way to do the transform\n\
	 \"P\" (from \"patient\")   - more time spend in finding the optimal way to do the transform\n\
	 \"EX\" (from \"exhaustive\") - the most optimal way is found\n\
	  This option should be chosen depending on how many transforms of the same size and\n\
	  type are going to be done. Planning is only done once, for the first transform of this\n\
	  size and type. Default is \"ES\".\n\
";

	static Int_t mag = 1, re = 0 , img = 0 , ph = 0,
					 r2c = 1, dht = 0;
	static TString opt_r2r("R2R_0");

	static void *valp[50];
	Int_t ind = 0;
	Bool_t ok = kTRUE;
	TList *row_lab = new TList(); row_lab->SetName("row_lab");
	row_lab->Add(new TObjString("CheckButton_              Magnitude"));
	row_lab->Add(new TObjString("CheckButton_              Real Part"));
	row_lab->Add(new TObjString("CheckButton_         Imaginary Part"));
	row_lab->Add(new TObjString("CheckButton_                  Phase"));
	row_lab->Add(new TObjString("RadioButton_TType:  Real to Complex"));
	row_lab->Add(new TObjString("RadioButton_TType: Discrete Hartley"));
//   row_lab->Add(new TObjString("RadioButton_TType: Real to Real"));
//   row_lab->Add(new TObjString("StringValue_Option for Real to Real"));

	valp[ind++] = &mag;
	valp[ind++] = &re;
	valp[ind++] = &img;
	valp[ind++] = &ph;
	valp[ind++] = &r2c;
	valp[ind++] = &dht;
//   valp[ind++] = &r2r;
//   valp[ind++] = &opt_r2r;

	Int_t itemwidth = 320;
	ok = GetStringExt("FFT Parameters", NULL, itemwidth, mycanvas,
						 NULL, NULL, row_lab, valp,
						 NULL, NULL, &helpText[0]);
	if ( !ok )
		return;
	TString option;
	TString name;
	TString title;

	if (r2c) option = "R2C";
	if (dht) option = "DHT";
//   if (r2r) option = opt_r2r;
	if (mag != 0) {
		TH1 * hresult = NULL;
		name = fSelHist->GetName();
		name = name + "_" + option + "_" + "MAG";
		title = fSelHist->GetTitle();
		title += "-Magnitude of FFT, ";
		title += option;
		option += " MAG";
		cout << "Option: " << option << endl;
		hresult = fSelHist->FFT(hresult, option);
		hresult->SetName(name);
		hresult->SetTitle(title);
		hresult->SetStats(kFALSE);
		if  (gHpr) gHpr->ShowHist(hresult);
	}
	if (re != 0) {
		TH1 * hresult = NULL;
		name = fSelHist->GetName();
		name = name + "_" + option + "_" + "RE";
		title = fSelHist->GetTitle();
		title += "-Real part of FFT, ";
		title += option;
		option += " RE";
		hresult = fSelHist->FFT(hresult, option);
		hresult->SetName(name);
		hresult->SetTitle(title);
		hresult->SetStats(kFALSE);
		if  (gHpr) gHpr->ShowHist(hresult);
	}
	if (img != 0) {
		TH1 * hresult = NULL;
		name = fSelHist->GetName();
		name = name + "_" + option + "_" + "IM";
		title = fSelHist->GetTitle();
		title += "-Imaginary part of FFT, ";
		title += option;
		option += " IM";
		hresult = fSelHist->FFT(hresult, option);
		hresult->SetName(name);
		hresult->SetTitle(title);
		hresult->SetStats(kFALSE);
		if  (gHpr) gHpr->ShowHist(hresult);
	}
	if (ph != 0) {
		TH1 * hresult = NULL;
		name = fSelHist->GetName();
		name = name + "_" + option + "_" + "PH";
		title = fSelHist->GetTitle();
		title += "-Phase of FFT, ";
		title += option;
		option += " PH";
		hresult = fSelHist->FFT(hresult, option);
		hresult->SetName(name);
		hresult->SetTitle(title);
		hresult->SetStats(kFALSE);
		if  (gHpr) gHpr->ShowHist(hresult);
	}
#endif
}

//____________________________________________________________________________________

// Rotate 2dim histograms

void FitHist::Rotate(Int_t sense)
{
	if (!is2dim(fSelHist)) {
		Hpr::WarnBox("Only 2dim hist can be rotated");
		return;
	}
	Double_t tan_alpha = 0;
	Double_t alpha_deg = 0;
	TList * fl = fSelHist->GetListOfFunctions();
	if (fl->GetSize() > 0) {
		TIter next(fl);
		while (TObject * obj = next()){
			if(obj->IsA() == TF1::Class()) {
				TF1 * func = (TF1*)obj;
				if (func->GetNpar() == 2) {
					if (tan_alpha != 0)
					 cout << setred << "More than 1 Pol1 defined, take last"
							<< setblack << endl;
				  tan_alpha = func->GetParameter(1);
				}
			}
		}
	}
	if (tan_alpha == 0) {
		Bool_t ok;
		alpha_deg = (Double_t)GetFloat("Enter angle in degrees or 0 to define Pol1",
			0, &ok, mycanvas);
		if (!ok ||  alpha_deg == 0) {
			Hpr::WarnBox("Please fit a Pol1 to define rotation angle");
			return;
		}
		alpha_deg = TMath::Abs(alpha_deg);
		if (sense == 1) alpha_deg = - alpha_deg;
	} else {
		if (sense == 1) tan_alpha = - 1 / tan_alpha;
		alpha_deg = TMath::ATan(tan_alpha) * 180. / TMath::Pi();
	}
	TH2 * hrot = rotate_hist((TH2*)fSelHist, alpha_deg, fSerialRot);
	fSerialRot++;
	fCanvas->cd();
	if  (gHpr) gHpr->ShowHist(hrot);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Transpose 2dim histograms

void FitHist::Transpose(Int_t invert)
{
	// exchange x an y axis, if invert = 1, invert result x axis
	if (!is2dim(fSelHist)) {
		Hpr::WarnBox("Only 2dim hist can be transposed");
		return;
	}
	TH2F *h2 = (TH2F *)fOrigHist;
	TString hname(h2->GetName());
	TString htitle(h2->GetTitle());
	if (invert == 1) {
		hname += "_rot90";
		htitle += "_rot90";
	} else {
		hname += "_transp";
		htitle += "_transp";
	}
	TAxis *xa = h2->GetXaxis();
	TAxis *ya = h2->GetYaxis();
	Int_t nbinsX = xa->GetNbins();
	TH2F *h2_transp = new TH2F(hname, htitle,
										ya->GetNbins(), ya->GetXmin(), ya->GetXmax(),
										xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
	for (Int_t ix = 1; ix <= nbinsX; ix++) {
		for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
			if (invert == 1 ) {
				h2_transp->SetBinContent(iy, nbinsX - ix +1, h2->GetBinContent(ix, iy));
				h2_transp->SetBinError  (iy, nbinsX - ix +1, h2->GetBinError  (ix, iy));
			} else {
				h2_transp->SetBinContent(iy, ix, h2->GetBinContent(ix, iy));
				h2_transp->SetBinError  (iy, ix, h2->GetBinError  (ix, iy));
			}
		}
	}
	h2_transp->SetEntries(h2->GetEntries());
	h2_transp->GetXaxis()->SetTitle(h2->GetYaxis()->GetTitle());
	h2_transp->GetYaxis()->SetTitle(h2->GetXaxis()->GetTitle());
//   fSelHist = h2_transp;
	fCanvas->cd();
	if  (gHpr) gHpr->ShowHist(h2_transp);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileX()
{
	if (!is2dim(fSelHist)) {
		Hpr::WarnBox("Only 2dim hist can be profiled");
		return;
	}
	TH2F *h2 = (TH2F *)fOrigHist;
	TString hname(h2->GetName());
	hname += "_profX";
	hname += fSerialPx;
	fSerialPx++;
	TString htitle(h2->GetTitle());
	htitle += "_profX";
	TAxis *xa = h2->GetXaxis();
	TAxis *ya = h2->GetYaxis();
	TMrbArrayD yslice(ya->GetNbins());
	yslice.SetBinWidth(ya->GetBinWidth(1));
	yslice.SetLowEdge(ya->GetXmin());
//   Int_t non_zero = 0;
	TH1D *h_prof = new TH1D(hname, htitle,
										xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
	UpdateCut();
	Int_t nc = Ncuts();
	Double_t max_error = 0;
	for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
		yslice.Reset();
		for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
			if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
										ya->GetBinCenter(iy)))
				continue;
			yslice[iy-1] = h2->GetBinContent(ix, iy);
		}
//      cout << yslice.GetMean() << endl;
		if (yslice.GetSum() != 0) {
			h_prof->SetBinContent(ix, yslice.GetMean());
			Double_t error = yslice.GetRMS() / TMath::Sqrt(yslice.GetSum());
			h_prof->SetBinError(ix, error);
			max_error = TMath::Max(error, max_error);
		}
	}
 //     cout << "max_error " << max_error << endl;
	for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
		if (h_prof->GetBinContent(ix) != 0 &&
			 h_prof->GetBinError(ix) == 0)
				h_prof->SetBinError(ix, max_error);
	}
	h_prof->SetEntries(h2->GetEntries());
	fCanvas->cd();
	if  (gHpr) gHpr->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileY()
{
	if (!is2dim(fSelHist)) {
		Hpr::WarnBox("Only 2dim hist can be profiled");
		return;
	}
	TH2F *h2 = (TH2F *)fOrigHist;
	TString hname(h2->GetName());
	hname += "_profY";
	hname += fSerialPx;
	fSerialPx++;
	TString htitle(h2->GetTitle());
	htitle += "_profY";
	TAxis *xa = h2->GetXaxis();
	TAxis *ya = h2->GetYaxis();
	TMrbArrayD xslice(xa->GetNbins());
	xslice.SetBinWidth(xa->GetBinWidth(1));
	xslice.SetLowEdge(xa->GetXmin());
//   Int_t non_zero = 0;
	TH1D *h_prof = new TH1D(hname, htitle,
										xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
	UpdateCut();
	Int_t nc = Ncuts();
	Double_t max_error = 0;
	for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
		xslice.Reset();
		for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
			if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
										ya->GetBinCenter(iy)))
				continue;
			xslice[ix-1] = h2->GetBinContent(ix, iy);
		}
//      cout << xslice.GetMean() << endl;
		if (xslice.GetSum() != 0) {
			h_prof->SetBinContent(iy, xslice.GetMean());
			Double_t error = xslice.GetRMS() / TMath::Sqrt(xslice.GetSum());
			h_prof->SetBinError(iy, error);
			max_error = TMath::Max(error, max_error);
		}
	}
//      cout << "max_error " << max_error << endl;
	for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
		if (h_prof->GetBinContent(ix) != 0 &&
			 h_prof->GetBinError(ix) == 0)
				h_prof->SetBinError(ix, max_error);
	}
	h_prof->SetEntries(h2->GetEntries());
	fCanvas->cd();
	if  (gHpr) gHpr->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::ExpandProject(Int_t what)
{
//   enum dowhat {expand, projectx, projecty, statonly, profilex, profiley};
	if (gHprDebug > 0)
		cout << "enter ExpandProject(Int_t what)" << what << endl;
	if (!fSelPad) {
		cout << "Cant find pad, call Debugger" << endl;
	}

	if (GetMarks(fSelHist) <= 0) {
		cout << "No selection" << endl;
	}
	TString expname;
	if (what == projectf) {
		ClearMarks();
	}
	GetLimits();
//  1-dim case
	if (fSelHist->GetDimension() == 1 ) {
		Double_t x = 0, cont = 0;
		Double_t sum = 0;
		Double_t sumx = 0;
		Double_t sumx2 = 0;
//      Double_t mean;
//     Double_t sigma;
//     get values in expanded hist
		TAxis *xaxis_new = fSelHist->GetXaxis();
		Axis_t bw = xaxis_new->GetBinWidth(fBinX_1);
		fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
		fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + bw;

		fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * bw);
		fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * bw);
		Int_t NbinX = fBinux - fBinlx + 1;

		if (what == expand) {
			if (fExpHist)
				fExpHist->GetListOfFunctions()->Clear();
			expname = fEname;
			expname += fExpInd;
			fExpInd++;
			fExpHist = new TH1D(expname, fOrigHist->GetTitle(),
									 NbinX, fExplx, fExpux);
			fExpHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
			fExpHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
		}
		for (Int_t i = 0; i <= fOrigHist->GetNbinsX() + 1; i++) {
			cont = fOrigHist->GetBinContent(i);
			x = fOrigHist->GetBinCenter(i);
			if (i >= fBinlx && i <= fBinux) {
				sum += cont;
				sumx += x * cont;
				sumx2 += x * x * cont;
			}
			if (what == expand) {
				Int_t newbin = i - fBinlx + 1;
				if (fOrigHist->GetSumw2N()) {
					fExpHist->Fill(x, cont);
				} else {
					fExpHist->SetBinContent(newbin, cont);
				}
				if (i >= fBinlx && i <= fBinux && fOrigHist->GetSumw2N())
					fExpHist->SetBinError(newbin, fOrigHist->GetBinError(i));
			}
		}
//      if (sum > 0.) {
//         mean = sumx / sum;
//         sigma = sqrt(sumx2 / sum - mean * mean);
//      }
		if (fExpHist)
			fExpHist->SetEntries(Int_t(sum));

		if (what == statonly) {
			return;
		}
	} else {

//  2-dim case
		Double_t sum = 0;
//      Double_t x = 0;
		TF1 * func = NULL;
//		Double_t apol = 0;
//		Double_t cpol = 0;
		if (what == projectf || what == projectx_func) {
			TIter next(fSelHist->GetListOfFunctions());
			while (TObject * obj =(TObject *)next()) {
				if (is_a_function(obj)) {
					if (func != NULL)
						Hpr::WarnBox("More than 1 function defined, take first");
					 else func =(TF1 *)obj;
				}
			}
			if (!func) {
				Hpr::WarnBox("No function found");
				return;
			}
			TString tit = func->GetTitle();
//			if (tit != "pol1") {
//				Hpr::WarnBox("Function must be a pol1");
//				return;
//			}
//			apol = func->GetParameter(1);
//			cpol = func->GetParameter(0);
	
		}
		TAxis *xaxis = fOrigHist->GetXaxis();
		TAxis *yaxis = fOrigHist->GetYaxis();
//     get values in expanded hist
		TAxis *xaxis_new = fSelHist->GetXaxis();
		fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
		Axis_t xbw = xaxis_new->GetBinWidth(fBinX_1);
		fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + xbw;

		fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * xbw);
		fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * xbw);
		Int_t NbinX = fBinux - fBinlx + 1;

		TAxis *yaxis_new = fSelHist->GetYaxis();
		fExply = yaxis_new->GetBinLowEdge(fBinY_1);
		Axis_t ybw = yaxis_new->GetBinWidth(fBinY_1);
		fExpuy = yaxis_new->GetBinLowEdge(fBinY_2) + ybw;
		fBinly = YBinNumber(fOrigHist, fExply + 0.5 * ybw);
		fBinuy = YBinNumber(fOrigHist, fExpuy - 0.5 * ybw);
		Int_t NbinY = fBinuy - fBinly + 1;
		if (gHprDebug > 0) {
			cout << "fExplx, ux, ly, uy "
			 <<fExplx << " "  << fExpux<< " "  << fExply<< " "  << fExpuy<< endl;
			cout << "NBinX, NbinY fBinlx, ux, ly, uy " << NbinX << " " << NbinY  << " "
			 <<fBinlx << " "  << fBinux<< " "  << fBinly<< " "  << fBinuy<< endl;
		 }
		Int_t nperbinX = 1, nperbinY = 1;
		TString pname(fOrigHist->GetName());
		if (what == projectx ||what == projectx_func || what == projectboth) {
			pname += "_ProjX";
			if ( what == projectx_func ) {
				pname += "_func";
			}
			pname += fSerialPx;

			fProjHistX = new TH1F(pname, fOrigHist->GetTitle(),
										NbinX, fExplx, fExpux);
			fSerialPx++;
		}
		
		if (what == projecty || what == projectf || what == projectboth) {
			Axis_t low = fExply;
			Axis_t up  = fExpuy;
			if (what == projectf) {
				up = 1.5 * 0.5 * TMath::Max((fExpuy - fExply), (fExpux - fExplx));
				low = -up;
				pname += "_Pfunc";
				pname += fSerialPf;
				cout << "projectf " <<up << " " << low<< endl;
				fSerialPf++;
			} else {
				pname += "_ProjY";
				pname += fSerialPf;
				fSerialPy++;
			}
			fProjHistY = new TH1F(pname, fOrigHist->GetTitle(),
										NbinY, low, up);
		}
		if (what == expand) {
			Int_t MaxBin2dim = 1000;
			nperbinX = (NbinX - 1) / MaxBin2dim + 1;
			Int_t NbinXrebin = (NbinX - 1) / nperbinX + 1;
			nperbinY = (NbinY - 1) / MaxBin2dim + 1;
			Int_t NbinYrebin = (NbinY - 1) / nperbinY + 1;
			if (fExpHist)
				fExpHist->GetListOfFunctions()->Clear();
			expname = fEname;
			expname += fExpInd;
			fExpInd++;
			fExpHist = new TH2F(expname, fOrigHist->GetTitle(),
									 NbinXrebin, fExplx, fExpux, NbinYrebin, fExply,
									 fExpuy);
			fExpHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
			fExpHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
			fSelHist = fExpHist;
			if (gHprDebug > 0)
				cout << "nperbinX " << nperbinX << " nperbinY " << nperbinY << endl;
		}
		TAxis *xa = fSelHist->GetXaxis();
		TAxis *ya = fSelHist->GetYaxis();
//     is there a cut active
		Axis_t xcent;
		Axis_t ycent;
		UpdateCut();
		int nc = Ncuts();
		if ( what == projecty ) {
			for (Int_t j = fBinly; j <= fBinuy + 1; j++) {
				ycent = yaxis->GetBinCenter(j);
				Double_t errsum2 = 0.;
				Double_t cont = 0.;
				for (Int_t i = fBinlx; i <= fBinux; i++) {
					xcent = xaxis->GetBinCenter(i);
					if (nc && !InsideCut(xcent, ycent))
						continue;
					cont += fOrigHist->GetBinContent(i, j);
					Stat_t exy = fOrigHist->GetBinError(i, j);
					errsum2 += exy;
				}
				sum += cont;
				fProjHistY->SetBinContent(j-fBinly+1, cont);
				fProjHistY->SetBinError(j-fBinly+1, TMath::Sqrt(errsum2));
			}
		} else {
			for (Int_t i = fBinlx; i <= fBinux + 1; i++) {
//			for (Int_t i = 1; i <= fOrigHist->GetNbinsX(); i++) {
				Double_t yf = 0;
				xcent = xaxis->GetBinCenter(i);
				if (func && what == projectx_func ) {
					// calculate function value +- road width/2
					yf = func->Eval(xcent);
					fBinly = ya->FindBin(yf - fRoad2);
					fBinuy = ya->FindBin(yf + fRoad2);
				}
				Double_t errsum2 = 0.;
				Double_t cont = 0.;
				for (Int_t j = fBinly; j <= fBinuy + 1; j++) {
//				for (Int_t j = 1; j <= fOrigHist->GetNbinsY(); j++) {
					ycent = yaxis->GetBinCenter(j);
					if (nc && !InsideCut((float) xcent, (float) ycent))
						continue;
					cont += fOrigHist->GetBinContent(i, j);
					Stat_t exy = fOrigHist->GetBinError(i, j);
					errsum2 += exy*exy;
					if (what == projectboth || what == projectx_func) {
						if (j >= fBinly && j <= fBinuy)
							fProjHistX->Fill(xcent, fOrigHist->GetBinContent(i, j));
					}
					if ( what == projectboth) {
						if (i >= fBinlx && i <= fBinux)
							fProjHistY->Fill(ycent, fOrigHist->GetBinContent(i, j));
					}
					if (what == projectf) {
						if (i >= fBinlx && i <= fBinux) {
							Double_t dist = ycent - func->Eval(xcent);
	//						Double_t dist = (apol*xcent -ycent +cpol)/TMath::Sqrt(apol*apol +1);
							fProjHistY->Fill(dist, fOrigHist->GetBinContent(i, j));
						}
					}
					if (what == expand) {
						Int_t irebin = Int_t((i - fBinlx) / nperbinX + 1);
						if (irebin < 0)
							irebin = 0;
						if (irebin > fSelHist->GetNbinsX() + 1)
							irebin = fSelHist->GetNbinsX() + 1;
						Int_t jrebin = Int_t((j - fBinly) / nperbinY + 1);
						if (jrebin < 0)
							jrebin = 0;
						if (jrebin > fSelHist->GetNbinsY() + 1)
							jrebin = fSelHist->GetNbinsY() + 1;
						Stat_t oldcont = fSelHist->GetBinContent(irebin, jrebin);
	//                 cout << i << " " << j << " "<< irebin << " "<< jrebin << endl;
	//                 cout << xcent << " " << ycent << " " << oldcont << " " << cont<< endl;
						oldcont +=  fOrigHist->GetBinContent(i, j);
						xcent = xa->GetBinCenter(irebin);
						ycent = ya->GetBinCenter(jrebin);

						fSelHist->SetBinContent(irebin, jrebin, 0);
						TH2 *ph2 = (TH2 *) fSelHist;
						ph2->Fill(xcent, ycent, oldcont);
					}
					sum += cont;
					if (what == projectx) {
						fProjHistX->SetBinContent(i-fBinlx+1, cont);
						fProjHistX->SetBinError(i-fBinlx+1, TMath::Sqrt(errsum2));
					}
				}
			}
		}
		if (what == statonly) {
			ClearMarks();
			return;
		}
//		ClearMarks();
		fSelPad->cd();
		if (what == projectboth) {
			TList temp;
			TList * lof = fSelPad->GetListOfPrimitives();
			TIter next( lof );
			TObject * o;
			while ( o = next() ) {
				if ( o->IsA() == TPolyLine::Class() ||
					o->IsA() == TGaxis::Class() ) {
					temp.Add(o);
				}
			}
			TIter next1(&temp);
			while ( o = next1() ) {
				lof->Remove(o);
//				delete o;
			}
			Double_t bothratio = WindowSizeDialog::fProjectBothRatio;

			if (bothratio > 1 || bothratio <= 0)
				bothratio = 0.8;
			fCanvas->SetTopMargin(1 - bothratio);
			fCanvas->SetRightMargin(1 - bothratio);
			Double_t xmin = xa->GetBinLowEdge(xa->GetFirst());
			Double_t xmax = xa->GetBinUpEdge(xa->GetLast());
			Double_t dx = (xmax - xmin) / (Double_t) fProjHistX->GetNbinsX();
			Double_t ymin = ya->GetBinLowEdge(ya->GetFirst());
			Double_t ymax = ya->GetBinUpEdge(ya->GetLast());
			Double_t y0 = ymax + 0.005 * (ymax - ymin);
			Double_t y1 = ymin + 0.9 * (ymax - ymin) / bothratio;
//         cout << "ymax " << ymax << " ymin " << ymin<< " y1 " << y1<< endl;
			Double_t maxcont = -10000000;
			Int_t nbins = fProjHistX->GetNbinsX();
			for (Int_t i = 1; i <= nbins; i++) {
				if (fProjHistX->GetBinContent(i) > maxcont)
					maxcont = fProjHistX->GetBinContent(i);
			}
			if (maxcont == 0)
				return;
			Double_t yfac = (y1 - y0) / maxcont;
			Int_t np = 0;
			Double_t x = xmin;
			Double_t y = y0;
			TPolyLine *lpx = new TPolyLine(nbins * 2 + 3);
			for (Int_t i = 1; i <= nbins; i++) {
				lpx->SetPoint(np, x, y);
				np++;
				y = y0 + yfac * fProjHistX->GetBinContent(i);
				lpx->SetPoint(np, x, y);
				np++;
				x += dx;
			}
			lpx->SetPoint(np, x, y);
			np++;
			lpx->SetPoint(np, x, y0);
			np++;
			lpx->SetPoint(np, xmin, y0);
 //        cout << "np " << np << endl;
			TString sopt;
			if (fFill1Dim) sopt= "F";
			lpx->Draw(sopt);
			lpx->SetFillStyle(fFillStyle);
			lpx->SetFillColor(fFillColor);
			lpx->Draw();
			TGaxis *naxis =
				 new TGaxis(xmin + dx * nbins, y0, xmin + dx * nbins, y1,
								0, maxcont, 404, "-");
			naxis->SetLabelOffset(0.01);
			naxis->SetLabelSize(0.02);
			naxis->Draw();

//         xmin = xa->GetXmin();
//         xmax = xa->GetXmax();
//         ymin = ya->GetXmin();
//         ymax = ya->GetXmax();
			Double_t dy = (ymax - ymin) / (Double_t) fProjHistY->GetNbinsX();
			Double_t x0 = xmax + 0.005 * (xmax - xmin);
			Double_t x1 = xmin + 0.9 * (xmax - xmin) / bothratio;
			maxcont = -10000000;
			for (Int_t i = 1; i <= fProjHistY->GetNbinsX(); i++) {
				if (fProjHistY->GetBinContent(i) > maxcont)
					maxcont = fProjHistY->GetBinContent(i);
			}
			if (maxcont == 0)
				return;
			Double_t xfac = (x1 - x0) / maxcont;
			np = 0;
			x = x0;
			y = ymin;
			TPolyLine *lpy = new TPolyLine(fProjHistY->GetNbinsX() * 2 + 3);
			nbins = fProjHistY->GetNbinsX();
			for (Int_t i = 1; i <= nbins; i++) {
				lpy->SetPoint(np, x, y);
				np++;
				x = x0 + xfac * fProjHistY->GetBinContent(i);
				lpy->SetPoint(np, x, y);
				np++;
				y += dy;
			}
			lpy->SetPoint(np, x, y);
			np++;
			lpy->SetPoint(np, x0, y);
			np++;
			lpy->SetPoint(np, x0, ymin);
 //        cout << "np " << np << endl;
			lpy->Draw(sopt);
			lpy->SetFillStyle(fFillStyle);
			lpy->SetFillColor(fFillColor);
			lpy->Draw();
			naxis = new TGaxis(x0, ymin + nbins * dy, x1, ymin + nbins * dy,
									 0, maxcont, 404, "-");
			naxis->SetLabelOffset(0.01);
			naxis->SetLabelSize(0.02);
			naxis->Draw();
			fCanvas->Update();
			return;
		} else if (what == projectx ||what == projectx_func  ) {
			ClearMarks();
			gHpr->ShowHist(fProjHistX);
			return;
		} else if (what == projecty || what == projectf) {
			ClearMarks();
			gHpr->ShowHist(fProjHistY);
			return;
		} else {
			fSelHist->SetEntries(sum);
		}
	}
	TList *lof = fOrigHist->GetListOfFunctions();
	if (lof) {
		TIter next(lof);
		TObject * p;
		while ( (p = next()) ) {
//            p->Print();
//  dont add TPaletteAxis
			if (strcmp(p->GetName(),"palette"))
				fExpHist->GetListOfFunctions()->Add(p);
		}
	}
	fSelHist = fExpHist;
	fCanvas->cd();
	ClearMarks();
	if (is2dim(fSelHist))
		Draw2Dim();
	else if (is3dim(fSelHist))
		Draw3Dim();
	else
		Draw1Dim();

//   ClearMarks();
	fCanvas->Modified();
}
//____________________________________________________________________________

void FitHist::ProjectOnAnyAxis()
{
	TIter next(fSelHist->GetListOfFunctions());
	TF1 * func = NULL;
	while (TObject * obj =(TObject *)next()) {
		if (is_a_function(obj)) {
			if (func != NULL)
				Hpr::WarnBox("More than 1 function defined, take first");
			else func =(TF1 *)obj;
		}
	}
	if (!func) {
		Hpr::WarnBox("No function found");
		return;
	}
	TString tit = func->GetTitle();
	if (tit != "pol1") {
		Hpr::WarnBox("Function must be a pol1");
		return;
	}
	Double_t apol = func->GetParameter(1);
	Double_t cpol = func->GetParameter(0);
	TH1F * hproj = Hpr::projectany((TH2*)fSelHist, (TH1F*)NULL, cpol, apol);
	if (gHpr) gHpr->ShowHist(hproj);
	else      hproj->Draw();
}

//____________________________________________________________________________

void FitHist::ExecDefMacro()
{
	if (!gSystem->AccessPathName(attrname, kFileExists)) {
		gROOT->LoadMacro(attrname);
		TString cmd = attrname;
		Int_t p = cmd.Index(".");
		cmd.Remove(p, 2);
		cmd =
			 cmd + "(\"" + fCname.Data() + "\",\"" + fSelHist->GetName() +
			 "\")";
//      cout << cmd << endl;
		gHpr->SetCurrentHist(fSelHist);
		gROOT->ProcessLine((const char *) cmd);
	}
}

//____________________________________________________________________________

void FitHist::Draw1Dim()
{
	TString drawopt;
	fCanvas->cd();
	TPaveStats * st1 = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
	if ( st1 )
		fSelHist->GetListOfFunctions()->Remove(st1);
	if ( fCanvas->GetListOfPrimitives()->FindObject(fSelHist) )
		fCanvas->GetListOfPrimitives()->Remove(fSelHist);
	
	if (fShowStatBox) {
		TEnv env(".hprrc");
		gStyle->SetStatX(env.GetValue("SetHistOptDialog.StatX",0.9));
		gStyle->SetStatY(env.GetValue("SetHistOptDialog.StatY",0.9));
		gStyle->SetStatW(env.GetValue("SetHistOptDialog.StatW",0.2));
		gStyle->SetStatH(env.GetValue("SetHistOptDialog.StatH",0.16));
		gStyle->SetOptStat(fOptStat);
		fSelHist->SetStats(1);
		if ( gHprDebug > 1 ) {
			cout << "Draw1Dim fOptStat " << fOptStat << " " << endl;
			cout << "Draw1Dim StatX, Y " << gStyle->GetStatX() << " " << gStyle->GetStatY() << endl;
			cout << "Draw1Dim StatW, H " << gStyle->GetStatW() << " " << gStyle->GetStatH() << endl;
		}
	} else {
		fSelHist->SetStats(0);
		gStyle->SetOptStat(0);
				//			 cout << "fSelHist->SetStats(0); " << endl;
	} 
//	TPaveStats * st1 = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
// 	cout << "Draw1Dim() st1 " << st1  << endl;
	if ( !HasFunctions(fSelHist) ) {
		gStyle->SetOptFit(0);
	} else {
		gStyle->SetOptFit(fShowFitBox);
	}
	fSelHist->Draw();
	if ( gROOT->GetForceStyle() ) {
		fCanvas->UseCurrentStyle();
		if (fErrorMode != "none") {
			 drawopt += fErrorMode;
		} 
//		if ( fMarkerSize > 0 && fShowContour == 0 ) {
//			 drawopt += "P";
//		}
		if (fShowContour != 0) drawopt += "HIST";
//		if (drawopt.Length() == 0 || fShowContour != 0) drawopt += "HIST";
		if (fShowMarkers != 0) drawopt += "P";
		gStyle->SetOptTitle(fShowTitle);
//		if (fFill1Dim && fSelHist->GetNbinsX() < 50000) {
		if ( fFill1Dim ) {
			 if (!drawopt.Contains("HIST"))  drawopt += "HIST";
			 fSelHist->SetFillStyle(fFillStyle);
			 fSelHist->SetFillColor(fFillColor);
		} else
			 fSelHist->SetFillStyle(0);
		if (fSimpleLine)
			drawopt += "L";
		else if (fSmoothLine)
			drawopt += "C";
		else if (fBarChart)
			drawopt += "B";
		else if (fBarChartH)
			drawopt += "BARH";
		else if (fBarChart3D)
			drawopt += "BAR";
		else if (fPieChart)
			drawopt += "PIE";
		else if (fText) {
			drawopt +="TEXT";
			if (fTextAngle > 0){
				drawopt += fTextAngle;
			}
		}
		if (gPad->GetTickx() < 2 && fLabelsTopX)
			drawopt += "X+";
		if (gPad->GetTicky() < 2 && fLabelsRightY)
			drawopt += "Y+";
		fSelHist->SetDrawOption(drawopt.Data());
	 //   fSelHist->DrawCopy();
	 //   fSelHist->Draw();
		if (fTitleCenterX)
			 fSelHist->GetXaxis()->CenterTitle(kTRUE);
		if (fTitleCenterY)
			 fSelHist->GetYaxis()->CenterTitle(kTRUE);
		fCanvas->GetFrame()->SetFillStyle(0);
	 //   Int_t save_optstat = gStyle->GetOptStat();
	}
	if ( gHprDebug > 0 ) {
		cout << "Draw1Dim() " << drawopt << " fSelHist->Draw() " 
		<<fSelHist->GetDrawOption() << " FillColor " 
		<<fSelHist->GetFillColor() << " logy " ;
		if (fCanvas->GetLogy())
			cout << " true";
		else
			cout << " false";
		cout <<  endl;
	}
	if (fShowMarkers == 0 && fText == 0)
		fSelHist->SetMarkerSize(0.01);
//   fSelHist->Draw(drawopt);
	TList *lof = fOrigHist->GetListOfFunctions();

	Double_t ymax  = fSelHist->GetMaximum();
	Double_t ymove = 0.05 * ymax;
	Double_t y0    = 6 * ymove;
	if (lof) {
		TObject *p;
		TIter next(lof);
		while ( (p = (TObject *) next()) ) {
			if ( p->InheritsFrom(TF1::Class()) &&
				drawopt.Contains("HIST") ) {
				((TF1*)p)->Draw("SAME");
//				cout << "Draw manually: " << p->GetName() << endl;
			} else if (p->InheritsFrom(TMrbWindow::Class())) {
				TMrbWindow *w = (TMrbWindow *) p;
				w->SetParent(fSelHist);
				if (w->GetY1() == 0) {
					w->SetY1(y0);
					w->SetY2(y0);
					y0 += ymove;
					if (y0 >= ymax) y0 = 3 * ymove;
				}
				w->Draw();
			}
		}
//      DrawWindows();
	}
	 
	// if xaxis looks like unix time (after year 2000)set time display
	Int_t year2000 = 339869696;
	if ((Int_t)fSelHist->GetXaxis()->GetXmin() > year2000 &&
		 (Int_t)fSelHist->GetXaxis()->GetXmax() > year2000) {
		fSelHist->GetXaxis()->SetTimeDisplay(1);
	}
	if (GeneralAttDialog::fUseAttributeMacro) {
		ExecDefMacro();
//      fSelPad->Modified(kTRUE);
	}
	DrawDate();
	TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
// 	cout << " st " << st << " " << stats << endl;
	if ( st ) {
		if ( HasFunctions(fSelHist) ) {
//		gStyle->SetOptFit(fShowFitBox);
			st->SetOptFit(1);
		} else {
			st->SetOptFit(0);
		}
	}
//	if ( st && GeneralAttDialog::fRememberStatBox ) {
//		Hpr::ResizeStatBox(st, 1);
//	}
	//  add extra axis (channels) at top
	if (fDrawAxisAtTop) {
		if (st) {
			st->SetY1NDC(st->GetY1NDC()-0.15);
			st->SetY2NDC(st->GetY2NDC()-0.15);
		}
//      DrawTopAxis();

		TString cmd("((FitHist*)gROOT->FindObject(\"");
		cmd += GetName();
		cmd += "\"))->DrawTopAxis()";

		TExec * exec = new TExec("exec", cmd.Data());
		exec->Draw();

	}
	UpdateDrawOptions();
	if (fLogy)
		fCanvas->SetLogy();
	if (fLogx)
		fCanvas->SetLogx();
	fCanvas->Update();
	if (gHprDebug > 0) {
		cout << "exit Draw1Dim() " <<fSelHist->GetDrawOption()
		<< " logy " ;
		if (fCanvas->GetLogy())
			cout << " true";
		else
			cout << " false";
		cout <<  endl;
	}
//   gStyle->SetOptStat(save_optstat);
}
//____________________________________________________________________________

void FitHist::DrawDate()
{
	if (fShowDateBox ) {
		TDatime dt;
		TString dtext("Drawn at: ");
		UInt_t da = fSelHist->GetUniqueID();
		if (da != 0 && !fUseTimeOfDisplay) {
			dt.Set(da);
			dtext = "Created at: ";
		}
		dtext += dt.AsSQLString();
		fDateText = new TText(gStyle->GetDateX(),gStyle->GetDateY(),dtext);
		fDateText->SetName("	DateBox");	
		fDateText->SetTextSize( gStyle->GetAttDate()->GetTextSize());
		fDateText->SetTextFont( gStyle->GetAttDate()->GetTextFont());
		fDateText->SetTextColor(gStyle->GetAttDate()->GetTextColor());
		fDateText->SetTextAlign(gStyle->GetAttDate()->GetTextAlign());
		fDateText->SetTextAngle(gStyle->GetAttDate()->GetTextAngle());
		fDateText->SetNDC();
		fDateText->Draw();
	}
}
//____________________________________________________________________________

void FitHist::Draw2Dim()
{
	fCanvas->cd();
	TPaveStats * st1 = (TPaveStats *)fSelHist->GetListOfFunctions()->FindObject("stats");
	if ( st1 )
		fSelHist->GetListOfFunctions()->Remove(st1);
	if ( fCanvas->GetListOfPrimitives()->FindObject(fSelHist) )
		fCanvas->GetListOfPrimitives()->Remove(fSelHist);
	if (gHprDebug > 2)
		cout << "Draw2Dim:" << " GetLogy " << fCanvas->GetLogy() << endl;
	if ( gHprDebug > 1 ) {
		cout << "FitHist::DrawOpt2Dim: " <<fDrawOpt2Dim 
		<< " gStyle->GetHistFillColor() :" <<gStyle->GetFillColor() << endl
		<< " fHistFillColor2Dim :" <<fHistFillColor2Dim<< endl
		<< " fHistFillStyle2Dim :" <<fHistFillStyle2Dim<< endl
		<< " fSelHist->GetMinimum() " << fSelHist->GetMinimum()<< endl;
	}
	if (fShowStatBox) {
		TEnv env(".hprrc");
		//      cout << "fSelHist->SetStats(1); " << fOptStat << endl;
		gStyle->SetStatX(env.GetValue("SetHistOptDialog.StatX2D",0.9));
		gStyle->SetStatY(env.GetValue("SetHistOptDialog.StatY2D",0.9));
		gStyle->SetStatW(env.GetValue("SetHistOptDialog.StatW2D",0.2));
		gStyle->SetStatH(env.GetValue("SetHistOptDialog.StatH2D",0.2));
		gStyle->SetOptStat(fOptStat);
		fSelHist->SetStats(1);
	} else {
		fSelHist->SetStats(0);
		gStyle->SetOptStat(0);
	} 
	if ( fDrawOpt2Dim.Contains("LEGO") 
	    && GeneralAttDialog::fLegoSuppressZero == 1 
	    && !fDrawOpt2Dim.Contains("0") )
		fDrawOpt2Dim.Append("0");
	new SetColorModeDialog(NULL, 1);    //Initialize color palettes
	fSelHist->Draw(fDrawOpt2Dim);
	if ( gROOT->GetForceStyle() ) {
		fCanvas->UseCurrentStyle();
	}
	fCanvas->SetLogx(fLogx);
	fCanvas->SetLogy(fLogy);
	SetLogz(fLogz);
	fCanvas->GetHandleMenus()->SetLog(fLogz);
	if ( gHprDebug > 1 ) {
		cout << " fSelHist->GetMinimum() " << fSelHist->GetMinimum()<< endl;
		cout << "Draw2Dim:fDrawOpt2Dim " << fDrawOpt2Dim << endl;
	}
	fSelHist->SetOption(fDrawOpt2Dim);
	fSelHist->SetDrawOption(fDrawOpt2Dim);
	if (fTitleCenterX)
		fSelHist->GetXaxis()->CenterTitle(kTRUE);
	if (fTitleCenterY)
		fSelHist->GetYaxis()->CenterTitle(kTRUE);
	if (fTitleCenterY)
		fSelHist->GetYaxis()->CenterTitle(kTRUE);
	fSelHist->SetFillColor(fHistFillColor2Dim);
	fSelHist->SetFillStyle(fHistFillStyle2Dim);
	fSelHist->SetLineColor(fHistLineColor2Dim);
	fSelHist->SetLineStyle(fHistLineStyle2Dim);
	fSelHist->SetLineWidth(fHistLineWidth2Dim);
	fSelHist->SetMarkerColor(fMarkerColor2Dim);  
	fSelHist->SetMarkerStyle(fMarkerStyle2Dim);  
	fSelHist->SetMarkerSize (fMarkerSize2Dim);   			
	DrawCut();
	TList *lof = fOrigHist->GetListOfFunctions();
	if (lof) {
		TObject *p;
		TIter next(lof);
		while ( (p = (TObject *) next()) ) {
//         p->Print();
			if (p->InheritsFrom(TF1::Class())) {
				TF1 *f =  dynamic_cast<TF1*>(p);
				 f->Draw("same");
				f->SetParent(fSelHist);
			} else if (p->InheritsFrom(TMrbWindow2D::Class())) {
				TMrbWindow2D *w = dynamic_cast<TMrbWindow2D*>(p);
				w->Draw("same");
				w->SetParent(fSelHist);
				fActiveCuts->Add(w);
			} else if  (p->InheritsFrom(TMrbNamedArrayI::Class())) {
				 TMrbNamedArrayI * nai = dynamic_cast<TMrbNamedArrayI*>(p);
				 TString name(nai->GetName());
				 if (name.BeginsWith("Pixel")) {
					 cout << "SetUserPalette" << endl;
					 Hpr::SetUserPalette(1001, nai);
				 }
			}
		}
	}
	if (GeneralAttDialog::fUseAttributeMacro) {
		ExecDefMacro();
		fSelPad->Modified(kTRUE);
	}
	UpdateCanvas();
	if (f2DimBackgroundColor == 0) {
		if (gStyle->GetCanvasColor() == 0) {
			fCanvas->GetFrame()->SetFillStyle(0);
		} else {
			fCanvas->GetFrame()->SetFillStyle(1001);
			fCanvas->GetFrame()->SetFillColor(10);
		}
	} else {
		fCanvas->GetFrame()->SetFillStyle(1001);
		fCanvas->GetFrame()->SetFillColor(f2DimBackgroundColor);
	}   
	UpdateDrawOptions();
	DrawDate();
	fCanvas->Modified();
	fCanvas->Update();
//	TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
//   cout << "Draw2Dim: st " << st << endl;
//	if ( st && GeneralAttDialog::fRememberStatBox ) {
//		Hpr::ResizeStatBox(st, 2);
//		fCanvas->Modified();
//	}
	if (!fCanvas->GetAutoExec())
		fCanvas->ToggleAutoExec();
	fCanvas->Update();
}
//____________________________________________________________________________

void FitHist::Draw3Dim()
{
//   TString drawopt("iso");

	if ( gHprDebug > 1 )
		cout << "fDrawOpt3Dim " << fDrawOpt3Dim << endl;
	TEnv env(".hprrc");
	fCanvas->SetPhi(env.GetValue("Set3DimOptDialog.fPhi3Dim",     30));
	fCanvas->SetTheta(env.GetValue("Set3DimOptDialog.fTheta3Dim", 30));
	if (fTitleCenterX) {
		fSelHist->GetXaxis()->CenterTitle(kTRUE);
//		cout << "fSelHist->GetXaxis()->CenterTitle(kTRUE); " << endl;
	}
	if (fTitleCenterY)
		fSelHist->GetYaxis()->CenterTitle(kTRUE);
	if (fTitleCenterZ) {
		fSelHist->GetZaxis()->CenterTitle(kTRUE);
//		cout << "fSelHist->GetZaxis()->CenterTitle(kTRUE); " << endl;
	}
//	fSelHist->GetXaxis()->SetTicks(env.GetValue("SetHistOptDialog.fTicksX3Dim","-"));
	if ( gROOT->GetForceStyle() ) {
		fCanvas->UseCurrentStyle();
	}
	fSelHist->GetYaxis()->SetTickLength(env.GetValue("SetHistOptDialog.fTickLengthX3Dim",0.01));
	fSelHist->GetZaxis()->SetTitleOffset(env.GetValue("SetHistOptDialog.fTitleOffsetZ3Dim", 1.5));
	TAxis * ax = fSelHist->GetXaxis();
	TAxis * ay = fSelHist->GetYaxis();
	TAxis * az = fSelHist->GetZaxis();
	Double_t x0, y0, z0, x1, y1, z1;
	x0 = ax->GetXmin();
	y0 = ay->GetXmin();
	z0 = az->GetXmin();
	x1 = ax->GetXmax();
	y1 = ay->GetXmax();
	z1 = az->GetXmax();
	TString cname = fCanvas->GetName();
//	cout << "Canvas name: " << cname << endl;
	if ( !cname.BeginsWith("C_F") ) {
		cout << "Illegal canvas name " << endl;
	} else {
		cname = cname(3,cname.Length()-3);
		TEnv * lastset = GetDefaults(cname);
		if (lastset) {
			x0 = lastset->GetValue("fRangeX0",x0);
			y0 = lastset->GetValue("fRangeY0",y0);
			z0 = lastset->GetValue("fRangeZ0",z0);
			x1 = lastset->GetValue("fRangeX1",x1);
			y1 = lastset->GetValue("fRangeY1",y1);
			z1 = lastset->GetValue("fRangeZ1",z1);
			ax->SetRangeUser(x0, x1);
			ay->SetRangeUser(y0, y1);
			az->SetRangeUser(z0, z1);
		}
	}
	if ( fDrawOpt3Dim.Contains("PolyMHist") ) {
		Draw3DimPolyMarker();
		TButton *cb = this->GetCmdButton();
		new Set3DimOptDialog((TRootCanvas*)fCanvas->GetCanvasImp(), cb);
		return;
	}
	if ( fDrawOpt3Dim.Contains("PolyMView") ) {
		Draw3DimView();
//		new Set3DimOptDialog((TRootCanvas*)fCanvas->GetCanvasImp(), GetCmdButton());
		return;
	}
	if (fShowStatBox) {
		gStyle->SetOptStat(fOptStat);
		fSelHist->SetStats(1);
		//      cout << "fSelHist->SetStats(1); " << fOptStat << endl;
		gStyle->SetStatX(env.GetValue("SetHistOptDialog.StatX2D",0.9));
		gStyle->SetStatY(env.GetValue("SetHistOptDialog.StatY2D",0.9));
		gStyle->SetStatW(env.GetValue("SetHistOptDialog.StatW2D",0.2));
		gStyle->SetStatH(env.GetValue("SetHistOptDialog.StatH2D",0.2));
	} else {
		fSelHist->SetStats(0);
	} 
	gTranspThresh      = env.GetValue("Set3DimOptDialog.gTranspThresh",      1);
	gTranspBelow       = env.GetValue("Set3DimOptDialog.gTranspBelow",   0.005);
	gTranspAbove       = env.GetValue("Set3DimOptDialog.gTranspAbove",     0.9);

	TList * lof = fSelHist->GetListOfFunctions();
	TObject * trf= lof->FindObject("TransferFunction");
	if ( fDrawOpt3Dim.Contains("GL") && fApplyTranspCut ) {
		if ( !trf ) {
			TF1 * tf = new TF1("TransferFunction", my_transfer_function);
			lof->Add(tf);
		}
	} else {
		if (trf) {
			lof->Remove(trf);
			delete trf;
		}
	}
	fSelHist->SetDrawOption(fDrawOpt3Dim);
	fSelHist->SetOption(fDrawOpt3Dim);
	fSelHist->Draw(fDrawOpt3Dim);
	fSelHist->SetFillColor(fHistFillColor3Dim);
	fSelHist->SetLineColor(fHistLineColor3Dim);
	fSelHist->SetLineStyle(1);
	fSelHist->SetLineWidth(1);
	fSelHist->SetMarkerColor(fMarkerColor3Dim);
	fSelHist->SetMarkerStyle(fMarkerStyle3Dim);
	fSelHist->SetMarkerSize (fMarkerSize3Dim);
	UpdateDrawOptions();
	fCanvas->cd();
	fCanvas->Update();
}
//____________________________________________________________________________

void FitHist::Draw3DimPolyMarker()
{
	TH3F * hempty = (TH3F*)fSelHist->Clone();
	hempty->Reset();
	TString nname = fSelHist->GetName();
	nname += "_Clone";
	hempty->SetName(nname);
	fCanvas->SetRightMargin(0.16);
	TGStatusBar *sb = ((TRootCanvas*)fCanvas->GetCanvasImp())->GetStatusBar();
	if ( sb ) {
		Int_t parts[4] = {20, 50, 10, 20};
		sb->SetParts(parts, 4);
	}
	hempty->Draw();

	hempty->SetStats(0);
	Double_t hmin = fSelHist->GetMinimum();
	Double_t hmax = fSelHist->GetMaximum();
	Int_t logc = 0;
	TEnv env(".hprrc");
	Double_t min = env.GetValue("Set3DimOptDialog.fContMin", hmin);
	Double_t max = env.GetValue("Set3DimOptDialog.fContMax", hmax);
	if (max > hmax)
		max = hmax;
	if (min < hmin)
		min = hmin;
	logc = env.GetValue("Set3DimOptDialog.fContLog", logc);
//	gStyle->SetPalette(1);
	TAxis * ax = fSelHist->GetXaxis();
	TAxis * ay = fSelHist->GetYaxis();
	TAxis * az = fSelHist->GetZaxis();
	Double_t min_col = min;
	Double_t max_col = max;
	if ( logc ) {
		if ( min_col <= 0 ) 
			min_col = 0.01;
		min_col = TMath::Log10(min_col);
		max_col = TMath::Log10(max_col);
	}
	// this corrects a bug in root?
	hempty->GetYaxis()->SetTickLength(hempty->GetXaxis()->GetTickLength());
	TPolyMarker3D * pm;
	Double_t x, y, z;
	Int_t ncolors = gStyle->GetNumberOfColors();
	if ( gHprDebug > 0)
		cout << "Draw3DimPolyMarker: hist Min, Max, log, ncolors: "
		<< fSelHist << " " << min << " " << max << " " << logc<< " " << ncolors << endl;
	
	for (Int_t ix = 0; ix < ax->GetNbins(); ix++) {
		for (Int_t iy = 0; iy < ay->GetNbins(); iy++) {
			for (Int_t iz = 0; iz < az->GetNbins(); iz++) {
				Float_t cont = fSelHist->GetBinContent(ix, iy, iz);
				if (cont <= 0 )
					continue;
				Float_t cont_col = cont;	
				if ( logc > 0 )
					cont_col = TMath::Log10(cont);
//				if (cont != 0 && cont >= min && cont <= max ) {
				if ( cont != 0 ) {
					pm = new TPolyMarker3D(1);
					x = ax->GetBinCenter(ix);
					y = ay->GetBinCenter(iy);
					z = az->GetBinCenter(iz);
					pm->SetPoint(0,x, y, z);
					Int_t col = (Int_t)((cont_col / (max_col-min_col) )* ncolors);
					Int_t theColor = Int_t((col+0.99)*Float_t(ncolors)/Float_t(ncolors));
					if (theColor > ncolors-1) theColor = ncolors-1;
//					cout << " max  cont Col "<< max << " "  << cont << " " << col << endl;
					UInt_t icont = (UInt_t)cont;
					pm->SetUniqueID(icont);
					pm->SetName(Form(" X =  %6.4g Y =  %6.4g Z = %6.4g Content %6.4g",
										  x, y, z, cont));
					pm->SetMarkerColor(gStyle->GetColorPalette(theColor));
					pm->SetMarkerStyle(fMarkerStyle3Dim);
					if (cont >= min && cont <= max ) {
						pm->SetMarkerSize(fMarkerSize3Dim);
					} else {
						pm->SetMarkerSize(0);
					}
					pm->Draw();
				}
			}
		}
	}
	TH2F* h3 = (TH2F*)gROOT->FindObject("DummyForTPaletteAxis");
	if ( !h3)
		h3 = new TH2F("DummyForTPaletteAxis","v",2,0,10, 2,0,10);
	h3->SetMinimum(min);
	h3->SetMaximum(max);
	/*
	Double_t uc;
	h3->SetContour(ncolors);
	for (int i=0; i < ncolors; i++) {
		uc = min + i * (max - min) / ncolors;
		if (logc > 0)
			uc = TMath::Log10(uc);
	}
	*/
	if ( logc == 0) {
		TArrayD uc(ncolors);
		for (int i=0; i < ncolors; i++) uc[i] = min + i * (max - min) / ncolors;
		h3->SetContour(ncolors, uc.GetArray());
		TPaletteAxis *pa = new TPaletteAxis(0.85, -0.95,0.9,0.9, h3);
		pa->Draw();
	}
}

///____________________________________________________________________________

void FitHist::Draw3DimView()
{
//	TH3F * hempty = (TH3F*)fSelHist->Clone();
//	hempty->Reset();
//	fCanvas->SetRightMargin(0.16);
	TGStatusBar *sb = ((TRootCanvas*)fCanvas->GetCanvasImp())->GetStatusBar();
	if ( sb ) {
		Int_t parts[4] = {20, 50, 10, 20};
		sb->SetParts(parts, 4);
	}
//   Double_t rx0 = 0, rx1 = 10, ry0 = 0, ry1 = 10, rz0 = 0, rz1 = 10;
	TAxis * ax = fSelHist->GetXaxis();
	TAxis * ay = fSelHist->GetYaxis();
	TAxis * az = fSelHist->GetZaxis();
	Double_t x0, y0, z0, x1, y1, z1;
	x0 = ax->GetXmin();
	y0 = ay->GetXmin();
	z0 = az->GetXmin();
	x1 = ax->GetXmax();
	y1 = ay->GetXmax();
	z1 = az->GetXmax();
	TString cname = fCanvas->GetName();
//	cout << "Canvas name: " << cname << endl;
	if ( !cname.BeginsWith("C_F") ) {
		cout << "Illegal canvas name " << endl;
	} else {
		cname = cname(3,cname.Length()-3);
		TEnv * env = GetDefaults(cname);
		if (env) {
			x0 = env->GetValue("fRangeX0",x0);
			y0 = env->GetValue("fRangeY0",y0);
			z0 = env->GetValue("fRangeZ0",z0);
			x1 = env->GetValue("fRangeX1",x1);
			y1 = env->GetValue("fRangeY1",y1);
			z1 = env->GetValue("fRangeZ1",z1);
			ax->SetRangeUser(x0, x1);
			ay->SetRangeUser(y0, y1);
			az->SetRangeUser(z0, z1);
		}
	}
	Double_t rmin[3], rmax[3];
	Int_t binlx = ax->GetFirst();
	Int_t binly = ay->GetFirst();
	Int_t binlz = az->GetFirst();
	Int_t binux = ax->GetLast();
	Int_t binuy = ay->GetLast();
	Int_t binuz = az->GetLast();
	
	rmin[0] = x0 = ax->GetBinLowEdge(binlx);
	rmin[1] = y0 = ay->GetBinLowEdge(binly);
	rmin[2] = z0 = az->GetBinLowEdge(binlz);
	rmax[0] = x1 = ax->GetBinLowEdge(binux)+ ax->GetBinWidth(binux);
	rmax[1] = y1 = ay->GetBinLowEdge(binuy)+ ay->GetBinWidth(binuy);
	rmax[2] = z1 = az->GetBinLowEdge(binuz)+ az->GetBinWidth(binuz);
	fCanvas->Range(rmin[0], rmin[1], rmax[0], rmax[1]);
	TView3D *view = new TView3D(1, rmin, rmax);
	view->ShowAxis();
	TEnv env1(".hprrc");
	Double_t vphi    = env1.GetValue("Set3DimOptDialog.fPhi3Dim", 135.);
	Double_t vtheta  = env1.GetValue("Set3DimOptDialog.fTheta3Dim", 30.);
	fCanvas->SetPhi(vphi);
	fCanvas->SetTheta(vtheta);

//	hempty->SetStats(0);
	Float_t min = fSelHist->GetMinimum();
	Float_t max = fSelHist->GetMaximum();
//	gStyle->SetPalette(1);
	TPolyMarker3D * pm;
	Double_t x, y, z;
	Hpr::BoundingB3D(NULL, x0, y0, z0, x1, y1, z1);
/*	
	TString tit = ax->GetTitle();
	
	if (tit.Length() > 0) {
		TText3D * tt =new TText3D(x0, y0 - 0.08*(x1-x0), z0,
										  (x1-x0), 0, 1, ax->GetTitle());
		tt->SetTextSize(0.04);
		tt->Draw();
	}*/
	
	Int_t ncolors = gStyle->GetNumberOfColors();
	for (Int_t ix = binlx; ix <= binux ; ix++) {
		for (Int_t iy = binly; iy <= binuy; iy++) {
			for (Int_t iz = binlz; iz < binuz; iz++) {
				Float_t cont = fSelHist->GetBinContent(ix, iy, iz);
				if (cont != 0) {
					pm = new TPolyMarker3D(1);
					x = ax->GetBinCenter(ix);
					y = ay->GetBinCenter(iy);
					z = az->GetBinCenter(iz);
					pm->SetPoint(0,x, y, z);
					Int_t col = (Int_t)((cont / (max-min) )* ncolors);
					Int_t theColor = Int_t((col+0.99)*Float_t(ncolors)/Float_t(ncolors));
					if (theColor > ncolors-1) theColor = ncolors-1;
//					Int_t col = (Int_t)((cont / (max-min) )* 50. + 50);
//					cout << " max  cont Col "<< max << " "  << cont << " " << col << endl;
//					UInt_t icont = (UInt_t)cont;
//					pm->SetUniqueID(icont);
					pm->SetName(Form(" X =  %6.4g Y =  %6.4g Z = %6.4g Content %6.4g",
										  x, y, z, cont));
					pm->SetMarkerColor(gStyle->GetColorPalette(theColor));
					pm->SetMarkerStyle(fMarkerStyle3Dim);
					pm->SetMarkerSize(fMarkerSize3Dim);
					pm->Draw();
				}
			}
		}
	}
	
	TH2F* h3 = (TH2F*)gROOT->FindObject("DummyForTPaletteAxis");
	if ( !h3)
		h3 = new TH2F("DummyForTPaletteAxis","v",2,0,10, 2,0,10);
	h3->SetMinimum(min);
	h3->SetMaximum(max);
	Double_t uc[20];
	for (int i=0; i < 20; i++) uc[i] = min + i * (max - min) / 20.;
	h3->SetContour(20,uc);
	TPaletteAxis *pa = new TPaletteAxis(0.85, -0.9,0.9,0.9, h3);
	pa->Draw();
	fCanvas->cd();
	fCanvas->Update();

}

//__________________________________________________________________________________

void FitHist::AddFunctionsToHist()
{
	Int_t nval = CheckList(fAllFunctions);
	if (nval>0) {
		TF1 *fun;
		TIter next(fAllFunctions);
		while ( (fun = (TF1*)next()) ) {
			TF1 * funcp= new TF1(*fun);
			fSelHist->GetListOfFunctions()->Add(funcp);
			fCanvas->cd();
//         funcp->Draw("same");
			fun->Draw("same");
			fCanvas->Modified(kTRUE);
			fCanvas->Update();
		}
	} else Hpr::WarnBox("No function selected");
}
//______________________________________________________________________________________

void FitHist::SetLogz(Int_t state)
{
//	cout << "SetLogz(Int_t state) " << state<< endl;
	fLogz = state;
	if (gHpr && gHpr->fLogScaleMin > 0){
		if(state > 0)
			fSelHist->SetMinimum(gHpr->fLogScaleMin);
		else
			fSelHist->SetMinimum(gHpr->fLinScaleMin);
	}
	fCanvas->SetLogz(state);
//   SaveDefaults();
}
//______________________________________________________________________________________

void FitHist::UpdateDrawOptions()
{
//   if (!gHpr) return;
//   SetSelectedPad();
	fSelPad->cd();
	TString drawopt;
	if (fDimension == 1) {
	} else if (fDimension  == 2) {
		fSelHist->SetOption(fDrawOpt2Dim);
		fSelHist->SetDrawOption(fDrawOpt2Dim);
	}
//   if (gROOT->GetForceStyle()) {
//      SetHistOptDialog::SetDefaults(fSelHist->GetDimension());
//   }
//   gPad->Modified();
//   gPad->Update();
}
//______________________________________________________________________________________

void FitHist::ColorMarked()
{
	if (fSelHist->GetDimension() != 1) {
		cout << "Currently only 1-dim supported" << endl;
		return;
	}
	GetLimits();
	Int_t color = getcol();
	if (color <= 0) return;
	TGraph * pl = PaintArea(fSelHist, fBinX_1, fBinX_2, color);
	if (pl) {
		fCanvas->cd();
		pl->Draw("LF");
		fSelHist->GetListOfFunctions()->Add(pl, "LF");
	}
	fCanvas->Modified();
	fCanvas->Update();
}
//____________________________________________________________________________________

void FitHist::Fit2DimD(Int_t npeaks)
{
	if (fFit2DimD == NULL)
		fFit2DimD = new Fit2DimDialog((TH2*)fSelHist, npeaks);
}
//____________________________________________________________________________________

void FitHist::Fit1DimDialog(Int_t type)
{
	if (fFit1DimD == NULL)
		fFit1DimD = new FitOneDimDialog(fSelHist, type);
}
///__________________________________________________________________

void FitHist::FindPeaks()
{
	if (fSelHist->GetDimension() == 1) 
		new FindPeakDialog(fSelHist);
	else if (fSelHist->GetDimension() == 2) 
		new FindPeakDialog2D((TH2*)fSelHist);
};

//__________________________________________________________________

void FitHist::Calibrate()
{
		new CalibrationDialog(fSelHist, 1);
};
//__________________________________________________________________

void FitHist::SetLogx(Int_t state)
{
	fLogx = state;
	fCanvas->SetLogx(state);
	fCanvas->Modified();
	fCanvas->Update();
};
//__________________________________________________________________

void FitHist::SetLogy(Int_t state)
{
	fLogy = state;
	if (state != 0)
		fSelHist->SetMinimum(-1111);
	fCanvas->SetLogy(state);
	fCanvas->Modified();
	fCanvas->Update();
};


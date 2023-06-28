#include "TROOT.h"
#include "TEnv.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TFile.h"
#include "TMath.h"
#include "TKey.h"
#include "TCutG.h"
#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TGMsgBox.h"
#include "TGFileDialog.h"
#include "TObject.h"
#include "TTree.h"
#include "TBranch.h"
#include "TBranchObject.h"
#include "TLeafObject.h"
#include "TProfile.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TPaveStats.h"
#include "TVirtualPad.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TApplication.h"
#include "TDiamond.h"
#include "TCanvas.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TFrame.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjectTable.h"
#include "TList.h"
#include "TMessage.h"
#include "TGraphAsymmErrors.h"
#include "TGraph2D.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TQObject.h"
#include "TProof.h"
#include "TBenchmark.h"

#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FitHist.h"
#include "FhContour.h"
//#include "FitHist_Help.h"
#include "support.h"
//#include "TGMrbValuesAndText.h"
#include "TGMrbInputDialog.h"
#include "TMrbStatistics.h"
#include "HTCanvas.h"
#include "GEdit.h"
#include "GroupOfHists.h"
#include "SetColor.h"
//#include "TMrbString.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbHelpBrowser.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "GraphAttDialog.h"
#include "AddFitMenus.h"
#include "HprStack.h"
#include "HprFunc2.h"

#include "HprGaxis.h"
#include "hprbase.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>

HistPresent *gHpr = NULL;
Int_t gHprDebug;
Int_t gHprClosing;
Int_t gHprTerminated;
TString gHprWorkDir;
TString gHprLocalEnv;
static const char  *fHlistSuffix=".histlist";
Int_t nHists;
extern TH1 * gHpHist;

//_____________________________________________________________________________________

const char AttrTemplate[]=
"FH_setdefaults(const char *cname, const char* hname) \n\
{\n\
\n\
// This is a template attributes file\n\
\n\
// this macro is called when FitHist draws / expands a histogram\n\
// please read the documentation (http://root.cern.ch) for all methods\n\
// defined for histograms and canvases and styles\n\
\n\
// Methods of HistPresent itself may be used via the pointer gHpr.\n\
// This can be used to get a histograms from any file  and overlay it\n\
// See line with GetHist()\n\
\n\
//  gROOT->Reset();\n\
// find pointer to HistPresent object\n\
  class HistPresent;\n\
//  HistPresent * gHpr = (HistPresent*)gROOT->GetList()->FindObject(\"gHpr\");\n\
  if (!gHpr) { \n\
	  cout << \"histogram presenter object not found\" << endl;\n\
	  return 0;\n\
  }\n\
  TCanvas* canvas = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(cname);\n\
  TH1* hist = 0;\n\
  if (gROOT->GetVersionInt() >= 22306) \n\
	  TH1* hist = gHpr->GetCurrentHist();   // assure backward comp\n\
  else    hist = (TH1*)gROOT->GetList()->FindObject(hname);\n\
\n\
  if (!hist || !canvas) {\n\
	  cout << \"hist or canvas not found\" << endl;\n\
	  return 0;\n\
  }\n\
\n\
//   cout << \" Setting defaults for histogram\" <<  endl;    // tell what we are doing\n\
//   cout << \" Name:  \" << hist->GetName() <<  endl;\n\
	TString title = hist->GetTitle();\n\
	TString name = hist->GetName();\n\
\n\
//  global attributes\n\
\n\
	gStyle->SetOptStat(1001111);    // draw a statistics box\n\
											  // Integral,(overf, underf,) mean, rms, entries, name \n\
//   gStyle->SetTitleYSize(0.08);\n\
//   gStyle->SetStatH(0.3);\n\
//   gStyle->SetStatW(0.3);\n\
//   gStyle->SetPadLeftMargin(0.07);\n\
//   gStyle->SetPadRightMargin(0.02);\n\
//   gStyle->SetPadTopMargin(0.08);\n\
//   gStyle->SetPadBottomMargin(0.05);\n\
\n\
//   canvas specific attributes\n\
\n\
//    gPad->GetFrame()->SetFillStyle(1001);       // colored background \n\
//    gPad->GetFrame()->SetFillColor(38);         // sea blue\n\
//    gPad->GetFrame()->SetFillStyle(0);   // empty background\n\
//    gPad->SetFillStyle(0);\n\
\n\
//   histogram specific attributes\n\
//   if it should apply for 1 or 2 dim put inside if (  )\n\
\n\
//   hist->SetLabelSize(0.06, \"Y\");\n\
//   hist->SetLabelSize(0.06, \"X\");\n\
//   hist->SetNdivisions(505, \"Y\");\n\
//   hist->SetNdivisions(505, \"X\");\n\
//   hist->SetDrawOption(\"hist\");\n\
//   hist->SetFillStyle(1001);\n\
//   hist->SetFillColor(42);\n\
\n\
//   Example how to use regular expressions \n\
\n\
//   TRegexp end_with_w=\"w$\";\n\
//   if (name.Index(end_with_w) >= 0) \n\
//     cout <<  \"Hist name ends with w\"<< endl;\n\
//   TRegexp start_with_e=\"^e\";\n\
//   if (name.Index(start_with_e) >= 0) \n\
//      cout <<  \"Hist name starts with e\" << endl;\n\
\n\
	 gROOT->ForceStyle();            // overwrite attributes store with hists\n\
\n\
	if (hist->InheritsFrom(\"TH3\"))  // for 3 dim hists\n\
	{\n\
//      if (title == \"mprer\")hist->SetMaximum(200);\n\
//      hist->SetMinimum(1000);\n\
		hist->SetDrawOption(\"box\");\n\
//      hist->SetFillStyle(1001);\n\
//      hist->SetFillColor(46);\n\
		canvas->Modified(kTRUE);\n\
//      canvas->SetLogz();                   // log scale \n\
	} else if (hist->InheritsFrom(\"TH2\"))  // for 2 dim hists\n\
	{\n\
//      if (title == \"mprer\")hist->SetMaximum(200);\n\
//      hist->SetMinimum(1000);\n\
		hist->SetDrawOption(\"box\");\n\
		hist->SetFillStyle(1001);\n\
		hist->SetFillColor(46);\n\
		canvas->Modified(kTRUE);\n\
//      canvas->SetLogz();                   // log scale \n\
	} else {                          // options for 1 dim \n\
\n\
//     overlay another histogram if name matches hsimple_hpx\n\
\n\
//      if (name == \"hsimple_hpx\") {\n\
//     get histogam named hpx1 from file hsimple.root\n\
//         TH1 * hs = gHpr->GetHist(\"hsimple.root\",\"hpx1\");\n\
//         if (hs) {\n\
//            draw and set line color to blue\n\
//            hs->Draw(\"same\");\n\
//            hs->SetLineColor(4);\n\
//         }\n\
//      }\n\
//      hist->GetXaxis()->SetRange(50,2050); // draw only bins 50-2050 \n\
		hist->SetDrawOption(\"hist\");\n\
//      hist->SetFillStyle(0);               // hollow\n\
//      hist->SetLineColor(6);\n\
		hist->SetFillStyle(1001);      // solid\n\
		hist->SetFillColor(45);\n\
//      hist->SetMaximum(1000);\n\
	}\n\
}\n\
";
//_____________________________________________________________________________________

ClassImp(HistPresent)

// default constructor

HistPresent::HistPresent()
{
//	cout<< setred<< "HistPresent default ctor " << this<< setblack <<endl;
	fComSocket       = NULL;
	cHPr = NULL;
	gHprClosing = 0;
}
// normal constructor

HistPresent::HistPresent(const Text_t *name, const Text_t *title)
				:TNamed(name,title)
{
	if (gDebug > 0)
		cout<< "HistPresent normal ctor " << this << " gHpr = " << gHpr  <<endl;
//   TDirectory *fDirectory;
//   TList *fFunctions;
	fComSocket       = NULL;
	cHPr = NULL;
//	if(gHpr != NULL) {
//		cout << setred << " HistPresent exists " << this << setblack<< endl;
//	} else {
	gHpr = this;
	TEnv envhpr(".hprrc");
	gHprDebug = envhpr.GetValue("HistPresent.Debuglevel", 0);
	gHprTerminated = 0;
	gHprWorkDir = gSystem->pwd();
	gHprLocalEnv = gHprWorkDir + "/.hprrc";
	gHprClosing = 0;
	fOpfac= 1.;
	fRebin = 2;
	fRMethod = 0;
	fSeqNumberMany = 0;
	fSeqNumberGraph = 0;
	fPageNumber = 0;
	fNtupleSeqNr = 0;

	WindowSizeDialog::fNwindows=   0;
	fDialogShowTree = NULL;
	fRootFile = NULL;
	activeFile= NULL;
	activeHist= NULL;
	fFileList = NULL;
	fControlBar = NULL;
	fRootCanvas= NULL;
	fCurrentHist = NULL;
	WindowSizeDialog::fMainWidth = 300;

	fByTitle=kFALSE;

//   fCanvasList = new TList();
	fAllCuts = new TList(); fAllCuts->SetName("fAllCuts");
	fAllWindows = new TList(); fAllWindows->SetName("fAllWindows");
	fAllFunctions = new TList(); fAllFunctions->SetName("fAllFunctions");
	fHistLists = new TList(); fHistLists->SetName("fHistLists");
	fSelectHist = new TList(); fSelectHist->SetName("fSelectHist");
	fSelectCanvas = new TList(); fSelectCanvas->SetName("fSelectCanvas");
	fHistListList = new TList(); fHistListList->SetName("fHistListList");
	fSelectCut = new TList(); fSelectCut->SetName("fSelectCut");
	fSelectWindow = new TList(); fSelectWindow->SetName("fSelectWindow");
	fSelectLeaf = new TList(); fSelectLeaf->SetName("fSelectLeaf");
	fSelectContour = new TList(); fSelectContour->SetName("fSelectContour");
	fSelectGraph = new TList(); fSelectGraph->SetName("fSelectGraph");
	fAllContours = new TList(); fAllContours->SetName("fAllContours");

	fGraphCut = new TString();
	fCutVarX = new TString();
	fCutVarY = new TString();
	fLeafCut = new TString();
	fExpression = new TString();
	fHelpDir    = new TString();
	fAutoUpdateDelay = 5;
	fHostToConnect  = new TString("localhost");
	fSocketToConnect = 9090;
	fConnectedOnce = kFALSE;
//	fSocketIsOpen = kFALSE;
	fAnyFromSocket = kFALSE;
	fHfromM_aButton = NULL;
	fLastWindow = NULL;
	fHelpBrowser = NULL;
	fComSocket = NULL;
	fUseHist = kFALSE;
	fApplyGraphCut = kFALSE;
	fApplyLeafCut = kFALSE;
	fApplyExpression = kFALSE;
	fCanvasClosing = kFALSE;
	fLabelMaxDigits = 3;
	fEditor = gSystem->Getenv("EDITOR");
	if (fEditor.Length() < 2)
		fEditor = "vi";
	fEditor.Append(" ");
	RestoreOptions();
	TEnv env(".rootrc");
	TString defdir(gSystem->Getenv("MARABOU"));
	TString icondir(defdir);
	defdir += "/doc/hpr";
	*fHelpDir =
		 env.GetValue("HistPresent.HelpDir",defdir.Data());
	if (!gSystem->AccessPathName(fHelpDir->Data()))
		fHelpBrowser = new TMrbHelpBrowser(fHelpDir->Data());
	else
		fHelpBrowser = NULL;
 //  SetColorPalette();
	icondir += "/icons";
//   gEnv->SetValue("Gui.IconPath", icondir.Data());
// look if we are in a NX session
	TString display(gSystem->Getenv("DISPLAY"));
	if (display.BeginsWith("unix")) {
		cout << "Running in NX session, force WindowWidth >= 300" << endl;
		if (WindowSizeDialog::fMainWidth < 300)     WindowSizeDialog::fMainWidth     = 300;
		if (WindowSizeDialog::fWinwidx_hlist < 300) WindowSizeDialog::fWinwidx_hlist = 300;
	}
	fCmdLine = new TList();
	if (gDirectory) {
		HistPresent *hold = (HistPresent*)gDirectory->GetList()->FindObject(GetName());
		if (hold) {
//         Warning("Build","Replacing existing : %s",GetName());
			delete hold;
		}
	}
	gROOT->GetList()->Add(this);
	gROOT->GetListOfCleanups()->Add(this);
	gHprClosing = 0;
};

//________________________________________________________________
// destructor

HistPresent::~HistPresent()
{
	if (gDebug > 0)
		cout << endl << setblue << "enter HistPresents dtor"<< setblack <<endl;
	if (fComSocket) {
		fComSocket->Send("M_client exit");
	}
	gHprClosing = 1;
//   CloseAllCanvases();
	gDirectory->GetList()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
	if (cHPr ) {
		delete cHPr;
		SaveOptions();
		cHPr = NULL;
	}
};
//________________________________________________________________
void HistPresent::RecursiveRemove(TObject * obj)
{
	if ( gHprClosing ) return;
	if (gHprDebug > 1 && obj) {
		cout << "HistPresent::RecursiveRemove for: " << obj << " " << obj->GetName() <<endl ;
	}
	if (obj == cHPr) {
		if ( gHprDebug > 0)
			cout << "HistPresent::RecursiveRemove for myself " << gHpr << endl << flush;
		gHprClosing = 1;
		TList toberemoved;
		TIter next(gROOT->GetList());
		TObject *o;
		while ( (o = next()) ) {
			if ( o->InheritsFrom("FitHist") )
				toberemoved.Add(o);
			if ( o->InheritsFrom("HTCanvas") )
				toberemoved.Add(o);
		}
		TIter next1(&toberemoved);
		while ( (o = next1()) ) {
			gROOT->GetListOfCleanups()->Remove(o);
		}
	}
	fCanvasClosing = kFALSE;
//      fCloseWindowsButton->SetMethod("gHpr->CloseAllCanvases();");
//      cout << "------> HistPresent: all canvases closed" << endl;
//   }
	if ( obj == fFileList ) {
		fFileList = NULL;
	}

	fAllCuts->Remove(obj);
	fAllWindows->Remove(obj);
	fAllFunctions->Remove(obj);
	fHistLists->Remove(obj);
	fSelectHist->Remove(obj);
	fSelectCanvas->Remove(obj);
	fHistListList->Remove(obj);
	fSelectCut->Remove(obj);
	fSelectWindow->Remove(obj);
	fSelectLeaf->Remove(obj);
	if (obj == cHPr) {
		SaveOptions();
		cHPr = NULL;
		if (gHprTerminated == 0) {
			gApplication->Terminate(0);
			if (gHprDebug > 0)
			cout << "HistPresent::RecursiveRemove: gApplication->Terminate(0)" <<endl;
		}
	}
	if (gHprDebug > 2 && obj) {
		cout << "Exit HistPresent::RecursiveRemove for: " << obj << " " << obj->GetName() <<endl ;
	}
}
//________________________________________________________________
void HistPresent::HandleTerminate(Int_t status)
{
	gHprTerminated = 1;
	
	if ( gHprDebug > 0)
		cout << setblue << "HandleTerminate called, status " << status << setblack << endl;
	if (cHPr != nullptr && gHprClosing == 0) {
		delete cHPr;
		cHPr = NULL;
	}
}
//________________________________________________________________
void HistPresent::ShowMain()
{
	nHists=0;
	Int_t mainheight = (Int_t)(WindowSizeDialog::fMainWidth * 1.6);
	gStyle->SetCanvasPreferGL(kFALSE);
	cHPr = new HTCanvas("cHPr", "HistPresent",5,5, WindowSizeDialog::fMainWidth, mainheight, this, 0);
// this window should be removed if user types .q
//	gSystem->RemoveOnExit(cHPr);
	TQObject::Connect(gApplication, "Terminate(Int_t)", "HistPresent", this, "HandleTerminate(Int_t)");
	cHPr->cd();
	fRootCanvas = (TRootCanvas*)cHPr->GetCanvasImp();

	Float_t  dy=0.068, y=1. - 6.5*dy - 0.001, x0=0.01, x1=0.97;
	TButton *b;
	Int_t hint_delay=1000;
	b = CButton("CloseHistLists",    "Close HistLists",this,1,1,dy,0.5);
	fCloseWindowsButton
	  = CButton("CloseAllCanvases",  "Close Windows",this,2,1,dy,0.5);
	b = CButton("ListSelect",        "List Hists,Cuts",this,1,2,dy,0.5);
	b = CButton("ClearSelect",       "Clear Select",this,2,2,dy,0.5);
//   b = CButton("Editrootrc",        "Edit .hprrc",this,1,3,dy,0.5);
	b = CButton("ListMacros",        "List Macros",this,1,3,dy,0.5);
	b->SetToolTipText("Edit resource file .hprrc in CWD",hint_delay);
	b = CButton("EditAttrFile",      "Edit AttrMacro",this,2,3,dy,0.5);
	b->SetToolTipText("Edit a macro used to customize drawing options",hint_delay);
	b = CButton("RebinHist",         "Rebin",this,1,4,dy,0.25);
	b->SetToolTipText("Rebin a selected histogram",hint_delay);
	fRebin2 = SButton("SetRebinValue(2);", "2",this,3,4,dy,1,0.125);
	fRebin2->SetToolTipText("Set rebin value to 2",hint_delay);
	fRebin4 = SButton("SetRebinValue(32);", "32",this,4,4,dy,0,0.125);
	fRebin4->SetToolTipText("Set rebin value to 4",hint_delay);
//   SButton("SetRebinValue(8);", "8",this,5,4,dy,0,0.1);
	fRebinOth = SButton("SetRebinValue(0);", "oth",this,3,4,dy,0,0.25);
	fRebinOth->SetToolTipText("Ask for rebin value",hint_delay);
	fRebinSumAvg = CButton("SetRebinMethod",    "Sum",this,4,4,dy,0.25);
	fRebinSumAvg->SetToolTipText("Use either sum or avarage when rebinning",hint_delay);
	b = CButton("OperateHist(1);",   "+",this,1,5,dy,0.1);
	b->SetToolTipText("Add 2 or more histograms",hint_delay);
	b = CButton("OperateHist(2);",   "-",this,2,5,dy,0.1);
	b->SetToolTipText("Subtract histogram from another",hint_delay);
	b = CButton("OperateHist(3);",   "*",this,3,5,dy,0.1);
	b->SetToolTipText("Multiply histograms, if only one is selected scale it by factor",hint_delay);
	b = CButton("OperateHist(4);",   "/",this,4,5,dy,0.1);
	b->SetToolTipText("Divide histograms ",hint_delay);
//   fValButton = 0;
	fValButton = CButton("SetOperateVal",     "1.",this,3,5,dy,0.25);
	fValButton->SetToolTipText("Factor used in operations (e.g. h1 + fac*h2)",hint_delay);

	TString cmd("gHpr->ShowFiles(\"A\")");
	TString tit("Show FileList (alphab)");
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Show files ending .root, or .histlist in CWD in alphabetical order",hint_delay);
	y-=dy;
/*
	cmd="gHpr->ShowFiles(\"R\")";
	tit="Show FileList (reverse)";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Show files ending .root, or .histlist in CWD in reverse alphabetical order",hint_delay);
	y-=dy;
*/
	cmd="gHpr->ShowFiles(\"D\")";
	tit="Show FileList (bydate)";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Show files ending .root or .histlist in CWD ordered by date",hint_delay);
	y-=dy;

	cmd="gHpr->ShowContents(\"Memory\",\"\")";
	tit="List Objects in Memory";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Show objects (hists, windows, functions currently in memory",hint_delay);
	y-=dy;

	cmd="gHpr->HistsFromProof()";
	tit="Histos from Proof";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Get histograms from proof output list",hint_delay);
	y-=dy;

	cmd="gHpr->ShowContents(\"Socket\",\"\")";
	tit="Hists from M_analyze";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Get hists from running M_analyze via TcpIp",hint_delay);
	y-=dy;

	cmd="gHpr->GetFileSelMask()";
	tit="File Selection mask";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Define a regular expression used file selection criterion",hint_delay);
	y-=dy;

	cmd="gHpr->GetHistSelMask()";
	tit="Histo/Leaf/Canvas mask";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Define a regular expression for selection of a subset of histograms",hint_delay);
	y-=dy;
	cmd = "gHpr->StackSelectedHists()";
	tit = "Stack selected hists";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Stackselected histograms",hint_delay);
	y-=dy;
/*	cmd = "gHpr->StackSelectedHistsScaled()";
	tit = "Stack sel hists scaled";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Stack selected histograms with a scale applied to each",hint_delay);
	y-=dy;*/
	
	cmd = "gHpr->ShowSelectedHists()";
	tit = "Show selected hists";
	b = CommandButton(cmd,tit,x0,y,x1,y+dy);
	b->SetToolTipText(
	"Show selected histograms in one single canvas",hint_delay);

	cHPr->SetEditable(kFALSE);
	cHPr->Update();
//   CreateDefaultsDir(fRootCanvas);
	if (gSystem->AccessPathName(gSystem->WorkingDirectory(), kWritePermission))
		WarnBox("You have no write permission on current working directory\n\
you will be unable to save current settings etc.");
}
//________________________________________________________________________________________

void HistPresent::Editrootrc()
{
	TString EditCmd = ".hprrc";
	Bool_t fok = kFALSE;
	if (!gSystem->AccessPathName(EditCmd.Data())) {
		cout << "Using .hprrc in cwd" << endl;
		fok = kTRUE;
	}
/*
	else {
		EditCmd.Prepend("/");
		EditCmd.Prepend(gSystem->HomeDirectory());
		if (!gSystem->AccessPathName(EditCmd.Data())) {
			cout << "Using " << EditCmd.Data() << endl;
			fok = kTRUE;
		}
	}
*/
	if (fok) {
		EditCmd.Prepend(fEditor.Data());
		gSystem->Exec(EditCmd.Data()); EditCmd += "&";
	}
	else   cout << "Cant find .hprrc" << endl;

	cout << setred << "Warning: values might be overwritten by current settings"
		  << setblack << endl;
}
//________________________________________________________________________________________

void HistPresent::EditAttrFile()
{
	if (gSystem->AccessPathName(attrname)) {
		ofstream attrfile(attrname, ios::out);
		if (!attrfile) {
			cout << "Cant open new " << attrname << endl;
			return;
		}
		attrfile << AttrTemplate;
		attrfile.close();
	}
	TString EditCmd = fEditor.Data();
	EditCmd += attrname; EditCmd += "&";
	gSystem->Exec(EditCmd.Data());
}
//________________________________________________________________________________________

void HistPresent::ShowFiles(const char *how, const char */*bp*/)
{
	if (fFileList) {
		if (gROOT->GetListOfCanvases()->FindObject("FileList")) delete fFileList;
		activeFile=NULL;
		fFileList=NULL;
	}
	TString fname;
	void* dirp=gSystem->OpenDirectory(gHprWorkDir);
	Int_t nfiles = 0;
	const Char_t * na;
	if ( fUseFileSelFromRun && fFileSelFromRun >= 0 && fFileSelToRun > 0 ) {
		cout << setblue << "Select files by run number, from: "
		<< fFileSelFromRun << " to: " << fFileSelToRun << setblack<< endl;
	}
	if ( fUseFileSelMask && fFileSelMask.Length() > 0 ) {
		cout << setblue << "Using file selection mask: \""
		<< fFileSelMask<< "\""<< setblack<< endl;
	}
	while ( (na=gSystem->GetDirEntry(dirp)) ) {
		fname = na;
		if (!fname.EndsWith(".root") && !fname.EndsWith(".histlist"))
			continue;
		if ( fUseFileSelMask && fFileSelMask.Length() > 0 
			&& !Hpr::IsSelected(fname, &fFileSelMask, fFileUseRegexp) )
				continue;
		if ( fUseFileSelFromRun && fFileSelFromRun >= 0 && fFileSelToRun > 0
				&& !Hpr::IsSelected(fname, fFileSelFromRun, fFileSelToRun) )
				continue;
		Long_t  id, flags, modtime;
		Long64_t size;
		gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
		TString nam=fname;
		TString cmd = "gHpr->Show";
		if (fname.EndsWith(".histlist")) {
			//only lists with histname + filenames are shown here
			if ( contains_filenames(fname) ){
				cmd = cmd + "List(\"\",\"" + gHprWorkDir.Data() +"/" + fname + "\")";
			} else {
				continue;
			}
		} else {
			 cmd = cmd + "Contents(\"" + gHprWorkDir.Data() +"/"+ fname + "\", \"\" )";
		}

		 TString tit;
		 TString sel;

		 if      (!strcmp(how,"A"))
			 fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel));
		 else if (!strcmp(how,"R"))
			 fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel, kTRUE));
		 else
			 fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel,modtime));
		 nfiles++;
	}
	fCmdLine->Sort();
	if ( GeneralAttDialog::fMaxFileListEntries <= 0 )
		GeneralAttDialog::fMaxFileListEntries = 100;
	if ( nfiles > GeneralAttDialog::fMaxFileListEntries ) {
		cout << endl<< setred << "More than " << GeneralAttDialog::fMaxFileListEntries 
		<< " selected files in directory" << endl;
		cout << "Please reduce number of files in directory" << endl <<
		"or apply a \"File selection mask\"" << setblack << endl;
		cout<< setblue << "Another " << nfiles - GeneralAttDialog::fMaxFileListEntries << 
		" entries are not shown" << endl;
		cout << setblack;
		nfiles = GeneralAttDialog::fMaxFileListEntries;
//		skipfirst = GeneralAttDialog::fSkipFirst;
	}
	if ( GeneralAttDialog::fSkipFirst > 0 )
		cout << setblue << "First " << GeneralAttDialog::fSkipFirst
		<< " entries  skipped" << setblack << endl;  

	if (fCmdLine->GetSize() <= 0) {
		Int_t buttons= kMBYes | kMBNo, retval=0;
		EMsgBoxIcon icontype = kMBIconQuestion;
		new TGMsgBox(gClient->GetRoot(), fRootCanvas,
		 "No files found?",
		 "No ROOT files in CWD found \n\
Should we create a sample file",
		 icontype, buttons, &retval);
		if(retval == kMBYes){
			Int_t ierr;
			gSystem->Exec("cp $MARABOU/macros/hsimple_hpr.C .");
			if (gSystem->AccessPathName("hsimple_hpr.C")) {
				cout << setred << "Cant copy file: $MARABOU/macros/hsimple_hpr.C"
					  << setblack << endl;
			} else {
				cout <<  "Copy and execute file: $MARABOU/macros/hsimple_hpr.C as example" << endl;
				gROOT->ProcessLine(".x $MARABOU/macros/hsimple_hpr.C", &ierr);
			}
			gDirectory = gROOT;
		}
//      WarnBox("No files found, check File Selection Mask");
	} else {
		Int_t yoff = (Int_t)(WindowSizeDialog::fMainWidth * 1.6 + 30);
		fFileList = CommandPanel(gSystem->BaseName(gSystem->WorkingDirectory()),
										 fCmdLine, 5, yoff, this,
										 WindowSizeDialog::fWinwidx_hlist,
										 nfiles, GeneralAttDialog::fSkipFirst);
		fFileList->SetName("FileList");
	}
	fCmdLine->Delete();
};
//________________________________________________________________________________________

void HistPresent::ListMacros(const char */*bp*/)
{
//   if (fControlBar) {
//      if (fControlBar->TestBit(TObject::kNotDeleted) &&
//          !fControlBar->TestBit(0xf0000000)) delete fControlBar;
//      fControlBar=0;
//   }
	Bool_t first = kTRUE;
	const char *fname;
	void* dirp=gSystem->OpenDirectory(gHprWorkDir);
	ifstream wstream;
	TString wline;
	TRegexp dotC = "\\.C$";
	while ( (fname=gSystem->GetDirEntry(dirp)) ) {
		TString sname(fname);
		if (sname.Index(dotC)>0) {
//         cout << fname << endl;
			wstream.open(fname, ios::in);
			if (!wstream.good()) {
				cerr	<< "ListMacros: "<< gSystem->GetError() << " - " << fname
					<< endl;
				wstream.close();
				continue;
			}
			wline.ReadLine(wstream, kFALSE);
			wline.Strip();
			Int_t len =wline.Length();
			Int_t n = wline.Index("Hint:");
			if (len > 0 && n >= 0) {
				n+=5;
//            if (!fControlBar)
				if (first) {
					fControlBar = new TControlBar("vertical", "Macros", 5,380);
					first = kFALSE;
				}
				TString cmd =".x "; cmd += fname;
				TString hint = wline(n,len);
				fControlBar->AddButton(fname, cmd.Data(), hint.Data());
			}
			wstream.close();
		}
	}
	if (fControlBar) {
		fControlBar->Show();
		gROOT->SaveContext();
	} else
		cout << "A macro must contain a line like: //Hint: a hint what is does " << endl;
};
//________________________________________________________________________________________

void HistPresent::TurnButtonGreen(TVirtualPad ** pad)
{
	if (*pad && (*pad)->TestBit(TObject::kNotDeleted)
			  && !((*pad)->TestBit(0xf0000000)) )
	{
		if (strcmp((*pad)->ClassName(), "TButton")) {
			cout << "No longer a TButton, but " << (*pad)->ClassName() << endl;
		} else {
			(*pad)->SetFillColor(0);
			(*pad)->Modified(kTRUE);
			(*pad)->Update();
		}
	}
	gPad->SetFillColor(3);
//   cout << " Green" << setbase(16) << gPad << endl;
	*pad=gPad;
}
//________________________________________________________________________________________

void HistPresent::HistsFromProof(const char* bp)
{
	if (      gROOT->GetListOfProofs()->GetSize() == 0 ) {
		cout << "No proof session found" << endl;
		return;
	} else if ( gROOT->GetListOfProofs()->GetSize()  > 1 ) {
		cout << "Warning: more than one proof session found" << endl;
	}
//	TProof * proof = (TProof*)gROOT->GetListOfProofs()->At(0);
	TList * hlist = gProof->GetOutputList();
	if ( hlist->GetSize() == 0 ) {
		cout << "No output objects found" << endl;
		return;
	}
	TIter next(hlist);
	TObject *obj = NULL;
	Int_t nh = 0;
	while ( obj = next() ) {
		if ( obj->InheritsFrom("TH1") ) {
			((TH1*)obj)->SetDirectory(gROOT);
			nh++;
		}
	}
	if ( nh == 0 ) {
		cout << "No histograms found" << endl;
		return;
	}
	ShowContents("Memory","",bp);
}
//________________________________________________________________________________________

void HistPresent::ShowContents(const char *fname, const char * dir, const char* bp)
{
	static Int_t ycanvas=5;
	TMrbStatistics * st = NULL;
	TList lofF;
	TList lofW1;
	TList lofW2;
	TList lofT;
	TList lofC;
	TList lofUc;
	TList lofG;
	Int_t nstat=0;
	Bool_t ok = kFALSE;
	TString cmd;
	TString title;
	TString hint;
	TString sel;
	Int_t maxkey = 0;
	if (GeneralAttDialog::fMaxListEntries > 1000) {
		cout << "Warning: Max number of entries in list\n\
 of histograms set to: 1000"
			  << endl;
		GeneralAttDialog::fMaxListEntries = 1000;
	}
//   TurnButtonGreen(&activeFile);
	void* dirp=gSystem->OpenDirectory(gHprWorkDir);
	const char * fn;
	TString suffix(fHlistSuffix);
	suffix += "$";
	TRegexp rsuf(suffix);
	while ( (fn=gSystem->GetDirEntry(dirp)) ) {
		hint = fn;
		if (hint.Index(rsuf) > 0) {
//			cout << "Histlist: " << fn << " fname " << fname << endl;
			if (contains_filenames(fn) > 0) continue;
			if (strstr(fname,".root")) {
				if (FindHistsInFile(fname, fn) <= 0) {
					cout << "No hist of: " << fn << " found in: " << fname << endl;
					continue;
				}
			}
			hint(rsuf) = "";
			cmd = "gHpr->ShowList(\"";
			cmd = cmd + fname + "\",\"" + hint + "\")";
			title = "ShowList "; title += hint;
			sel.Resize(0);
			fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
		}
	}
	if (fCmdLine->GetSize() > 0) fCmdLine->Sort();

	if (fRootFile) fRootFile->Close();
	TButton * b = NULL;
	if (bp) {
		b = (TButton *)strtoul(bp, 0, 16);
		if ( b ) {
			b->SetBit(kSelected);
			b->SetFillColor(3);
			b->Modified(kTRUE);
			b->Update();
		}
	}
	
	Int_t anything_to_delete = 0;
	if (strstr(fname,".root")) {
		TFile * rfile = NULL;
		rfile = TFile::Open(fname);
		if ( rfile == NULL ) {
			cout << setred << "Cant find keys, something wrong with file: " << fname << endl
			<< " For more info uncheck \"Suppress warning messages\" from Hpr-Options menu" 
			<< setblack << endl;
			return;
		}
		if (GeneralAttDialog::fShowListsOnly > 0) {
			st = NULL;
			nstat = 0;
			cout << "Skip hists in file, show hist lists only" << endl;
		} else {
			if (dir && strlen(dir) > 0) rfile->cd(dir);
//         gDirectory->ls();
			st = (TMrbStatistics*)gDirectory->Get("TMrbStatistics");
			if (!st) {
				st=new TMrbStatistics(fname);
				nstat = st->Fill(gDirectory);
			} else nstat = st->GetListOfEntries()->GetSize();
			// look for directory entries
			TIter next(gDirectory->GetListOfKeys());
			TKey* key;
			while( (key = (TKey*)next()) ){
				if(!strncmp(key->GetClassName(),"TDirectory",10)){
					cmd = "gHpr->ShowContents(\"";
					cmd = cmd + fname + "\",\"";
					if (strlen(dir) > 0) cmd = cmd + "/" + dir;
					cmd = cmd + key->GetName()+ "/" + "\")";
					sel = "";
					title = "Dir: ";
					title += key->GetName();
					hint = title;
					fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
				}
			}
		}
		maxkey = TMath:: Max(GetObjects(lofT, gDirectory, "TTree"),        maxkey);
		maxkey = TMath:: Max(GetObjects(lofT, gDirectory, "TNtuple"),      maxkey);
		maxkey = TMath:: Max(GetObjects(lofF, gDirectory, "TF1"),          maxkey);
		maxkey = TMath:: Max(GetObjects(lofF, gDirectory, "TF2"),          maxkey);
		maxkey = TMath:: Max(GetObjects(lofC, gDirectory, "TCanvas"),      maxkey);
		maxkey = TMath:: Max(GetObjects(lofC, gDirectory, "GrCanvas"),      maxkey);
		maxkey = TMath:: Max(GetObjects(lofC, gDirectory, "HTCanvas"),      maxkey);
		maxkey = TMath:: Max(GetObjects(lofG, gDirectory, "TGraph"),       maxkey);
		maxkey = TMath:: Max(GetObjects(lofG, gDirectory, "TGraphErrors"), maxkey);
		maxkey = TMath:: Max(GetObjects(lofG, gDirectory, "TGraph2D"),       maxkey);
		maxkey = TMath:: Max(GetObjects(lofUc, gDirectory, "FhContour"),   maxkey);
		maxkey = TMath:: Max(GetObjects(lofW1, gDirectory, "TMrbWindowF"), maxkey);
		maxkey = TMath:: Max(GetObjects(lofW1, gDirectory, "TMrbWindowI"), maxkey);
		maxkey = TMath:: Max(GetObjects(lofW2, gDirectory, "TMrbWindow2D"),maxkey);
		rfile->Close();
	} else if (strstr(fname,"Socket")) {
		ClearSelect();
		if (!fComSocket) {
			if (!fConnectedOnce) {
				*fHostToConnect = GetString("Host to connect", fHostToConnect->Data()
								,&ok,fRootCanvas);
				if (!ok) return;
				fSocketToConnect = GetInteger("Socket to connect",fSocketToConnect
								,&ok,fRootCanvas);
				if (!ok) return;
//            fConnectedOnce = kTRUE;
			}
			fComSocket = new TSocket(*fHostToConnect, fSocketToConnect);
			if (!fComSocket->IsValid()) {
				fComSocket->Close();
				fComSocket = 0;
				cout << setred <<
				"Cant connect to remote M_analyze" << setblack << endl;
				return;
			} else {
//           Wait till we get the start message
				TMessage * mess;
				fComSocket->Recv(mess);
				if ( mess->What() == kMESS_STRING ) {
					char str[30];
					mess->ReadString(str, 25);
					TString  smess = str; smess = smess.Strip(TString::kBoth);
					if (smess(0,6) != "accept") {
						cerr << "Connection not accepted, message: "
						<< setred << smess << setblack << endl;
						fComSocket->Close();
						fComSocket = NULL;
						if (mess) delete mess;
						return;
					}
				}
				fConnectedOnce = kTRUE;
			}
			if ( fComSocket ) {
				if (bp) {
					TButton * b1;
					b1 = (TButton *)strtoul(bp, 0, 16);
					//      cout << "bp " << b << endl;
					if (b1) {
						fHfromM_aButton = b1;
						SetSelected(fHfromM_aButton, kTRUE);
					} else {
						fHfromM_aButton = NULL;
					}
				}
			}
		}
		if (GeneralAttDialog::fShowListsOnly <= 0) {
			st = getstat(fComSocket);
			if (!st) {
				if ( !fComSocket->IsValid() ) {
					cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
					fComSocket->Close("force");
					fComSocket = NULL;
					if ( fHfromM_aButton )
						SetSelected(fHfromM_aButton, kFALSE);
				} else {
					cout << setred << "Cant get stat, connection seems alive" << setblack << endl;
				}
				return;
			} else {
				nstat = st->GetListOfEntries()->GetSize();
			}
		} else {
			st = NULL;
			nstat = 0;
			 cout << " Skip getstat(fComSocket)" << endl;
		}
	} else if (strstr(fname,"Memory")) {
		TSeqCollection *loc = gROOT->GetListOfCanvases();
		TCanvas *cm;
		if ( cm = (TCanvas*)loc->FindObject("ContentList")) {
			if ( !strncmp(cm->GetTitle(),"Memory",7) )
				delete cm;
		}
		st=new TMrbStatistics();
		nstat = st->Fill();
	} else {
		 WarnBox("Unknown file");
		 return;
	}
	// histograms
	Int_t n_enter = 0;
	Int_t not_shown = 0;
	if (nstat > 0) {
		if ( fUseHistSelMask && fHistSelMask.Length() > 0) {
			cout << setblue << "Using selectionmask: " << fHistSelMask 
			<< setblack << endl;
		}
		TMrbStatEntry * stent;
		TIter nextentry(st->GetListOfEntries());
		while ( (stent = (TMrbStatEntry*)nextentry()) ) {
			n_enter++;
			if ( fUseHistSelMask &&  !Hpr::IsSelected( stent->GetName(), &fHistSelMask, fHistUseRegexp ) )
				continue;
			TString shn(stent->GetName());
			if (shn.Contains(" ")) {
//			   cout << setred << "Histogram name: \"" << shn
//				<< "\" contains spaces!!!" << endl;
				static Int_t first_time = 1;
				if (first_time) {
					cout << setred << "Histogram name: \"" << shn
					<< "\" contains spaces!!!" << endl;
					first_time = 0;
					cout << "We strongly advise against using white space in names." << endl;
//					cout << "You will not be able to use these histograms in selections" << endl;
					cout << setblack << endl;
				}
			}
			cmd = fname;
			cmd = cmd + "\",\"";
			cmd += dir;
			cmd = cmd + "\",\"" + stent->GetName();
			if (stent->GetCycle() > 0) {
				cmd += ";";
				cmd += stent->GetCycle();
			}
			cmd += "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowHist(\"");
			sel.Prepend("gHpr->SelectHist(\"");

//         cout << cmd << endl;

			title = stent->GetName();
			if (stent->GetCycle() > 1) {
				title += ";";
				title += stent->GetCycle();
			}
			hint = title;
			if     ( stent->GetDimension() == 1  )title.Prepend("1d ");
			else if (stent->GetDimension() == 2 ) title.Prepend("2d ");
			else                                  title.Prepend("3d ");
			title +=  " " ;
			title +=  (Int_t)stent->GetEntries();

			hint = stent->GetName();
			if (stent->GetDimension() == 1)      hint += " 1d hist Ent: ";
			else if (stent->GetDimension() == 2 )hint += " 2d hist Ent: ";
			else                                 hint += " 3d hist Ent: ";
			hint += " ";
			hint += (Int_t)stent->GetEntries();
			hint += " Title: ";
			hint += stent->GetTitle();
			TString tmp = fname;
			tmp = tmp + " " + stent->GetName();
//         TObjString tobjs(tmp.Data());
			if (fSelectHist->Contains(tmp.Data())) {
				cout << "Already selected: " << tmp.Data() << endl;
				sel += " YES";
			}
			if (fCmdLine->GetSize() < GeneralAttDialog::fMaxListEntries) {
//				cout << " Add: " << n_enter<< " " << stent->GetName()<<endl;
				fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
				anything_to_delete++;
//            n_enter++;
			} else {
				if (not_shown <= 0){
					cout << setred << "Too many entries in list: "
					<< fCmdLine->GetSize()<< endl;
					cout << "this might crash X, please use selection mask"<< endl;
					cout << "to reduce number of entries below: "
					<<  GeneralAttDialog::fMaxListEntries  << endl;
					cout << "On your own risk you may increase value beyond: "
					<< GeneralAttDialog::fMaxListEntries << endl;
					cout << "Warning: not all hists will be shown" << setblack << endl;
				}
				not_shown++;
//            cout << "Not shown: " << stent->GetName() << endl;
			}
		}
	}
	if ( fUseHistSelMask ) {
		cout << "Total number of hists: " << n_enter 
		<< " Selected: " <<  fCmdLine->GetSize() << endl;
		if ( not_shown > 0 ) 
			cout << "Another: " << not_shown << " hists are not shown" << endl;
	}
	//  windows
	if (lofW1.GetSize() > 0) {
		TIter next(&lofW1);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			TString name(title);
			cmd = fname;
			cmd = cmd + "\",\"" + name.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->PrintWindow(\"");
			sel.Prepend("gHpr->LoadWindow(\"");
			title.Prepend("w1 ");
			hint =  title;
			hint+=" 1-dim window";
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
			anything_to_delete++;
		}
	}
	if (lofW2.GetSize() > 0) {
		TIter next(&lofW2);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			TString name(title);
			cmd = fname;
			cmd = cmd + "\",\"" + name.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->PrintWindow(\"");
			sel.Prepend("gHpr->LoadWindow(\"");
			title.Prepend("w2 ");
			hint =  title;
			hint+=" 2-dim window";
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
		}
		title = "2D Cuts to ASCII";
		hint = "Write " + title;
		cmd = "gHpr->CutsToASCII(\"";
		cmd = cmd + fname + "\")";
		sel = "";
		fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
		anything_to_delete++;
	}
//  functions
	if (lofF.GetSize() > 0) {
		TIter next(&lofF);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			cmd = fname;
			cmd = cmd + "\",\"" + dir + "\",\"" + title.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowFunction(\"");
			sel.Prepend("gHpr->LoadFunction(\"");
//         sel.Resize(0);
			title.Prepend("f ");
			hint =  title;
			hint+=" function";
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
			anything_to_delete++;
		}
	}
//  canvases
	if (lofC.GetSize() > 0) {
		TIter next(&lofC);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			if ( fUseCanvasSelMask && !Hpr::IsSelected( title.Data(), &fCanvasSelMask, fCanvasUseRegexp ) )
				continue;
			cmd = fname;
			cmd = cmd + "\",\"" + dir + "\",\"" + title.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowCanvas(\"");
			sel.Prepend("gHpr->SelectCanvas(\"");
			title.Prepend("c ");
			hint =  title;
			hint+=" canvas";
			if (fCmdLine->GetSize() < GeneralAttDialog::fMaxListEntries) {
				fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
				anything_to_delete++;
			} else {
				if (not_shown <= 0){
					cout << setred << endl<< 
					"Too many entries in list this might crash X." << endl;
					cout << "Please use selection mask to reduce number\n\
								of entries below: " 
					<<  GeneralAttDialog::fMaxListEntries  << endl;
					cout << "On your own risk you may increase value beyond: "
					<< GeneralAttDialog::fMaxListEntries << endl;
					cout << "WARNING: not all canvases will be shown"
					<< setblack << endl;
				}
				not_shown++;
				//            cout << "Not shown: " << stent->GetName() << endl;
			}
		}
	}
	if ( not_shown > 0 )
		cout << "Another: " << not_shown << " canvases are not shown" << endl;
	//  user contours
	if (lofUc.GetSize() > 0) {
		TIter next(&lofUc);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			cmd = fname;
			cmd = cmd + "\",\"" + dir + "\",\"" + title.Data() + "\")";
			cmd = cmd + "\",\"" + title.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowContour(\"");
			sel.Prepend("gHpr->SelectContour(\"");
			title.Prepend("U_C ");
			hint =  title;
			hint+=" contour";
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
			anything_to_delete++;
		}
	}
	if (lofG.GetSize() > 0) {
		TIter next(&lofG);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			cmd = fname;
			cmd = cmd + "\",\"" + dir + "\",\"" + title.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowGraph(\"");
			sel.Prepend("gHpr->SelectGraph(\"");
			title.Prepend("Graph ");
			hint =  title;
			hint+=" graph";
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
			anything_to_delete++;
		}
	}
//  trees
	if (lofT.GetSize() > 0) {
		cmd = "gHpr->SetShowTreeOptionsCint()";
		title = "Tree display options";
		hint = "Popup Tree display options menu";
		sel.Resize(0);
		fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
		TIter next(&lofT);
		TObjString * objs;
		while ( (objs = (TObjString*)next())) {
			title = objs->String();
			cmd = fname;
			cmd = cmd + "\",\"" + dir + "\",\"" + title.Data() + "\")";
			sel = cmd;
			cmd.Prepend("gHpr->ShowTree(\"");
			sel.Resize(0);;
			hint =  title;
			title.Prepend("Tree: ");
			fCmdLine->Add(new CmdListEntry(cmd, title, hint, sel));
		}
	}
	
	if (anything_to_delete > 0) {
		if (maxkey > 1) {
			cmd = "gHpr->PurgeEntries(\"";
			cmd = cmd + fname +  "\")";
			title = "Purge Entries";
			hint = "Purge: delete entries, keep last cycl";
			sel.Resize(0);
			fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
		}
//   	if (!strstr(fname,"Memory")) {
			cmd = "gHpr->DeleteSelectedEntries(\"";
			cmd = cmd + fname +  "\")";
			title = "Delete Sel Entries";
			hint = "Delete selected entries";
			sel.Resize(0);
			fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
//		}
	}
	if (fCmdLine->GetSize() > 0) {
		cmd = "gHpr->ShowStatOfAll(\"";
		cmd = cmd + fname + "\",\"" + dir + "\")";
		title = "Show Stats of all";
		hint = "Show statistics of all histograms and save to file";
		sel.Resize(0);
		fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
	}
	if (strstr(fname,"Socket")) {
		cmd = "gHpr->SaveFromSocket(\"";
		cmd = cmd + fname + "\")";
		hint = "Save all hists to a local ROOT file ";
		title = "Save to local ROOT file";
		fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
	}
//
	if (fCmdLine->GetSize() > 0) {
		if (fCmdLine->GetSize() > 3) {
			cmd = "gHpr->ComposeList()";
			title = "Compose list";
			hint = "Select histograms to be added to list first";
			sel.Resize(0);
			fCmdLine->AddFirst(new CmdListEntry(cmd, title, hint, sel));
		}
//      CheckList(fHistLists, "HTCanvas");
		TObject *obj =  fHistLists->FindObject(fname);
		if (obj) {
//         cout << "Delete canvas: " << fname << endl;
			fHistLists->Remove(obj);
			delete obj;
		}
			ycanvas = 5 + 50 * fHistLists->GetSize();
			fname = gSystem->BaseName(fname);
			TString cmd_title(fname);
			if (dir && strlen(dir) > 0) cmd_title = cmd_title + "_" + dir;
			HTCanvas *ccont = CommandPanel(cmd_title, fCmdLine,
									WindowSizeDialog::fMainWidth + 10, ycanvas,
									this, WindowSizeDialog::fWinwidx_hlist,
									fCmdLine->GetSize());
			ccont->SetName("ContentList");
//         cout << "HistPresent: CommandPanel: " <<ccont->GetName() << " " << ccont->GetTitle() << endl;

			if (fHistLists)fHistLists->Add(ccont);
	} else {
		cout << setred << "No objects found" << setblack << endl;
	}
	fCmdLine->Delete();
	if (st) delete st;
	gDirectory=gROOT;
}
//________________________________________________________________________________________

void HistPresent::PurgeEntries(const char * fname, const char * /*bp*/)
{
	TString question(fname);
	question.Prepend("Purge file: ");
	if (QuestionBox(question.Data(), fRootCanvas) == kMBYes) {
		TFile * f = new TFile(fname, "update");
		f->Purge();
		f->Close();
	}
}
//________________________________________________________________________________________

void HistPresent::DeleteSelectedEntries(const char * fname, const char * /*bp*/)
{
	cout << "DeleteSelectedEntries " << fname << endl;
	Int_t ndeleted = 0;
	ndeleted += DeleteOnFile(fname, fSelectCut, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fSelectHist, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fSelectCanvas, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fSelectWindow, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fSelectContour, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fAllFunctions, fRootCanvas);
	ndeleted += DeleteOnFile(fname, fSelectGraph, fRootCanvas);
	cout << ndeleted << " object";
	if (ndeleted !=1) cout << "s";
	cout << " deleted on " << fname << endl;
}
 //________________________________________________________________________________________

void HistPresent::ShowStatOfAll(const char * fname, const char * dir, const char * /*bp*/)
{
	TString sname(fname);
	TMrbStatistics * st = 0;
	TRegexp rname("\\.root");

	if (sname == "Socket") {
		if (!fComSocket) return;
		st = getstat(fComSocket);
		if (!st) {
			if ( !fComSocket->IsValid() ) {
				cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
				fComSocket->Close("force");
				fComSocket = NULL;
				if ( fHfromM_aButton )
					SetSelected(fHfromM_aButton, kFALSE);
			} else {
				cout << setred << "Cant get stat, connection seems alive" << setblack << endl;
			}
			return;
		}
	} else if (sname.Index(rname) > 0) {
		sname(rname) = "";
		TFile * rfile =0;
		rfile = new TFile(fname);

		if (strlen(dir) > 0) rfile->cd(dir);
		st = (TMrbStatistics*)gDirectory->Get("TMrbStatistics");
		if (!st) {
			TString snames(fname);
			if (strlen(dir) > 0) snames = snames + " Dir: " + dir;
			st=new TMrbStatistics(snames);
			st->Fill(gDirectory);
		}
		if (rfile )rfile->Close();
	} else {
		 cout << "Unkown suffix in : " << fname << endl;
		 return;
	}
	if (st) {
		sname += ".stat";
		ofstream output(sname.Data());
		st->Print(output);
		output.close();
		TString EditCmd(fEditor);
		EditCmd += sname.Data();
		EditCmd += "&";sname.Data();
		gSystem->Exec(EditCmd.Data());
		cout << "Statistic saved  to: " << sname.Data() << endl;
		delete st;
	}
	gDirectory = gROOT;
}
//________________________________________________________________________________________

void HistPresent::ComposeList(const char* /*bp*/)
{
	static const Char_t helptext[] =
	"Put selected histograms into list\n\
	With option \"Full path name\",i.e. filename + histonames\n\
	are written the list file will be displayed\n\
	With histnames only the histograms in the list\n\
	will be shown when the content of corresponding file\n\
	is displayed.\n\
	";
	Int_t nselect=fSelectHist->GetSize();
	if (nselect < 1) {
		WarnBox("No selection");
		return;
	}
	TString fname  = ((TObjString *)fSelectHist->At(0))->String();
	Int_t pp = fname.Index(",");
	if (pp < 0) {
		pp = fname.Index(" ");
		if (pp < 0 ) {
			cout << "ComposeList cant isolate file name: " << fname << endl;
			return;
		}
	}
	fname.Resize(pp);
//   Bool_t put_file = kFALSE;
	TString listname  =  fname;
	if      (fname.Contains(".root")) {
		TRegexp rname("\\.root");
		listname(rname) = fHlistSuffix;
	}
	Int_t lFullPath = 1, lHistOnly = 0;
	void * Valp[10];
	TList * row_lab = new TList();
	Int_t ind = 0;
	row_lab->Add(new TObjString("StringValue_Name of list"));
	Valp[ind++] = &listname;
	row_lab->Add(new TObjString("RadioButton_Full pathname"));
	Valp[ind++] = &lFullPath;
	row_lab->Add(new TObjString("RadioButton_Hist name only"));
	Valp[ind++] = &lHistOnly;
//   listname=GetString("Name of list:",listname.Data(), &ok, fRootCanvas);
	Int_t itemwidth = 420;
	static Int_t ok = -2;   // wait until closed
	new TGMrbValuesAndText ("ComposeList", NULL, &ok, itemwidth,
										NULL, NULL, NULL, row_lab, Valp,
										NULL, NULL, helptext);
//
//   listname += fHlistSuffix;
	cout << "ok " << ok << " " << listname << endl;
	if ( ok < 0) return;
	
	if (!gSystem->AccessPathName(listname)) {
		TString question=listname;
		question += " exists, Overwrite?";

		if (!QuestionBox(question.Data(), fRootCanvas)) return;
	}
	ofstream wstream;
	wstream.open(listname.Data(), ios::out);
	if (!wstream.good()) {
	cout	<< "Error in open file: "
		<< gSystem->GetError() << " - " << listname.Data()
		 << endl;
		return;
	}
	for(Int_t i = 0; i < nselect; i++) {
		TString hname  = ((TObjString *)fSelectHist->At(i))->String();
		pp = hname.Index(",");
		if (pp < 0) {
			pp = hname.Index(" ");
			if (pp < 0 ) {
				cout << "ComposeList cant isolate file name: " << fname << endl;
				return;
			}
		}
		fname = hname(0,pp);
		hname = hname(pp+1, hname.Length());
		cout << fname << " " << hname << endl;
//      pp = hname.Index(" ");
//      hname.Remove(0,pp+1);
		TRegexp vers(";[0-9]*");
		TRegexp comma(",");
		hname(vers) = "";
		hname(comma) = "";
		hname(comma) = "";
		if (lFullPath)
			wstream << fname.Data() << " ";
		wstream << hname.Data() << endl;
	}
	wstream.close();
	delete row_lab;
};
//_______________________________ShowList_________________________________________________________
// Show List

void HistPresent::ShowList(const char* fcur, const char* lname, const char* /*bp*/)
{
	static Int_t ycanvas=20;
	TString sl = lname;
	if (!sl.Contains(fHlistSuffix)) sl += fHlistSuffix;
	Int_t status = 1;
	Bool_t cfn = kTRUE;
	status = contains_filenames(sl.Data());
	if (status == -1) {
		cout << "Cant open: " << sl.Data() << endl;
		return;
	} else if (status == 0) cfn = kFALSE;

	TString fname = fcur;
	ifstream wstream;
	wstream.open(sl.Data(), ios::in);
	TH1 *hist=0;
	TString cmd; TString tit; TString sel;
	if (strstr(fname, "Socket"))
		cout << "Warning: Validity of entries in list are not checked" << endl;
	while ( 1) {
		TString line;
		line.ReadLine(wstream, kFALSE);
		if (wstream.eof()) {
			wstream.close();
			break;
		}
		line.Strip();
		if (line.Length() <= 0) continue;
		if (line[0] == '#') continue;
		if (cfn) {
			Int_t pp = line.Index(",");
			if (pp < 0 )
				pp = line.Index(" ");
			if (pp <= 0) continue;
			fname = line(0,pp);
			line.Remove(0,pp+1);
		}

		cmd = "gHpr->ShowHist(\"";
		cmd += fname;
		TString hname(line);
		TString dname;
		Int_t pp = line.Index(",");
		if (pp < 0) {
			pp = line.Index(" ");
		}
		if (pp > 0) {
			dname = line;
			hname.Resize(pp);
			dname.Remove(0,pp+1);
		}
		cmd = cmd + "\",\"" + dname + "\",\"";
//      cmd +=  "\",\"\",\"";
		sel = "gHpr->SelectHist";
//      sel = sel + "(\"" + fname + "\",\"\",\"";
		sel = sel + "(\"" + fname + "\",\"" + dname + "\",\"";

		cmd = cmd + hname + "\")";
		sel = sel + hname + "\")";
//		cout << cmd << endl;
//      TString empty;
		tit = line;
		if (is_a_file(fname)) {
//      cout << "fn: " << fname  << " dn: " << dname  << " hn: " << hname << endl;
			if (fRootFile) fRootFile->Close();
			fRootFile = TFile::Open(fname);
			if (dname.Length() > 0)gDirectory->cd(dname);
			hist = (TH1*)gDirectory->Get(hname);
		} else if (strstr(fname, "Socket")) {
//
		}
		if (hist) {
			if (is2dim(hist))   line.Prepend("2d ");
			else                line.Prepend("1d ");
			line +=  " " ;
			line +=  (Int_t)hist->GetEntries();
			if (is2dim(hist)) tit += " 2d hist Ent: ";
			else                tit += " 1d hist Ent: ";
			tit +=  " " ;
			tit +=  (Int_t)hist->GetEntries();
		} else {
			line.Prepend("?d ");
			line +=  " ??" ;
			tit += " ?d hist Ent: ??";
		}
		fCmdLine->Add(new CmdListEntry(cmd, line, tit, sel));
		if (is_a_file(fname)) {
			if (fRootFile) fRootFile->Close();
		} else {
			if (hist) delete hist;
		}
	}
	if (fCmdLine->GetSize() <= 0) {
		cout << "No Hists found" << endl;
	} else {
		cmd = "gHpr->ShowInOneCanvas()";
		tit = "ShowAllInOneCanvas";
		TString hint = "Show all hists in list in one Canvas";
		sel.Resize(0);
		fCmdLine->AddFirst(new CmdListEntry(cmd, tit, hint, sel));
		cmd = "gHpr->StackInOneCanvas()";
		tit = "StackAll";
		hint = "Stack all hists in list in one Canvas";
		sel.Resize(0);
		fCmdLine->AddFirst(new CmdListEntry(cmd, tit, hint, sel));

//      TString cname = fname;
//      cname = cname + "_" +lname;
		// Compose title of panel from file name
		TString ctitle= gSystem->BaseName(fname);
		Int_t ind_dot = ctitle.Index(".");
		if (ind_dot > 0)
			ctitle.Resize(ind_dot);
		// look if widget with this name already exist, if yes append name of list
		TCanvas *cc = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(ctitle);
		if (cc != NULL) {
			Int_t id = sl.Index(".");
			// chop off histlist
			if (id > 0)
				sl.Resize(id);
			ctitle += "_";
			ctitle += sl;
		}
		TCanvas *ccont = CommandPanel(ctitle.Data(), fCmdLine, WindowSizeDialog::fMainWidth + 10, ycanvas, this);
		fHistLists->Add(ccont);
		ycanvas += 50;
		if (ycanvas >= 500) ycanvas=5;
	}
	fCmdLine->Delete();
	gDirectory=gROOT;
}
//________________________________________________________________________________________
// Show List

void HistPresent::StackInOneCanvas(const char *bp)
{
	TList * hlist = BuildList(bp);
	if (hlist) {
		StackSelectedHists(hlist, hlist->GetName());
		hlist->Delete();
	}
}
//________________________________________________________________________________________
// Show List

void HistPresent::ShowInOneCanvas(const char *bp)
{
	TList * hlist = BuildList(bp);
	if (hlist) {
		ShowSelectedHists(hlist, hlist->GetName());
		hlist->Delete();
	}
}
//________________________________________________________________________________________

void HistPresent::GetFileSelMask(const char* bp)
{
	GetHistSelMask(bp);
}
//________________________________________________________________________________________

void HistPresent::GetHistSelMask(const char* /*bp*/)
{
//	Bool_t yesno = kFALSE;
//	if (GeneralAttDialog::fUseRegexp) yesno=kTRUE;
const char Help_SelectionMask_text[]=
"Selection mask may be defined in the following ways:\n\
\n\
If file names contain run numbers they may be\n\
selected by defining first and last run.\n\
Run numbers must have at least 3 digits like:\n\
009, 012 as normally used in Marabou.\n\
\n\
The following selections use patterns:\n\
\n\
If option 'Use Regexp' is active:\n\
\n\
Use regular expression as Selection Mask.\n\
 \n\
Examples: ^M:    all  starting with M\n\
          w$:    all  ending with w\n\
          [0-9]  all containing a number\n\
\n\
If option 'Use Regexp' is n o t active:\n\
Use wild card expression:\n\
\n\
Examples: *mxy*  all containing mxy\n\
          *mxy*1  all containing mxy and ending with 1\n\
\n\
If the expression contains:\n\
 & (read AND) or | (read OR) or ! (read NOT) the\n\
two expressions before and after the operator are\n\
used and the operator is applied.\n\
\n\
Examples: xxx & yyy  true if name contains xxx AND yyy\n\
          xxx | yyy  true if name contains xxx OR yy\n\
          xxx ! yyy  true if name contains xxx but not yyy\n\
          !zzz       true if name does not contain zzz\n\
";

	void * Valp[20];
	TList * row_lab = new TList();
	Int_t ind = 0;
	row_lab->Add(new TObjString("PlainIntVal_Files by Run, From:"));
	Valp[ind++] = &fFileSelFromRun;
	row_lab->Add(new TObjString("PlainIntVal-  To: "));
	Valp[ind++] = &fFileSelToRun;
	row_lab->Add(new TObjString("CheckButton-  Enable"));
	Valp[ind++] = &fUseFileSelFromRun;
	row_lab->Add(new TObjString("StringValue_      Files"));
	Valp[ind++] = &fFileSelMask;
	row_lab->Add(new TObjString("CheckButton-Use Regexp"));
	Valp[ind++] = &fFileUseRegexp;
	row_lab->Add(new TObjString("CheckButton-Enable"));
	Valp[ind++] = &fUseFileSelMask;
	row_lab->Add(new TObjString("StringValue_ Histograms"));
	Valp[ind++] = &fHistSelMask;
	row_lab->Add(new TObjString("CheckButton-Use Regexp"));
	Valp[ind++] = &fHistUseRegexp;
	row_lab->Add(new TObjString("CheckButton-Enable"));
	Valp[ind++] = &fUseHistSelMask;
	row_lab->Add(new TObjString("StringValue_Branch Leaf"));
	Valp[ind++] = &fLeafSelMask;
	row_lab->Add(new TObjString("CheckButton-Use Regexp"));
	Valp[ind++] = &fLeafUseRegexp;
	row_lab->Add(new TObjString("CheckButton-Enable"));
	Valp[ind++] = &fUseLeafSelMask;
	row_lab->Add(new TObjString("StringValue_   Canvases"));
	Valp[ind++] = &fCanvasSelMask;
	row_lab->Add(new TObjString("CheckButton-Use Regexp"));
	Valp[ind++] = &fCanvasUseRegexp;
	row_lab->Add(new TObjString("CheckButton-Enable"));
	Valp[ind++] = &fUseCanvasSelMask;
	Int_t itemwidth = 420;
	static Int_t ok = -3;   // wait until closed, no Apply button
	new TGMrbValuesAndText ("Edit Selection Masks", NULL, &ok, itemwidth,
									fRootCanvas, NULL, NULL, row_lab, Valp,
									NULL, NULL, Help_SelectionMask_text);
	delete row_lab;
}
//________________________________________________________________________________________

void HistPresent::SaveFromSocket(const char * /*name*/, const char* /*bp*/)
{
	if (!fComSocket) return;
	TString fname ("hists_XX.root");

	Bool_t ok;
	const char * foutname=GetString("File Name",(const char *)fname,
											  &ok, fRootCanvas);
	if (!ok) return;
	if (!gSystem->AccessPathName((const char *)foutname, kFileExists)) {
		TString question=foutname;
		question += " already exists, overwrite?";
		if (!QuestionBox(question.Data(), fRootCanvas)) return;
	}

	TMrbStatistics * st = getstat(fComSocket);
	if (!st) {
		if ( !fComSocket->IsValid() ) {
			cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
			fComSocket->Close("force");
			fComSocket = NULL;
			if ( fHfromM_aButton )
				SetSelected(fHfromM_aButton, kFALSE);
		} else {
			cout << setred << "Cant get stat, connection seems alive" << setblack << endl;
		}
		 return;
	} else {
//         st->Print();
	}
	TFile *f = new TFile(foutname,"RECREATE");
	TH1 *hist=0;
	Int_t nhists = 0;
	TMrbStatEntry * stent;
	TIter nextentry(st->GetListOfEntries());
	while ( (stent = (TMrbStatEntry*)nextentry()) ) {
		hist = (TH1 *) gethist(stent->GetName(), fComSocket);
		if (!hist){
			if ( !fComSocket->IsValid() ) {
				cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
				fComSocket->Close("force");
				fComSocket = NULL;
				if ( fHfromM_aButton )
					SetSelected(fHfromM_aButton, kFALSE);
			} else {
				cout << setred << "Cant get histogram, connection seems alive" << setblack << endl;
			}
			break;
		}
		f->cd();
		if (hist) hist->Write();
//           cout << "Writing: " << stent->GetName()<< endl;
		nhists++;
	}
	if (f) f->Close();
	if (st) delete st;
	cout << nhists << " histograms saved to " << foutname << endl;
	gDirectory=gROOT;
}

//________________________________________________________________________________________
// Show Function

void HistPresent::ShowFunction(const char* fname, const char* dir, const char* name, const char*/*bp*/ )
{
	TF1* func;
	if (strstr(fname,".root")) {
		if (fRootFile) fRootFile->Close();
		fRootFile=new TFile(fname);
		if (strlen(dir) > 0) fRootFile->cd(dir);
		func = (TF1*)gDirectory->Get(name);
//      func->SetDirectory(gROOT);
//      fRootFile->Close();
	} else {
		func=(TF1*)gROOT->GetListOfFunctions()->FindObject(name);
	}
//  gROOT->ls();
	if (func) {
		cout  << endl << "Function " << func->GetName() << ": " << func->GetTitle()<< endl;
		cout << "Chi2 / NDF " << func->GetChisquare() << "/" << func->GetNDF() << endl;
		for (Int_t i = 0; i < func->GetNpar(); i++) {
			cout << func->GetParName(i) << "  " << func->GetParameter(i)
			<< " +- " <<  func->GetParError(i)<< endl;
		}
		cout  << endl << "----------------------------" << endl;

		if (func->InheritsFrom("TF2") ) {
			new HprFunc2((TF2*)func); 
		} else {
			new TCanvas();
			func->Draw();
		}
	} else {
		 WarnBox("Function not found");
	}
	if (fRootFile) fRootFile->Close();
}
//________________________________________________________________________________________
// Show user contour

void HistPresent::ShowContour(const char* fname, const char* dir, const char* name, const char*/*bp*/ )
{
	FhContour * co;
	if (strstr(fname,".root")) {
		if (fRootFile) fRootFile->Close();
		fRootFile=new TFile(fname);
		if (strlen(dir) > 0) fRootFile->cd(dir);
		co = (FhContour*)gDirectory->Get(name);
//      func->SetDirectory(gROOT);
		fRootFile->Close();
	} else {
		co = (FhContour*)gROOT->GetList()->FindObject(name);
	}
//  gROOT->ls();
	if (co) {
		TArrayI colsav(*(co->GetColorArray()));
		TArrayD levsav(*(co->GetLevelArray()));

		Int_t result = co->Edit(fRootCanvas);

		if (result >= 0){
			Bool_t changed = kFALSE;
			for (Int_t i = 0l; i < co->GetColorArray()->GetSize(); i++) {
				if (colsav[i] !=   (*(co->GetColorArray()))[i] ) {
					changed = kTRUE;
					break;
				}
				if (levsav[i] !=   (*(co->GetLevelArray()))[i]){
					changed = kTRUE;
					break;
				}
			}
			if (changed) {
				cout << "Changed" << endl;
				int buttons = kMBOk | kMBDismiss, retval = 0;
				EMsgBoxIcon icontype = kMBIconQuestion;
				new TGMsgBox(gClient->GetRoot(), fRootCanvas,
						 "Question", "Save modified contour?",
						 icontype, buttons, &retval);
				 if (retval == kMBOk) {
					 fRootFile=new TFile(fname, "UPDATE");
					 co->Write();
					 fRootFile->Close();
					 fRootFile = 0;
				}
			}
		}
	} else     WarnBox("Contour not found");

//   if (fRootFile) fRootFile->Close();
}
//________________________________________________________________________________________
// set rebin val

void HistPresent::SetRebinValue(Int_t val)
{
	Bool_t ok;
	if (val == 0) {
		Int_t i = GetInteger("Rebin value", fRebin, &ok, fRootCanvas);
		if (!ok || i <= 0) return;
		fRebinOth->SetTitle(Form("%d", i));
		fRebinOth->SetFillColor(3);
		fRebin2->SetFillColor(2);
		fRebin4->SetFillColor(2);
		fRebin=i;
	}else {
		fRebin=val;
		fRebinOth->SetFillColor(2);
		if (val == 2) {fRebin4->SetFillColor(2); fRebin2->SetFillColor(3);}
		if (val == 32) {fRebin4->SetFillColor(3); fRebin2->SetFillColor(2);}
	}
	fRebinOth->Modified(kTRUE);
	fRebin2->Modified(kTRUE);
	fRebin4->Modified(kTRUE);
}
//________________________________________________________________________________________
// set rebin val

void HistPresent::SetRebinMethod()
{
	if (fRMethod == 1) {
		fRebinSumAvg->SetTitle("Sum");
		fRMethod=0;
	} else {
		fRebinSumAvg->SetTitle("Avg");
		fRMethod=1;
	}
	fRebinSumAvg->Modified(kTRUE); fRebinSumAvg->Update();
}
//____________________________________________________________________________________

void HistPresent::SetOperateVal()
{
	Bool_t ok;
	Float_t fac = GetFloat("Factor", fOpfac, &ok, fRootCanvas);
	fOpfac = fac;
	fValButton->SetTitle(Form("%lg", fac));
}
//________________________________________________________________________________________
// Operate on  histograms

void HistPresent::OperateHist(Int_t op)
{
	Int_t nselect=fSelectHist->GetSize();
	if (nselect < 1) {
		WarnBox("No selection");
		return;
	}
//   if (nselect < 2 && op == 1) {
//      WarnBox("Less than 2 selections for Add, to scale choose *");
//      return;
//   }
	if (nselect > 2 && op != 1) {
		WarnBox("More than 2 selections only allowed for Add (+)");
//      nselect = 2;
		return;
	}
	if (nselect == 1 && op == 3) {
	  cout << "OperateHist: Scale errors linearly" << endl;
	}
	TH1* hist1 = GetSelHistAt(0);
	if (!hist1) {WarnBox("Histogram not found");return;};
	TString name1 = hist1->GetName();
	Int_t last_sem = name1.Last(';');    // chop off version
	if (last_sem >0) name1.Remove(last_sem);

	cout << "Do: " << name1;
	TString nameop;
	TString name2;
	TH1* hresult =  (TH1*)hist1->Clone();
	if ( hresult->GetSumw2()->GetSize() == 0 )
		hresult->Sumw2();
	Int_t nbinsx_1 = hist1->GetNbinsX();
	TH1* hist2;
	if (nselect == 2 || (op == 1 && nselect != 1)) {
//   if (nselect == 2) {
		for(Int_t i = 1; i < nselect; i++) {
			hist2 = GetSelHistAt(i);
			if (!hist2) {WarnBox("Histogram not found");return;};
			if (hist2->GetNbinsX() != hist1->GetNbinsX()
				 || (hresult->GetDimension() == 2 &&
				 (hist2->GetNbinsY() != hist1->GetNbinsY()))) {
				cout << setred << " canceled" << endl
				<< "Cant operate on hists with different number of bins "
				<< nbinsx_1 << " " << hist2->GetNbinsX();
				if (hist1->GetDimension() == 2)
					cout << " " << hist1->GetNbinsY() << " " << hist1->GetNbinsX();
				cout << setblack << endl;
				return;
			}
			if ( hist2->GetSumw2()->GetSize() == 0 )
				hist2->Sumw2();
			name2 = hist2->GetName();
			last_sem = name2.Last(';');    // chop off version
			if (last_sem > 0) name2.Remove(last_sem);
			switch (op) {
				case 1:
					hresult->Add(hist2, fOpfac);
					name1 = name1 + "_plus_" + name2;
					cout << " + " <<  fOpfac << " * " << name2;
					break;
				case 2:
					hresult->Add(hist2, -1.*fOpfac);
					nameop = "_minus_";
					cout << " - "<<  fOpfac << " * "  << name2;
					break;
				case 3:
					hresult->Multiply(hist1, hist2, 1., fOpfac);
					nameop = "_times_";
					cout << " * "<<  fOpfac << " * " << name2;
					break;
				case 4:
					hresult->Divide(hist1, hist2, 1., fOpfac);
					nameop = "_divby_";
					cout << " / "<<  fOpfac << " * " << name2;
					break;
			}
		}
	} else {
		Float_t fac = fOpfac;
		Int_t nbinsx;
		switch (op) {
			case 2:                       // subtract
				fac = - fOpfac;
//            break;
			case 1:                       // add
				name1 += "_biased";
				 nbinsx = hresult->GetNbinsX();
				if (hresult->GetDimension() == 1) {
					for(Int_t binx = 1; binx <= nbinsx; binx ++) {
					  Stat_t cont = hresult->GetBinContent(binx);
						hresult->SetBinContent(binx, cont + fac);
					}
				} else if (hresult->GetDimension() == 2) {
					Int_t nbinsy = hresult->GetNbinsY();
					for(Int_t binx = 1; binx <= nbinsx; binx ++) {
						for(Int_t biny = 1; biny <= nbinsy; biny ++) {
						  Stat_t cont = hresult->GetBinContent(binx, biny);
							hresult->SetBinContent(binx, biny, cont + fac);
						}
					}
				} else {
					Int_t nbinsy = hresult->GetNbinsY();
					Int_t nbinsz = hresult->GetNbinsZ();
					for(Int_t binx = 1; binx <= nbinsx; binx ++) {
						for(Int_t biny = 1; biny <= nbinsy; biny ++) {
							for(Int_t binz = 1; binz <= nbinsz; binz ++) {
								Stat_t cont = hresult->GetBinContent(binx, biny, binz);
								hresult->SetBinContent(binx, biny, binz, cont + fac);
							}
						}
					}
				}
				cout<< " + " << fac;
				break;
			case 4:                       // divide
				if (fOpfac != 0.) fac = 1./fOpfac;
				else                fac = 1.;
//            break;
			case 3:                        // multiply
				hresult->Sumw2();
				hresult->Scale(fac);
//            for( Int_t i=1; i <= hresult->GetNbinsX(); i++) {
//               hresult->SetBinError(i, fOpfac * hist1->GetBinError(i));
//            }
				nameop = "_scaled";
				cout<< " * " << fac;
				break;
		}
	}
	if (op != 1) {
		name1 = name1 + nameop + name2;
	}
	if (name1.Length() > 64) {
//      TDatime td;
//      name1 = "hist_";
//      ostrstream buf;

 //     buf << setfill('0') << setw(6) << td.GetTime()<< '\0';
 //     name1 += buf.str();
 //     buf.rdbuf()->freeze(0);
		WarnBox("Name for result hist got too long, watch terminal");
	}
	hresult->SetName((const char *)name1);
	cout << " store as: " << name1  << endl;
	ShowHist(hresult);
}
//________________________________________________________________________________________
// Differentiate histogram

void HistPresent::DiffHist()
{
	TH1* hist = GetSelHistAt(0);
	if (!hist) return;
	if (fSelectHist->GetSize() > 1) {
		WarnBox("More than 1 selection, take first");
	}
	if (is2dim(hist)) {
		WarnBox("2 dim not supported yet");
		return;
	}
//  gROOT->ls();
	if (hist) {
		// create a clone of the old histogram
		TH1F* hnew = (TH1F*)hist->Clone();

		// change name and axis specs
		TString name = hist->GetName();
		name += "_diffed";
		hnew->SetName((const char*)name);
		TAxis *xold   = hist->GetXaxis();
		Int_t nbins   = xold->GetNbins();

		// copy merged bin contents (ignore under/overflows)
		Int_t bin;
		Stat_t diff;
		for (bin = 1;bin<=nbins-1;bin++) {
			diff = hist->GetBinContent(bin) - hist->GetBinContent(bin+1) ;
			hnew->SetBinContent(bin,diff);
		}
		hnew->SetEntries(nbins);
		ShowHist(hnew);
	} else {
		WarnBox("No hist found");
	}

}
//________________________________________________________________________________________
// Rebin histograms

void HistPresent::RebinHist()
{
	TH1* hist = GetSelHistAt(0);
	if (!hist) {WarnBox("No hist found"); return;}
	if (fSelectHist->GetSize() > 1) {
		WarnBox("More than 1 selection, take first");
	}
	if (is2dim(hist)) {
		WarnBox("2 dim not supported yet");
		return;
	}
	TString buf(hist->GetName());
	buf += "_rebinby_";  buf += fRebin;
//   TString name = hist->GetName();
//   name += "_rebin_";
	TH1* hold=(TH1*)gROOT->GetList()->FindObject(buf.Data());
	if (hold) {
//      cout << "Delete existing " <<  buf.str()<< endl;
//      delete hold;
	}
	TH1F *hnew = (TH1F*)hist->Rebin(fRebin,buf.Data());
	if (fRMethod == 1) hnew->Scale(1./(Float_t)fRebin);
	ShowHist(hnew);
}
//________________________________________________________________________________________
//

TH1* HistPresent::GetSelHistAt(Int_t pos, TList * hl, Bool_t try_memory,
										 const char * hsuffix)
{
	TList * hlist;
	if (hl) hlist = hl;
	else   hlist = fSelectHist;
	if (hlist->IsEmpty()) {
		WarnBox("Please select a hist");
		return NULL;
	}
	if (hlist->GetSize() < pos) {
		WarnBox("Requested non existing entry");
		return NULL;
	}

	TObjString * obj = (TObjString *)hlist->At(pos);
//  Bool_t ok;

	TString fname = obj->String();
	if (gHprDebug > 0)
		cout << "GetSelHistAt |" << fname << "|" << endl;
	Int_t pp = fname.Index(",");
	if (pp < 0) {
		pp = fname.Index(" ");
		if (pp <= 0) {
			cout << "No file name in: " << obj->String() << endl;
			return NULL;
		}
	}
	fname.Resize(pp);

	TString hname = obj->String();
	hname.Remove(0,pp+1);
	TString dname = hname.Data();
	pp = hname.Index(",");
	if (pp < 0) {
		pp = hname.Index(" ");
		if (pp <= 0) {
			cout << "No histogram name in: " << obj->String()<< endl;
			return NULL;
		}
	}
	hname.Resize(pp);
	dname.Remove(0,pp+1);
//   cout << fname << "|" << hname << "|" << dname << "|" << endl;

	hname = hname.Strip(TString::kBoth);
	dname = dname.Strip(TString::kBoth);
	TH1* hist;
	hist = (TH1*)gROOT->GetList()->FindObject(hname);
	if (!hist && try_memory) {
		TString newname(fname.Data());
		pp = newname.Index(".");
		if (pp>0) newname.Resize(pp);
		newname += "_";
		newname += hname.Data();
		hist = (TH1*)gROOT->GetList()->FindObject(newname);
//      cout << "Use hist in memory: " << hname << " Fn: " << fname << " Nn: " << newname << endl;
	}
//   if (hist) hist->Print();
	if (hist && (fname == "Memory" || try_memory)) {
		if (gHprDebug > 0) 
			cout << "GetSelHistAt: " << fname << "|" << hname << "|" << dname << endl;
		return hist;
	}
	if (!(fname == "Memory")) {
		TString hn(hname);
		if ( hsuffix != NULL )
			hn += hsuffix;
		hist=(TH1*)gROOT->GetList()->FindObject(hn);
//     gROOT->ls();
		if (hist) {
//         cout << "Deleting existing histogram " << hname << endl;
			hist->Delete();
//         WarnBox("Using existing histogram");
//         return hist;
		}
	}
	if (strstr(fname,"Socket")) {
		if(!fComSocket){
			 cout << setred << "No connection open"  << setblack << endl;
			 return NULL;
		}
		hist = gethist(hname.Data(), fComSocket);
		if (!hist){
			if ( !fComSocket->IsValid() ) {
				cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
				fComSocket->Close("force");
				fComSocket = NULL;
				if ( fHfromM_aButton )
					SetSelected(fHfromM_aButton, kFALSE);
			} else {
				cout << setred << "Cant get histogram, connection seems alive" << setblack << endl;
			}
		} else {
			if ( hsuffix != NULL ) {
				TString hn(hist->GetName());
				hn += hsuffix;
				hist->SetName(hn);
			}
		}
		return hist;
	}
// watch out: is it.root or in memory

	TFile fi(fname);
	if (dname.Length() > 0)fi.cd(dname);
	hist = (TH1*)gDirectory->Get(hname);
	if (hist) hist->SetDirectory(gROOT);
		else
	cout << "Histogram: " << hname << " not found" << endl;
	gDirectory=gROOT;
//  gROOT->ls();
	TString newname(hname.Data());
	pp = newname.Index(";");
	if (pp >0)
		newname.Resize(pp);
	if (pos > 0) {
		newname += "_",
		newname += pos;
	}
	if ( GeneralAttDialog::fPrependFilenameToTitle != 0 ) {
		TString title = hist->GetTitle();
		fname = gSystem->BaseName(fname);
		pp = fname.Index(".");
		fname.Resize(pp);
		title.Prepend("_");
		title.Prepend(fname);
		hist->SetTitle(title);
	}
	if (hist) hist->SetName(newname.Data());
	return hist;
}
//________________________________________________________________________________________
//

TObject* HistPresent::GetSelGraphAt(Int_t pos)
{
	TList * hlist = fSelectGraph;
	if (hlist->IsEmpty()) {
		WarnBox("Please select a graph first");
		return NULL;
	}
	if (hlist->GetSize() < pos) {
		WarnBox("Requested non existing entry");
		return NULL;
	}
 //  Bool_t ok;
	TObjString * obj = (TObjString *)hlist->At(pos);

	TString fname = obj->String();
	if (gHprDebug > 0)
		cout << "GetSelGraphAt |" << fname << "|" << endl;
	Int_t pp = fname.Index(" ");
	if (pp <= 0) {cout << "No file name in: " << obj->String() << endl; return NULL;};
	fname.Resize(pp);

	TString hname = obj->String();
	hname.Remove(0,pp+1);
	TString dname = hname.Data();
	pp = hname.Index(" ");
	if (pp <= 0) {cout << "No graph name in: " << obj->String()<< endl; return NULL;};
	hname.Resize(pp);
	dname.Remove(0,pp+1);
	//cout << fname << "|" << hname << "|" << dname << "|" << endl;

	hname = hname.Strip(TString::kBoth);
	pp = hname.Index(";");
	if (pp >0) hname.Resize(pp);
	dname = dname.Strip(TString::kBoth);
	TObject* graph;
	graph = gROOT->GetList()->FindObject(hname);
/*
	if (!hist) {
		TString newname(fname.Data());
		pp = newname.Index(".");
		if (pp>0) newname.Resize(pp);
		newname += "_";
		newname += hname.Data();
		hist = (TGraph*)gROOT->GetList()->FindObject(newname);
		cout << "Use graph in memory: " << hname << " Fn: " << fname << " Nn: " << newname << endl;
	}
*/
//   if (hist) hist->Print();
	if (graph && (fname == "Memory")) return graph;
	if (fRootFile) fRootFile->Close();
	fRootFile=new TFile((const char *)fname);
	if (dname.Length() > 0) fRootFile->cd(dname);
	graph = gDirectory->Get(hname);
	if (graph->InheritsFrom("TGraph")) {
//		((TGraph*)graph)->SetDirectory(gROOT);
	} else if (graph->InheritsFrom("TGraph2D")) {
		((TGraph2D*)graph)->SetDirectory(gROOT);
	} else {
		graph = NULL;
	}
	if (fRootFile) {fRootFile->Close(); fRootFile=NULL;};

	gDirectory=gROOT;
//  gROOT->ls();
/*
	TString newname(hname.Data());
	newname += "_",
	newname += pos;
	if ( GeneralAttDialog::fPrependFilenameToTitle != 0 ) {
		TString title = hist->GetTitle();
		fname = gSystem->BaseName(fname);
		pp = fname.Index(".");
		fname.Resize(pp);
		title.Prepend("_");
		title.Prepend(fname);
		hist->SetTitle(title);
	}
	if (hist && hi) hist->SetName(newname.Data());
	*/
	return graph;
}
//________________________________________________________________________________________
// Get Ccut

void HistPresent::SelectCut(const char* fname, const char* cname, const char* /*bp*/)
{
	if (!gPad) cout << "Cut: Cant find Pad, call debugger" << endl;
	else      gPad->SetFillColor(3);

	if (fRootFile) fRootFile->Close();
	fRootFile=new TFile(fname);
	fRootFile->Get(cname);
//   TCutG * cut= (TCutG*)fRootFile->Get(cname);
 //  if (cut) cut->SetDirectory(gROOT);
	fRootFile->Close();
	gDirectory=gROOT;
}
//_______________________________________________________________________

void HistPresent::CloseHistLists()
{
	ClearSelect();
//   fHistLists->ls();
	fHistLists->Delete();
}
//________________________________________________________________________________________
// Select

void HistPresent::SelectHist(const char* fname, const char* dir, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	sel = sel + "," + hname + "," + dir;
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
		if (b->TestBit(kSelected)) {
			TObjString tobjs((const char *)sel);
	//      tobjs.Print(" ");
			fSelectHist->Remove(&tobjs);
			b->SetFillColor(16);
			b->ResetBit(kSelected);
	//      gPad->SetFillColor(16);
	//     case not already selected
		} else {
			fSelectHist->Add(new TObjString((const char *)sel));
			b->SetFillColor(3);
			b->SetBit(kSelected);
	//      gPad->SetFillColor(3);
		}
		b->Modified(kTRUE);b->Update();
	}
}
//________________________________________________________________________________________
// Select

void HistPresent::SelectCanvas(const char* fname, const char* dir, const char* hname, const char* bp)
{
	cout << " HistPresent::SelectCanvas "  << fname << " " << hname << endl;
	TString sel = fname;
	sel = sel + "," + hname + "," + dir;
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
		//      cout << "bp " << b << endl;
		if (b->TestBit(kSelected)) {
			TObjString tobjs((const char *)sel);
			//      tobjs.Print(" ");
			fSelectCanvas->Remove(&tobjs);
			b->SetFillColor(16);
			b->ResetBit(kSelected);
			//      gPad->SetFillColor(16);
			//     case not already selected
		} else {
			fSelectCanvas->Add(new TObjString((const char *)sel));
			b->SetFillColor(3);
			b->SetBit(kSelected);
			//      gPad->SetFillColor(3);
		}
		b->Modified(kTRUE);b->Update();
	}
}
//________________________________________________________________________________________
// Select

void HistPresent::SelectContour(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	sel = sel + " " + hname;
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
		if (b->TestBit(kSelected)) {
			TObjString tobjs((const char *)sel);
	//      tobjs.Print(" ");
			fSelectContour->Remove(&tobjs);
			b->SetFillColor(16);
			b->ResetBit(kSelected);
	//      gPad->SetFillColor(16);
	//     case not already selected
		} else {
			fSelectContour->Add(new TObjString((const char *)sel));
			b->SetFillColor(3);
			b->SetBit(kSelected);
	//      gPad->SetFillColor(3);
		}
		b->Modified(kTRUE);b->Update();
	}
}
//________________________________________________________________________________________
// Select

void HistPresent::SelectGraph(const char* fname, const char* dir, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	sel = sel + " " + hname + " " + dir;
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
		if (b->TestBit(kSelected)) {
			TObjString tobjs((const char *)sel);
			fSelectGraph->Remove(&tobjs);
			b->SetFillColor(16);
			b->ResetBit(kSelected);
		} else {
			b->SetFillColor(3);
			b->SetBit(kSelected);
			fSelectGraph->Add(new TObjString((const char *)sel));
	//      gPad->SetFillColor(3);
		}
		b->Modified(kTRUE);b->Update();
	}
}
//________________________________________________________________________________________

void HistPresent::PrintCut(const char* fname, const char* hname, const char* /*bp*/)
{
	TCutG* cut;
	if (is_memory(fname)) {
		cut = (TCutG*)gROOT->GetList()->FindObject(hname);
	} else {
		if (fRootFile) fRootFile->Close();
		fRootFile=new TFile(fname);
		cut = (TCutG*)fRootFile->Get(hname);
		if (fRootFile) fRootFile->Close();
		gDirectory=gROOT;
	}
	if (cut) {
		cout << cut->GetName() << ":"<<endl;
		cout<< "fVarX:" << cut->GetVarX() <<endl;
		cout<< "fVarY:" << cut->GetVarY() <<endl;
		int n = cut->GetN();
		for(int i=0; i<n; i++)  {
			Double_t x,y;
		  cut->GetPoint(i,x,y);
		  cout << "x: " << x << " \ty: " << y << endl;
		}
		if (strncmp("Memory",fname,6)) delete cut;
	}
}
//________________________________________________________________________________________

void HistPresent::CutsToASCII(const char* name, const char* /*bp*/)
{
	if (fAllCuts->GetSize() <= 0) {
		WarnBox("No cuts selected");
		return;
	}
	const char helpText[] = "Write values of cuts (Window2D) as ASCII file";
	TRegexp rname("root");
	TString fname = name;
	fname(rname) ="wdw2D";
	Bool_t ok;
	fname = GetString("Write ASCII-file with name",fname.Data(),  &ok, fRootCanvas,
	0, 0, helpText);
	if (!ok) {
		cout << " Canceled " << endl;
		return;
	}
	if (!gSystem->AccessPathName((const char *)fname, kFileExists)) {
//      cout << fname << " exists" << endl;
		int buttons= kMBOk | kMBDismiss, retval=0;
		EMsgBoxIcon icontype = kMBIconQuestion;
		new TGMsgBox(gClient->GetRoot(), fRootCanvas,
		 "Question", "File exists, overwrite?",
		 icontype, buttons, &retval);
		if (retval == kMBDismiss) return;
	}
	ofstream outfile;
	outfile.open((const char *)fname);
	if (!outfile) {
		WarnBox("Cant open file");
		return;
	}
	TIter next(fAllCuts);
	while ( TMrbWindow2D * cut = (TMrbWindow2D *)next()) {
		Int_t n = cut->GetN();
		outfile << cut->GetName() <<endl;
		outfile << n << endl;
		for(Int_t i=0; i < n; i++) {
			Double_t x,y;
			cut->GetPoint(i,x,y);
			outfile << x<< "\t" << y<< endl;
		}
//      cut->Print();
	}
	outfile.close();
}
//________________________________________________________________________________________
void HistPresent::CutsFromASCII(TGWindow * win)
{
	const char helpText[] = "Read values of cuts (Window2D) from ASCII file";
	TRegexp endwithwdw2D("wdw2D$");
	TString fname = "xxx.wdw2D";
	void* dirp=gSystem->OpenDirectory(gHprWorkDir);
	while (const char * name=gSystem->GetDirEntry(dirp)) {
		TString sname(name);
		if (sname.Index(endwithwdw2D) >= 0) fname = sname;
	}
	Bool_t ok;
	fname = GetString("Filename",fname.Data(), &ok, win,0, 0, helpText);
	if (!ok) {
		cout << " Canceled " << endl;
		return;
	}
	ifstream infile;
	infile.open((const char *)fname, ios::in);
	if (!infile.good()) {
	cerr	<< "CutsFromASCII: "
			<< gSystem->GetError() << " - " << fname
			<< endl;
		return;
	}
	TString cutname;
	Int_t npoints;
	Float_t x,y;
	while ( 1) {
		cutname.ReadLine(infile, kFALSE);
		if (infile.eof()) {
			infile.close();
			return;
		}
		cutname = cutname.Strip();
		if (cutname.Length() <= 0) continue;
		infile >> npoints;
		cout << endl;
		cout << cutname.Data() << " " << npoints << endl;
		if (npoints <= 0) {WarnBox("npoints <= 0"); return;}

		TMrbWindow2D * wdw = new TMrbWindow2D((const char *)cutname, npoints);
		for(Int_t i = 0; i < npoints; i++) {
			infile >> x; infile >> y;
			wdw->SetPoint(i, x, y);
		}
		fAllCuts->Add(wdw);
	}
}
//________________________________________________________________________________________
void HistPresent::LoadCut(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	sel = sel + " " + hname;
	TButton * b;
	b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
	if (!b->TestBit(kSelected)) {
//   if (color != 3) {
		if (!strncmp("Memory",fname,6)) {
			cout << " Cut was removed" << endl;
		} else {
			if (fRootFile) fRootFile->Close();
			fRootFile=new TFile(fname);
			TCutG* cut= (TCutG*)fRootFile->Get(hname);
//         cout << "TCutG, class " << cut->ClassName() << endl;
			TMrbWindow2D * wdw2d=
			new TMrbWindow2D(cut->GetName(),cut->GetN(),cut->GetX(),cut->GetY());
			fAllCuts->Add(wdw2d);
//  cut->SetDirectory(gROOT);
			fRootFile->Close();
			gDirectory=gROOT;
			fSelectCut->Add(new TObjString((const char *)sel));
//     case not already selected
			b->SetFillColor(3);
			b->SetBit(kSelected);
		}
	} else {
		TMrbWindow2D* cc = (TMrbWindow2D*)fAllCuts->FindObject(hname);
		TList *tl=gDirectory->GetList();
		TIter next(tl);
		while ( TObject* obj=next()) {
			if (!(strncmp(obj->ClassName(),"FitHist",7))) {
				FitHist *fhist=(FitHist*)obj;
				if (fhist->UseCut(cc)) {
					cout << "Cut: " << hname << " used by fithist" << endl;
					return;
				}
			}
		}
		TObjString tobjs((const char *)sel);
		cout << "remove from list: ";
		tobjs.Print(" ");
		if (cc) {
			cout << "remove from memory: " << hname << endl;
			fAllCuts->Remove(cc);
			delete cc;
		}
		fSelectCut->Remove(&tobjs);
		b->SetFillColor(16);
		b->ResetBit(kSelected);
	}
	b->Modified(kTRUE);
	b->Update();
}
//________________________________________________________________________________________
void HistPresent::PrintWindow(const char* fname, const char* hname, const char* /*bp*/)
{
	TObject * obj = NULL;
	TString sel = fname;
	if (fRootFile) fRootFile->Close();
	fRootFile=new TFile(fname);
	obj = (TObject*)fRootFile->Get(hname);
	if (obj) {
		 TNamed* nobj=(TNamed*)obj;
		 cout << " " << endl;
		 cout << "---   Window: " << nobj->GetName() << " -----------" << endl;
		 obj->Print(); delete obj; obj = 0;
	}
	if (fRootFile) {fRootFile->Close(); fRootFile = 0;}
	gDirectory=gROOT;
}
//________________________________________________________________________________________

void HistPresent::LoadFunction(const char* fname, const char* dir, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	TObject * fun = 0;
	sel = sel + " " + hname;
	TObjString tobjs((const char *)sel);
	TButton * b;
	b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
	if (!b->TestBit(kSelected)) {
//   if (color != 3) {
		if (!strncmp("Memory",fname,6)) {
			cout << " Function was removed??" << endl;
		} else {
			if (fRootFile) fRootFile->Close();
			fRootFile=new TFile(fname);
			if (strlen(dir) > 0) fRootFile->cd(dir);
			fun= (TObject*)gDirectory->Get(hname);
			fRootFile->Close(); fRootFile=0;
		}
		if (fun) {
//         cout << "fun->GetClassName(): "<< fun->ClassName() << endl;
			TObject * tmp;
			tmp = fAllFunctions->FindObject(fun->GetName());
			if (tmp) {
				cout << fun->GetName()<< " existing "  << endl;
				TString newname(hname);
				TRegexp sem(";");
				newname(sem)="_v";
				TF1* fun1= (TF1*)fun;
				fun1->SetName(newname.Data());
//               fAllCuts->Remove(tmp);
			}
			fAllFunctions->Add(new TObjString((const char *)sel));
		}
		gDirectory=gROOT;
//     case not already selected
		b->SetFillColor(3);
		b->SetBit(kSelected);
	} else {
//     sel = hname;
//      Int_t isem = sel.Index(";");
//      if (isem > 0) sel.Resize(isem);
//      cout << sel.Data() << endl;
//      fun = fAllFunctions->FindObject(sel.Data());
//      if (fun) {
//         cout << "Remove:" << tobjs.GetString()<< endl;
			fAllFunctions->Remove(&tobjs);
//      }
		b->SetFillColor(16);
		b->ResetBit(kSelected);
	}
	b->Modified(kTRUE);
	b->Update();
}
//________________________________________________________________________________

void HistPresent::LoadWindow(const char* fname, const char* hname, const char* bp)
{
//   cout << fname << " " << hname << endl;
	TString sel = fname;
	TObject * wdw = 0;
	sel = sel + " " + hname;
	TButton * b;
	b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
	if (!b->TestBit(kSelected)) {
//   if (color != 3) {
		if (!strncmp("Memory",fname,6)) {
			cout << " Window was removed??" << endl;
		} else {
			if (fRootFile) fRootFile->Close();
			fRootFile=new TFile(fname);
			wdw= (TObject*)fRootFile->Get(hname);
		}
		if (wdw) {
//         cout << "wdw->GetClassName(): "<< wdw->ClassName() << endl;
			TObject * tmp;
			if (wdw->InheritsFrom("TCutG")) {
			  CheckList(fAllCuts, "TMrbWindow");
			  if ( (tmp = fAllCuts->FindObject(wdw->GetName()) ) ) {
					cout << wdw->GetName()<< " existing "  << endl;
					TString newname(hname);
					TRegexp sem(";");
					newname(sem)="_v";
					TMrbWindow2D* wdw2d= (TMrbWindow2D*)wdw;
					wdw2d->SetName(newname.Data());
				}
				fAllCuts->Add(wdw);
			} else {
//            wdw->Dump();
				CheckList(fAllWindows, "TMrbWindow");
				tmp = fAllWindows->FindObject(wdw->GetName());
				if (tmp) {
					cout << wdw->GetName()<< " existing "  << endl;
					TString newname(hname);
					TRegexp sem(";");
					newname(sem)="_v";
					TMrbWindow* wdw1d= (TMrbWindow*)wdw;
					wdw1d->SetName(newname.Data());
//               fAllCuts->Remove(tmp);
				}
				fAllWindows->Add(wdw);
			}
		}
		fSelectWindow->Add(new TObjString((const char *)sel));
		if (fRootFile) {fRootFile->Close(); fRootFile=0;}
		gDirectory=gROOT;
//     case not already selected
		b->SetFillColor(3);
		b->SetBit(kSelected);
	} else {
		TMrbWindow* cc=(TMrbWindow*)fAllWindows->FindObject(hname);
		TList *tl=gDirectory->GetList();
		TIter next(tl);
		while ( TObject* obj=next()) {
			if (!(strncmp(obj->ClassName(),"FitHist",7))) {
				FitHist *fhist=(FitHist*)obj;
				if (fhist->UseWindow(cc)) {
					cout << "Window: " << hname <<
					" used by FitHist " << fhist->GetName()
					<< " cant remove from memory" << endl;
					return;
				}
			}
		}
		TObjString tobjs((const char *)sel);
		fSelectWindow->Remove(&tobjs);
		fAllWindows->Remove(&tobjs);
		fAllCuts->Remove(&tobjs);
		if (cc) {
//         cout << "remove from memory: " << hname << endl;
			gDirectory->GetList()->Remove(cc);
			delete cc;
		}
		b->SetFillColor(16);
		b->ResetBit(kSelected);
	}
	b->Modified(kTRUE);
	b->Update();
}
//__________________________________________________________________________________

void HistPresent::ClearSelect()
{
		if (fHistLists->GetSize() > 0) {
			TIter next(fHistLists);
//         cout << "hl->GetSize() " << hl->GetSize() << endl;
			TCanvas *ca;
			while ( (ca = (TCanvas*)next()) ) {
				TIter nextobj(ca->GetListOfPrimitives());
				TButton *b;
				TObject * obj;
				while ( (obj = nextobj()) ) {
					if (!strcmp(obj->ClassName(),"TButton")) {
						b = (TButton*)obj;
						if (b->TestBit(kSelected) && b->TestBit(kSelection)) {
							b->ResetBit(kSelected);
							b->SetFillColor(15);
							b->Modified(kTRUE);
							b->Update();
						}
					}
				}
			}
		}
		fSelectHist->Clear();
		fAllFunctions->Clear();
		fSelectCut->Clear();
		fSelectWindow->Clear();
		fSelectContour->Clear();
		fSelectGraph->Clear();
};
//__________________________________________________________________________________

void HistPresent::ListSelect()
{
	TObjString *sel;
	cout << "-------------------- " << endl;
	cout << "Selected histogramms " << endl;
	TIter next(fSelectHist);
//   pointers->Sort();
	Int_t count = 0;
	while ( (sel= (TObjString*)next()) ) {
		count ++;
		cout << count << ". ";
		sel->Print(" ");
	}
	if (count>0) cout << "----------------" << endl;
	cout << "Selected cuts " << endl;
	cout << "-------------" << endl;
	TIter nextcut(fSelectCut);
//   pointers->Sort();
	count = 0;
	while ( (sel = (TObjString*)nextcut()) ) {
		count ++;
		cout << count << ". ";
		sel->Print(" ");
	}
	if (count>0) cout << "----------------" << endl;
	cout << "All windows " << endl;
	cout << "----------------" << endl;
	TIter allwdw(fAllWindows);
//   pointers->Sort();
	count = 0;
	while ( TMrbWindow * wdw = (TMrbWindow*)allwdw()) {
		count ++;
		cout << count << ". ";
		wdw->Print(" ");
	}
	if (count>0) cout << "----------------" << endl;
	cout << "Selected windows " << endl;
	cout << "----------------" << endl;
	TIter nextwdw(fSelectWindow);
//   pointers->Sort();
	count = 0;
	while ( (sel= (TObjString*)nextwdw()) ) {
		count ++;
		cout << count << ". ";
		sel->Print(" ");
	}
	if (count>0) cout << "----------------" << endl;
	cout << "Selected Functions " << endl;
	cout << "----------------" << endl;
	fAllFunctions->Print();
	if (count>0) cout << "----------------" << endl;
	cout << "Selected User Contours " << endl;
	cout << "-------------------- " << endl;
	fSelectContour->Print();
	if (count>0) cout << "----------------" << endl;
	cout << "Selected Graphs " << endl;
	cout << "-------------------- " << endl;
	fSelectGraph->Print();
	cout << "-------------------- " << endl;
};
//_______________________________________________________________________

TH1* HistPresent::GetHist(const char* fname, const char* dir, const char* hname)
{
//   TurnButtonGreen(&activeHist);
	TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
	TH1* hist=0;
	TString shname = hname;
//	if (shname.Index(";") > 0) {
//		TString sind = shname(shname.Index(";")+1, 10);
//		if (sind.IsDigit() && sind.Atoi() == 1) {
//			shname.Resize(shname.Index(";"));
//		}
//	}
//    shname = shname.Strip(TString::kBoth);
	if (strstr(fname,".root")) {
		TString newhname;
		if (fRootFile) fRootFile->Close();
		if ( GeneralAttDialog::fPrependFilenameToName ) {
			newhname = gSystem->BaseName(fname);
			Int_t pp = newhname.Index(".");
			if (pp) newhname.Resize(pp);
			newhname += "_";
		}
		newhname += shname.Data();
		
		while (newhname.Index(notascii) >= 0) {
			newhname(notascii) = "_";
		}
		if (!hist) {
			fRootFile=new TFile(fname);
			if (strlen(dir) > 0) fRootFile->cd(dir);
			Int_t kn = 9999;
			Int_t is = shname.Index(";");
			if (is > 0){
				TString sk = shname(is+1, shname.Length()-1);
				kn = sk.Atoi();
				if (kn <= 0) kn = 9999;
				shname.Resize(is);
			}
//         cout << shname.Data() << " " << kn << endl;
			TKey * key = gDirectory->GetKey(shname.Data(), kn);
			if (key)hist = (TH1*)key->ReadObj();
			if (hist) {
				hist->SetDirectory(gROOT);
//            TDatime dt = gDirectory->GetModificationDate();
				TDatime dt = key->GetDatime();
				hist->SetUniqueID(dt.Convert());
				hist->SetName(newhname);
			} else {
				cout << "Histogram: " << hname <<
						  " not found in: " << fname << endl;
			}
			fRootFile->Close();
		}

	} else if (strstr(fname,"Socket")) {
		if(!fComSocket){
			 cout << setred << "No connection open"  << setblack << endl;
			 return NULL;
		}
//      Bool_t ok;
		hist = gethist(hname, fComSocket);
		if (!hist){
			if ( !fComSocket->IsValid() ) {
				cout << setred << "Connection lost, M_analyze dead??" << setblack << endl;
				fComSocket->Close("force");
				fComSocket = NULL;
				if ( fHfromM_aButton )
					SetSelected(fHfromM_aButton, kFALSE);
			} else {
				cout << setred << "Cant get histogram, connection seems alive" << setblack << endl;
			}
		} else {
			hist->SetUniqueID(0);
		}
	} else {
		hist=(TH1*)gROOT->GetList()->FindObject(shname.Data());
	}
//   cout << "End GetHist" << endl;
	gDirectory=gROOT;
	return hist;
}
//_____________________________________________________________________________________________

void HistPresent::CleanWindowLists(TH1* hist)
{
	if (!hist) return;
//   cout << "CleanWindowLists" << endl;
	TList *lof = hist->GetListOfFunctions();
	if (lof) {
		TObject *p;
		TIter next(lof);
		while ( (p = (TObject *) next()) ) {
			fAllCuts->Remove(p);
			fAllWindows->Remove(p);
			fAllFunctions->Remove(p);
			fSelectCut->Remove(p);
			fSelectWindow->Remove(p);
		}
	}
}
//_______________________________________________________________________

void HistPresent::ShowHist(const char* fname, const char* dir, const char* hname, const char* bp)
{
	TButton * b = NULL;
	TString fn(fname);
	HTCanvas *mother_canvas = NULL;
	if (bp) {
		b = (TButton *)strtoul(bp, 0, 16);
		if ( b ) {
			mother_canvas = (HTCanvas*)b->GetCanvas();
			mother_canvas->SetEnableButtons(kFALSE);
		}
	}
	TH1* hist = GetHist(fname, dir, hname);
	if ( hist ) {
		if ( !hist->TestBit(kNotDeleted) ) {
			cout << "Histogram is deleted: " << hname << endl;
			if (mother_canvas)
				mother_canvas->SetEnableButtons(kTRUE);
			return;
		}

		if ( b ) {
			b->SetBit(kSelected);
			b->SetFillColor(3);
			b->Modified(kTRUE);
			b->Update();
		}
//      TurnButtonGreen(&activeHist);
		if ( GeneralAttDialog::fPrependFilenameToTitle != 0 ) {
			TString title = hist->GetTitle();
			fn = gSystem->BaseName(fn);
			Int_t pp = fn.Index(".");
			fn.Resize(pp);
			title.Prepend("_");
			title.Prepend(fn);
			hist->SetTitle(title);
		}
		if ( fn != "Memory" && GeneralAttDialog::fAppendTimestampToHistname != 0 ) {
			TString name = hist->GetName();
			TDatime dt;
			name += "_";
			name += dt.GetTime();
			hist->SetName(name);
		}
		ShowHist(hist, hname, b);
	} else     WarnBox("Histogram not found");
	 if (mother_canvas)
		 mother_canvas->SetEnableButtons(kTRUE);
}
//_______________________________________________________________________

FitHist * HistPresent::ShowHist(TH1* hist, const char* hname, TButton *b)
{
//   RestoreOptions();

	static TString FHnameSave;
	static FitHist *fh=0;
	static Int_t first_warn = 0;
	TString origname;
	if (hname) origname = hname;
	else       origname = hist->GetName();
	if (origname.Index(";") > 1)origname.Resize(origname.Index(";"));
	TString FHname("F");
	FHname += hist->GetName();
	if (FHname.Index(";") > 1) {
		TString sind = FHname(FHname.Index(";")+1, 10);
		if (sind.IsDigit() && sind.Atoi() == 1) {
			FHname.Resize(FHname.Index(";"));
		}
	}
	if (gHprDebug > 0){
		cout << "enter HistPresent::ShowHist: "<< FHname << endl << flush;
		cout << "hist->GetName() " <<   hist->GetName();
		if (hname ) cout <<  " input hname " << hname;
		cout << " origname " << origname << endl;
	}
	TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
	while (FHname.Index(notascii) >= 0) {
		FHname(notascii) = "_";
	}
	while (origname.Index(notascii) >= 0) {
		origname(notascii) = "_";
	}
	/* following section must be adjusted for 6.12.04
	if (gDirectory) {
		TList *tl=gDirectory->GetList();
		FitHist *fhist = (FitHist*)tl->FindObject(FHname);
		if (fhist) {
			if (gHprDebug > 0)
				cout << "In ShowHist: Delete canvas: " <<  fhist->GetCanvas()<< endl<< flush;
//         gDirectory->GetList()->Remove(hold);
			gROOT->GetListOfCanvases()->Remove(fhist->GetCanvas());
			TH1 *oldhist=fhist->GetSelHist();
//			delete fhist->GetCanvas();
//			if (oldhist != hist)
//				delete oldhist;
//			gSystem->ProcessEvents();
//			gSystem->Sleep(500);
//			gSystem->ProcessEvents();
//			gSystem->Sleep(100);
		}
//     Cleaning FitHist objects for which Canvas was closed
		TIter next(tl);
		while ( TObject* obj=next()) {
			if (!(strncmp(obj->ClassName(),"FitHist",7))) {
				fhist=(FitHist*)obj;
				const char * cn=fhist->GetCanvasName();
				if (cn) {
					if (!(gROOT->GetListOfCanvases()->FindObject(cn))) {
						cout << "Not Deleting unused: " << fhist << " " << fhist->GetName() << endl;
//                  fhist->Delete();
					}
				}
			}
		}
//      AppendDirectory();
	}
	*/
	nHists++;
//	cout << "ShowHist: fNwindows, FHname, save " << WindowSizeDialog::fNwindows<< " " << FHname << " " <<FHnameSave << endl;
//   if (FHnameSave != FHname) {
		if (WindowSizeDialog::fNwindows > 0) {       // not the 1. time
			WindowSizeDialog::fWincurx += WindowSizeDialog::fWinshiftx;
			WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;
		}
		WindowSizeDialog::fNwindows++;
		FHnameSave = FHname;
//   }
//   cout << " fNwindows " << WindowSizeDialog::fNwindows << " " <<WindowSizeDialog::fWinshiftx<< endl;
	if (WindowSizeDialog::fNwindows > 10 && first_warn == 0) {
//		if (QuestionBox("More than 10 hists on screen!! Remove them?", fLastWindow))
//			 CloseAllCanvases();
		first_warn = 1;
		cout << setblue << "More than 10 hists on screen, this may cause trouble"
			<< setblack << endl;
	}
	Int_t wwidx;
	Int_t wwidy;

	if (hist->GetDimension() == 1) {
	  wwidx = WindowSizeDialog::fWinwidx_1dim;
	  wwidy = WindowSizeDialog::fWinwidy_1dim;
	} else {
		wwidx = WindowSizeDialog::fWinwidx_2dim;
		wwidy = WindowSizeDialog::fWinwidy_2dim;
	}
//    cout << "FHname " << FHname << endl;
//    cout << "hist->GetName() " << hist->GetName() << endl;
//    cout << "origname " << origname << endl;
	if (gHprDebug > 0)
		cout << "HistPresent::ShowHist new FitHist: "<< FHname << endl<< flush;
	fh=new FitHist((const char*)FHname,"A FitHist object",hist, origname.Data(),
			 WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury, wwidx, wwidy, b);
	fLastWindow = fh->GetMyCanvas();
//	fh->SetCmdButton(b);
	return fh;
}
//_______________________________________________________________________

void HistPresent::CloseAllCanvases()
{
// Cleaning all FitHist objects
   if (gHprDebug > 0 )
		cout << "Enter CloseAllCanvases()" << endl;
	TQObject::Disconnect("TPad", "Modified()");
	if (fHelpBrowser) fHelpBrowser->Clear();
	fCloseWindowsButton->SetMethod(".! echo \"Please be patient\"");
	fCanvasClosing = kTRUE;
//   TIter next(fCanvasList);
	TIter next(gROOT->GetListOfCanvases());
	TCanvas * htc;
	Int_t nc = 0;
	while ( (htc =(TCanvas *)next()) ) {
//		if (gHprDebug > 0 )
//			cout << "CloseAllCanvases: " << htc << " " << htc->GetName()<< endl;
		TString cn(htc->GetName());
		if ( cn == "cHPr" || cn == "FileList" || cn.EndsWith("histlist")
			|| cn == "ContentList"  || cn == "Windows"|| cn == "Cuts" 
			|| cn.BeginsWith("Tree:")) continue;
		htc->Disconnect("TPad", "Modified()");
		TRootCanvas *rc = (TRootCanvas*)htc->GetCanvasImp();
		if (rc == NULL) {
			if (gHprDebug>1)
				cout << "CloseAllCanvases delete: " << htc << endl;
			if (fHelpBrowser) fHelpBrowser->GetCanvasList()->Remove(htc);
			delete htc;
			continue;
		}
		if (htc->TestBit(HTCanvas::kIsAEditorPage)) {
			cout << "rc->ShowEditor(kFALSE);"<< endl;
			rc->ShowEditor(kFALSE);
		}
		if (gHprDebug > 1)
			cout << "SendCloseMessage: " << htc << " " << htc->GetName()<< " " << rc<< endl;
		rc->SendCloseMessage();
//		delete htc;
		nc++;
	}
	WindowSizeDialog::fNwindows = 0;
	WindowSizeDialog::fWincury = WindowSizeDialog::fWintopy;
	WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
//   fNtupleSeqNr = 0;
	cout << "Closed: " << nc << " canvas" ;
	if ( nc != 1 ) cout << "es";
	cout << endl;
	fCloseWindowsButton->SetMethod("gHpr->CloseAllCanvases()");
	fCanvasClosing = kFALSE;
}
//_______________________________________________________________________

void HistPresent::StackSelectedHists(const char */*bp*/)
{
	StackSelectedHists(fSelectHist);
}
//_______________________________________________________________________
/*
void HistPresent::StackSelectedHistsScaled(const char *bp)
{
	StackSelectedHists(fSelectHist," ", 1);
}*/
//_______________________________________________________________________

void HistPresent::StackSelectedHists(TList * hlist, const char* /*title*/)
{
	TList hl;
	Int_t nsel = hlist->GetSize();
//	cout << "StackSelectedHists  " << nsel << endl;
	for(Int_t i=0; i<nsel; i++) {
		TH1* hist = GetSelHistAt(i, hlist);
		if (!hist) {
			cout << " Hist not found at: " << i << endl;
			continue;
		} else {
			if (gHprDebug > 0)
				cout << "StackSelectedHists  " << hist->GetName()<< " " << hist << endl;
			hl.Add(hist);
		}
	}
	new HprStack(&hl, GeneralAttDialog::fScaleStack);
}
//_______________________________________________________________________

void HistPresent::ShowSelectedHists(const char */*bp*/)
{
	ShowSelectedHists(fSelectHist);
}
//_______________________________________________________________________

void HistPresent::ShowSelectedHists(TList * hlist, const char* title)
{
	new GroupOfHists(hlist, this, title);
}
//____________________________________________________________________________

void HistPresent::WarnBox(const char *message)
{
	int retval = 0;
	new TGMsgBox(gClient->GetRoot(), fRootCanvas,
					 "Warning", message, kMBIconExclamation, kMBDismiss,
					 &retval);
}
//____________________________________________________________________________

Int_t NofEditorPages()
{
	Int_t n = 0;
	TIter next(gROOT->GetListOfCanvases());
	TObject * obj;
	while ( (obj = next()) ) {
		if (obj->TestBit(HTCanvas::kIsAEditorPage)) n++;
	}
	return n;
}
//____________________________________________________________________________

void HistPresent::DinA4Page(Int_t form)
{
	GrCanvas *c1 = NULL;
	TString name("page_");
	fPageNumber++;
	name += fPageNumber;

	TList *row_lab = new TList();
	static void *valp[25];
	Int_t ind = 0;
	Int_t Xlow, Ylow, Xwidth, Ywidth;

	Double_t xmin = 0;
	Double_t XRange = 0;
	Double_t ymin = 0;
	Double_t YRange = 0;
	if (form == 2) {          // user
		Xlow   = fEditUsXlow  ;
		Ylow   = fEditUsYlow  ;
		Xwidth = fEditUsXwidth;
		Ywidth = fEditUsYwidth;
		XRange = fEditUsXRange;

	} else if (form == 1) {          // landscape
		Xlow   = fEditLsXlow  ;
		Ylow   = fEditLsYlow  ;
		Xwidth = fEditLsXwidth;
		Ywidth = fEditLsYwidth;
		XRange = fEditLsXRange;
	} else if (form == 0) {   // portrait
		Xlow   = fEditPoXlow  ;
		Ylow   = fEditPoYlow  ;
		Xwidth = fEditPoXwidth;
		Ywidth = fEditPoYwidth;
		XRange = fEditPoXRange;
	}
	row_lab->Add(new TObjString("StringValue_Name of page"));
	row_lab->Add(new TObjString("PlainIntVal_  X Position"));
	row_lab->Add(new TObjString("PlainIntVal_  Y Position"));
	row_lab->Add(new TObjString("PlainIntVal_  X Width   "));
	row_lab->Add(new TObjString("PlainIntVal_  Y Width   "));
	row_lab->Add(new TObjString("DoubleValue_  X Range   "));
	valp[ind++] = &name;
	valp[ind++] = &Xlow;
	valp[ind++] = &Ylow;
	valp[ind++] = &Xwidth;
	valp[ind++] = &Ywidth;
	valp[ind++] = &XRange;
	Bool_t ok;
	Int_t itemwidth = 240;
	ok = GetStringExt("Canvas parameters", NULL, itemwidth, fRootCanvas,
							 NULL, NULL, row_lab, valp);
	if (!ok) return;

	if (form == 2) {          // user
		fEditUsXlow   = Xlow  ;
		fEditUsYlow   = Ylow  ;
		fEditUsXwidth = Xwidth;
		fEditUsYwidth = Ywidth;
		fEditUsXRange = XRange;

	} else if (form == 1) {          // landscape
		fEditLsXlow   = Xlow  ;
		fEditLsYlow   = Ylow  ;
		fEditLsXwidth = Xwidth;
		fEditLsYwidth = Ywidth;
		fEditLsXRange = XRange;
	} else if (form == 0) {   // portrait
		fEditPoXlow   = Xlow  ;
		fEditPoYlow   = Ylow  ;
		fEditPoXwidth = Xwidth;
		fEditPoYwidth = Ywidth;
		fEditPoXRange = XRange;
	}

	c1= new GrCanvas(name.Data(), name.Data(),
						Xlow, Ylow, Xwidth, Ywidth);

	//compute the pad range to get a fix aspect ratio
	Double_t w = gPad->GetWw()*gPad->GetAbsWNDC();
	Double_t h = gPad->GetWh()*gPad->GetAbsHNDC();
	YRange = XRange*h/w;

//   cout << "XRange, YRange " << XRange << " " << YRange << endl;
//   c1->SetFixedAspectRatio();
	c1->Range(xmin,ymin,XRange,YRange);

//   GetCanvasList()->Add(c1);
	c1->SetRightMargin(0);
	c1->SetLeftMargin(0);
	c1->SetBottomMargin(0);
	c1->SetTopMargin(0);
//   c1->SetEditGrid(5, 5, 10, 10);
	c1->Modified(kTRUE);
	c1->Update();
	c1->SetEditable(kTRUE);
	c1->SetBit(GrCanvas::kIsAEditorPage);
//   c1->GetCanvasImp()->ShowEditor();
//   c1->GetCanvasImp()->ShowToolBar();
//   new GEdit(c1);
	new GEdit(c1);
	c1->GetCanvasImp()->ShowEditor();
}
//________________________________________________________________________________________
// Show Canvas

void HistPresent::ShowCanvas(const char* fname, const char* dir, const char* name, const char* /*bp*/)
{
//	static Int_t seqnr = 0;
	if (gHprDebug > 0)
		cout << "ShowCanvas: " << fname << " " << dir << " " << name << endl;
	TString sname(name);
	HTCanvas *c = NULL;
	if (strstr(fname,".root")) {
		if (fRootFile) fRootFile->Close();
		fRootFile=new TFile(fname);
		if (strlen(dir) > 0) fRootFile->cd(dir);
		c = (HTCanvas*)gDirectory->Get(sname);
		c->SetName(sname);
		
	} else {
		c=(HTCanvas*)gROOT->GetListOfCanvases()->FindObject(sname);
	}
	gDirectory = gROOT;
	if (!c)  return;
	UInt_t ww;
	UInt_t wh;

	if (!c->TestBit(GrCanvas::kIsAEditorPage)) {
//      c->Draw();
		ww = c->GetWindowWidth();
		wh = c->GetWindowHeight();
		TList * logr = new TList();
		TList * lohi = new TList();
		c->Draw();
		c->SetWindowPosition(WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury);
		WindowSizeDialog::fWincurx += WindowSizeDialog::fWinshiftx;
		WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;
		WindowSizeDialog::fNwindows++;
		c->cd();
		Int_t ngr = FindGraphs(gPad, logr);
		
		if (ngr > 0) {
			gStyle->SetOptStat(0);
/*			TObject *objg;
			TIter nextgr(c->GetListOfPrimitives());
			while ( (objg = nextgr()) ) {
				if ( objg->InheritsFrom("TGraph") ) {
					TString opt=((TGraph*)objg)->GetDrawOption();
					cout << "opt: " << opt << endl;
//					if( opt.Length() <=0 )
						((TGraph*)objg)->SetDrawOption("AL");
				}
			}*/
		}
		if ( c->GetListOfExecs() )
			c->GetListOfExecs()->Clear();
		if (c->GetAutoExec())
			c->ToggleAutoExec();
		if ( c->InheritsFrom("HTCanvas") ) {
//	   	c->SetCanvasSize(c->GetOrigWw(), c->GetOrigWh());
			if (ngr > 0) {
				c->BuildHprMenus(this,0, (TGraph*)logr->First());
			} else {
//				Int_t nhi = FindObjs(gPad, logr, NULL, "TH1");
//				if (nhi > 0) {
					c->BuildHprMenus(this,NULL, NULL);
//				}
			}
		}
//		c->SetWindowSize(c->GetOrigWw(), c->GetOrigWh());
//		c->SetWindowSize(ww, wh);
		c->Modified();
		c->Update();
		logr->Clear("nodelete");
		lohi->Clear("nodelete");
		delete logr;
		delete lohi;
		return;
	} else {
		cout << "kIsAEditorPage " << endl;
		GrCanvas *gc = (GrCanvas*)c;
		gc->SetRightMargin(0);
		gc->SetLeftMargin(0);
		gc->SetBottomMargin(0);
		gc->SetTopMargin(0);
		gc->Draw();
		gPad = (TVirtualPad*)gc;
		gc->GetCanvasImp()->ShowEditor();
//   c1->GetCanvasImp()->ShowToolBar();
		gc->Update();
		gc->SetEditable(kTRUE);
//		if (fRootFile) fRootFile->Close();
		new GEdit(gc);
		gc->GetCanvasImp()->ForceUpdate();
		gSystem->ProcessEvents();
		gc->cd();
		return;
	}

	TString new_name(c->GetName());
	if (new_name == "dina4page") {
		new_name = "page_";
		fPageNumber ++;
		new_name += fPageNumber;
		c->SetBit(HTCanvas::kIsAEditorPage);
	}
//   c->SetBit(HTCanvas::kIsAEditorPage);
	HTCanvas* oldc = (HTCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
	if (oldc) {
		cout << new_name << " already exists, please save und delete" << endl;
		if (fRootFile) fRootFile->Close();
		return;
//         gROOT->GetListOfCanvases()->Remove(oldc);
//         delete oldc;
	}
	gROOT->ForceStyle(kFALSE);
	if (!c->TestBit(HTCanvas::kIsAEditorPage)) {
		c->Draw();
		TList *lofe = c->GetListOfExecs();
		if (lofe) {
//         lofe->ls();
			lofe->Clear();
		}
		c->BuildHprMenus(this, NULL, NULL);
		TIter next(c->GetListOfPrimitives());
		while (TObject * obj = next()) {
			if (obj->InheritsFrom("TH1")) {
				TH1*h = (TH1*)obj;
				obj->SetBit(kCanDelete);
				if (h->GetDimension() == 1) {
					new AddFitMenus(c, h);
					break;
				}
			}
		}
//      cout << "not kIsAEditorPage " << endl;
		c->cd();
		return;
	}
	TString tempname(c->GetName());
	c->SetName("abcxyz");
	ww = c->GetWw() + 2 * (gStyle->GetCanvasBorderSize() + 1);
	wh =  c->GetWindowHeight();
	wh =  c->GetWh();
//   cout << "ww, wh: " << ww << " " << wh << endl;
//   sname(name);
//   Int_t sem= sname.Last(';');    // chop off verion
//   if(sem >0)sname.Remove(sem);

	HTCanvas * c1 = new HTCanvas(sname, c->GetTitle(),
							 c->GetWindowTopX(), c->GetWindowTopY(),
							 ww, wh, this, NULL, NULL, HTCanvas::kIsAEditorPage);
	cout << "HTCanvas *c1 = new (HTCanvas*)"<< c1 << endl;

	Double_t x1, y1, x2, y2;
	c->GetRange(x1, y1, x2, y2);
	c1->Range(x1, y1, x2, y2);

//      cout << c->GetWindowWidth() << " " << c->GetWindowHeight() << " "
//           << c->GetWw() << " " << c->GetWh() << " "
//           << x1 << " "<<  y1 << " "<<  x2 << " " << y2 << endl;

//   GetCanvasList()->Add(c1);
	TObject *obj;
	TObjOptLink *lnk = (TObjOptLink*)c->GetListOfPrimitives()->FirstLink();
	TH1* h;
	TList lpads;
	while (lnk) {
		obj = lnk->GetObject();
		if (obj->InheritsFrom(TH1::Class())) {
			h = (TH1*)obj;
			h->SetDirectory(gROOT);
			h->Draw();
		} else {
			obj->ResetBit(kCanDelete);

			if (obj->InheritsFrom("TPad")){
				lpads.Add(obj);
			} else {
//             cout << "At HistPresent::ShowCanvas -= "
//               << obj->GetName()  << endl;
//            obj->Dump();
				obj->SetDrawOption(obj->GetOption());
				obj->Draw(lnk->GetOption());
//            obj->Dump();
			}
		}
		lnk = (TObjOptLink*)lnk->Next();

	}
	if (lpads.GetSize() > 0) {
		TIter next(&lpads);
		TPad * pad;
		while ( (pad = (TPad*)next()) ) {
			pad->Draw();
		}
	}

	delete c;
	c1->SetRightMargin(0);
	c1->SetLeftMargin(0);
	c1->SetBottomMargin(0);
	c1->SetTopMargin(0);
//   c1->SetEditGrid(5, 5, 10, 10);
	c1->Modified(kTRUE);
	c1->Update();
	c1->SetEditable(kTRUE);
	c1->GetCanvasImp()->ShowEditor();
//   c1->GetCanvasImp()->ShowToolBar();
	if (fRootFile) fRootFile->Close();
	c1->cd();
	new GEdit(c1);
	c1->cd();
	c1->Modified(kTRUE);
	c1->Update();
	GEdit::RestoreAxisAtts(c1);
	cout << "gPad " << gPad << endl;
}
//________________________________________________________________

void HistPresent::SelectdCache()
{
	TGFileInfo* fi = new TGFileInfo();
	const char * filter[] = {"dCache FileList", "*", 0, 0};
	fi->fFileTypes = filter;
	new  TGFileDialog(gClient->GetRoot(), fRootCanvas, kFDOpen, fi);
//	cout << "SelectdCache file list() " <<fi->fFilename << endl;
	ifstream infile;
	infile.open(fi->fFilename, ios::in);
	if (!infile.good()) {
		cerr	<< "SelectdCache: "
		<< gSystem->GetError() << " - " << fi->fFilename
		<< endl;
		return;
	}
	TString fname;
	while ( 1 ) {
		fname.ReadLine(infile);
		if (infile.eof()) break;
// assume lines with no " to be plain filenames
// extract filenames from lines like: aaa->Add("fname.root.1")

		Int_t ind_dq = fname.Index("\"");
		if ( ind_dq >= 0 ) {
			Int_t ind_Add = fname.Index("Add(");
			if ( ind_Add < 0 || ind_Add > ind_dq )
				continue;
			fname = fname(ind_dq + 1, fname.Length());
			ind_dq = fname.Index("\"");
			if ( ind_dq < 0 )
				continue;
			fname = fname(0, ind_dq);
		}
		TString cmd = "gHpr->Show";
		cmd = cmd + "Contents(\"" + fname + "\", \"\" )";

		TString nam=fname;
		TString tit;
		TString sel;
		fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel));
//		cout << cmd << endl;
	}
	if ( fCmdLine->GetSize() <= 0 )
		return;
	TObject *obj =  fHistLists->FindObject(fname);
	if (obj) {
		//         cout << "Delete canvas: " << fname << endl;
		fHistLists->Remove(obj);
		delete obj;
	}
	Int_t yoff = (Int_t)(WindowSizeDialog::fMainWidth * 1.6 + 30);
	TString cmd_title(fi->fFilename);
//	if (dir && strlen(dir) > 0) cmd_title = cmd_title + "_" + dir;
	HTCanvas *ccont = CommandPanel(cmd_title.Data(), fCmdLine,
		5, yoff, this, WindowSizeDialog::fWinwidx_hlist);
		ccont->SetName("dCFileList");
//         cout << "HistPresent: CommandPanel: " <<ccont->GetName() << " " << ccont->GetTitle() << endl;

	if (fHistLists)fHistLists->Add(ccont);
	fCmdLine->Delete();
	delete fi;
}
//________________________________________________________________

void HistPresent::SelectFromOtherDir()
{
	TGFileInfo* fi = new TGFileInfo();
	const char * filter[] = {"Root files", "*.root", 0, 0};
	fi->fFileTypes = filter;
	new  TGFileDialog(gClient->GetRoot(), fRootCanvas, kFDOpen, fi);
	cout << "SelectFromOtherDir() " <<fi->fFilename << endl;
	if (fi->fFilename) ShowContents(fi->fFilename , "", NULL);
	delete fi;
}
//________________________________________________________________

Int_t HistPresent::GetWindowPosition(Int_t * winx, Int_t * winy)
{
	if (WindowSizeDialog::fNwindows>0) {       // not the 1. time
		if (WindowSizeDialog::fWinshiftx != 0 && WindowSizeDialog::fNwindows%2 != 0) WindowSizeDialog::fWincurx += WindowSizeDialog::fWinshiftx;
		else   {WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx; WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;}
	}
	WindowSizeDialog::fNwindows++;
	*winx = WindowSizeDialog::fWincurx;
	*winy = WindowSizeDialog::fWincury;
	return WindowSizeDialog::fNwindows;
}

//________________________________________________________________

void HistPresent::HandleRemoveAllCuts()
{
	HTCanvas *c;
	TButton *b;
	TObject *obj;
	TIter next(fHistLists);
	TIter *next1;
	TString space(" ");
	TString line;
	while ( (c = (HTCanvas*)next()) ) {
		next1 = new TIter(c->GetListOfPrimitives());
		while ( (obj = (*next1)()) ) {
			if (obj->InheritsFrom("TButton")) {
				b = (TButton*)obj;
				line =  b->GetMethod();
//             cout << "HandleDeleteCanvas " << line << endl;
				if (line.Contains("LoadWindow") && b->TestBit(kSelected)) {
					b->SetFillColor(17);
					b->Modified(kTRUE);
					b->Update();
				}
			}
		}
		delete next1;
	}
}
//________________________________________________________________

void HistPresent::HandleDeleteCanvas( HTCanvas *htc)
{
//   cout << "enter HandleDeleteCanvas " << htc << endl;
	if (gHprClosing > 0 ) {
		if (gHprDebug > 0 ) {
			 cout << "HandleDeleteCanvas called during CloseDown" << endl;
		}
		return;
	}
	GetHistList()->Remove(htc);
	HTCanvas *c;
	TButton *b;
	TString fname;
	TObject *obj;
	TString line;
//  has it a histogram?
	FitHist * fh = htc->GetFitHist();
	if (fh) {
//  reset color of command button which invoked the Canvas
		TH1* hh = fh->GetSelHist();
		if (gHprDebug > 1)
			cout << "HandleDeleteCanvas: fh->GetSelHist() " << hh << endl;
		if (!hh || !hh->TestBit(TObject::kNotDeleted)) return;
		TString histname(hh->GetName());
		// care for renamed histos 
		TRegexp supi("_has_supimp");
		histname(supi) = "";
// does it end with a _number
		TRegexp us_num("_[0-9]*$");
		Int_t indus = histname.Index(us_num);
		if (indus > 1) histname.Resize(indus);
		if (gHprDebug > 1)
			cout << "HandleDeleteCanvas hname: " <<  hh->GetName()<< " histname: " 
			<< histname << endl;
		TIter next(fHistLists);
		TIter *next1;
		TString hname;
		TString space(" ");

		TObjArray * oa;
		while ( (c = (HTCanvas*)next()) ) {
			next1 = new TIter(c->GetListOfPrimitives());
			fname = c->GetTitle();
			Int_t inddot = fname.Index(".");
			if (inddot > 0) fname.Resize(inddot); // chop of .root etc.
			while ( (obj = (*next1)()) ) {
				if (obj->InheritsFrom("TButton")) {
					b = (TButton*)obj;
//               cout << b->GetTitle() << endl;
//             detangle title line, 2nd item is hist name
					line =  b->GetTitle();
					oa = line.Tokenize(space);
					if (oa->GetEntries() < 2) continue;
					hname = ((TObjString*)oa->At(1))->String();
					if (GeneralAttDialog::fPrependFilenameToName) {
						hname.Prepend("_");
						hname.Prepend(fname);
					}
					if (histname == hname) {
						if (gHprDebug>1)
							cout << "Set color for: " << histname << endl;
						b->SetFillColor(17);
						b->Modified(kTRUE);
						b->Update();
					}
				}
			}
			delete next1;
		}
//		cout << "exit HandleDeleteCanvas FitHist" << htc << endl;
		return;
	}
//  is it a list of objects, look in filelist
//   TString name = htc->GetName();
	TString title = htc->GetTitle();
	if (title.EndsWith(".root") && fFileList != NULL) {
		if (gHprDebug > 1)
      cout << "|" << htc->GetTitle()<< "|" << endl;
		TIter next(fFileList->GetListOfPrimitives());
		while ( (obj = next()) ) {
			if (obj->InheritsFrom("TButton")) {
				b = (TButton*)obj;
//            cout << "|" << b->GetTitle()<< "|" << endl;
				line =  b->GetTitle();
				line = line.Strip();
				if (line == title) {
//                  cout << "Set color for: " << fname << endl;
					b->SetFillColor(17);
					b->Modified(kTRUE);
					b->Update();
				}
			}
		}
	}
//   cout << "exit HandleDeleteCanvas " << htc << endl;
}
//________________________________________________________________________________________
// Show a graph

void HistPresent::ShowGraph(const char* fname, const char* dir, const char* name, const char* bp)
{
	TGraph     * graph1d = NULL;
	TGraph2D   * graph2d = NULL;
	TObject    * obj;
	if (gHprDebug > 0 ) {
		cout << "ShowGraph:fname " << fname <<" dir " << dir << endl;
	}
	if (strstr(fname,".root")) {
		if (fRootFile) fRootFile->Close();
		fRootFile = new TFile(fname);
		if (strlen(dir) > 0) fRootFile->cd(dir);
		obj = gDirectory->Get(name);
		//      fRootFile->Close();
	} else {
		obj = gROOT->GetList()->FindObject(name);
	}
	if (!obj)
		return;
	if ( obj->InheritsFrom("TGraph2D") ) {
		graph2d = (TGraph2D*)obj;
		graph2d->SetDirectory(gROOT);
	} else if ( obj->InheritsFrom("TGraph") ) {
		graph1d = (TGraph*)obj;
	} else {
		cout << "Graph not found" << endl;
		return;
	}
	if ( GeneralAttDialog::fPrependFilenameToTitle != 0 ) {
		TString title = ((TNamed*)obj)->GetTitle();
		TString fn(fname);
		fn = gSystem->BaseName(fn);
		Int_t pp = fn.Index(".");
		fn.Resize(pp);
		title.Prepend("_");
		title.Prepend(fn);
		((TNamed*)obj)->SetTitle(title);
	}
	TString cname = name;
	if (cname.Index(";") > 1)cname.Resize(cname.Index(";"));
	// check name for non allowed chars
	TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
	while (cname.Index(notascii) >= 0) {
		cname(notascii) = "_";
	}
	TString gname = cname;
	gname.Append("_0");
//	gname.Prepend("Graph_");
	cname.Prepend("C_");
	cname += "_";
	cname += fSeqNumberGraph++;

	if (WindowSizeDialog::fNwindows>0) {       // not the 1. time
		//		if (WindowSizeDialog::fWinshiftx != 0 && WindowSizeDialog::fNwindows%2 != 0) {
		WindowSizeDialog::fWincurx += WindowSizeDialog::fWinshiftx;
		//		} else {
			//         WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
			WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;
			//      }
	}
	WindowSizeDialog::fNwindows++;
	TEnv env(".hprrc");

	//      graph->SetName(hname);
	//      graph->SetTitle(htitle);
	if ( graph2d ) {
		graph2d->SetName(gname);
		TGraph2D * grnew = graph2d;
		TString opt2d = env.GetValue("Set2DimGraphDialog.fDrawOpt2Dim", "TRI2");
		Int_t nuse = env.GetValue("Set2DimGraphDialog.fUseNPoints", 1000);
		Int_t randomize = env.GetValue("Set2DimGraphDialog.fRandomizePoints", 0);
		Int_t np = graph2d->GetN();
		if (nuse > np)
			nuse = np;
		cout << setgreen << "HistPresent, Opt2d: " << opt2d
		<< " number of points: " << np
		<< " for display use: " << nuse << setblack << endl;
		opt2d.Strip(TString::kBoth);
		if (opt2d == "GL" || opt2d.Length() == 0) {
			opt2d += "TRI2";
			cout << setblue << "HistPresent, resetting Opt2d: " << opt2d << setblack<< endl;
		}
		if ( nuse > 0 ) {
			grnew = new TGraph2D(nuse);
			grnew->SetDirectory(gROOT);
			np = Hpr::MixPointsInGraph2D(graph2d, grnew, nuse);
//			grnew->Dump();
			TString nname(gname);
			nname += "_rdm";
			grnew->SetName(nname);
			grnew->SetTitle(graph2d->GetTitle());
			if (gHprDebug > 2) {
				Double_t *xx = grnew->GetX();
				Double_t *yy = grnew->GetY();
				Double_t *zz = grnew->GetZ();
				for (Int_t i=0; i < np; i++) {
//					if (TMath::Abs(zz[i]) < 0.00001)
					cout <<">>> "<< i <<" "<<xx[i]<<" "<<yy[i]<<" " << zz[i]<< endl;
				}
				TFile * ft = new TFile("temp.root", "RECREATE");
				grnew->Write();
				ft->Close();
			}
			if (np < nuse) {
				cout << setred << "Something wrong nuse: " << nuse
						<< " np " << endl;
				if (np < 100)
					return;
			}
		} else {
			nuse = np;
		}
		TRegexp sa("SAME");
		opt2d(sa) = "";
		if ( nuse <= 0 ) 
			nuse = np;
		Double_t texpected = -3 + 0.0024 * nuse + 2.30782e-06 * nuse*nuse;
//		Double_t texpected = 4 * TMath::Power(3, graph2d->GetN() / 900.) - 10;
		if (texpected > 1) {
			cout << setmagenta << "Warning rendering may take: " << texpected
			<< " Seconds" << setblack << endl;
		}
		if (opt2d.Contains("GL")) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}

		HTCanvas * htc =
		new HTCanvas(cname, cname, WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
								WindowSizeDialog::fWinwidx_1dim, WindowSizeDialog::fWinwidy_1dim, this, 0, (TGraph*)graph2d);
		gBenchmark->Start("graph2d");
//		graph2d->SetMaxIter(3000);
//		htc->cd();
		if (randomize > 0) {
			Double_t *zv = grnew->GetZ();
			for (Int_t ii=0; ii<grnew->GetN(); ii++){
				zv[ii] += 0.0001*(gRandom->Rndm()-0.5);
			}
		}
  		grnew->Draw(opt2d);
		TButton * b = NULL;
		if (bp) {
			b = (TButton *)strtoul(bp, 0, 16);
		}
		htc->SetCmdButton(b);
	} else {
		graph1d->SetName(gname);
		new HTCanvas(cname, cname, WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
		WindowSizeDialog::fWinwidx_1dim, WindowSizeDialog::fWinwidy_1dim, this, 0, graph1d);
		TH1* hh = (TH1*)gROOT->GetList()->FindObject("hist_for_graph");
		if (hh) {
			cout << "Use Predefined Hist " << hh<< endl;
			TAxis *xa = graph1d->GetHistogram()->GetXaxis();
			TAxis *ya = graph1d->GetHistogram()->GetYaxis();

			TString tx;
			TString ty;

			if ( xa ) {
				tx = xa->GetTitle();
			}
			if ( ya ) {
				ty = ya->GetTitle();
			}
			hh->SetTitle(graph1d->GetTitle());
			TString nn(graph1d->GetName());
			nn += "_hist_for_graph";
			hh->SetName(nn);
			graph1d->SetHistogram((TH1F*)hh);
			if ( xa ) {
				TAxis * xan = graph1d->GetHistogram()->GetXaxis();
				xan->SetTitle(tx);
				if ( xa->TestBit(TAxis::kCenterTitle) )
					xan->SetBit(TAxis::kCenterTitle);
			}
			if ( ya ) {
				TAxis * yan = graph1d->GetHistogram()->GetYaxis();
				yan->SetTitle(ty);
				if ( ya->TestBit(TAxis::kCenterTitle) )
					yan->SetBit(TAxis::kCenterTitle);
			}
		}
		TString drawopt("");
		if (gROOT->GetForceStyle()) {
			drawopt = env.GetValue("GraphAttDialog.fDrawOpt", "PA");
			if (drawopt.Length() == 0 || drawopt == " ")
				drawopt = "PA";
		}
//		cout << "graph1d->Draw " << drawopt<< " " << graph1d->GetName()<< " "
//		<< graph1d->GetHistogram()->GetName()<< endl;
		graph1d->Draw(drawopt);
		// if xaxis looks like unix time (after year 2000)set time display
		Int_t year2000 = 339869696;
		if ((Int_t)graph1d->GetXaxis()->GetXmin() > year2000 &&
			 (Int_t)graph1d->GetXaxis()->GetXmax() > year2000) {
			graph1d->GetXaxis()->SetTimeDisplay(1);
		}
		
		if (gROOT->GetForceStyle()) {
			graph1d->SetLineStyle  (env.GetValue("GraphAttDialog.fLineStyle",  1));
			graph1d->SetLineWidth  (env.GetValue("GraphAttDialog.fLineWidth",  1));
			graph1d->SetLineColor  (env.GetValue("GraphAttDialog.fLineColor",  1));
			graph1d->SetMarkerStyle(env.GetValue("GraphAttDialog.fMarkerStyle",7));
			graph1d->SetMarkerSize (env.GetValue("GraphAttDialog.fMarkerSize", 1.));
			graph1d->SetMarkerColor(env.GetValue("GraphAttDialog.fMarkerColor",1));
			graph1d->SetFillStyle  (env.GetValue("GraphAttDialog.fFillStyle",  0));
			if ( drawopt.Contains("F") )
				graph1d->SetFillColor  (env.GetValue("GraphAttDialog.fFillColor",  1));
			else
				graph1d->SetFillColor  (0);
		}
		TH1 * hist = graph1d->GetHistogram();
		if ( hist ) {
			TString hn = hist->GetName();
			if (!hn.Contains("Graph")) {
				hn.Prepend("Graph_");
				hist->SetName(hn);
			}
			hist->SetStats(kFALSE);
			if ( env.GetValue("SetHistOptDialog.fTitleCenterY", 0) == 1 )
				graph1d->GetHistogram()->GetYaxis()->SetBit(TAxis::kCenterTitle);
			if ( env.GetValue("SetHistOptDialog.fTitleCenterX", 0) == 1 )
				graph1d->GetHistogram()->GetXaxis()->SetBit(TAxis::kCenterTitle);
		}
	}
	gPad->SetLogy(env.GetValue("GraphAttDialog.fLogY",  0));
	gPad->SetLogx(env.GetValue("GraphAttDialog.fLogX",  0));
	if (fRootFile) fRootFile->Close();
	//	GraphAttDialog::SetGraphAtt(cg);
	gPad->Modified();
	gPad->Update();
  	gBenchmark->Show("graph2d");
  	gBenchmark->Reset();
}
//____________________________________________________________

void HistPresent::SuperimposeGraph(TCanvas * current, Int_t mode)
{
	static const char helptext[] =
	"\n\
	A selected graph is drawn in the same pad\n\
	The graph may be scaled automatically. In this case\n\
	the scale is adjusted such that the maximum value\n\
	is 10% below the maximum of the pad. Any value may be\n\
	selected manually.\n\
	An extra axis may be drawn on the right side.\n\
	The AxOffs determines its position, 0 is at\n\
	the right edge of the frame, negative is left\n\
	of it inside the frame, positive is outside on the right\n\
	\n\
	";
//	Color_t scale_color[kMaxColors] = {kRed, kGreen, kYellow, kBlue, kMagenta};

	current->cd();
	TH1 * hist = NULL;
	hist = GetHistOfGraph(gPad);
	if ( !hist && current ) {
		hist = GetHistOfGraph(current);
		if ( !hist ) {
			cout << "No graph in active pad" << endl;
			return;
		} else {
			current->cd();
		}
	}
	if (fSelectGraph == NULL || fSelectGraph->GetSize() != 1) {
		cout << "Please select exactly  one graph" << endl;
		return;
	}
	Int_t ngr_exist = 0;
	TObject * obj  = GetSelGraphAt(0);
	if (!obj ||!obj->InheritsFrom("TGraph") )
		return;
	TGraph * gr = (TGraph*)obj;
	TEnv env(".hprrc");
	Double_t xmin, xmax, ymin, ymax;
	Double_t hxmin, hxmax, hymin, hymax;
	gr->ComputeRange(xmin, ymin, xmax, ymax);
	hxmin = gPad->GetUxmin();
	hxmax = gPad->GetUxmax();
	hymin = gPad->GetUymin();
	hymax = gPad->GetUymax();
	if ( mode == 0 && (ymin < hymin || ymax > hymax) ) {
		cout << "Warning: Graph: " << ymin << " " << ymax <<
		" does not fit in Yrange: " << hymin << " " << hymax  << endl;
	}
	if ( xmin < hxmin || xmax > hxmax ) {
		cout << "Warning: Graph: << " << xmin << " " << xmax <<
		" does not fit in Xrange: "<< hxmin << " " << hxmax << endl;
	}
	//
	Double_t rightmax = 1.1 * ymax;
	Int_t        do_scale = 0;
	Int_t      auto_scale = 0;
	Int_t        new_axis = 0;
	if ( mode == 1 ) {
		do_scale = auto_scale = new_axis = 1;
	}
	TGraph * gr_exist = FindGraph(current, &ngr_exist);
	if ( !gr_exist )
		return;
	TString lLineMode;
//	TString drawopt     = env.GetValue("GraphAttDialog.fDrawOpt", "PA");
	TString drawopt     = gr_exist->GetDrawOption();
	if ( drawopt.Contains("C") ) {
		lLineMode = "C(smooth)";
	} else if (drawopt.Contains("L")) {
		lLineMode = "L(simple)";
	} else {
		lLineMode = "(noline)";
	}
	Int_t lShowMarkers = 0;
	if (drawopt.Contains("P", TString::kIgnoreCase)) {
		lShowMarkers= 1;
	}
/*	Style_t lFStyle   = env.GetValue("GraphAttDialog.fFillStyle", 0);
	Int_t   lFill     = env.GetValue("GraphAttDialog.fFill", 0);
	Style_t lLStyle   = env.GetValue("GraphAttDialog.fLineStyle", 1);
	Size_t  lLWidth   = env.GetValue("GraphAttDialog.fLineWidth", 1);
	Style_t lMStyle   = env.GetValue("GraphAttDialog.fMarkerStyle", 7);
	Size_t  lMSize    = env.GetValue("GraphAttDialog.fMarkerSize",  1);*/

	Style_t lFStyle   = gr_exist->GetFillStyle();
	Style_t lLStyle   = gr_exist->GetLineStyle();
	Size_t  lLWidth   = gr_exist->GetLineWidth();
	Style_t lMStyle   = gr_exist->GetMarkerStyle();
	Size_t  lMSize    = gr_exist->GetMarkerSize();
	
	Double_t  axis_offset = env.GetValue("SuperImposeGraph.axis_offset", 0.);
	
	Double_t label_offset = 0.01;
	Color_t def_col = gr_exist->GetMarkerColor();
//	Color_t def_col = 2;
// if (def_col == gr_exist->GetMarkerColor())
//		def_col = 3;
	Color_t lLColor    = def_col + 1;
	Color_t lMColor    = lLColor;
	Color_t lFColor    = lLColor;
	Color_t axis_color = lLColor;
	/*
	static Color_t lLColor    = env.GetValue("SuperImposeGraph.lLColor", def_col);
	static Color_t lMColor    = env.GetValue("SuperImposeGraph.lMColor", def_col);
	static Color_t lFColor    = env.GetValue("SuperImposeGraph.lFColor", def_col);
	static Color_t axis_color = env.GetValue("SuperImposeGraph.axis_color", def_col);
	*/
	static Int_t   lFill      = env.GetValue("SuperImposeGraph.fFill", 0);
	
	Double_t new_scale = 1;
	TString axis_title;
	Int_t   lLegend      = env.GetValue("SuperImposeGraph.DrawLegend", 1);
	Int_t   lIncrColors  = env.GetValue("SuperImposeGraph.AutoIncrColors", 1);
	Int_t   lSkipDialog  = env.GetValue("SuperImposeGraph.SkipDialog", 0);

	axis_title= gr->GetTitle();
	void *valp[50];
	Int_t ind = 0;
	TList *row_lab = new TList();
	TRootCanvas * win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
	Bool_t ok = kTRUE;
	row_lab->Add(new TObjString("CheckButton_New scale  "));
	valp[ind++] = &do_scale;
	row_lab->Add(new TObjString("CheckButton+Auto scale  "));
	valp[ind++] = &auto_scale;
	row_lab->Add(new TObjString("DoubleValue+Factor"));
	valp[ind++] = &new_scale;
	row_lab->Add(new TObjString("CheckButton_Extra axis "));
	valp[ind++] = &new_axis;
	row_lab->Add(new TObjString("DoubleValue+AxOffs "));
	valp[ind++] = &axis_offset;
	row_lab->Add(new TObjString("DoubleValue+LabOffs"));
	valp[ind++] = &label_offset;
	row_lab->Add(new TObjString("StringValue_AxTit"));
	valp[ind++] = &axis_title;
	row_lab->Add(new TObjString("ColorSelect+AxCol"));
	valp[ind++] = &axis_color;
	row_lab->Add(new TObjString("CheckButton+Incr Col"));
	valp[ind++] = &lIncrColors;
	row_lab->Add(new TObjString("ComboSelect_LineM;(noline);L(simple);C(smooth)"));
	valp[ind++] = &lLineMode;
	row_lab->Add(new TObjString("CheckButton+Draw Marker"));
	valp[ind++] = &lShowMarkers;
	row_lab->Add(new TObjString("CheckButton+Legend  "));
	valp[ind++] = &lLegend;
	row_lab->Add(new TObjString("Mark_Select_MarkStyle"));
	valp[ind++] = &lMStyle;
	row_lab->Add(new TObjString("Float_Value+MSize"));
	valp[ind++] = &lMSize;
	row_lab->Add(new TObjString("ColorSelect+MarkColor"));
	valp[ind++] = &lMColor;

	row_lab->Add(new TObjString("LineSSelect_LStyle"));
	valp[ind++] = &lLStyle;
	row_lab->Add(new TObjString("PlainShtVal+LineWidth"));
	valp[ind++] = &lLWidth;
	row_lab->Add(new TObjString("ColorSelect+LineColor"));
	valp[ind++] = &lLColor;
	row_lab->Add(new TObjString("CheckButton_DoFill"));
	valp[ind++] = &lFill;
	row_lab->Add(new TObjString("Fill_Select+FStyle"));
	valp[ind++] = &lLStyle;
	row_lab->Add(new TObjString("ColorSelect+FillColor"));
	valp[ind++] = &lFColor;

	Int_t itemwidth = 380;
	ok = GetStringExt("Superimpose Graph", NULL, itemwidth, win,
					NULL, NULL, row_lab, valp,
					NULL, NULL, helptext);
	if (!ok )
		return;
	if ( do_scale != 0 && auto_scale != 0 ) {
		new_scale = hymax / rightmax;
		cout << "Scale will be auto adjusted " << new_scale << endl;
	}
	TGraphErrors *gre = NULL;
	TGraphAsymmErrors *grase = NULL;
	if ( gr->InheritsFrom("TGraphAsymmErrors") ) {
		grase = (TGraphAsymmErrors*)gr;
	} else if ( gr->InheritsFrom("TGraphErrors") ) {
		gre = (TGraphErrors*)gr;
	}
	Double_t *x = gr->GetX();
	Double_t *y = gr->GetY();
	if ( do_scale != 0 ) {
		if ( TMath::Abs(new_scale) < 1.E-20 ) {
			cout << "Scale = 0! " << new_scale << endl;
			return;
		}
		for (Int_t i = 0; i < gr->GetN(); i++) {
			gr->SetPoint(i, x[i], new_scale * y[i]);
			if ( gre ) {
				gre->SetPointError(i, gre->GetErrorX(i), new_scale * gre->GetErrorY(i));
			} else if ( grase ) {
				grase->SetPointError(i, grase->GetErrorXlow(i), grase->GetErrorXhigh(i),
				new_scale * grase->GetErrorYlow(i), new_scale * grase->GetErrorYhigh(i));
			}
		}
	}
	// check name for non allowed chars
	TString gname=gr->GetName();
	TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
	while (gname.Index(notascii) >= 0) {
		gname(notascii) = "_";
	}
//	gname.Prepend("Graph_");
	gname += "_";
	gname += ngr_exist;
	gr->SetName(gname);
	gr->SetLineColor(axis_color);
	gr->SetMarkerColor(axis_color);
//	drawopt = env.GetValue("GraphAttDialog.fDrawOpt", "P");
	Int_t inda = drawopt.Index("A", 0, TString::kIgnoreCase);
	if (inda>=0) drawopt.Remove(inda,1);
	TString lm(lLineMode);
	if (lm.Index("(") >=0 )
		lm.Resize(lm.Index("("));
	drawopt += lm;
	if (lShowMarkers) drawopt += "P";
	if ( lFill ) drawopt += "F";
	//	 drawopt += "SAME";
	gr->Draw(drawopt);
	if ( lFill )
		gr->SetFillColor(lFColor);
	else
		gr->SetFillColor(0);
	gr->SetFillStyle(lFStyle);
	gr->SetLineColor(lLColor);
	gr->SetLineStyle(lLStyle);
	gr->SetLineWidth(lLWidth);

	gr->SetMarkerColor(lMColor);
	gr->SetMarkerStyle(lMStyle);
	gr->SetMarkerSize (lMSize );
	//draw an axis on the right side
	if ( do_scale != 0 ) {
		Double_t ledge = gPad->GetFrame()->GetY1();
		Double_t uedge = gPad->GetFrame()->GetY2();
		if ( gPad->GetLogy() ) {
			ledge = TMath::Power(10, ledge);
			uedge = TMath::Power(10, uedge);
		}
		ledge /= new_scale;
		uedge /= new_scale;
		HprGaxis *axis = Hpr::DoAddAxis(current, hist, 2, ledge, uedge, axis_offset, axis_color);
//		TString opt("+SL");
//		TGaxis *axis = new TGaxis(
//		gPad->GetUxmax()+axis_offset*(gPad->GetUxmax()-gPad->GetUxmin()), gPad->GetUymin(),
//		gPad->GetUxmax()+axis_offset*(gPad->GetUxmax()-gPad->GetUxmin()), gPad->GetUymax(),
//		gPad->GetUymin() / new_scale ,gPad->GetUymax() / new_scale ,510, opt);

		TString ax_name("axis_");
		ax_name += gr->GetTitle();
		axis->SetName(ax_name);
		axis->SetLineColor(axis_color);
		axis->SetLabelColor(axis_color);
		axis->SetTickSize   (env.GetValue("SetHistOptDialog.fTickLength", 0.01));
		axis->SetLabelFont  (env.GetValue("SetHistOptDialog.fLabelFont", 62));
		axis->SetLabelOffset(label_offset);
		axis->SetLabelOffset(env.GetValue("SetHistOptDialog.fLabelOffsetY", 0.01));
		axis->SetLabelSize  (env.GetValue("SetHistOptDialog.fLabelSize", 0.03));
		axis->SetMaxDigits  (env.GetValue("SetHistOptDialog.fLabelMaxDigits", 4));
		if (axis_title.Length() > 0) {
			axis->SetTitle(axis_title);
			axis->SetTitleColor(axis_color);
			axis->SetTitleFont( env.GetValue("SetHistOptDialog.fTitleFont", 62));
			axis->SetTitleSize( env.GetValue("SetHistOptDialog.fTitleSize",0.03));
			axis->SetTitleOffset( 1);
			if ( env.GetValue("SetHistOptDialog.fTitleCenterY", 0) == 1 )
				axis->CenterTitle();
		}
	}
	if ( lLegend != 0 ) {
		// remove possible TLegend
 		TIter next2( current->GetListOfPrimitives() );
//		TObject *obj;
		TLegend * leg = NULL;
		while ( obj = next2() ) {
			if ( obj->InheritsFrom("TLegend") ) {
				leg = (TLegend*)obj;
				break;
			}
		}
		if ( leg )
			delete leg;
//		TEnv env(".hprrc");
		Double_t x1 = env.GetValue("SuperImposeGraph.fLegendX1", 0.11);
		Double_t x2 = env.GetValue("SuperImposeGraph.fLegendX2", 0.3);
		Double_t y1 = env.GetValue("SuperImposeGraph.fLegendY1", 0.8);
		Double_t y2 = env.GetValue("SuperImposeGraph.fLegendY2", 0.95);

		leg = current->BuildLegend(x1, y1, x2,y2);
		leg->SetName("Legend_SuperImposeGraph");
		leg->SetFillColor (env.GetValue("HprLegend.fFillColor", 0));
		leg->SetFillStyle (env.GetValue("HprLegend.fFillStyle", 0));
		leg->SetLineColor (env.GetValue("HprLegend.fLineColor", 1));
		leg->SetBorderSize(env.GetValue("HprLegend.fBorderSize",1));
		leg->SetTextColor (env.GetValue("HprLegend.fTextColor", 1));
		leg->SetTextFont  (env.GetValue("HprLegend.fTextFont", 62));
		// remove entry for extra axis
		TIter next( leg->GetListOfPrimitives() );
		TList * tmp = new TList();
		TLegendEntry *le;
		while ( le = (TLegendEntry*)next() ) {
			if ( le->GetObject()->IsA() == HprGaxis::Class() )
				tmp->Add(le);
		}
		TIter next1(tmp);
		while ( le = (TLegendEntry*)next1() ) {
			leg->GetListOfPrimitives()->Remove(le);
		}
		delete tmp;
	}
	gPad->Modified();
	gPad->Update();
	if (lIncrColors > 0) {
//		lLColor++;
//		lMColor++;
//		axis_color++;
//		lFColor++;
		axis_offset += 0.05;
	}
	gHpr->ClearSelect();
	/*
	env.SetValue("SuperImposeGraph.lLColor", lLColor);
	env.SetValue("SuperImposeGraph.lMColor", lMColor);
	env.SetValue("SuperImposeGraph.lFColor", lFColor);
	env.SetValue("SuperImposeGraph.axis_color", axis_color);
	*/
	env.SetValue("SuperImposeGraph.lFill",   lFill);
	env.SetValue("SuperImposeGraph.axis_offset", axis_offset);
	env.SetValue("SuperImposeGraph.DrawLegend", lLegend);
	env.SetValue("SuperImposeGraph.AutoIncrColors", lIncrColors);
	env.SetValue("SuperImposeGraph.SkipDialog", lSkipDialog);
	env.SaveLevel(kEnvLocal);
}

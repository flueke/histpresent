#include "TROOT.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TH1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "Set1DimOptDialog.h"
#include "hprbase.h"
#include "TMrbHelpBrowser.h"
#include <iostream>

extern Int_t gHprDebug;

namespace std {} using namespace std;

//_______________________________________________________________________

Set1DimOptDialog::Set1DimOptDialog(Int_t /*batch*/)
{
//	cout << "ctor Set1DimOptDialog, non interactive" <<endl;
	fDialog = NULL;
}
//_______________________________________________________________________

Set1DimOptDialog::Set1DimOptDialog(TGWindow * win)
{
const Char_t *helptext=
"<!DOCTYPE HTML>\n\
<HTML>\n\
<TITLE>HelpSet1DimOpt</TITLE>\n\
This widget allows to modify the appearance of the currently\n\
displayed histogram.\n\
To make them default for subsequent histograms press: \n\
<b>Set as global default</b>\n\
 \n\
<b>Reset all to default</b> sets values back to factory defaults\n\
To make these permanent also needs <b>Set as global default</b>\n\
 \n\
A histogram can either been drawn as a line (<b>Contour</b>)\n\
or with markers. To fill a histogram <b>Contour</b> must\n\
be selected\n\
 \n\
Other options are <b>SmoothLine</b>, <b>SimpleLine</b> or <b>Text</b>\n\
In case of <b>Text</b> the size is control by <b>MSiz</b> (MarkerSize)\n\
 \n\
Error Drawing Modes:\n\
E  Draw error bars.\n\
E0 Draw error bars. Markers are drawn for bins with 0 contents.\n\
E1 Draw error bars with perpendicular lines at the edges.\n\
   Length is controled by EndErrSize.\n\
E2 Draw error bars with rectangles.\n\
E3 Draw a fill area through the end points of the\n\
   vertical error bars.\n\
E4 Draw a smoothed filled area through the end points\n\
	of the error bars.\n\
E5 Like E3 but ignore the bins with 0 contents.\n\
E6 Like E4 but ignore the bins with 0 contents.\n\
Options E3-E6: Choose: <b>Contour Off</b> and <b>FillHist On</b> to get area\n\
               filled between the error lines.\n\
 \n\
X ErrorSz controls drawing of error bars in X.\n\
A value of 0.5 draws a line X +- 0.5*BinWidth\n\
<b>EndErrorSz</b> controls the length of the perpendicular lines\n\
at the edges (option <b>E1</b>).\n\
 \n\
For more detailed information please consult <a href = http://root.cern/doc/master/classTHistPainter.html> ROOTs documention</a>\n\
<b>Min Y=Min Cont</b>: Set minimimum of Y scale to minumum of content.\n\
 \n\
X and Y scales can be set as default to logarithmic or linear.\n\
This can still be reset for individual histograms.\n\
In detail: If a canvas is closed and its lin-log state differs\n\
from the global default its state is stored and restored when\n\
the histogram is shown again.\n\
 \n\
If <b>Live stats</b> is active a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values (Current bin, bin content, mean, integral.\n\
Selecting <b>Live Gauss</b> fits a gaussian to the dragged region.\n\
A constant: <b>Const bg</b> or linear background (a+ bx): <b>Linear bg</b>\n\
may be selected.\n\
For this functionality the FillStyle of the underlying frame\n\
must be 0. This is controlled by the <b>Canvas, Pad, Frame</b>\n\
dialog invoked from the <b>GraphicsAtt</b> popup menu\n\
</HTML>\n\
";

	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	fHistList.SetName("1dim_histlist");
	fPadList.SetName("1dim_padlist");
	TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
	TH1* hist;
	fNHists = 0;
	fNPads = 0;
	TPad* actpad = (TPad*)fCanvas;
	while ( (obj = next()) ) {
		hist = NULL;
		if (obj->InheritsFrom("TH1")) {
			hist = (TH1*)obj;
		} else if ( obj->InheritsFrom("TPad") ) {
			hist = Hpr::FindHistInPad((TVirtualPad*)obj);
			actpad= (TPad*)obj;
			fNPads++;
		}
		if ( hist  && hist->GetDimension() == 1 ) {
			fNHists++;
			fHistList.Add(hist);
			fPadList.Add(actpad);
		}
	}
	if ( fNHists == 0 ) {
		cout << "No 1dim histogram in Canvas" << endl;
		return;
	}
	cout << "Set1DimOptDialog, fNHists " << fNHists 
			<< " fNPads " << fNPads << endl;
	RestoreDefaults();
//	GetValuesFromHist();
//	fLiveBG = 1;  //force lin bg
	fAdvanced1Dim = 1;

	fTitle       = new TString[fNHists];
	fTitleModBid = new Int_t[fNHists];
	fShowContour = new Int_t[fNHists];
	fFill        = new Int_t[fNHists];
	fFillColor   = new Color_t[fNHists];
	fFillStyle   = new Style_t[fNHists];
	fLineColor   = new Color_t[fNHists];
	fLineStyle   = new Style_t[fNHists];
	fLineWidth   = new Width_t[fNHists];
	fMarkerColor = new Color_t[fNHists];
	fMarkerStyle = new Style_t[fNHists];
	fMarkerSize  = new Size_t[fNHists];
	fShowMarkers = new Int_t[fNHists];
	fErrorMode   = new TString[fNHists];
	fSame        = new Int_t[fNHists];
//   static Int_t dummy;
	static TString stycmd("SetHistAttPermLocal()");
	static TString sadcmd("SetAllToDefault()");
//   static TString helpcmd("HelpBr()");

	fRow_lab = new TList();
	Int_t ind = 0;
	TString opt, erm, errmode, title;
	TRegexp semicolon(";");
	fCanvas->cd();
	for ( Int_t i =0; i < fNHists; i++ ) {
		hist =(TH1*)fHistList.At(i);
		((TPad*)fPadList.At(i))->cd();
		fOneDimLogX        = gPad->GetLogx();
		fOneDimLogY        = gPad->GetLogy();
		fTitle[i]     = hist->GetTitle();
		if ( i == 0 ) {
			fTitleX = hist->GetXaxis()->GetTitle();
			fTitleY = hist->GetYaxis()->GetTitle();
		}
		opt           = hist->GetDrawOption();
//		if (opt.Length() == 0 && hist->GetSumw2N()>0) {
//			if (gHprDebug > 0)
//				cout << "Force ErrorMode[" <<i<<"] = E " << endl;
//			opt   = "E";
//		}
		opt.ToUpper();
		TRegexp SAME("SAME");
//		fSame[i] = 0;
//		if (opt.Index(SAME) >=0 ) {
//			fSame[i] = 1;
//			opt(SAME) = "";
//		}
		// text, pie, bar
		fText = 0;
		fTextAngle = 0;
		TRegexp TEXT("TEXT");
		Int_t it = opt.Index(TEXT);
		if (it >= 0) {
			Int_t le = 4;
			TString an = opt(it+4, 2);
			if (an.IsDigit()) {
				fTextAngle = an.Atoi();
				le += 2;
			}
			opt(it, le) = "";
			fText = 1;
		}
		fPieChart = 0;
		TRegexp PIE("PIE");
		if (opt.Index(PIE) >= 0) {
			opt(PIE) = "";
			fPieChart = 1;
		}
		fBarChart = 0;
		fBarChart3D = 0;
		fBarChartH = 0;
		TRegexp HBAR("HBAR");
		if (opt.Index(HBAR) >= 0 ) {
			opt(HBAR) = "";
			fBarChartH = 1;
		}
		TRegexp BAR("BAR");
		if (opt.Index(BAR) >= 0) {
			opt(BAR) = "";
			fBarChart3D = 1;
		}
		if (opt.Contains("B"))
			fBarChart = 1;
		fSimpleLine = 0;
		if (opt.Contains("L"))
			fSimpleLine = 1;
		fSmoothLine = 0;
		if (opt.Contains("C"))
			fSmoothLine = 1;
		fShowMarkers[i] = 0;
		if (opt.Contains("P")) {
			fShowMarkers[i] = 1;
		}
		// Error modes
		errmode = "";
		for ( Int_t k = 0; k <= 6; k++ ) {
			erm ="E"; erm += k;
			if ( opt.Contains(erm,TString::kIgnoreCase) ) {
				errmode = erm;
			}
		}
//		cout << "errmode:" << errmode << " " << errmode.Length() << endl;
		if (errmode.Length() == 0 && opt.Contains("E",TString::kIgnoreCase)) {
			errmode = "E";
		}
		fErrorMode[i] = errmode;
		if ( opt.Contains("HIST") ) {
			fShowContour[i] = 1;
		} else {
			fShowContour[i] = 0;
		}
		if (opt.Contains("X+"))  fLabelsTopX = 1;
		else                         fLabelsTopX = 0;
		if (opt.Contains("Y+"))  fLabelsRightY = 1;
		else                         fLabelsRightY = 0;

		fFillColor[i] = hist->GetFillColor();
		fFillStyle[i] = hist->GetFillStyle();
		if (fFillColor[i] != 0 && fFillStyle[i] != 0 ) {
			fFill[i] = 1;
		} else {
			fFill[i] = 0;
		}
		fLineColor[i] = hist->GetLineColor();
		fLineStyle[i] = hist->GetLineStyle();
		fLineWidth[i] = hist->GetLineWidth();
		fMarkerColor[i] =  hist->GetMarkerColor();
		fMarkerStyle[i] =  hist->GetMarkerStyle();
		fMarkerSize[i]  =  hist->GetMarkerSize();

		title = "StringValue_Hist \"";
		title += hist->GetName();
		title += "\" Title ";
		while ( title.Contains(";") )  {
			title(semicolon) = "";
		}
		fRow_lab->Add(new TObjString(title));
		fTitleModBid[i] = ind;
		fValp[ind++] = &fTitle[i];
		fRow_lab->Add(new TObjString("CheckButton_HistContour"));
		fRow_lab->Add(new TObjString("ColorSelect+LColor"));
		fRow_lab->Add(new TObjString("LineSSelect+LSty"));
		fRow_lab->Add(new TObjString("PlainShtVal+LWid"));
		fValp[ind++] = &fShowContour[i];
		fValp[ind++] = &fLineColor[i];
		fValp[ind++] = &fLineStyle[i];
		fValp[ind++] = &fLineWidth[i];

		fRow_lab->Add(new TObjString("CheckButton_Draw Mark  \
&Draw Marks excludes Fill"));
		fValp[ind++] = &fShowMarkers[i];
		fRow_lab->Add(new TObjString("ColorSelect+MColor"));
		fValp[ind++] = &fMarkerColor[i];
		fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
		fValp[ind++] = &fMarkerStyle[i];
		fRow_lab->Add(new TObjString("Float_Value+MSiz;0;5\
&Note: Markers 1,6,7 (dots)\n\
can not be scaled"));
		fValp[ind++] = &fMarkerSize[i];

		fRow_lab->Add(new TObjString("ComboSelect_ErrMod;none;E;E1;E2;E3;E4;E5;E6"));
		fValp[ind++] = &fErrorMode[i];
		fRow_lab->Add(new TObjString("CheckButton+   Fill\
&Fill histogram with selected style\n\
Needs \"HistContour\" to be activated\n\
Marks and Fill are mutually exclusive"));
		fValp[ind++] = &fFill[i];
		fRow_lab->Add(new TObjString("ColorSelect+FColor"));
		fValp[ind++] = &fFillColor[i];
		fRow_lab->Add(new TObjString("Fill_Select+FStyle"));
		fValp[ind++] = &fFillStyle[i];
	}
	
	// this makes sense only for 1 histogram in canvas
	if (fNPads == 0) {
		fRow_lab->Add(new TObjString("StringValue_Title X "));
		fValp[ind++] = &fTitleX;
		fRow_lab->Add(new TObjString("StringValue+Title Y "));
		fValp[ind++] = &fTitleY;
	}
	fRow_lab->Add(new TObjString("Float_Value_EndErrSz\
&Size in pixel of small lines\n\
drawn at end of error bars"));
	fValp[ind++] = &fEndErrorSize;
	fRow_lab->Add(new TObjString("Float_Value-X ErrSz;0.0,0.5\
&Length of X error bars\n\
relative to binwidth "));
	fValp[ind++] = &fErrorX;
	fRow_lab->Add(new TObjString("CheckButton-Set MinY=MinCont\
&Set minimum of Y axis\n\
to minimum of hist content\n\
default is minimum Y = 0"));
	fAdjustMinYButton = ind;
	fValp[ind++] = &fAdjustMinY;
	fRow_lab->Add(new TObjString("CheckButton_     Log X"));
	fValp[ind++] = &fOneDimLogX;
	fRow_lab->Add(new TObjString("CheckButton+     Log Y"));
	fValp[ind++] = &fOneDimLogY;

	fRow_lab->Add(new TObjString("CheckButton+X Lab Top\
&Show labels at top of frame"));
	fRow_lab->Add(new TObjString("CheckButton+Y LabRight\
&Show labels at right side of frame"));
	fValp[ind++] = &fLabelsTopX;
	fValp[ind++] = &fLabelsRightY;
	if ( fAdvanced1Dim ) {
		fRow_lab->Add(new TObjString("CheckButton_SmoothLine"));
		fValp[ind++] = &fSmoothLine;
		fRow_lab->Add(new TObjString("CheckButton+SimpleLine"));
		fValp[ind++] = &fSimpleLine;
		fRow_lab->Add(new TObjString("CheckButton+     Text"));
		fValp[ind++] = &fText;
		fRow_lab->Add(new TObjString("PlainIntVal+Angle&Text angle"));
		fValp[ind++] = &fTextAngle;

		fRow_lab->Add(new TObjString("CheckButton_  BarChart"));
		fValp[ind++] = &fBarChart;
		fRow_lab->Add(new TObjString("CheckButton+BarChart3D"));
		fValp[ind++] = &fBarChart3D;
		fRow_lab->Add(new TObjString("CheckButton+BarChartH"));
		fValp[ind++] = &fBarChartH;
		fRow_lab->Add(new TObjString("CheckButton+ PieChart"));
		fValp[ind++] = &fPieChart;
	}
	// this works only for 1 histogram in canvas
	if (fNPads == 0) {
		fRow_lab->Add(new TObjString("CheckButton_Live Stats\
	&Show a widget with statistics\n\
	when dragging mouse in histogram\n\
	with button 1 pressed"));
		fValp[ind++] = &fLiveStat1Dim;
		fRow_lab->Add(new TObjString("CheckButton+Live Gauss\
	&Do a Gauss fit when dragging\n\
	mouse in histogram\n\
	with button 1 pressed"));
		fValp[ind++] = &fLiveGauss;
		fRow_lab->Add(new TObjString("CheckButton+Const bg \
	&Use constant background"));
		fValp[ind++] = &fLiveConstBG;
		fRow_lab->Add(new TObjString("CheckButton+Linear bg\
	&Use linear background"));
		fValp[ind++] = &fLiveBG;
	}
	fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
	fValp[ind++] = &stycmd;
	fRow_lab->Add(new TObjString("CommandButt+Reset all to default"));
	fValp[ind++] = &sadcmd;
//	fRow_lab->Add(new TObjString("CommandButt+Help"));
//	fValp[ind++] = &helpcmd;

	fOk = 0;
   Int_t itemwidth =  58 * TGMrbValuesAndText::LabelLetterWidth();
	fDialog =
		new TGMrbValuesAndText(fCanvas->GetName(), NULL, &fOk,itemwidth, win,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

Set1DimOptDialog::~Set1DimOptDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
	fHistList.Clear("nodelete");
	fPadList.Clear("nodelete");
	if (gHprDebug>0)cout << "Set1DimOptDialog dtor " << this <<endl;
}
//_______________________________________________________________________

void Set1DimOptDialog::CloseDialog()
{
	if (gHprDebug > 0)
		cout << "Set1DimOptDialog::CloseDialog() " << endl;
//   if (fCanvas) {
//		fCanvas->Disconnect("Closed()", this, "CloseDialog()");
//	}
		
	if (fDialog) fDialog->CloseWindowExt();
	if (fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
	delete this;
}
//_______________________________________________________________________

void Set1DimOptDialog::SetHistAttNow(TCanvas *canvas, Int_t bid)
{
	SetHistAtt(canvas, bid);
}
//_______________________________________________________________________

void Set1DimOptDialog::SetHistAtt(TCanvas *canvas, Int_t bid)
{
	Bool_t changed =kFALSE;
	/*
	fCanvas->cd();
	if ( fCanvas->GetLogx() != fOneDimLogX ){
		fCanvas->SetLogx(fOneDimLogX);
		changed = kTRUE;
	}
	if ( fCanvas->GetLogy() != fOneDimLogY ){
		fCanvas->SetLogy(fOneDimLogY);
		changed = kTRUE;
	}
	*/
	gStyle->SetEndErrorSize (fEndErrorSize);
	gStyle->SetErrorX       (fErrorX);
	TEnv env(".hprrc");
	env.SetValue("Set1DimOptDialog.fLiveStat1Dim" , fLiveStat1Dim);
	env.SetValue("Set1DimOptDialog.fLiveGauss"    , fLiveGauss);
	env.SetValue("Set1DimOptDialog.fLiveBG"       , fLiveBG);
	env.SetValue("Set1DimOptDialog.fLiveConstBG"       , fLiveConstBG);
	env.SaveLevel(kEnvLocal);
	TList * leg_entries = NULL;
	TLegend * leg = (TLegend*)canvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
	if ( leg ) {
		leg_entries = leg->GetListOfPrimitives();
		if ( leg_entries->GetSize() != fNHists )
			leg_entries = NULL;
	}
	TH1* hist;
	TPad* pad;
	for ( Int_t i =0; i < fNHists; i++ ) {
		hist =(TH1*)fHistList.At(i);
		pad = (TPad*)fPadList.At(i);
		pad->cd();
		if ( pad->GetLogx() != fOneDimLogX ){
			pad->SetLogx(fOneDimLogX);
			changed = kTRUE;
		}
		if ( pad->GetLogy() != fOneDimLogY ){
			pad->SetLogy(fOneDimLogY);
			changed = kTRUE;
		}

		if (i == 0 ) {
			hist->SetTitle(fTitle[0]);
			hist->GetXaxis()->SetTitle(fTitleX);
			hist->GetYaxis()->SetTitle(fTitleY);
		}
		if ( leg_entries != NULL  && bid == fTitleModBid[i] ) {
			((TLegendEntry*)leg_entries->At(i))->SetLabel(fTitle[i]);
		}
		if (fFill[i]) {
			if ( fFillColor[i] == 0 )
				fFillColor[i] = 4;
			if ( fFillStyle[i] == 0 )
				fFillStyle[i] = 3001;
			hist->SetFillColor(fFillColor[i]);
			hist->SetFillStyle(fFillStyle[i]);
		} else {
			hist->SetFillStyle(0);
		}
		hist->SetLineColor(fLineColor[i]);
		hist->SetLineStyle(fLineStyle[i]);
		hist->SetLineWidth(fLineWidth[i]);
		hist->SetMarkerColor(fMarkerColor[i]);
		hist->SetMarkerStyle(fMarkerStyle[i]);
		if (fErrorMode[i] == "E1" && fMarkerSize[i] == 0) {
			hist->SetMarkerSize(0.01);
		}
		TString oldopt(hist->GetDrawOption());
		if ( gHprDebug > 0 ) {
			cout << "oldoption " << oldopt << " fErrorMode[i] " << fErrorMode[i]<< endl;
		}
		TString drawopt;
		if (oldopt.Contains("SAME", TString::kIgnoreCase)) {
			drawopt = "SAME";
		}
		if (fErrorMode[i] != "none") {
			drawopt += fErrorMode[i];
		}
		if (fShowMarkers[i] != 0) {
			if ( fMarkerSize[i] < 0.01 )
				fMarkerSize[i] = 1;
			drawopt += "P";
			hist->SetMarkerSize(fMarkerSize[i]);
		}
		if ( fAdvanced1Dim ) {
			if (fSimpleLine)
				drawopt += "L";
			else if (fSmoothLine)
				drawopt += "C";
			else if (fBarChart)
				drawopt += "B";
			else if (fBarChartH)
				drawopt += "HBAR";
			else if (fBarChart3D)
				drawopt += "BAR";
			else if (fPieChart)
				drawopt += "PIE";
			else if (fText) {
				drawopt +="TEXT";
				if (fTextAngle > 0){
					drawopt += fTextAngle;
				}
				if ( fMarkerSize[i] < 0.01 )
					fMarkerSize[i] = 1;
				hist->SetMarkerSize(fMarkerSize[i]);
			}
		}
		if (gPad->GetTickx() < 2 && fLabelsTopX)
			drawopt += "X+";
		if (gPad->GetTicky() < 2 && fLabelsRightY)
			drawopt += "Y+";
//		if ( fSame[i] )
//			drawopt+= "SAME";
		if (fShowContour[i] != 0 || fFill[i] != 0) drawopt += "HIST";
//		if (drawopt.Length() == 0 || fShowContour[i] != 0) drawopt += "HIST";
		if ( gHprDebug > 0 )
			cout << "Set1DimOptDialog::SetAtt " << drawopt << endl;
		hist->SetDrawOption(drawopt);
		hist->SetOption(drawopt);
	}
	fCanvas->Pop();
	TIter next=fCanvas->GetListOfPrimitives();
	TObject *obj;
	while (obj = next()) {
		if (obj->InheritsFrom("TPad") )
			((TPad*)obj)->Modified();
	}
	fCanvas->Modified();
	fCanvas->Update();
	if (changed)
		LinLogChanged(fCanvas);
}
//____________________________________________________________
void Set1DimOptDialog::LinLogChanged(TObject* o)
{
	Long_t args[1];
	args[0] = (Long_t)o;
	if (gHprDebug > 1 )
		cout << "Set1DimOptDialog::Emit(LinLogChanged(TObject*)" << endl;

	Emit("LinLogChanged(TObject*)", args );
}
//____________________________________________________________

void Set1DimOptDialog::SetAtt(TH1* /*hist*/)
{
}
//______________________________________________________________________

void Set1DimOptDialog::SetHistAttPermLocal()
{
//   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
	SaveDefaults();
	SetHistAttPerm();
}
//______________________________________________________________________

void Set1DimOptDialog::SetDefaults()
{
//   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
	TEnv env(".hprrc");
	gStyle->SetHistFillColor(env.GetValue("Set1DimOptDialog.fFillColor", 2));
	gStyle->SetHistLineColor(env.GetValue("Set1DimOptDialog.fLineColor", 1));
	gStyle->SetHistFillStyle(env.GetValue("Set1DimOptDialog.fFillStyle", 0));
	gStyle->SetHistLineStyle(env.GetValue("Set1DimOptDialog.fLineStyle", 1));
	gStyle->SetHistLineWidth(env.GetValue("Set1DimOptDialog.fLineWidth", 2));
	gStyle->SetEndErrorSize (env.GetValue("Set1DimOptDialog.fEndErrorSize",  1));
	gStyle->SetErrorX       (env.GetValue("Set1DimOptDialog.fErrorX", 0.5));
//   fAdjustMinY            = env.GetValue("Set1DimOptDialog.fAdjustMinY", 1);
	TString temp =  env.GetValue("Set1DimOptDialog.fErrorMode", "");
	if (  temp != "none" )
		gStyle->SetDrawOption(temp);
	else
		gStyle->SetDrawOption("");
	gStyle->SetMarkerColor     (env.GetValue("Set1DimOptDialog.MarkerColor",       1));
	gStyle->SetMarkerStyle     (env.GetValue("Set1DimOptDialog.MarkerStyle",       7));
	gStyle->SetMarkerSize      (env.GetValue("Set1DimOptDialog.MarkerSize",      1.));
}
//______________________________________________________________________

void Set1DimOptDialog::SetHistAttPerm()
{
//   cout << "Set1DimOptDialog:: SetHistAttPerm()" << endl;
	SaveDefaults();
	gStyle->SetHistFillColor(fFillColor[0]);
	gStyle->SetHistLineColor(fLineColor[0]);
	gStyle->SetHistFillStyle(fFillStyle[0]);
	gStyle->SetHistLineStyle(fLineStyle[0]);
	gStyle->SetHistLineWidth(fLineWidth[0]);
	gStyle->SetEndErrorSize (fEndErrorSize);
	gStyle->SetErrorX       (fErrorX);
	if (fErrorMode[0] != "none")
		gStyle->SetDrawOption(fErrorMode[0]);
	else
		gStyle->SetDrawOption("");
	if (fShowMarkers[0] != 0) {
		TString temp = gStyle->GetDrawOption();
		temp+= "P";
		gStyle->SetDrawOption(temp);
	}
	gStyle->SetMarkerColor  (fMarkerColor[0]);
	gStyle->SetMarkerStyle  (fMarkerStyle[0]);
	gStyle->SetMarkerSize   (fMarkerSize[0] );
//	gStyle->SetOptLogx      (fOneDimLogX);
//	gStyle->SetOptLogy      (fOneDimLogY);
}
//______________________________________________________________________

void Set1DimOptDialog::SaveDefaults()
{
//	cout << "Set1DimOptDialog::SaveDefaults() fShowMarkers[0] " << fShowMarkers[0]<< endl;
	TEnv env(".hprrc");
	env.SetValue("Set1DimOptDialog.fFill1Dim",      fFill[0]      );
	env.SetValue("Set1DimOptDialog.fFillColor",     fFillColor[0] );
	env.SetValue("Set1DimOptDialog.fLineColor",     fLineColor[0] );
	env.SetValue("Set1DimOptDialog.fFillStyle",     fFillStyle[0] );
	env.SetValue("Set1DimOptDialog.fLineStyle",     fLineStyle[0] );
	env.SetValue("Set1DimOptDialog.fLineWidth",     fLineWidth[0] );
	env.SetValue("Set1DimOptDialog.fErrorMode",     fErrorMode[0]  );
	env.SetValue("Set1DimOptDialog.MarkerColor",    fMarkerColor[0]);
	env.SetValue("Set1DimOptDialog.MarkerStyle",    fMarkerStyle[0]);
	env.SetValue("Set1DimOptDialog.MarkerSize",     fMarkerSize[0] );
	env.SetValue("Set1DimOptDialog.fShowMarkers",   fShowMarkers[0]);

	env.SetValue("Set1DimOptDialog.fLiveStat1Dim"  , fLiveStat1Dim );
	env.SetValue("Set1DimOptDialog.fLiveGauss"     , fLiveGauss    );
	env.SetValue("Set1DimOptDialog.fLiveBG"        , fLiveBG       );
	env.SetValue("Set1DimOptDialog.fLiveConstBG"   , fLiveConstBG       );
	env.SetValue("Set1DimOptDialog.fDrawAxisAtTop" , fDrawAxisAtTop);
	env.SetValue("Set1DimOptDialog.fShowContour"   , fShowContour[0]  );
	env.SetValue("Set1DimOptDialog.fLabelsTopX"    , fLabelsTopX   );
	env.SetValue("Set1DimOptDialog.fLabelsRightY"  , fLabelsRightY );
	if (fNPads == 0) {
		env.SetValue("Set1DimOptDialog.fOneDimLogX"    , fOneDimLogX   );
		env.SetValue("Set1DimOptDialog.fOneDimLogY"    , fOneDimLogY   );
	} else {  
		env.SetValue("GroupOfHists.fDisplayLogX"       , fOneDimLogX   );
		env.SetValue("GroupOfHists.fDisplayLogY"    , fOneDimLogY   );
	}
	env.SetValue("Set1DimOptDialog.fEndErrorSize" , fEndErrorSize  );
	env.SetValue("Set1DimOptDialog.fErrorX",        fErrorX        );
	env.SetValue("Set1DimOptDialog.fAdjustMinY",    fAdjustMinY    );
	env.SetValue("Set1DimOptDialog.fSmoothLine",    fSmoothLine    );
	env.SetValue("Set1DimOptDialog.fSimpleLine",    fSimpleLine    );
	env.SetValue("Set1DimOptDialog.fBarChart",      fBarChart      );
	env.SetValue("Set1DimOptDialog.fBarChart3D",    fBarChart3D    );
	env.SetValue("Set1DimOptDialog.fBarChartH",     fBarChartH     );
	env.SetValue("Set1DimOptDialog.fPieChart",      fPieChart      );
	env.SetValue("Set1DimOptDialog.fText",          fText          );
	env.SetValue("Set1DimOptDialog.fTextAngle",     fTextAngle     );

	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set1DimOptDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void Set1DimOptDialog::RestoreDefaults(Int_t resetall)
{
//    cout << "SetHistOptDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
	TEnv env(envname);

 	fEndErrorSize   = env.GetValue("Set1DimOptDialog.fEndErrorSize" , 1);
 	fErrorX         = env.GetValue("Set1DimOptDialog.fErrorX",      0.5);
	fAdjustMinY     = env.GetValue("Set1DimOptDialog.fAdjustMinY",    1);
	fLiveStat1Dim   = env.GetValue("Set1DimOptDialog.fLiveStat1Dim"  ,0);
	fLiveGauss      = env.GetValue("Set1DimOptDialog.fLiveGauss"     ,0);
	fLiveBG         = env.GetValue("Set1DimOptDialog.fLiveBG"        ,1);
	fLiveConstBG    = env.GetValue("Set1DimOptDialog.fLiveConstBG"   ,1);
	fDrawAxisAtTop  = env.GetValue("Set1DimOptDialog.fDrawAxisAtTop" ,0);
//	fShowContour    = env.GetValue("Set1DimOptDialog.fShowContour"   ,1);
	fLabelsTopX     = env.GetValue("Set1DimOptDialog.fLabelsTopX"    ,0);
	fLabelsRightY   = env.GetValue("Set1DimOptDialog.fLabelsRightY"  ,0);
	fOneDimLogX     = env.GetValue("Set1DimOptDialog.fOneDimLogX"    ,0);
	fOneDimLogY     = env.GetValue("Set1DimOptDialog.fOneDimLogY"    ,0);
}
//______________________________________________________________________

void Set1DimOptDialog::GetValuesFromHist()
{
}
//______________________________________________________________________

void Set1DimOptDialog::CloseDown(Int_t wid)
{
	if (gHprDebug > 0)
		cout << "CloseDown(" << wid<< ")" <<endl;
	fDialog = NULL;
	if (wid == -1)
		SaveDefaults();
}
//______________________________________________________________________

void Set1DimOptDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject *obj)
{
	TCanvas *canvas = (TCanvas *)obj;
	if (gHprDebug > 0) {
		cout << "CRButtonPressed " <<bid;
		if (obj) cout  << ", " << canvas->GetName() << ")";
		cout << endl;
	}
	SetHistAttNow(canvas, bid);
}


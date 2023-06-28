#include "TROOT.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TRootCanvas.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "Set2DimGraphDialog.h"
#include "GeneralAttDialog.h"
#include "hprbase.h"
#include "SetColor.h"
#include <iostream>

extern Int_t gHprDebug;
namespace std {} using namespace std;

//_______________________________________________________________________

Set2DimGraphDialog::Set2DimGraphDialog(Int_t /*batch*/)
{
	cout << "ctor Set2DimGraphDialog, non interactive" <<endl;
	fDialog = NULL;
}
//_______________________________________________________________________

Set2DimGraphDialog::Set2DimGraphDialog(TGWindow * win)
{
static const Char_t helptext[] =
"<!DOCTYPE HTML>\n\
<HTML>\n\
<TITLE>HelpSet2DimOpt</TITLE>\n\
Note: Changeing options only influence the current histogram\n\
		 To make them active for subsequently displayed histograms\n\
		 press: \"Set as global default\"\n\
 \n\
Most of the value in this widget are self explaining\n\
If \"Live statbox\" a box is displayed when dragging the\n\
pressed mouse in the histogram area showing various statistics\n\
values.\n\
 \n\
X, Y and Z scales can be set by default to logarithmic or linear.\n\
This can still be reset for individual histograms.\n\
In detail: If a canvas is closed and its lin-log state differs\n\
from the global default its state is stored and restored when\n\
the histogram is shown again.\n\
 \n\
Specific drawing options can be used to paint a TGraph2D:\n\
\n\
<b>TRI </b>  : The Delaunay triangles are drawn using filled area.\n\
		  An hidden surface drawing technique is used. The surface is\n\
		  painted with the current fill area color. The edges of each\n\
		  triangles are painted with the current line color.\n\
<b>TRIW</b>  : The Delaunay triangles are drawn as wire frame\n\
<b>TRI1</b>  : The Delaunay triangles are painted with color levels. The edges\n\
		  of each triangles are painted with the current line color.\n\
<b>TRI2</b>  : the Delaunay triangles are painted with color levels.\n\
<b>P   </b>  : Draw a marker at each vertex\n\
<b>P0  </b>  : Draw a circle at each vertex. Each circle background is white.\n\
<b>PCOL</b>  : Draw a marker at each vertex. The color of each marker is\n\
		  defined according to its Z position.\n\
<b>CONT</b>  : Draw contours\n\
<b>LINE</b>  : Draw a 3D polyline\n\
\n\
<b>CONT0</b> : Draw a contour plot using surface colors to distinguish contours\n\
<b>CONT1</b> : Draw a contour plot using line styles to distinguish contours\n\
<b>CONT2</b> : Draw a contour plot using the same line style for all contours\n\
<b>CONT3</b> : Draw a contour plot using fill area colors\n\
<b>CONT4</b> : Draw a contour plot using surface colors (SURF option at theta = 0)\n\
<b>CONT5</b> : (TGraph2D only) Draw a contour plot using Delaunay triangles\n\
<b>LEGO </b> : Draw a lego plot with hidden line removal\n\
<b>LEGO1</b> : Draw a lego plot with hidden surface removal\n\
<b>LEGO2</b> : Draw a lego plot using colors to show the cell contents\n\
<b>SURF </b> : Draw a surface plot with hidden line removal\n\
<b>SURF1</b> : Draw a surface plot with hidden surface removal\n\
<b>SURF2</b> : Draw a surface plot using colors to show the cell contents\n\
<b>SURF3</b> : same as SURF with in addition a contour view drawn on the top\n\
<b>SURF4</b> : Draw a surface using Gouraud shading\n\
<b>SURF5</b> : Same as SURF3 but only the colored contour is drawn. Used with\n\
			 option CYL, SPH or PSR it allows to draw colored contours on a\n\
			 sphere, a cylinder or a in pseudo rapidy space. In cartesian\n\
			 or polar coordinates, option SURF3 is used.\n\
 \n\
Surface with option \"GLSURF\"\n\
The surface profile is displayed on the slicing plane. The profile\n\
projection is drawn\n\
on the back plane by pressing 'p' or 'P' key.\n\
For further details contact ROOTs documentation.\n\
 \n\
When surface options are selected rendering of the TGraph2D\n\
uses the DeLaunay triangulation algorithm. With many points\n\
(> 1000) this may take minutes especially if the points are\n\
ordered by rows and columns as a typical result of a scan.\n\
In this case reordering the points randomly may gain\n\
a factor upto 10 in speed. In addition for display\n\
purposes in draft mode e.g. 500 points may be sufficient\n\
The parameter fUseNPoints is used to control this.\n\
If <= no reordering is done, all points are used.\n\
If it is > 0 reorder and use only fUseNPoints but of course\n\
only max number of points of the graph.\n\
Option: \"Randomize points\"\n\
The current implementation of the Delauny TRI algorithm\n\
has a problem when points are exactly aligned\n\
In the plot you see weird extra points outside the plot\n\
Shifting the z-value randomly by a tiny amount avoids this\n\
It is not visible in the final plot \n\
</HTML>";

	const Char_t *fDrawOpt2[kNGraph2Dopt] =
	{"TRI ", "TRIW",   "TRI1",  "TRI2",
	"CONT0", "CONT1" , "CONT2", "CONT3", "CONT4", "SURF0",
	"SURF1", "SURF2",  "SURF3", "SURF4", "SURF5", "LEGO",
	"LEGO1", "LEGO2",  "P",     "P0",    "PCOL",  "LINE"};
	
	if (win) {
		TCanvas *cc = ((TRootCanvas*)win)->Canvas();
		fCanvas = reinterpret_cast<HTCanvas*>(cc);
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	fCmdButton = fCanvas->GetCmdButton();
	fHist = NULL;
//	cout << "Enter Set2DimGraphDialog" << endl;
	//	Int_t nh1 = 0, nh2 = 0;
	TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TGraph2D")) {
			fGraph2D = (TGraph2D*)obj;
			fHist = fGraph2D->GetHistogram();
		}
	}
	if ( fHist == NULL ) {
		cout << "No Histogram in Canvas" << endl;
//		return;
	} else {
		if ( gHprDebug > 0 )
			cout << "Set2DimGraphDialog: " << endl
			<< "TCanvas* canvas = (TCanvas*)" << fCanvas
			<< "; TH2* hist = (TH2*)" << fHist 
			<< "; Set2DimGraphDialog * d2 = (Set2DimGraphDialog*)" << this << endl;
	}
	RestoreDefaults();
	fViewingAngle ="Def(30,45)";
	if (gHprDebug > 0)
		cout << "Set2DimGraphDialog: fDrawOpt2Dim " << fDrawOpt2Dim << endl;
//   Int_t selected = -1;
	for (Int_t i = 0; i < kNGraph2Dopt; i++) {
		fDrawOpt2DimArray[i] = fDrawOpt2[i];
	}
	fSameOpt = "";
	if ( fHist ) {
		GetValuesFrom();
	}
	if (fDrawOpt2Dim.Contains("GL")) fUseGL = 1;
	else                             fUseGL = 0;
// extract draw option
	cout << "fDrawOpt2Dim " << fDrawOpt2Dim << endl;
	for (Int_t i = kNGraph2Dopt-1; i >= 0; i--) {
//		cout << "fDrawOpt2DimArray "<< i << " "
//     << fDrawOpt2DimArray[i] << endl;
		Int_t ind = fDrawOpt2Dim.Index(fDrawOpt2DimArray[i]);
		if ( ind >= 0 ) {
			Int_t len = strlen(fDrawOpt2DimArray[i]);
			fDrawOpt2Dim = fDrawOpt2Dim.Replace(ind, len, "");
			fOptRadio[i] = 1;
		} else {
			fOptRadio[i] = 0;
		}
	}
	fRow_lab = new TList();

	Int_t ind = 0;
	Int_t indopt = 0;
	Int_t fDummy;
//   static Int_t dummy;
	static TString stycmd("SetAttPerm()");
	static TString sadcmd("SetAllToDefault()");

	
	for (Int_t i = 0; i < kNGraph2Dopt; i++) {
//   cout << " indopt  " <<  indopt << endl;
		TString text("RadioButton");
		if ( i%6 == 0 )text += "_";
		else       text += "+";
		if (strlen(fDrawOpt2DimArray[indopt]) < 5)
			text += " ";
		else if (strlen(fDrawOpt2DimArray[indopt]) < 4)
			text += "  ";
		else if (strlen(fDrawOpt2DimArray[indopt]) < 3)
			text += "   ";
		else if (strlen(fDrawOpt2DimArray[indopt]) < 2)
			text += "    ";
		text += fDrawOpt2DimArray[indopt];
		fRow_lab->Add(new TObjString(text.Data()));
		fValp[ind++] = &fOptRadio[indopt++];
	}
	fRow_lab->Add(new TObjString("CommentOnly+ OpenGL:"));
	fValp[ind++] = &fDummy;
	fRow_lab->Add(new TObjString("CheckButton+Use"));
	fBidGL = ind;
	fValp[ind++] = &fUseGL;
//   cout << " indopt  " <<  indopt << endl;
//	fRow_lab->Add(new TObjString("PlainShtVal+LineWid"));
//	fValp[ind++] = &fHistLineWidth2Dim;
	
	fRow_lab->Add(new TObjString("ColorSelect_MColor "));
	fBidMarkerColor = ind; fValp[ind++] = &fMarkerColor2Dim;
	fRow_lab->Add(new TObjString("Mark_Select+MStyle "));
	fBidMarkerStyle = ind; fValp[ind++] = &fMarkerStyle2Dim;
	fRow_lab->Add(new TObjString("Float_Value+MSize  "));
	fBidMarkerSize = ind; fValp[ind++] = &fMarkerSize2Dim;
	fRow_lab->Add(new TObjString("CheckButton_Log X"));
	fRow_lab->Add(new TObjString("CheckButton-Log Y"));
	fRow_lab->Add(new TObjString("CheckButton-Log Z"));
	fValp[ind++] = &fTwoDimLogX;
	fValp[ind++] = &fTwoDimLogY;
	fValp[ind++] = &fTwoDimLogZ;
	fRow_lab->Add(new TObjString("ComboSelect-View;Def(30,45);Top(XY);Front(XZ);Side(YZ)"));
	fBidViewingAngle = ind;
	fValp[ind++] = &fViewingAngle;
	fRow_lab->Add(new TObjString("CheckButton_Randomize Points"));
	fValp[ind++] = &fRandomizePoints;
	fRow_lab->Add(new TObjString("PlainIntVal+Use N Points"));
	fValp[ind++] = &fUseNPoints;

	fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
	fValp[ind++] = &stycmd;
	fRow_lab->Add(new TObjString("CommandButt+Reset all to default"));
	fValp[ind++] = &sadcmd;

	static Int_t ok;
	Int_t itemwidth = 380;
	fDialog =
		new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

Set2DimGraphDialog::~Set2DimGraphDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________

void Set2DimGraphDialog::CloseDialog()
{
//   cout << "Set2DimGraphDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
	delete this;
}
//_______________________________________________________________________

void Set2DimGraphDialog::SetAttNow(TCanvas *canvas)
{
	if (!canvas) return;
	fDrawOpt2Dim = "";
	for (Int_t i = 0; i < kNGraph2Dopt; i++) {
		if (fOptRadio[i] == 1) {
//			cout << "fDrawOpt2DimArray[" << i << "]" << endl;
			fDrawOpt2Dim += fDrawOpt2DimArray[i];
			if (fUseGL && !fDrawOpt2Dim.Contains("GL"))
				fDrawOpt2Dim.Prepend("GL");
		}
	}
	SetAttAll(canvas);
	SetPadAtt(canvas);
	canvas->Pop();
	canvas->Modified();
	canvas->Update();
}
//_______________________________________________________________________

void Set2DimGraphDialog::SetAttAll(TCanvas *canvas)
{
	TIter next(canvas->GetListOfPrimitives());
	TObject *obj;
	canvas->cd();
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TGraph2D")) {
			TGraph2D* gr = (TGraph2D*)obj;
			gr->SetDrawOption(fDrawOpt2Dim);
			if ( fDrawOpt2Dim.Contains("P")){
				gr->SetMarkerColor(fMarkerColor2Dim);  
				gr->SetMarkerStyle(fMarkerStyle2Dim);  
				gr->SetMarkerSize (fMarkerSize2Dim);
			} 
			if ( gHprDebug > 0 ) 
				cout << "SetAttAll: " << fDrawOpt2Dim<< endl;
		}
	}
}
//_______________________________________________________________________

void Set2DimGraphDialog::SetPadAtt(TPad *pad)
{
	if ( !pad ) return;
	pad->cd();
	pad->SetLogx(fTwoDimLogX);
	pad->SetLogy(fTwoDimLogY);
	pad->SetLogz(fTwoDimLogZ);
	if (f2DimBackgroundColor == 0) {
		if (gStyle->GetCanvasColor() == 0) {
			pad->GetFrame()->SetFillStyle(0);
		} else {
			pad->GetFrame()->SetFillStyle(1001);
			pad->GetFrame()->SetFillColor(10);
		}
	} else {
		pad->GetFrame()->SetFillStyle(1001);
		pad->GetFrame()->SetFillColor(f2DimBackgroundColor);
	}
		// 0: default 60,120; 1: ;Top(XY) 0,-180;Front 0,0;Side 90,0 
		if (fViewingAngle.BeginsWith("Def")) {
			pad->SetPhi(30);
			pad->SetTheta(45);
			fHist->GetXaxis()->SetLabelOffset(.01);
			fHist->GetYaxis()->SetLabelOffset(.01);
		} else if (fViewingAngle.BeginsWith("Top")) {
			pad->SetPhi(0);
			pad->SetTheta(90);
			fHist->GetXaxis()->SetLabelOffset(-.04);
			fHist->GetYaxis()->SetLabelOffset(.01);
		} else if (fViewingAngle.BeginsWith("Front")) {
			pad->SetPhi(0);
			pad->SetTheta(0);
			fHist->GetXaxis()->SetLabelOffset(-.04);
			fHist->GetYaxis()->SetLabelOffset(.01);
		} else if (fViewingAngle.BeginsWith("Side")) {
			pad->SetPhi(90);
			pad->SetTheta(0);
			fHist->GetXaxis()->SetLabelOffset(.01);
			fHist->GetYaxis()->SetLabelOffset(-.03);
			
		}
	
}
//______________________________________________________________________

void Set2DimGraphDialog::SetAttPerm()
{
	if (gHprDebug > 0)
		cout << "Set2DimGraphDialog:: SetHistAttPerm()" << endl;
	TEnv env(".hprrc");
	TRegexp sa("SAME");
	fDrawOpt2Dim(sa) = "";
	env.SetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim);
	env.SetValue("Set2DimGraphDialog.fDrawOpt2Dim", fDrawOpt2Dim);
	env.SetValue("Set2DimGraphDialog.fShowZScale", fShowZScale);
	env.SetValue("Set2DimGraphDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimGraphDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimGraphDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimGraphDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogX",       fTwoDimLogX       );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogY",       fTwoDimLogY       );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogZ",       fTwoDimLogZ       );
	env.SetValue("Set2DimGraphDialog.fUseGL",            fUseGL            );
	env.SetValue("Set2DimGraphDialog.fContourLevels",    fContourLevels    );
	env.SetValue("Set2DimGraphDialog.fUseNPoints",       fUseNPoints    );
//	env.SetValue("Set2DimGraphDialog.fViewingAngle",       fViewingAngle    );
	env.SetValue("Set2DimGraphDialog.fRandomizePoints",  fRandomizePoints   );

	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set2DimGraphDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	TRegexp sa("SAME");
	fDrawOpt2Dim(sa) = "";
	env.SetValue("Set2DimGraphDialog.fDrawOpt2Dim",      fDrawOpt2Dim);
	env.SetValue("Set2DimGraphDialog.fShowZScale",       fShowZScale);
	env.SetValue("Set2DimGraphDialog.f2DimBackgroundColor",f2DimBackgroundColor);
	env.SetValue("Set2DimGraphDialog.fMarkerColor2Dim",  fMarkerColor2Dim  );
	env.SetValue("Set2DimGraphDialog.fMarkerStyle2Dim",  fMarkerStyle2Dim  );
	env.SetValue("Set2DimGraphDialog.fMarkerSize2Dim",   fMarkerSize2Dim   );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogX",       fTwoDimLogX       );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogY",       fTwoDimLogY       );
	env.SetValue("Set2DimGraphDialog.fTwoDimLogZ",       fTwoDimLogZ       );
	env.SetValue("Set2DimGraphDialog.fUseGL",            fUseGL            );
	env.SetValue("Set2DimGraphDialog.fContourLevels",    fContourLevels    );
	env.SetValue("Set2DimGraphDialog.fUseNPoints",       fUseNPoints    );
//	env.SetValue("Set2DimGraphDialog.fViewingAngle",     fViewingAngle    );
	env.SetValue("Set2DimGraphDialog.fRandomizePoints",  fRandomizePoints   );
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void Set2DimGraphDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void Set2DimGraphDialog::GetValuesFrom()
{
	if ( !fGraph2D ) return;
	fCanvas->cd();
	fDrawOpt2Dim = fGraph2D->GetDrawOption();
	fDrawOpt2Dim.ToUpper();
	if ( fDrawOpt2Dim.Contains("SAME") )
		fSameOpt = "SAME";
	if ( gHprDebug > 0 )
		cout << "fDrawOpt2Dim::GetValuesFrom() " << fDrawOpt2Dim << endl;
	fTwoDimLogX        = fCanvas->GetLogx();
	fTwoDimLogY        = fCanvas->GetLogy();
	fTwoDimLogZ        = fCanvas->GetLogz();
	fMarkerColor2Dim   = fHist->GetMarkerColor();
	fMarkerStyle2Dim   = fHist->GetMarkerStyle();
	fMarkerSize2Dim    = fHist->GetMarkerSize();
}
//______________________________________________________________________

void Set2DimGraphDialog::RestoreDefaults(Int_t resetall)
{
	if ( gHprDebug > 0 )
		cout << "Set2DimGraphDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
	TEnv env(envname);
	fDrawOpt2Dim       = env.GetValue("Set2DimGraphDialog.fDrawOpt2Dim", "TRI1");
	TRegexp sa("SAME");
	fDrawOpt2Dim(sa) = "";
	fShowZScale        = env.GetValue("Set2DimGraphDialog.fShowZScale", 1);
	f2DimBackgroundColor = env.GetValue("Set2DimGraphDialog.f2DimBackgroundColor", 0);
	fMarkerColor2Dim   = env.GetValue("Set2DimGraphDialog.fMarkerColor2Dim",   1);
	fMarkerStyle2Dim   = env.GetValue("Set2DimGraphDialog.fMarkerStyle2Dim",   1);
	fMarkerSize2Dim    = env.GetValue("Set2DimGraphDialog.fMarkerSize2Dim",    1);
	fTwoDimLogX        = env.GetValue("Set2DimGraphDialog.fTwoDimLogX",        0);
	fTwoDimLogY        = env.GetValue("Set2DimGraphDialog.fTwoDimLogY",        0);
	fTwoDimLogZ        = env.GetValue("Set2DimGraphDialog.fTwoDimLogZ",        0);
	fUseGL             = env.GetValue("Set2DimGraphDialog.fUseGL",             0);
	fContourLevels     = env.GetValue("Set2DimGraphDialog.fContourLevels",    20);
	fUseNPoints        = env.GetValue("Set2DimGraphDialog.fUseNPoints",      500);
	fRandomizePoints   = env.GetValue("Set2DimGraphDialog.fRandomizePoints",   0);
}
//______________________________________________________________________

void Set2DimGraphDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
	fDialog = NULL;
	if (wid == -1)
		SaveDefaults();
}
//______________________________________________________________________

void Set2DimGraphDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
	TCanvas *canvas = (TCanvas *)obj;
	if ( gHprDebug > 0 ) {
		cout << "CRButtonPressed:" << wid<< ", " 
		<<bid << " canvas "  << canvas 
		<< " fViewingAngle " << fViewingAngle
		<< endl;
	}
	if ( bid == fBidViewingAngle ) {
	}
	if ( bid == fBidGL ) {
		TString opt = fGraph2D->GetDrawOption();
		opt.ToUpper();
		if ( gHprDebug > 0 )
			cout << "CRButtonPressed: " << opt << " fUseGL: " << fUseGL<< endl;
		Int_t indgl = opt.Index("GL");
		if ( indgl >= 0 && fUseGL == 0 ) {
			opt(indgl,2) = "";
		} else if ( indgl < 0 && fUseGL == 1 ) {
			opt.Prepend("GL");
		}
		if (fCanvas->UseGL() != ((Bool_t)fUseGL) ) {
			TEnv env(".hprrc");
			env.SetValue("Set2DimGraphDialog.fDrawOpt2Dim", opt);
			env.SaveLevel(kEnvLocal);
			if ( fCmdButton && fCmdButton->TestBit(TObject::kNotDeleted) ) { 
				gROOT->ProcessLine(fCmdButton->GetMethod());
				gSystem->ProcessEvents();
				fCanvas->Close();
				CloseDialog();
			} else {
				cout << setred << "Did you close the list of objects?" << endl
					<< "Cant auto redisplay, please redisplay manually"
					<< setblack  << endl;
			}
			return;
		}
	}		
	SetAttNow(canvas);
}


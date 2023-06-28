#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TGraph.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "GraphAttDialog.h"
#include "support.h"
#include "hprbase.h"
#include <iostream>

extern Int_t gHprDebug;

namespace std {} using namespace std;
//_______________________________________________________________________

GraphAttDialog::GraphAttDialog(TGWindow * win)
{
static const char helptext[] =
"\n\
The root doc reads:\n\
When a graph is painted with the option \"C\" or \"L\",\n\
it is possible to draw a filled area on one side of the line.\n\
This is useful to show exclusion zones. This drawing mode is\n\
activated when the absolute value of the graph line width\n\
(set thanks to SetLineWidth) is greater than 99.\n\
In that case the line width number is interpreted as 100*ff+ll = ffll.\n\
The two-digit numbers \"ll\" represent the normal line width\n\
whereas \"ff\" is the filled area width. The sign of \"ffll\" allows\n\
flipping the filled area from one side of the line to the other.\n\
The current fill area attributes are used to draw the hatched zone.\n\
\n\
\"X\"	By default the error bars are drawn. If option \"X\" is specified, the errors\n\
      are not drawn. The graph with errors in drawn like a normal graph.\n\
\"Z\"	By default horizonthal and vertical small lines are drawn at the end \n\
      of the error bars. If option \"z\" or \"Z\" is specified, these lines are \n\
      not drawn.\n\
\">\"	An arrow is drawn at the end of the error bars. The size of the arrow is\n\
      set to 2/3 of the marker size.\n\
\"|>\"A filled arrow is drawn at the end of the error bars. The size of the\n\
      arrow is set to 2/3 of the marker size.\n\
\"||\"	Only the end vertical/horizonthal lines of the error bars are drawn.\n\
      This option is interesting to superimpose systematic errors on top\n\
      of a graph with statistical errors.\n\
\"[]\"Does the same as option \"||\" except that it draws additionnal\n\
      tick marks at the end of the vertical/horizonthal lines.\n\
      This makes less ambiguous plots in case several graphs are drawn on\n\
      the same picture.\n\
\"2\"	Error rectangles are drawn.\n\
\"3\"	A filled area is drawn through the end points of the vertical error bars.\n\
\"4\"	A smoothed filled area is drawn through the end points of the\n\
      vertical error bars.\n\
\n\
_________________________________________________________________________\n\
";
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	fNGraphs = FindGraphs((TPad*)fCanvas, &fGraphList);
	if (fNGraphs <= 0) {
		cout << " No graph found " <<endl;
		return;
	} 
	RestoreDefaults();
	fCanvas->cd();
	
	fDrawOpt   = new TString[fNGraphs];
	fFill      = new Int_t[fNGraphs];
	fTitleModBid = new Int_t[fNGraphs];
	fFillColor = new Color_t[fNGraphs];
   fFillStyle = new Style_t[fNGraphs];
   fLineColor = new Color_t[fNGraphs];
   fLineStyle = new Style_t[fNGraphs];
   fLineWidth = new Width_t[fNGraphs];
   fMarkerColor = new Color_t[fNGraphs];
   fMarkerStyle = new Style_t[fNGraphs];
   fMarkerSize  = new Size_t[fNGraphs];
   fShowMarkers = new Int_t[fNGraphs];
	fLineMode    = new TString[fNGraphs];
	fErrorMode   = new TString[fNGraphs];
	fTitle       = new TString[fNGraphs];
	for ( Int_t i =0; i < fNGraphs; i++ ) {
		TGraph *gr =(TGraph*)fGraphList.At(i);
		if (i == 0 ) {
			fTitleX = gr->GetHistogram()->GetXaxis()->GetTitle();
			fTitleY = gr->GetHistogram()->GetYaxis()->GetTitle();
		}
		fTitle[i] = gr->GetTitle();
		fDrawOpt[i]   = gr->GetDrawOption();
		if (gHprDebug > 0)
			cout << "GraphAttDialog GetDrawOption() " << i << ": " << fDrawOpt[i] << endl;
		fFillColor[i] = gr->GetFillColor(); 
		fFillStyle[i] = gr->GetFillStyle(); 
		fLineColor[i] = gr->GetLineColor(); 
		fLineStyle[i] = gr->GetLineStyle(); 
		fLineWidth[i] = gr->GetLineWidth(); 
		fMarkerColor[i] =  gr->GetMarkerColor(); 
		fMarkerStyle[i] =  gr->GetMarkerStyle(); 
		fMarkerSize[i]  =  gr->GetMarkerSize(); 
		if (     fDrawOpt[i].Contains("|>"))
			fErrorMode[i] = "|>";
		else if (fDrawOpt[i].Contains(">"))
			fErrorMode[i] = ">";
		else if (fDrawOpt[i].Contains("||"))
			fErrorMode[i] = "||(only end)";
		else if (fDrawOpt[i].Contains("Z", TString::kIgnoreCase))
			fErrorMode[i] = "Z(no XErr)";
		else if (fDrawOpt[i].Contains("X", TString::kIgnoreCase))
			fErrorMode[i] = "X(no Err)";
		else if (fDrawOpt[i].Contains("2"))
			fErrorMode[i] = "2";
		else if (fDrawOpt[i].Contains("3"))
			fErrorMode[i] = "3";
		else if (fDrawOpt[i].Contains("4"))
			fErrorMode[i] = "4";
		if (     fDrawOpt[i].Contains("C", TString::kIgnoreCase))
			fLineMode[i] = "C(smooth)";
		else if (fDrawOpt[i].Contains("L", TString::kIgnoreCase))
			fLineMode[i] = "L(simple)";
		else
			fLineMode[i] = "(noline)";
		if (     fDrawOpt[i].Contains("P", TString::kIgnoreCase))
			fShowMarkers[i] = 1;
		else
			fShowMarkers[i] = 0;
		if (     fDrawOpt[i].Contains("F", TString::kIgnoreCase))
			fFill[i] = 1;
		else
			fFill[i] = 0;
	}
//	fEndErrorSize = gStyle->GetEndErrorSize();
//	fErrorX       = gStyle->GetErrorX();
	fShowAxis     = 1;
	fShowTitle    = 1;
	fRow_lab = new TList();
   Int_t ind = 0;
   static Int_t dummy;
//   static TString stycmd("SetAsDefault()");
//   RestoreDefaults();
	fRow_lab->Add(new TObjString("CheckButton_Show Axis   "));
	fValp[ind++] = &fShowAxis;
	fRow_lab->Add(new TObjString("CheckButton+Show Title  "));
	fValp[ind++] = &fShowTitle;
	fRow_lab->Add(new TObjString("Float_Value+EndErrS "));
	fValp[ind++] = &fEndErrorSize;
	fRow_lab->Add(new TObjString("Float_Value+X ErrS"));
	fValp[ind++] = &fErrorX;
	fRow_lab->Add(new TObjString("StringValue_Title X "));
	fValp[ind++] = &fTitleX;
	fRow_lab->Add(new TObjString("StringValue+Title Y "));
	fValp[ind++] = &fTitleY;
	
	for ( Int_t i =0; i < fNGraphs; i++ ) {
		if (gHprDebug > 0)
			cout << "fLineMode[0] " << fLineMode[0]<< endl;
		TString comment("CommentOnly_GraphNo ");
		comment +=i;
		fRow_lab->Add(new TObjString(comment));
		fValp[ind++] = &dummy;
		fRow_lab->Add(new TObjString("StringValue+Graph Title"));
		fTitleModBid[i] = ind;
		fValp[ind++] = &fTitle[i];
		fRow_lab->Add(new TObjString("ComboSelect_ErrMo;(default);X(no Err);Z(no XErr);>;|>;||(only end);2;3;4"));
		fValp[ind++] = &fErrorMode[i];
		fRow_lab->Add(new TObjString("CheckButton+DoFill"));
		fValp[ind++] = &fFill[i];
		fRow_lab->Add(new TObjString("Fill_Select+FStyle"));
		fValp[ind++] = &fFillStyle[i];
		fRow_lab->Add(new TObjString("ColorSelect+FColor"));
		fValp[ind++] = &fFillColor[i];
		
		fRow_lab->Add(new TObjString("ComboSelect_LineM;(noline);L(simple);C(smooth)"));
		fValp[ind++] = &fLineMode[i];
		fRow_lab->Add(new TObjString("LineSSelect+LStyle"));
		fValp[ind++] = &fLineStyle[i];
		fRow_lab->Add(new TObjString("PlainShtVal+LWidth"));
		fValp[ind++] = &fLineWidth[i];
		fRow_lab->Add(new TObjString("ColorSelect+LColor"));
		fValp[ind++] = &fLineColor[i];
		fRow_lab->Add(new TObjString("CheckButton_ShowMark"));
		fValp[ind++] = &fShowMarkers[i];
		fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
		fValp[ind++] = &fMarkerStyle[i];
		fRow_lab->Add(new TObjString("Float_Value+MSize ;0;10"));
		fValp[ind++] = &fMarkerSize[i];
		fRow_lab->Add(new TObjString("ColorSelect+MColor"));
		fValp[ind++] = &fMarkerColor[i];
	}
	fRow_lab->Add(new TObjString("CheckButton_Log X scale"));
	fValp[ind++] = &fLogX;
	fRow_lab->Add(new TObjString("CheckButton+Log Y scale"));
	fValp[ind++] = &fLogY;
	/*	
	Int_t logz = ind;
	fRow_lab->Add(new TObjString("CheckButton+Log Z scale"));
	fValp[ind++] = &fLogZ;*/
//	fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
//   fValp[ind++] = &stycmd;

//	fRow_lab->Add(new TObjString("CheckButton_ylow=rwymin"};
 //  fValp[ind++] = &;

   static Int_t ok;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
//	fDialog->DisableButton(logz);
}
//_______________________________________________________________________

GraphAttDialog::~GraphAttDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________

void GraphAttDialog::CloseDialog()
{
//   cout << "GraphAttDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void GraphAttDialog::SetGraphAtt(TCanvas *ca, Int_t bid)
{
	TCanvas * canvas;
	if ( ca == NULL )
		canvas = fCanvas;
	else
		canvas = ca;
	gStyle->SetEndErrorSize(fEndErrorSize);
	gStyle->SetErrorX(fErrorX);
	if (fShowTitle) 
		gStyle->SetOptTitle(kTRUE);
	else 
		gStyle->SetOptTitle(kFALSE);
	canvas->cd();
	TString drawopt;
	TString errmod;
	
	TList * leg_entries = NULL;
	TLegend * leg = (TLegend*)canvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeGraph");
	if ( leg ) {
		leg_entries = leg->GetListOfPrimitives();
		if ( leg_entries->GetSize() != fNGraphs ) 
			leg_entries = NULL;
	}
	for ( Int_t i =0; i < fNGraphs; i++ ) {
		TGraph *gr =(TGraph*)fGraphList.At(i);
		if (i == 0 ) {
			gr->GetHistogram()->GetXaxis()->SetTitle(fTitleX);
			gr->GetHistogram()->GetYaxis()->SetTitle(fTitleY);
		}
		drawopt = fLineMode[i];
		if (drawopt.Index("(") >= 0)
			drawopt.Resize(drawopt.Index("("));
		errmod = fErrorMode[i];
		if (errmod.Index("(") >= 0)
			errmod.Resize(errmod.Index("("));
		if ( i == 0 && fShowAxis )
			drawopt += "A";
		if (fFill[i] && TMath::Abs(fLineWidth[i])<100) {
			drawopt += "F";
			if ( fFillStyle[i] == 0 )
				fFillStyle[i] = 3001;
		} 
		drawopt += errmod;
		drawopt += "P";
		if (fShowMarkers[i] == 0){
			gr->SetMarkerStyle(0);
		}	
//		cout << "drawopt |" << drawopt<< "| " << endl;
		gr->SetDrawOption(drawopt);
		gr->SetLineStyle(fLineStyle[i]);
		gr->SetLineWidth(fLineWidth[i]);
		gr->SetLineColor(fLineColor[i]);
		gr->SetFillStyle(fFillStyle[i]);
		if (fFill[i] ) 
			gr->SetFillColor(fFillColor[i]);
		else 
			gr->SetFillColor(0);
		if (fShowMarkers[i]) {
			gr->SetMarkerStyle(fMarkerStyle[i]);
			gr->SetMarkerSize (fMarkerSize[i] );
			gr->SetMarkerColor(fMarkerColor[i]);
		}
		gr->SetTitle(fTitle[i]);
		if ( leg_entries != NULL  && bid == fTitleModBid[i] ) {
			((TLegendEntry*)leg_entries->At(i))->SetLabel(fTitle[i]);
		}
	}
	canvas->SetLogx(fLogX);
	canvas->SetLogy(fLogY);
	canvas->Modified();
	canvas->Update();
}
//_______________________________________________________________________

void GraphAttDialog::SetAsDefault()
{
	SaveDefaults();
}
//_______________________________________________________________________

void GraphAttDialog::SaveDefaults()
{
   TEnv env(".hprrc");  
   env.SetValue("GraphAttDialog.fDrawOpt",    fDrawOpt[0]);
	env.SetValue("GraphAttDialog.fFill",       fFill[0]);
	env.SetValue("GraphAttDialog.fShowMarkers",fShowMarkers[0]);
	env.SetValue("GraphAttDialog.fShowTitle",  fShowTitle);
	env.SetValue("GraphAttDialog.fShowAxis",   fShowAxis);
	env.SetValue("GraphAttDialog.fLineStyle",  fLineStyle[0]);
	env.SetValue("GraphAttDialog.fLineWidth",  fLineWidth[0]);
	env.SetValue("GraphAttDialog.fLineColor",  fLineColor[0]);
	env.SetValue("GraphAttDialog.fMarkerStyle",fMarkerStyle[0]);
	env.SetValue("GraphAttDialog.fMarkerSize", fMarkerSize[0]);
	env.SetValue("GraphAttDialog.fMarkerColor",fMarkerColor[0]);
	env.SetValue("GraphAttDialog.fFillStyle",  fFillStyle[0]);
	env.SetValue("GraphAttDialog.fFillColor",  fFillColor[0]);
	env.SetValue("GraphAttDialog.fLogX",       fLogX);
	env.SetValue("GraphAttDialog.fLogY",       fLogY);
//	env.SetValue("GraphAttDialog.fLogZ",       fLogZ);
// 	env.SetValue("GraphAttDialog.fErrorMode",  fErrorMode[0]);
	env.SetValue("GraphAttDialog.fEndErrorSize",fEndErrorSize);
	env.SetValue("GraphAttDialog.fErrorX",      fErrorX);
	env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void GraphAttDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
	
	fShowTitle    = env.GetValue("GraphAttDialog.fShowTitle",   1);
	fEndErrorSize = env.GetValue("GraphAttDialog.fEndErrorSize", 1.);
	fErrorX       = env.GetValue("GraphAttDialog.fErrorX",       1.);
	/*
	fDrawOpt    = env.GetValue("GraphAttDialog.fDrawOpt", "A*");
   fLineStyle     = env.GetValue("GraphAttDialog.fLineStyle", 1);
   fLineWidth     = env.GetValue("GraphAttDialog.fLineWidth", 1);
   fLineColor     = env.GetValue("GraphAttDialog.fLineColor", 1);
   fMarkerStyle   = env.GetValue("GraphAttDialog.fMarkerStyle", 7);
   fMarkerSize    = env.GetValue("GraphAttDialog.fMarkerSize",  1);
   fMarkerColor   = env.GetValue("GraphAttDialog.fMarkerColor", 1.);
	fFillStyle     = env.GetValue("GraphAttDialog.fFillStyle", 0);
	fFillColor     = env.GetValue("GraphAttDialog.fFillColor", 1);
	
   fSmoothLine = env.GetValue("GraphAttDialog.fSmoothLine", 0);
   fSimpleLine = env.GetValue("GraphAttDialog.fSimpleLine", 0);
	fFill       = env.GetValue("GraphAttDialog.fFill", 0);
   fShowMarkers = env.GetValue("GraphAttDialog.fShowMarkers", 1);
   fBarChart    = env.GetValue("GraphAttDialog.fBarChart", 0);
   fShowAxis   = env.GetValue("GraphAttDialog.fShowAxis", 1);
*/
	fLogX       = env.GetValue("GraphAttDialog.fLogX", 0);
	fLogY       = env.GetValue("GraphAttDialog.fLogY", 0);
	/*
	fLogZ       = env.GetValue("GraphAttDialog.fLogZ", 0);
	fErrorMode       = env.GetValue("GraphAttDialog.fErrorMode",    "");
	fEndErrorSize    = env.GetValue("GraphAttDialog.fEndErrorSize",  1);
	fErrorX          = env.GetValue("GraphAttDialog.fErrorX",        1);
	*/
//	gStyle->SetOptLogx      (fLogX);
//	gStyle->SetOptLogy      (fLogY);
}
//______________________________________________________________________

void GraphAttDialog::CloseDown(Int_t wid)
{
	if ( gHprDebug > 0 )
		cout << "CloseDown(" << wid<< ")" <<endl;
	SetGraphAtt();
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void GraphAttDialog::CRButtonPressed(Int_t wid, Int_t bid, TObject *obj)
{
	TCanvas *canvas = (TCanvas *)obj;
	if ( gHprDebug > 0 ) {
		cout << ClassName() << " CRButtonPressed(" << wid << ", " << bid;
		if (obj) cout  << ", " << canvas->GetName() << ")";
		cout << endl;
	}
   SetGraphAtt(canvas, bid);
}


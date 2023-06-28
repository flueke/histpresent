#include "TROOT.h"
#include "THprArrow.h"
#include "THprCurlyArc.h"
#include "THprCurlyLine.h"
#include "THprLine.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "FeynmanDiagramDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(FeynmanDiagramDialog)

FeynmanDiagramDialog::FeynmanDiagramDialog()
{
static const Char_t helptext[] =
"This is a collection for graphics primitives\n\
used to draw feynman diagrams. Note: Sizes are\n\
defined as fraction relative to the pad size.\n\
Arcs are defined by clicking at the center and\n\
dragging the pressed mouse to a point on the \n\
circumference. Start and end point are defined\n\
by Phi start / end. (counterclock wise)\n\
";
	fCanvas = gPad->GetCanvas();
	TRootCanvas* win = NULL;
	if (fCanvas) {
		win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "FeynmanDiagramDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
	RestoreDefaults();
	fRow_lab = new TList();
	static TString far("FeynmanArrow()");
	static TString fwl("FeynmanWavyLine()");
	static TString fcl("FeynmanCurlyLine()");
	static TString fwa("FeynmanWavyArc()");
	static TString fca("FeynmanCurlyArc()");
	static TString fsl("FeynmanSolidLine()");
	static TString fdl("FeynmanDashedLine()");

	fRow_lab->Add(new TObjString("DoubleValue_WaveL"));
	fValp[ind++] = &fWaveLength;
	fRow_lab->Add(new TObjString("DoubleValue+Amp"));
	fValp[ind++] = &fAmplitude;
	fRow_lab->Add(new TObjString("ArrowSelect_AShape"));
	fValp[ind++] = &fArrowStyle;
	fRow_lab->Add(new TObjString("Float_Value+ASize"));
	fValp[ind++] = &fArrowSize;
	fRow_lab->Add(new TObjString("ColorSelect_Color"));
	fValp[ind++] = &fColor;
	fRow_lab->Add(new TObjString("PlainShtVal+Width"));
	fValp[ind++] = &fWidth;
	fRow_lab->Add(new TObjString("CommandButt_Arrow()"));
	fValp[ind++] = &far;
	fRow_lab->Add(new TObjString("CommandButt+Line()"));
	fValp[ind++] = &fsl;
	fRow_lab->Add(new TObjString("CommandButt+DashedL()"));
	fValp[ind++] = &fdl;
	fRow_lab->Add(new TObjString("CommandButt_WavyLine()"));
	fValp[ind++] = &fwl;
	fRow_lab->Add(new TObjString("CommandButt+CurlyLine()"));
	fValp[ind++] = &fcl;
	fRow_lab->Add(new TObjString("CommandButt_WavyArc()"));
	fValp[ind++] = &fwa;
	fRow_lab->Add(new TObjString("CommandButt+CurlyArc()"));
	fValp[ind++] = &fca;
	fRow_lab->Add(new TObjString("PlainIntVal_Phi start"));
	fValp[ind++] = &fPhi1;
	fRow_lab->Add(new TObjString("PlainIntVal+Phi end"));
	fValp[ind++] = &fPhi2;
	Int_t itemwidth =  40 * TGMrbValuesAndText::LabelLetterWidth();
	static Int_t ok;
	fDialog =
		new TGMrbValuesAndText("Feynman diagram", NULL, &ok,itemwidth, win,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
	if (gPad) {
		 GrCanvas* hc = (GrCanvas*)fCanvas;
		 hc->Add2ConnectedClasses(this);
	}
}
//______________________________________________________________________________

void FeynmanDiagramDialog::FeynmanArrow()
{
	const char * ArrowOption[] =
		{" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
	TArrow * a = (TArrow*)GrCanvas::WaitForCreate("TArrow", &fPad);
//   if (a == NULL) {
//cout << "Interrupted TArrow" << endl;
//      return;
//   }
//   a = (TArrow *)gPad->GetListOfPrimitives()->Last();
	THprArrow * aa = new THprArrow(a->GetX1(), a->GetY1(),
											 a->GetX2(), a->GetY2());
	delete a;
	aa->Draw();
	aa->SetFillColor(fColor);
	aa->SetLineColor(fColor);
	aa->SetLineWidth(fWidth);
	aa->SetArrowSize(fArrowSize);
	aa->SetDrawOption(ArrowOption[fArrowStyle]);
	aa->SetOption(ArrowOption[fArrowStyle]);
	gPad->Modified();
	gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyLine()
{
	FeynmanCurlyWavyLine(0);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyLine()
{
	FeynmanCurlyWavyLine(1);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyWavyLine(Int_t curly)
{
	TCurlyLine * a = (TCurlyLine*)GrCanvas::WaitForCreate("TCurlyLine", &fPad);
	if (a == NULL) {
		return;
	}
	THprCurlyLine * ha =  new THprCurlyLine(a->GetStartX(), a->GetStartY(),
								 a->GetEndX(), a->GetEndY());
	delete a;
	ha->Draw();
	if (curly)
	  ha->SetCurly();
	else
	  ha->SetWavy();
	ha->SetAmplitude(fAmplitude);
	ha->SetWaveLength(fWaveLength);
	ha->SetLineColor(fColor);
	ha->SetLineWidth(fWidth);
	gPad->Modified();
	gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanWavyArc()
{
	FeynmanCurlyWavyArc(0);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyArc()
{
	FeynmanCurlyWavyArc(1);
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanCurlyWavyArc(Int_t curly)
{
//	TCurlyArc::SetDefaultIsCurly(kTRUE);
	TCurlyArc * a = (TCurlyArc*)GrCanvas::WaitForCreate("TCurlyArc", &fPad);
	if (a == NULL) {
		return;
	}
	THprCurlyArc * ha = new THprCurlyArc(a->GetStartX(), a->GetStartY(),
								 a->GetRadius(), a->GetPhimin(), a->GetPhimax());
	delete a;
	ha->Draw();

	if (curly)
	  ha->SetCurly();
	else
	  ha->SetWavy();
//   cout << "TCurlyArc" <<endl;
	ha->SetAmplitude(fAmplitude);
	ha->SetWaveLength(fWaveLength);
	ha->SetPhimin(fPhi1);
	ha->SetPhimax(fPhi2);
	ha->SetLineColor(fColor);
	ha->SetLineWidth(fWidth);
	gPad->Modified();
	gPad->Update();
};
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanSolidLine()
{
  FeynmanLine(kSolid);
}
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanDashedLine()
{
  FeynmanLine(7);
}
//______________________________________________________________________________
void FeynmanDiagramDialog::FeynmanLine(Style_t lstyle )
{
//   Style_t save = gStyle->GetLineStyle();
//   gStyle->SetLineStyle(kDashed);
	TLine * a = (TLine*)GrCanvas::WaitForCreate("TLine", &fPad);
	if (a == NULL) {
		return;
	}
	THprLine * la = new THprLine(a->GetX1(), a->GetY1(), a->GetX2(), a->GetY2());
	delete a;
	la->Draw();
	if (lstyle > 1 && fWidth > 3)lstyle = 9;
	la->SetLineStyle(lstyle);
	la->SetLineColor(fColor);
	la->SetLineWidth(fWidth);
	gPad->Modified();
	gPad->Update();
};
//______________________________________________________________________________

void FeynmanDiagramDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("FeynmanDiagramDialog.fWaveLength" , fWaveLength);
	env.SetValue("FeynmanDiagramDialog.fAmplitude"  , fAmplitude );
	env.SetValue("FeynmanDiagramDialog.fColor"		, fColor 	 );
	env.SetValue("FeynmanDiagramDialog.fWidth"		, fWidth 	 );
	env.SetValue("FeynmanDiagramDialog.fPhi1" 		, fPhi1  	 );
	env.SetValue("FeynmanDiagramDialog.fPhi2" 		, fPhi2  	 );
	env.SetValue("FeynmanDiagramDialog.fArrowSize"  ,fArrowSize  );
	env.SetValue("FeynmanDiagramDialog.fArrowStyle" ,fArrowStyle  );
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void FeynmanDiagramDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fWaveLength = env.GetValue("FeynmanDiagramDialog.fWaveLength" , 0.02);
	fAmplitude  = env.GetValue("FeynmanDiagramDialog.fAmplitude"  , 0.01);
	fColor   	= env.GetValue("FeynmanDiagramDialog.fColor"      , 1);
	fWidth   	= env.GetValue("FeynmanDiagramDialog.fWidth"  	  , 2);
	fPhi1	 	   = env.GetValue("FeynmanDiagramDialog.fPhi1"  	  , 0);
	fPhi2	 	   = env.GetValue("FeynmanDiagramDialog.fPhi2"  	  , 180);
	fArrowSize  = env.GetValue("FeynmanDiagramDialog.fArrowSize"  , 0.03);
	fArrowStyle  = env.GetValue("FeynmanDiagramDialog.fArrowStyle"  , 7);
}
//______________________________________________________________________

void FeynmanDiagramDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << " FeynmanDiagramDialog::CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
}

//_________________________________________________________________________

FeynmanDiagramDialog::~FeynmanDiagramDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if (fRow_lab ) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void FeynmanDiagramDialog::CloseDialog()
{
//   cout << "FeynmanDiagramDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void FeynmanDiagramDialog::CloseDown(Int_t wid)
{
//   cout << "FeynmanDiagramDialog::CloseDown()" << endl;
	if (wid != -2) SaveDefaults();
	delete this;
}


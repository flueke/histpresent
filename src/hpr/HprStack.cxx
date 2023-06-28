#include <TROOT.h>
#include <TEnv.h>
#include <TH1.h>
#include <TLegend.h>
#include <TString.h>
#include <TStyle.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TRegexp.h>
#include <TGMsgBox.h>
#include "SetColor.h"
#include "TGMrbValuesAndText.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "SetHistOptDialog.h"
#include "Set1DimOptDialog.h"
#include "SetCanvasAttDialog.h"
#include "HprStack.h"
#include "hprbase.h"

ClassImp (HprStack)
static Int_t gSeqNumberStack = 0;

enum EGoHCommandIds {
	M_RememberOptions,
	M_SetOptions,
	M_OptionAxisAtt,
	M_Option1Dim,
	M_Option2Dim,
	M_Option2DimCol,
	M_OptionHist,
	M_SuperImpose,
	M_OptionPad
};
//________________________________________________________________________

HprStack::HprStack()
{
	//   cout << " def ctor HprStack::" << this << endl;
	fCanvas = NULL;
	fDialog = NULL;
}
//________________________________________________________________________

HprStack::HprStack(TList * hlist, Int_t scaled)
				: fHList(hlist)
{
	static const Char_t helptext[] =
	"Apply scale to stacked histograms\n\
	Please consult: http://root.cern.ch/root/html532/TH1.html#TH1:Scale\n\
	";
	
	//   cout << " ctor HprStack::" << this << endl;
	fCanvas = NULL;
	fDialog = NULL;
	fScaled = scaled;
//
	fNhists = fHList->GetSize();
//   TH1* hist = 0;
	if (fNhists == 0) {
		cout << setred << "No histogram selected" << setblack << endl;
		return;

	} else if (fNhists > 32) {
		cout << setred << "Maximum 32 histograms allowed" << setblack << endl;
		return;
	}
	fFill_1Dim = new Int_t[fNhists];
	fFillColor = new Color_t[fNhists];
	fFillTrans = new Float_t[fNhists];
	fFillStyle = new Style_t[fNhists];
	fLineColor = new Color_t[fNhists];
	fLineTrans = new Float_t[fNhists];
	fLineStyle = new Style_t[fNhists];
	fLineWidth = new Width_t[fNhists];
	fMarkerColor = new Color_t[fNhists];
	fMarkerTrans = new Float_t[fNhists];
	fMarkerStyle = new Style_t[fNhists];
	fMarkerSize = new Size_t[fNhists];
	fMinScale   = new Double_t[fNhists];
	fMaxScale   = new Double_t[fNhists];
	
	fScales   = new Double_t[fNhists];
	fColorW    = new Color_t[fNhists];
	RestoreDefaults();
	
	TString hname_com;
	TH1  *hist;
	Int_t dummy;
	if ( scaled > 0 ) {
		fRow_labW = new TList();
		Int_t ind = 0;
	//	static Int_t dummy = 0;
		for(Int_t i=0; i<fNhists; i++) {
//			fScales[i] = 1.;
			hist = (TH1*)fHList->At(i);
			hname_com = "CommentOnly_";
			hname_com += hist->GetName();
			fRow_labW->Add(new TObjString(hname_com));
			fValpW[ind++] = &dummy;
			fRow_labW->Add(new TObjString("DoubleValue+Scale"));
			fValpW[ind++] = &fScales[i];
			fRow_labW->Add(new TObjString("ColorSelect+Color"));
			fValpW[ind++] = &fColorW[i];
		}
		Int_t itemwidth = 420;
		static Int_t ok = -3;   // wait until closed
		fDialogW =
		new TGMrbValuesAndText ("Scale histos", NULL, &ok, itemwidth,
										NULL, NULL, NULL, fRow_labW, fValpW,
										NULL, NULL, helptext);
	}								
	BuildCanvas();
	BuildMenu();
	gROOT->GetList()->Add(this);
	gROOT->GetListOfCleanups()->Add(this);
//	cout << "THStack *st = (THStack*)" << fStack << ";" << endl;
//	cout << "TObjArray *oa = (TObjArray*)" << fStack->GetStack() << ";" << endl;
	
}
//________________________________________________________________________

void HprStack::BuildCanvas()
{
	TString buf("cstack_");
	buf += gSeqNumberStack++;
	const char * tit = buf.Data();
//   fCanvas = new TCanvas(buf.Data(), tit, 
//		WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
//		fWindowXWidth, fWindowYWidth);
	Int_t lStackOpenGL_save = gStyle->GetCanvasPreferGL();
	if ( fStackOpenGL > 0) {
		gStyle->SetCanvasPreferGL(1);
	}
	fCanvas = new HTCanvas(buf.Data(), tit, 
		WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
		fWindowXWidth, fWindowYWidth);
	
	WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
	WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;
//   cout << "TCanvas *ca = (TCanvas*)" <<  fCanvas << endl;

	TString hname;
	TString stitle("Stacked: ");

//   TString ytitle("");
//   Int_t nbins = 0, binlx = 0, binux = 0, binly = 0, binuy = 0;
	fDim = -1;
	fStack = new THStack("hstack","");
//   cout << "THStack *st = (THStack*)" <<  fStack << endl;
	fNDrawn = 0;
	TH1 *orig_hist, *hist;
	for(Int_t i=0; i<fNhists; i++) {
		orig_hist = (TH1*)fHList->At(i);
		hist = (TH1*)orig_hist->Clone();
		if (!hist) {
//         cout << " Hist not found at: " << i << endl;
			continue;
		}
		if ( fDim < 0 ) {
			fDim = hist->GetDimension();
		} else if ( hist->GetDimension() != fDim ) {
			cout << " Hist has wrong dimension: " << hist->GetName() << endl;
			continue;
		}
		if ( fDim == 2 ) {
			hist->SetFillColor(fFillColor[i]);
			hist->SetFillStyle(1001);
		}
//      hist->Print();
		fMinScale[i] = hist->GetMinimum();
		fMaxScale[i] = hist->GetMaximum();
		hname = hist->GetName();
//      Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
//      if(last_us >0)hname.Remove(last_us);
		Int_t last_us = hname.Last(';');    // chop off version
		if (last_us >0) hname.Remove(last_us);
		hist->SetName(hname);
		if ( fScaled > 0 ) {
			hist->Scale(fScales[i]);
			cout << "TH1 *hist = (TH1*)" <<  hist << ";// added with scale: " 
			<< fScales[i] << endl;
		}
		fStack->Add(hist);
		fLastHist = hist;
		fNDrawn ++;
		stitle += hist->GetName();
		if ( i < fNhists - 1 ) stitle += "_";
	}
//   cout << " fOpt: " << fOpt<< endl;
	TString opt;
	if ( GeneralAttDialog::fStackedNostack )
		opt = "nostack";
	else if ( GeneralAttDialog::fStackedPads )
		opt = "pads";
	if ( fDim == 2) {
		opt += "lego1";
		fStack->Draw(opt);
	} else {
		fStack->Draw(opt);
	}
	SetAttributes();
	fStack->SetTitle(stitle);
	TLegend * leg = fCanvas->BuildLegend(fLegendX1, fLegendY1, fLegendX2, fLegendY2);
	leg->SetName("Legend_HprStack");
	fCanvas->Modified();
	fCanvas->Update();
	gStyle->SetCanvasPreferGL(lStackOpenGL_save);
}
//________________________________________________________________________

void HprStack::RecursiveRemove(TObject *obj)
{

	if (fCanvas && obj == fCanvas) {
//      cout <<  "HprStack::RecursiveRemove,this: " << this << " obj: "  << obj << " "  
//		     << obj->GetName() << endl;
		fWindowXWidth = fCanvas->GetWindowWidth();
		fWindowYWidth = fCanvas->GetWindowHeight();
		gROOT->GetList()->Remove(this);
		gROOT->GetListOfCleanups()->Remove(this);
 //     cout <<  "RecursiveRemove,fWindowXWidth  " << fWindowXWidth << endl;
		delete this;
	}
}
//________________________________________________________________________

HprStack::~HprStack()
{
//   cout <<"dtor HprStack, fDialog " << fDialog << endl;
	gROOT->GetList()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
	if (fDialog) {
		fDialog->CloseWindowExt();
		SaveDefaults();
		delete [] fFill_1Dim;
		delete [] fFillColor;
		delete [] fFillTrans;
		delete [] fFillStyle;
		delete [] fLineColor;
		delete [] fLineTrans;
		delete [] fLineWidth;
		delete [] fMarkerColor;
		delete [] fMarkerTrans;
		delete [] fMarkerStyle;
		delete [] fMarkerSize;
		delete [] fMinScale;
		delete [] fMaxScale;
		delete [] fScales;
		delete [] fColorW;
	}
}
//________________________________________________________________________

void HprStack::BuildMenu()
{
//   cout << "HprStack::BuildMenu() " <<this << endl;
	fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
	TGMenuBar * menubar = fRootCanvas->GetMenuBar();
	TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
	fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
	menubar->AddPopup("Display-Options", fMenu, layoh_left, menubar->GetPopup("Help"));
	fMenu->AddEntry("Stacks / Hists Attributes", M_SetOptions);
	fMenu->AddEntry("Axis / Title / StatBox Attributes", M_OptionHist);
	fMenu->AddEntry("Canvas, Pad, Frame", M_OptionPad);
	fMenu->AddEntry("Store changed Attributes", M_RememberOptions);
	if (fDim == 1) {
		fMenu->AddEntry("SuperImpose hist", M_SuperImpose);
		fMenu->AddEntry("Draw option for superimposed hist", M_Option1Dim);
		
	}
	fMenu->Connect("Activated(Int_t)", "HprStack", this,
								"HandleMenu(Int_t)");
	menubar->MapSubwindows();
	menubar->Layout();
}
//________________________________________________________________________

void HprStack::HandleMenu(Int_t id)
{
	switch (id) {

		case M_OptionHist:
			if ( GeneralAttDialog::fStackedPads ) {
				new SetHistOptDialog(fRootCanvas, fStack->GetHists());
			} else {
				TList *temp = new TList();
				temp->Add(fStack->GetHistogram());
				new SetHistOptDialog(fRootCanvas, temp);
			}
			break;
		case M_OptionPad:
			new SetCanvasAttDialog(fRootCanvas);
			break;
		case M_SetOptions:
			SetOptions();
			break;
		case M_RememberOptions:
			CRButtonPressed();
			break;
		case M_Option1Dim:
			new Set1DimOptDialog(fRootCanvas);
			break;
		case M_SuperImpose:
			Hpr::SuperImpose(fCanvas, fLastHist, 0);
			break;
	}
}
//_______________________________________________________
void HprStack::SetOptions()
{
static const Char_t helptext[] =
"This menu controls the parameters when stacking histograms\n\
\"Really stack\"\n\
The first histogram is drawn as it is, subsequent histograms\n\
in the selection are added up and each new sum is drawn\n\
\"Superimpose\"\n\
The selected histograms are drawn in the same picture\n\
\"One pad for each\"\n\
The canvas is divided and each histogram is drawn in a\n\
separate pad.\n\
For 2-dim histograms only \"Really stack\" is supported.\n\
\n\
Error Drawing Modes:\n\
E	Draw error bars.\n\
E0	Draw error bars. Markers are drawn for bins with 0 contents.\n\
E1	Draw error bars with perpendicular lines at the edges.\n\
	Length is controled by\" EndErrSize\".\n\
E2	Draw error bars with rectangles.\n\
E3	Draw a fill area through the end points of the vertical error bars.\n\
E4	Draw a smoothed filled area through the end points of the error bars.\n\
E5	Like E3 but ignore the bins with 0 contents.\n\
E6	Like E4 but ignore the bins with 0 contents.\n\
\n\
Options E3-E6: Choose: Contour Off and FillHist On to get area\n\
					filled between the error lines.\n\
X ErrorS controls drawing of error bars in X.\n\
A value of 0.5 draws a line X +- 0.5*BinWidth\n\
\n\
Option \"Same scale\" forces same range of Y-scale in case\n\
of \"One pad for each\"\n\
Option \"OpenGL\" activates use of the GL rendering in the canvas.\n\
This is needed to use variable transparency when superimposing\n\
histograms together with the \"Fill option\".\n\
This must be activated b e f o r e the canvas is drawn\n\
i.e. After activating this option the picture must be redrawn\n\
to become effective.\n\
On the file output it is visible with PDF, PNG, Gif, JPEG, SVG ... \n\
but not PostScript.\n\
";
	fRow_lab = new TList();
//	static void *fValp[50];
	Int_t ind = 0;
//	static Int_t dummy = 0;
	fRow_lab->Add(new TObjString("RadioButton_Really stack"));
	fValp[ind++] = &GeneralAttDialog::fStackedReally;
	fRow_lab->Add(new TObjString("RadioButton+Superimpose"));
	fValp[ind++] = &GeneralAttDialog::fStackedNostack;
	fRow_lab->Add(new TObjString("RadioButton+One pad for each"));
	fValp[ind++] = &GeneralAttDialog::fStackedPads;
	fRow_lab->Add(new TObjString("PlainIntVal_WinWid"));
	fValp[ind++] = &fWindowXWidth;
	fRow_lab->Add(new TObjString("PlainIntVal+WinHei"));
	fValp[ind++] = &fWindowYWidth;
	fRow_lab->Add(new TObjString("DoubleValue+Ymin"));
	fValp[ind++] = &fStackMin;
	fRow_lab->Add(new TObjString("DoubleValue+Ymin"));
	fValp[ind++] = &fStackMax;
	fRow_lab->Add(new TObjString("ComboSelect_Error Mode;none;E;E1;E2;E3;E4;E5;E6"));
	fValp[ind++] = &fErrorMode;
	fRow_lab->Add(new TObjString("Float_Value+EndErrSize "));
	fValp[ind++] = &fEndErrorSize;
	fRow_lab->Add(new TObjString("Float_Value+X ErrorS"));
	fValp[ind++] = &fErrorX;
	fRow_lab->Add(new TObjString("CheckButton_Show Contour"));
	fValp[ind++] = &fShowContour;
	fRow_lab->Add(new TObjString("CheckButton+Draw Markers"));
	fValp[ind++] = &fShowMarkers;
	fRow_lab->Add(new TObjString("CheckButton+Same scale"));
	fValp[ind++] = &fSameScale;
	fRow_lab->Add(new TObjString("CheckButton+OpenGL"));
	fValp[ind++] = &fStackOpenGL;
	TString lab;
	for ( Int_t i = 0; i < fNDrawn; i++ ) {
		lab = "CheckButton_DoFill [";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fFill_1Dim[i];
		lab = "Fill_Select+fFillSt[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fFillStyle[i];
		lab = "ColorSelect+FillCol[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fFillColor[i];
		lab = "Float_Value+Transp;0.;1.";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fFillTrans[i];

		lab = "LineSSelect_LineSty[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fLineStyle[i];
		lab = "PlainShtVal+LineWid[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fLineWidth[i];
		lab = "ColorSelect+LineCol[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fLineColor[i];
		lab = "Float_Value+Transp;0.;1.";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fLineTrans[i];

		lab = "Mark_Select_MarkSty[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fMarkerStyle[i];
		lab = "Float_Value+fMarkSi[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fMarkerSize[i];
		lab = "ColorSelect+MarkCol[";
		lab += i; lab += "]";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fMarkerColor[i];
		lab = "Float_Value+Transp;0.;1.";
		fRow_lab->Add(new TObjString(lab));
		fValp[ind++] = &fMarkerTrans[i];
	}
	Int_t itemwidth = 530;
	static Int_t ok = 0;
	fDialog =
		new TGMrbValuesAndText ("Set Params", NULL, &ok, itemwidth,
								fRootCanvas, NULL, NULL, fRow_lab, fValp,
								NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________
void HprStack::RestoreDefaults()
{
	TEnv env(".hprrc");
	fWindowXWidth = env.GetValue("HprStack.fWindowXWidth", 800);
	fWindowYWidth = env.GetValue("HprStack.fWindowYWidth", 600);
	fStackMin = env.GetValue("HprStack.fStackMin", -1111);
	fStackMax = env.GetValue("HprStack.fStackMax", -1111);
	if (fWindowXWidth < 20 || fWindowXWidth > 1600) fWindowXWidth = 800;
	if (fWindowYWidth < 20 || fWindowYWidth > 1200) fWindowYWidth = 600;
	fLegendX1 = env.GetValue("HprStack.fLegendX1", 0.7);
	fLegendX2 = env.GetValue("HprStack.fLegendX2", 0.95);
	fLegendY1 = env.GetValue("HprStack.fLegendY1", 0.85);
	fLegendY2 = env.GetValue("HprStack.fLegendY2", 0.95);
	fEndErrorSize  = env.GetValue("Set1DimOptDialog.fEndErrorSize", 1.);
	fErrorX        = env.GetValue("Set1DimOptDialog.fErrorX", 0.);
	fShowContour   = env.GetValue("HprStack.fShowContour", 0);
	fErrorMode     = env.GetValue("Set1DimOptDialog.fErrorMode", "none");
	fShowMarkers   = env.GetValue("HprStack.fShowMarkers", 0);
	fSameScale     = env.GetValue("HprStack.fSameScale", 0);
	fStackOpenGL   = env.GetValue("HprStack.fStackOpenGL", 1);

	TString lab;
	for ( Int_t i = 0; i < fNhists; i++ ) {
		lab = "HprStack.fFill_1Dim["; lab+=i; lab+="]";
		fFill_1Dim[i] = 0;
		if (env.Lookup(lab))
			fFill_1Dim[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fFillColor["; lab+=i; lab+="]";
		fFillColor [i] = i+2;
		if (env.Lookup(lab))
			fFillColor[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fFillTrans["; lab+=i; lab+="]";
		fFillTrans[i] =env.GetValue(lab, 1.);
		lab = "HprStack.fFillStyle["; lab+=i; lab+="]";
		fFillStyle[i] = 3001;
		if (env.Lookup(lab)) {
			fFillStyle[i]  = env.GetValue(lab, 0);
		}
		lab = "HprStack.fLineColor["; lab+=i; lab+="]";
		fLineColor[i] = i+2;
		if (env.Lookup(lab)){
			fLineColor[i]  = env.GetValue(lab, 0);
			fColorW[i] =  fLineColor[i];
		}
		lab = "HprStack.fLineTrans["; lab+=i; lab+="]";
		fLineTrans[i] =  env.GetValue(lab, 1.);
		lab = "HprStack.fLineStyle["; lab+=i; lab+="]";
		fLineStyle[i] = 1;
		if (env.Lookup(lab))
			fLineStyle[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fLineWidth["; lab+=i; lab+="]";
		fLineWidth[i] = 1;
		if (env.Lookup(lab))
			fLineWidth[i]  = env.GetValue(lab, 0);

		lab = "HprStack.fMarkerColor["; lab+=i; lab+="]";
		fMarkerColor[i] = i+2;
		if (env.Lookup(lab))
			fMarkerColor[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fMarkerTrans["; lab+=i; lab+="]";
		fMarkerTrans[i] =  env.GetValue(lab, 1.);
		lab = "HprStack.fMarkerStyle["; lab+=i; lab+="]";
		fMarkerStyle[i] = 1;
		if (env.Lookup(lab))
			fMarkerStyle[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fMarkerSize["; lab+=i; lab+="]";
		fMarkerSize[i] = 1;
		if (env.Lookup(lab))
			fMarkerSize[i]  = env.GetValue(lab, 0);
		lab = "HprStack.fScales["; lab+=i; lab+="]";
		fScales[i] = 1.;
		if (env.Lookup(lab))
			fScales[i]  = env.GetValue(lab, 1.);
	}
}
//_______________________________________________________

void HprStack::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("HprStack.fWindowXWidth", fWindowXWidth);
	env.SetValue("HprStack.fWindowYWidth", fWindowYWidth);
	env.SetValue("HprStack.fStackMin", fStackMin);
	env.SetValue("HprStack.fStackMax", fStackMax);
/*	TLegend *leg = (TLegend*)fCanvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
	if ( leg ) {
		fLegendX1 = leg->GetX1NDC();
		//      cout << "fLegendX1 " << fLegendX1<< endl;
		fLegendX2 = leg->GetX2NDC();
		fLegendY1 = leg->GetY1NDC();
		fLegendY2 = leg->GetY2NDC();
	}*/
/*	env.SetValue("HprStack.fLegendX1", fLegendX1);
	env.SetValue("HprStack.fLegendX2", fLegendX2);
	env.SetValue("HprStack.fLegendY1", fLegendY1);
	env.SetValue("HprStack.fLegendY2", fLegendY2);*/
	env.SetValue("Set1DimOptDialog.fEndErrorSize" , fEndErrorSize);
	env.SetValue("Set1DimOptDialog.fErrorX",        fErrorX);
	env.SetValue("HprStack.fShowContour", fShowContour);
	env.SetValue("Set1DimOptDialog.fErrorMode",     fErrorMode);
	env.SetValue("HprStack.fShowMarkers", fShowMarkers );
	env.SetValue("HprStack.fSameScale",   fSameScale );
	env.SetValue("HprStack.fStackOpenGL",   fStackOpenGL );
	
	TString lab;
	for ( Int_t i = 0; i < fNhists; i++ ) {
		lab = "HprStack.fFill_1Dim["; lab+=i; lab+="]";
		env.SetValue(lab,fFill_1Dim[i]);
		lab = "HprStack.fFillColor["; lab+=i; lab+="]";
		env.SetValue(lab,fFillColor[i]);
		lab = "HprStack.fFillTrans["; lab+=i; lab+="]";
		env.SetValue(lab,fFillTrans[i]);
		lab = "HprStack.fFillStyle["; lab+=i; lab+="]";
		env.SetValue(lab,fFillStyle[i]);

		lab = "HprStack.fLineColor["; lab+=i; lab+="]";
		env.SetValue(lab,fLineColor[i]);
		lab = "HprStack.fLineTrans["; lab+=i; lab+="]";
		env.SetValue(lab,fLineTrans[i]);
		lab = "HprStack.fLineStyle["; lab+=i; lab+="]";
		env.SetValue(lab,fLineStyle[i]);
		lab = "HprStack.fLineWidth["; lab+=i; lab+="]";
		env.SetValue(lab,fLineWidth[i]);

		lab = "HprStack.fMarkerColor["; lab+=i; lab+="]";
		env.SetValue(lab,fMarkerColor[i]);
		lab = "HprStack.fMarkerTrans["; lab+=i; lab+="]";
		env.SetValue(lab,fMarkerTrans[i]);
		lab = "HprStack.fMarkerStyle["; lab+=i; lab+="]";
		env.SetValue(lab,fMarkerStyle[i]);
		lab = "HprStack.fMarkerSize["; lab+=i; lab+="]";
		env.SetValue(lab,fMarkerSize[i]);
		lab = "HprStack.fScales["; lab+=i; lab+="]";
		env.SetValue(lab,fScales[i]);
	}
	GeneralAttDialog::SaveDefaults();
	env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void HprStack::CloseDown(Int_t)
{
//   cout << "HprStack::CloseDown() fDialog " << fDialog<< endl;
	fDialog = NULL;
}
//_______________________________________________________________________
void HprStack::CRButtonPressed(Int_t /*widgetId*/, Int_t /*buttonId*/, TObject */*obj*/)
{
//   cout << "HprStack::CRButtonPressed " << widgetId << " " << buttonId << endl;
	fWindowXWidth = (Int_t)fCanvas->GetWindowWidth();
	fWindowYWidth = (Int_t)fCanvas->GetWindowHeight();
	TLegend *leg = (TLegend*)fCanvas->GetListOfPrimitives()->FindObject("Legend_HprStack");
	if ( leg ) {
		fLegendX1 = leg->GetX1NDC();
//      cout << "fLegendX1 " << fLegendX1<< endl;
		fLegendX2 = leg->GetX2NDC();
		fLegendY1 = leg->GetY1NDC();
		fLegendY2 = leg->GetY2NDC();
	}
	SetAttributes();
	SaveDefaults();
	fCanvas->Modified();
	fCanvas->Update();
}
//_______________________________________________________

void HprStack::SetAttributes()
{
	TObjArray * stack = fStack->GetStack();
	TList * orighist = fStack->GetHists();
	TString opt;
	if (fShowContour) opt += "HIST";
	if (fErrorMode != "none") {
		opt += fErrorMode;
	}
	gStyle->SetEndErrorSize (fEndErrorSize );
	gStyle->SetErrorX       (fErrorX       );
//	Int_t idx;
	Double_t min = 1.E20, max = -1.E20;
	for(Int_t i=0; i<fNDrawn; i++) {
		TH1 * hist = (TH1*)stack->At(i);
		TH1 * ohist = (TH1*)orighist->At(i);
//		TObjLink *objlnk = orighist->FindLink(ohist, idx);
		if ( fMinScale[i] < min)
			min = fMinScale[i];
		if (fMaxScale[i] > max)
			max = fMaxScale[i];
		
		if ( fDim  == 2 ) {
			hist->SetFillColor(fFillColor[i]);
			ohist->SetFillColor(fFillColor[i]);
			hist->SetFillStyle(1001);
		} else {
			if (TMath::Abs(fStackMin+1111) >0.001)
				fStack->SetMinimum(fStackMin);
			if (TMath::Abs(fStackMax+1111) >0.001)
				fStack->SetMaximum(fStackMax);
			if ( fFill_1Dim[i] ) {
				hist->SetFillStyle(fFillStyle[i]);
#if ROOTVERSION >= 53418
				hist->SetFillColorAlpha(fFillColor[i], fFillTrans[i]);
				ohist->SetFillColorAlpha(fFillColor[i], fFillTrans[i]);
#else
				hist->SetFillColor(fFillColor[i]);
				ohist->SetFillColor(fFillColor[i]);
#endif
				hist->SetFillStyle(fFillStyle[i]);
				ohist->SetFillStyle(fFillStyle[i]);
				if ( !opt.Contains("HIST", TString::kIgnoreCase) )
					opt += "HIST";
//				ohist->SetFillColorAlpha(fFillColor[i], fFillTrans[i]);
//       cout  << " fFillColor " << fFillColor[i]  <<  " fFillTrans "<< fFillTrans[i]<< endl;
			} else {
				hist->SetFillStyle(0);
				ohist->SetFillStyle(0);
				hist->SetFillColor(0);
				ohist->SetFillColor(0);
			}
			hist->SetLineColor(fLineColor[i]);
			hist->SetLineStyle(fLineStyle[i]);
			hist->SetLineWidth(fLineWidth[i]);
#if ROOTVERSION >= 53418
			ohist->SetLineColorAlpha(fLineColor[i], fLineTrans[i]);
#else
			ohist->SetLineColor(fLineColor[i]);
#endif
			ohist->SetLineStyle(fLineStyle[i]);
			ohist->SetLineWidth(fLineWidth[i]);
//			cout << "ohist->GetOption() " << ohist->GetOption()<< endl;
			ohist->SetOption(opt);
			if ( fShowMarkers ) {
				hist->SetMarkerStyle(fMarkerStyle[i]);
				hist->SetMarkerColor(fMarkerColor[i]);
				hist->SetMarkerSize(fMarkerSize[i]);
				ohist->SetMarkerStyle(fMarkerStyle[i]);
#if ROOTVERSION >= 53418
				ohist->SetMarkerColorAlpha(fMarkerColor[i], fMarkerTrans[i]);
#else
				ohist->SetMarkerColor(fMarkerColor[i]);
#endif
				ohist->SetMarkerSize(fMarkerSize[i]);
			} else {
				hist->SetMarkerSize(0);
				ohist->SetMarkerSize(0);
				hist->SetMarkerStyle(1);
				ohist->SetMarkerStyle(1);
			}
		}
	}
	if ( GeneralAttDialog::fStackedPads) {
//		max *= 1.05;
//		cout << min << " " << max << endl;
		for(Int_t i=0; i<fNDrawn; i++) {
			TH1 * hist = (TH1*)orighist->At(i);
			if ( fSameScale ) {
				hist->SetMinimum(min);
				hist->SetMaximum(1.05 * max);
			} else {
				hist->SetMinimum(fMinScale[i]);
				hist->SetMaximum(1.05 * fMaxScale[i]);
			}
		}
	}
	fCanvas->cd();
	if ( GeneralAttDialog::fStackedNostack ) {	
		opt += "nostack";
		//	   ClearSubPads();
	} else if ( GeneralAttDialog::fStackedPads ) {
		opt += "pads";
	} else {
		//	   ClearSubPads();
	}
//	cout << "DrawOpt: "<< opt << endl;
	if ( fDim == 2 ) {
	  opt = "lego1";
	  fStack->SetDrawOption(opt);
	} else {
		fStack->SetDrawOption(opt);
	}
 //  if ( !GeneralAttDialog::fStackedPads ) 
		ClearSubPads();
	fCanvas->Modified();
	fCanvas->Update();
}
//_______________________________________________________

void HprStack::ClearSubPads()
{
	fCanvas->cd();
	TList * pl = fCanvas->GetListOfPrimitives();
	TList temp;
	TIter nextpad(pl);
	TObject * p;
	while ( (p = nextpad()) ) {
		if (p->InheritsFrom("TPad")) {
			temp.Add(p);
		}
	}
	fCanvas->cd();
//	cout << "ClearSubPads " << temp.GetSize()<< endl;
	if (temp.GetSize() > 0) {
		temp.Delete();
	}
	fCanvas->Modified();
	fCanvas->Update();
}

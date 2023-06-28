#include "TROOT.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaletteAxis.h"
#include "TPavesText.h"
#include "TLegend.h"
#include "hprbase.h"
#include "SetHistOptDialog.h"
#include <iostream>

extern Int_t gHprDebug;

namespace std {} using namespace std;

ClassImp(SetHistOptDialog)
/*
Int_t   SetHistOptDialog::fFill1Dim      = 0;
Color_t SetHistOptDialog::fHistFillColor = 0;
Color_t SetHistOptDialog::fHistLineColor = 1;
Style_t SetHistOptDialog::fHistFillStyle = 0;
Style_t SetHistOptDialog::fHistLineStyle = 1;
Width_t SetHistOptDialog::fHistLineWidth = 2;
Float_t SetHistOptDialog::fEndErrorSize  =0.1;
Int_t   SetHistOptDialog::fErrorX        = 1;
Int_t   SetHistOptDialog::fDrawErrors       = 1;
Int_t    SetHistOptDialog::fNdivisionsX = 510;
Int_t    SetHistOptDialog::fNdivisionsY = 510;
Int_t    SetHistOptDialog::fNdivisionsZ = 510;
Color_t  SetHistOptDialog::fAxisColor = 0;
Color_t  SetHistOptDialog::fLabelColor = 0;
Style_t  SetHistOptDialog::fLabelFont = 0;
Float_t  SetHistOptDialog::fLabelOffsetX = 0;
Float_t  SetHistOptDialog::fLabelOffsetY = 0;
Float_t  SetHistOptDialog::fLabelOffsetZ = 0;
Float_t  SetHistOptDialog::fLabelSize = 0;
Float_t  SetHistOptDialog::fTickLength = 0;
Float_t  SetHistOptDialog::fTitleOffsetX = 0;
Float_t  SetHistOptDialog::fTitleOffsetY = 0;
Float_t  SetHistOptDialog::fTitleOffsetZ = 0;
Float_t  SetHistOptDialog::fTitleSize = 0;
Color_t  SetHistOptDialog::fTitleColor = 0;
Style_t  SetHistOptDialog::fTitleFont = 0;
*/
Int_t    SetHistOptDialog::fTitleCenterX = 1;
Int_t    SetHistOptDialog::fTitleCenterY = 1;
Int_t    SetHistOptDialog::fTitleCenterZ = 1;

//_______________________________________________________________________

SetHistOptDialog::SetHistOptDialog(Int_t /*batch*/)
{
	cout << "ctor SetHistOptDialog, non interactive" <<endl;
	fDialog = NULL;
	fRow_lab = NULL;
	fCanvas = NULL;
	fHistList = NULL;
	fWindow = NULL;
	fHist = NULL;
	fParent = NULL;
	fPad = NULL;
	fAxisX = NULL;
	fAxisY = NULL;
	fAxisZ = NULL;
	fStatBox = NULL;
	fTitleBox = NULL;
	fLegendBox = NULL;
}
//_______________________________________________________________________

SetHistOptDialog::SetHistOptDialog(TGWindow * win, TCollection * /*hlist*/)
{
const Char_t *helptext =
"<!DOCTYPE HTML>\n\
<HTML>\n\
<TITLE>HelpHistOpt</TITLE>\n\
This widget allows adjustment of various graphics attributes\n\
of a histogram or graph.\n\
The size of the histogram title is either set to TextSiz(e)\n\
if it not zero or to 0.7 * Height(box) if zero.\n\
The position or size of the StatBox may also be adjusted by the mouse.\n\
Values may be remembered within or between sessions. This behaviour is\n\
controlled from <b>Various HistPresent Options</b> menu.\n\
<b>Caveat:</b>\n\
The position of a StatBox is defined by its upper right corner\n\
Root itself will adjust width and height of the statbox as follows:\n\
\n\
Double_t  statw  = gStyle->GetStatW();\n\
if (fit) statw   = 1.8*gStyle->GetStatW();\n\
	nlines: number of entries like cont, mean, sigma, etc\n\
	nlinesf: for fitting parameters\n\
	Double_t  stath  = (nlines+nlinesf)*gStyle->GetStatFontSize();\n\
	if (stath <= 0 || 3 == (gStyle->GetStatFont()%10)) {\n\
		stath = 0.25*(nlines+nlinesf)*gStyle->GetStatH();\n\
	}\n\
\n\
The textsize of the StatBox should normally be 0, in this case it is\n\
adjusted automatically to the size of the box\n\
If you want do it by yourself it might be tricky.\n\
<b>Note:</b> Changeing options only influences the current histogram\n\
To make them active for subsequently displayed histograms\n\
press: <b>Set as global def</b>\n\
<b>Reset all to def</b> sets <b>factory defaults</b>\n\
To make these permanent also needs <b>Set as global def</b>\n\
 \n\
If more than 1 histogram is in the canvas one may selected\n\
by pressing the middle mouse before this widget is invoked\n\
If this widget is already displayed the command <b>Reselect</b>\n\
is needed in addition. The current attributes will be applied\n\
to the selected histogram.\n\
</HTML>";
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	fDialog = NULL;
	fHistList = NULL;
	fWindow = NULL;
	fHist = NULL;
	fParent = NULL;
	fPad = NULL;
	fAxisX = NULL;
	fAxisY = NULL;
	fAxisZ = NULL;
	fStatBox = NULL;
	fTitleBox = NULL;
	fLegendBox = NULL;
	
	fWindow = win;	
	
	if ( ! SetPointers() ) {
		return;
	}
	fCustomStyleName = "";  // ???
	
   RestoreDefaults();
	GetValuesFromHist();
	if ( gHprDebug > 0 ) 
		cout << "SetHistOptDialog * hd = (SetHistOptDialog *)" << this<< ";" << endl;
   fRow_lab = new TList();
//   static void *fValp[100];
   Int_t ind = 0;
   static TString sgdcmd("SetHistAttPermLocal()");
   static TString stycmd("SetCustomStyle()");
   static TString rescmd("SetAllToDefault()");
	static TString relcmd("SetPointers()");
	Int_t div = TMath::Abs(fNdivisionsX);
   fPdivX = div%100;
   fSdivX = (div/100)%100;
   fTdivX = (div/10000)%100;
   fOptimizedivX = 1;
   if (fNdivisionsX < 0) fOptimizedivX = 0;

   div = TMath::Abs(fNdivisionsY);
   fPdivY = div%100;
   fSdivY = (div/100)%100;
   fTdivY = (div/10000)%100;
   fOptimizedivY = 1;
   if (fNdivisionsY < 0) fOptimizedivY = 0;

   div = TMath::Abs(fNdivisionsZ);
   fPdivZ = div%100;
   fSdivZ = (div/100)%100;
   fTdivZ = (div/10000)%100;
   fOptimizedivZ = 1;
   if (fNdivisionsZ < 0) fOptimizedivZ = 0;

   fTitleCmd1 = ind;
   fRow_lab->Add(new TObjString("CommentOnly_Histogram Title Attributes"));
   fValp[ind++] = &fDummy;
   fRow_lab->Add(new TObjString("ColorSelect_LineCol"));
	fRow_lab->Add(new TObjString("ColorSelect+FillCol"));
	fRow_lab->Add(new TObjString("Fill_Select+FillSty"));
   fRow_lab->Add(new TObjString("PlainShtVal+BordSiz"));
	fValp[ind++] = &fTitleLineColor;
	fValp[ind++] = &fTitleFillColor;
	fValp[ind++] = &fTitleStyle;
   fValp[ind++] = &fTitleBorderSize;

   fRow_lab->Add(new TObjString("CfontSelect_TFont"));
   fRow_lab->Add(new TObjString("ColorSelect+TextCol"));
   fRow_lab->Add(new TObjString("Float_Value+TextSiz;0.0;0.1"));
	fRow_lab->Add(new TObjString("AlignSelect+Align"));
	fValp[ind++] = &fTitleFont;
   fValp[ind++] = &fTitleTextColor;
   fValp[ind++] = &fHistTitleSize;
	fValp[ind++] = &fTitleAlign;
	
   fRow_lab->Add(new TObjString("Float_Value_X;0;1"));
   fRow_lab->Add(new TObjString("Float_Value+Y;0;1"));
	fRow_lab->Add(new TObjString("Float_Value+Wid;0;0.9"));
	fRow_lab->Add(new TObjString("Float_Value+Hei;0;0.2"));
	//   fRow_lab->Add(new TObjString("Float_Value_H"));
//   fRow_lab->Add(new TObjString("AlignSelect+Align"));
   fValp[ind++] = &fTitleX;
   fValp[ind++] = &fTitleY;
	fValp[ind++] = &fTitleW;
	fValp[ind++] = &fTitleH;
	//   fValp[ind++] = &fTitleH;
   fTitleCmd2 = ind - 1;
	fStatCmd1 = fStatCmd2 = 0;
	if (fStatBox || fLegendBox ) {
		fStatCmd1 = ind;
		if ( fStatBox )
			fRow_lab->Add(new TObjString("CommentOnly_Statistic Box"));
		else 
			fRow_lab->Add(new TObjString("CommentOnly_Legend Box"));
		fValp[ind++] = &fDummy;
		fRow_lab->Add(new TObjString("Float_Value_X;0;1"));
		fRow_lab->Add(new TObjString("Float_Value+Y;0;1"));
		fRow_lab->Add(new TObjString("Float_Value+Wi ;0;1"));
		fRow_lab->Add(new TObjString("Float_Value+He ;0;1"));
		if (fHist && fHist->GetDimension() == 2 ) {
			fValp[ind++] = &fStatX2D;
			fValp[ind++] = &fStatY2D;
			fValp[ind++] = &fStatW2D;
			fValp[ind++] = &fStatH2D;
		} else {
			fValp[ind++] = &fStatX;
			fValp[ind++] = &fStatY;
			fValp[ind++] = &fStatW;
			fValp[ind++] = &fStatH;
		}
		fRow_lab->Add(new TObjString("ColorSelect_LineCol"));
		fRow_lab->Add(new TObjString("ColorSelect+FillCol"));
		fRow_lab->Add(new TObjString("Fill_Select+FillSty"));
		fRow_lab->Add(new TObjString("PlainShtVal+BorderSz"));
		fValp[ind++] = &fStatLineColor;
		fValp[ind++] = &fStatFillColor;
		fValp[ind++] = &fStatStyle;
		fValp[ind++] = &fStatBorderSize;
		
		fRow_lab->Add(new TObjString("CfontSelect_TFont"));
		fRow_lab->Add(new TObjString("ColorSelect+TextCol "));
		fRow_lab->Add(new TObjString("Float_Value+TSize;0;1  "));
		fRow_lab->Add(new TObjString("StringValue+Format"));
		fValp[ind++] = &fStatFont;
		fValp[ind++] = &fStatTextColor;
		fValp[ind++] = &fStatFontSize;
		fValp[ind++] = &fStatFormat;
		fStatCmd2 = ind - 1;
	}
//    fValp[ind++] = &fStatX;
//    fValp[ind++] = &fStatY;
//   fStatFont /= 10;
 //  fRow_lab->Add(new TObjString("CommentOnly_X Axis Div: PrimX SecX TertX  PrimY SecY TertY Optimize"));
   fRow_lab->Add(new TObjString("CommentOnly_AxisDiv"));
   fRow_lab->Add(new TObjString("CommentOnly+Prim"));
   fRow_lab->Add(new TObjString("CommentOnly+Sec"));
   fRow_lab->Add(new TObjString("CommentOnly+Tert"));
   fRow_lab->Add(new TObjString("CommentOnly+Optimize"));
//   fRow_lab->Add(new TObjString("CommentOnly+TickSide"));
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fDummy;
//   fValp[ind++] = &fDummy;

   fRow_lab->Add(new TObjString("CommentOnly_X"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("CheckButton+"));
//   fRow_lab->Add(new TObjString("StringValue+"));
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fPdivX;
   fValp[ind++] = &fSdivX;
   fValp[ind++] = &fTdivX;
   fValp[ind++] = &fOptimizedivX;
//   fValp[ind++] = &fTickSideX;

   fRow_lab->Add(new TObjString("CommentOnly_Y"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("PlainIntVal+"));
   fRow_lab->Add(new TObjString("CheckButton+"));
//   fRow_lab->Add(new TObjString("StringValue+"));
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fPdivY;
   fValp[ind++] = &fSdivY;
   fValp[ind++] = &fTdivY;
   fValp[ind++] = &fOptimizedivY;
//   fValp[ind++] = &fTickSideY;
   if (fHist && fHist->GetDimension() > 1) {
		fRow_lab->Add(new TObjString("CommentOnly_Z"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("PlainIntVal+"));
		fRow_lab->Add(new TObjString("CheckButton+"));
//		fRow_lab->Add(new TObjString("StringValue+"));
		fValp[ind++] = &fDummy;
		fValp[ind++] = &fPdivZ;
		fValp[ind++] = &fSdivZ;
		fValp[ind++] = &fTdivZ;
		fValp[ind++] = &fOptimizedivZ;
//      fValp[ind++] = &fTickSideZ;
   }

   fRow_lab->Add(new TObjString("ColorSelect_Axis Color"));
   fRow_lab->Add(new TObjString("Float_Value+TickLength"));
   fValp[ind++] = &fAxisColor;
   fValp[ind++] = &fTickLength;

   fRow_lab->Add(new TObjString("CommentOnly_Label Attributes"));
   fRow_lab->Add(new TObjString("ColorSelect_Color"));
   fRow_lab->Add(new TObjString("CfontSelect+Fo"));
   fRow_lab->Add(new TObjString("Float_Value+Sz;0;1"));
   fRow_lab->Add(new TObjString("PlainIntVal+MaxDigs"));
   fRow_lab->Add(new TObjString("Float_Value_Offset X"));
   fRow_lab->Add(new TObjString("Float_Value+Offset Y"));
   if (fHist && fHist->GetDimension() > 1)
      fRow_lab->Add(new TObjString("Float_Value+Offset Z"));
   fValp[ind++] =  &fDummy;
   fValp[ind++] = &fLabelColor;
   fValp[ind++] = &fLabelFont;
   fValp[ind++] = &fLabelSize;
   fValp[ind++] = &fLabelMaxDigits;
   fValp[ind++] = &fLabelOffsetX;
   fValp[ind++] = &fLabelOffsetY;
   if (fHist && fHist->GetDimension() > 1)
      fValp[ind++] = &fLabelOffsetZ;
   fRow_lab->Add(new TObjString("CommentOnly_Axis Title Attributes"));
   fRow_lab->Add(new TObjString("ColorSelect_Color"));
   fRow_lab->Add(new TObjString("CfontSelect+Font"));
   fRow_lab->Add(new TObjString("Float_Value+Size;0;1"));
   fValp[ind++] = &fDummy;
   fValp[ind++] = &fTitleColorA;
   fValp[ind++] = &fTitleFontA;
   fValp[ind++] = &fTitleSize;
   fRow_lab->Add(new TObjString("Float_Value_Offs X"));
   fRow_lab->Add(new TObjString("Float_Value+Offs Y"));
   fRow_lab->Add(new TObjString("Float_Value+Offs Z"));
   fRow_lab->Add(new TObjString("CheckButton_Cent X "));
   fRow_lab->Add(new TObjString("CheckButton+Cent Y"));
   fRow_lab->Add(new TObjString("CheckButton+Cent Z"));

   fValp[ind++] = &fTitleOffsetX;
   fValp[ind++] = &fTitleOffsetY;
   Int_t offz = ind;
   fValp[ind++] = &fTitleOffsetZ;
   fValp[ind++] = &fTitleCenterX;
   fValp[ind++] = &fTitleCenterY;
	Int_t centz = ind;
	fValp[ind++] = &fTitleCenterZ;

   fRow_lab->Add(new TObjString("CommandButt_Reset to def"));
   fValp[ind++] = &rescmd;
   fRow_lab->Add(new TObjString("CommandButt+Set global def"));
   fValp[ind++] = &sgdcmd;
   fRow_lab->Add(new TObjString("CommandButt+Custom style"));
   fValp[ind++] = &stycmd;
	if (fNSubPads > 0 ) {
		fRow_lab->Add(new TObjString("CommandButt+Reselect"));
		fBidReselect = ind;
		fValp[ind++] = &relcmd;
	}
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
	fOk = 0;
   fDialog =
      new TGMrbValuesAndText(fObjTitle, NULL, &fOk,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fHist && fHist->GetDimension() == 1) {
		fDialog->DisableButton(offz);
		fDialog->DisableButton(centz);
	}

}
//_______________________________________________________________________

SetHistOptDialog::~SetHistOptDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//______________________________________________________________________

Bool_t SetHistOptDialog::SetPointers()
{
	fHist = NULL;
	fAxisX = NULL;
	fAxisY = NULL;
	fAxisZ = NULL;
	fStatBox  = NULL;
	fTitleBox = NULL;
	fLegendBox= NULL;
	fNSubPads = 0;
	fPad = fCanvas;
	// are there 1 dim hists?
//	TObject * parent;
	fParent = NULL;
	fHist = Hpr::FindHistInPad(fPad, 1, 1, &fParent);
	// look for subpads
	// if gPad is one of the pads found take it otherwise take first
	TPad *pad = NULL;
	TH1* hist = NULL;
	if ( fHist == NULL ) {
		TIter next(fCanvas->GetListOfPrimitives());
		while (TObject * obj = next()) {
			if (obj->InheritsFrom("TPad")) {
				pad  = (TPad*)obj;
				hist = Hpr::FindHistInPad(pad, 1, 1);
				if ( hist && fHist == NULL ) {
					fPad = pad;
					fHist = hist;
				}
//				cout << "Hpr::FindHistInPad, gPad, pad, hist  " << gPad << " " << pad
//				<< " " << hist << endl;
				fNSubPads ++;
				if (hist && (TPad*)obj == gPad ) {
					fPad = pad;
					fHist = hist;
					break;
				}
			}
		}
	}
	if ( fHist != NULL ) {
		fPad->cd();
		fAxisX = fHist->GetXaxis();
		fAxisY = fHist->GetYaxis();
		if (fHist && fHist->GetDimension() > 1)
			fAxisZ = fHist->GetZaxis();
		fObjTitle = fHist->GetTitle();
		if ( fParent != NULL && gHprDebug > 0)
			cout << "parent->ClassName() " << fParent->ClassName()<< endl;
		if ( fDialog )
			fDialog->SetWindowName(fObjTitle.Data());
	} else {
		cout << "No Histogram in active Canvas" << endl;
		return kFALSE;
	}
	fStatBox = (TPavesText*)fHist->GetListOfFunctions()->FindObject("stats");
	fTitleBox = (TPavesText*)fPad->GetListOfPrimitives()->FindObject("title");
	TIter next1(fCanvas->GetListOfPrimitives());
	while (TObject * obj = next1()) {
		if (obj->InheritsFrom("TLegend")) {
			fLegendBox = (TLegend*)obj;
		}
	}
	if ( gHprDebug > 0 ) 
		cout << "SetHistOptDialog, current selected: " << fHist->GetName()<<
	" Title: " << fHist->GetTitle() << endl;
	return kTRUE;
}	
//_______________________________________________________________________
/*
void SetHistOptDialog::RecursiveRemove(TObject * obj)
{
   if (obj && (obj == fStatBox || obj == fHist 
		|| obj  == fTitleBox || obj == fLegendBox) ) {
		if (gHprDebug > 0) {
			cout << "SetHistOptDialog: RecursiveRemove:obj "  << obj;
			if (obj)
				cout << " " << obj->ClassName();
			cout << endl;
		}
		CloseDialog();
	}
}
*/
//_______________________________________________________________________

void SetHistOptDialog::CloseDialog()
{
 //  cout << "SetHistOptDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void SetHistOptDialog::SetHistAttNow(Int_t bid)
{
   fNdivisionsX  = fTdivX*10000 + fSdivX*100 + fPdivX;
   if (fOptimizedivX == 0) fNdivisionsX *= -1;
   fNdivisionsY  = fTdivY*10000 + fSdivY*100 + fPdivY;
   if (fOptimizedivY == 0) fNdivisionsY *= -1;
   fNdivisionsZ  = fTdivZ*10000 + fSdivZ*100 + fPdivZ;
   if (fOptimizedivZ == 0) fNdivisionsZ *= -1;
   SetHistOptDialog::SetHistAtt(bid);
}
//_______________________________________________________________________

void SetHistOptDialog::SetHistAtt(Int_t bid)
{
   if (gHprDebug > 0)
		cout << "SetHistOptDialog::SetHistAtt "<< bid << endl;
	Bool_t mod_statbox =
	gStyle->GetOptStat() && fStatCmd1 > 0 && fStatCmd2 > 0
	&& ((bid == fBidReselect) || (bid == 999) ||(bid >= fStatCmd1 && bid <= fStatCmd2));
	Bool_t mod_titlebox =
	gStyle->GetOptTitle() && ((bid == fBidReselect) || (bid == 999) ||(bid <= fTitleCmd2));
	Bool_t mod_axis = ( bid > fStatCmd2 );
	if (fTitleBox && mod_titlebox)
		SetTitleBoxAttr(gStyle);
	if (fStatBox &&  mod_statbox) 
		SetStatBoxAttr(gStyle);
	else if ( fLegendBox )
		SetLegendBoxAttr(gStyle);
	if ( mod_axis ) {
		fAxisX->SetNdivisions( fNdivisionsX);
		fAxisX->SetAxisColor(  fAxisColor);
		fAxisX->SetLabelColor( fLabelColor);
		fAxisX->SetLabelFont(  fLabelFont);
		fAxisX->SetLabelOffset(fLabelOffsetX);
		fAxisX->SetLabelSize(  fLabelSize);
		fAxisX->SetTickLength( fTickLength);
//		fAxisX->SetTicks(      fTickSideX);
		fAxisX->SetTitleOffset(fTitleOffsetX);
		fAxisX->SetTitleSize(  fTitleSize);
		fAxisX->SetTitleColor( fTitleColorA);
		fAxisX->SetTitleFont(  fTitleFontA);
		if (fTitleCenterX)
			fAxisX->CenterTitle(kTRUE);
		else
			fAxisX->CenterTitle(kFALSE);

		fAxisY->SetNdivisions( fNdivisionsY);
		fAxisY->SetAxisColor(  fAxisColor);
		fAxisY->SetLabelColor( fLabelColor);
		fAxisY->SetLabelFont(  fLabelFont);
		fAxisY->SetLabelOffset(fLabelOffsetY);
		fAxisY->SetLabelSize(  fLabelSize);
		fAxisY->SetTickLength( fTickLength);
//		fAxisY->SetTicks(      fTickSideY);
		fAxisY->SetTitleOffset(fTitleOffsetY);
		fAxisY->SetTitleSize  (fTitleSize);
		fAxisY->SetTitleColor( fTitleColorA);
		fAxisY->SetTitleFont(  fTitleFontA);
		if ( fAxisZ != NULL) {
			TPaletteAxis *pl = (TPaletteAxis*)fHist->GetListOfFunctions()->FindObject("palette");
			if ( pl != NULL ) {
				pl->SetLabelColor( fLabelColor);
				pl->SetLabelFont(  fLabelFont);
				pl->SetLabelOffset(fLabelOffsetZ);
				pl->SetLabelSize(fLabelSize);
			}
			fAxisZ->SetNdivisions( fNdivisionsZ);
			fAxisZ->SetAxisColor(  fAxisColor);
			fAxisZ->SetLabelColor( fLabelColor);
			fAxisZ->SetLabelFont(  fLabelFont);
			fAxisZ->SetLabelOffset(fLabelOffsetZ);
			fAxisZ->SetLabelSize(  fLabelSize);
			fAxisZ->SetTickLength( fTickLength);
//			fAxisZ->SetTicks(      fTickSideZ);
			fAxisZ->SetTitleOffset(fTitleOffsetZ);
			fAxisZ->SetTitleSize(  fTitleSize);
			fAxisZ->SetTitleColor( fTitleColorA);
			fAxisZ->SetTitleFont(  fTitleFontA);
			if (fTitleCenterZ)
				fAxisZ->CenterTitle(kTRUE);
			else
				fAxisZ->CenterTitle(kFALSE);
			}
		if (fLabelMaxDigits > 0)  {
			TGaxis::SetMaxDigits(fLabelMaxDigits);
		}
		if (fTitleCenterY)
			fAxisY->CenterTitle(kTRUE);
		else
			fAxisY->CenterTitle(kFALSE);
	}
   fPad->Modified();
	fPad->Update();
}
//______________________________________________________________________

void SetHistOptDialog::SetTitleBoxAttr(TStyle *sty)
{
	sty->SetTitleFillColor (fTitleFillColor );
	sty->SetLineColor      (fTitleLineColor );
	sty->SetTitleTextColor (fTitleTextColor );
	sty->SetTitleBorderSize(fTitleBorderSize);
	sty->SetTitleFont      (fTitleFont ,"t" );
	sty->SetTitleSize      (fHistTitleSize ,"h" );
	sty->SetTitleFontSize  (fHistTitleSize);
	sty->SetTitleStyle     (fTitleStyle     );
	sty->SetTitleAlign     (fTitleAlign     );
	sty->SetTitleX         (fTitleX         );
	sty->SetTitleY         (fTitleY         );
	sty->SetTitleW         (fTitleW         );
	sty->SetTitleH         (fTitleH         );
	if (sty->GetFillColor() == 0) {
		sty->SetFillColor(1);
	}
	if ( fTitleBox ) {
		if (fTitleW < 0.01) fTitleW = 0.3;
		if (fTitleH < 0.01) fTitleH = 0.06;
//		if (fHistTitleSize < 0.001) fHistTitleSize = 0.04;
		if (fHistTitleSize > 0.1) fHistTitleSize = 0.04;
		if (fTitleW > 0.8) fTitleW = 0.5;
		if (fTitleH > 0.4) fTitleH = 0.06;
		
		fTitleBox->SetTextSize(fHistTitleSize);
		fTitleBox->SetTextColor(fTitleTextColor);
		fTitleBox->SetLineColor(fTitleLineColor);
		fTitleBox->SetFillColor(fTitleFillColor);
		fTitleBox->SetFillStyle(fTitleStyle);
		fTitleBox->SetTextFont  (fTitleFont);
		fTitleBox->SetTextAlign (22);
		fTitleBox->SetBorderSize(fTitleBorderSize);
		
		Int_t talh = gStyle->GetTitleAlign()/10;
		if (talh < 1)
			talh = 1;
		if (talh > 3)
			talh = 3;
		Int_t talv = gStyle->GetTitleAlign()%10;
		if (talv < 1)
			talv = 1;
		if (talv > 3)
			talv = 3;
		Double_t xpos, ypos;
		if (talh == 1) xpos = fTitleX;
		if (talh == 2) xpos = fTitleX-fTitleW/2.;
		if (talh == 3) xpos = fTitleX-fTitleW;
		
		if (talv == 1) ypos = fTitleY;
		if (talv == 2) ypos = fTitleY - fTitleH/2.;
		if (talv == 3) ypos = fTitleY - fTitleH;
		
		
		fTitleBox->SetX1NDC         (xpos         );
		fTitleBox->SetY1NDC         (ypos         );
		fTitleBox->SetX2NDC         (xpos + fTitleW );
		fTitleBox->SetY2NDC         (ypos + fTitleH );

	}
}
//______________________________________________________________________

void SetHistOptDialog::SetLegendBoxAttr(TStyle *sty)
{
	TEnv env(".hprrc");
	if (gHprDebug > 0)
		cout << "SetHistOptDialog::SetLegendBoxAttr"  << fLegendBox<< endl;
	if ( fLegendBox) {
		sty->SetLegendBorderSize(fStatBorderSize);
		fLegendBox->SetBorderSize(fStatBorderSize);
		fLegendBox->SetFillColor     (fStatFillColor);
		fLegendBox->SetLineColor     (fStatLineColor);
		fLegendBox->SetTextColor     (fStatTextColor);
		fLegendBox->SetFillStyle     (fStatStyle    );
		if (fStatX < 0 || fStatX > 0.95) fStatX = 0.7;
		if (fStatY < 0 || fStatY > 0.95) fStatX = 0.8;
		fLegendBox->SetX2NDC         (fStatX         );
		fLegendBox->SetY2NDC         (fStatY         );
		fLegendBox->SetX1NDC         (fStatX - fStatW );
		fLegendBox->SetY1NDC         (fStatY - fStatH );
		
		env.SetValue("HprLegend.fFillColor", fStatFillColor);
		env.SetValue("HprLegend.fFillStyle", fStatStyle);
		env.SetValue("HprLegend.fLineColor", fStatLineColor);
		env.SetValue("HprLegend.fBorderSize",fStatBorderSize);
		env.SetValue("HprLegend.fTextColor", fStatTextColor);
		env.SetValue("HprLegend.fTextFont",  fStatFont);
	}
}
	//______________________________________________________________________

void SetHistOptDialog::SetStatBoxAttr(TStyle *sty)
{
	TEnv env(".hprrc");
	if (gHprDebug > 0)
		cout << "SetHistOptDialog::SetStatBoxAttr" << fStatBox << endl;
	if (fStatBox) {
		Float_t statX, statY, statW, statH;
		if (fHist && fHist->GetDimension() == 2) {
			statX = fStatX2D;
			statY = fStatY2D;
			statW = fStatW2D;
			statH = fStatH2D;
		} else {
			statX = fStatX;
			statY = fStatY;
			statW = fStatW;
			statH = fStatH;
		}
		if (statX < 0 || statX > 0.95) statX = 0.7;
		if (statY < 0 || statY > 0.95) statX = 0.8;
		fStatBox->SetTextSize(fStatFontSize);
		fStatBox->SetTextColor(fStatTextColor);
		fStatBox->SetLineColor(fStatLineColor);
		fStatBox->SetFillColor(fStatFillColor);
		fStatBox->SetFillStyle(fStatStyle);
		fStatBox->SetTextFont  (fStatFont);
		fStatBox->SetBorderSize(fStatBorderSize);
		fStatBox->SetX2NDC         (statX         );
		fStatBox->SetY2NDC         (statY         );
		fStatBox->SetX1NDC         (statX - statW );
		fStatBox->SetY1NDC         (statY - statH );
		sty->SetStatColor     (fStatFillColor );
		sty->SetLineColor     (fStatLineColor );
		sty->SetStatColor     (fStatFillColor );
		sty->SetStatTextColor (fStatTextColor );
		sty->SetStatBorderSize(fStatBorderSize);
		sty->SetStatFont      (fStatFont      );
		sty->SetStatFontSize  (fStatFontSize  );
		sty->SetStatStyle     (fStatStyle     );
		sty->SetStatFormat    (fStatFormat    );
// 		fStatX         = fStatBox->GetX2NDC();
// 		fStatY         = fStatBox->GetY2NDC();
// 		fStatW         = fStatBox->GetX2NDC() - fStatBox->GetX1NDC();
// 		fStatH         = fStatBox->GetY2NDC() - fStatBox->GetY1NDC();
		sty->SetStatX         (fStatX);
		sty->SetStatY         (fStatY);
		sty->SetStatW         (fStatW);
		sty->SetStatH         (fStatH);
	}
	env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void SetHistOptDialog::SetCustomStyle()
{
	cout << "SetHistOptDialog:: SetCustomStyle" << endl;
   Bool_t ok;
	fNewStyle = 1;
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
	TString nsn;
	fCustomStyleName = ("hpr_custom_styles.root|TStyle|noselection");

   row_lab->Add(new TObjString("FileContReq_File name"));
   valp[ind++] = &fCustomStyleName;
   row_lab->Add(new TObjString("StringValue_Style name"));
   valp[ind++] = &nsn;
//	valp[ind++] = &fNewStyleName;
	row_lab->Add(new TObjString("CheckButton_Create a new Style"));
   valp[ind++] = &fNewStyle;
   Int_t itemwidth = 400;
   ok = GetStringExt("Create / Update Custom Style", NULL, itemwidth, fWindow,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

	TString fname, cname, oname;

	TObjArray * oa = fCustomStyleName.Tokenize("|");
	Int_t nent = oa->GetEntries();
   if (nent < 3) {
      cout << "fCustomStyleName not enough (3) fields" << endl;
      return;
   }

	cname =((TObjString*)oa->At(1))->String();
   cout << "fCustomStyleName (1) " << cname << endl;
	oname =((TObjString*)oa->At(2))->String();
   cout << "fCustomStyleName (2) " << oname << endl;
	if ( oname == "noselection" || fNewStyle) {
	   oname =nsn;
//		oname = fNewStyleName;
	}
	TStyle *style = NULL;
	TFile *sf  = NULL;
	fname =((TObjString*)oa->At(0))->String();
	if (gSystem->AccessPathName(fname, kFileExists)) {
	   sf = new TFile(fname, "NEW");
	} else {
	   sf = new TFile(fname, "UPDATE");
		if ( fNewStyle == 0 )
		   style = (TStyle*)sf->Get(oname);
	}
   cout << "fCustomStyleName " << oname << endl;
	if ( !style ) {
	  style = new TStyle(oname, oname);
	}
	SetHistAttPerm(style);
	if ( fHist ) {
		style->SetHistFillColor(fHist->GetFillColor());
		style->SetHistLineColor(fHist->GetLineColor());
		style->SetHistFillStyle(fHist->GetFillStyle());
		style->SetHistLineStyle(fHist->GetLineStyle());
		style->SetHistLineWidth(fHist->GetLineWidth());
		style->SetEndErrorSize (gStyle->GetEndErrorSize());
		style->SetErrorX       (gStyle->GetErrorX());
		TString temp = fHist->GetOption();
		if (temp != "none")
			 style->SetDrawOption(temp);
		else
			 style->SetDrawOption("");
		style->SetMarkerColor  (fHist->GetMarkerColor());
		style->SetMarkerStyle  (fHist->GetMarkerStyle());
		style->SetMarkerSize   (fHist->GetMarkerSize());
		style->SetPadBottomMargin (gStyle->GetPadBottomMargin ());
		style->SetPadTopMargin    (gStyle->GetPadTopMargin    ());
		style->SetPadLeftMargin   (gStyle->GetPadLeftMargin   ());
		style->SetPadRightMargin  (gStyle->GetPadRightMargin  ());
		style->SetPadGridX        (gStyle->GetPadGridX        ());
		style->SetPadGridY        (gStyle->GetPadGridY        ());
		style->SetPadTickX        (gStyle->GetPadTickX        ());
		style->SetPadTickY        (gStyle->GetPadTickY        ());
		style->SetFrameFillColor  (gStyle->GetFrameFillColor  ());
		style->SetFrameLineColor  (gStyle->GetFrameLineColor  ());
		style->SetFrameFillStyle  (gStyle->GetFrameFillStyle  ());
		style->SetFrameLineStyle  (gStyle->GetFrameLineStyle  ());
		style->SetFrameLineWidth  (gStyle->GetFrameLineWidth  ());
		style->SetFrameBorderSize (gStyle->GetFrameBorderSize ());
		style->SetFrameBorderMode (gStyle->GetFrameBorderMode ());
		style->SetCanvasColor     (gStyle->GetCanvasColor     ());
		style->SetCanvasDefH      (gStyle->GetCanvasDefH      ());
		style->SetCanvasDefW      (gStyle->GetCanvasDefW      ());
		style->SetCanvasDefX      (gStyle->GetCanvasDefX      ());
      style->SetPalette         (1, NULL);
    }
	style->Write();
	sf->Close();
	gDirectory = gROOT;
}
//______________________________________________________________________

void SetHistOptDialog::SetHistAttPermLocal()
{
//   cout << "SetHistOptDialog:: SetHistAttPerm()" << endl;
   fNdivisionsX  = fTdivX*10000 + fSdivX*100 + fPdivX;
   if (fOptimizedivX == 0) fNdivisionsX *= -1;
   fNdivisionsY  = fTdivY*10000 + fSdivY*100 + fPdivY;
   if (fOptimizedivY == 0) fNdivisionsY *= -1;
   fNdivisionsZ  = fTdivZ*10000 + fSdivZ*100 + fPdivZ;
   if (fNdivisionsZ == 0) fNdivisionsZ = 510;
   if (fOptimizedivZ == 0) fNdivisionsZ *= -1;
   SaveDefaults();
   SetHistAttPerm();
}
//______________________________________________________________________

void SetHistOptDialog::SetHistAttPerm(TStyle * style)
{
//   cout << "SetHistOptDialog:: SetHistAttPerm()" << endl;
   TStyle * sty = style;
   if (sty == NULL)
      sty = gStyle;
   sty->SetNdivisions   (fNdivisionsX );
   sty->SetAxisColor    (fAxisColor   );
   sty->SetLabelColor   (fLabelColor  );
   sty->SetLabelFont    (fLabelFont   );
   sty->SetLabelOffset  (fLabelOffsetX, "X");
   sty->SetLabelSize    (fLabelSize   );
   sty->SetTickLength   (fTickLength  );
   sty->SetTitleOffset  (fTitleOffsetX, "X");
   sty->SetTitleSize    (fTitleSize   );
   sty->SetTitleColor   (fTitleColorA  );
   sty->SetTitleFont    (fTitleFont   );

   sty->SetNdivisions   (fNdivisionsY, "Y");
   sty->SetAxisColor    (fAxisColor,   "Y");
   sty->SetLabelColor   (fLabelColor,  "Y");
   sty->SetLabelFont    (fLabelFont,   "Y");
   sty->SetLabelOffset  (fLabelOffsetY,"Y");
   sty->SetLabelSize    (fLabelSize,   "Y");
   sty->SetTickLength   (fTickLength,  "Y");
   sty->SetTitleOffset  (fTitleOffsetY,"Y");
   sty->SetTitleSize    (fTitleSize,   "Y");
   sty->SetTitleColor   (fTitleColorA,  "Y");
   sty->SetTitleFont    (fTitleFont,   "Y");

   sty->SetNdivisions   (fNdivisionsZ, "Z");
   sty->SetAxisColor    (fAxisColor,   "Z");
   sty->SetLabelColor   (fLabelColor,  "Z");
   sty->SetLabelFont    (fLabelFont,   "Z");
   sty->SetLabelOffset  (fLabelOffsetZ,"Z");
   sty->SetLabelSize    (fLabelSize,   "Z");
   sty->SetTickLength   (fTickLength,  "Z");
   sty->SetTitleOffset  (fTitleOffsetZ,"Z");
   sty->SetTitleSize    (fTitleSize,   "Z");
   sty->SetTitleColor   (fTitleColorA,  "Z");
   sty->SetTitleFont    (fTitleFont,   "Z");
   if (fLabelMaxDigits > 0)  {
//      cout << "SetMaxDigits" << fLabelMaxDigits<< endl;
      TGaxis::SetMaxDigits(fLabelMaxDigits);
   }
	SetTitleBoxAttr(sty);
	SetStatBoxAttr(sty);
}
//______________________________________________________________________

void SetHistOptDialog::SaveDefaults()
{
   if ( gHprDebug > 0 ) 
		cout << "SetHistOptDialog:: SaveDefaults()" << endl;
	TString envname;
	TString resname;
	TEnv env(".hprrc");
   env.SetValue("SetHistOptDialog.fNdivisionsX",   fNdivisionsX);
   env.SetValue("SetHistOptDialog.fNdivisionsY",   fNdivisionsY);
   env.SetValue("SetHistOptDialog.fNdivisionsZ",   fNdivisionsZ);
   env.SetValue("SetHistOptDialog.fAxisColor",     fAxisColor);
   env.SetValue("SetHistOptDialog.fLabelColor",    fLabelColor);
   env.SetValue("SetHistOptDialog.fLabelFont",     fLabelFont);
	if (fHist && fHist->GetDimension() == 3 ) {
		env.SetValue("SetHistOptDialog.fLabelOffsetX3Dim",  fLabelOffsetX);
		env.SetValue("SetHistOptDialog.fLabelOffsetY3Dim",  fLabelOffsetY);
		env.SetValue("SetHistOptDialog.fLabelOffsetZ3Dim",  fLabelOffsetZ);
		env.SetValue("SetHistOptDialog.fLabelSize3Dim",     fLabelSize);
		env.SetValue("SetHistOptDialog.fTickLength3Dim",    fTickLength);
		env.SetValue("SetHistOptDialog.fTickSideX3Dim",     fTickSideX);
		env.SetValue("SetHistOptDialog.fTickSideY3Dim",     fTickSideY);
		env.SetValue("SetHistOptDialog.fTickSideZ3Dim",     fTickSideZ);
		env.SetValue("SetHistOptDialog.fTitleOffsetX3Dim",  fTitleOffsetX);
		env.SetValue("SetHistOptDialog.fTitleOffsetY3Dim",  fTitleOffsetY);
		env.SetValue("SetHistOptDialog.fTitleOffsetZ3Dim",  fTitleOffsetZ);
	} else {
		env.SetValue("SetHistOptDialog.fLabelOffsetX",  fLabelOffsetX);
		env.SetValue("SetHistOptDialog.fLabelOffsetY",  fLabelOffsetY);
		env.SetValue("SetHistOptDialog.fLabelOffsetZ",  fLabelOffsetZ);
		env.SetValue("SetHistOptDialog.fLabelSize",     fLabelSize);
		env.SetValue("SetHistOptDialog.fTickLength",    fTickLength);
		env.SetValue("SetHistOptDialog.fTickSideX",     fTickSideX);
		env.SetValue("SetHistOptDialog.fTickSideY",     fTickSideY);
		env.SetValue("SetHistOptDialog.fTickSideZ",     fTickSideZ);
		env.SetValue("SetHistOptDialog.fTitleOffsetX",  fTitleOffsetX);
		env.SetValue("SetHistOptDialog.fTitleOffsetY",  fTitleOffsetY);
		env.SetValue("SetHistOptDialog.fTitleOffsetZ",  fTitleOffsetZ);
	}
   env.SetValue("SetHistOptDialog.fLabelMaxDigits",fLabelMaxDigits);
   env.SetValue("SetHistOptDialog.fTitleSize",      fTitleSize);
   env.SetValue("SetHistOptDialog.fTitleColorA",    fTitleColorA);
   env.SetValue("SetHistOptDialog.fTitleFontA",     fTitleFontA);
   env.SetValue("SetHistOptDialog.fTitleCenterX",   fTitleCenterX);
   env.SetValue("SetHistOptDialog.fTitleCenterY",   fTitleCenterY);
   env.SetValue("SetHistOptDialog.fTitleCenterZ",   fTitleCenterZ);
   env.SetValue("SetHistOptDialog.TitleColor",      fTitleFillColor );
	env.SetValue("SetHistOptDialog.TitleLineColor",  fTitleLineColor );
	env.SetValue("SetHistOptDialog.TitleTextColor",  fTitleTextColor );
   env.SetValue("SetHistOptDialog.TitleBorderSize", fTitleBorderSize);
   env.SetValue("SetHistOptDialog.TitleFont",       fTitleFont      );
   env.SetValue("SetHistOptDialog.fHistTitleSize",  fHistTitleSize  );
   env.SetValue("SetHistOptDialog.TitleStyle",      fTitleStyle     );
   env.SetValue("SetHistOptDialog.TitleX",          fTitleX         );
   env.SetValue("SetHistOptDialog.TitleY",          fTitleY         );
   env.SetValue("SetHistOptDialog.TitleW",          fTitleW         );
   env.SetValue("SetHistOptDialog.TitleH",          fTitleH         );
   env.SetValue("SetHistOptDialog.TitleAlign",      fTitleAlign     );
	if ( fStatBox ) {
		env.SetValue("SetHistOptDialog.StatColor",       fStatFillColor  );
		env.SetValue("SetHistOptDialog.StatLineColor",   fStatLineColor  );
		env.SetValue("SetHistOptDialog.StatTextColor",   fStatTextColor  );
		env.SetValue("SetHistOptDialog.StatBorderSize",  fStatBorderSize );
		env.SetValue("SetHistOptDialog.StatFont",        fStatFont       );
		env.SetValue("SetHistOptDialog.StatFormat",     fStatFormat     );
		env.SetValue("SetHistOptDialog.StatFontSize",    fStatFontSize   );
		env.SetValue("SetHistOptDialog.StatStyle",       fStatStyle      );
		Double_t sh = fStatBox->GetY2NDC() - fStatBox->GetY1NDC();
		Int_t nlines = fStatBox->GetListOfLines()->GetSize();
		if (nlines <= 0) 
			nlines = 1;
		if ( gHprDebug > 0 )
			cout << "SetHistOptDialog::SaveDefaults(): sh, nlines " << " " << sh << " " << nlines << endl;
		sh = 4. * sh / nlines;
		if (fHist &&  fHist->GetDimension() == 2) {
			env.SetValue("SetHistOptDialog.StatX2D",      fStatX2D        );
			env.SetValue("SetHistOptDialog.StatY2D",      fStatY2D        );
			env.SetValue("SetHistOptDialog.StatW2D",      fStatW2D        );
			env.SetValue("SetHistOptDialog.StatH2D",      sh        );
		} else {
			env.SetValue("SetHistOptDialog.StatX",           fStatX        );
			env.SetValue("SetHistOptDialog.StatY",           fStatY        );
			env.SetValue("SetHistOptDialog.StatW",           fStatW        );
			env.SetValue("SetHistOptDialog.StatH",           sh          );
		}
/*		envname = "StatBox";
		if (fHist && fHist->GetDimension() == 2) {
			envname += "2D.f";
		} else {
			envname += "1D.f";
		}	
		resname = envname + "X1";	
		env.SetValue(resname, fStatX - fStatW);
		resname = envname + "X2";	
		env.SetValue(resname, fStatX);
		resname = envname + "Y1";	
		env.SetValue(resname, fStatY - fStatH);
		resname = envname + "Y2";	
		env.SetValue(resname, fStatY);*/
		
	} else if ( fLegendBox ) {
		TString pavename(fLegendBox->GetName());
		cout << " SetHistOptDialog::SaveDefaults(): " << pavename << endl;
		if (pavename == "Legend_SuperImposeHist") {
			envname = "SuperImposeHist";
		} else if (pavename == "Legend_SuperImposeGraph") {
			envname = "SuperImposeGraph";
		} else {
			envname = "HprStack";
		}
		envname += ".fLegend";
		resname = envname + "X1";	
		env.SetValue(resname, fStatX - fStatW);
		resname = envname + "X2";	
		env.SetValue(resname, fStatX);
		resname = envname + "Y1";	
		env.SetValue(resname, fStatY - fStatH);
		resname = envname + "Y2";	
		env.SetValue(resname, fStatY);
	}
   env.SaveLevel(kEnvLocal);
}
//______________________________________________________________________

void SetHistOptDialog::SetAllToDefault()
{
	RestoreDefaults(1);
}
//______________________________________________________________________

void SetHistOptDialog::RestoreDefaults(Int_t resetall)
{
	if ( gHprDebug > 0) 
		cout << "SetHistOptDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
   TEnv env(envname);
	if (fHist && fHist->GetDimension() == 3 ) {
		fLabelOffsetX = env.GetValue("SetHistOptDialog.fLabelOffsetX3Dim",  0.01);
		fLabelOffsetY = env.GetValue("SetHistOptDialog.fLabelOffsetY3Dim",  0.01);
		fLabelOffsetZ = env.GetValue("SetHistOptDialog.fLabelOffsetZ3Dim",  0.01);
		fLabelSize    = env.GetValue("SetHistOptDialog.fLabelSize3Dim",     0.03);
		fTickLength   = env.GetValue("SetHistOptDialog.fTickLength3Dim",    0.01);
		fTickSideX    = env.GetValue("SetHistOptDialog.fTickSideX3Dim",      "-");
		fTickSideY    = env.GetValue("SetHistOptDialog.fTickSideY3Dim",      "+");
		fTickSideZ    = env.GetValue("SetHistOptDialog.fTickSideZ3Dim",      "+");
		fTitleOffsetX = env.GetValue("SetHistOptDialog.fTitleOffsetX3Dim",    1.);
		fTitleOffsetY = env.GetValue("SetHistOptDialog.fTitleOffsetY3Dim",    1.6);
		fTitleOffsetZ = env.GetValue("SetHistOptDialog.fTitleOffsetZ3Dim",    1.4);
	} else {
		fLabelOffsetX = env.GetValue("SetHistOptDialog.fLabelOffsetX",       0.01);
		fLabelOffsetY = env.GetValue("SetHistOptDialog.fLabelOffsetY",       0.01);
		fLabelOffsetZ = env.GetValue("SetHistOptDialog.fLabelOffsetZ",       0.01);
		fLabelSize    = env.GetValue("SetHistOptDialog.fLabelSize",          0.03);
		fTickLength   = env.GetValue("SetHistOptDialog.fTickLength",         0.01);
		fTickSideX    = env.GetValue("SetHistOptDialog.fTickSideX",           "+");
		fTickSideY    = env.GetValue("SetHistOptDialog.fTickSideY",           "+");
		fTickSideZ    = env.GetValue("SetHistOptDialog.fTickSideZ",           "+");
		fTitleOffsetX = env.GetValue("SetHistOptDialog.fTitleOffsetX",         1.);
		fTitleOffsetY = env.GetValue("SetHistOptDialog.fTitleOffsetY",         1.);
		fTitleOffsetZ = env.GetValue("SetHistOptDialog.fTitleOffsetZ",         1.);
	}
   fNdivisionsX   = env.GetValue("SetHistOptDialog.fNdivisionsX",   510);
   fNdivisionsY   = env.GetValue("SetHistOptDialog.fNdivisionsY",   510);
   fNdivisionsZ   = env.GetValue("SetHistOptDialog.fNdivisionsZ"  , 510);
   fAxisColor     = env.GetValue("SetHistOptDialog.fAxisColor",       1);
   fLabelColor    = env.GetValue("SetHistOptDialog.fLabelColor",      1);
   fLabelFont     = env.GetValue("SetHistOptDialog.fLabelFont",      62);
   fLabelMaxDigits= env.GetValue("SetHistOptDialog.fLabelMaxDigits",  4);
   fTickLength    = env.GetValue("SetHistOptDialog.fTickLength",   0.01);
   fTitleSize     = env.GetValue("SetHistOptDialog.fTitleSize",    0.03);
	fHistTitleSize = env.GetValue("SetHistOptDialog.fHistTitleSize",0.03);
	fTitleColorA   = env.GetValue("SetHistOptDialog.fTitleColorA",     1);
   fTitleFontA    = env.GetValue("SetHistOptDialog.fTitleFontA",     62);
   fTitleCenterX  = env.GetValue("SetHistOptDialog.fTitleCenterX",    1);
   fTitleCenterY  = env.GetValue("SetHistOptDialog.fTitleCenterY",    1);
   fTitleCenterZ  = env.GetValue("SetHistOptDialog.fTitleCenterZ",    1);
	fLabelMaxDigits = TGaxis::GetMaxDigits();
   fStatFillColor= env.GetValue("SetHistOptDialog.StatColor",         0);
   fStatTextColor= env.GetValue("SetHistOptDialog.StatTextColor",     1);
	fStatLineColor= env.GetValue("SetHistOptDialog.StatLineColor",     1);
	fStatBorderSize=env.GetValue("SetHistOptDialog.StatBorderSize",    1);
   fStatFont=      env.GetValue("SetHistOptDialog.StatFont"     ,    62);
	if (fStatFont < 12 || fStatFont > 123) fStatFont = 62;
	fStatFormat=    env.GetValue("SetHistOptDialog.StatFormat",  "6.2g");
   fStatFontSize=  env.GetValue("SetHistOptDialog.StatFontSize",   0.04);
   fStatStyle=     env.GetValue("SetHistOptDialog.StatStyle",         0);
   fStatX=         env.GetValue("SetHistOptDialog.StatX",           0.9);
   fStatY=         env.GetValue("SetHistOptDialog.StatY",           0.9);
   fStatW=         env.GetValue("SetHistOptDialog.StatW",           0.2);
   fStatH=         env.GetValue("SetHistOptDialog.StatH",          0.16);
	fStatX2D=         env.GetValue("SetHistOptDialog.StatX2D",       0.9);
	fStatY2D=         env.GetValue("SetHistOptDialog.StatY2D",       0.9);
	fStatW2D=         env.GetValue("SetHistOptDialog.StatW2D",       0.2);
	fStatH2D=         env.GetValue("SetHistOptDialog.StatH2D",       0.2);
	fTitleFillColor = env.GetValue("SetHistOptDialog.TitleColor",      0);
	fTitleLineColor = env.GetValue("SetHistOptDialog.TitleLineColor",  1);
	fTitleStyle     = env.GetValue("SetHistOptDialog.TitleStyle",      0);
   fTitleTextColor = env.GetValue("SetHistOptDialog.TitleTextColor",  1);
   fTitleBorderSize= env.GetValue("SetHistOptDialog.TitleBorderSize", 1);
   fTitleFont      = env.GetValue("SetHistOptDialog.TitleFont",      62);
   if (fTitleFont < 12 || fTitleFont > 123) fTitleFont = 62;
   fTitleFontA     = env.GetValue("SetHistOptDialog.TitleFontA",     62);
	fTitleX         = env.GetValue("SetHistOptDialog.TitleX",        0.5);
	fTitleY         = env.GetValue("SetHistOptDialog.TitleY",       .995);
	fTitleW         = env.GetValue("SetHistOptDialog.TitleW",         0.);
	fTitleH         = env.GetValue("SetHistOptDialog.TitleH",         0.);
	fTitleAlign     = env.GetValue("SetHistOptDialog.TitleAlign",     23);
	fHistTitleSize  = env.GetValue("SetHistOptDialog.HistTitleSize",0.03);
}
//______________________________________________________________________

void SetHistOptDialog::GetValuesFromHist()
{
	Int_t dim = 1;
	if (fHist && fHist->GetDimension() > 1) {
		fAxisZ = fHist->GetZaxis();
		dim = 2;
	}
	fAxisColor    = fAxisX->GetAxisColor  ();
	fLabelColor   = fAxisX->GetLabelColor ();
	fLabelFont    = fAxisX->GetLabelFont  ();
	fLabelSize    = fAxisX->GetLabelSize  ();
	fTickLength   = fAxisX->GetTickLength ();
	fTitleSize    = fAxisX->GetTitleSize  ();
	fTitleColorA  = fAxisX->GetTitleColor ();
	fTitleFontA   = fAxisX->GetTitleFont  ();

	fNdivisionsX  = fAxisX->GetNdivisions ();
	fLabelOffsetX = fAxisX->GetLabelOffset();
	fTickSideX    = fAxisX->GetTicks      ();
	fTitleOffsetX = fAxisX->GetTitleOffset();
	fTitleCenterX = fAxisX->GetCenterTitle();

	fNdivisionsY  = fAxisY->GetNdivisions ();
	fLabelOffsetY = fAxisY->GetLabelOffset();
// 	if ( !fAxisY->TestBit(TAxis::kTickPlus) &&
// 		!fAxisY->TestBit(TAxis::kTickMinus) )
// 		fAxisY->SetTicks("-");
	fTickSideY    = fAxisY->GetTicks      ();
	fTitleOffsetY = fAxisY->GetTitleOffset();
	fTitleCenterY = fAxisY->GetCenterTitle();

	if (fAxisZ != NULL) {
		fNdivisionsZ  = fAxisZ->GetNdivisions ();
		fLabelOffsetZ = fAxisZ->GetLabelOffset();
		fTickSideZ    = fAxisZ->GetTicks      ();
		fTitleOffsetZ = fAxisZ->GetTitleOffset();
		fTitleCenterZ = fAxisZ->GetCenterTitle();
	}
	if (fStatBox) {
		fStatFillColor     = fStatBox->GetFillColor();
		fStatTextColor = fStatBox->GetTextColor();
		fStatBorderSize= fStatBox->GetBorderSize();
		fStatFont      = fStatBox->GetTextFont();
		fStatFontSize  = fStatBox->GetTextSize();
		fStatStyle     = fStatBox->GetFillStyle();
		fStatFormat    = gStyle->GetStatFormat();
		if ( dim == 1 ) {
			fStatX         = fStatBox->GetX2NDC();
			fStatY         = fStatBox->GetY2NDC();
			fStatW         = fStatBox->GetX2NDC() - fStatBox->GetX1NDC();
			fStatH         = fStatBox->GetY2NDC() - fStatBox->GetY1NDC();
		} else {
			fStatX2D         = fStatBox->GetX2NDC();
			fStatY2D         = fStatBox->GetY2NDC();
			fStatW2D         = fStatBox->GetX2NDC() - fStatBox->GetX1NDC();
			fStatH2D         = fStatBox->GetY2NDC() - fStatBox->GetY1NDC();
		}
	} else if ( fLegendBox ) {
		fStatFillColor     = fLegendBox->GetFillColor();
		fStatTextColor = fLegendBox->GetTextColor();
		fStatBorderSize= fLegendBox->GetBorderSize();
		fStatFont      = fLegendBox->GetTextFont();
		fStatFontSize  = fLegendBox->GetTextSize();
		fStatStyle     = fLegendBox->GetFillStyle();
		fStatFormat    = gStyle->GetStatFormat();
		fStatX         = fLegendBox->GetX2NDC();
		fStatY         = fLegendBox->GetY2NDC();
		fStatW         = fLegendBox->GetX2NDC() - fLegendBox->GetX1NDC();
		fStatH         = fLegendBox->GetY2NDC() - fLegendBox->GetY1NDC();
	}
	if ( fTitleBox ) {
		fTitleFillColor =   fTitleBox->GetFillColor();
		fTitleStyle =       fTitleBox->GetFillStyle();
		fTitleTextColor =   fTitleBox->GetTextColor();
		fTitleBorderSize =  fTitleBox->GetBorderSize();
		fTitleFont =        fTitleBox->GetTextFont();
		fHistTitleSize =    fTitleBox->GetTextSize();
		if ( fHistTitleSize <= 0 ) {
			TEnv env(".hprrc");
			fHistTitleSize  = env.GetValue("SetHistOptDialog.HistTitleSize", 0.03);
		}
	}
}
//______________________________________________________________________

void SetHistOptDialog::SetDefaults(Int_t ndim)
{
	if ( gHprDebug > 0 ) 
	   cout << "SetHistOptDialog:: SetDefaults" << endl;
   TEnv env(".hprrc");
	if ( ndim != 3 ) {
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetX",  0.01), "X");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize",     0.03), "X");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength",    0.01), "X");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetX",   1.1), "X");
		
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetY",  0.01), "Y");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize",     0.03), "Y");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength",    0.01), "Y");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetY",   1.5), "Y");
		
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetZ",  0.01), "Z");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize",     0.03), "Z");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength",    0.01), "Z");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetZ",  0.01), "Z");
	} else {
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetX3Dim",  0.01), "X");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize3Dim",     0.03), "X");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength3Dim",    0.01), "X");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetX3Dim",   1.1), "X");
		
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetY3Dim",  0.01), "Y");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize3Dim",     0.03), "Y");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength3Dim",    0.01), "Y");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetY3Dim",   1.6), "Y");
		
		gStyle->SetLabelOffset    (env.GetValue("SetHistOptDialog.fLabelOffsetZ3Dim",  0.01), "Z");
		gStyle->SetLabelSize      (env.GetValue("SetHistOptDialog.fLabelSize3Dim",     0.03), "Z");
		gStyle->SetTickLength     (env.GetValue("SetHistOptDialog.fTickLength3Dim",    0.01), "Z");
		gStyle->SetTitleOffset    (env.GetValue("SetHistOptDialog.fTitleOffsetZ3Dim",   1.4), "Z");
	}
	gStyle->SetNdivisions     (env.GetValue("SetHistOptDialog.fNdivisionsX",   510), "X");
	gStyle->SetAxisColor      (env.GetValue("SetHistOptDialog.fAxisColor",       1), "X");
	gStyle->SetLabelColor     (env.GetValue("SetHistOptDialog.fLabelColor",      1), "X");
	gStyle->SetLabelFont      (env.GetValue("SetHistOptDialog.fLabelFont",      62), "X");
	gStyle->SetTitleSize      (env.GetValue("SetHistOptDialog.fTitleSize",    0.03), "X");
	gStyle->SetTitleColor     (env.GetValue("SetHistOptDialog.fTitleColorA",     1), "X");
	gStyle->SetTitleFont      (env.GetValue("SetHistOptDialog.fTitleFontA",     62), "X");
	
	gStyle->SetNdivisions     (env.GetValue("SetHistOptDialog.fNdivisionsY",   510), "Y");
	gStyle->SetAxisColor      (env.GetValue("SetHistOptDialog.fAxisColor",       1), "Y");
	gStyle->SetLabelColor     (env.GetValue("SetHistOptDialog.fLabelColor",      1), "Y");
	gStyle->SetLabelFont      (env.GetValue("SetHistOptDialog.fLabelFont",      62), "Y");
	gStyle->SetTitleSize      (env.GetValue("SetHistOptDialog.fTitleSize",    0.03), "Y");
	gStyle->SetTitleColor     (env.GetValue("SetHistOptDialog.fTitleColorA",     1), "Y");
	gStyle->SetTitleFont      (env.GetValue("SetHistOptDialog.fTitleFontA",     62), "Y");
	
	gStyle->SetNdivisions     (env.GetValue("SetHistOptDialog.fNdivisionsZ",   510), "Z");
	gStyle->SetAxisColor      (env.GetValue("SetHistOptDialog.fAxisColor",       1), "Z");
	gStyle->SetLabelColor     (env.GetValue("SetHistOptDialog.fLabelColor",      1), "Z");
	gStyle->SetLabelFont      (env.GetValue("SetHistOptDialog.fLabelFont",      62), "Z");
	gStyle->SetTitleSize      (env.GetValue("SetHistOptDialog.fTitleSize",    0.03), "Z");
	gStyle->SetTitleColor     (env.GetValue("SetHistOptDialog.fTitleColorA",     1), "Z");
	gStyle->SetTitleFont      (env.GetValue("SetHistOptDialog.fTitleFontA",     62), "Z");
	
	TGaxis::SetMaxDigits      (env.GetValue("SetHistOptDialog.fLabelMaxDigits",  4));
	
   gStyle->SetTitleColor     (env.GetValue("SetHistOptDialog.TitleColor",        0), "t");
   gStyle->SetTitleTextColor (env.GetValue("SetHistOptDialog.TitleTextColor",    1));
   gStyle->SetTitleBorderSize(env.GetValue("SetHistOptDialog.TitleBorderSize"   ,1));
   gStyle->SetTitleFont      (env.GetValue("SetHistOptDialog.TitleFont",        62), "t");
   gStyle->SetTitleSize      (env.GetValue("SetHistOptDialog.fHistTitleSize",  0.03),"h");
	gStyle->SetTitleFontSize  (env.GetValue("SetHistOptDialog.fHistTitleSize",  0.03));
	gStyle->SetTitleStyle     (env.GetValue("SetHistOptDialog.TitleStyle",     1001));
   gStyle->SetTitleX         (env.GetValue("SetHistOptDialog.TitleX",          0.5));
   gStyle->SetTitleY         (env.GetValue("SetHistOptDialog.TitleY",         .995));
   gStyle->SetTitleW         (env.GetValue("SetHistOptDialog.TitleW",           0.));
   gStyle->SetTitleH         (env.GetValue("SetHistOptDialog.TitleH",           0.));
   gStyle->SetTitleAlign     (env.GetValue("SetHistOptDialog.TitleAlign",       23));
	gStyle->SetStatColor      (env.GetValue("SetHistOptDialog.StatColor",        19));
	gStyle->SetStatTextColor  (env.GetValue("SetHistOptDialog.StatTextColor",     1));
	gStyle->SetStatBorderSize (env.GetValue("SetHistOptDialog.StatBorderSize",    1));
	gStyle->SetStatFont       (env.GetValue("SetHistOptDialog.StatFont",         62));
	gStyle->SetStatFormat     (env.GetValue("SetHistOptDialog.StatFormat",    "6.4g"));
	gStyle->SetStatFontSize   (env.GetValue("SetHistOptDialog.StatFontSize",   0.04));
	gStyle->SetStatStyle      (env.GetValue("SetHistOptDialog.StatStyle",      1001));
	gStyle->SetStatX          (env.GetValue("SetHistOptDialog.StatX",           0.9));
	gStyle->SetStatY          (env.GetValue("SetHistOptDialog.StatY",           0.9));
	gStyle->SetStatW          (env.GetValue("SetHistOptDialog.StatW",           0.2));
	gStyle->SetStatH          (env.GetValue("SetHistOptDialog.StatH",           0.16));
	fTitleCenterX  =           env.GetValue("SetHistOptDialog.fTitleCenterX",      1);
	fTitleCenterY  =           env.GetValue("SetHistOptDialog.fTitleCenterY",      1);
	fTitleCenterZ  =           env.GetValue("SetHistOptDialog.fTitleCenterZ",      1);
}
//______________________________________________________________________

void SetHistOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void SetHistOptDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
	if (gHprDebug > 0 ) {
		cout << "CRButtonPressed, bid: " << bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
		cout << endl;
	}
   SetHistAttNow(bid);
}

#include "TROOT.h"
#include "TEnv.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TFrame.h"
#include "THStack.h"
#include "TH3.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TList.h"
#include "TMath.h"
#include "TGMsgBox.h"
#include "TRootCanvas.h"
#include "TRandom.h"
#include "TRegexp.h"
#include "TPolyLine3D.h"
#include "TGMrbValuesAndText.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "GeneralAttDialog.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "hprbase.h"
#include "HistPresent.h"
#include "SetColor.h"

extern Int_t gHprDebug;

using std::cout;
using std::cerr;
using std::endl;

namespace Hpr 
{
//______________________________________________________________________________________

void WriteGraphasASCII(TGraph * g,  TRootCanvas * mycanvas)
{
static const Char_t helpText[] = 
"Write graph as ASCII";
	TString fname = g->GetName();
	fname += ".ascii";
	void *Valp[10];
	Int_t ind = 0;
	TList Row_lab;
	
	TString formX("%10.10g");
	TString formY("%10.10g");
	Int_t print_errorX = 0;
	Int_t print_errorY = 1;
	Row_lab.Add(new TObjString("StringValue_File name"));
	Valp[ind++] = &fname;
	Row_lab.Add(new TObjString("StringValue_Format X values"));
	Valp[ind++] = &formX;
	Row_lab.Add(new TObjString("StringValue+Format Y, errors"));
	Valp[ind++] = &formY;
	Row_lab.Add(new TObjString("CheckButton_Output ErrorX"));
	Valp[ind++] = &print_errorX;
	Row_lab.Add(new TObjString("CheckButton+Output ErrorY"));
	Valp[ind++] = &print_errorY;
	
	Int_t itemwidth = 400;
	Int_t ok = -2;
	
	new TGMrbValuesAndText("Write Graph as ASCII", NULL, &ok,itemwidth, mycanvas,
							 NULL, NULL, &Row_lab, Valp,
							 NULL, NULL, helpText);
	if ( ok < 0 )
		return;
	if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
//      cout << fname << " exists" << endl;
		int buttons = kMBOk | kMBDismiss, retval = 0;
		EMsgBoxIcon icontype = kMBIconQuestion;
		new TGMsgBox(gClient->GetRoot(), mycanvas,
						 "Question", "File exists, overwrite?",
						 icontype, buttons, &retval);
		if (retval == kMBDismiss)
			return;
	}
	ofstream outfile;
	outfile.open((const char *) fname);
	Double_t * x = 0;
	Double_t * y = 0;
	Double_t * ex = 0;
	Double_t * ey = 0;
	Double_t * exl = 0;
	Double_t * exh = 0;
	Double_t * eyl = 0;
	Double_t * eyh = 0;
	Double_t Sum = 0.;
	if (outfile) {
		x = g->GetX();
		y = g->GetY();
		if (g->IsA() == TGraphErrors::Class()) {
			ex = ((TGraphErrors *)g)->GetEX();
			ey = ((TGraphErrors *)g)->GetEY();
		}
		if (g->IsA() == TGraphAsymmErrors::Class()) {
			exl = ((TGraphAsymmErrors *)g)->GetEXlow();
			exh = ((TGraphAsymmErrors *)g)->GetEXhigh();
			eyl = ((TGraphAsymmErrors *)g)->GetEYlow();
			eyh = ((TGraphAsymmErrors *)g)->GetEYhigh();
		}
		for (Int_t i = 0; i < g->GetN(); i++) {
			outfile << Form(formX.Data(), x[i]) << " " << Form(formY.Data(),y[i]);
			Sum += y[i];
			if (ex && print_errorX)  outfile << " " << Form(formY.Data(), ex[i]);
			if (ey && print_errorY)  outfile << " " << Form(formY.Data(), ey[i]);
			if (exl && print_errorX && print_errorY)
			outfile << " " << Form(formY.Data(), exl[i]) << " " << Form(formY.Data(), exh[i])
					  << " " << Form(formY.Data(), eyl[i]) << " " << Form(formY.Data(),  eyh[i]);
			outfile << endl;
		}
		outfile.close();
		cout << g->GetN() << " Lines written to: "
			 << (const char *) fname << endl 
			 << " Sum: " << Sum <<  endl;
	} else {
		cout << " Cannot open: " << fname << endl;
	}
}
//______________________________________________________________________________________

void WriteOutGraph(TGraph * g, TRootCanvas * mycanvas)
{
	if (g) {
		TString name = g->GetName();
		Bool_t ok;
		name =
			 GetString("Save hist with name", name.Data(), &ok, mycanvas);
		if (!ok)
			return;
		g->SetName(name.Data());
		Save2FileDialog sfd(g);
//     if (OpenWorkFile(mycanvas)) {
//         g->Write();
//        CloseWorkFile();
//     }
	}
}

void WriteHistasASCII(TH1 *hist, TGWindow *window, Int_t ascii_graph)
{
// *INDENT-OFF*
  const char helpTextASCII[] =
"As default only the channel contents are written\n\
to the file.\n\
\"Errors X\" and \"Errors Cont(Y)\" are useful if file\n\
should be used as input to Ascii2Graph: X, Y, ErrX, ErrY\n\
If First_binX and Last_binX are 0 all channels arr written";
  const char helpTextGraph[] =
"Store BinCenters, BinContents in a TGraphErrors object\n\
and write the Graph to a ROOT file\n\
If \"First_binX\" and \"Last_binX\" are 0 all channels arr written";
// *INDENT-ON*
	
//	if ( hist->GetDimension() == 3 ) {
//		WarnBox(" WriteHistasASCII: 3-dim not yet supported ", window);
//		return;
//	}
	if ( hist->GetDimension() != 1 && ascii_graph == 1) {
		WarnBox(" WriteHistasGraph: only 1-dim supported", window);
		return;
	}
	static Int_t channels   = 0;
	static Int_t bincenters = 1;
	static Int_t binw2      = 0;
	static Int_t errors     = 0;
	if ( hist->GetDimension() == 1 )
		errors     = 1;
	static Int_t first_binX  = 0;
	static Int_t last_binX   = 0;
	static Int_t first_binY  = 0;
	static Int_t last_binY   = 0;
	static Int_t first_binZ  = 0;
	static Int_t last_binZ   = 0;
	Int_t transX             = 0;
	Double_t offsetX = 0;
	Double_t slope = 1;
	Int_t dummy;
	static Int_t suppress_zeros = 1;
	TString formX("%10.10g");
	TString formY("%10.10g");
	TFile *rfile = NULL;
	TGraphErrors *graph = NULL;
	TString fname;
	TString gname;
	TString gtitle;
	const char *helpText = NULL;
	if ( ascii_graph == 0 ) {
		fname = hist->GetName();
		fname += ".ascii";
		helpText =  &helpTextASCII[0];
	} else {
		gname  = hist->GetName();
		gtitle = hist->GetTitle();
		fname = "workfile.root";
		helpText =  &helpTextGraph[0];
	}

	TList *row_lab = new TList();
	static void *valp[50];
	Int_t ind = 0;
	Bool_t ok = kTRUE;

	if ( ascii_graph == 0 ) {
		row_lab->Add(new TObjString("StringValue_File name"));
		valp[ind++] = &fname;
	} else {
		row_lab->Add(new TObjString("StringValue_  Rootfile"));
		valp[ind++] = &fname;
		row_lab->Add(new TObjString("StringValue_Graph name"));
		valp[ind++] = &gname;
		row_lab->Add(new TObjString("StringValue_ Graph tit"));
		valp[ind++] = &gtitle;
	}
	if (hist->GetDimension() == 1) {
		if (  ascii_graph == 0 ) {
			row_lab->Add(new TObjString("CheckButton_ChannelNumbs"));
			valp[ind++] = &channels;
			row_lab->Add(new TObjString("CheckButton+   Bin Centers"));
			valp[ind++] = &bincenters;
		}
	}
	row_lab->Add(new TObjString("CheckButton_    Errors X"));
	valp[ind++] = &binw2;
	row_lab->Add(new TObjString("CheckButton+Errors Cont(Y)"));
	valp[ind++] = &errors;
	row_lab->Add(new TObjString("PlainIntVal_First_binX"));
	valp[ind++] = &first_binX;
	row_lab->Add(new TObjString("PlainIntVal+Last_binX"));
	valp[ind++] = &last_binX;
	if (hist->GetDimension() > 1 && ascii_graph == 0) {
		row_lab->Add(new TObjString("PlainIntVal_First_binY"));
		valp[ind++] = &first_binY;
		row_lab->Add(new TObjString("PlainIntVal+Last_binY"));
		valp[ind++] = &last_binY;
	}
	if (hist->GetDimension() > 2 && ascii_graph == 0) {
		row_lab->Add(new TObjString("PlainIntVal_First_binZ"));
		valp[ind++] = &first_binY;
		row_lab->Add(new TObjString("PlainIntVal+Last_binZ"));
		valp[ind++] = &last_binY;
	}
	row_lab->Add(new TObjString("CheckButton_Suppress empty channels"));
	valp[ind++] = &suppress_zeros;
	row_lab->Add(new TObjString("CommentOnly+ "));
	valp[ind++] = &dummy;
	row_lab->Add(new TObjString("StringValue_Form BinCenter"));
	valp[ind++] = &formX;
	row_lab->Add(new TObjString("StringValue+Form Cont, Err"));
	valp[ind++] = &formY;

	row_lab->Add(new TObjString("CheckButton_Apply linear trans to Xscale"));
	valp[ind++] = &transX;
	row_lab->Add(new TObjString("DoubleValue_OffsetX"));
	valp[ind++] = &offsetX;
	row_lab->Add(new TObjString("DoubleValue+Slope"));
	valp[ind++] = &slope;

	Int_t itemwidth=400;
	TString tit;
	if (  ascii_graph == 0 ) {
		tit = "Hist to ASCII-file";
	} else {
		tit = "Hist to Graph";
	}
	ok = GetStringExt(tit, NULL, itemwidth, window,
						 NULL, NULL, row_lab, valp, NULL, NULL, helpText);
	if (!ok) {
		return;
	}
	Int_t nl = 0;
	Int_t nbx1 = 1;
	Int_t nbx2 = hist->GetNbinsX();
	if (first_binX > 0) nbx1 = first_binX;
	if (last_binX > 0)  nbx2 = last_binX;
	Int_t ntot = 0;
	if ( hist->GetDimension() == 1 ) {
		for (Int_t i = nbx1; i <= nbx2; i++) {
			if ( hist->GetBinContent(i) == 0 && suppress_zeros )
				continue;
			ntot++;
		}
	}
	ofstream *outfile;
	if ( ascii_graph == 0 ) {
		if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
	//      cout << fname << " exists" << endl;
			if ( !QuestionBox("File exists, overwrite?",window) ) 
				return;
		}
		outfile= new ofstream((const char *) fname);
		if (outfile->fail()) {
			cout << "Opening: " << fname << " failed" << endl;
			return;
		}
	} else {
		rfile = new TFile(fname, "UPDATE");
		graph = new TGraphErrors(ntot);
		graph->SetName(gname);
		graph->SetTitle(gtitle);
	}
	if ( hist->GetDimension() == 1 ) {
		for (Int_t i = nbx1; i <= nbx2; i++) {
			Double_t xerr = hist->GetBinWidth(i) / TMath::Sqrt(12);
			Double_t x = hist->GetBinCenter(i);
			if ( transX )
				x = offsetX + slope * x;
			if ( ascii_graph == 0 && binw2 == 0) 
				xerr = 0;
			if (bincenters && suppress_zeros &&
				 hist->GetBinContent(i)  == 0) continue;
			if ( ascii_graph == 0 ) {
				if (channels)
					*outfile << i << " ";
				if (bincenters)
					*outfile << Form(formX.Data(), x) << " ";
				*outfile << hist->GetBinContent(i);
				if (binw2)
					*outfile << " " <<  Form(formY.Data(), xerr);
				if (errors)
					*outfile << " " << Form(formY.Data(), hist->GetBinError(i));
				*outfile << std::endl;
			} else {
				graph->SetPoint(nl, x, hist->GetBinContent(i)); 
				if (errors)
					graph->SetPointError(nl,xerr,hist->GetBinError(i)); 
			}
			nl++;
		}

	} else if ( hist->GetDimension() == 2 ) {
		Int_t nby1 = 1;
		Int_t nby2 = hist->GetNbinsY();
		if (first_binY > 0) nby1 = first_binY;
		if (last_binY > 0)  nby2 = last_binY;
		TAxis * xa = hist->GetXaxis();
		TAxis * ya = hist->GetYaxis();
		for (Int_t i = nbx1; i <= nbx2; i++) {
			for (Int_t k = nby1; k <= nby2; k++) {
				if (suppress_zeros && hist->GetBinContent(i, k)  == 0) continue;
				*outfile << Form(formX.Data(), xa->GetBinCenter(i)) << " ";
				*outfile << Form(formX.Data(), ya->GetBinCenter(k)) << " ";

				*outfile << Form(formY.Data(), hist->GetBinContent(i,k));
				if (errors)
				*outfile << " " << Form(formY.Data(), hist->GetCellError(i,k));
				*outfile << std::endl;
				nl++;
			}
		}
	} else if ( hist->GetDimension() == 3 ) {
		TH3S *h3 = (TH3S*)hist;
		Int_t nby1 = 1;
		Int_t nby2 = hist->GetNbinsY();
		if (first_binY > 0) nby1 = first_binY;
		if (last_binY > 0)  nby2 = last_binY;
		Int_t nbz1 = 1;
		Int_t nbz2 = hist->GetNbinsZ();
		if (first_binZ > 0) nbz1 = first_binZ;
		if (last_binZ > 0)  nbz2 = last_binZ;
		TAxis * xa = h3->GetXaxis();
		TAxis * ya = h3->GetYaxis();
		TAxis * za = h3->GetZaxis();
		if (gHprDebug > 0)
			h3->Dump();
		for (Int_t i = nbx1; i <= nbx2; i++) {
			for (Int_t k = nby1; k <= nby2; k++) {
				for (Int_t l = nbz1; l <= nbz2; l++) {
					if (suppress_zeros && h3->GetBinContent(i, k, l)  == 0) continue;
					*outfile << Form(formX.Data(), xa->GetBinCenter(i)) << " ";
					*outfile << Form(formX.Data(), ya->GetBinCenter(k)) << " ";
					*outfile << Form(formX.Data(), za->GetBinCenter(l)) << " ";

					*outfile << Form(formY.Data(), h3->GetBinContent(i,k,l));
					if (errors)
					*outfile << " " << Form(formY.Data(), hist->GetBinError(i,k,l));
					*outfile << std::endl;
					nl++;
				}
			}
		}
	}
	if ( ascii_graph == 0 ) {
		cout << nl << " lines written to: " << (const char *) fname << endl;
		outfile->close();
	} else {
		graph->Write();
		rfile->Close();
		cout << "Graph with " << nl << " points written to: " << fname << endl;
	}
};
//___________________________________________________________________________

void WarnBox(const char *message, TGWindow *window)
{
	Int_t retval = 0;
	new TGMsgBox(gClient->GetRoot(), window,
					 "Warning", message, kMBIconExclamation, kMBDismiss,
					 &retval);
}
//___________________________________________________________________________

Bool_t QuestionBox(const char *message, TGWindow *window)
{
		int buttons = kMBOk | kMBDismiss, retval = 0;
		new TGMsgBox(gClient->GetRoot(), window, "Question", message,
						 kMBIconQuestion, buttons, &retval);
		 return ( retval != kMBDismiss );
}

//_______________________________________________________________________________________

TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t push_pop)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) {
			TList *lof = ((TH1*)obj)->GetListOfFunctions();
			TObject *o = lof->FindObject(fname);
			if (o) {
				if (push_pop != 0) {
					lof->Remove(o);
					if (push_pop > 0) 
						lof->AddFirst(o);
					else
						lof->Add(0);
				}
				ca->Modified();
				ca->Update();
				return (TH1*)obj;
			}
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TH1 * FindHistInPad(TVirtualPad * ca, Int_t lfgraph, Int_t lfstack, TObject **parent)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if ( parent )
			*parent = obj;
		if (obj->InheritsFrom("TH1")) {
			TH1* h = (TH1*)obj;
			if ( h->GetEntries() == 0 ) {
				TString na = h->GetName();
				if ( na.EndsWith("_Clone") ) {
					na = na(0, na.Length()-6);
					TH1* ho = (TH1*)gROOT->GetList()->FindObject(na);
					if ( ho ) {
						ho->GetXaxis()->SetRange(h->GetXaxis()->GetFirst(),h->GetXaxis()->GetLast()); 
						if (ho->GetDimension() > 1)
							ho->GetYaxis()->SetRange(h->GetYaxis()->GetFirst(),h->GetYaxis()->GetLast()); 
						if (ho->GetDimension() > 2)
							ho->GetZaxis()->SetRange(h->GetZaxis()->GetFirst(),h->GetZaxis()->GetLast()); 
						cout << setblue << "Selected hist was a clone, using original one" 
						<< setblack << endl;
						h = ho;
					}
				}
			}	
			return h;
		}
		if (lfgraph && obj->InheritsFrom("TGraph")) {
			return ((TGraph*)obj)->GetHistogram();
		}
		if (lfgraph && obj->InheritsFrom("TGraph2D")) {
			return ((TGraph2D*)obj)->GetHistogram();
		}
		if (lfstack && obj->InheritsFrom("THStack")) {
			return ((THStack*)obj)->GetHistogram();
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TGraph * FindGraphInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph")) {
			return (TGraph*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TGraph2D * FindGraph2D(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TGraph2D * gr = NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph2D")) {
			 gr = (TGraph2D*)obj;
		}
	}
	return gr;
};
//____________________________________________________________

void SuperimposeGraph2Dim(TCanvas * current)
{
/*	static const char helptext[] =
	"\n\
	A selected graph is drawn in the same pad\n\
	";
*/
	current->cd();
	TObject * obj = gHpr->GetSelGraphAt(0);
   if ( !obj || !obj->InheritsFrom("TGraph2D") ) {
      cout << "Please select exactly  one Graph2D" << endl;
      return;
   }
   TGraph2D * gr = (TGraph2D*)obj;
   TGraph2D * gr_exist = FindGraph2D(current);
	if ( !gr_exist )
		return;
	TString opt(gr_exist->GetDrawOption());
	opt+= "SAME";
	cout << "SuperimposeGraph2Dim " << opt << endl;
	gr->Draw(opt);
	current->Modified();
	current->Update();

}
//_______________________________________________________________________________________

TLegend * FindLegendInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TLegend")) {
			return (TLegend*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

Bool_t HistLimitsMatch(TH1* h1, TH1* h2)
{
//	cout << " HistLimitsMatch : " << h1->GetName() 
//		<< " " << h2->GetName()<< endl;
	if (h1->GetDimension() != h2->GetDimension() ) 
		return kFALSE;
	TAxis * a1 = h1->GetXaxis();
	TAxis * a2 = h2->GetXaxis();
	if ( a1->GetNbins() != a2->GetNbins() )
		return kFALSE;
	if ( a1->GetXmin() != a2->GetXmin() )
		return kFALSE;
	if ( a1->GetXmax() != a2->GetXmax() )
		return kFALSE;
	if (h1->GetDimension() > 1 && h2->GetDimension() > 1 ) {
		a1 = h1->GetYaxis();
		a2 = h2->GetYaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	if (h1->GetDimension() > 2 && h2->GetDimension() > 2 ) {
		a1 = h1->GetZaxis();
		a2 = h2->GetZaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	return kTRUE;
}
//__________________________________________________________________

HprGaxis * DoAddAxis(TCanvas * canvas, TH1 *hist, Int_t where, 
			Double_t ledge, Double_t uedge, Double_t axis_offset, Color_t col)
{	
	canvas->cd();
//	GrCanvas* hc = (GrCanvas*)canvas;
//   canvas->Add2ConnectedClasses(this);
// we dont have a pointer to Set1DimOptDialog,
// it might not even exist (yet), so use static method
//	TQObject::Connect("Set1DimOptDialog", "LinLogChanged(TObject*)",
//						   "FitHist", this, "HandleLinLogChanged(TObject*)");
//	cout << "DoAddAxis " << ledge << " " << uedge << endl;
	Axis_t x1=0, y1=0, x2=1, y2=1;
	TIter next(canvas->GetListOfPrimitives());
//	canvas->GetListOfPrimitives()->ls();
	
	TString side("S");
	TAxis *orig_axis = NULL;
	HprGaxis *naxis;
	Double_t offset = 0, ratio = 1;
	if (where == 1) {
		if (canvas->GetLogx()) 
			side += "G";
		side += "-";
		x1 = canvas->GetFrame()->GetX1();
		x2 = canvas->GetFrame()->GetX2();
		y1 = canvas->GetFrame()->GetY2();
		// axis offset in X
		y1 = y1 + (y1 - canvas->GetFrame()->GetY1()) * axis_offset;
		y2 = y1;
		orig_axis = hist->GetXaxis();
		if ( canvas->GetLogx() ) {
			x1 = TMath::Power(10, x1);
			x2 = TMath::Power(10, x2);
		}
		offset = ledge - x1;
		ratio  = (uedge - ledge) / (x2 - x1);
	} else {
		if (canvas->GetLogy()) 
			side += "G";
		side += "+L";
		x1 = canvas->GetFrame()->GetX2();
		// axis offset in X
		x1 = x1 + (x1 - canvas->GetFrame()->GetX1()) * axis_offset;
		x2 = x1;
		y1 = canvas->GetFrame()->GetY1();
		y2 = canvas->GetFrame()->GetY2();
		orig_axis = hist->GetYaxis();
		if ( canvas->GetLogy() ) {
			y1 = TMath::Power(10, y1);
			y2 = TMath::Power(10, y2);
		}
		offset = ledge - y1;
		ratio  = (uedge - ledge) / (y2 - y1);
	}
	if ( where == 2 && canvas->GetLogx() ) {
		x1 = TMath::Power(10, x1);
		x2 = TMath::Power(10, x2);
	}
	if ( where == 1 &&  canvas->GetLogy() ) {
		y1 = TMath::Power(10, y1);
		y2 = TMath::Power(10, y2);
	}
	
	Int_t nd   = 0;
	Size_t ls  = 1;
	Color_t lc = 1;
	Font_t  lf = 62;
	Double_t lo= 0;
	Double_t tl= 1;
	if( orig_axis != NULL ) {
		// a TAxis
		nd = orig_axis->GetNdivisions();
		ls = orig_axis->GetLabelSize();
		lc = orig_axis->GetLabelColor();
		lf = orig_axis->GetLabelFont();
		lo = orig_axis->GetLabelOffset();
		tl = orig_axis->GetTickLength();
	}
	if ( col > 0 ) {
		lc = col;
	}	
	
	if (gHprDebug > 0)
	 cout   << "DoAddAxis: x1"
			  << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2 << " ledge " 
			  << ledge << " uedge " << uedge << " ratio " << ratio << " offset " << offset
			  << " axis_offset " << axis_offset << endl;
	
	naxis = new HprGaxis(canvas, x1, y1, x2, y2, ledge, uedge, nd, side.Data());
	naxis->SetWhere(where);
	naxis->SetOffset(offset);
	naxis->SetAxisOffset(axis_offset);
	naxis->SetRatio(ratio);
	
	naxis->SetLabelSize(ls);
	naxis->SetLineColor(lc);
	naxis->SetLabelColor(lc);
	naxis->SetLabelFont(lf);
	naxis->SetLabelOffset(lo);
	naxis->SetTickSize(tl);
	naxis->Draw();
	canvas->Modified(kTRUE);
	canvas->Update();

	return naxis;
};
//____________________________________________________________________________________

Int_t SuperImpose(TCanvas * canvas, TH1 * selhist, Int_t mode)
{
	static const char helptext[] =
	"\n\
	A selected hist is drawn in the same pad\n\
	The histogram may be scaled automatically: \n\
	The scale is adjusted such that the maximum value\n\
	of the superimposed histogram is equal to the maximum\n\
	of the original histogram in the displayed range.\n\
	So a certain peak can used as reference by zooming in\n\
	on this peak. Any value may be selected manually.\n\
	\n\
	An extra axis may be drawn on the right side:\n\
	The AxOffs determines its position, 0 is on top\n\
	of the right edge of the frame, negative is left\n\
	of it in the pad, positive at the right side\n\
	This is useful if more than one histogram is superimposed\n\
	The color of the extra axis equals the color of the hist.\n\
	With option \"Incr Colors\" Line-, Mark- FillColors are\n\
	automatically incremented (red->green->blue etc.)\n\
	In the case the histogram is filled the opacity may be\n\
	choosen between: 1 = completely opaque and 0 = transprent\n\
	With \"Skip Dialog\" this widget is only shown once\n\
	for the first histogram\n\
		\n\
		";
//	HprLegend * fLegend = (HprLegend*)canvas->GetListOfPrimitives()->FindObject("Legend_SuperImposeHist");
//	HprLegend * fLegend = (HprLegend*)FindLegendInPad(canvas);
	TH1 *hist;
	Int_t nhs = 0;
	//   TPaveLabel *tname;
	//  choose from histo list
	//   Int_t nh = gHpr->GetSelectedHist()->GetSize();
	if (gHpr->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
		if (gHpr->GetSelectedHist()->GetSize() > 1) {
			Hpr::WarnBox("More than 1 selection,\n please choose only one");
			return nhs;
		}
		hist = gHpr->GetSelHistAt(0);
	} else {
		hist = Hpr::GetOneHist(selhist, canvas);      // look in memory
	}
	if ( !hist ) {
//		Hpr::WarnBox("No hist selected");
		cout << setred << "No histogram selected" << setblack << endl;
		return nhs;
	}
	if (hist->GetDimension() != selhist->GetDimension()) {
		Hpr::WarnBox("Dimensions of histograms differ");
		return nhs;
	}
	hist->SetMinimum(-1111);
	hist->SetMaximum(-1111);
	
	//    cout << "hist->GetName() " << hist->GetName() << endl;
	TEnv env(".hprrc");
	static Int_t   lLegend      = env.GetValue("SuperImposeHist.DrawLegend", 1);
	static Int_t   lIncrColors  = env.GetValue("SuperImposeHist.AutoIncrColors", 1);
	static Int_t   lSkipDialog  = env.GetValue("SuperImposeHist.SkipDialog", 0);
	static Int_t   lWarnDiffBin = env.GetValue("SuperImposeHist.WarnDiffBin", 1);
	static Int_t   lPaintStat   = env.GetValue("SuperImposeHist.PaintStat", 1);
	
	Double_t axis_offset;
	static Double_t label_offset = 0.01;
	Color_t  axis_color;
	
	Color_t lLColor = 2; 
	Color_t lMColor = 2;
	Color_t lFillColor = 2; 
	
	Color_t maxLColor = 2; 
	Color_t maxMColor = 2;
	Color_t maxFillColor = 2; 
	
	TRootCanvas * win = (TRootCanvas*)canvas->GetCanvasImp();
	TIter next(canvas->GetListOfPrimitives());
	Int_t nhists = 0;
	Int_t naxis  = 0;
	TObject *obj;
	TH1* horig = NULL;
	while ( obj  = next() ) {
		if ( obj->InheritsFrom("TGaxis") ) {
			naxis ++;
			continue;
		}
		if ( obj->InheritsFrom("TH1") ) {
			horig = (TH1*)obj;
			TString oname(obj->GetName());
			TString hname(hist->GetName());
			if ( lWarnDiffBin && !Hpr::HistLimitsMatch(horig, hist) ) {
				if ( !QuestionBox("Hist limits or bins differ, really superimpose?",win) ) {
					return nhs;
				} else {
					hist->GetXaxis()->Set(horig->GetXaxis()->GetNbins(),
						 horig->GetXaxis()->GetXmin(), horig->GetXaxis()->GetXmax());
					if (horig->GetDimension() == 2) {
						hist->GetYaxis()->Set(horig->GetYaxis()->GetNbins(),
						horig->GetYaxis()->GetXmin(), horig->GetYaxis()->GetXmax());
					}
				}
			}
			if (horig->GetLineColor() > maxLColor)
					maxLColor = horig->GetLineColor();
			if (horig->GetMarkerColor() > maxMColor)
					maxMColor = horig->GetMarkerColor();
			if (horig->GetFillColor() > maxFillColor)
					maxFillColor = horig->GetFillColor();
			nhists++;
		}
	}
	if (gHprDebug > 0) 
		cout << "nhists " << nhists
		 << " maxLColor " <<  maxLColor 
		 << " maxMColor " << maxMColor 
		 << " maxFillColor " << maxFillColor 
		<< endl;
	// extract rgb from transparent color
	if (maxFillColor > 10) {
		TColor *ct = gROOT->GetColor(maxFillColor);
		if (ct->GetRed() > 0.95) {
			if (ct->GetGreen() > 0.95) {
				if (ct->GetBlue() > 0.95) {
					maxFillColor = 0;			//white
				} else {
					maxFillColor = 5; 		// yellow
				}
			} else {
				if ( ct->GetBlue() > 0.95) {
					maxFillColor = 6;				// magenta
				} else {
					maxFillColor = 2;				// red
				}
			}
		} else {
			if (ct->GetGreen() > 0.95) {
				if (ct->GetBlue() > 0.95) {
					maxFillColor = 7;			// cyan
				} else {
					maxFillColor = 3; 		// green
				}
			} else {
				if (ct->GetBlue() > 0.95) {
					maxFillColor = 4;			// blue
				} else {
					maxFillColor = 1; 		// black
				}
			}
		}
	}
	if (gHprDebug > 0) 
		cout << " maxFillColor conv " << maxFillColor 
		<< endl;
	if ( lIncrColors && nhists > 1) {
		lLColor = maxLColor + 1;
		lMColor = maxMColor + 1;
		lFillColor = maxFillColor + 1;
	}
	axis_offset = 0.06 * naxis;;
	if ( !horig ) 
		horig = selhist;
	Int_t do_scale = mode;
	Int_t auto_scale = mode;
//	lFillColor = env.GetValue("SuperImposeHist.FillColor", 2);

	axis_color = lLColor;
	static Style_t lFStyle;
	static Style_t lLStyle;
	static Width_t lLWidth;
	static Style_t lMStyle;
	static Size_t  lMSize;
	static Float_t lOpacity;
	static Double_t lStatBoxDX1;
	static Double_t lStatBoxDX2;
	static Double_t lStatBoxDY1;
	static Double_t lStatBoxDY2;
	// 	static Int_t   dummy;
	lFStyle     = horig->GetFillStyle();
	lLStyle     = horig->GetLineStyle();
	lLWidth     = horig->GetLineWidth();
	lMStyle     = horig->GetMarkerStyle();
	lMSize      = horig->GetMarkerSize();
	lOpacity		= env.GetValue("SuperImposeHist.FillOpacity",0.8);
	TPaveStats *sbo = (TPaveStats*)horig->GetListOfFunctions()->FindObject("stats");
	if ( sbo ) {
		lStatBoxDX1 = sbo->GetX1NDC();
		lStatBoxDX2 = sbo->GetX2NDC();
		lStatBoxDY1 = sbo->GetY1NDC();
		lStatBoxDY2 = sbo->GetY2NDC();
		
	}
	if (hist->GetDimension() == 1 ) {
		if ( !sbo ) {
			lStatBoxDX1 = env.GetValue("Set1DimOptDialog.StatBox1D.fX1", 0.8);
			lStatBoxDX2 = env.GetValue("Set1DimOptDialog.StatBox1D.fX2", 0.9);
			lStatBoxDY1 = env.GetValue("Set1DimOptDialog.StatBox1D.fY1", 0.8);
			lStatBoxDY2 = env.GetValue("Set1DimOptDialog.StatBox1D.fY2", 0.9);
		}
	} else {
		
		if ( !sbo ) {
			lStatBoxDX1 = env.GetValue("Set2DimOptDialog.StatBox2D.fX1", 0.8);
			lStatBoxDX2 = env.GetValue("Set2DimOptDialog.StatBox2D.fX2", 0.9);
			lStatBoxDY1 = env.GetValue("Set2DimOptDialog.StatBox2D.fY1", 0.8);
			lStatBoxDY2 = env.GetValue("Set2DimOptDialog.StatBox2D.fY2", 0.9);
		}
	}
	
	if ( lMSize <= 0 ) lMSize = 1;
	Double_t new_scale = 1;   
	static TString axis_title;
	axis_title= hist->GetYaxis()->GetTitle();
	Int_t new_axis = do_scale;
	canvas->cd();
	TString drawopt = horig->GetDrawOption();
	if (drawopt.Length() == 0 && horig->GetSumw2N()>0) {
		if (gHprDebug > 0)
			cout << "Force drawopt = E " << endl;
		drawopt = "E";
	}
	if ( gHprDebug  > 0 ) {
		cout << "horig->GetDrawOption() |" << drawopt << "|"<< endl;
		horig->Print();
		cout << "hist->GetDrawOption() |" << hist->GetDrawOption()<< "|" << endl;
		hist->Print();
	}
	if ( lSkipDialog == 0 || nhists < 2 ) {
		// Error modes 
		static void *valp[50];                    
		Int_t ind = 0;                            
		TList *row_lab = new TList();
		Bool_t ok = kTRUE;
		row_lab->Add(new TObjString("CheckButton_New scale  "));
		valp[ind++] = &do_scale;
		row_lab->Add(new TObjString("CheckButton+Auto scale "));
		valp[ind++] = &auto_scale;
		row_lab->Add(new TObjString("DoubleValue+Factor"));
		valp[ind++] = &new_scale;
		row_lab->Add(new TObjString("CheckButton_Extra axis "));
		valp[ind++] = &new_axis;
		row_lab->Add(new TObjString("DoubleValue+AxOffs;-1;1 "));
		valp[ind++] = &axis_offset;
		row_lab->Add(new TObjString("DoubleValue+LabOffs"));
		valp[ind++] = &label_offset;
		row_lab->Add(new TObjString("ColorSelect_AxisColor"));
		valp[ind++] = &axis_color;
		row_lab->Add(new TObjString("StringValue+AxTitle"));
		valp[ind++] = &axis_title;
		row_lab->Add(new TObjString("CheckButton+Legend  "));
		valp[ind++] = &lLegend;
		row_lab->Add(new TObjString("ColorSelect_MarkColor"));
		valp[ind++] = &lMColor;
		row_lab->Add(new TObjString("Mark_Select+MarkStyle"));
		valp[ind++] = &lMStyle;
		row_lab->Add(new TObjString("Float_Value+MSize"));
		valp[ind++] = &lMSize;
		row_lab->Add(new TObjString("ColorSelect_LineColor"));
		valp[ind++] = &lLColor;
		row_lab->Add(new TObjString("LineSSelect+LStyle"));
		valp[ind++] = &lLStyle;
		row_lab->Add(new TObjString("PlainShtVal+LineWidth"));
		valp[ind++] = &lLWidth;
		row_lab->Add(new TObjString("ColorSelect_FillColor"));
		valp[ind++] = &lFillColor;
		row_lab->Add(new TObjString("Fill_Select+FillStyle"));
		valp[ind++] = &lFStyle;
		// 		row_lab->Add(new TObjString("CommentOnly+-"));
		// 		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("Float_Value+Opacity;0.;1."));
		valp[ind++] = &lOpacity;
//		row_lab->Add(new TObjString("StringValue+DrawOpt"));
//		valp[ind++] = &drawopt;
		row_lab->Add(new TObjString("CheckButton_Incr Colors "));
		valp[ind++] = &lIncrColors;
		row_lab->Add(new TObjString("CheckButton+Skip Dialog"));
		valp[ind++] = &lSkipDialog;
		row_lab->Add(new TObjString("CheckButton+Statboxes"));
		valp[ind++] = &lPaintStat;
		//		row_lab->Add(new TObjString("CheckButton+NoStatBox"));
		//		valp[ind++] = &lNoStatBox;
		
		Int_t itemwidth = 380;
		ok = GetStringExt("Superimpose Histogram", NULL, itemwidth, win,
								NULL, NULL, row_lab, valp,
								NULL, NULL, helptext);
								if (!ok )
									return nhs;
	}
	if ( do_scale != 0 && auto_scale != 0 ) {
		//		new_scale = hymax / rightmax;
		cout << "Scale will be auto adjusted " << endl;
	}
	// change name of original histogram to protect against autodelete
	TString origname(horig->GetName());
	if ( !origname.Contains("supimp") ) {
//		horig->Clone();
		origname += "_has_supimp";
		horig->SetName(origname);
		if (gHprDebug > 0)
		cout << "Make a clone and rename histogram to: " << origname << endl;
	}
	//	TGaxis *naxis = 0;
	TH1 *hdisp = (TH1 *) hist->Clone();
	TString name = hdisp->GetName();
	name += "_is_supimp";
	hdisp->SetName(name.Data());
	if (do_scale && selhist->GetDimension() != 2)  {
		//		cout << "!!!!!!!!!!!!!!!!" << endl;
		if ( auto_scale ) {
			Stat_t maxy = 0;
			if (selhist->GetXaxis()->GetNbins() != hist->GetXaxis()->GetNbins()) {
				//  case expanded histogram
				Axis_t x, xmin, xmax;
				xmin = selhist->GetXaxis()->GetXmin();
				xmax = selhist->GetXaxis()->GetXmax();
				for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++) {
					x = hist->GetBinCenter(i);
					if (x >= xmin && x < xmax && hist->GetBinContent(i) > maxy)
						maxy = hist->GetBinContent(i);
				}
			} else {
				//  case zoomed histogram
				for (Int_t i = selhist->GetXaxis()->GetFirst();
				i <= selhist->GetXaxis()->GetLast();
				i++) {
					if (hist->GetBinContent(i) > maxy)
						maxy = hist->GetBinContent(i);
				}
				
			}
			if ( maxy == 0 ) {
				cout << "Max = 0 " << endl;
				return nhs;
			}
			new_scale = selhist->GetMaximum() / maxy;
		} else {
			//			new_scale = 1;
		}
		cout << "Scale " << hdisp->GetName() << " by " << new_scale << endl;
		hdisp->Scale(new_scale);
		for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
			hdisp->SetBinError(i, new_scale * hist->GetBinError(i));
		}
		cout << "Superimpose: Scale errors linearly" << endl;
	} 
	canvas->cd();
	hdisp->SetLineColor(lLColor);
	hdisp->SetLineStyle(lLStyle);
	hdisp->SetFillStyle(lFStyle);
	if ( hist->GetDimension() == 2 ) 
		hdisp->SetFillColor(lFillColor);
	hdisp->SetLineWidth(lLWidth);
	hdisp->SetMarkerColor(lMColor);
	hdisp->SetMarkerStyle(lMStyle);
	hdisp->SetMarkerSize(lMSize);
	if ( hist->GetDimension() == 1 ) {
		if (!drawopt.Contains("E", TString::kIgnoreCase)) {
			if (!drawopt.Contains("hist",TString::kIgnoreCase)) {
				drawopt += "hist";
			}
		}
		if ( env.GetValue("Set1DimOptDialog.fFill1Dim", 0 ) > 0 ){ 
#if ROOTVERSION >= 53418
			hdisp->SetFillColorAlpha(lFillColor, lOpacity);
#else
			hdisp->SetFillColor(lFillColor);
#endif
			drawopt += "F";
			axis_color = lFillColor;
		} else {
			hdisp->SetFillColor(0);
			if (drawopt.Contains("hist",TString::kIgnoreCase)) {
				axis_color = lLColor;
			} else {
				axis_color = lMColor;
			}
		}
	}
	if (hdisp->GetDimension() == 2) {
		TRegexp zopt("Z");
		drawopt(zopt)="";
	}
	if (gHprDebug > 0) 
		cout << "DrawCopy(drawopt) 1 " << drawopt << endl;
	if ( lPaintStat > 0) {
		if ( !drawopt.Contains("SAMES") )
		drawopt += "SAMES";
	} else {
		if ( !drawopt.Contains("SAME") )
		 drawopt += "SAME";
	}
	if (gHprDebug > 0) 
		cout << "DrawCopy(drawopt) 2 " << drawopt << endl;
	TH1 * hdrawn = hdisp->DrawCopy(drawopt.Data());
	gPad->Modified();
	gPad->Update();
	nhs++;
	TPaveStats *sb = (TPaveStats*)hdrawn->GetListOfFunctions()->FindObject("stats");
	if ( sb ) {
		if (gHprDebug > 0) {
			cout << "SetStatBox: " << " n " << nhists 
			<< " X1 " << lStatBoxDX1<< 
			" X2 " << lStatBoxDX2<< 
			" Y1 " << lStatBoxDY1<< 
			" Y2 " << lStatBoxDY2<< 
			endl;
		}
		sb->SetX1NDC(lStatBoxDX1);
		sb->SetX2NDC(lStatBoxDX2);
		sb->SetY1NDC(lStatBoxDY1 - (lStatBoxDY2 - lStatBoxDY1));
		sb->SetY2NDC(lStatBoxDY2 - (lStatBoxDY2 - lStatBoxDY1));
	}
	if ( new_axis != 0 && hist->GetDimension() == 1 ) {
		//		TString opt("+SL");->GetFrame()->GetX1()
		Double_t ledge = gPad->GetFrame()->GetY1();
		Double_t uedge = gPad->GetFrame()->GetY2();
		if ( gPad->GetLogy() ) {
			ledge = TMath::Power(10, ledge);
			uedge = TMath::Power(10, uedge);
		}
		ledge /= new_scale;
		uedge /= new_scale;
		HprGaxis *axis = Hpr::DoAddAxis(canvas, hdisp, 2, ledge, uedge, axis_offset, axis_color);
		
		TString ax_name("axis_");
		ax_name += hdisp->GetTitle();
		axis->SetName(ax_name);
		axis->SetLineColor(axis_color);
		axis->SetLabelColor(axis_color);
		axis->SetTickSize   (env.GetValue("SetHistOptDialog.fTickLength", 0.01));
		axis->SetLabelFont  (env.GetValue("SetHistOptDialog.fLabelFont", 62));
		axis->SetLabelOffset(env.GetValue("SetHistOptDialog.fLabelOffsetY", 0.01));
		axis->SetLabelSize  (env.GetValue("SetHistOptDialog.fLabelSize", 0.03));
		axis->SetMaxDigits  (env.GetValue("SetHistOptDialog.fLabelMaxDigits", 4));
		if (axis_title.Length() > 0) {
			axis->SetTitle(axis_title);
			axis->SetTitleColor(axis_color);
			axis->SetTitleFont( env.GetValue("SetHistOptDialog.fTitleFont", 62));
			axis->SetTitleSize( env.GetValue("SetHistOptDialog.fTitleSize",0.03));
			axis->SetTitleOffset( 1. );
			if ( env.GetValue("SetHistOptDialog.fTitleCenterY", 0) == 1 ) 
				axis->CenterTitle();
		}
	}
	
	if ( lLegend != 0 ) {
//		TEnv env(".hprrc");
		Double_t x1 = 0.11;
		Double_t x2 = 0.3;
		Double_t y1 = 0.8;
		Double_t y2 = 0.95;
		if (GeneralAttDialog::fRememberLegendBox > 0) {
			x1 = env.GetValue("SuperImposeHist.fLegendX1",x1 );
			x2 = env.GetValue("SuperImposeHist.fLegendX2",x2 );
			y1 = env.GetValue("SuperImposeHist.fLegendY1",y1 );
			y2 = env.GetValue("SuperImposeHist.fLegendY2",y2 );
			if (x1 < 0 || x1 > 1 || x2 < 0 || x2 > 1
			 || y1 < 0 || y1 > 1 || y2 < 0 || y2 > 1 ) {
				cout << setred << "Unreasonable saved legend coordinates" <<
				endl << "set to defaults" << setblack << endl;
				x1 = 0.11;
				x2 = 0.3;
				y1 = 0.8;
				y2 = 0.95;
			}
		}
		// make new legend and remove possible extra TGaxis
		TIter no(gPad->GetListOfPrimitives());
		TObject *ob;
		while (( ob = no() )){
			if (ob->InheritsFrom("TLegend")) {
				gPad->GetListOfPrimitives()->Remove(ob);
				delete ob;
				break;
			}
		}
		TLegend * leg = gPad->BuildLegend(x1, y1, x2, y2);
		leg->SetName("Legend_SuperImposeHist");
		TList *lof = leg->GetListOfPrimitives();
		TIter nl(lof);
		while (( ob = nl() )){
			if (ob->IsA() == TLegendEntry::Class() ) {
				TObject *pr = ((TLegendEntry*)ob)->GetObject();
				if (pr && pr->InheritsFrom("TGaxis")) {
					lof->Remove(ob);
					delete ob;
				}
			}
		}
/*		
		TString opt;
		TString dopt;
		if ( fLegend == NULL ) {
			fLegend = new HprLegend(x1, y1, x2, y2, "", "brNDC");
			fLegend->SetName("Legend_SuperImposeHist");
			dopt = selhist->GetOption();
			if (selhist->GetDimension() == 2 ) {
				if ( dopt.Contains("SCAT", TString::kIgnoreCase) ) opt = "P";
				if ( dopt.Contains("BOX", TString::kIgnoreCase) && 
					!dopt.Contains("BOX1", TString::kIgnoreCase) ) opt += "L";
				if ( dopt.Contains("BOX", TString::kIgnoreCase) &&
					selhist->GetFillColor() != 0 && selhist->GetFillStyle() != 0 || 
					dopt.Contains("BOX1", TString::kIgnoreCase) ) opt+= "F";
			} else {
				if (dopt.Length() == 0 || dopt.Contains("HIST",TString::kIgnoreCase) ||
					dopt.Contains("E", TString::kIgnoreCase) )
					opt = "L";
				if (selhist->GetFillStyle() != 0 )
					opt+= "F";
				if (dopt.Contains("P",TString::kIgnoreCase) )
					opt += "P";
			}
			fLegend->AddEntry(selhist, "", opt);
			//			cout << "fLegend->AddEntry: " << opt << endl;
			fLegend->Draw();
			*/
//		}
		/*
		opt = "";
		dopt = hdrawn->GetOption();
		if (selhist->GetDimension() == 2 ) {
			if ( dopt.Contains("SCAT", TString::kIgnoreCase) ) opt = "P";
			if ( dopt.Contains("BOX", TString::kIgnoreCase) && 
				!dopt.Contains("BOX1", TString::kIgnoreCase) ) opt += "L";
			if ( (dopt.Contains("BOX", TString::kIgnoreCase) &&
				lFillColor != 0 && lFStyle != 0) || 
				dopt.Contains("BOX1", TString::kIgnoreCase)) opt+= "F";
		} else {
			if (dopt.Length() == 0 || dopt.Contains("HIST",TString::kIgnoreCase) ||
				dopt.Contains("E", TString::kIgnoreCase) )
				opt = "L";
			if (hdrawn->GetFillStyle() != 0 )
				opt+= "F";
			if (dopt.Contains("P",TString::kIgnoreCase) )
				opt += "P";
		}
		//		cout << "fLegend->AddEntry: " << opt << endl;
		fLegend->AddEntry(hdrawn, "", opt);
		*/
	}
	canvas->Update();
	gHpr->ClearSelect();
	if ( hdrawn ) {
		TRegexp sa("SAME");
		TString dro(hdrawn->GetOption());
		dro(sa)="";
		//		hdrawn->SetDrawOption(dro);
		//		THistPainter *hp = (THistPainter*)hdrawn->GetPainter();
		//		hdrawn->Pop();
		canvas->Modified();
		canvas->Update();
	}
	/*
	if (lIncrColors > 0) {
		lFillColor += 1;
		lLColor += 1;
		lMColor += 1;
		axis_offset += 0.08;
	}
	*/
	env.SetValue("SuperImposeHist.FillColor", lFillColor);
	env.SetValue("SuperImposeHist.FillOpacity",lOpacity);
	env.SetValue("SuperImposeHist.DrawLegend", lLegend);
	env.SetValue("SuperImposeHist.AutoIncrColors", lIncrColors);
	env.SetValue("SuperImposeHist.SkipDialog", lSkipDialog);
	env.SetValue("SuperImposeHist.WarnDiffBin", lWarnDiffBin);
	env.SetValue("SuperImposeHist.PaintStat", lPaintStat);
	env.SaveLevel(kEnvLocal);
	return nhs;
}
//____________________________________________________________________________________

TH1 * GetOneHist(TH1 * selhist, TCanvas * canvas)
{
	Int_t nhists = 0;
//	TOrdCollection *entries = new TOrdCollection();
	TOrdCollection entries;
	TH1 * hist = NULL;
	TString hlist;
	TString hselect;
	TList *tl = gDirectory->GetList();
	TIter next(tl);
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) {
			hist = (TH1 *) obj;
			TString hn(hist->GetName());
			if ( hist != selhist && !hn.EndsWith("is_supimp") 
				&& hist->GetDimension() == selhist->GetDimension() ) {
				entries.AddFirst(new TObjString(hist->GetName()));
				hlist.Prepend(hist->GetName());
				hlist.Prepend(";");
				nhists++;
			}
		}
	}
	if (nhists <= 0) {
		Hpr::WarnBox("No Histogram found");
		return 0;
	}
	TRootCanvas * win = NULL;
	if ( canvas )
		win = (TRootCanvas*)canvas->GetCanvasImp();
	Int_t retval=0;
	Int_t itemwidth = 300;
	if ( nhists > 10 ) {
		void *lValp[2];
		TList lRow_lab;
		hlist.Prepend("ComboSelect_SelHist");
		
		lRow_lab.Add( new TObjString(hlist) );
		lValp[0] = &hselect;
		new TGMrbValuesAndText("Select a histogram", NULL, &retval,itemwidth, win,
								NULL, NULL, &lRow_lab, lValp,
								NULL, NULL);
		if ( retval < 0 )
			return NULL;
		
		hist = (TH1 *) gROOT->FindObject(hselect);
	} else {
		TArrayI flags(nhists);
		flags.Reset();
		retval = 2;
		
		itemwidth = 240;
		new TGMrbTableFrame(win, &retval, "Select a histogram", itemwidth, 1, nhists,
			&entries, 0, 0, &flags, nhists);
		if (retval < 0){
			return NULL;
		}
		TObjString * objs;
		TString s;
		for (Int_t i = 0; i < nhists; i++) {
			//      Char_t sel = selected[i];
			if (flags[i] == 1) {
				objs = (TObjString *) entries.At(i);
				s = objs->String();
				hist = (TH1 *) gROOT->FindObject((const char *) s);
				break;
			}
		}
	}
	
	if (!hist) {
		  Hpr::WarnBox("No Histogram found");
	}
	if (hist == selhist) {
		  Hpr::WarnBox("Cant use same histogram");
		  hist = NULL;
	}
//	hist->Print();
	return hist;
}
//__________________________________________________________________

void ResizeStatBox(TPaveStats * st, Int_t ndim) 
{
	TEnv env(".hprrc");
	TString envname("SetHistOptDialog.Stat");
	TString sdim;
	if (ndim == 2)
		sdim = "2D";
	TString resname;
	resname = envname + "X"  + sdim;
	Float_t statX = env.GetValue(resname, 0.9);
	st->SetX2NDC(statX);
	resname = envname + "W"  + sdim;
	Float_t statW  = env.GetValue(resname, 0.2);
	st->SetX1NDC(statX - statW);
	resname = envname + "Y" + sdim;
	Float_t statY =  env.GetValue(resname, 9);
	st->SetY2NDC(statY);
	resname = envname + "H" + sdim;
	Float_t statH  = env.GetValue(resname, 0.16);
	st->SetY1NDC(statY - statH);
	if (gHprDebug > 0) {
		cout << " ResizeStatBox statY"  << resname << " " << statY << endl;
	}	
}
//__________________________________________________________________

Bool_t IsSelected(const char * name, Int_t from_run, Int_t to_run)
{
	// Select file name by run number
	if (from_run < 0 || from_run > to_run) {
		cout << "Select files by run numbers, Illegal limits: "
		<< from_run << " " <<to_run << endl;
		return kFALSE;
	}
	TString fn(name);
	TRegexp numb("[0-9]+");
	Int_t len;
	Int_t st = numb.Index(fn,&len,0);
	if ( st < 0 ) 
		return kFALSE;
	if ( len < 3 ) {
		cout << "Not enough digits, skipping: " << name << endl;
		return kFALSE;
	}
	TString ns = fn(st,len);
	Int_t rn = ns.Atoi();
	if ( rn < from_run || rn > to_run) {
		return kFALSE;
	} 
	return kTRUE;
}
//__________________________________________________________________

Bool_t IsSelected(const char * name, TString * mask, Int_t use_regexp)
{
	enum e_HsOp {kHsOp_None, kHsOp_And, kHsOp_Or, kHsOp_Not};
	Int_t SelOp;
	Bool_t wildcard = kTRUE;
	if ( use_regexp > 0)
		wildcard = kFALSE;
	if ( mask->Length() <= 0 )
		return kTRUE;
	TString mask_1;
	TString mask_2;
	if      (mask->Contains("&")) {
		SelOp = kHsOp_And;
		Int_t indop = mask->Index("&");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else if (mask->Contains("|")) {
		SelOp = kHsOp_Or;
		Int_t indop = mask->Index("|");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else if (mask->Contains("!")) {
		SelOp = kHsOp_Not;
		Int_t indop = mask->Index("!");
		mask_1 = (*mask)(0, indop);
		mask_2 = (*mask)(indop+1, mask->Length());
	} else  {
		SelOp = kHsOp_None;
	}
	mask_1 = mask_1.Strip(TString::kBoth);
	mask_2 = mask_2.Strip(TString::kBoth);
	TString sname(name);
	if        (SelOp == kHsOp_None) {
		TRegexp re((const char *)*mask, wildcard);
		if (sname.Index(re) <0) return kFALSE;
		
	} else if ( wildcard == 0 ) {
		TRegexp re1((const char *)mask_1);
		TRegexp re2((const char *)mask_2);
		if (SelOp == kHsOp_And) {
			if (sname.Index(re1) < 0 ||
				sname.Index(re2) < 0)  return kFALSE;
		} else if (SelOp == kHsOp_Or) {
			if (sname.Index(re1) < 0 &&
				sname.Index(re2) < 0)  return kFALSE;
		} else if (SelOp == kHsOp_Not) {
			if (( mask_1.Length() > 0 &&
				sname.Index(re1) < 0) ||
				(sname.Index(re2) >=0))  return kFALSE;
		}
	} else {
		if (SelOp == kHsOp_And) {
			if (!sname.Contains(mask_1.Data()) ||
				!sname.Contains(mask_2.Data()))  return kFALSE;
		} else if (SelOp == kHsOp_Or) {
			if (!sname.Contains(mask_1.Data()) &&
				!sname.Contains(mask_2.Data()))  return kFALSE;
		} else if (SelOp == kHsOp_Not) {
			if (( mask_1.Length() > 0 &&
				!sname.Contains(mask_1.Data())) ||
				(sname.Contains(mask_2.Data())))  return kFALSE;
		}
	}
	return kTRUE;
}
//_________________________________________________________________________
	
void BoundingB3D(TPolyLine3D * pl,  Double_t x0, Double_t y0, Double_t z0, 
						  Double_t x1, Double_t y1, Double_t z1) 
{
//	cout << "TPolyLine3D: " << x0 << " " << y0 << " " << z0 
//	     << " " << x1 << " " << y1 << " "  << z1 << endl;
	TPolyLine3D * pl3;	  
	if ( pl )
		pl3 = pl;
	else	
		pl3 = new TPolyLine3D(16);
	pl3->SetPoint(0, x0, y0, z0);
	pl3->SetPoint(1, x1, y0, z0);
	pl3->SetPoint(2, x1, y0, z1);
	pl3->SetPoint(3, x0, y0, z1);
	pl3->SetPoint(4, x0, y0, z0);

	pl3->SetPoint(5, x0, y1, z0);
	pl3->SetPoint(6, x0, y1, z1);
	pl3->SetPoint(7, x0, y0, z1);
	pl3->SetPoint(8, x0, y1, z1);
	
	pl3->SetPoint(9, x1, y1, z1);
	pl3->SetPoint(10, x1, y0, z1);
	pl3->SetPoint(11, x1, y1, z1);
	pl3->SetPoint(12, x1, y1, z0);
	
	pl3->SetPoint(13, x1, y0, z0);
	pl3->SetPoint(14, x1, y1, z0);
	pl3->SetPoint(15, x0, y1, z0);
	pl3->Draw();
}
//________________________________________________________________________

TF1 * FindFunctionInPad(TVirtualPad * pad)
{
	if (pad )
		pad->cd();
	TList * lop = gPad->GetListOfPrimitives();
	TIter next(lop);
	TF1 *func = NULL;
	Int_t nfound = 0;
	TObject *obj;
	while ( (obj = next()) ) {
		if(obj->InheritsFrom("TF1")) {
			func = (TF1*)obj;
			nfound ++;
//			break;
		}
	}
	if (nfound > 1) {
		cout << "Warning: More than 1 function found " << nfound << endl;
		cout << "Using: " << func->GetName() << endl;
	}
	return func;
}
//________________________________________________________________________

void FillHistRandom(TVirtualPad* pad)
{
	if ( pad == NULL ) {
		cout << "NULL pointer to pad" << endl;
		return;
	}
		
	pad->cd();
	TF1 * func = FindFunctionInPad(pad);
	if ( func == 0 ) {
		cout << "No function found in: " << pad->GetName() << endl;
		return;
	}
	TH1 * hist = FindHistInPad(pad);
	if ( hist == 0 ) {
		cout << "No hist found in: " << pad->GetName() << endl;
		return;
	}
	Double_t integral = func->Integral(hist->GetXaxis()->GetXmin(),
													hist->GetXaxis()->GetXmax());
	cout << "Integral[" <<hist->GetXaxis()->GetXmin() << ", "
	<< hist->GetXaxis()->GetXmax() << "] = " << integral << endl;
//	func->Dump();
//	Int_t fillN = fNevents;
	Int_t fillN = (Int_t) integral / hist->GetBinWidth(1);
	if ( fillN <= 10 ) {
		fillN = 10000;
		cout << "Setting nentries to: " << fillN << endl;
	}
	if ( hist->GetEntries() != 0 ) {
		TRootCanvas * rc = NULL;
		if ( pad->GetCanvas() ) {
			rc = (TRootCanvas *)pad->GetCanvasImp();
		}
		if (Hpr::QuestionBox("Histogram not empty,\n Reset before filling?", rc) ) {
			hist->Reset();
		}
	}
	hist->FillRandom(func->GetName(), fillN);
	hist->SetLineColor(func->GetLineColor());
	hist->SetMarkerColor(func->GetLineColor());
	hist->SetMaximum(hist->GetBinContent(hist->GetMaximumBin()));
//	hist->Print();
	gPad->Modified();
	gPad->Update();
}
//______________________________________
void ReplaceUS(const char * fname, Int_t latex_header)
{
	ifstream infile;
	ofstream ofile;
   infile.open(fname, ios::in);
	if (!infile.good()) {
	cerr	<< "ReplaceUS: "
			<< gSystem->GetError() << " - " << fname
			<< endl;
		return;
	}
	const char repl[] = "\\textunderscore ";
	TString oname(fname);
	oname+= "_mod";
   ofile.open(oname.Data(), ios::out);
	if (!ofile.good()) {
	cerr	<< "ReplaceUS: "
			<< gSystem->GetError() << " - " << oname
			<< endl;
		return;
	}
	if (latex_header > 0) {
		ofile << "\\documentclass[a4paper,12pt]{article}" << endl;
		ofile << "\\usepackage{tikz}" << endl;
		ofile << "\\usetikzlibrary{patterns}" << endl;
		ofile << "\\usetikzlibrary{plotmarks}" << endl;
		ofile << "\\setlength{\\topmargin}{-2.5cm}" << endl;
		ofile << "\\setlength{\\oddsidemargin}{-0.5cm}" << endl;
		ofile << "\\setlength{\\evensidemargin}{-0.5cm}" << endl;
		ofile << "\\setlength{\\textwidth}{17cm}" << endl;
		ofile << "\\setlength{\\textheight}{28cm}" << endl;
		ofile << "\\begin{document}" << endl;
		ofile << "\\begin{figure}[htbp]" << endl;
		ofile << "\\begin{center}" << endl;
		ofile << "\\scalebox{0.8}{" << endl;
		ofile << "%% Begin code generated by TTexDump" << endl;
	}
   TString line;
	TRegexp us("_");
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
		if (line.Contains("_") && !line.Contains("$") ) {
			while (line.Contains("_") ) {
				line(us) = repl;
			}
		}
		ofile << line.Data() << endl;
	}
	if (latex_header > 0) {
		ofile << "%% End code generated by TTeXDump" << endl;
		ofile << "}"<< endl;
		ofile << "\\caption{Image ({\tt hp2.tex}) generated by {\tt TTeXDump}}" << endl;
		ofile << "\\end{center}" << endl;
		ofile << "\\end{figure}" << endl;
		ofile << "\\end{document}" << endl;
	}
	TString cmd("mv ");
	cmd += oname;
	cmd += " ";
	cmd += fname;
	gSystem->Exec(cmd);
}
//_________________________________________________________

Int_t MixPointsInGraph2D(TGraph2D * grin, TGraph2D * grout, Int_t npoints)
{
//
// Fill TGraph2D grout with points randomly reordered
// from grin. If npoints > 0 only npoints are filled
//
// With points ordered by increasing X, Y values 
// as a typical result from scanning procedures the
// Delaunay triangulation algorithm converges very
// slowly. Reordering can gain a factor 5 - 10 in speed
//

	Int_t np = grin->GetN();
	Int_t npuse = np;
	if (npoints > 0 && npoints < np) {
		npuse = npoints;
	} 
	Double_t *x = grin->GetX();
	Double_t *y = grin->GetY();
	Double_t *z = grin->GetZ();
	// fill index array
	std::vector<int>indarr;
	for (int i=0; i<np; ++i) {
		indarr.push_back(i);
	}
// shuffle using built-in random generator of std
	std::random_shuffle ( indarr.begin(), indarr.end() );
	Int_t n = 0;
	for (Int_t i=0; i<npuse; i++) {
		grout->SetPoint(i, x[indarr[i]], y[indarr[i]], z[indarr[i]]);
		n++;
	}
	return n;
}
//_______________________________________________________________________________________

TCanvas * FindCanvas(const Char_t * pat)
{
	TIter next(gROOT->GetListOfCanvases());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TCanvas")) {
			TString cname = ((TCanvas*)obj)->GetName();
			if (cname.Contains(pat))
				return (TCanvas*)obj;
		}
	}
	return NULL;
};
//___________________________________________________________________________

void SetUserPalette(Int_t startindex, TArrayI * pixels)
{
	Int_t ncont = pixels->GetSize();
	if (ncont <= 0) return;
	TColor * c;

	TArrayI colind(ncont);
	Float_t r=0, g=0, b=0;
	for (Int_t i = 0; i < ncont; i++) {
		colind[i] = i + startindex;
		c = gROOT->GetColor(i + startindex);
		if (c) delete c;
		c = new TColor(i + startindex, r, g, b);
		c->Pixel2RGB((*pixels)[i], r, g, b);
		c->SetRGB(r, g, b);
		gStyle->SetPalette(ncont, colind.GetArray());
	}
}
//___________________________________________________________________________

Int_t GetFreeColorIndex()
{
   // Search for the highest color index not used in ROOT:
   // We do not want to overwrite some colors...
   Int_t highestIndex = 0;
   TColor * color;
   TSeqCollection *colorTable = gROOT->GetListOfColors();
   if ((color = (TColor *) colorTable->Last()) != 0) {
      if (color->GetNumber() > highestIndex) {
         highestIndex = color->GetNumber();
      }
      while ((color = (TColor *) (colorTable->Before(color))) != 0) {
         if (color->GetNumber() > highestIndex) {
            highestIndex = color->GetNumber();
         }
      }
   }
   return highestIndex + 1;
}
//___________________________________________________________________________

Double_t DtoL(Double_t px, Double_t py, Double_t x1, Double_t y1, Double_t x2, Double_t y2 )
{
////////////////////////////////////////////////////////////////////////////////
/// Compute distance from point px,py to a line.
/// partially taken from TLine
///
/// Algorithm:
///   A(x1,y1)         P                             B(x2,y2)
///   -----------------+------------------------------
///                    |
///                    |
///                    |
///                    |
///                   M(x,y)
///
/// Let us call  a = distance AM     A=a**2
///              b = distance BM     B=b**2
///              c = distance AB     C=c**2
///              d = distance PM     D=d**2
///              u = distance AP     U=u**2
///              v = distance BP     V=v**2     c = u + v
///
/// D = A - U
/// D = B - V  = B -(c-u)**2
///    ==> u = (A -B +C)/2c
///        d = sqrt(A - u*u)
///
////////////////////////////////////////////////////////////////////////////////

   Double_t xl, xt, yl, yt;
   Double_t x     = px;
   Double_t y     = py;
//   if (x1 < x2)
		{xl = x1; xt = x2;}
//   else
//		{xl = x2; xt = x1;}
//  if (y1 < y2)
		{yl = y1; yt = y2;}
 //  else
//		{yl = y2; yt = y1;}
//  f(x) = co + sl * x
	Double_t sl = (yt - yl)/(xt - xl);
	Double_t co = yt - sl * xt;
	
   Double_t xx1   = x  - x1;
   Double_t xx2   = x  - x2;
   Double_t x1x2  = x1 - x2;
   Double_t yy1   = y  - y1;
   Double_t yy2   = y  - y2;
   Double_t y1y2  = y1 - y2;
   Double_t a     = xx1*xx1   + yy1*yy1;
   Double_t b     = xx2*xx2   + yy2*yy2;
   Double_t c     = x1x2*x1x2 + y1y2*y1y2;
   if (c <= 0)  return 9999;
   Double_t v     =  TMath::Sqrt(c);
   Double_t u     = (a - b + c)/(2*v);
   Double_t d     = TMath::Sqrt(TMath::Abs(a - u*u));
	// check if below or above
   if ( py < (co + sl*px) ) return  TMath::Sign(d,sl);
   else                     return -TMath::Sign(d,sl);
}
//_______________________________________________________________________________________

TH1F* projectany(TH2* hin , TH1F* hp, Double_t co, Double_t sl)
{
// project 2dim hist hin on an axis given by its normal f(x) = co + sl*x
// if hproj is NULL a default hist is constructed
//
	cout << "const  " << co << " slope " << sl << endl;
	if (TMath::Abs(sl) < 1.e-10) {
		cout << "Slope of normal == 0, use project X instead" << endl;
		return NULL;
	} else if ( TMath::Abs(1./sl) < 1.e-10) {
		cout << "Slope of normal == inf, use project Y instead" << endl;
		return NULL;
	}
	TAxis* xa = hin->GetXaxis();
	Int_t nbinsX = xa->GetNbins();
	TAxis* ya = hin->GetYaxis();
	Int_t nbinsY = ya->GetNbins();
//		Double_t sl = (y2 - y1)/(x2 - x1);
//		Double_t co = y2 - sl * x2;
//		cout << "const  " << co << " slope " << sl << endl;
	Double_t x1, y1, x2, y2;
	x1 = xa->GetXmin();
	x2 = xa->GetXmax();
	y1 = co + sl*x1;
	y2 = co + sl*x2;
	Double_t xmin = 0, xmax = 0, dist;
	TH1F * hproj;
	if (hp == NULL) {
		Int_t nbinsProj= TMath::Sqrt(2)* ((Double_t)nbinsX) +10;
//		Int_t nbinsProj= nbinsX + nbinsY;
		// find min / max in projection
		for (Int_t ix = 1; ix <= nbinsX; ix++) {
			for (Int_t iy = 1; iy <= nbinsY; iy++) {
				Int_t cont = hin->GetBinContent(ix, iy);
				Double_t xc = xa->GetBinCenter(ix);
				Double_t yc = ya->GetBinCenter(iy);
				if ( cont != 0) {
					dist = DtoL(xc, yc, x1, y1, x2, y2);
					if (dist < xmin) xmin = dist;
					if (dist > xmax) xmax = dist;
				}
			}
		}
		cout << "nbinsProj " << nbinsProj  << " xmin " << xmin<< " xmax " << xmax << endl;
		cout << " x1 " << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2 << endl;
		// some margin around
		xmin = xmin - 2 * (xmax - xmin)/nbinsProj;
		xmax = xmax + 2 * (xmax - xmin)/nbinsProj;
		TString title( hin->GetTitle());
		title.Append("_proj_");
		// for title use points on frame of histogram
		Double_t x1d = x1, y1d = y1, x2d = x2, y2d = y2;
		if (y1 < ya->GetXmin() && sl != 0 ) {
			y1d = ya->GetXmin();
			x1d = (y1d - co) / sl;
		}
		if (y1 > ya->GetXmax() && sl != 0 ) {
			y1d = ya->GetXmax();
			x1d = (y1d - co) / sl;
		}
		
		if (y2 < ya->GetXmin() && sl != 0 ) {
			y2d = ya->GetXmin();
			x2d = (y2d - co) / sl;
		}
		if (y2 > ya->GetXmax() && sl != 0 ) {
			y2d = ya->GetXmax();
			x2d = (y2d - co) / sl;
		}
		TString tt(Form("%4.2g_%4.2g_%4.2g_%4.2g", x1d, y1d, x2d, y2d));
//		cout << "tt |" << tt << "|" <<endl;
		TRegexp bl(" ");
		while (tt.Contains(" ")){
			tt(bl)="";
		}
		title.Append(tt);
		for (Int_t ic = 0; ic < tt.Length(); ic++) {
			if (tt(ic) == '.') tt(ic)='d';
			if (tt(ic) == '-') tt(ic)='m';
			if (tt(ic) == '+') tt(ic)='p';
		}
		tt.Prepend("hproj_");
//

		cout << "tt |" << tt << "|" <<endl;
		hproj = new TH1F(tt, title, nbinsProj, xmin, xmax);
	} else {
		hproj = hp;
	}
	// fill result, to minimize binning effect distribute
	// entries randomly in cell
	for (Int_t ix = 1; ix <= nbinsX; ix++) {
		for (Int_t iy = 1; iy <= nbinsY; iy++) {
			Int_t cont = hin->GetBinContent(ix, iy);
			if (cont != 0){
				Double_t xc = xa->GetBinCenter(ix);
				Double_t yc = ya->GetBinCenter(iy);
				Double_t xbw = xa->GetBinWidth(ix);
				Double_t ybw = ya->GetBinWidth(iy);
				for (Int_t i=0;i<cont; i++) {
					Double_t x =xc + xbw * (gRandom->Rndm() - 0.5);
					Double_t y =yc + ybw * (gRandom->Rndm() - 0.5);
					dist = DtoL(x, y, x1, y1, x2, y2);
					hproj->Fill(dist);
				}
			}
		}
	}
	return hproj;
}
//______________________________________________________________________

TCanvas * HT2TC(GrCanvas *htc)
{
	Int_t ww = htc->GetWindowWidth();
	Int_t wh = htc->GetWindowHeight()-26;	// account for decor
	TString cn = htc->GetName();
	// make name differ
	if (cn.BeginsWith("C_")){
		cn = cn(2,100);
	} else {
		cn.Prepend("TC_");
	}
	TString ct = htc->GetTitle();
	TCanvas * tc = new TCanvas(cn, ct, ww,wh);
	tc->Draw();
	TList *lop = tc->GetListOfPrimitives();
	TIter next( htc->GetListOfPrimitives());
	TObject *obj;
	while ( (obj = next()) ){
		lop->Add(obj);
	}
//	tc->Draw();
	return tc;
}
//______________________________________________________________________

TObject *FindObjectByWildcard(const char *s, TList *list)
{
	TIter next(list);
	TString name;
	TObject *obj;
	while ( (obj = next()) ) {
		name = obj->GetName();
		if (name.Contains(s))
			return obj;
	}
	return NULL;
}
//______________________________________________________________________

Int_t PrintList(TList *list)
{
	TIter next(list);
	TObject *obj;
	Int_t nn = 0;
	cout << "Content of list: " << list->GetName() << endl;
	while ( (obj = next()) ) {
		cout << obj->GetName()<< " " << obj->GetTitle() << endl;
		nn++;
	}
	return nn;
}

}   // end namespace Hpr

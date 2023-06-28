#include "TObject.h"
#include "TButton.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "HTCanvas.h"
#include "TFrame.h"
#include "TLatex.h"
#include "TText.h"
#include "TArc.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TGraph2D.h"
#include "TH1.h"
#include "TF1.h"
#include "TRegexp.h"
#include "TKey.h"
#include "TMath.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TGMsgBox.h"
#include "TRootHelpDialog.h"
#include "TVirtualPad.h"
#include "TString.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TList.h"
#include "TRandom.h"
#include "TASImage.h"
#include "CmdListEntry.h"
#include "FitHist.h"
#include "GEdit.h"
#include "SetColor.h"

#include "support.h"
//#include "TMrbString.h"
#include "TGMrbInputDialog.h"
#include "TGMrbValuesAndText.h"
#include "TMrbStatistics.h"
#include "Save2FileDialog.h"
#include "WindowSizeDialog.h"

#include <iostream>
#include <fstream>
#include <sstream>

extern Int_t gHprDebug;
TSocket * gSocket = 0;
//-----------------------------------------------------------------------
//  a command button

int GetBPars(const char *cmd, TObject * /*cc*/, int xpos, int ypos, float dx,
				 float dy, TString *tmp, float *x0, float *x1, float *y0,
				 float *y1)
{
	if (xpos < 1 || xpos > 8 || ypos < 1 || ypos > 8) {
		printf("Illegal button position: %d %d\n", xpos, ypos);
		return 0;
	}
	*tmp = "gHpr";
//	*tmp = cc->GetName();
	(*tmp) += "->";
	(*tmp) += cmd;
	if ( !(*tmp).EndsWith(");"))
		(*tmp) += "();";

	*x0 = (float) ((xpos - 1) * dx);
	*x1 = *x0 + 0.97 * dx;
	*y0 = (float) (1 - ypos * dy);
	*y1 = *y0 + dy;
	return 1;
};

//-----------------------------------------------------------------------
//  a command button

TButton *CButton(const char *cmd, const char *title, TObject * cc,
					  int xpos, int ypos, float dy, float dx)
{
	TString tmp;
	float x0, x1, y0, y1;
	if (GetBPars(cmd, cc, xpos, ypos, dx, dy, &tmp, &x0, &x1, &y0, &y1)) {
		TButton *b = new TButton((char *) title, tmp.Data(), x0, y0, x1, y1);
//		cout << "CButton " << x0 << " " << y0 << " " << x1 << " " << y1 << endl;
		b->SetTextFont(60);
		b->SetTextAlign(22);
		b->Draw();
		return b;
	} else
		return 0;
};

//-----------------------------------------------------------------------
//  a toggle button

TButton *SButton(const char *cmd, const char *title, TObject * cc,
					  int xpos, int ypos, float dy, int onoff, float dx)
{
	TString tmp;
	float x0, x1, y0, y1;
	if (GetBPars(cmd, cc, xpos, ypos, dx, dy, &tmp, &x0, &x1, &y0, &y1)) {
		int col = 2;
		if (onoff)
			col = 3;
		TButton *b = new TButton((char *) title, tmp.Data(), x0, y0, x1, y1);
		b->SetFillColor(col);
		b->Draw();
		return b;
	} else
		return 0;
};

//-----------------------------------------------------------------------
//  a help button

void HButton(const char *cmd, TObject * cc, int xpos, int ypos,
				 float dy, float dx)
{
	TString tmp;
	float x0, x1, y0, y1;
	if (GetBPars(cmd, cc, xpos, ypos, dx, dy, &tmp, &x0, &x1, &y0, &y1)) {
		x0 += dx;
		x1 = x0 + dx * 0.25;
		TButton *b = new TButton((char *) "?", tmp.Data(), x0, y0, x1, y1);
		b->Draw();
	}
};

//-----------------------------------------------------------------------

TLatex *GetPadLatex(TButton * pad)
{
	TIter next(pad->GetListOfPrimitives());
	while (TObject * obj = next()) {
		const char *name = obj->ClassName();
		if (!strncmp(name, "TLatex", 5))
			return (TLatex *) obj;
	}
	return NULL;
}
//-----------------------------------------------------------------------
Bool_t HasFunctions(TH1 * hist)
{
	TIter next(hist->GetListOfFunctions());
	TObject *obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TF1")) return kTRUE;
	}
	return kFALSE;
};
//-----------------------------------------------------------------------

TH1 * GetHistOfGraph(TVirtualPad * pad)
{
	TIter next(pad->GetListOfPrimitives());
	TObject *obj;
	TH1 * hist = NULL;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TGraph")) {
			hist = ((TGraph*)obj)->GetHistogram();
			if ( hist ) 
				return hist;
		}
		if (obj->InheritsFrom("TGraph2D")) {
			hist = ((TGraph2D*)obj)->GetHistogram();
			if ( hist ) 
				return hist;
		}
	}
	return NULL;
};
//-----------------------------------------------------------------------

Int_t GetNofGraphs(TVirtualPad * pad)
{
	TIter next(pad->GetListOfPrimitives());
	TObject *obj;
	Int_t ng = 0;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("TGraph")) {
			ng++;
		}
	}
	return ng;
};

//-----------------------------------------------------------------------
Bool_t is2dim(TH1 * hist)
{
	return hist->InheritsFrom("TH2");
};

//-----------------------------------------------------------------------

Bool_t is3dim(TH1 * hist)
{
	return hist->InheritsFrom("TH3");
};
//____________________________________________________________________________

Bool_t is_a_list(TObject * obj)
{
	return obj->InheritsFrom("TList");
}

//____________________________________________________________________________

Bool_t is_a_function(TObject * obj)
{
	return obj->InheritsFrom("TF1");
}

//____________________________________________________________________________

Bool_t is_a_hist(TObject * obj)
{
	return obj->InheritsFrom("TH1");
}

//------------------------------------------------------
Bool_t is_a_window(TObject * obj)
{
	return (!strncmp(obj->ClassName(), "TMrbWindow", 10));
};

//------------------------------------------------------
Bool_t is_a_cut(TObject * obj)
{
	return (!strncmp(obj->ClassName(), "TCutG", 5));
};
Bool_t is_a_tree(TObject * obj)
{
	return (!strncmp(obj->ClassName(), "TTree", 5));
};

//------------------------------------------------------

Bool_t is_memory(const char *name)
{
	return (!strcmp(name, "Memory"));
}

//------------------------------------------------------

Bool_t is_a_file(const char *name)
{
	if (strstr(name, ".root") != 0)
		return kTRUE;
	else
		return kFALSE;
}

//------------------------------------------------------

Int_t CheckList(TList * list, const char *name)
{
	if (!list)
		return 0;
	Int_t entries, old_entries = -10;
	TObject *obj;
	Int_t lname = strlen(name);
 loop:
	entries = list->GetSize();
	if (entries == old_entries) {
		cout << "Infinite loop: " << entries << endl;
		list->Print();
		return entries;
	}
	if (entries > 0) {
		old_entries = entries;
//		cout << "CheckList: entries: " << entries << endl;
		for (Int_t i = 0; i < entries; i++) {
			obj = list->At(i);
//			cout	 << "at[i] " << i << " obj: " <<  obj << endl;
			if (!obj->TestBit(TObject::kNotDeleted) ||
				 obj->TestBit(0xf0000000)) {
				cout << " remove deleted " << obj << " name: " << name <<
					 " at " << i << " in " << list << endl;
				obj->Dump();
				list->RemoveAt(i);

				goto loop;
			} else {
				if (lname > 0) {
					if (strncmp(obj->ClassName(), name, lname)) {
						cout << "remove wrong Class " << obj->
							 ClassName() << endl;
						list->RemoveAt(i);
						goto loop;
					}
				}
			}
		}
	}
	return entries;
}

//------------------------------------------------------

TButton *CommandButton(TString & cmd, TString & tit,
							  Float_t x0, Float_t y, Float_t x1, Float_t y1,
							  Bool_t selected)
{
	TButton *button = new TButton((const char *) tit, "", x0, y, x1, y1);
	TRegexp brace(")");
	TString newcmd(cmd);
	ostringstream  buf;
	if (cmd.Contains("\""))
		buf << ",";
	if (newcmd.EndsWith("_")) {
		newcmd = newcmd.Chop();
		buf << "\"" << gPad << "\")";
	} else {
		buf << "\"" << button << "\")";
	}
	newcmd(brace) = buf.str();
//	cout << "CommandButton: " << newcmd << endl;
	button->SetBit(kCommand);
	if (selected) {
		button->SetFillColor(3);
		button->SetBit(kSelected);
	} else {
		button->SetFillColor(19);
		button->ResetBit(kSelected);
	}
	button->SetMethod((char *) newcmd.Data());
	button->SetTextAlign(12);
//	button->SetTextFont(101);
	button->SetTextFont(100);
	button->SetTextSize(0.72);
//	if(selected)button->SetFillColor(3);
//	if(gROOT->GetVersionInt() <= 22308){
//		TText *text = GetPadText(button);
//		if(text)text->SetX(0.03);
	//  } else {
	TLatex *text = GetPadLatex(button);
	if (text && tit.Length() > 0)
		text->SetX(0.2 / (Double_t)tit.Length());
//	}
	button->Draw();
	return button;
}

//________________________________________________________________________________

TString *GetTitleString(TObject * obj)
{
	TString *btitle = new TString(obj->GetName());
	*btitle = *btitle + " C: ";
	if (is_a_hist(obj)) {
		char ccont[9];
		TH1 *hist = (TH1 *) obj;
		sprintf(ccont, "%6.0f", hist->GetSumOfWeights());
		*btitle = *btitle + ccont;
	}
	*btitle = *btitle + " T: " + obj->GetTitle();

	if (!strncmp(obj->ClassName(), "TCutG", 5)) {
		btitle->Prepend("Cut:");
	}
	return btitle;
}

//________________________________________________________________________________

void SelectButton(TString & cmd,
						Float_t x0, Float_t y, Float_t x1, Float_t y1,
						Bool_t selected)
{
//	cout << ctitle << endl;
	TButton *button = new TButton("", "", x0, y, x1, y1);
	TRegexp brace(")");
	TString newcmd(cmd);

	ostringstream buf;
	if (cmd.Contains("\""))
		buf << ",";
//	buf += Form("\"%x\")", button);
	buf << "\"" << button << "\")";
	newcmd(brace) = buf.str();

	button->SetMethod((char *) newcmd.Data());
	button->SetBit(kSelection);
//	cout << "SelectButton: " << newcmd << endl;
	if (selected) {
		button->SetFillColor(3);
		button->SetBit(kSelected);
	} else {
		button->SetFillColor(15);
		button->ResetBit(kSelected);
	}
	button->Draw();
}
//______________________________________________________________________________________
void SetSelected(TButton *b, Bool_t sel) 
{
	if ( b ) {
		if ( sel ) {
			b->SetBit(kSelected);
			b->SetFillColor(3);
		} else {
			b->ResetBit(kSelected);
			b->SetFillColor(19);
		}
		b->Modified(kTRUE);
		b->Update();
	}
}
//______________________________________________________________________________________

HTCanvas *CommandPanel(const char *fname, TList * fcmdline,
							  Int_t xpos, Int_t ypos, HistPresent * hpr,
								Int_t xwid, Int_t maxentries, Int_t skipfirst)
{
	Int_t xwid_default = 250;
	Int_t entry_height = WindowSizeDialog::fEntryHeight;
	xwid_default = WindowSizeDialog::fMainWidth;
	Int_t ywid_default = (Int_t)((Float_t)xwid_default * 2.4);
	Float_t magfac = (Float_t)xwid_default / 250.;
//	Int_t xwid_default = 250;
//	Int_t ywid_default = 600;

	Int_t xw = xwid_default;
	Int_t yw = ywid_default;
	if (xwid > 0) xwid_default = xwid;
	Int_t maxlen_tit = 0;
	Int_t maxlen_nam = 0;
	Bool_t anysel = kFALSE;
	Int_t Nentries = fcmdline->GetSize();
	Int_t maxe = maxentries;
	if (maxe <= 0)
		maxe = 1000;
	Int_t first = 0;
	Int_t last = Nentries;
	if (skipfirst > 0 && skipfirst < Nentries-1)
		first = skipfirst;
	if (maxe > 0 && last - first >  maxe)
		last = first + maxe;
	if (last > Nentries)
		last = Nentries;
	Nentries = last - first;
	for (Int_t i = first; i < last; i++) {
		CmdListEntry *cle = (CmdListEntry *) fcmdline->At(i);
		if (cle->fNam.Length() > maxlen_nam)
			maxlen_nam = cle->fNam.Length();
		if (cle->fTit.Length() > maxlen_tit)
			maxlen_tit = cle->fTit.Length();
		if (cle->fSel.Length() > 0)
			anysel = kTRUE;
//		if(cle->fSel != "NoOp") anysel=kTRUE;
	}
//	cout << "xw " << xw << " maxlen_nam " << maxlen_nam << endl;
	if (maxlen_nam < 15) maxlen_nam = 13;
	xw = 40 + Int_t((Float_t)(maxlen_nam) * 9 * magfac) ;

	if (xwid > 0) xw = xwid;

	if (Nentries < 25)
		yw = (Int_t)(magfac * entry_height) * (Nentries + 2);
//		yw = 40 * (Nentries + 1);
//	cout << fname << " xpos " << xpos << " ypos " << ypos << " xw " << xw << " yw " << yw << endl;

	TString pname(fname);
	gStyle->SetCanvasPreferGL(kFALSE);
	HTCanvas *cHCont = new HTCanvas(pname.Data(), pname.Data(),
											  -xpos, ypos, xw, yw, hpr, 0);
//											  xpos, ypos, -xw, yw, hpr, 0);
	Int_t item_height = TMath::Min(Int_t(magfac * entry_height), 10000/Nentries);
//	cout << " Nentries " << Nentries << endl;

	if ( item_height < entry_height ) item_height = entry_height;
	cHCont->SetCanvasSize(xw, item_height * Nentries);

	Float_t expandx = xw / (250. * magfac);
	if(expandx < 1.) expandx=1.;
//	Float_t expandy = (Float_t) Nentries / 25;
	Float_t expandy = (Float_t) Nentries / 20;

	Float_t x1, y0, y, dy;
	Float_t xcmd_with_sel = 0.08 / expandx;
	Float_t xcmd_no_sel	= 0.01 / expandx;
	x1 = 0.99;
	dy = .999 / (Float_t) (Nentries);

//	 cout << "Nentries, dy " << Nentries << " " <<  dy << endl;
	y0 = 1.;
	y = y0 - dy;
	TString sel;
	TString title;
	for (Int_t i = first; i < last; i++) {
		Float_t xcmd = xcmd_no_sel;
		CmdListEntry *cle = (CmdListEntry *) fcmdline->At(i);
//		cout << "new: " << cle->fCmd << endl;
		if (anysel) {
			sel = cle->fSel;
			if (sel != "NoOp" && sel.Length() > 0) {
				Bool_t selected;
				if (sel.Index(" YES") > 0) {
					selected = kTRUE;
					sel.Remove(sel.Index(" YES"), sel.Length());
				} else {
					selected = kFALSE;
				}
				xcmd = xcmd_with_sel;
				SelectButton(sel, xcmd_no_sel, y, xcmd_with_sel, y + dy, selected);
			} else {
				xcmd = xcmd_no_sel;
			}
		}
		title = cle->fNam;
		title.Resize(maxlen_nam + 1);
//		title += cle->fTit;
//		cout << "y, dy " << y << " " << dy << endl;
		CommandButton(cle->fCmd, title, xcmd, y, x1, y + dy, kFALSE);
//		if((cle->fTit).Length()>0)b->SetToolTipText((const char *)cle->fTit,500);

//		b->SetName((const char *)cle->fNam);
		y -= dy;
	}
	Int_t usedxw = cHCont->GetWw();
	Int_t usedyw = cHCont->GetWh();
//	cout << "usedxw  " << usedxw << " usedyw  " << usedyw << endl;

//	Int_t newxw = (Int_t) ((Float_t) usedxw * expandx);
	Int_t newxw = usedxw;
	Int_t newyw;
	if (Nentries > 25)
		newyw = (Int_t) ((Float_t) usedyw * expandy);
	else
		newyw = usedyw;
//  adjust canvas to fit text without scroll bars
	if (xwid < 0 || (xwid == 0 && newxw < usedxw)) {
		newxw = TMath::Max(xwid_default, newxw);
		if (newxw > xwid_default) newxw += 20;
	} else {
		newxw = xwid_default;
	}
	if (gHprDebug > 0) {
		cout << "xwid_default " << xwid_default << "xwid " << xwid
			  << " newxw " << newxw
			  << " usedxw " << usedxw
			  << endl;
		cout << "ywid_default " << ywid_default
			  << " newyw " << newyw
			  << " usedyw " << usedyw
			  << endl;
	}
	if (xwid <= 0 && usedxw < xwid_default) newxw = usedxw + 6;
//  acount for scrollbar
	if (usedxw > newxw )  newyw += 15;

	cHCont->SetWindowSize(newxw, TMath::Min(ywid_default+10, newyw+10));
	cHCont->SetCanvasSize(usedxw-5, usedyw);
	cHCont->SetEditable(kFALSE);
	cHCont->Update();
	return cHCont;
};

//--------------------------------------------------

Int_t XBinNumber(TH1 * hist, Axis_t xval)
{
	TAxis *xaxis = hist->GetXaxis();
	Axis_t xmin = xaxis->GetXmin();
	Axis_t xmax = xaxis->GetXmax();
	Int_t nxbins = hist->GetNbinsX();
	Axis_t xbinwidth = (xmax - xmin) / nxbins;
	Int_t binlx = (Int_t) ((xval - xmin) / xbinwidth + 1.);
	if (binlx < 1)
		binlx = 1;
	if (binlx > nxbins)
		binlx = nxbins;
	return binlx;
}

//--------------------------------------------------
Int_t YBinNumber(TH1 * hist, Axis_t yval)
{
	TAxis *yaxis = hist->GetYaxis();
	Axis_t ymin = yaxis->GetXmin();
	Axis_t ymax = yaxis->GetXmax();
	Int_t nybins = hist->GetNbinsY();
	Axis_t ybinwidth = (ymax - ymin) / nybins;
	Int_t binly = (Int_t) ((yval - ymin) / ybinwidth + 1.);
	if (binly < 1)
		binly = 1;
	if (binly > nybins)
		binly = nybins;
	return binly;
}

//__________________________________________________________________

Stat_t Content(TH1 * hist, Axis_t xlow, Axis_t xup,
					Stat_t * mean, Stat_t * sigma)
{
	if (!hist) {
		cout << "Content: NULL pointer to histogram" << endl;
		return -1;
	}
	Double_t x = 0, cont = 0;
	Double_t sum = 0;
	Double_t sumx = 0;
	Double_t sumx2 = 0;
	Double_t sigi = 0, mi = 0;
	for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
		x = hist->GetBinCenter(i);
		if (x >= xlow && x < xup) {
			cont = hist->GetBinContent(i);
			sum += cont;
			sumx += x * cont;
			sumx2 += x * x * cont;
		}
	}
	if (sum > 0.) {
		mi = sumx / sum;
		sigi = TMath::Sqrt(sumx2 / sum - mi * mi);
	}
	*mean = mi;
	*sigma = sigi;
	return sum;
}

//__________________________________________________________________

void WarnBox(const char *message, TGWindow * win)
{
	int retval = 0;
	new TGMsgBox(gClient->GetRoot(), win,
					 "Warning", message,
					 kMBIconExclamation, kMBDismiss, &retval);
}

//__________________________________________________________________

void InfoBox(const char *message, TGWindow * win)
{
	int retval = 0;
	new TGMsgBox(gClient->GetRoot(), win,
					 "Info", message,
					 kMBIconExclamation, kMBDismiss, &retval);
}

//__________________________________________________________________

Bool_t QuestionBox(const char *message, TGWindow * win)
{
	int retval = 0;
	new TGMsgBox(gClient->GetRoot(), win,
					 "Question", message,
					 kMBIconQuestion, kMBYes | kMBNo, &retval);
	if (retval == kMBNo)
		return kFALSE;
	else
		return kTRUE;
}
//------------------------------------------------------

Int_t GetObjects(TList & list, TDirectory * rfile, const char * classname)
{
	Int_t maxkey = 0;
	if (!rfile)
		return maxkey;
	TIter next(rfile->GetListOfKeys());
//	TList *list = 0;
	TKey *key;
	TString cnin(classname);
	TString cn;
	while ( (key = (TKey *) next()) ) {
		cn = key->GetClassName();
//		cout << "GetObjects: " << cn << endl;
		if (cn == cnin) {
			TString kn(key->GetName());
			kn += ";";
			kn += key->GetCycle();
			maxkey = TMath::Max(maxkey, (Int_t)key->GetCycle());
			list.Add(new TObjString(kn.Data()));
		}
	}
	return maxkey;
}

//------------------------------------------------------
Int_t contains_filenames(const char *lname)
{
	if (gSystem->AccessPathName(lname)) {
		cout << "Cant find: " << lname << endl;
		return -1;
	}
	ifstream wstream;
	wstream.open(lname, ios::in);
	if (!wstream.good()) {
		cout << "Error in open file: "
			 << gSystem->GetError() << " - " << lname << endl;
		return -1;
	}
	TString line;
	Int_t ok = 0;
	while (1) {
		line.ReadLine(wstream, kFALSE);
		if (wstream.eof())
			break;
		line.Strip();
		if (line.Length() <= 0)
			continue;
		if (line[0] == '#')
			continue;
		Int_t pp = line.Index(",");
		if (pp < 0) {
			pp = line.Index(" ");
			if (pp <= 0)
			break;
		}
		line.Resize(pp);
		if (line.Contains(".root"))
			ok = 1;
		break;
	}
	wstream.close();
	return ok;
}

//_______________________________________________________________________________________

TH1 *GetTheHist(TVirtualPad * pad)
{
	TList *l = pad->GetListOfPrimitives();
	TIter next(l);
	TObject *o;
	while ( (o = next()) ) {
		if (is_a_hist(o))
			return (TH1 *) o;
	}
	return NULL;
}
//_______________________________________________________________________________________

void Canvas2LP(TCanvas * ca, Bool_t to_printer, TGWindow * win)
{
// if opt contains plain: force white background
//						  ps: write to PostScript file
	const char helpText_PS[] =
"Write picture to PostScript file or send\n\
directly to a printer. With Option\"Force white background\"\n\
background of pads will be white. This is\n\
recommended to save toner of printers.\n\
\n\
Recipe to fill in forms (pdf):\n\
Convert pdf -> png, find out size of png : Xw x Yw\n\
file form.png\n\
In Histpresent \"Open Edit Canvas A4\" use: \n\
	Xwidth = Xw+4, Ywidth = Yw+28, X range = 210 mm\n\
\n\
In \"Picture to PS-file\" use:\n\
Page size X: 21, Y 29.7, Shift X: -.8 Shift Y: -.1 cm \n\
\n\
Fine tuning of picture size and position on paper\n\
may be done by setting Scale and shift values.\n\
Shift is done prior to scale, so if scale is 0.5\n\
a shift value of 10 will only shift by 5 cm";

	if (!ca) return;
	Int_t *padfs = 0;
	Int_t *padframefs = 0;
	Int_t cafs = 0, caframefs = 0;
	Int_t npads = 0;
	const Int_t maxpads = 1000;

	const char hist_file[] = {"printer_hist.txt"};
	Bool_t ok;
	Int_t itemwidth = 320;
//	ofstream hfile(hist_file);
	static Int_t plain = 0;
	static Double_t xpaper;  // A4 and letter
	static Double_t ypaper;  // A4 24, letter 26
	static Double_t scale;
	static Double_t xshift;
	static Double_t yshift;
	static Int_t	 view_ps;
	static Int_t	 remove_picture = 0;

	TEnv env(".hprrc");
	xpaper = env.GetValue("HistPresent.PaperSizeX", 20.);
	ypaper = env.GetValue("HistPresent.PaperSizeY", 26.);
	xshift = env.GetValue("HistPresent.PrintShiftX", 0.);
	yshift = env.GetValue("HistPresent.PrintShiftY", 0.);
	scale  = env.GetValue("HistPresent.PrintScale",  1.);
	view_ps= env.GetValue("HistPresent.AutoShowPSFile", 0);
	plain  = env.GetValue("HistPresent.PlainStyle", 1);

	TList *row_lab = new TList();
	static void *valp[25];
	Int_t ind = 0;
	row_lab->Add(new TObjString("CheckButton_Force white background"));
	row_lab->Add(new TObjString("DoubleValue_Page size X [cm]"));
	row_lab->Add(new TObjString("DoubleValue_Page size Y [cm]"));
	row_lab->Add(new TObjString("DoubleValue_Apply scale factor"));
	row_lab->Add(new TObjString("DoubleValue_Apply X shift[cm]"));
	row_lab->Add(new TObjString("DoubleValue_Apply Y shift[cm]"));
	row_lab->Add(new TObjString("CheckButton_Remove underlaid picture"));
	row_lab->Add(new TObjString("CheckButton_View ps file automatically"));

	valp[ind++] = &plain;
	valp[ind++] = &xpaper;
	valp[ind++] = &ypaper;
	valp[ind++] = &scale;
	valp[ind++] = &xshift;
	valp[ind++] = &yshift;
	valp[ind++] = &remove_picture;
	valp[ind++] = &view_ps;

	static TString cmd_name;
	TString prompt;
	if (to_printer) {
		prompt = "Printer command";
		cmd_name = "lpr ";
		const char *pc = gSystem->Getenv("PRINTER");
		if (pc) {
			cmd_name += "-P";
			cmd_name += pc;
		}
		const char *ccmd =
			 env.GetValue("HistPresent.PrintCommand", cmd_name.Data());
		cmd_name = ccmd;
	} else {
		cmd_name = ca->GetName();
		Int_t last_sem = cmd_name.Last(';');	 // chop off version
		if (last_sem >0) cmd_name.Remove(last_sem);
		cmd_name += ".ps";
		prompt = "PS file name";
	}
	ok = GetStringExt(prompt.Data(), &cmd_name, itemwidth, win,
						 hist_file, NULL, row_lab, valp,
						 NULL, NULL, helpText_PS);
	if (!ok) {
		cout << "Printing canceled" << endl;
		return;
	}

	if (remove_picture) {
		TList *l = ca->GetListOfPrimitives();
		TIter next(l);
		TObject *obj;
		while (( obj = next()) ) {
			if (obj->InheritsFrom("TASImage")) {
				delete obj;
			}
		}
	}
	if (plain) {
		TList *l = ca->GetListOfPrimitives();
		TIter next(l);
		TObject *obj;
		padfs = new Int_t[maxpads];
		padframefs = new Int_t[maxpads];
		while (( obj = next()) ) {
			if (obj->InheritsFrom(TPad::Class())) {
				TPad *p = (TPad *) obj;
				if (npads < maxpads) {
					padfs[npads] = p->GetFillStyle();
					padframefs[npads] = p->GetFrame()->GetFillStyle();
					npads++;
				}
				p->SetFillStyle(0);
				p->GetFrame()->SetFillStyle(0);
			}
		}
		cafs = ca->GetFillStyle();
		caframefs = ca->GetFrame()->GetFillStyle();
		ca->SetFillStyle(0);
		ca->GetFrame()->SetFillStyle(0);
		ca->SetBorderMode(0);
	}
//	cout << "xshift "  << xshift<< " yshift " << yshift << endl;
	gStyle->SetPaperSize((Float_t) xpaper, (Float_t) ypaper);
	if (scale != 1 || xshift != 0 || yshift != 0) {
		TString extra_ps;
		extra_ps = "";
		if (xshift != 0 || yshift != 0) {
//	 convert to points (* 4 to compensate for ROOTs .25)
			Int_t xp = (Int_t) (xshift * 4 * 72. / 2.54);
			Int_t yp = (Int_t) (yshift * 4 * 72. / 2.54);
			extra_ps += Form("%d %d  translate ", xp, yp);
		}
		if (scale != 1)extra_ps += Form("%f %f scale ", scale, scale);
		gStyle->SetHeaderPS(extra_ps.Data());
	}
	TString fname;
	if (to_printer) {
		fname = "/tmp/temp_pict_";
		fname +=gSystem->Getenv("USER");
		fname += ".ps";
	} else {
		fname = cmd_name;
	}
	ca->SaveAs(fname.Data());
	if (to_printer) {
		TString prtcmd(cmd_name.Data());
		prtcmd += " ";
		prtcmd += fname.Data();
		cout << "Execute: " << prtcmd << endl;
		gSystem->Exec(prtcmd.Data());
	} else {
		cout << "Store files as: " << fname << endl;
	}
	if (view_ps) {
		char * gvcmd = gSystem->Which("/usr/bin:/usr/local/bin", "gv");
		if (gvcmd == NULL)gvcmd = gSystem->Which("/usr/bin:/usr/local/bin", "ggv");
		if (gvcmd != NULL) {
			TString cmd(gvcmd);
			cmd += " ";
			cmd += fname.Data();
			cmd += "&";
			gSystem->Exec(cmd.Data());
			delete gvcmd;
		}
	}
// restore backgrounds
	if (plain) {
		TList *l = ca->GetListOfPrimitives();
		TIter next(l);
		TObject *obj;
		while ( (obj = next()) ) {
			if (obj->InheritsFrom(TPad::Class())) {
				TPad *p = (TPad *) obj;
				if (npads < maxpads) {
					p->SetFillStyle(padfs[npads]);

					p->GetFrame()->SetFillStyle(padframefs[npads]);
					npads++;
				}
			}
		}
		ca->SetFillStyle(cafs);
		ca->GetFrame()->SetFillStyle(caframefs);
		ca->Modified(kTRUE);
		ca->Update();
		delete[]padfs;
		delete[]padframefs;
	}
	env.SetValue("HistPresent.PaperSizeX", xpaper);
	env.SetValue("HistPresent.PaperSizeY", ypaper);
	env.SetValue("HistPresent.PrintShiftX", xshift);
	env.SetValue("HistPresent.PrintShiftY", yshift);
	env.SetValue("HistPresent.PrintScale", scale);
	env.SetValue("HistPresent.AutoShowPSFile", view_ps);
	env.SetValue("HistPresent.PlainStyle", plain);
	env.SaveLevel(kEnvLocal);
}


//_________________________________________________________________________________________

TEnv *GetDefaults(TString & hname, Bool_t mustexist, Bool_t renew)
{
	return GetDefaults(hname.Data(), mustexist,renew);
}

//_________________________________________________________________________________________

TEnv *GetDefaults(const char * hname, Bool_t mustexist, Bool_t renew)
{
	TEnv *lastset = 0;
	TString defname("default/Last");
	TEnv env(".hprrc");			// inspect ROOT's environment
	defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
//	cout << "Got : " << defname.Data() << endl;
	if (defname.Length() > 0) {
		defname += "_";
		defname += hname;
		Int_t ip = defname.Index(";");
		if (ip > 0) defname.Resize(ip);
		defname += ".def";
//		cout << "Look for : " << defname.Data() << endl;
		if (mustexist && gSystem->AccessPathName(defname.Data()))
			return 0;
		if (renew && ! gSystem->AccessPathName(defname.Data())) {
			TString cmd(defname.Data());
			cmd.Prepend("rm ");
			gSystem->Exec(cmd.Data());
//			cout << "Removing: " << cmd.Data() << endl;
		}
//		cout << "GetDefaults: Looked for : " << defname.Data() << endl;
		lastset = new TEnv(defname.Data());
	}
	return lastset;
}

//___________________________________________________________________________________

TH1 *gethist(const char *hname, TSocket * sock)
{
	if (!sock) return 0;
	Int_t nobs;
	TH1 * hist = 0;
	TMessage *message;
	TString mess("M_client gethist ");
	mess += hname;
	sock->Send(mess);				// send message

	while ((nobs = sock->Recv(message)))	// receive next message
	{
		if (nobs <= 0)
			break;
//	  cout << "nobs " << nobs << endl;
		if (message->What() == kMESS_STRING) {
			char *str0 = new char[nobs];
			message->ReadString(str0, nobs);
			cout << str0 << endl;
			delete str0;
			break;
		} else if (message->What() == kMESS_OBJECT) {

			hist = (TH1 *) message->ReadObject(message->GetClass());
			break;
		} else {
			cout << "Unknown message type" << endl;
		}
	}
	if (message) delete message;
	gDirectory = gROOT;
	return hist;
}

//_______________________________________________________________________________

TMrbStatistics *getstat(TSocket * sock)
{
	if (!sock) return 0;
	Int_t nobs;
	TMrbStatistics *stat = 0;
	TMessage *message;
	TString mess("M_client getstat ");

	sock->Send(mess);				// send message

	while ((nobs = sock->Recv(message)))	// receive next message
	{
		if (nobs <= 0)
			break;
//	  cout << "nobs " << nobs << endl;
		if (message->What() == kMESS_STRING) {
			char *str0 = new char[nobs];
			message->ReadString(str0, nobs);
			cout << str0 << endl;
			delete str0;
			break;
		} else if (message->What() == kMESS_OBJECT) {
			stat =
				 (TMrbStatistics *) message->ReadObject(message->GetClass());
			break;
		} else {
			cout << "Unknown message type" << endl;
		}
	}
	if (message) delete message;
	gDirectory = gROOT;
	return stat;
}
//___________________________________________________________________________
void AdjustMaximum(TH1 * h2, TArrayD * xyvals)
{
	Int_t nv = xyvals->GetSize();
	if (nv <= 0) return;
	Double_t highest_cont = (*xyvals)[0];
	for (Int_t i = 0; i < nv; i++) {
		if ((*xyvals)[i] > highest_cont) highest_cont = (*xyvals)[i] ;
	}
	if (highest_cont > h2->GetMaximum()) {
		h2->SetMaximum(highest_cont);
		cout << "SetMaximum to highest contour level: " << highest_cont << endl;
	}
}
//___________________________________________________________________________

Int_t DeleteOnFile(const char * fname, TList* list, TGWindow * win)
{
	Int_t ndeleted = 0;
	TString name, fn, entry;
	TIter next(list);
	TObjString * sel;
	TRegexp space(" +");
	
	while ( (sel = (TObjString*)next()) ) {
		name = sel->GetString();
		if (gHprDebug > 0)
		cout << "DeleteOnFile " << endl << "fname: " << fname << endl<< "entry: " << name << endl;
		if (name.BeginsWith(fname)) {
			Ssiz_t pos = 0,slen;
			Bool_t ok;
			while ( name.Contains(" ") ) {
				name(space) = ",";
			}
			
			const Char_t * del = ",";
			ok = name.Tokenize(fn,pos, del);
			if ( ok ) {
				slen = fn.Length();
				name = name(slen+1, name.Length());
				cout << "DeleteOnFile fn: " << fn << endl;
			} else {
				cout << "Cant get file name from: " << name << endl;
				return -1;
			}
			pos = 0;
			if ( name.Tokenize(entry,pos, del) ) {
				cout << "DeleteOnFile entry: " << entry << endl;
			} else {
				cout << "Cant get entry from: " << fn << endl;
				return -1;
			}
			if (entry.Length() < 1) {
				cout << "Cant get entry from: " << fn << endl;
				return -1;
			}
//			Int_t ip = name.Index(";");
//			if (ip > 0) name.Resize(ip);
			// is there a FitHist object with this name
			TString fhn(entry.Data());
			fhn.Prepend("F");
			if ( gROOT->FindObject(fhn) ) {
				cout << "A canvas containing this object: "
				<< entry << " seems on screen" << endl;
				continue;
			}

			TString question("Delete: ");
			question += entry;
			question += " from ";
			question += fn;
			if (QuestionBox(question.Data(), win) == kMBYes) {
				if ( !strncmp(fname, "Memory", 7) ) {
					TObject * obj = gROOT->FindObject(entry.Data());
					if ( obj ) {
						list->Remove(sel);
						delete obj;
						ndeleted++;
					}
				} else {
					TFile * f = new TFile(fname, "update");
					if (f->Get(entry.Data())) {
						list->Remove(sel);
	//					Int_t cycle = f->GetKey(name.Data())->GetCycle();
	//					if (cycle > 0) {
	//						name += ";";
	//						name += cycle;
	//					}
						cout << "Deleting: " << name.Data() << endl;
						f->Delete(entry.Data());
						ndeleted++;
					} else {
						cout << entry.Data() << " not found on " << fname << endl;
					}
				f->Close();
				}
			}
		}
	}
	return ndeleted;
}
//_______________________________________________________________________________________

void PrintGraph(TGraphErrors * gr)
{

	cout << endl << "Graph Object, Npoints: " << gr->GetN()<< endl;
	gr->Print();
/*
	cout << "			  X" << "	 Error(X)"
		  << "			  Y" << "	 Error(Y)" << endl;
	for (Int_t i = 0; i < gr->GetN(); i++) {
		cout << setw(12) << (gr->GetX())[i] << setw(12) << (gr->GetEX())[i]
			  << setw(12) << (gr->GetY())[i] << setw(12) << (gr->GetEY())[i]
			  << endl;
	}
*/
	TIter next(gr->GetListOfFunctions());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TF1")) {
			TF1 * f = (TF1*)obj;
			cout << endl << "Fitted function name: " << f->GetName()
				  << " Type: " << f->GetTitle()
				  << " Chi2/NDF: " << f->GetChisquare()
				  << "/" << f->GetNDF() << endl << endl;

			for (Int_t i = 0; i < f->GetNpar(); i++) {
				cout << "Par_" << i << setw(12) << f->GetParameter(i)
					  << " +- " << setw(12) << f->GetParError(i)  << endl;
			}
			cout << endl;
		}
	}
}
//_______________________________________________________________________________________

Bool_t IsInsideFrame(TCanvas * c, Int_t px, Int_t py)
{
//	Bool_t inside = kFALSE;
	TIter next(c->GetListOfPrimitives());
	TObject * obj =0;
	Axis_t x = gPad->AbsPixeltoX(px);
	Axis_t y = gPad->AbsPixeltoY(py);
	while ((obj = next())) {
		if (obj->IsA() == TFrame::Class()) {
			TFrame * fr = (TFrame*)obj;
			if (x < fr->GetX1())return kFALSE;
			if (x > fr->GetX2())return kFALSE;
			if (y < fr->GetY1())return kFALSE;
			if (y > fr->GetY2())return kFALSE;
			return kTRUE;
		}
	}
	return kFALSE;
}
//_______________________________________________________________________________________
TGraph * FindGraph(TVirtualPad * ca, Int_t *ngr)
{
	if (!ca) return NULL;
	TGraph * gr = NULL;
	Int_t ng = 0;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph")) {
			 gr = (TGraph*)obj;
			 ng++;
		}
	}
// look for subpads
/*	TIter next1(ca->GetListOfPrimitives());
	while (TObject * obj = next1()) {
		if (obj->InheritsFrom("TPad")) {
			 TPad * p = (TPad*)obj;
			 TIter next2(p->GetListOfPrimitives());
			 while (TObject * obj = next2()) {
				 if (obj->InheritsFrom("TGraph")) {
					 ngr++;
					 if (logr) logr->Add(obj);
					 if (pads) pads->Add(p);
				 }
			 }
		}
	}*/
	if (ngr) *ngr=ng;
	return gr;
};
//_______________________________________________________________________________________

Int_t FindGraphs(TVirtualPad * ca, TList * logr, TList * pads)
{
	if (!ca) return -1;
	Int_t ngr = 0;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph")) {
			ngr++;
			if (logr) logr->Add(obj);
			if (pads) pads->Add(ca);
		}
	}
	// look for subpads
	TIter next1(ca->GetListOfPrimitives());
	while (TObject * obj1 = next1()) {
		if (obj1->InheritsFrom("TPad")) {
			TPad * p = (TPad*)obj1;
			TIter next2(p->GetListOfPrimitives());
			while (TObject * obj = next2()) {
				if (obj->InheritsFrom("TGraph")) {
					ngr++;
					if (logr) logr->Add(obj);
					if (pads) pads->Add(p);
				}
			}
		}
	}
	return ngr;
};
//_______________________________________________________________________________________
Int_t FindObjs(TVirtualPad * ca, TList * logr, TList * pads, const char * oname)
{
	if (!ca) return -1;
	Int_t ngr = 0;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom(oname)) {
			 ngr++;
			 if (logr) logr->Add(obj);
			 if (pads) pads->Add(ca);
		}
	}
// look for subpads
	TIter next1(ca->GetListOfPrimitives());
	while (TObject * obj1 = next1()) {
		if (obj1->InheritsFrom("TPad")) {
			 TPad * p = (TPad*)obj1;
			 TIter next2(p->GetListOfPrimitives());
			 while (TObject * obj2 = next2()) {
				 if (obj2->InheritsFrom(oname)) {
					 ngr++;
					 if (logr) logr->Add(obj2);
					 if (pads) pads->Add(p);
				 }
			 }
		}
	}
	return ngr;
};

//_______________________________________________________________________________________
Int_t FindPaveStats(TVirtualPad * ca, TList * lops, TList * pads)
{
	if (!ca) return -1;
	Int_t nps = 0;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TPaveStats")) {
			 nps++;
			 if (lops) lops->Add(obj);
			 if (pads) pads->Add(ca);
		}
	}
// look for subpads
	TIter next1(ca->GetListOfPrimitives());
	while (TObject * obj1 = next1()) {
		if (obj1->InheritsFrom("TPad")) {
			 TPad * p = (TPad*)obj1;
			 TIter next2(p->GetListOfPrimitives());
			 while (TObject * obj2 = next2()) {
				 if (obj2->InheritsFrom("TPaveStat")) {
					 nps++;
					 if (lops) lops->Add(obj2);
					 if (pads) pads->Add(p);
				 }
			 }
		}
	}
	return nps;
};
//__________________________________________________________________

void Show_Fonts()
{
	const char text[] = "The big brown fox jumps over the lazy dog.";
	gStyle->SetCanvasPreferGL(kFALSE);
	HTCanvas * cc = new HTCanvas("ct", "Text fonts used by root", 100, 100, 600, 600);
	Float_t x0 = 0.05, y = 0.9, dy = 0.05;
	TText *t1;
	TText *t;
	for (Int_t i = 10; i <= 120; i += 10) {
		t1 = new TText(x0, y, Form("%d", i));
		t = new TText(x0 + 0.1, y, text);
		t1->SetTextSize(0.035);
		t1->SetTextFont(i);
		t->SetTextSize(0.035);
		t->SetTextFont(i);
		t->SetNDC();
 		t1->SetNDC();
		t1->Draw();
		t->Draw();
		y -= dy;
	}
	t = new TText(0.45, y, "TextAlign");
	t->SetTextSize(0.04);
	t->SetTextAlign(22);
	t->SetTextFont(60);
	t->SetNDC();
	t->Draw();

	TPad * pp = new TPad("apad", "apad", 0.3, 0.01, 0.6 ,0.25);
	pp->SetLineWidth(2);
	pp->SetFillColor(11);
	pp->SetFillStyle(1001);
	pp->Draw();
	pp->cd();

	t = new TText(0.05, 0.05, "11"); t->SetTextAlign(11);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.05, 0.5, "12");  t->SetTextAlign(12);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.05, 0.95, "13"); t->SetTextAlign(13);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();

	t = new TText(0.5, 0.05, "21");	t->SetTextAlign(21);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.5, 0.5, "22");	t->SetTextAlign(22);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.5, 0.95, "23");	t->SetTextAlign(23);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();

	t = new TText(0.95, 0.05, "31");	t->SetTextAlign(31);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.95, 0.5, "32");	t->SetTextAlign(32);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	t = new TText(0.95, 0.95, "33");	t->SetTextAlign(33);t->SetNDC();
	t->SetTextSize(0.2); t->SetTextFont(60); t->Draw();
	cc->Modified();
	cc->Update();
};

//______________________________________________________________________________________

void DrawColors()
{
//	TString hexcol;
	TString scol;
//	TString cmd;
	new TCanvas("rgb_colors", "Predefined RGB colors", -400, 20, 800, 400);
	Float_t dx = 1./10.2 , dy = 1./10.2 ;
	Float_t x0 = 0.1 * dx,  y0 =0.1 *  dy;
	Float_t x = x0, y = y0;;
	TButton * b;
	TColor  * c;
	Int_t maxcolors = 100;
	Int_t basecolor = 1;
	Int_t colindex = basecolor;
//	Int_t palette = new Int_t[maxcolors];
	for (Int_t i=basecolor; i<= maxcolors; i++) {
		scol = Form("%d", colindex);
		b = new TButton(scol.Data(), scol.Data(), x, y, x + .9*dx , y + .9*dy );
		b->SetFillColor(colindex);
		b->SetTextAlign(22);
		b->SetTextFont(100);
		b->SetTextSize(0.8);
		c = gROOT->GetColor(colindex);
		if (c) {
			if ( c->GetRed() + c->GetBlue() + c->GetGreen() < 1.5 ) b->SetTextColor(0);
			else						 b->SetTextColor(1);
		} else {
			cout << "color not found " << colindex << endl;
		}
		if ( (colindex++ >= maxcolors + basecolor) ) {
			cout << "Too many colors " << maxcolors << endl;
			break;
		}
		b->Draw();
		y += dy;
		if ( y >= 1 - dy ){
			y = y0;
			x+= dx;
		}
	}

	new TCanvas("hue_colors", "Hue values", -400,450, 400, 400);
	TArc * a;
	TColor * col;
	Int_t hue;
	Double_t x1, y1, radius, phi1, phi2;
	Float_t red,green,blue;
	x1= 0.5;
	y1= 0.5;
	radius = 0.4;
	Int_t ind =1001;
	for (hue = 1; hue < 360; hue += 2) {
		phi1 = hue - 1;
		phi2 = hue + 1;
		TColor::HLS2RGB((Float_t)hue, 0.5, 1, red, green, blue);

		col = gROOT->GetColor(ind);
		if (col) delete col;
		col = new TColor(ind, red,green,blue);
		a = new TArc(x1, y1, radius, phi1, phi2);
 //	  col->Print();
		a->SetFillColor(ind);
		a->SetLineColor(0);
		a->SetLineWidth(0);
		ind++;
		a->Draw();
	}
	 a = new TArc(x1, y1, radius / 3);
	 a->SetFillColor(10);
	 a->SetFillStyle(1001);
	 a->Draw();
	 TText * tt = new TText();
	 tt->SetTextAlign(22);
	 tt->DrawText(x1 + 0.45, y1,  "0");
	 tt->DrawText(x1 , y1 + 0.43, "90");
	 tt->DrawText(x1 - 0.45 , y1 , "180");
	 tt->DrawText(x1 , y1 - 0.43, "270");
}
//______________________________________________________________________________________

void DrawFillStyles()
{
	Int_t styles[30];
	styles[0] = 0;
	styles[1] = 1001;
	styles[2] = 2001;
	styles[3] = 4000;
	for (Int_t i=1; i<= 25; i++) styles[3 + i] = i +3000;

	TString scol;
//	TString cmd;
	new TCanvas("fillstyles", "fillstyles", 400, 20, 800, 400);
	Float_t dx = 1./7.2 , dy = 1./5.2 ;
	Float_t x0 = 0.1 * dx,  y0 = 1 - 1.1 *  dy;
	Float_t x = x0, y = y0;
	TPaveText * b;
//	Int_t palette = new Int_t[maxcolors];

	for (Int_t i=0; i< 29 ; i++) {
		scol = Form("%d", styles[i]);
		if		(i==0) scol = Form("%d \n hollow", styles[i]);
		else if (i==1) scol += "\n solid";

		b = new TPaveText(x, y, x + .9*dx , y + .9*dy );
		b->AddText(Form("%d", styles[i]));
		if (i==0) {
			b->AddText("hollow");
			b->SetFillColor(0);
		} else if (i==1) {
			 b->AddText("solid");
			 b->SetFillColor(1);
			 b->SetTextColor(10);
		} else if (i==2) {
			 b->AddText("hatch");
			 b->SetFillColor(1);
			 b->SetTextColor(10);
		} else if (i==3) {
			b->SetFillColor(0);
			b->AddText("trans-");
			b->AddText("parent");
		} else {
			 b->SetTextColor(1);
			 b->SetFillColor(1);
		}
		b->SetFillStyle(styles[i]);
		b->SetTextAlign(22);
		b->SetTextFont(100);
		b->SetTextSize(0.06);
		b->Draw();
		x += dx;
		if ( x >= 1 - dx ){
			y -= dy;
			x = x0;
		}
	}
}
//______________________________________________________________________________________

void DrawLineStyles()
{
	Int_t nstyles = 4;
	new TCanvas("linestyles", "linestyles", 400, 20, 500, 300);
	Float_t dy = 1./ (nstyles + 2) ;
	Float_t y = 1 - 1.1 *  dy;
	Float_t ts = 0.1;
	TLine * l;
	TText * t;
	for (Int_t i = 1; i <= nstyles; i++) {
		t = new TText(0.1, y - 0.3 * ts , Form("%d", i));
		l = new TLine(0.15, y, 0.8, y);
		l->SetLineStyle(i);
		l->SetLineWidth(2);
		t->SetTextSize(ts);
		t->Draw();
		l->Draw();
		y -= dy;
	}
}
//______________________________________________________________________________________

Bool_t CreateDefaultsDir(TRootCanvas * /*mycanvas*/, Bool_t checkonly)
{
	TString defname("default/Last");
	Bool_t fok = kFALSE;
	TEnv env(".hprrc");			// inspect ROOT's environment
	defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
	if (env.Lookup("HistPresent.LastSettingsName") == NULL) {
		cout << "Setting defaults dir/name to: " << defname.Data() << endl;
		env.SetValue("HistPresent.LastSettingsName", defname.Data());
		env.SaveLevel(kEnvLocal);
	} else {
		defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());
	}
	fok = kTRUE;
	Int_t lslash = defname.Last('/');
	if (lslash > 0) {
		TString dirname = defname;
		dirname.Remove(lslash, 100);
		if (gSystem->AccessPathName(dirname.Data())) {
			if (checkonly) return kFALSE;
				if (gSystem->MakeDirectory((const char *) dirname) == 0) {
					fok = kTRUE;
				} else {
					dirname.Prepend("Error creating ");
					dirname.Append(": ");
					dirname.Append(gSystem->GetError());
					cout << dirname.Data() << endl;
//					WarnBox(dirname.Data());
				}
//			}
		} else {
			return kTRUE;
		}
	}
	return fok;
}
//______________________________________________________________________________________

Bool_t fixnames(TFile * * infile, Bool_t checkonly)
{
	TFile * outfile = 0;
	TString outfile_n((*infile)->GetName());
	if (!checkonly) {
		if (outfile_n.EndsWith(".root")) outfile_n.Resize(outfile_n.Length()-5);
		else									  cout << "Warning: " << outfile_n
												<< " doesnt end with .root" << endl;
		outfile_n += "_cor.root";
		outfile = new TFile(outfile_n, "RECREATE");
	}
	TIter next((*infile)->GetListOfKeys());
	TKey* key;
	TNamed * obj;
	TString name;
	Bool_t needsfix = kFALSE;
//	cout << "enter fixnames" << endl;
	TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
	while( (key = (TKey*)next()) ){
		(*infile)->cd();
		obj = (TNamed*)key->ReadObj();
		name = obj->GetName();
		Bool_t changed = kFALSE;
		Int_t isem = name.Index(";");
		if (isem > 0) {
			name.Resize(isem);
			changed = kTRUE;
		}
		while (name.Index(notascii) >= 0) {
			name(notascii) = "_";
			changed = kTRUE;
		}
		if (changed) {
			if (checkonly) {
				cout << "Orig name: " << obj->GetName() << endl;
				cout << "New  name: " << name << endl;
			}
			needsfix = kTRUE;
			obj->SetName(name);
		}
		if (!checkonly) {
			outfile->cd();
			obj->Write();
		}
	}
	if (outfile) {
//		cout << "new file ++++++++++++++++++++++++++++++++++++++++++++=" <<endl;
 //	  outfile->ls();
		outfile->Close();
		if (needsfix) {
//			 cout << "new file: " <<outfile_n<<endl;
			(*infile)->Close();
			*infile = new TFile(outfile_n, "READ");
//			(*infile)->ls();
		}
	}
	return needsfix;
}
//______________________________________________________________________________________

TGraph * PaintArea (TH1 *h, Int_t binl, Int_t binh, Int_t color)
{
	Int_t nbins = binh - binl + 1;
	cout << "PaintArea " << binl << " " << binh << endl;
	TGraph * pl  = new TGraph(2 * nbins + 3);
	Int_t ip = 0;
	for (Int_t bin = binl; bin <= binh; bin++) {
		pl->SetPoint(ip, h->GetBinLowEdge(bin), h->GetBinContent(bin));
		pl->SetPoint(ip + 1 , h->GetBinLowEdge(bin) + h->GetBinWidth(bin),
									 h->GetBinContent(bin));
		ip += 2;
	}
	Axis_t ymin = h->GetYaxis()->GetXmin();
	pl->SetPoint(ip, (pl->GetX())[ip-1], ymin);
	pl->SetPoint(ip+1, (pl->GetX())[0],  ymin);
	pl->SetPoint(ip+2, (pl->GetX())[0],  (pl->GetY())[1]);
	pl->SetFillColor(color);
	pl->SetFillStyle(1001);
	return pl;
}
//______________________________________________________________________________________

Int_t getcol()
{
//	TString hexcol;
	TNamed * colobj = new TNamed("colobj", "colobj");
	gROOT->GetListOfSpecials()->Add(colobj);
	colobj->SetUniqueID(0);
	TString scol;
	TString cmd;
	TCanvas * ccol = new TCanvas("colcol", "rgb colors", 400, 20, 800, 400);
	Float_t dx = 1./10.2 , dy = 1./10.2 ;
	Float_t x0 = 0.1 * dx,  y0 =0.1 *  dy;
	Float_t x = x0, y = y0;;
	TButton * b;
	TColor  * c;
	Int_t maxcolors = 100;
	Int_t basecolor = 1;
	Int_t colindex = basecolor;
//	Int_t palette = new Int_t[maxcolors];
	for (Int_t i=basecolor; i<= maxcolors; i++) {
		scol = Form("%d", colindex);
		cmd = "gROOT->GetListOfSpecials()->FindObject(\"colobj\")->SetUniqueID(";
		cmd += colindex;
		cmd += ");";
		b = new TButton(scol.Data(), cmd.Data(), x, y, x + .9*dx , y + .9*dy );
		b->SetFillColor(colindex);
		b->SetTextAlign(22);
		b->SetTextFont(100);
		b->SetTextSize(0.8);
		c = gROOT->GetColor(colindex);
		if (c) {
		  if ( c->GetRed() + c->GetBlue() + c->GetGreen() < 1.5 ) b->SetTextColor(0);
			else						 b->SetTextColor(1);
		} else {
			cout << "color not found " << colindex << endl;
		}
//		if (colindex == 1) b->SetTextColor(10);
//		else					b->SetTextColor(1);
		if ( (colindex++ >= maxcolors + basecolor) ) {
			cout << "Too many colors " << maxcolors << endl;
			break;
		}
		b->Draw();
		y += dy;
		if ( y >= 1 - dy ){
			y = y0;
			x+= dx;
		}
	}
	Int_t ind = 0;
	while ( colobj->GetUniqueID() == 0) {
		if (gSystem->ProcessEvents())
			break;
		gSystem->Sleep(50);
		if (!gROOT->GetListOfCanvases()->FindObject("colcol")) {
			gROOT->GetListOfSpecials()->Remove(colobj);
			delete colobj;
			return 0;
		}
	}
	ind = colobj->GetUniqueID();
	gROOT->GetListOfSpecials()->Remove(colobj);
	delete colobj;
	delete ccol;
	return ind;
}
//________________________________________________________________
void IncrementIndex(TString * arg)
{
// find number at end of string and increment,
// if no number found add "_0";
	Int_t len = arg->Length();
	if (len < 0) return;
	Int_t ind = len - 1;
	Int_t first_digit = ind;
	TString subs;
	while (ind > 0) {
		subs = (*arg)(ind, len - ind);
		cout << subs << endl;
		if (!subs.IsDigit()) break;
		first_digit = ind;
		ind--;
	}
	if (first_digit == ind) {
	  *arg += "_0";
	} else {
		subs = (*arg)(first_digit, len - first_digit);
		Int_t num = atol(subs.Data());
		arg->Resize(first_digit);
		*arg += (num + 1);
	}
}
//________________________________________________________________

TString * GetStringArg(TString * arg,  Int_t nth)
{
	TRegexp apo("\"");
	TString * result = new TString(*arg);
	for (Int_t i = 0; i < 2*nth; i++) (*result)(apo) = "";
	Int_t ip = result->Index(apo);
	if (ip < 0) {
	  delete result;
	  return NULL;
	}
	result->Remove(0,ip+1);
//	cout << *result << endl;
	ip = result->Index(apo);
	if (ip < 0) {
	  delete result;
	  return NULL;
	}
	result->Resize(ip);
	return result;
}
//________________________________________________________________

TList * BuildList(const char *bp)
{
	TButton * b;
	b = (TButton *)strtoul(bp, 0, 16);
//		cout << "bp " << b << endl;
	TCanvas * mcanvas = b->GetCanvas();
//	cp_many title start
	TString ctitle(mcanvas->GetTitle());
	TRegexp cprefix("CP_");
	TRegexp cpostfix(".histlist");
	ctitle(cprefix) = "";
	ctitle(cpostfix) = "";
	TList * hlist = new TList();
	hlist->SetName(ctitle);
//	cout << "Title of mcanvas: " << ctitle.Data() << endl;
//  cp_many title end
	TIter next(mcanvas->GetListOfPrimitives());
	TString cmdline;
	TString entry;
	TString * arg;
	while (  TObject *obj=next()) {
		if (!strncmp(obj->ClassName(), "TButton", 7)) {
			TButton * button = (TButton*)obj;
			cmdline = button->GetMethod();
			if (cmdline.Contains("Select")) {
//				cout << cmdline  << endl;
				arg = GetStringArg(&cmdline, 0);
				if (arg) {
					entry = *arg;
					delete arg;
				} else {
					cout << "Illegal cmd: " << cmdline << endl;
					continue;
				}
				entry += " ";
				arg = GetStringArg(&cmdline, 2);
				if (arg) {
					entry += *arg;
					delete arg;
				} else {
					cout << "Illegal cmd: " << cmdline << endl;
					continue;
				}
				entry += " ";
				arg = GetStringArg(&cmdline, 1);
				if (arg) {
					entry += *arg;
					delete arg;
				} else {
					cout << "Illegal cmd: " << cmdline << endl;
					continue;
				}
//				cout << "|" << entry.Data() << "|" << endl;
				hlist->Add(new TObjString(entry.Data()));
			}
		}
	}
	return hlist;
}
//_______________________________________________________

Int_t FindHistsInFile(const char * rootf, const char * listf)
{
	ifstream list(listf);
	if (!list.good()) {
		cout << "Cant open: " << listf << endl;
		return -1;
	}
	TFile f(rootf);
	if (!f.IsOpen()) {
		cout << "Cant open: " << rootf << endl;
		return -1;
	}
	char line[100];
	Int_t nfound = 0;
	while (1) {
		list >> line;
		if (list.eof()) break;
		TObject * obj = f.Get(line);
		if (obj) nfound ++;
	}
//	cout << "FindHistsInFile " << rootf<< " " << listf << " : " << nfound<< endl;
	return nfound;
}
//__________________________________________________________________________

TH2 * rotate_hist(TH2 * hist, Double_t angle_deg, Int_t serial_nr)
{
	if (!hist) return NULL;
//	cout << "Enter rotate_hist" << endl << flush;
	TString hname(hist->GetName());
	Int_t us = hname.Index("_");
	if (us >= 0) hname.Remove(0, us + 1); // remove all before first underscore
	hname += "_rot";
	hname += serial_nr;
	TString htitle(hist->GetTitle());
	htitle += "_rot";
	htitle += serial_nr;
	TH1 * hold = (TH1*)gROOT->GetList()->FindObject(hname);
	if (hold) {
		delete hold;
//		cout << "rotate_hist,hold: " << hold << endl;
	}
//	cout << "rotate_hist before  hist->Clone()" << endl << flush;
	TH2 * hrot = (TH2*)hist->Clone();
//	cout << "rotate_hist after hist->Clone()" << endl << flush;
	hrot->Reset();
	hrot->SetName(hname);
	hrot->SetTitle(htitle);
//	TH2F * hrot = new TH2F(hname, htitle, nbinx, 0, xa, nbiny, 0, ya);
	Int_t firstx = hist->GetXaxis()->GetFirst();
	Int_t lastx  = hist->GetXaxis()->GetLast();
	Int_t firsty = hist->GetYaxis()->GetFirst();
	Int_t lasty  = hist->GetYaxis()->GetLast();
	Int_t nbinsx = hist->GetXaxis()->GetNbins();
	Int_t nbinsy = hist->GetYaxis()->GetNbins();
	Axis_t xoff = 0.5 * (Axis_t)(lastx - firstx + 2);
	Axis_t yoff = 0.5 * (Axis_t)(lasty - firsty + 2);
	Int_t xcent = firstx + (Int_t)xoff;
	Int_t ycent = firsty + (Int_t)yoff;
	
//	Axis_t xoff = 0.5 * (Axis_t)(nbinsx + 1);
//	Axis_t yoff = 0.5 * (Axis_t)(nbinsy + 1);
	cout<< " xcent, ycent "  << xcent << " " << ycent << endl;
	Float_t angle = TMath::Pi() * angle_deg / 180.;
//	Float_t angle = TMath::ATan(tan_a);
	Float_t sina = TMath::Sin(angle);
	Float_t cosa = TMath::Cos(angle);
//	for (Int_t binx = firstx; binx <= lastx; binx++) {
//		for (Int_t biny = firsty; biny <= lasty; biny++) {
	for (Int_t binx = 1; binx <= nbinsx; binx++) {
		for (Int_t biny = 1; biny <= nbinsy; biny++) {
			Axis_t x = (Axis_t)(-xcent + binx);
			Axis_t y = (Axis_t)(-ycent + biny);
			Int_t xr = Int_t(x * cosa - y *sina + xcent);
			Int_t yr = Int_t(x * sina + y *cosa + ycent);
//			if (xr >= firstx && xr <= lastx && yr >= firsty && yr <= lasty)
			if (xr >= 1 && xr <= nbinsx && yr >= 1 && yr <= nbinsy)
				hrot->SetBinContent(binx, biny, hist->GetBinContent(xr, yr));
		}
	}
	
//	cout << "Exit rotate_hist" << endl << flush;
	return hrot;
}

//__________________________________________________________

void SetAxisGraphY(TCanvas *c, TGraph *gr)
{
	TH1 *hist = gr->GetHistogram();
	if (!hist) {
		cout << "Graph must be drawn to set axis range" << endl;
	} else {
		SetAxisHistY(c, hist);
	}
}
//__________________________________________________________

void SetAxisHistY(TCanvas *c, TH1* hist)
{
	static void *valp[50];
	Int_t ind = 0;
	static Double_t xl, xu;
	TList *row_lab = new TList();
	TRootCanvas * win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
	Bool_t ok = kTRUE;
	xl = ((TVirtualPad*)c)->GetUymin();
	xu = ((TVirtualPad*)c)->GetUymax();	
	row_lab->Add(new TObjString("DoubleValue_Yaxis min"));
	row_lab->Add(new TObjString("DoubleValue_Yaxis max"));
	valp[ind++] = &xl;
	valp[ind++] = &xu;
	
	Int_t itemwidth = 320;
	ok = GetStringExt("Xmin, Xmax", NULL, itemwidth, win,
						 NULL, NULL, row_lab, valp,
						 NULL, NULL);
	if (ok) {
		TGaxis *a = (TGaxis*)gPad->GetListOfPrimitives()->FindObject("taxis_scaled");
		if ( a ) {
			Double_t scale = gPad->GetUymax() / a->GetWmax(); 
			cout<< "scale " << scale << " " <<gPad->GetUymin() / a->GetWmin() << endl; 
			a->SetY1(xl);
			a->SetY2(xu);
			a->SetWmin(xl/scale);
			a->SetWmax(xu/scale);
		}
		hist->SetMinimum(xl);
		hist->SetMaximum(xu);
		gPad->Modified();
		gPad->Update();
	}
}
//__________________________________________________________

void SetAxisGraphX(TCanvas *c, TGraph *gr)
{
	static void *valp[50];
	Int_t ind = 0;
	Bool_t ok = kTRUE;
	static Double_t xl;
	static Double_t xu;
	TList *row_lab = new TList();
	TH1 *hist = gr->GetHistogram();
	if (!hist) {
		cout << "Graph must be drawn to set axis range" << endl;
	} else {
		TRootCanvas * win = (TRootCanvas*)c->GetCanvasImp();
		row_lab->Add(new TObjString("DoubleValue_Xaxis min"));
		row_lab->Add(new TObjString("DoubleValue_Xaxis max"));
		valp[ind++] = &xl;
		valp[ind++] = &xu;
		Int_t itemwidth = 320;
		ok = GetStringExt("Xmin, Xmax", NULL, itemwidth, win,
							NULL, NULL, row_lab, valp,
							NULL, NULL);
		if (ok) {
			hist->GetXaxis()->Set(hist->GetNbinsX(),xl, xu );
			c->Modified();
			c->Update();
		}
	}
	delete row_lab;
}
//__________________________________________________________

void SetAxisHistX(TCanvas *c, TAxis * xa)
{
	static void *valp[50];
	Int_t ind = 0;
	Bool_t ok = kTRUE;
	static Double_t xl;
	static Double_t xu;
	static Int_t ixl;
	static Int_t ixu;
	static Int_t tof;
	static Int_t tofinit;
	static TString starttime;
	static TString endtime;
	static TString timeoffset;

	TList *row_lab = new TList();
	TRootCanvas * win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
//	TH1 * hist = gr->GetHistogram();
 //  if (hist == NULL) return;
//	TAxis * xa = hist->GetXaxis();
	xl = xa->GetBinLowEdge(TMath::Max(xa->GetFirst(),1));
	xu = xa->GetBinUpEdge(TMath::Min(xa->GetLast(),xa->GetNbins()));
//	xl = xa->GetXmin();
//	xu = xa->GetXmax();
//	cout << " xl, xu " << xl << " " << xu << endl;
	if (xa->GetTimeDisplay()) {
		ixl = (Int_t)xl;
		ixu = (Int_t)xu;
		tof = (Int_t)gStyle->GetTimeOffset();
		tofinit = tof;
		ixl += tof;
		ixu += tof;
 //	  cout << " ixl, ixu, tof " << ixl << " " << ixu << " " << tof << endl;
		ConvertTimeToString(ixl, xa, &starttime);
		ConvertTimeToString(ixu, xa, &endtime);
		ConvertTimeToString(tof, xa, &timeoffset);
		row_lab->Add(new TObjString("StringValue_Start time"));
		row_lab->Add(new TObjString("StringValue_End time"));
		row_lab->Add(new TObjString("StringValue_Time Offset"));
		valp[ind++] = &starttime;
		valp[ind++] = &endtime;
		valp[ind++] = &timeoffset;

//		row_lab->Add(new TObjString("PlainIntVal_Xaxis min"));
//		row_lab->Add(new TObjString("PlainIntVal_Xaxis max"));
//		valp[ind++] = &ixl;
//		valp[ind++] = &ixu;
	} else {
		row_lab->Add(new TObjString("DoubleValue_Xaxis min"));
		row_lab->Add(new TObjString("DoubleValue_Xaxis max"));
		valp[ind++] = &xl;
		valp[ind++] = &xu;
	}
	Int_t itemwidth = 320;
	ok = GetStringExt("Xmin, Xmax", NULL, itemwidth, win,
						 NULL, NULL, row_lab, valp,
						 NULL, NULL);
	if (ok) {
		if (xa->GetTimeDisplay()) {
			xl = ConvertToTimeOrDouble(starttime.Data(), xa);
			xu = ConvertToTimeOrDouble(endtime.Data(), xa);
			if (xl < 0 || xu < 0) return;
			tof = (Int_t)ConvertToTimeOrDouble(timeoffset.Data(), xa);
			if  (tof > 0)
				gStyle->SetTimeOffset((Double_t)tof);
 //		  cout << xl << " " << xu << endl;
			if (gStyle->GetTimeOffset() != 0) {
				xl -= tofinit;
				xu -= tofinit;
			}
			xa->SetTimeDisplay(kFALSE);
			Int_t bin1 = xa->FindBin(xl);
			Int_t bin2 = xa->FindBin(xu);
			xa->SetRange(bin1, bin2);
//			xa->SetLimits(xl, xu);
			xa->SetTimeDisplay(kTRUE);
		} else {
			Int_t bin1 = xa->FindBin(xl);
			Int_t bin2 = xa->FindBin(xu);
			xa->SetRange(bin1, bin2);
//		  xa->SetLimits(xl, xu);
		}
	}
	c->cd();
	gPad->Modified();
	gPad->Update();
}
//_______________________________________________________________________________________

void ConvertTimeToString(time_t t, TAxis * /*a*/, TString * string)
{
	if (t == 0) *string = "";
	TDatime dt;
	dt.Set(t);
	*string = dt.AsSQLString();
}
//_______________________________________________________________________________________

Double_t ConvertToTimeOrDouble(const char * string, TAxis * /*a*/)
{
	Int_t yy, mm, dd, hh, mi, ss;
	TString s(string);
	if (s.Length() < 2) return 0;
	if (sscanf(string, "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &mi, &ss) != 6) {
		cout << setblue << " Trying to correct time format: " << s << setblack << endl;
		if (s.Contains('-') && !s.Contains(':')) {
		  s += " 0:0:0";
		} else {
			cout << setred << "Wrong time format: " << s << endl << "must be: yy-mm-dd hh:mm:ss" << setblack << endl;
			return -1;
		}
	}
	if (sscanf(s.Data(), "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &mi, &ss) != 6) {
		cout << setred << "Cant correct time format: " << s << endl << "must be: yy-mm-dd hh:mm:ss" << setblack << endl;
		return -1;
	}

	TDatime dt;
	dt.Set(s.Data());
	return dt.Convert();
}

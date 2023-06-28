#include "TROOT.h"
#include "TEnv.h"
//#include "TPad.h"
#include "TCanvas.h"
#include "THprLatex.h"
#include "TMarker.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TRootCanvas.h"
#include "TSystem.h"
#include "TGMrbValuesAndText.h"
#ifdef MARABOUVERS
#include "GroupOfGObjects.h"
#include "TCutG.h"
#endif
#include "TextOnArc.h"
#include "InsertTextDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertTextDialog)

//______________________________________________________________________________

Int_t getm(TString& cmd, Int_t sind)
{
	Int_t ind = sind;
	Int_t nbopen = 0;
	while (ind < cmd.Length()) {
	  if (cmd[ind] == '}') {
		  nbopen -= 1;
		  if (nbopen == 0) return ind;
	  }
	  if (cmd[ind] == '{') nbopen += 1;
	  ind += 1;
	}
	return -1;
}
Int_t getmb(TString& cmd, Int_t sind)
{
	Int_t ind = sind;
	Int_t nclose = 0;
	while (ind >= 0) {
	  if (cmd[ind] == '{') {
		  nclose -= 1;
		  if (nclose == 0) return ind;
	  }
	  if (cmd[ind] == '}') nclose += 1;
	  ind -= 1;
	}
	return -1;
}
//______________________________________________________________________________
TString lat2root(TString& cmd)
{
// this tries to translate standard Latex into ROOTs
// latex like formular processor TLatex format

//    remove latex's $ (mathstyle), replace \ by #, ~ by space
	Int_t ind;
	Int_t sind = 0;
	TString ill("Illegal syntax");
	while (cmd.Index("$") >=0) cmd.Remove(cmd.Index("$"),1);
	while (cmd.Index("\\{") >=0) cmd.Remove(cmd.Index("\\{"),1);
	while (cmd.Index("\\}") >=0) cmd.Remove(cmd.Index("\\}"),1);
	while (cmd.Index("\\") >=0) cmd(cmd.Index("\\")) = '#';
	while (cmd.Index("~") >=0) cmd(cmd.Index("~")) = ' ';
//  caligraphics not yet supported
	while (cmd.Index("#cal") >=0) {
		Int_t ob = cmd.Index("#cal");
		Int_t cb = getm(cmd, ob + 4);
		if (cb < 0) {
			cout << "in #cal no closing }" << endl;
			return ill;
		}
		cmd.Remove(cb,1);
		cmd.Remove(ob,5);
	}
//    make sure super / sub scripts are enclosed in {}
	TRegexp supsub("[_^]");
	TString rep;
	sind = 0;
	while (cmd.Index(supsub, sind) >=0) {
		ind = 1 + cmd.Index(supsub, sind);
		if (ind >= cmd.Length()) break;
		char c = cmd[ind];
		sind = ind + 1;
		if (c != '{') {
			rep = "{";
			rep += c;
			rep += "}";
			cmd.Replace(ind, 1, rep);
			sind += 2;
		}
	}

//   add space around operators

	TRegexp oper("[-+*/=]");
	sind = 0;
	while (cmd.Index(oper, sind) >=0) {

		ind = 1 + cmd.Index(oper, sind);
		char c = cmd[ind];
		sind = ind + 1;
//   are we within sub / superscript?

		TString le = cmd(0, ind -1);
		Int_t ob = le.Last('{');
		if (ob > 0 && (cmd(ob-1) == '^'
					  || (cmd(ob-1) == '_'))) {
			if (getm(le, ob) == - 1)  continue; // no match before
		}
		if (c != ' ' && c!= '{' && c!= '}') {
			if (ind > -1 && ind < cmd.Length()) {
				cmd.Insert(ind," ");
				sind += 1;
			}
		}
		if (ind > 1) {
			c = cmd[ind - 2];
			if (c != ' ' && c!= '{' && c!= '}') {
				cmd.Insert(ind-1," ");
				sind += 1;
			}
		}
	}
//   replace \over by \frac{}{}

 ind = cmd.Index("#over");
 Int_t ind1 = cmd.Index("#overline");
 if (ind > 0 && ind != ind1) {
	  TString le = cmd(0, ind);
	  Int_t cb = le.Last('}');
	  if (cb < 0) {
		  cout << "no closing } found" << endl;
		  return ill;
	  }
	  Int_t ob = getmb(cmd, cb);
	  if (ob < 0) {
		  cout << "no matching { found" << endl;
		  return ill;
	  }
	  cmd.Remove(ind, 5);
	  cmd.Insert(ob, " #frac");
  }

//   remove not used \cos etc,
//   replace overline by bar

	TRegexp re_Ra("#Ra");
	while (cmd.Index(re_Ra) >= 0) cmd(re_Ra) = " #Rightarrow ";
	TRegexp re_La("#La");
	while (cmd.Index(re_La) >= 0) cmd(re_La) = " #Leftarrow ";
	TRegexp re_cdot("#cdot");
	while (cmd.Index(re_cdot) >= 0) cmd(re_cdot) = " #upoint ";
	TRegexp re_exp("#exp");
	while (cmd.Index(re_exp) >= 0) cmd(re_exp) = " e^";
	TRegexp re_ln("#ln");
	while (cmd.Index(re_ln) >= 0) cmd(re_ln) = "ln";
	TRegexp re_cos("#cos");
	while (cmd.Index(re_cos) >= 0)cmd(re_cos) = "cos";
	TRegexp re_sin("#sin");
	while (cmd.Index(re_sin) >= 0)cmd(re_sin) = "sin";
	TRegexp re_tan("#tan");
	while (cmd.Index(re_tan) >= 0)cmd(re_tan) = "tan";
	TRegexp re_ovl("#overline");
	while (cmd.Index(re_ovl) >= 0)cmd(re_ovl) = "#bar";
//   cout << cmd << endl;
	return cmd;
}
//___________________________________________________________________________

InsertTextDialog::InsertTextDialog(Bool_t from_file, TObject *calling_object)
{
static const Char_t helptext[] =
"This widget is used to insert text either directly\n\
from the keyboard or from a file. In the keyboard case\n\
text is edited in this widget and placed by clicking \n\
\"ExecuteTextInsert\" and then at the desired position.\n\
In the file case serveral lines may be arranged as a compound\n\
which may shifted together.\n\
\"InsertTextOnArc\" allows placing of the text on circle.\n\
Radius, Start Angle and Align determine the position\n\
e.g. Angle: 90 Align: Center will place the text centered\n\
at top of a circle\n\
\"Char Sep\" allows extra space between characters \n\
measured in fractions of the length of an \"a\".\n\
\n\
The program remembers the input in a file \"text_hist.txt\"\n\
located in the current working directory.\n\
It might be handy to prepare/modify this file by your\n\
favorite editor.\n\
Since ROOT Version 5.34.13 pure LaTeX input is supported\n\
To profit from this use \"\\\" instead of \"#\" and let\n\
\"Apply latex filter\" off.\n\
If the canvas is Saved As \".tex\" the LateX code is inserted\n\
directly into the TeX file, e.g. formula will be rendered by\n\
LaTeX itself.\n\
";
	fCanvas = gPad->GetCanvas();
	TRootCanvas* win = NULL;
	if (fCanvas){
		win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "InsertTextDialog", this, "CloseDialog()");
	}
	fCaller = calling_object;
	Int_t ind = 0;
	fRow_lab = new TList();
	RestoreDefaults();
	fEditTextFromFile = (Int_t)from_file;

	static TString toacmd("InsertTextOnArc()");
	static TString excmd("InsertTextExecute()");
	static TString showcmd("Show_Head_of_File()");
	static const char hist_file[] = {"text_hist.txt"};

	if (fEditTextFromFile) {
		fRow_lab->Add(new TObjString("FileRequest_File Name with text"));
		fValp[ind++] = &fEditTextFileName;
		fEditTextMarkCompound = 1;
	} else {
		fEditTextMarkCompound = 0;
	}
	fRow_lab->Add(new TObjString("DoubleValue_X Pos/Offs"));
	fValp[ind++] = &fEditTextX0;
	fRow_lab->Add(new TObjString("DoubleValue+Y Pos/Offs"));
	fValp[ind++] = &fEditTextY0;
	fRow_lab->Add(new TObjString("DoubleValue_Radius (TOArc)"));
	fValp[ind++] = &fEditTextRadius;
	fRow_lab->Add(new TObjString("DoubleValue+Start Angle"));
	fValp[ind++] = &fEditTextStartAngle;
	fRow_lab->Add(new TObjString("CfontSelect_Font"));
	fValp[ind++] = &fEditTextFont;
	fRow_lab->Add(new TObjString("PlainIntVal+Precission"));
	fValp[ind++] = &fEditTextPrec;
	fRow_lab->Add(new TObjString("DoubleValue_LineSpc"));
	fValp[ind++] = &fEditTextDy;
	fRow_lab->Add(new TObjString("Float_Value+Size"));
	fValp[ind++] = &fEditTextSize;
	fRow_lab->Add(new TObjString("DoubleValue+Char Sep"));
	fValp[ind++] = &fEditTextCharSep;
	fRow_lab->Add(new TObjString("ColorSelect_Color"));
	fValp[ind++] = &fEditTextColor;
	fRow_lab->Add(new TObjString("AlignSelect+Align"));
	fValp[ind++] = &fEditTextAlign;
	fRow_lab->Add(new TObjString("Float_Value+Angle"));
	fValp[ind++] = &fEditTextAngle;
	fRow_lab->Add(new TObjString("CheckButton_Mark as compound"));
	 fValp[ind++] = &fEditTextMarkCompound;
	fRow_lab->Add(new TObjString("CheckButton+Apply latex filter"));
	fValp[ind++] = &fEditTextLatexFilter;
	fRow_lab->Add(new TObjString("CommandButt_InsertTextExecute"));
	fValp[ind++] = &excmd;
	fRow_lab->Add(new TObjString("CommandButt_InsertTextOnArc"));
	fValp[ind++] = &toacmd;
	if (fEditTextFromFile) {
		fRow_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
		fValp[ind++] = &showcmd;
	}

//   if (!from_file) {
//       fRow_lab->Add(new TObjString("CheckButton_Keep Dialog"));
//       fValp[ind++] = &keepdialog;
//   }

	static TString text;
//   TString * tpointer = 0;
	fEditTextPointer = NULL;
	const char * history = 0;
	if (fEditTextFromFile == 0) {
		fEditTextPointer = &text;
		history = hist_file;
//      if (gROOT->GetVersionInt() < 40000) history = NULL;
	}
	Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
	static Int_t ok;
	TObject *caller = this;
	if ( fCaller )
		caller =  fCaller;
//   cout << "TObject* co = (TObject*) " <<caller << endl;
	fDialog =
		new TGMrbValuesAndText("Insert Text", fEditTextPointer, &ok,itemwidth, win,
							 history, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, caller, caller->ClassName());
	if (fCanvas) {
		 GrCanvas* hc = (GrCanvas*)fCanvas;
		 hc->Add2ConnectedClasses(this);
	}
};
//_________________________________________________________________________

void InsertTextDialog::InsertTextOnArc()
{
	InsertTextExecute(1);
}
//_________________________________________________________________________

void InsertTextDialog::InsertTextExecute(Int_t onarc)
{
	Bool_t clear_textposition = kFALSE;
	if (fEditTextX0 == 0 && fEditTextY0 == 0) {
		cout << "Mark position with left mouse" << endl;
		TMarker * mark  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
		if (mark == NULL) {
			return;
		}
		clear_textposition = kTRUE;
		fEditTextX0 = mark->GetX();
		fEditTextY0 = mark->GetY();
		delete mark;
	}
	fCanvas->cd();
	ifstream infile;
	TString line;
	TString cmd;
	TString converted_line;

	TLatex  * latex;
	Double_t xt = fEditTextX0;
	Double_t yt = fEditTextY0;
	Double_t longestline = 0, th_first = 0, th_last = 0;
	TList llist;
	Bool_t loop = kTRUE;
	if (fEditTextFromFile != 0) {
		infile.open(fEditTextFileName.Data());
		if (!infile.good()){
			cout << "Cant open: " << fEditTextFileName << endl;
			return;
		}
	}
	while(loop) {
// read lines, concatinate lines ending with
		if (fEditTextFromFile != 0) {
			line.ReadLine(infile);
			if (infile.eof()) {
				infile.close();
				if (cmd.Length() > 0) {
					cout << "Warning: Files ends with \\" << endl;
					cout << cmd << endl;
				}
				break;
			}
			line = line.Strip(TString::kBoth);
			cmd = cmd + line;
			if (cmd.EndsWith("\\")) {
				cmd(cmd.Length()-1) = ' ';
				continue;
			}
		} else {
			cmd = fEditTextPointer->Data();
//         cout << fEditTextPointer << " " << cmd.Data() << endl;
			loop = kFALSE;
		}
//      cout << "Text:  " << cmd << endl;
		Int_t nbrace_open = 0;
		Int_t nbrace_close = 0;
		for (Int_t i = 0; i < cmd.Length(); i++) {
			if (cmd[i] == '}')
				nbrace_close++;
			if (cmd[i] == '{')
				nbrace_open++;
		}
		if (nbrace_close != nbrace_open) {
			cout << "Braces dont match, open:	" << nbrace_open 
			<< " close: " << nbrace_close << endl;
			continue;
		}
		if ( onarc == 1 ) {
			 TextOnArc *tt = new TextOnArc(fEditTextX0, fEditTextY0, fEditTextRadius, 
								 cmd, fEditTextStartAngle, fEditTextAlign/10);
				tt->SetTextFont(10 * (fEditTextFont/10) + fEditTextPrec);
				tt->SetTextSize(fEditTextSize);
				tt->SetTextColor(fEditTextColor);
				tt->SetSeperator(fEditTextCharSep);
				tt->Draw();
		} else {
			if (fEditTextLatexFilter > 0) converted_line = lat2root(cmd);
			else                          converted_line = cmd;
//			latex = new TLatex(xt, yt, converted_line.Data());
			latex = new THprLatex(xt, yt, converted_line.Data());
			latex->SetTextAlign(fEditTextAlign);
			latex->SetTextFont(10 * (fEditTextFont/10) + fEditTextPrec);
			latex->SetTextSize(fEditTextSize);
			latex->SetTextColor(fEditTextColor);
			latex->SetTextAngle(fEditTextAngle);
//			cout << "Text ent: |" << converted_line <<"| " << converted_line.Length()<< endl;
//			converted_line="\\frac{\\pi}{BB}";
//			cout << "Text set: |" << converted_line <<"|" << converted_line.Length()<< endl;
			latex->DrawLatex(xt, yt, converted_line.Data());
			llist.Add(latex);
			yt -= fEditTextDy;
			cmd.Resize(0);
			if (latex->GetXsize() > longestline) longestline = latex->GetXsize();
			if (th_first <= 0) th_first = latex->GetYsize();
			th_last = latex->GetYsize();
			latex->SetTextAngle(fEditTextAngle);
		}
	}

	Int_t nlines = llist.GetSize();
	if (nlines > 1) {
#ifdef MARABOUVERS
		GroupOfGObjects * text_group = NULL;
		TString cname("text_obj_");

		if (fEditTextMarkCompound) {
			text_group = new GroupOfGObjects(cname.Data(), 0, 0, NULL);
			cname += fEditTextSeqNr;
		}
#endif
		Double_t yshift = 0;
		if (fEditTextAlign%10 == 1)yshift =  (nlines -1) * fEditTextDy;
		if (fEditTextAlign%10 == 2)yshift =  (nlines  -1)* (0.5 * fEditTextDy);
		TIter next(&llist);
		while ( (latex = (TLatex*)next()) ) {
			 latex->SetY(latex->GetY() + yshift);
#ifdef MARABOUVERS
			 if (text_group) text_group->AddMember(latex, "");
#endif
		}
#ifdef MARABOUVERS
		if (fEditTextMarkCompound) {
			yt += fEditTextDy;        // last displayed line
			Double_t xenc[5];
			Double_t yenc[5];
			yenc[0] = yt + yshift;
//      	if (fEditTextAlign%10 == 1)yenc[0] -= 0.5 * th_last;
			if (fEditTextAlign%10 == 2)yenc[0] -= 0.5 * th_last;
			if (fEditTextAlign%10 == 3)yenc[0] -= th_last;
			yenc[1] = yenc[0];

			yenc[2] = fEditTextY0 + yshift;
			if (fEditTextAlign%10 == 2)yenc[2] += 0.5 * th_first;
			if (fEditTextAlign%10 == 1)yenc[2] += th_first;
			yenc[3] = yenc[2];
			yenc[4] = yenc[0];

			Int_t halign = fEditTextAlign / 10;
			if (halign == 1) {
				xenc[0] = fEditTextX0;
				xenc[1] = xenc[0] + longestline;
				xenc[0] -= 0.001;
			} else if (halign == 2) {
				xenc[0] = fEditTextX0 + 0.5 * longestline;
				xenc[1] = xenc[0] - longestline;
			} else {
				xenc[0] = fEditTextX0 - longestline;
				xenc[1] = fEditTextX0 + 0.001;
			}
			xenc[2] = xenc[1];
			xenc[3] = xenc[0];
			xenc[4] = xenc[0];
			TCutG cut(cname.Data(), 5, xenc, yenc);
	//      cut.Print();
			text_group->SetEnclosingCut(&cut);
			text_group->Draw();
		}
#endif
	}
	gPad->Modified();
	gPad->Update();
	if ( clear_textposition ) {
		fEditTextX0 = 0;
		fEditTextY0 = 0;
	}
};
//_________________________________________________________________________

void InsertTextDialog::SaveDefaults()
{
	TEnv env(".hprrc");
//   cout << "InsertTextDialog SaveDefaults()" << endl;
	env.SetValue("InsertTextDialog.EditTextX0"  		   , fEditTextX0);
	env.SetValue("InsertTextDialog.EditTextY0"  		   , fEditTextY0);
	env.SetValue("InsertTextDialog.EditTextFileName"	, fEditTextFileName   );
	env.SetValue("InsertTextDialog.EditTextDy"			, fEditTextDy  		 );
	env.SetValue("InsertTextDialog.EditTextAlign"		, fEditTextAlign  	 );
	env.SetValue("InsertTextDialog.EditTextColor"		, fEditTextColor  	 );
	env.SetValue("InsertTextDialog.EditTextFont" 		, fEditTextFont		 );
	env.SetValue("InsertTextDialog.EditTextPrec" 		, fEditTextPrec		 );
	env.SetValue("InsertTextDialog.EditTextSize" 		, fEditTextSize		 );
	env.SetValue("InsertTextDialog.EditTextCharSep"		, fEditTextCharSep	 );
	env.SetValue("InsertTextDialog.EditTextAngle"		, fEditTextAngle  	 );
	env.SetValue("InsertTextDialog.EditTextLatexFilter", fEditTextLatexFilter);
	env.SetValue("InsertTextDialog.EditTextStartAngle" , fEditTextStartAngle);
	env.SetValue("InsertTextDialog.EditTextRadius"     , fEditTextRadius);
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertTextDialog::RestoreDefaults()
{
//   cout << "GrCanvas::InsertTextSetDefaults()" << endl;
	TEnv env(".hprrc");
	fEditTextFileName = "latex.txt";
	fEditTextFromFile = 0;
	fEditTextMarkCompound = 0;
	fEditTextSeqNr = 0;

	fEditTextX0          = env.GetValue("InsertTextDialog.EditTextX0"  		  , 0.);
	fEditTextY0          = env.GetValue("InsertTextDialog.EditTextY0"  		  , 0.);
	fEditTextFileName    = env.GetValue("InsertTextDialog.EditTextFileName"   , "latex.txt");
	fEditTextDy          = env.GetValue("InsertTextDialog.EditTextDy"  		  , 10.);
	fEditTextAlign       = env.GetValue("InsertTextDialog.EditTextAlign"  	  , 11);
	fEditTextColor       = env.GetValue("InsertTextDialog.EditTextColor"  	  , 1);
	fEditTextFont        = env.GetValue("InsertTextDialog.EditTextFont"		  , 62);
	if ( fEditTextFont < 10 )
		fEditTextFont *= 10;
	if ( fEditTextFont > 150 )
		fEditTextFont /= 10;
	fEditTextPrec        = env.GetValue("InsertTextDialog.EditTextPrec"		  , 2);
	fEditTextSize        = env.GetValue("InsertTextDialog.EditTextSize"		  , 0.02);
	fEditTextCharSep     = env.GetValue("InsertTextDialog.EditTextCharSep"	  , 0.);
	fEditTextAngle       = env.GetValue("InsertTextDialog.EditTextAngle"  	  , 0.);
	fEditTextLatexFilter = env.GetValue("InsertTextDialog.EditTextLatexFilter", 0);
	fEditTextStartAngle  = env.GetValue("InsertTextDialog.EditTextStartAngle" , 90.);
	fEditTextRadius      = env.GetValue("InsertTextDialog.EditTextRadius"     , 30.);
	
}
//_________________________________________________________________________

void InsertTextDialog::Show_Head_of_File()
{
	TString cmd(fEditTextFileName.Data());
	cmd.Prepend("head ");
//   cmd.Append("\")");
	gSystem->Exec(cmd);
}

//_________________________________________________________________________

InsertTextDialog::~InsertTextDialog()
{
//   cout << "InsertTextDialog: dtor "  << endl;
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void InsertTextDialog::CloseDialog()
{
//   cout << "InsertTextDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void InsertTextDialog::CloseDown(Int_t wid)
{
//   cout << "InsertTextDialog::CloseDown()" << endl;
	if (wid != -2) SaveDefaults();
	delete this;
}

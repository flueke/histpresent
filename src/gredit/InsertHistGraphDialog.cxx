
#include "TROOT.h"
#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TGMsgBox.h"
#include "GEdit.h"
#include "HTPad.h"
#include "InsertHistGraphDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertHistGraphDialog)

InsertHistGraphDialog::InsertHistGraphDialog(Int_t what) : fWhat(what)
{
static const Char_t helptext[] =
"\n\
";
	fCanvas = gPad->GetCanvas();
	fWindow = NULL;
	if (fCanvas) {
		fWindow = (TRootCanvas*)fCanvas->GetCanvasImp();
		fWindow->Connect("CloseWindow()", "InsertHistGraphDialog", this, "CloseDialog()");
	}
	Int_t ind = 0;
	RestoreDefaults();
	fRow_lab = new TList();
	static TString execute_cmd("ExecuteInsert()");
	fHistFromFile = fFileName;
	if (fWhat == 1 ) {
		fHistFromFile += "|TGraph| ";
		fRow_lab->Add(new TObjString("FileContReq_ROOT file / graph"));
	} else {
		fHistFromFile += "|TH1| ";
		fRow_lab->Add(new TObjString("FileContReq_ROOT file / histo"));
	}
	fValp[ind++] = &fHistFromFile;

	fRow_lab->Add(new TObjString("StringValue_Drawing option"));
	fValp[ind++] = &fDrawopt;

	fRow_lab->Add(new TObjString("DoubleValue_Scale factor labels/titles"));
	fValp[ind++] = &fScale;

	fRow_lab->Add(new TObjString("CommandButt_Execute Insert()"));
	fValp[ind++] = &execute_cmd;

	Int_t itemwidth =  35 * TGMrbValuesAndText::LabelLetterWidth();
	static Int_t ok;
	fDialog =
		new TGMrbValuesAndText("Insert hist / graph", NULL, &ok,itemwidth, fWindow,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
}
//____________________________________________________________________________

void InsertHistGraphDialog::ExecuteInsert()
{
	fCanvas->cd();
	HTPad *pad = GEdit::GetEmptyPad();
	if ( !pad ) {
		Int_t retval = 0;
		new TGMsgBox(gClient->GetRoot(), fWindow, "Warning",
		"Please create a new Pad in this Canvas",
		kMBIconExclamation, kMBDismiss, &retval);
		return;
	}
	cout << "fHistFromFile: "<< fHistFromFile << endl;
	TObjArray * oa = fHistFromFile.Tokenize("|");
	Int_t nent = oa->GetEntries();
	if (nent < 3) {
		cout << "fHistFromFile not enough (3) fields" << endl;
		return;
	}
	TString fname =((TObjString*)oa->At(0))->String();
	TString cname =((TObjString*)oa->At(1))->String();
	TString oname =((TObjString*)oa->At(2))->String();
//   if (cname != "TH1") {
//      cout << "fHistFromFile " << oname << " is not a hist " << endl;
//      return;
//   }
	TFile f(fname);
	TGraph *graph = NULL;
	TH1    *hist  = NULL;
	if (fWhat == 1 ) {
		graph = (TGraph*)f.Get(oname);
		if (graph == NULL) {
			cout << "No graph found / selected" << endl;
			return;
		}
//      fDrawopt += "AL";
		cout << "graph fDrawopt " <<fDrawopt << endl;
		graph->Draw(fDrawopt.Data());
		gPad->Update();
		hist = graph->GetHistogram();
	} else {
		hist = (TH1*)f.Get(oname);
		if (hist == NULL) {
			cout << "No histo found / selected" << endl;
			return;
		}

		TString hn = hist->GetName();
		if (hn.Index(";") > 0) {
			hn.Resize(hn.Index(";"));
			hist->SetName(hn);
		}
		hist->SetDirectory(gROOT);
		hist->Draw(fDrawopt.Data());
		cout << "hist fDrawopt " <<fDrawopt << endl;
	}
	pad->cd();
	if (TMath::Abs(fScale -1) > 0.0001) {
		TAxis * a = hist->GetXaxis();
		if (a) {
			a->SetLabelSize( fScale * a->GetLabelSize());
			a->SetTickLength( fScale * a->GetTickLength());
		}
		a = hist->GetYaxis();
		if (a) {
			a->SetLabelSize( fScale * a->GetLabelSize());
			a->SetTickLength( fScale * a->GetTickLength());
		}
	}
	gPad->Modified();
	gPad->Update();
};
//____________________________________________________________________________

void InsertHistGraphDialog::SaveDefaults()
{
//   cout << "InsertHistGraphDialog::InsertFunction::SaveDefaults()" << endl;
	TEnv env(".hprrc");
	env.SetValue("InsertHistGraphDialog.fScale"   ,fScale);
	env.SetValue("InsertHistGraphDialog.fDrawopt" ,fDrawopt);
	env.SetValue("InsertHistGraphDialog.fFileName" ,fFileName);
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertHistGraphDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fScale = env.GetValue("InsertHistGraphDialog.fScale" ,1.);
	fDrawopt = env.GetValue("InsertHistGraphDialog.fDrawopt" ,"AL");
	fFileName = env.GetValue("InsertHistGraphDialog.fFileName" ,"gr.root");
}
//_________________________________________________________________________

InsertHistGraphDialog::~InsertHistGraphDialog()
{
	if(fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void InsertHistGraphDialog::CloseDialog()
{
//   cout << "InsertHistGraphDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void InsertHistGraphDialog::CloseDown(Int_t wid)
{
//   cout << "InsertHistGraphDialog::CloseDown()" << endl;
	if (wid != -2) SaveDefaults();
	delete this;
}

void  InsertHistGraphDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   cout << "InsertHistGraphDialog::CRButtonPressed " << bid << " fScale "
//        << fScale  << endl;
}

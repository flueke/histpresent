
#include "TROOT.h"
#include "TEnv.h"
#include "HprImage.h"
#include "TGMsgBox.h"
#include "TSystem.h"
#include "TMarker.h"
#include "HTPad.h"
#include "XGrabDialog.h"
#include "SetColor.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(XGrabDialog)

XGrabDialog::XGrabDialog()
{
static const Char_t helptext[] =
"\n\
";
	fCanvas = gPad->GetCanvas();
	fWindow = NULL;
	if (fCanvas)
		fWindow = (TRootCanvas*)fCanvas->GetCanvasImp();
	Int_t ind = 0;
	RestoreDefaults();
	fRow_lab = new TList();
	static TString execute_cmd("ExecuteInsert()");

	fRow_lab->Add(new TObjString("CheckButton_Fixed size"));
	fValp[ind++] = &fFixedSize;
	fRow_lab->Add(new TObjString("AlignSelect+Alignment"));
	fValp[ind++] = &fAlign;
	fRow_lab->Add(new TObjString("PlainIntVal+fDelay [sec]"));
	fValp[ind++] = &fDelay;
	fRow_lab->Add(new TObjString("PlainIntVal_X Width"));
	fValp[ind++] = &fXwidth;
	fRow_lab->Add(new TObjString("PlainIntVal+Y Width"));
	fValp[ind++] = &fYwidth;
	fRow_lab->Add(new TObjString("StringValue_Picture name"));
	fValp[ind++] = &fPname;
	fRow_lab->Add(new TObjString("StringValue+Picture type"));
	fValp[ind++] = &fPext;
	fRow_lab->Add(new TObjString("CheckButton_Ask for ok after grab"));
	fValp[ind++] = &fQuery_ok;

	fRow_lab->Add(new TObjString("CommandButt+Execute Insert()"));
	fValp[ind++] = &execute_cmd;
	Int_t itemwidth =  35 * TGMrbValuesAndText::LabelLetterWidth();
	static Int_t ok;
	fDialog =
		new TGMrbValuesAndText("Graphics Pad", NULL, &ok,itemwidth, fWindow,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
		 GrCanvas* hc = (GrCanvas*)fCanvas;
		 hc->Add2ConnectedClasses(this);
	}
}
//____________________________________________________________________________

void XGrabDialog::ExecuteInsert()
{
	TString pname(fPname.Data());
	pname += fSerNr;
	pname += ".";
	pname += fPext.Data();
	
	cout << "Mark pick area: pname " <<  pname << endl;

	TString cmd("xgrabsc -ppm -verbose 1> /dev/null 2> xgrabsc.log");
	gSystem->Exec(cmd.Data());

	Int_t xleg = 0, yleg = 0, xwg = 0, ywg = 0;
	ifstream str("xgrabsc.log");
	TString line;
	TString number;
	while (1) {
		line.ReadLine(str);
		if(str.eof()) break;
		if (!line.Contains("bounding box")) continue;
		Int_t ip;
		ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
		number = line(0,ip);
		xleg = number.Atoi();

		ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
		number = line(0,ip);
		yleg = number.Atoi();

		ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index(" ");
		number = line(0,ip);
		xwg = number.Atoi();

		ip = line.Index("="); line.Remove(0, ip+1); ip = line.Index("]");
		number = line(0,ip);
		ywg = number.Atoi();
		break;
	}
//   str.close();
	if (xwg == 0 && fFixedSize == 0) {
		cout << setred
		<< "Variable width requested, please mark area with Button 1 pressed"
		<< setblack  << endl;
		return;
	}
// fAlignment
	if (fFixedSize != 0) {
		if        (fAlign%10 == 1) {
			yleg = yleg + ywg - fYwidth;
		} else if (fAlign%10 == 2) {
			yleg = yleg + (ywg - fYwidth) / 2;
		}
		if        (fAlign/10 == 3) {
			xleg = xleg + xwg - fXwidth;
		} else if (fAlign/10 == 2) {
			xleg = xleg + (xwg - fXwidth) / 2;
		}
		xwg = fXwidth;
		ywg = fYwidth;
	}
	fSerNr++;
	cmd = "xgrabsc -ppm -coords ";
	cmd += xwg; cmd += "x"; cmd += ywg;
	cmd += "+";  cmd += xleg;
	cmd += "+"; cmd += yleg;
	cmd += " | convert - ";
	cmd += fPname.Data();
	cmd += " &";

	cout << "Cmd: " << cmd << endl;
	if (fDelay > 0) {
		cout << "Waiting " << fDelay << " seconds before grab" << endl;
		Int_t wt = 10 * fDelay;
		while (wt-- >= 0) {
			gSystem->ProcessEvents();
			gSystem->Sleep(100);
		}
	}
	gSystem->Exec(cmd.Data());
	gSystem->ProcessEvents();

	if (fQuery_ok != 0) {
		gSystem->Exec(cmd.Data());
		cmd = "kview --geometry ";
		cmd += fXwidth;
		cmd += "x";
		cmd += fYwidth;
		cmd += " ";
		cmd += fPname.Data();
		cmd += "&";
		gSystem->Exec(cmd.Data());
		cout << cmd << endl;
		Int_t retval = 0;
		new TGMsgBox(gClient->GetRoot(), fWindow, "Question", "Is it ok?",
					 kMBIconQuestion, kMBYes | kMBNo, &retval);
		cmd = "killall kview";
		gSystem->Exec(cmd.Data());
		if (retval == kMBNo) return;
	}
	cout << "Mark position where to put (lower left corner)" << endl;
	TMarker * ma = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
	if (ma == NULL)
		return;
	Double_t x1, y1, x2, y2, xr1, yr1, xr2, yr2, dx, dy;
	x1 = ma->GetX();
	y1 = ma->GetY();
	delete ma;
	gPad->GetRange(xr1, yr1, xr2, yr2);
	dx = fXwidth / (Double_t)fCanvas->GetWw();
	dy = fYwidth / (Double_t)fCanvas->GetWh();
	dx *= (xr2 - xr1);
	dy *= (yr2 - yr1);

	if (fFixedSize != 0) {
		if        (fAlign%10 == 3) {
			y1 -= dy;
		} else if (fAlign%10 == 2) {
			y1 -= (dy / 2);
		}
		if        (fAlign/10 == 3) {
			x1 -= dx;
		} else if (fAlign/10 == 2) {
			x1 -= ( dx / 2);
		}
	}
//    back to NDC 
	x1 = (x1 - xr1) / (xr2 - xr1);
	y1 = (y1 - yr1) / (yr2 - yr1);
	x2 = x1 + fXwidth / (Double_t)fCanvas->GetWw();
	y2 = y1 + fYwidth / (Double_t)fCanvas->GetWh();

	HTPad * pad = new HTPad(fPname.Data(), "For HprImage",
									x1, y1, x2, y2);
	pad->Draw();
//      TImage *hprimg = TImage::Open(fPname.Data());
	HprImage * hprimg = new HprImage(fPname.Data(), pad);
	gROOT->SetSelectedPad(pad);
	pad->cd();
	hprimg->Draw();
	fCanvas->cd();
	gPad->Modified();
	gPad->Update();
	cout << "quit ExecuteInsert " << endl;
//   SaveDefaults();
};
//____________________________________________________________________________

void XGrabDialog::SaveDefaults()
{
//   cout << "XGrabDialog::InsertFunction::SaveDefaults()" << endl;
	TEnv env(".hprrc");
	env.SetValue("XGrabDialog.fFixedSize", fFixedSize);
	env.SetValue("XGrabDialog.fAlign", fAlign);
	env.SetValue("XGrabDialog.fDelay", fDelay);
	env.SetValue("XGrabDialog.fXwidth", fXwidth);
	env.SetValue("XGrabDialog.fYwidth", fYwidth);
	env.SetValue("XGrabDialog.fSerNr", fSerNr);
	env.SetValue("XGrabDialog.fQuery_ok", fQuery_ok);
	env.SetValue("XGrabDialog.fPname", fPname);
	env.SetValue("XGrabDialog.fPext", fPext);
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void XGrabDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fFixedSize = env.GetValue("XGrabDialog.fFixedSize", 1);
	fAlign = env.GetValue("XGrabDialog.fAlign", 22);
	fDelay = env.GetValue("XGrabDialog.fDelay", 1);
	fXwidth = env.GetValue("XGrabDialog.fXwidth", 100);
	fYwidth = env.GetValue("XGrabDialog.fYwidth", 100);
	fSerNr = env.GetValue("XGrabDialog.fSerNr", 0);
	fQuery_ok = env.GetValue("XGrabDialog.fQuery_ok", 1);
	fPname = env.GetValue("XGrabDialog.fPname", "pict_");
	fPext   = env.GetValue("XGrabDialog.fPext", "gif");
}
//_________________________________________________________________________

XGrabDialog::~XGrabDialog()
{
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

void XGrabDialog::CloseDialog()
{
//   cout << "XGrabDialog::CloseDialog() " << endl;
	if (fDialog) fDialog->CloseWindowExt();
	fDialog = NULL;
	delete this;
}
//_________________________________________________________________________

void XGrabDialog::CloseDown(Int_t wid)
{
//   cout << "XGrabDialog::CloseDown()" << endl;
	if (wid != -2) SaveDefaults();
	delete this;
}


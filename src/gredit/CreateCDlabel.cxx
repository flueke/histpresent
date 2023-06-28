#include "TROOT.h"
#include "TRootCanvas.h"
#include "TFile.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TArc.h"
#include "TPolyLine.h"
#include "TStyle.h"
#include "TSystem.h"
#include <TVirtualX.h>
#include "GEdit.h"
#include "HprImage.h"
#include "Save2FileDialog.h"
#include "CreateCDlabel.h"

using std::cout;
using std::endl;

ClassImp (CreateCDlabel)
ClassImp (CDisc)

CDisc::CDisc(Double_t x, Double_t y, Double_t r, Double_t ratio)
{
	fInnerRadiusRatio = ratio;
	SetX1(x);
	SetY1(y);
	SetR1(r);
	SetR2(r);
	SetFillStyle(0);
	SetFillColor(0);
	fInnerArc = new TArc(x,y, r * ratio);
	fInnerArc->SetFillStyle(1001);
	fInnerArc->SetFillColor(0);
}

void CDisc::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	THprArc::ExecuteEvent(event, px, py);
	fInnerArc->SetX1(GetX1());
	fInnerArc->SetY1(GetY1());
	fInnerArc->SetR1(GetR1() * fInnerRadiusRatio);
	fInnerArc->SetR2(GetR1() * fInnerRadiusRatio);
}
void CDisc::Paint(const Option_t *opt)
{
//	cout << "Paint " << endl;
	fInnerArc->SetX1(GetX1());
	fInnerArc->SetY1(GetY1());
	fInnerArc->SetR1(GetR1() * fInnerRadiusRatio);
	fInnerArc->SetR2(GetR1() * fInnerRadiusRatio);
	TArc::Paint(opt);
	fInnerArc->Paint(opt);
}

CreateCDlabel::CreateCDlabel()
{
   const char * helptext =
"Insertion of a Background Image:\n\
\n\
A background image may be extracted from an existing\n\
gif, jpg, png file (\"Background Imagefile\") and inserted\n\
This is done in 3 steps:\n\
\"Select BG Image\" shows the selected file and overlays\n\
a circle with a inner hole representing a disc. The radius can\n\
be changed with the mouse at 0 90, 180 270 degree positions\n\
on the circle (a cursor like this ->| should appear) and shifted \n\
by picking it at other positions(a hand like cursor) \n\
The enclosing square portion of the image gets selected with this.\n\
\"Create BG Image\" extracts it and writes it into new file\n\
with a new name (\"_square\"). This also sets \"Background Imagefile\"\n\
to this name.\n\
\"Insert BG Image\" puts the image into the project.\n\
If the \"Background Imagefile\" has already the correct (quadratic)\n\
size it can be inserted directly\n\
\n\
";
	gROOT->GetListOfCleanups()->Add(this);
	TList *row_lab = new TList();
	static void *valp[500];
	Int_t ind = 0;
//	static Int_t dummy = 0;
   static TString cdlcmd("NewCDlabel()");
   static TString porcmd("NewPortrait()");
   static TString lancmd("NewLandscape()");
   static TString usecmd("NewUserDef()");
   static TString setcmd("SetName()");
   static TString clscmd("CloseProject()");
   static TString getcmd("ExecuteGetProject()");
   static TString selcmd("SelectBackgroundPicture()");
   static TString crecmd("CreateBackgroundPicture()");
   static TString inscmd("InsertBackgroundPicture()");
   static TString stocmd("ExecuteStoreProject()");
   static TString mpscmd("MakePS()");
   static TString cdacmd("CoverDeadArea()");
   static TString exicmd("Leave()");
	
	TString * text = NULL;
//   fXRange = fYRange = 120;
//	fPaperSize = 122.;    // 122 mm
	fDrawCircles = 1;
	fCanvas = NULL;
	fImage = NULL;
	RestoreDefaults();
	if ( fSmallInnerHole == 1 ) {
		fInnerRadius = 12;
	} else {
		fInnerRadius = 21;
	}
	fPsSuffix = ".ps";
	fCanvasFromFile = fRootFileName;
	fCanvasFromFile += "|GrCanvas|pic";
	Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
	Int_t ok = 0;
	row_lab->Add(new TObjString("StringValue+Name"));
	valp[ind++] = &fProjectName;
	row_lab->Add(new TObjString("CommandButt+Set Name"));
	valp[ind++] = &setcmd;
	row_lab->Add(new TObjString("CommandButt_CD Label"));
	valp[ind++] = &cdlcmd;
	row_lab->Add(new TObjString("CommandButt+A4 portrait"));
	valp[ind++] = &porcmd;
	row_lab->Add(new TObjString("CommandButt+A4 Landscape"));
	valp[ind++] = &lancmd;
	row_lab->Add(new TObjString("CommandButt_User def"));
	valp[ind++] = &usecmd;
	row_lab->Add(new TObjString("PlainIntVal+WinX"));
	valp[ind++] = &fUserWindowX;
	row_lab->Add(new TObjString("PlainIntVal+WinY"));
	valp[ind++] = &fUserWindowY;
	row_lab->Add(new TObjString("DoubleValue+RangeX"));
	valp[ind++] = &fUserRX;
	
	row_lab->Add(new TObjString("RadioButton_CD Small Inner Hole  "));	
	valp[ind++] = &fSmallInnerHole;
	row_lab->Add(new TObjString("RadioButton+CD Big Inner Hole    "));	
	valp[ind++] = &fBigInnerHole;
	row_lab->Add(new TObjString("FileRequest_Background Imagefile"));
	fSelectBgIBid = ind;
	valp[ind++] = &fBGImage;
	row_lab->Add(new TObjString("CommandButt_Select BG Image"));
	valp[ind++] = &selcmd;
	row_lab->Add(new TObjString("CommandButt+Create BG Image"));
	valp[ind++] = &crecmd;
	row_lab->Add(new TObjString("CommandButt+Insert BG Image"));
	valp[ind++] = &inscmd;
	
//	row_lab->Add(new TObjString("DoubleValue+Paper Size[mm]"));	
//	valp[ind++] = &fPaperSize;
//	row_lab->Add(new TObjString("CheckButton+Draw Circle"));	
//	valp[ind++] = &fDrawCircles;
	
	row_lab->Add(new TObjString("FileContReq_Project from rootfile"));
	fLoadFileBid = ind;
	valp[ind++] = &fCanvasFromFile;
	row_lab->Add(new TObjString("CommandButt_Get Project"));
	valp[ind++] = &getcmd;
	row_lab->Add(new TObjString("CommandButt+Store Project"));
	valp[ind++] = &stocmd;
	row_lab->Add(new TObjString("CommandButt+Cover unused area"));
	valp[ind++] = &cdacmd;
	row_lab->Add(new TObjString("CommandButt_MakePS"));
	valp[ind++] = &mpscmd;
	row_lab->Add(new TObjString("CommandButt+Close Project"));
	valp[ind++] = &clscmd;
	row_lab->Add(new TObjString("CommandButt+Exit"));
	valp[ind++] = &exicmd;
//	fDialog =
	new TGMrbValuesAndText ("CreateCDlabel.cxx", text, &ok, itemwidth,
							NULL, NULL, NULL, row_lab, valp,
							NULL, NULL, helptext, this, "CreateCDlabel");
}
//_______________________________________________________________________

void CreateCDlabel::Leave()
{
	SaveDefaults();
	cout << "exit CreateCDlabel" << endl;
	gROOT->ProcessLine(".q");
}
//_______________________________________________________________________

void CreateCDlabel::RecursiveRemove(TObject * obj)
{
	if (obj == fCanvas) {
		cout << " CloseProject"  << endl;
		fCanvas = NULL;
		SaveDefaults();
	}
}
//_____________________________________________________________________

void CreateCDlabel::MakePS()
{
//	cout << "MakePS" << endl;
//  gStyle->SetPaperSize(12, 12);
	fEditor->RemoveEditGrid();
   gStyle->SetPaperSize(0.1 * fPaperSizeX, 0.1 * fPaperSizeY);
	// ps file has a .25 0.25 scale
	gStyle->SetHeaderPS("-64 -96 translate");
   TString pn;
   pn = fProjectName;
	pn += fPsSuffix;
	fCanvas->SaveAs(pn);
// 	
	TString dcmd("sed  \"1d \" ");
	TString acmd("sed -i \"1i %!PS-Adobe-2.0 \" ");
	dcmd += pn;
	dcmd += " -i";
	acmd += pn;
	gSystem->Exec(dcmd);
	gSystem->Exec(acmd);
};
//_____________________________________________________________________

void CreateCDlabel::CloseProject()
{
	if ( fCanvas != NULL ){
		fCanvas->Close();
		fCanvas = NULL;
		SaveDefaults();
	}
}
//_____________________________________________________________________

void CreateCDlabel::NewCDlabel()
{
	NewProject(604, 628, 120, 120);
	fPaperSizeX = 120.; 
	fPaperSizeY = 120.; 
	fPsSuffix = ".ps";
}
//_____________________________________________________________________

void CreateCDlabel::NewPortrait()
{
	NewProject(646, 936, 210, 297);
	fPaperSizeX = 210.; 
	fPaperSizeY = 297.; 
	fPsSuffix = ".eps";
}
//_____________________________________________________________________

void CreateCDlabel::NewLandscape()
{
	NewProject(936, 646, 297, 210);
	fPaperSizeX = 297; 
	fPaperSizeY = 210; 
	fPsSuffix = ".eps";
}
//_____________________________________________________________________

void CreateCDlabel::NewUserDef()
{
	Double_t userRY = fUserRX * (Double_t)(fUserWindowY) / (Double_t)(fUserWindowX);
	NewProject(fUserWindowX+4, fUserWindowY+28, fUserRX,  userRY);
	fPaperSizeX = 297; 
	fPaperSizeY = 210; 
	fPsSuffix = ".eps";
}
//_____________________________________________________________________

void CreateCDlabel::NewProject(Int_t ww, Int_t wh, Double_t xr, Double_t yr)
{
   gROOT->SetStyle("Plain");
	if ( fCanvas != NULL ) {
		cout << "NewProject: Please close current project first"  << endl;
		return;
	}
   fXRange = xr;
	fYRange = yr;
//	Int_t window_width  = 604; // 2x2pixels decoration
//	Int_t window_height = 628; // 2x2 pixels frame + 24 decoration at top
	
   fCanvas = new GrCanvas(fProjectName, fProjectName, 420, 500, ww, wh);
	fCanvas->Range(0,0,fXRange,fYRange);
	fCanvas->SetRightMargin(0);
	fCanvas->SetLeftMargin(0);
	fCanvas->SetBottomMargin(0);
	fCanvas->SetTopMargin(0);
	fCanvas->SetEditable(kTRUE);
   fCanvas->SetBit(GrCanvas::kIsAEditorPage);
	fEditor = new GEdit(fCanvas);
	fCanvas->GetCanvasImp()->ShowEditor();
}
//________________________________________________________________________________
void CreateCDlabel::SelectBackgroundPicture()
{
	fImage = TImage::Open(fBGImage);
   Int_t iw = fImage->GetWidth();
		Int_t ih = fImage->GetHeight();
//	cout << "iw, ih " << iw << " " << ih << endl;
	fRedFac = 1;
	if (iw > 1400) {
		fRedFac = 2;
//		cout << "iw, ih, redf " << iw << " " << ih << " " << fRedFac <<  endl;
		iw /= fRedFac;
		ih /= fRedFac;
		fImage->Scale(iw,ih);
	}
   UInt_t ww, wh;
   Int_t screen_x, screen_y;
//  screen size in pixels
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),screen_x, screen_y, ww, wh);
	Int_t iwx = ww - iw - 10;
//	Int_t iwy = wh - ih + 34;
	fImgCanvas = new GrCanvas("ImgCanvas", "ImgCanvas", iwx, 20, iw+4, ih+28);
	fImgCanvas->Range(0, 0, Double_t(iw), Double_t(ih));
	fImage->Draw("xxx");
	Double_t xcd = iw / 2;
	Double_t ycd = ih / 2;
	Double_t rcd = TMath::Min(xcd, ycd);;
	fCDP = new CDisc(xcd, ycd, rcd, fInnerRadius/fXRange);
	fCDP->Draw();
   gPad->Modified();
   gPad->Update();
}
//________________________________________________________________________________
void CreateCDlabel::CreateBackgroundPicture()
{
	if ( fImage == NULL ) {
		cout <<"No image selected yet" << endl;
		return;
	}
	TString newname(fBGImage);
	TString bn = gSystem->BaseName(newname);
	newname.Resize(newname.Index(bn));
	Int_t idot = bn.Index(".");
	bn.Replace(idot, 1, "_square.");
	newname += bn;
	cout <<"Create: " << newname << endl;
	Int_t xoff = (Int_t)(fCDP->GetX1() - fCDP->GetR1()); 
	Double_t yy = (fImgCanvas->GetUymax() - fCDP->GetY1());
	Int_t yoff = (Int_t)(yy - fCDP->GetR1()); 
	Int_t size = 2 * (Int_t)fCDP->GetR1();
	fImage->Crop(xoff, yoff, size,size);
	fCDP->SetX1(fCDP->GetR1());
	fCDP->SetY1(fImgCanvas->GetUymax() - fCDP->GetR1());
	
   gPad->Modified();
   gPad->Update();
   fImage->WriteImage(newname);
	fBGImage=newname;
}
//________________________________________________________________________________
void CreateCDlabel::InsertBackgroundPicture()
{
	if (fCanvas == NULL) {
		cout << "Please create or get a project first" << endl;
		return;
	}
	fCanvas->cd();
//	TPad * pad = (TPad*)fCanvas;
//   HprImage * hprimg = new HprImage(fBGImage, fCanvas);
   fImage = TImage::Open(fBGImage);
   if (!fImage) {
      cout << "Could not get the image... exit" << endl;
      return;
   }
//	cout << "Image: iw, ih: " << fImage->GetWidth() << " " << fImage->GetHeight() << endl;
   fImage->SetConstRatio(kTRUE);
   fImage->SetEditable(kTRUE);
   fImage->SetImageQuality(TAttImage::kImgBest);
   fImage->Draw("xxx");
   gPad->Modified();
   gPad->Update();
}
//________________________________________________________________________________

void CreateCDlabel::CoverDeadArea()
{
	fCanvas->cd();
   Double_t xc = 0.5 * fXRange;
   Double_t yc = 0.5 * fYRange;
	if ( fSmallInnerHole == 1 ) {
		fInnerRadius = 12;
	} else {
		fInnerRadius = 21;
	}
	fOuterRadius = 57.5;
	
	TArc *arc2 = new TArc(xc, yc, fInnerRadius);
	if (fDrawCircles > 0) {
	   arc2->Draw();
		arc2->SetFillColor(0);
		arc2->SetLineColor(0);
		arc2->SetLineWidth(0);
	}
// closed polyline	
	Int_t nsteps = 360 / 5;
	Double_t step = 2 * TMath::Pi() / nsteps;
	Double_t ang = 0; 
	Double_t x, y;
	TPolyLine * pl = new TPolyLine(nsteps + 6);
	for (Int_t i = 0; i <= nsteps; i++) {
	   x = xc + fOuterRadius * TMath::Cos(ang);
		y = yc + fOuterRadius * TMath::Sin(ang);
		pl->SetPoint(i, x, y);
		ang += step;
	}
	Double_t margin = 2.5;
	nsteps++;
	pl->SetPoint(nsteps++, xc + fOuterRadius + margin, yc);
	pl->SetPoint(nsteps++ , xc + fOuterRadius + margin, yc + 0.5*fYRange +margin);
	pl->SetPoint(nsteps++ , 0 , yc + 0.5*fYRange + margin);
	pl->SetPoint(nsteps++ , 0 , yc -( 0.5*fYRange + margin));
	pl->SetPoint(nsteps++ , xc + fOuterRadius + margin , yc -( 0.5*fYRange + margin));
	pl->SetPoint(nsteps++ , xc + fOuterRadius + margin, yc);
	pl->SetFillColor(0);
	pl->SetLineColor(0);
   pl->SetLineWidth(0);
	if (fDrawCircles > 0) {
		pl->Draw("F");
		cout << "fs " << pl->GetFillStyle() << endl;
	}
	fCanvas->Modified();
	fCanvas->Update();
}
//________________________________________________________________________________

void CreateCDlabel::ExecuteGetProject()
{
//   TString fname;
	if ( fCanvas != NULL ) {
		cout << "ExecuteGetProject: Please close current project first"  << endl;
		return;
	}
   TString cname;
   TString oname;
   cout << "ExecuteGetProject " << fCanvasFromFile << endl;
	TObjArray * oa = fCanvasFromFile.Tokenize("|");
	Int_t nent = oa->GetEntries();
   if (nent < 3) {
      cout << "fCanvasFromFile not enough (3) fields" << endl;
      return;
   }
	fRootFileName =((TObjString*)oa->At(0))->String();
	cname =((TObjString*)oa->At(1))->String();
	oname =((TObjString*)oa->At(2))->String();
   if (cname != "GrCanvas") {
      cout << "fCanvasFromFile: " << oname << " is not a TCanvas " << endl;
      return;
   }
   TFile f(fRootFileName);
   f.GetObject(oname,fCanvas);
   if (fCanvas == NULL) {
      cout << "No canvas found / selected" << endl;
      return;
   }
	fProjectName = fCanvas->GetName();
   fCanvas->Draw();
	fCanvas->GetCanvasImp()->ForceUpdate();
	gSystem->ProcessEvents();
   fCanvas->SetEditable(kTRUE);
	fCanvas->SetBit(GrCanvas::kIsAEditorPage);
	//   c1->GetCanvasImp()->ShowToolBar();
   fEditor = new GEdit(fCanvas);
	fCanvas->GetCanvasImp()->ForceUpdate();
	gSystem->ProcessEvents();
   fCanvas->GetCanvasImp()->ShowEditor();
//	fCanvas->SetCanvasSize(cw, ch);
//	fCanvas->SetWindowSize(ww,wh);
	fXRange = fCanvas->GetUxmax();
	fYRange = fCanvas->GetUymax();
	fPaperSizeX = fXRange;
	fPaperSizeY = fYRange;
//	if ( fPaperSizeX == 120 && fPaperSizeY == 120 ) {
//		fPaperSizeX = fPaperSizeY = 122;
//	}
	cout << "fPaperSizeX, fPaperSizeY " <<fPaperSizeX << " "<< fPaperSizeY<< endl;
}
//________________________________________________________________________________

void CreateCDlabel::ExecuteStoreProject()
{
   if (fCanvas == NULL) {
      cout << "No canvas found / selected" << endl;
      return;
   }

	fEditor->ShowToolBar(kFALSE);
	fCanvas->GetCanvasImp()->ShowEditor(kFALSE);
	fCanvas->GetCanvasImp()->ForceUpdate();
	gSystem->ProcessEvents();
	fCanvas->Pop();
	
   Save2FileDialog sfd(fCanvas, NULL, (TRootCanvas*)fCanvas->GetCanvasImp());
//	fCanvas->GetCanvasImp()->ShowEditor();
	fEditor->ShowToolBar(kTRUE);
	fCanvas->GetCanvasImp()->ShowEditor();
	fCanvas->GetCanvasImp()->ForceUpdate();
	gSystem->ProcessEvents();
}
	
//________________________________________________________________________________

void CreateCDlabel::SetName()
{
		cout << "Set ProjectName to: " << fProjectName << endl;
		if (fCanvas != NULL) {
			fCanvas->SetName(fProjectName);
		}
}
	
//______________________________________________________________________

void CreateCDlabel::CRButtonPressed(Int_t wid, Int_t bid, TObject* obj)
{
//	cout << "Bid " << bid << endl;
	if ( bid == fLoadFileBid ) {
		if (wid) {};
		if (obj) {};
//		cout << "fLoadFileBid " << endl;
	}
//	if ( bid == fSelectBgIBid ) {
//		SelectBackgroundPicture();
//	}
};
//____________________________________________________________________________

void CreateCDlabel::SaveDefaults()
{
//   cout << "CreateCDlabel::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("CreateCDlabel.fRootFileName",   fRootFileName  );
   env.SetValue("CreateCDlabel.fBigInnerHole",   fBigInnerHole  );
   env.SetValue("CreateCDlabel.fSmallInnerHole", fSmallInnerHole);
   env.SetValue("CreateCDlabel.fProjectName",    fProjectName   );
   env.SetValue("CreateCDlabel.fBGImage",        fBGImage       );
   env.SetValue("CreateCDlabel.fUserRX",      fUserRX     );
   env.SetValue("CreateCDlabel.fUserWindowX", fUserWindowX);
   env.SetValue("CreateCDlabel.fUserWindowY", fUserWindowY);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void CreateCDlabel::RestoreDefaults()
{
   TEnv env(".hprrc");
   fRootFileName   = env.GetValue("CreateCDlabel.fRootFileName", "w5.root");
   fBigInnerHole   = env.GetValue("CreateCDlabel.fBigInnerHole",   0);
   fSmallInnerHole = env.GetValue("CreateCDlabel.fSmallInnerHole", 1);
   fProjectName    = env.GetValue("CreateCDlabel.fProjectName",    "CDlabel");
   fBGImage        = env.GetValue("CreateCDlabel.fBGImage",        "agv.gif");
   fUserRX         = env.GetValue("CreateCDlabel.fUserRX",          100.);
   fUserWindowX    = env.GetValue("CreateCDlabel.fUserWindowX",     500);
   fUserWindowY    = env.GetValue("CreateCDlabel.fUserWindowY",     500);
	
}

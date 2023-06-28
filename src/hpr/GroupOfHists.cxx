#include <TEnv.h>
#include <TH1.h>
#include <TString.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TGMsgBox.h>
#include "TGMrbInputDialog.h"
#include "support.h"
#include "hprbase.h"
#include "SetColor.h"
#include "GroupOfHists.h"
#include "Set1DimOptDialog.h"
#include "SetCanvasAttDialog.h"
#include "SetHistOptDialog.h"
#include "Set2DimOptDialog.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"

ClassImp (GoHTimer)
ClassImp (GroupOfHists)

GoHTimer::GoHTimer(Long_t ms, Bool_t synch, GroupOfHists * goh):TTimer(ms,synch){
	  cout << "init GoHTimer" << endl;
	fGoH = goh;
	gSystem->AddTimer(this);
}

Bool_t GoHTimer::Notify() {
	fGoH->UpdateHists();
	Reset();
	return kTRUE;
};


enum EGoHCommandIds {
	M_AllAsSel,
	M_RebinAll,
	M_CalAllAsSel,
	M_CommonRotate,
	M_Redraw,
	M_AllAsFirst,
	M_SetOptions,
	M_AllAsSelRangeOnly,
	M_ActivateTimer,
	M_OptionCanvasAtt,
	M_OptionAxisAtt,
	M_Option1Dim,
	M_Option2Dim,
	M_Option2DimCol
};
//________________________________________________________________________

GroupOfHists::GroupOfHists()
{
	fCanvas = NULL;
	fTimer = NULL;
	fDialog = NULL;
}
//________________________________________________________________________

GroupOfHists::GroupOfHists(TList * hlist, HistPresent * hpr, const Char_t */*title*/)
				: fHList(hlist), fHistPresent(hpr)
{
	if (gHprDebug > 0) {
   cout << " ctor GroupOfHists::" << this << endl;
//   gHpr->CloseAllCanvases();
	}
	// Close canvases which may contain a selected histogram
	TIter next1(hlist);
	while ( TObjString * objs = (TObjString*)next1()) {
		// extract histname
		TString s = objs->String();
		if (gHprDebug > 0) cout << "Selected: " << s << endl;
		Int_t ik = s.Index(",");
		if (ik<0) continue;
		s = s(ik+1,s.Length());
		ik = s.Index(",");
		Int_t is = s.Index(";");
		if (is >= 0 && is < ik) ik=is;
		s = s(0,ik);
//		s.Prepend("_");
//		s.Append("_");
		if (gHprDebug > 0) cout << " Look for " << s << endl;
		TCanvas *cc = (TCanvas*)Hpr::FindObjectByWildcard(s.Data(), (TList*)gROOT->GetListOfCanvases());
		if (cc) {
			if (gHprDebug > 0)
				cout << "Deleting: " << cc->GetName() << endl;
			TRootCanvas *rc = (TRootCanvas*)cc->GetCanvasImp();
			rc->SendCloseMessage();
		}
	}

	fCanvas = NULL;
	fTimer = NULL;
	fDialog = NULL;
	fNh1Dim = fNh2Dim = 0;
	fDisplayLowX = fDisplayLowY = fDisplayUpX = fDisplayUpY = 0;
	fAnyFromSocket = kFALSE;
	RestoreDefaults();
	SetColorModeDialog::RestoreDefaults();
	SetColorModeDialog::SetColorMode();
//
	Int_t nsel = fHList->GetSize();
	fNx = 1;
	fNy =1;
//   TH1* hist = 0;
	if (nsel == 0) {
		cout << setred << "No histogram selected" << setblack << endl;
		return;

	} else if (nsel > 32) {
		cout << setred << "Maximum 32 histograms allowed" << setblack << endl;
		return;
	}

//  make a copy of  hlist, add its pointer to list of histlists fHistListList
//  pass its pointer to HTCanvas,
//  destructor of HTCanvas shall delete the list and remove its
//  pointer from fHistListList
	TIter next(fHList);
	while ( TObjString * objs = (TObjString*)next()) {
		fHistList.Add(new TObjString(*objs));
		if (gHprDebug > 0) {
			cout << objs->String() << endl;
		}
	}
	if (nsel == 2 ) {fNx = 1, fNy = 2;};
	if (nsel >= 3 ) {fNx = 2, fNy = 2;};
	if (nsel >= 5 ) {fNx = 2, fNy = 3;};
	if (nsel >= 7 ) {fNx = 3, fNy = 3;};
	if (nsel >= 10) {fNx = 3, fNy = 4;};
	if (nsel >= 13) {fNx = 4, fNy = 4;};

	if (nsel >= 16) {fNx = 4, fNy = 5;};
	if (nsel == 21) {fNx = 7, fNy = 3;};
	if (nsel >  21) {fNx = 5, fNy = 5;};

	if (nsel > 25) {fNx = 6, fNy = 5;};
	if (nsel == 32) {fNx = 8, fNy = 4;};

	if (fArrangeOnTop)      {fNx = 1; fNy = nsel;};
	if (fArrangeSideBySide) {fNx =  nsel; fNy = 1;};
	TString buf("cmany_");
	buf += fHistPresent->fSeqNumberMany++;
//   const char * tit = buf.Data();
//   if (title) tit = title;
	buf.Prepend("GroupOfHists_");
	this->SetName(buf.Data());
//   this->SetTitle(tit);
	BuildCanvas();
	BuildMenu();
	gROOT->GetList()->Add(this);
	gROOT->GetListOfCleanups()->Add(this);
	if (gHprDebug >0)
		cout << "GroupOfHists *goh = (GroupOfHists*)" << this << ";" << endl;
}
//________________________________________________________________________

GroupOfHists::~GroupOfHists()
{
	if (gHprDebug > 0)
		cout <<"dtor GroupOfHists, fDialog " << fDialog << endl;
	gROOT->GetList()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
	if (fTimer) delete fTimer;
	if (fDialog) {
		fDialog->CloseWindowExt();
		SaveDefaults();
	}
}
//________________________________________________________________________

void GroupOfHists::BuildCanvas()
{
	Int_t nsel = fHList->GetSize();
	gStyle->SetPadLeftMargin(fPadLeftMargin);
	gStyle->SetPadRightMargin(fPadRightMargin);
	gStyle->SetPadBottomMargin(fPadBottomMargin);
	gStyle->SetPadTopMargin(fPadTopMargin);
	if ( fCanvas  == NULL ) {
		fCanvas =  new HTCanvas(GetName(), "GroupOfHists",
				  WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury, fWindowXWidth, fWindowYWidth);

		 fCanvas->SetBit(kMustCleanup);

		 WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx;
		 WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;

//       fHistPresent->fCanvasList->Add(fCanvas);
	} else {
		fCanvas->Clear();
	}

	Double_t mx, my;
	if (fNoSpace ) {
		mx = -1;
		my = -1;
	} else {
		mx = 0.00001;
		my = 0.00001;
	}
	fCanvas->Divide(fNx, fNy, mx, my);
//   fCanvas->SetEditable(kTRUE);
	TEnv * lastset = 0;
	TString hname;
	TString fname;
	fAnyFromSocket = kFALSE;
	fPadList = new TList();
	TPad * firstpad = NULL;
	TH1* hist;
	for(Int_t i=0; i < nsel; i++) {
		fCanvas->cd(i+1);
		TPad * p = (TPad *)gPad;
		fPadList->Add(p);
		TString cmd2("((GroupOfHists*)gROOT->GetList()->FindObject(\"");
		cmd2 += GetName();
		cmd2 += "\"))->auto_exec()";
		p->AddExec("ex2", cmd2.Data());
		if (firstpad == NULL) firstpad = p;
		TString temp = ((TObjString *)fHList->At(i))->String();
		hist = fHistPresent->GetSelHistAt(i, fHList);
		if (!hist) {
         cout<< setred << " Hist not found: " << temp << setblack << endl;
			continue;
		}
//		TString temp = ((TObjString *)fHList->At(i))->String();
		if (temp.Index("Socket") == 0) fAnyFromSocket = kTRUE;
		fname = temp;
//		cout << "temp: " << temp << endl;
		Int_t pp = fname.Index(",");
		if (pp < 0) {
			pp = fname.Index(" ");
			if ( pp <= 0 ) {
				cout << "Cant get file name from: " << temp << endl;
				return;
			}
		}
		fname.Resize(pp);
//		cout << "fname: " << fname << endl;
		hname = temp(pp+1, temp.Length());
		if (hname.Length() <= 0) {
			cout << "Cant get hist name from: " << temp << endl;
			return;
		}
//		cout << "hname: " << hname << endl;
		pp = hname.Index(",");
		if (pp < 0) {
			pp = fname.Index(" ");
			if ( pp > 0 ) {
				hname.Resize(pp);
			}
		} else {
			hname.Resize(pp);
		}
//		cout << "hname: " << hname << endl;
		
//      hname = hist->GetName();
		if (gHprDebug > 0)
		cout << "In GroupOfHists::BuildCanvas: " << fname<< " " << hist << " " << hname << endl;

//      Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
//      if(last_us >0)hname.Remove(last_us);
		if (hname.Index(";") > 1) {
			TString sind = hname(hname.Index(";")+1, 10);
			if (sind.IsDigit() && sind.Atoi() == 1) {
				hname.Resize(hname.Index(";"));
			}
		}
		TString hname_temp(hname);
		if ( fname.Index("Socket") < 0 && fname.Index("Memory") < 0
			&& GeneralAttDialog::fPrependFilenameToName ) {
			fname = gSystem->BaseName(fname);
			if ( fname.EndsWith("root") )
				fname.Resize(fname.Length() - 5);
			hname.Prepend("_");
			hname.Prepend(fname);
		}
		if ( gROOT->GetList()->FindObject(hname) ) {
			hname += "_";
			hname += i;
		}
//      TRegexp sem(";");
//      hname(sem) ="_";
		hist->SetName(hname);
//		cout << "Aft chop: " << hname << endl;
		lastset = GetDefaults(hname_temp);
		if (gHprDebug > 0)
			cout << "GetDefaults: "  << hname << " " << lastset << endl;
		if (lastset) {
			if (gHprDebug > 0)
				lastset->Print();
			if (lastset->Lookup("fRangeLowX") )
				hist->GetXaxis()->Set(hist->GetNbinsX(),
				lastset->GetValue("fRangeLowX", 0),
				lastset->GetValue("fRangeUpX",  0));
			if (lastset->Lookup("fBinlx") )
			{
				hist->GetXaxis()->SetRange(
				lastset->GetValue("fBinlx", 0),
				lastset->GetValue("fBinux", 0));
			}
			if (lastset->Lookup("fXtitle") )
			  hist->GetXaxis()->SetTitle(lastset->GetValue("fXtitle",""));
			if (lastset->Lookup("fYtitle") )
			  hist->GetYaxis()->SetTitle(lastset->GetValue("fYtitle",""));
		}
// global settings overwrite individual settings
		if (fDisplayLowX != 0 || fDisplayUpX != 0){
			hist->GetXaxis()->Set(hist->GetNbinsX(),fDisplayLowX, fDisplayUpX);
		}
		if (fDisplayLogY != 0) {
			p->SetLogy();
		}
		if (fDisplayLogX != 0) {
			p->SetLogx();
		}
		if (fDisplayLogZ != 0) {
			p->SetLogz();
		}
		if (hist->GetDimension() == 2) {
			if (fTwoDimLogY != 0) {
				p->SetLogy();
			}
			if (fTwoDimLogX != 0) {
				p->SetLogx();
			}
			if (fTwoDimLogZ != 0) {
				p->SetLogz();
			}
			fNh2Dim++;
			hist->SetStats(fShowStatBox2Dim);
			if (fShowStatBox2Dim)
				gStyle->SetOptStat(fOptStat2Dim);
			hist->Draw(fDrawOpt2Dim);
			hist->SetLineWidth(fHistLineWidth2Dim);
			hist->SetLineColor(fHistLineColor2Dim);
			if (lastset) {
//				if (lastset->GetValue("LogZ", 0) )p->SetLogz();
				if (lastset->Lookup("fRangeLowY") )
					hist->GetXaxis()->Set(hist->GetNbinsY(),
					lastset->GetValue("fRangeLowY", 0),
					lastset->GetValue("fRangeUpY",  0));
				if (lastset->Lookup("fBinly") )
				{
					hist->GetYaxis()->SetRange(
					lastset->GetValue("fBinly", 0),
					lastset->GetValue("fBinuy", 0));
				}
			}
		} else {
			if (fDisplayLogY != 0) {
				p->SetLogy();
			}
			if (fDisplayLogX != 0) {
				p->SetLogx();
			}
			if (fDisplayLogZ != 0) {
				p->SetLogz();
			}
			fNh1Dim++;
//			if (lastset && lastset->GetValue("LogY", 0) )p->SetLogy();
			hist->SetStats(fShowStatBox1Dim);
			if (fShowStatBox1Dim)
				gStyle->SetOptStat(fOptStat1Dim);
			TString drawopt;
//         hist->Draw(drawopt.Data());
//         gStyle->SetOptTitle(fHistPresent->GetShowTitle());
			if (fShowContour) drawopt = "hist";
			if (fErrorMode != "none")  drawopt += fErrorMode;
			hist->SetLineWidth(fHistLineWidth1Dim);
			hist->SetLineColor(fHistLineColor1Dim);
			if (fFill1Dim) {
				hist->SetFillStyle(fHistFillStyle);
				hist->SetFillColor(fHistFillColor);
				if ( gHprDebug > 0)
				cout << "fHistPresent->fHistFillStyle " << fHistFillStyle << endl;
			} else {
				hist->SetFillStyle(0);
			}
			hist->Draw(drawopt.Data());
		}
		TAxis * xa = hist->GetXaxis();
		TAxis * ya = hist->GetYaxis();
		TAxis * za = hist->GetZaxis();
		xa->SetLabelSize(gStyle->GetLabelSize("X") * fMagFac);
		ya->SetLabelSize(gStyle->GetLabelSize("Y") * fMagFac); // nb y det text size
		xa->SetTitleSize(gStyle->GetTitleSize("X") * fMagFac);
		ya->SetTitleSize(gStyle->GetTitleSize("Y") * fMagFac);
		if (za && hist->GetDimension() > 1) {
			za->SetLabelSize(gStyle->GetLabelSize("Z") * fMagFac);
			za->SetTitleSize(gStyle->GetTitleSize("Z") * fMagFac);
		}	
		if (fNx > 1)xa->SetNdivisions(205);
		if (fNy > 2)ya->SetNdivisions(205);
		hist->SetTitleSize(gStyle->GetTitleSize("C") * fMagFac);
		fHistPresent->SetCurrentHist(hist);
	}
	if (!fCanvas->GetAutoExec())
		 fCanvas->ToggleAutoExec();
	gPad->Modified(kTRUE);
	gPad->Update();
	if (firstpad) {
		if (fShowAllAsFirst != 0 )
			ShowAllAsSelected(firstpad, fCanvas, 0, NULL);
		firstpad->cd();
	}
	fCanvas->SetEditable(kTRUE);
	if (gHprDebug>0)
	cout << "Exit GroupOfHists::BuildCanvas: " << endl;
}
//________________________________________________________________________

void GroupOfHists::RecursiveRemove(TObject *obj)
{
	if (gHprDebug > 0) 
		cout <<  "GroupOfHists::RecursiveRemove,obj " << obj << " "  << obj->GetName() << endl;
	if (fCanvas && obj == fCanvas) {
		fWindowXWidth = fCanvas->GetWindowWidth();
		fWindowYWidth = fCanvas->GetWindowHeight();
		if (gHprDebug > 0) 
			cout <<  "GroupOfHists::RecursiveRemove,fWindowXWidth  " << fWindowXWidth << endl;
		delete this;
	}
}
//________________________________________________________________________

void GroupOfHists::ActivateTimer(Int_t delay)
{
	if(fTimer){
		cout << "Deleting existing timer" << endl;
		delete fTimer;
		fTimer = 0;
	}
	if(delay > 0) fTimer = new GoHTimer(delay, kTRUE, this);
}
//________________________________________________________________________

void GroupOfHists::UpdateHists()
{
	static Int_t nof_updates = 0;
	static Int_t maxwarn = 0;
	Int_t nhist = fHistList.GetSize();
	if(nhist <= 0)return;
	Int_t fx = 0, lx = 0, fy = 0, ly = 0;
	for(Int_t i=0; i<nhist; i++){
		fCanvas->cd(i+1);
// Get display ranges from currently drawn histo
		TH1 * hold  = GetTheHist(gPad);
		if ( hold ) {
			fx =  hold->GetXaxis()->GetFirst();
			lx =  hold->GetXaxis()->GetLast();
		}
		TH1 * hist = fHistPresent->GetSelHistAt(i,&fHistList, kFALSE, "_updated");
		if (!hist) {
			if (maxwarn <= 5) {
				TObjString * objs = (TObjString*)fHistList.At(i);
				cout << setred << "Cant get histogram: "
				<< objs->String() << " Is M_analyze stopped? " << endl;
				if ( maxwarn == 5 ) {
					cout << "No longer warn you" << endl;
				}
         if((objs->String()).Contains("Socket") 
				&& fTimer && fHistPresent->fComSocket == NULL){
				cout << "Deleting existing timer"<< setblack << endl;
            delete fTimer;
            fTimer = 0;
            return;
//            fHistPresent->fSocketIsOpen = kFALSE;
         }
				cout << setblack << endl;
			}
//			if ( hold ) {
//				delete hold;
//				hold = NULL;
//			}
			maxwarn++;
			continue;
		}
//     hist->Print();
		if(hist->GetDimension() == 2){
			hist->Draw(fDrawOpt2Dim);
			if ( hold ) {
				fy =  hold->GetYaxis()->GetFirst();
				ly =  hold->GetYaxis()->GetLast();
				hist->GetXaxis()->SetRange(fx, lx);
				hist->GetYaxis()->SetRange(fy, ly);
			}
		} else {
			TString drawopt;
			if(fShowContour)drawopt = "hist";
			if (fErrorMode != "none")  drawopt += fErrorMode;
			if (fFill1Dim) {
				hist->SetFillStyle(fHistFillStyle);
				hist->SetFillColor(fHistFillColor);
			} else {
				hist->SetFillStyle(0);
			}
			hist->Draw(drawopt.Data());
			if ( hold ) {
				hist->GetXaxis()->SetRange(fx, lx);
			}
		}
		if ( hold ) {
			delete hold;
			hold = NULL;
		}
	}
	nof_updates ++;
	if (nof_updates % 10 == 0)
		cout << "nof_updates " << nof_updates << endl;
	fCanvas->Modified(kTRUE);
	fCanvas->Update();
}
//________________________________________________________________________

void GroupOfHists::BuildMenu()
{
//   cout << "GroupOfHists::BuildMenu() " <<this << endl;
	fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
	TGMenuBar * menubar = fRootCanvas->GetMenuBar();
	TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
	fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
	menubar->AddPopup("Display", fMenu, layoh_left, menubar->GetPopup("Help"));
	fMenu->AddEntry("Show now all as selected, Range only", M_AllAsSelRangeOnly);
	fMenu->AddEntry("Show now all as selected, Range, Min, Max ",  M_AllAsSel);
//   fMenu->AddEntry("Calibrate all as selected",  M_CalAllAsSel);
//   fMenu->AddEntry("Show always all as First", M_AllAsFirst);
//   if (fShowAllAsFirst != 0) fMenu->CheckEntry(M_AllAsFirst);
//   else                 fMenu->UnCheckEntry(M_AllAsFirst);
	fMenu->AddEntry("Rebin all",  M_RebinAll);
	fMenu->AddSeparator();
	fMenu->AddEntry("Activate automatic update",  M_ActivateTimer);
	fMenu->Connect("Activated(Int_t)", "GroupOfHists", this,
						"HandleMenu(Int_t)");
	fMenu->AddEntry("Activate simultanous rotation", M_CommonRotate);
	fMenu->AddEntry("Redraw picture", M_Redraw);

	fOptionMenu     = new TGPopupMenu(fRootCanvas->GetParent());
	menubar->AddPopup("Options", fOptionMenu, layoh_left, menubar->GetPopup("Help"));
	fOptionMenu->AddEntry("Specific to this window", M_SetOptions);
	fOptionMenu->AddEntry("Axis / title statbox attributes", M_OptionAxisAtt);
	fOptionMenu->AddEntry("Canvas / pad attributes", M_OptionCanvasAtt);
	if (fNh1Dim > 0)
		fOptionMenu->AddEntry("How to display a 1-dim histogram", M_Option1Dim);
	if (fNh2Dim > 0) {
		fOptionMenu->AddEntry("How to display a 2-dim histogram ", M_Option2Dim);
		fOptionMenu->AddEntry("Color mode of 2-dim histogram", M_Option2DimCol);
	}
	fOptionMenu->AddEntry("Redraw picture", M_Redraw);
	fOptionMenu->Connect("Activated(Int_t)", "GroupOfHists", this,
								"HandleMenu(Int_t)");
	menubar->MapSubwindows();
	menubar->Layout();
}
//________________________________________________________________________

void GroupOfHists::HandleMenu(Int_t id)
{
	switch (id) {

		case M_ActivateTimer:
			if (fTimer != NULL) {
				ActivateTimer(-1);    // deactivate
				fMenu->UnCheckEntry(M_ActivateTimer);
			} else {
				Int_t tms = 0;
				tms = (Int_t)(1000 * fAutoUpdateDelay);
				if(tms <= 0) tms = 2000;
				cout << "Setting AutoUpdateDelay to " << tms << endl;
				ActivateTimer(tms);    // in milli second
				fMenu->CheckEntry(M_ActivateTimer);
			}
			break;
		case M_RebinAll:
			RebinAll(fCanvas);
			break;
		case M_AllAsSelRangeOnly:
			ShowAllAsSelected(gPad, fCanvas, 0,
						 (TGWindow*)fRootCanvas);
			break;
//      case M_CalAllAsSel:
//         CalibrateAllAsSelected(gPad, fHCanvas, 1);
//         break;

		case M_AllAsSel:
			ShowAllAsSelected(gPad, fCanvas, 1);
			break;
		case M_Redraw:
			BuildCanvas();
			break;
		case M_SetOptions:
			SetOptions();
			break;
		case M_CommonRotate:
			{
			if (fCommonRotate !=0) {
				fCommonRotate = 0;
				fMenu->UnCheckEntry(M_CommonRotate);
			} else {
				fCommonRotate = 1;
				fMenu->CheckEntry(M_CommonRotate);
			}
			TEnv env(".hprrc");		// inspect ROOT's environment
			env.SetValue("GroupOfHists.CommonRotate", fCommonRotate);
			env.SaveLevel(kEnvLocal);
			}
			break;
		case M_AllAsFirst:
			{
			if (fShowAllAsFirst !=0) {
				fShowAllAsFirst = 0;
				fMenu->UnCheckEntry(M_AllAsFirst);
			} else {
				fShowAllAsFirst = 1;
				fMenu->CheckEntry(M_AllAsFirst);
			}
			TEnv env(".hprrc");		// inspect ROOT's environment
			env.SetValue("GroupOfHists.ShowAllAsFirst", fShowAllAsFirst);
			env.SaveLevel(kEnvLocal);
			}
			break;

		case M_OptionAxisAtt:
				new SetHistOptDialog(fRootCanvas);
			break;
		case M_OptionCanvasAtt:
				new SetCanvasAttDialog(fRootCanvas);
			break;
		case M_Option1Dim:
				new Set1DimOptDialog(fRootCanvas);
			break;

		case M_Option2Dim:
				new Set2DimOptDialog(fRootCanvas);
			break;

		case M_Option2DimCol:
				new SetColorModeDialog(fRootCanvas);
			break;
	}
}
//________________________________________________________________________

void GroupOfHists::auto_exec()
{
	static Double_t phi;
	static Double_t theta;
	Int_t event = gPad->GetEvent();

//   cout << "event "<< event << endl;
	if (event != kButton1Down && event != kMouseMotion && event != kButton1Up) return;
	TObject *select = gPad->GetSelected();
	if(!select) return;
//   cout << "auto_exec_2() selected " << select->GetName() << endl;
	if(gPad == gPad->GetMother()){
	 cout << "not in divided" << endl;
		return;
	}
	if (select->InheritsFrom("TPad")) {
		TVirtualPad *pad = (TVirtualPad*)select;
		TH1 * hist = GetTheHist(pad);
		if (hist && hist->GetDimension() == 1 && event == kButton1Down) {
			if (fHistPresent) {
				TH1* hclone =  (TH1*)hist->Clone();
				TString na(hclone->GetName());
				Int_t indupd = na.Index("_updated");
				if ( indupd > 0 ) {
					na.Resize(indupd);
					hclone->SetName(na);
				}
				fHistPresent->ShowHist(hclone);
			}
		}
		return;
	}
	if (select->InheritsFrom("TH2")) {
		if (!fCommonRotate) {
			if(event == kButton1Down) {
				if (fHistPresent) {
					TH1 *hist = (TH1*)select;
					fHistPresent->ShowHist(hist);
				}
			}
			return;
		}
	}
	if (fCommonRotate) {
		TList * l = gPad->GetListOfPrimitives();
		TIter next(l);
		TObject * o;
		while ( (o = next()) ){
			if (o->InheritsFrom("TH2")) {
				TH1* h = (TH1*)o;
//            cout << "h->GetDrawOption() " << h->GetDrawOption()<< endl;
				TString dropt = h->GetDrawOption();

				if (dropt.Contains("LEGO",TString::kIgnoreCase) ||
					 dropt.Contains("SURF",TString::kIgnoreCase) ) {
					if (event == kButton1Down) {
						phi = gPad->GetPhi();
						theta = gPad->GetTheta();
					} else if (event == kButton1Up || event == kButton1Motion) {
						Double_t phi_n = gPad->GetPhi();
						Double_t theta_n = gPad->GetTheta();
						if (phi != phi_n || theta != theta_n) {
							TList * pl = gPad->GetMother()->GetListOfPrimitives();
							TIter nextpad(pl);
							TObject * p;
							while ( (p = nextpad()) ) {
								if (p->InheritsFrom("TPad")) {
									TPad* pp = (TPad*)p;
									pp->SetPhi(phi_n);
									pp->SetTheta(theta_n);
									pp->Modified();
									pp->Update();
								}
							}
						}
					}
				}
				return;
			}
		}
	}
}
//____________________________________________________________________________

TH1 * GroupOfHists::GetTheHist(TVirtualPad * pad)
{
	TList *l = pad->GetListOfPrimitives();
	TIter next(l);
	TObject *o;
	while ( (o = next()) ) {
		if (o->InheritsFrom("TH1"))
			return (TH1 *) o;
	}
	return NULL;
}
//_______________________________________________________________________________________

void GroupOfHists::ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, Int_t mode, TGWindow * win)
{
// find reference histogram

	TList *l = canvas->GetListOfPrimitives();
	TObject *obj;
	TH1 *href = GetTheHist(pad);
	if (!href) {
//      WarnBox("Selected pad contains no hist,\n please select with middle mouse");
		cout << "No pad selected, using first" << endl;
		TIter next(l);
		while ( (obj = next()) ) {
			if (obj->InheritsFrom(TPad::Class())) {
				TPad *p = (TPad *) obj;
				href = GetTheHist(p);
				if (href) {
					p->cd();
					break;
				}
			}
		}
	}
	if (!href) {
		WarnBox
			 ("Selected pad contains no hist,\n please select with middle mouse", win);
		return;
	}
//   cout << "ShowAllAsSelected " << href->GetName()<< endl;
	TAxis *ax = href->GetXaxis();
	Axis_t lowedgeX = ax->GetBinLowEdge(ax->GetFirst());
	Axis_t upedgeX = ax->GetBinLowEdge(ax->GetLast()) +
		 ax->GetBinWidth(ax->GetLast());
	Axis_t lowedgeY = 0, upedgeY = 0,  lowedgeZ = 0, upedgeZ = 0;
	if ( href->GetDimension() > 1 ) {
		ax = href->GetYaxis();
		lowedgeY = ax->GetBinLowEdge(ax->GetFirst());
		upedgeY = ax->GetBinLowEdge(ax->GetLast()) +
		ax->GetBinWidth(ax->GetLast());
	}	
	if ( href->GetDimension() > 2 ) {
		ax = href->GetZaxis();
		lowedgeZ = ax->GetBinLowEdge(ax->GetFirst());
		upedgeZ = ax->GetBinLowEdge(ax->GetLast()) +
		ax->GetBinWidth(ax->GetLast());
	}	
	Axis_t min = 0, max = 0;
	if (mode > 0) {
		min = href->GetMinimum();
		max = href->GetMaximum();
	}
	l = canvas->GetListOfPrimitives();
	TIter next(l);
	while ( (obj = next()) ) {
		if (obj->InheritsFrom(TPad::Class())) {
			TPad *p = (TPad *) obj;
			TH1 *hist = GetTheHist(p);
			if (hist) {
				hist->GetXaxis()->SetRangeUser(lowedgeX, upedgeX);
				if (hist->GetDimension() > 1 && (lowedgeY != 0 || upedgeY != 0) ) 
					hist->GetYaxis()->SetRangeUser(lowedgeY, upedgeY);
				if (hist->GetDimension() > 2&& (lowedgeZ != 0 || upedgeZ != 0) ) 
					hist->GetZaxis()->SetRangeUser(lowedgeZ, upedgeZ);
				p->SetLogy(pad->GetLogy());
				if (mode > 0 && pad->GetLogy() == 0) {
					hist->SetMinimum(min);
					hist->SetMaximum(max);
				}
				p->Modified(kTRUE);
				p->Update();
			}
		}
	}
	canvas->Modified(kTRUE);
	canvas->Update();
}
//_______________________________________________________________________________________

void GroupOfHists::RebinAll(TCanvas * canvas)
{

	static Int_t ngroup = 2;
	Bool_t ok;
	ngroup = GetInteger("Rebin value", ngroup, &ok);
	if (!ok || ngroup <= 0)
		return;
	TList *l = canvas->GetListOfPrimitives();
	TIter next(l);
	TObject *obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom(TPad::Class())) {
			TPad *p = (TPad *) obj;
			TH1 *hist = GetTheHist(p);
			if (hist) {
//            cout << hist->GetName()<< ", first, last, min, max  "
//            << first << " " << last  << " " << min << " " << max<< endl;
				Int_t first = hist->GetXaxis()->GetFirst();
				Int_t last = hist->GetXaxis()->GetLast();
				hist->Rebin(ngroup);
				first /= ngroup;
				last /= ngroup;
				hist->GetXaxis()->SetRange(first, last);
				p->Modified(kTRUE);
				p->Update();
			}
		}
	}
	canvas->Modified(kTRUE);
	canvas->Update();
}
//_______________________________________________________
void GroupOfHists::SetOptions()
{
static const Char_t helptext[] =
"This menu controls the parameters when multiple histograms are\n\
shown in one canvas.\n\
\"X - Y Width\":  Size of the main canvas in pixel\n\
The apparent distance between pads is determined / dominated by the\n\
values of the pad margins (\"Canvas, pad attributes\")\n\
With option \"No space between pads\" the pictures are clued together.\n\
Only labels  of the yaxis of the pads on the left side and the xaxis\n\
of the bottom pads are shown. The space reserved for that is\n\
determined by the values of RightMargin and BottomMargin for pads\n\
(\"Canvas, pad attributes\")\n\
Changing the above parameters needs a complete redrawing of the\n\
canvas and will only be effective with the next command.\n\
Histograms may be updated automatically:\n\
\"AutoUpdateDelay\": determines the interval (sec)\n\
With 3d Views (LEGO, SURF) 2-dim hists are rotated simultanously, if \n\
\"Enable CommonRotate\" is activated\n\
\"Always ShowAllAsFirst\": Always use range of first histogram\n\
displayed for the other hists.\n\
Histograms may be arranged: on top of each other, side by side\n\
or as tiles\n\
";
	fRow_lab = new TList();
//	static void *fValp[50];
	Int_t ind = 0;
//	static Int_t dummy = 0;
	fRow_lab->Add(new TObjString("PlainIntVal_Window X Width"));
	fValp[ind++] = &fWindowXWidth;
	fRow_lab->Add(new TObjString("PlainIntVal+Window Y Width"));
	fValp[ind++] = &fWindowYWidth;
	fRow_lab->Add(new TObjString("DoubleValue_Top margin"));
	fValp[ind++] = &fPadTopMargin;
	fRow_lab->Add(new TObjString("DoubleValue+Bottom margin"));
	fValp[ind++] = &fPadBottomMargin;
	fRow_lab->Add(new TObjString("DoubleValue_Left margin"));
	fValp[ind++] = &fPadLeftMargin;
	fRow_lab->Add(new TObjString("DoubleValue+Right margin"));
	fValp[ind++] = &fPadRightMargin;
	fRow_lab->Add(new TObjString("CheckButton_No Spc betw pads"));
	fNoSpaceButton = ind;
	fValp[ind++] = &fNoSpace;
	fRow_lab->Add(new TObjString("CheckButton+StatBox 1 dim"));
	fValp[ind++] = &fShowStatBox1Dim;
	fRow_lab->Add(new TObjString("CheckButton+StatBox 2 dim"));
	fValp[ind++] = &fShowStatBox2Dim;
	fRow_lab->Add(new TObjString("DoubleValue_Label Magnification;0.01;5."));
	fMagFacButton = ind;
	fValp[ind++] = &fMagFac;
	fRow_lab->Add(new TObjString("PlainIntVal+AutoUpdateDelay"));
	fValp[ind++] = &fAutoUpdateDelay;
	fRow_lab->Add(new TObjString("CheckButton_CommonRotate"));
	fValp[ind++] = &fCommonRotate;
	fRow_lab->Add(new TObjString("CheckButton+ShowAllAsFirst"));
	fValp[ind++] = &fShowAllAsFirst;
	fRow_lab->Add(new TObjString("RadioButton_ArrangeOnTop"));
	fValp[ind++] = &fArrangeOnTop;
	fRow_lab->Add(new TObjString("RadioButton+SideBySide"));
	fValp[ind++] = &fArrangeSideBySide;
	fRow_lab->Add(new TObjString("RadioButton+AsTiles"));
	fValp[ind++] = &fArrangeAsTiles;
	Int_t itemwidth = 420;
	static Int_t ok = 0;
	fDialog =
		new TGMrbValuesAndText ("Set Params", NULL, &ok, itemwidth,
								fRootCanvas, NULL, NULL, fRow_lab, fValp,
								NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________
void GroupOfHists::RestoreDefaults()
{
	TEnv env(".hprrc");
	fDisplayLowX = env.GetValue("GroupOfHists.fDisplayLowX", 0);
	fDisplayLowY = env.GetValue("GroupOfHists.fDisplayLowY", 0);
	fDisplayUpX  = env.GetValue("GroupOfHists.fDisplayUpX", 0);
	fDisplayUpY  = env.GetValue("GroupOfHists.fDisplayUpY", 0);
	
	fDisplayLogY  = env.GetValue("GroupOfHists.fDisplayLogY", 0);
	fDisplayLogX  = env.GetValue("GroupOfHists.fDisplayLogX", 0);
	fDisplayLogZ  = env.GetValue("GroupOfHists.fDisplayLogZ", 0);
	
	fTwoDimLogY  = env.GetValue("GroupOfHists.fTwoDimLogY", 0);
	fTwoDimLogX  = env.GetValue("GroupOfHists.fTwoDimLogX", 0);
	fTwoDimLogZ  = env.GetValue("GroupOfHists.fTwoDimLogZ", 0);
	fWindowXWidth = env.GetValue("GroupOfHists.fWindowXWidth", 800);
	fWindowYWidth = env.GetValue("GroupOfHists.fWindowYWidth", 800);
	if (fWindowXWidth < 20 || fWindowXWidth > 1600) fWindowXWidth = 800;
	if (fWindowYWidth < 20 || fWindowYWidth > 1200) fWindowYWidth = 800;
	fAutoUpdateDelay = env.GetValue("GroupOfHists.fAutoUpdateDelay", 5);
	fMagFac          = env.GetValue("GroupOfHists.fMagFac", 0.8);
	fCommonRotate = env.GetValue("GroupOfHists.fCommonRotate", 1);
	fNoSpace = env.GetValue("GroupOfHists.fNoSpace", 0);
	fShowAllAsFirst = env.GetValue("GroupOfHists.fShowAllAsFirst", 0);
	fArrangeOnTop = env.GetValue("GroupOfHists.fArrangeOnTop", 0);
	fArrangeSideBySide = env.GetValue("GroupOfHists.fArrangeSideBySide", 0);
	fArrangeAsTiles = env.GetValue("GroupOfHists.fArrangeAsTiles", 1);
	fFill1Dim      = env.GetValue("Set1DimOptDialog.fFill1Dim", 0);
	fHistFillColor = env.GetValue("Set1DimOptDialog.fFillColor", 2);
	fHistLineColor1Dim = env.GetValue("Set1Dim OptDialog.fLineColor", 1);
	fHistLineColor2Dim = env.GetValue("Set2DimOptDialog.fHistLineColor2Dim", 1);
	fHistFillStyle = env.GetValue("Set1DimOptDialog.fFillStyle", 0);
	fHistLineStyle = env.GetValue("Set1DimOptDialog.fLineStyle", 1);
	fHistLineWidth1Dim = env.GetValue("Set1DimOptDialog.fLineWidth", 2);
	fHistLineWidth2Dim = env.GetValue("Set2DimOptDialog.fHistLineWidth2Dim", 1);
	fShowContour   = env.GetValue("Set1DimOptDialog.fShowContour", 0);
	fErrorMode    = env.GetValue("Set1DimOptDialog.fErrorMode", "e");
	fDrawOpt2Dim  = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
	fShowZScale   = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
	fShowStatBox2Dim	= env.GetValue("GroupOfHists.fShowStatBox2Dim", 0);
	fShowStatBox1Dim  = env.GetValue("GroupOfHists.fShowStatBox1Dim", 0);
	if (fShowZScale)
		fDrawOpt2Dim += "Z";
	fOptStat1Dim    = env.GetValue("WhatToShowDialog.fOptStat1Dim ", 1000001);
	fOptStat2Dim    = env.GetValue("WhatToShowDialog.fOptStat1Dim ", 1000001);

	fPadBottomMargin	= env.GetValue("GroupOfHists.fPadBottomMargin",     0.1);
	fPadTopMargin	   = env.GetValue("GroupOfHists.fPadTopMargin",        0.1);
	fPadLeftMargin  	= env.GetValue("GroupOfHists.fPadLeftMargin",       0.1);
	fPadRightMargin	= env.GetValue("GroupOfHists.fPadRightMargin",      0.15);

}
//_______________________________________________________
void GroupOfHists::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("GroupOfHists.fWindowXWidth", fWindowXWidth);
	env.SetValue("GroupOfHists.fWindowYWidth", fWindowYWidth);
	env.SetValue("GroupOfHists.fAutoUpdateDelay", fAutoUpdateDelay);
	env.SetValue("GroupOfHists.fMagFac", fMagFac);
	env.SetValue("GroupOfHists.fNoSpace", fNoSpace);
	env.SetValue("GroupOfHists.fCommonRotate", fCommonRotate);
	env.SetValue("GroupOfHists.fShowAllAsFirst", fShowAllAsFirst);
	env.SetValue("GroupOfHists.fArrangeOnTop", fArrangeOnTop);
	env.SetValue("GroupOfHists.fArrangeSideBySide", fArrangeSideBySide);
	env.SetValue("GroupOfHists.fArrangeAsTiles",  fArrangeAsTiles);
	env.SetValue("GroupOfHists.fShowStatBox2Dim", fShowStatBox2Dim);
	env.SetValue("GroupOfHists.fShowStatBox1Dim", fShowStatBox1Dim);
	env.SetValue("GroupOfHists.fPadBottomMargin", fPadBottomMargin	);
	env.SetValue("GroupOfHists.fPadTopMargin",    fPadTopMargin	   );
	env.SetValue("GroupOfHists.fPadLeftMargin",   fPadLeftMargin  	);
	env.SetValue("GroupOfHists.fPadRightMargin",  fPadRightMargin	);
//	env.SetValue("GroupOfHists.fDisplayLogX"    , fDisplayLogX   );
//	env.SetValue("GroupOfHists.fDisplayLogY"    , fDisplayLogY   );
//	env.SetValue("GroupOfHists.fDisplayLogZ"    , fDisplayLogZ   );
	env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void GroupOfHists::CloseDown(Int_t)
{
   cout << "GroupOfHists::CloseDown() fDialog " << fDialog<< endl;
	fDialog = NULL;
}
//_______________________________________________________________________
void GroupOfHists::CRButtonPressed(Int_t /*widgetId*/, Int_t buttonId, TObject */*obj*/)
{
	if (gHprDebug > 0)
   cout << "GroupOfHists::CRButtonPressed " << buttonId << endl;
	if (fCanvas->GetWindowWidth() != (UInt_t)fWindowXWidth ||
		 fCanvas->GetWindowHeight()!= (UInt_t)fWindowYWidth) {
		 fCanvas->SetWindowSize(fWindowXWidth,fWindowYWidth);
	}
	if (buttonId == fNoSpaceButton || buttonId == fMagFacButton) {
		BuildCanvas();
	}
	if (buttonId >= 2 && buttonId <=5) {
		TPad *pad;
		TIter next(fPadList);
		while ( (pad = (TPad*)next() ) ) {
			pad->SetLeftMargin(fPadLeftMargin);
			pad->SetRightMargin(fPadRightMargin);
			pad->SetBottomMargin(fPadBottomMargin);
			pad->SetTopMargin(fPadTopMargin);
			pad->Modified();
		}
	}
	SaveDefaults();
	fCanvas->Modified();
	fCanvas->Update();
	if (!fCanvas->GetAutoExec())
		 fCanvas->ToggleAutoExec();
}


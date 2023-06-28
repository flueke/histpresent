#include "TRegexp.h"
#include "TTree.h"
#include "TBranch.h"
#include "TBranchObject.h"
#include "TLeafObject.h"
#include "TGMsgBox.h"
#include "TRootCanvas.h"
#include "TCanvasImp.h"
#include "TPaletteAxis.h"
#include "TStopwatch.h"

#include "HistPresent.h"
#include "GeneralAttDialog.h"
#include "FitHist.h"
#include "FhContour.h"
//#include "FitHist_Help.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "TMrbStatistics.h"
#include "HTCanvas.h"
#include "SetColor.h"
#include "hprbase.h"
//#include "TMrbVarWdwCommon.h"
#include "TMrbHelpBrowser.h"
#include "CmdListEntry.h"
#include "WindowSizeDialog.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
//#include <cstdlib>
extern Int_t gHprDebug;

//________________________________________________________________________________________

void AddMathExpressions(TList * var_list)
{
	if (!var_list) return;
	var_list->Add(new TObjString("TMath::Sqrt("));
	var_list->Add(new TObjString("TMath::Sin("));
	var_list->Add(new TObjString("TMath::Cos("));
	var_list->Add(new TObjString("TMath::ATan("));
	var_list->Add(new TObjString("TMath::ATan2("));
	var_list->Add(new TObjString("TMath::Log("));
	var_list->Add(new TObjString("TMath::Log10("));
	var_list->Add(new TObjString("TMath::Abs("));
	var_list->Add(new TObjString("TMath::Prob("));
	var_list->Add(new TObjString("TMath::Power("));
	var_list->Add(new TObjString("TMath::Pi()"));
	var_list->Add(new TObjString("TMath::TwoPi()"));
	var_list->Add(new TObjString("TMath::RadToDeg("));
	var_list->Add(new TObjString("TMath::DegToRad("));
}
//________________________________________________________________________________________
// Show Tree

void HistPresent::ShowTree(const char* fname, const char* dir, const char* tname, const char* /*bp*/)
{
	static Int_t ycanvas=180;
//  const Int_t MAXLEAF=33;
  if (fRootFile) fRootFile->Close();
//  fRootFile=new TFile(fname);
	fRootFile=TFile::Open(fname);
	if ( fRootFile && gHprDebug > 1 )
		fRootFile->ls();
	if (strlen(dir) > 0) fRootFile->cd(dir);
	TTree *tree = (TTree*)gDirectory->Get(tname);
	TObjArray *leaves = tree->GetListOfLeaves();
	Int_t nleaves = leaves->GetEntriesFast();
	if (nleaves <=0) {
		 cout << "No leaves" << endl;
		 gDirectory = gROOT;
		 return;
	} else {
		cout<< endl << "Number of leaves in " << tname << " " << nleaves << endl;
	}	

	fSelectLeaf->Delete();
//   *fLeafCut="";
//   TButton *button;
	Int_t len, l, not_shown = 0;
//	Bool_t showit;
	
//   TLeaf *leafcount;
//   char *bname;
//   char branchname[1000];
	TString cmd;
	TString nam;
	TString empty;
	TString sel;
	TList * var_list = new TList();
/*
	cmd = "mypres->DefineGraphCut()";
	nam = "Define graphical cut";
	sel = "mypres->ToggleGraphCut()";
	fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));
*/
	cmd = "mypres->SetShowTreeOptionsCint()_"; // dont add address of button
	nam = "Set Options / Help";
//   sel = "mypres->ToggleUseHist()";
	fCmdLine->Add(new CmdListEntry(cmd, nam, empty, empty));

	cmd = "mypres->ShowLeaf(\"";
	cmd = cmd + fname + "\",\"" + dir + "\",\"" + tname + "\",\"\")";
	nam = "Fill histogram";
	fCmdLine->Add(new CmdListEntry(cmd, nam, empty, empty));

	cmd = "mypres->MkClass(\"";
	cmd = cmd + fname + "\",\"" + dir + "\",\"" + tname + "\",\"\")";
	nam = "C++ Code (MakeClass)";
	fCmdLine->Add(new CmdListEntry(cmd, nam, empty, empty));

	TString cmd_base("mypres->ShowLeaf(\"");
	TString sel_base("mypres->SelectLeaf(\"");
	cmd_base = cmd_base + fname + "\",\"" + dir + "\",\"" + tname + "\",\"";
	
	for (l=0;l<nleaves;l++) {
		TLeaf *leaf = (TLeaf*)leaves->UncheckedAt(l);
		TBranch *branch = leaf->GetBranch();
//		cout << branch->GetName() <<  " BranchStatus() ";
//		if (!tree->GetBranchStatus(branch->GetName())) cout << " not " ;
// //		cout << " ok"<< endl;
		if (branch->IsA() == TBranchObject::Class()) {
//
		} else {
			len = leaf->GetLen();
			if (gHprDebug > 0) {
				cout << leaf->GetName() << " " << len << endl;
			}
//  apply selection if needed
			if (fUseLeafSelMask && fLeafSelMask.Length() > 0) {
				if ( ! Hpr::IsSelected(leaf->GetName(), &fLeafSelMask, fLeafUseRegexp ) )
					continue;
			}

			if (fCmdLine->GetSize() >= GeneralAttDialog::fMaxListEntries) {
//				showit = kFALSE;
				not_shown++;
			} else {
//				showit = kTRUE;
			}

			if (len > 1) {
			  for (Int_t ix = 0; ix < len; ix++) {
				  cmd = cmd_base + Form("%s[%d]",leaf->GetName(), ix) + "\")";
				  sel = sel_base + Form("%s[%d]",leaf->GetName(), ix) + "\")";
				  nam = Form("%s[%d]",leaf->GetName(), ix);
				  var_list->Add(new TObjString(Form("%s[%d]",leaf->GetName(), ix)));
				  fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));
					if (gHprDebug > 0) {
						cout << cmd << " " << sel << endl;
					}
			  }
			} else {
				cmd = cmd_base + leaf->GetName() + "\")";
				sel = sel_base + leaf->GetName() + "\")";
				nam = leaf->GetName();
				TString ltype = ((TLeafObject*)leaf)->GetTypeName();
				if (ltype.BeginsWith("vector")) {
					nam += "[]";
		  			if (ltype.BeginsWith("vector<vector")) {
						nam += "[]";
					}
				}
				var_list->Add(new TObjString(leaf->GetName()));
				fCmdLine->Add(new CmdListEntry(cmd, nam, empty, sel));
				if (gHprDebug > 0) {
					cout << cmd << " " << sel << endl;
				}

			}
		}
	}
	if ( var_list->GetSize() == 0 && fLeafSelMask.Length() > 0 ) {
		cout << setred << "Warning: No entries found satisfying LeafSelMask: " 
		<< fLeafSelMask << setblack << endl;
	}
	if ( not_shown > 0) {
		cout << setred << "Too many entries in list" << endl;
		cout << "this might crash X, please use \"Leaf selection mask\""<< endl;
		cout << "to reduce number of entries below: " <<  GeneralAttDialog::fMaxListEntries  << endl;
		cout << "On your own risk you may increase value beyond: " << GeneralAttDialog::fMaxListEntries << endl;
		cout << "WARNING: branches not shown: " << not_shown << setblack << endl;
	}
	if ( not_shown > 0 || fWriteLeafNamesToFile) { 
		TString fn("TREE_");
		fn  += tname;
		Int_t pp = fn.Index(";");
		if (pp>0) fn.Resize(pp);
		fn += ".list";
		ofstream lfile(fn);
		for (l=0;l<nleaves;l++) {
			TLeaf *leaf = (TLeaf*)leaves->UncheckedAt(l);
			lfile << leaf->GetName();
			if ( fWriteLeafMinMaxtoFile ) {
				lfile << "    [" 
				<< tree->GetMinimum(leaf->GetName()) << ", "
				<< tree->GetMaximum(leaf->GetName()) << "]" ;
			}
			lfile << endl;
		}
		cout << setblue << endl << "Names of leaves written to: " << fn << setblack << endl;
	}
	cmd = "mypres->EditLeafCut(\"";
	ostringstream buf;
	buf << var_list;
	cmd += buf.str();
//	cmd += Form("0x%x",var_list);
	cmd += "\")";
	nam = "Edit selection/weight";
	sel = "mypres->ToggleLeafCut()";
	fCmdLine->AddFirst(new CmdListEntry(cmd, nam, empty, sel));

	cmd = "mypres->EditExpression(\"";
	ostringstream buf1;
	buf1 << var_list;
	cmd += buf1.str();
//	cmd += Form("0x%x",var_list);
	cmd += "\")";
	nam = "Edit expression";
	sel = "mypres->ToggleExpression()";
	fCmdLine->AddFirst(new CmdListEntry(cmd, nam, empty, sel));

	fRootFile->Close();
	TString title("Tree: ");
	title += tname;
	HTCanvas *ccont = CommandPanel(title.Data(), fCmdLine,
							WindowSizeDialog::fMainWidth + 10, ycanvas, this, WindowSizeDialog::fWinwidx_hlist);
	if (fHistLists)fHistLists->Add(ccont);
	ycanvas += 50;
	if (ycanvas >= 500) ycanvas=5;
	fCmdLine->Delete();

}
//________________________________________________________________________________________

void HistPresent::SelectLeaf(const char* lname, const char* bp)
{
	if (!bp) return;
//   cout << " Adding leaf " << lname << endl;
	const char * aname[4] = {"X-axis", "Y-axis", "Z-axis", "4-axis"};
	TString sel = lname;
	TButton * b;
	b = (TButton *)strtoul(bp, 0, 16);
	if (b->TestBit(kSelected)) {
		TObjString tobjs((const char *)sel);
		cout << "Remove leaf: " << tobjs.GetString() << endl;
		fSelectLeaf->Remove(&tobjs);
		b->SetFillColor(16);
//      b->SetTitle("");
		b->ResetBit(kSelected);
	} else {
		fSelectLeaf->Add(new TObjString((const char *)sel));
//      cout << " Add leaf " << lname << " Index: " << fSelectLeaf->LastIndex()<< endl;
//      b->SetTitle(Form("%d", fSelectLeaf->LastIndex()));
		b->SetFillColor(3);
		b->SetBit(kSelected);
	}
//   b->Paint();
	if (fSelectLeaf->GetSize() > 0) {
		cout << "Current selection:" << endl;
		for (Int_t i = 0; i < fSelectLeaf->GetSize(); i++)
			cout << i << " " << aname[i] << ": " <<  ((TObjString*)fSelectLeaf->At(i))->GetString() << endl;
	}
	b->Modified(kTRUE);b->Update();
}
//________________________________________________________________________________________

void HistPresent::ToggleExpression(const char* bp)
{
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
//      cout << "bp " << b << endl;
		if (b->TestBit(kSelected)) {
			cout << "set use expression =kFALSE"<< endl;
			fApplyExpression = kFALSE;
			b->SetFillColor(16);
			b->ResetBit(kSelected);
		} else {
			cout << "set use expression =kTRUE"<< endl;
			fApplyExpression = kTRUE;
			b->SetFillColor(3);
			b->SetBit(kSelected);
		}
	}
}
//________________________________________________________________________________________

void HistPresent::UseHist(const char* /*bp*/)
{
	ShowContents("Memory", "");
	if (!fUseHist) ToggleUseHist();
}
//________________________________________________________________________________________

void HistPresent::ToggleUseHist(const char* bp)
{
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
		if (b->TestBit(kSelected)) {
			cout << "set UseHist = kFALSE"<< endl;
			fUseHist = kFALSE;
			b->SetFillColor(16);
			b->ResetBit(kSelected);
		} else {
			cout << "set UseHist= kTRUE"<< endl;
			fUseHist = kTRUE;
			b->SetFillColor(3);
			b->SetBit(kSelected);
		}
	}
}
//________________________________________________________________________________________

void HistPresent::ToggleLeafCut(const char* bp)
{
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
		if (b->TestBit(kSelected)) {
			cout << "set formula cut =kFALSE"<< endl;
			fApplyLeafCut = kFALSE;
			b->SetFillColor(16);
			b->ResetBit(kSelected);
		} else {
			if (fApplyGraphCut) {
				WarnBox("Cant have formula and graphical cut simultanously");
				return;
			}
			cout << "set formula cut =kTRUE "<< endl;
			fApplyLeafCut = kTRUE;
			b->SetFillColor(3);
			b->SetBit(kSelected);
		}
	}
}
//________________________________________________________________________________________
/*
void HistPresent::ToggleGraphCut(const char* bp)
{
	if (bp) {
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
		if (b->TestBit(kSelected)) {
			cout << "set graphical cut =kFALSE"<< endl;
			fApplyGraphCut = kFALSE;
			b->SetFillColor(16);
		} else {
			if (fApplyLeafCut) {
				WarnBox("Cant have formula and graphical cut simultanously");
				return;
			}
			cout << "set graphical cut =kTRUE"<< endl;
			fApplyGraphCut = kTRUE;
			b->SetFillColor(3);
		}
	}
}
*/
//________________________________________________________________________________________

void HistPresent::EditExpression(const char* vl, const char* bp)
{
	Bool_t ok;
	const char hist_file[] = {"ntupleCmds.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40000) hf = NULL;
	TList * var_list = 0;
	if (bp) var_list = (TList*)strtoul(vl, 0, 16);
	if (var_list) AddMathExpressions(var_list);
//   if (var_list)var_list->Print();
//   *fExpression=GetString("Edit expression",(const char *)*fExpression,
//                          &ok, fRootCanvas, 0,0,0,0,0, hf, var_list);
	ok = GetStringExt("Edit expression", fExpression,
								  300, fRootCanvas, hf, var_list);
	if (!ok) return;
	if (strlen(*fExpression) > 1) {
		cout << *fExpression<< endl;
		cout << "Press grey button to activate" << endl;
//       if (!fApplyExpression)ToggleExpression();
	 }
}
//________________________________________________________________________________________

void HistPresent::EditLeafCut(const char* vl, const char* bp)
{

	Bool_t ok;
	const char hist_file[] = {"ntupleCuts.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40000) hf = NULL;
	TList * var_list = 0;
	if (bp) var_list = (TList*)strtoul(vl, 0, 16);
	if (var_list) AddMathExpressions(var_list);
//   *fLeafCut=GetString("Edit formula cut",(const char *)*fLeafCut,
//                         &ok, fRootCanvas, 0,0,0,0,0, hf, var_list);
	ok = GetStringExt("Edit expression", fLeafCut,
								  300, fRootCanvas, hf, var_list);
	if (!ok) return;
	 if (strlen(*fLeafCut) > 1) {

//       if (!fApplyLeafCut)ToggleLeafCut();
		 cout << *fLeafCut << endl;
		 cout << "Please press grey button to activate" << endl;
	 }
}
//________________________________________________________________________________________
/*
void HistPresent::DefineGraphCut(const char* bp)
{
	Bool_t ok;
	const char hist_file[] = {"ntupleGraphCuts.txt"};
	const char * hf = hist_file;
	if (gROOT->GetVersionInt() < 40101) hf = NULL;
	*fGraphCut=GetString("Cut name",(const char *)*fGraphCut, &ok, fRootCanvas,
								 0,0,0,0,0, hf);
	if (!ok) return;
	if (strlen(*fGraphCut) > 1) {
//      TCutG* cc=(TCutG*)gROOT->FindObject(fGraphCut->Data());
		TCutG* cc=(TCutG*)fAllCuts->FindObject(fGraphCut->Data());
		if (!cc) {
			WarnBox("Cut not found in memory");
			return;
		}
		*fCutVarX = cc->GetVarX();
		*fCutVarX=GetString("X-value name",fCutVarX->Data(), &ok, fRootCanvas);
		cc->SetVarX(fCutVarX->Data());
		*fCutVarY = cc->GetVarY();
		*fCutVarY=GetString("Y-value name",fCutVarY->Data(), &ok, fRootCanvas);
		cc->SetVarY(fCutVarY->Data());
		if (!fApplyGraphCut) ToggleGraphCut();
		cout << "Enable: " << *fGraphCut << endl;
	} else {
		if (fApplyGraphCut) ToggleGraphCut();
	}
//  svalues->Delete();
//  delete svalues;
}
*/
//________________________________________________________________________________________

void HistPresent::ShowLeaf( const char* fname, const char* dir, const char* tname,
									 const char* leafname, const char* bp)
{
	TStopwatch *stopwatch = new TStopwatch();
	TString leaf0;
	TString leaf1;
	TString leaf2;
	TString leaf3;
	TString * leaf[4] = {&leaf0, &leaf1, &leaf2, &leaf3};

	TString cmd;
	TString hname;
	TString htitle;
	if ( fNtuplePrependFN ) {
		hname += fname;
		Int_t inddot = hname.Index(".");
		if (inddot > 0) hname.Resize(inddot); // chop of .root .map etc.
		hname += "_";
	}
	if ( fNtuplePrependTN ) {
		hname += tname;
		hname += "_";	
	}
	Int_t nent = 1;
	Double_t nbin[4] = {0, 0, 0, 0};
	Double_t vmin[4] = {0, 0, 0, 0};
	Double_t vmax[4] = {0, 0, 0, 0};
	static Int_t first_event = 0;
	static Int_t nof_events = 0;
	Int_t indbr;
//   cout << "leafname " << leafname << " " << strlen(leafname)
//    << " nent " << fSelectLeaf->GetSize() << endl;
	if (strlen(leafname)>0) {
		leaf0=leafname;
		indbr = leaf0.Index("<");
		if (indbr > 0) leaf0.Resize(indbr);
		cmd=leaf0;
		nent = 1;
	} else {
		if (fSelectLeaf->IsEmpty() && !fApplyExpression) {
			WarnBox("Please select a leaf");
			return;
		}
		nent = fSelectLeaf->GetSize();
		if (nent > 4) {
			WarnBox("More the 4 leafs selected");
			return;
		}
		if (nent > 0) {
			TObjString *objs;
			TIter next(fSelectLeaf);
			objs = (TObjString *)next();
			leaf0 = objs->String();
			indbr = leaf0.Index("<");
			if (indbr > 0) leaf0.Resize(indbr);
			cmd = leaf0;
			if (nent > 1) {
				objs = (TObjString *)next();
				leaf1 = objs->String();
				indbr = leaf1.Index("<");
				if (indbr > 0) leaf1.Resize(indbr);
				cmd.Prepend(":");
				cmd.Prepend(leaf1.Data());
			}
			if (nent > 2) {
				objs = (TObjString *)next();
				leaf2 = objs->String();
				indbr = leaf2.Index("<");
				if (indbr > 0) leaf2.Resize(indbr);
				cmd.Prepend(":");
				cmd.Prepend(leaf2.Data());
			}
			if (nent > 3) {
				objs = (TObjString *)next();
				leaf3 = objs->String();
				indbr = leaf3.Index("<");
				if (indbr > 0) leaf3.Resize(indbr);
// nota bene 4 dim is special
				cmd.Append(":");
				cmd.Append(leaf3.Data());
			}
		}
	}

	if (fApplyExpression) {
		cmd = *fExpression;
		cout << cmd << endl;
		if (nent == 0) {
//         find # of dimensions
			Int_t sind = 0, colind;
			while (1) {
				colind = cmd.Index(":", sind);
				if (colind < 0) break;
				if (cmd[colind+1] == ':') {
					sind = colind + 2;
					continue;
				}
				*leaf[nent] = cmd(sind, colind-sind);

				sind = colind + 1;
				nent++;
			}
			*leaf[nent] = cmd(sind, cmd.Length() - sind);
			nent++;
		}
//     invert order
		TString * temp;
		if (nent == 2) {
			temp = leaf[0];
			leaf[0] = leaf[1];
			leaf[1] = temp;
		} else if (nent == 3) {
			temp = leaf[0];
			leaf[0] = leaf[2];
			leaf[2] = temp;
		} else if (nent == 4) {
			temp = leaf[0];
			leaf[0] = leaf[3];
			leaf[3] = temp;
			temp = leaf[1];
			leaf[1] = leaf[2];
			leaf[2] = temp;
		}
		TRegexp a1("\\$1");
		while (cmd.Index(a1)>=0) {cmd(a1)=leaf0.Data();}
		TRegexp a2("\\$2");
		if (cmd.Index(a2)>=0) {
			if (nent <2) {
				WarnBox("2 args expected, only 1 given");
				return;
			} else {while (cmd.Index(a2)>=0) {cmd(a2)=leaf1.Data();}}
		}
		TRegexp a3("\\$3");
		if (cmd.Index(a3)>=0) {
			if (nent <3) {
				WarnBox("3 args expected");
				return;
			} else {while (cmd.Index(a3)>=0) {cmd(a3)=leaf2.Data();}}
		}
	}
	if (nent > 0) hname += leaf0;
	else hname += "userdef";
	if (nent > 1)
		hname += "_";
	hname += leaf1;
	if (nent > 2)
		hname += "_";
	hname += leaf2;
	if (nent > 3)
		hname += "_";
	hname += leaf3;
	htitle = hname;
	hname.Prepend( "hist_");	

	TString option = "goff";

	TString cmd_orig(cmd);
	cmd += ">>";
		if (fNtupleVersioning) {
			cout << "fNtupleSeqNr " <<fNtupleSeqNr << endl;
			hname += "_v";
			hname += fNtupleSeqNr;
			fNtupleSeqNr++;
		}
		TRegexp notascii("[^a-zA-Z0-9_]", kFALSE);
		while (hname.Index(notascii) >= 0) {
			hname(notascii) = "_";
		}

		TString fh_name(hname);
		fh_name.Prepend("F");

//      cout << "look for:" << fh_name << endl;
		TObject *fobj = (TObject *)gROOT->FindObject(fh_name.Data());
		if (fobj) {
			cout << " deleting " << fh_name << endl;
			delete fobj;
		}
		fobj = (TObject *)gROOT->FindObject(hname.Data());
		if (fobj) {
			cout << " deleting " << hname << endl;
			delete fobj;
		}
//   }
	cmd += hname;

	gDirectory=gROOT;
//   if (nent==3) new TCanvas("ctemp","ctemp",800,800);

	if (fRootFile) fRootFile->Close();
//   fRootFile=new TFile(fname);
	fRootFile=TFile::Open(fname);
	if (strlen(dir) > 0) fRootFile->cd(dir);
	TTree *tree = (TTree*)gDirectory->Get(tname);

	gDirectory = gROOT;

	TEnv * env = 0;
	Bool_t limits_defined = kFALSE;
	TString tag = leaf0.Data();
	if (nent > 0 && fRememberTreeHists) {
		if (gSystem->AccessPathName("ntuplerc")) {
			ofstream rcfile("ntuplerc", ios::out);
			if (!rcfile) {
			  cout << "Cant open new ntuplerc" << endl;
			}
			rcfile << "ntuple.resourcefile: ntuplerc" << endl;;
			cout <<  "New file: ntuplerc" << endl;
			rcfile.close();
		}
		env = new TEnv("ntuplerc");
//     look if at least one of nbin, min max is defined;
		if (env) {
			limits_defined = kTRUE;
			for(Int_t i = 0; i < nent; i++) {
				Int_t nb;
				tag = *leaf[i]; tag += ".nbin";
				if (env->Lookup(tag.Data())) {
					nb = (Int_t)nbin[i];
					nb = env->GetValue(tag, nb); nbin[i] = nb;
				} else {
					limits_defined = kFALSE;
				}
				tag = *leaf[i]; tag += ".min";
				if (env->Lookup(tag.Data())) {
					vmin[i] = atof(env->GetValue(tag.Data(), Form("%f",vmin[i])));
				}else {
					limits_defined = kFALSE;
				}

				tag = *leaf[i]; tag += ".max";
				if (env->Lookup(tag.Data())) {
					vmax[i] = atof( env->GetValue(tag.Data(), Form("%f",vmax[i])));
				}else {
					limits_defined = kFALSE;
				}

			}
			first_event = env->GetValue("FirstEvent", 0);
			nof_events =  env->GetValue("NofEvents", 0);
		}
	}
//  look if we need to redefine limits
	Bool_t must_find_limits = kFALSE;
	for(Int_t i = 0; i < nent; i++) {
	  if (nbin[i] == 0) must_find_limits = kTRUE;
	}
	if (fAlwaysFindLimits) must_find_limits = kTRUE;
	if (must_find_limits) limits_defined = kFALSE;

	Bool_t modified = kFALSE;
		Double_t umin=0, umax=0;
	if (must_find_limits) {
		cout << setblue << "Do tree->Draw() to find limits: "<< cmd_orig << setblack << endl;
		/*
		xmin = tree->GetMinimum((*leaf[0]).Data());
		xmax = tree->GetMaximum((*leaf[0]).Data());
		
//      cout << "xmin, xmax " << xmin << " " << xmax << endl;
		if (nent > 1) {
			ymin = tree->GetMinimum((*leaf[1]).Data());
			ymax = tree->GetMaximum((*leaf[1]).Data());
//			cout << "ymin, ymax " << ymin << " " << ymax << endl;
		}
		if (nent > 2) {
			zmin = tree->GetMinimum((*leaf[2]).Data());
			zmax = tree->GetMaximum((*leaf[2]).Data());
//			cout << "zmin, zmax " << zmin << " " << zmax << endl;
		}
		*/
		if (nent > 3) {
			umin = tree->GetMinimum((*leaf[3]).Data());
			umax = tree->GetMaximum((*leaf[3]).Data());
//			cout << "umin, umax " << umin << " " << umax << endl;
		}

		tree->Draw(cmd_orig.Data(),"","goff");
		TH1* htemp = (TH1*)gDirectory->FindObject("htemp");
		if (htemp) {
			for(Int_t i = 0; i < TMath::Min(nent, 3); i++) {
//            if (nbin[i] > 0) continue;
				modified = kTRUE;
				TAxis * a = 0;
				if      (i == 0) a = htemp->GetXaxis();
				else if (i == 1) a = htemp->GetYaxis();
				else if (i == 2) a = htemp->GetZaxis();
				vmin[i] = a->GetXmin();
				vmax[i] = a->GetXmax();
//            cout << i << " " << vmin[i] << " " << vmax[i] << endl;
				if (nbin[i] <= 0) nbin[i] = 100;
			}
		}
	}
	if ((!limits_defined || fAlwaysRequestLimits) && nent < 4) {
		TOrdCollection *row_lab = new TOrdCollection();
		TOrdCollection *col_lab = new TOrdCollection();
		col_lab->Add(new TObjString("Nbins"));
		col_lab->Add(new TObjString("Min or 1. Ev"));
		col_lab->Add(new TObjString("Max or NofEv"));
		row_lab->Add(new TObjString((*leaf[0]).Data()));
		if (nent > 1 ) row_lab->Add(new TObjString((*leaf[1]).Data()));
		if (nent > 2 ) row_lab->Add(new TObjString((*leaf[2]).Data()));
		if (nent > 3 ) row_lab->Add(new TObjString((*leaf[3]).Data()));
		row_lab->Add(new TObjString("Event Select"));
		TArrayD xyvals(3*(nent + 1));
		Int_t p = 0;
		for(Int_t i = 0; i < nent; i++) { xyvals[p] = nbin[i]; p++;}
		xyvals[p] = 0; p++;
		for(Int_t i = 0; i < nent; i++) { xyvals[p] = vmin[i]; p++;}
		xyvals[p] = (Double_t)first_event; p++;
		for(Int_t i = 0; i < nent; i++) { xyvals[p] = vmax[i]; p++;}
		xyvals[p] = (Double_t)nof_events;
// show values to caller and let edit
		TButton * b;
		b = (TButton *)strtoul(bp, 0, 16);
		TRootCanvas * pwin = NULL;
		if (b) {
			pwin = (TRootCanvas *)(b->GetCanvas()->GetCanvasImp());
		}

		Int_t ret = 0,  itemwidth=120, precission = 5;
		TGMrbTableOfDoubles(pwin, &ret, "Set axis ranges",
								itemwidth,3, nent + 1, xyvals, precission,
							  col_lab, row_lab);
		delete row_lab; delete col_lab;
		if (ret < 0) {
 //        delete [] vmin; delete [] vmax; delete [] nbin;
//         delete [] xyvals;
			return;
		}
		p = 0;
		for(Int_t i = 0; i < nent; i++) {
			if   (nbin[i] != xyvals[p]) {
				nbin[i] = xyvals[p];
				modified = kTRUE;
			}
			p++;
		}
		p++;      // skip
		for(Int_t i = 0; i < nent; i++) {
			if   (vmin[i] != xyvals[p]) {
				vmin[i] = xyvals[p];
				modified = kTRUE;
			}
			p++;
		}

		if (first_event != (Int_t)xyvals[p]) {
			first_event = (Int_t)xyvals[p];
			modified = kTRUE;
		}
		p++;
		for(Int_t i = 0; i < nent; i++) {
			if   (vmax[i] != xyvals[p]) {
				vmax[i] = xyvals[p];
				modified = kTRUE;
			}
			p++;
		}
		if (nof_events != (Int_t)xyvals[p]) {
			nof_events = (Int_t)xyvals[p];
			modified = kTRUE;
		}
//      delete [] xyvals;
		if (modified && fRememberTreeHists) {
/*
			Int_t buttons= kMBYes | kMBNo, retval;
			EMsgBoxIcon icontype = kMBIconQuestion;
			new TGMsgBox(gClient->GetRoot(), pwin,
			 "Qustion","Save values to ntuplerc",
			 icontype, buttons, &retval);
			if (retval == kMBYes) {
*/
				for(Int_t i = 0; i < nent; i++) {
					tag = *leaf[i]; tag += ".nbin";
					env->SetValue(tag.Data(),(Int_t)nbin[i]);
					tag = *leaf[i]; tag += ".min";
					env->SetValue(tag.Data(),vmin[i]);
					tag = *leaf[i]; tag += ".max";
					env->SetValue(tag.Data(),vmax[i]);
				}
				env->SetValue("FirstEvent", first_event);
				env->SetValue("NofEvents", nof_events);
				env->SaveLevel(kEnvLocal);
//         }
		}
	}
	if (fApplyLeafCut && fAppendSelections){
		htitle+="_";
		htitle+=*fLeafCut;
	}
	TCanvas *cc = NULL;
	if (nent==1) {
		new TH1F(hname.Data(),htitle,
				 (Int_t)nbin[0],vmin[0], vmax[0]);
	} else if ( nent < 4 && f2dimAsGraph == 0 ) {
		if ( nent==2 )
			new TH2F(hname.Data(),htitle,
					(Int_t)nbin[0],vmin[0], vmax[0],
					(Int_t)nbin[1],vmin[1], vmax[1]);
		if ( nent==3 )
			new TH3F(hname.Data(),htitle,
						(Int_t)nbin[0],vmin[0], vmax[0],
						(Int_t)nbin[1],vmin[1], vmax[1],
						(Int_t)nbin[2],vmin[2], vmax[2]);
	} else {
		if (nent < 4 ) {
			cmd += " (";
			cmd += (Int_t)nbin[0]; cmd += ",";
			cmd += vmin[0]; cmd += ",";
			cmd += vmax[0]; cmd += ",";
			cmd += (Int_t)nbin[1]; cmd += ",";
			cmd += vmin[1]; cmd += ",";
			cmd += vmax[1];
			if (nent > 2) {
				cmd += ",";
				cmd += (Int_t)nbin[2]; cmd += ",";
				cmd += vmin[2]; cmd += ",";
				cmd += vmax[2];
			}
			if (nent > 3) {
				cmd += ",";
				cmd += (Int_t)nbin[3]; cmd += ",";
				cmd += vmin[3]; cmd += ",";
				cmd += vmax[3];
			}
			cmd += ")";
			}
		option = "";
		cc = new TCanvas("cc", "cc", 500,10, 800, 800);
		cc->Draw();
	}
//   delete [] vmin; delete [] vmax; delete [] nbin;
	if (env) delete env;


//   TEnv rootenv(".rootrc");		// inspect ROOT's environment
//   Int_t max_events = 10000000;
//   max_events = rootenv.GetValue("HistPresent.MaxEvents", max_events);
//   cout << cmd << endl;
	tree->SetMarkerStyle(fMarkStyle);
	tree->SetMarkerSize(fMarkSize);
	tree->SetMarkerColor(fMarkColor);

	if (nof_events <= 0) nof_events = 100000000;
	cout << "Start tree->Draw()" << endl;
	stopwatch->Start();
	
	if (fApplyLeafCut) {
		TString cut = *fLeafCut;
		TRegexp a1("\\$1");
		if (cut.Index(a1)>=0) {while (cut.Index(a1)>=0) {cut(a1)=leaf0.Data();}}
		TRegexp a2("\\$2");
		if (cut.Index(a2)>=0) {
			if (nent < 2) {
				WarnBox("2 args expected, only 1 given");
				return;
			} else { while (cut.Index(a2)>=0) {cut(a2)=leaf1.Data();}}
		}
		TRegexp a3("\\$3");
		if (cut.Index(a3)>=0) {
		  if (nent < 3) {
			  WarnBox("3 args expected");
			  return;
		  } else { while (cut.Index(a3)>=0) {cut(a3)=leaf2.Data();}}
		}
		cout << "Execute: " << cmd << " Cut: " << cut << " 1.Ev, NofEv: "
								  << first_event << " " <<nof_events << endl;
		tree->Draw((const char*)cmd, cut.Data(),option.Data(), nof_events, first_event);
/*		
	} else if (fApplyGraphCut && nent == 2) {
		tree->Draw((const char*)cmd, fGraphCut->Data(),option.Data(),
						nof_events, first_event);
		cout << "Execute: " << cmd << " 1.Ev, NofEv: "
								  << first_event << " " <<nof_events << endl;
		cout << "Apply graphical cut: " <<  fGraphCut->Data() <<
		" with X :" << fCutVarX->Data() <<
		" and Y :" << fCutVarX->Data() <<  endl;
*/
	} else {
		cout << "Execute: " << cmd << " DrawOpt: "<< option  << " 1.Ev, NofEv: "
								  << first_event << " " <<nof_events << endl;
		tree->Draw((const char*)cmd,"",option, nof_events, first_event);
	}
	stopwatch->Stop();
	cout << "End tree->Draw()" << endl;
	stopwatch->Print();
	
	fRootFile->Close();
	fRootFile=NULL;
	if ( cc ) cc->Modified();
//   if (nent > 1 && f2dimAsGraph != 0) return;
	TObject *obj = (TObject *)gROOT->FindObject(hname.Data());
//  if (!obj)WarnBox("No Object"); if (nent==2 && f2dimAsGraph == 0)
	TH1* hist = (TH1*)obj;
	if (hist) {
		hist->GetXaxis()->SetTitle(*leaf[0]);
		if (nent >= 2) hist->GetYaxis()->SetTitle(*leaf[1]);
		if (nent >= 3) hist->GetZaxis()->SetTitle(*leaf[2]);
		if (nent != 4 && f2dimAsGraph == 0)
			ShowHist(hist);
		if (nent == 4) {
			TH2F* h3 = (TH2F*)gROOT->FindObject("DummyForTPaletteAxis");
			if ( !h3)
				h3 = new TH2F("DummyForTPaletteAxis","v",2,0,10, 2,0,10);
			h3->SetMinimum(umin);
			h3->SetMaximum(umax);
			Double_t uc[50];
			for (int i=0; i < 50; i++) uc[i] = umin + i * (umax - umin) / 50.;
			h3->SetContour(50,uc);
			TPaletteAxis *pa = new TPaletteAxis(0.85, -0.9,0.9,0.9, h3);
			pa->Draw();

		}
	} else {
		WarnBox("No hist");
	}
	gPad->Modified();
	gPad->Update();
}

//________________________________________________________________________________________

void HistPresent::MkClass( const char* fname, const char* dir, const char* tname,
									 const char* /*leafname*/, const char* /*bp*/)
{
	if (fRootFile) fRootFile->Close();
	fRootFile=new TFile(fname);
	if (strlen(dir) > 0) fRootFile->cd(dir);
	TTree *tree = (TTree*)gDirectory->Get(tname);
	Bool_t ok;
	TString clname(tname);
	if (clname.Index(";") > 0)clname.Resize(clname.Index(";"));
	clname = GetString("Name of generated class", clname.Data(), &ok);
	if (!ok) return;
	tree->MakeClass(clname.Data());
	cout << clname.Data()<< ".h and .C have been generated" << endl;
	gDirectory = gROOT;
}
//_______________________________________________________________________

void HistPresent::SetShowTreeOptionsCint(const char *pointer)
{
	if (pointer) {
		TCanvas* c = (TCanvas *)strtoul(pointer, 0, 16);
		TRootCanvas * win = (TRootCanvas*)c->GetCanvasImp();         SetShowTreeOptions(win);
	}
}
//_______________________________________________________________________

void HistPresent::SetShowTreeOptions(TGWindow * win)
{
// *INDENT-OFF*
static const char helptext[] =
"\n\
\"Fill histogram\" calls the ROOT function:\n\
\n\
TTree::Draw(const char* varexp, const char* selection, \n\
	  Option_t* option = \"\", Long64_t nentries = 1000000000, \n\
	  Long64_t firstentry = 0)\n\
 \n\
\"varexp\" may be edited explicitly via \"Edit expression\"\n\
In this case the grey button must be activated i.e. green\n\
Otherwise the data fields to be drawn are selected from the\n\
list of leaves\n\
\n\
\"selection/weight\" is used to apply weights or cuts\n\
\n\
In the \"Edit ..\" fields tab completion for leafnames\n\
and members of \"TMath::\" (e.g. \"TMath::Sqrt\" is implemented\n\
\n\
____________________________________________________________\n\
The following options apply when  showing trees\n\
____________________________________________________________\n\
Always write leaf names to file\n\
-------------------------------\n\
Leaf names are written to a file name TREE_nameoftree.list\n\
This file is also written if too many entries are in the tree\n\
to help to find a suitable selection mask\n\
____________________________________________________________\n\
Also write leaf [ Min,  Max]\n\
----------------------------\n\
In addition to leaf names also write the minimum and maximum\n\
(tree->GetMinumum(leafname)). This can take time for a tree\n\
with many entries as in case of Atlas D3PDs.\n\
____________________________________________________________\n\
Remember hist limits\n\
--------------------\n\
When displaying trees (ntuples) remember limits for histograms \n\
(number of channels, lower, upper edge)\n\
____________________________________________________________\n\
Always ask for hist limits\n\
--------------------------\n\
As default only the first time a variable (leaf) is\n\
displayed the histogram limits are shown and may be\n\
changed from a menu.\n\
____________________________________________________________\n\
Always recalculate hist limits\n\
------------------------------\n\
As default only the first time a variable (leaf) is\n\
displayed the histogram limits calculated since this\n\
requires to read the complete leaf\n\
____________________________________________________________\n\
Keep all hists (add version# to name)\n\
-------------------------------------\n\
As default histograms for the same leaf are overwritten\n\
if they are shown again\n\
____________________________________________________________\n\
Leaf Select Mask\n\
----------------\n\
See help for \"Histo/Leaf/Canvas mask\" on main window\n\
";
// *INDENT-ON*
	TList *row_lab = new TList();
	static void *valp[50];
	Int_t ind = 0;
	static Int_t dummy;
	row_lab->Add(new TObjString("CommentOnly_Options when showing trees:"));
	valp[ind++] = &dummy;
	row_lab->Add(new TObjString("CheckButton_  Always write leaf names to file"));
	valp[ind++] = &fWriteLeafNamesToFile;
	row_lab->Add(new TObjString("CheckButton_     Also write leaf [ Min,  Max]"));
	valp[ind++] = &fWriteLeafMinMaxtoFile;
	row_lab->Add(new TObjString("CheckButton_        Remember histogram limits"));
	valp[ind++] = &fRememberTreeHists;
	row_lab->Add(new TObjString("CheckButton_       Always ask for hist limits"));
	valp[ind++] = &fAlwaysRequestLimits;
	row_lab->Add(new TObjString("CheckButton_   Always recalculate hist limits"));
	valp[ind++] = &fAlwaysFindLimits;
	row_lab->Add(new TObjString("CheckButton_Keep all hists(add vers# to name)"));
	valp[ind++] = &fNtupleVersioning;
	row_lab->Add(new TObjString("CheckButton_        Prepend tree name to vars"));
	valp[ind++] = &fNtuplePrependTN;
	row_lab->Add(new TObjString("CheckButton_        Prepend file name to vars"));
	valp[ind++] = &fNtuplePrependFN;
	row_lab->Add(new TObjString("CheckButton_Append selections (cuts) to title"));
	valp[ind++] = &fAppendSelections;
	row_lab->Add(new TObjString("CheckButton_               Show 2dim as graph"));
	valp[ind++] = &f2dimAsGraph;
	row_lab->Add(new TObjString("Mark_Select_MarkStyle"));
	valp[ind++] = &fMarkStyle;
	row_lab->Add(new TObjString("ColorSelect+Color"));
	valp[ind++] = &fMarkColor;
	row_lab->Add(new TObjString("Float_Value+Size"));
	valp[ind++] = &fMarkSize;
	row_lab->Add(new TObjString("StringValue_Leaf Select Mask"));
	valp[ind++] = &fLeafSelMask;
	row_lab->Add(new TObjString("CheckButton_Use Regexp"));
	valp[ind++] = &fLeafUseRegexp;
	TGWindow *w = win;
	if ( w == NULL)
		w = fRootCanvas;
	static Int_t ok;
	Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
	fDialogShowTree =
		new TGMrbValuesAndText("Set Show tree options",
			  NULL, &ok,itemwidth, win,
			  NULL, NULL, row_lab, valp,
			  NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

void HistPresent::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *c = (TCanvas *)obj;
//   cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << c->GetName() << ")";
//   cout << endl;
}
//________________________________________________________________________

void HistPresent::CloseDown(Int_t wid)
{
	cout << "HistPresent::CloseDown(" << wid << ")" << endl;
	fDialogShowTree= NULL;
	SaveOptions();
}

//________________________________________________________________________________________

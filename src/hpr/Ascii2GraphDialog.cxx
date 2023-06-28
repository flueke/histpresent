#include "TROOT.h"
#include "TRootCanvas.h"
#include "TEnv.h"
#include "TPad.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TString.h"
#include "TGFont.h"
#include "TGWindow.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TGraph2D.h"
#include "TH2D.h"
#include "HTCanvas.h"
#include "GraphAttDialog.h"
#include "SetHistOptDialog.h"
#include "Ascii2GraphDialog.h"
#include "FitOneDimDialog.h"
#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "hprbase.h"
#include "SetColor.h"
//#include "support.h"
#include <fstream>
extern TString gHprLocalEnv;
//____________________________________________________________________________

void ExecGausFitG(TGraph * graph, Int_t type)
{
   if (!gPad || !graph) return;
   graph->SetBit(kMustCleanup);
   new FitOneDimDialog(graph, type);
}
//______________________________________________________________________________________
//_______________________________________________________________________________________

ClassImp(Ascii2GraphDialog)

Ascii2GraphDialog::Ascii2GraphDialog(TGWindow * win, Int_t winx,  Int_t winy, Int_t dim)
              : fWinx(winx), fWiny(winy), fDim(dim)
{

static const Char_t helpText[] =
"Read values from ASCII file and construct a graph\n\
Input data can have the formats:\n\
X, Y:                     simple graph, no errors\n\
X, Y, Ex, Ey:             symmetric errors in X and/or Y\n\
Note: if only 3 values are given the 3rd is assumed to be Ey\n\
X, Y, Exl, Exu, Eyl, Eyu: asymmetric errors in X and Y\n\
   Values after Y may be ommitted, they are set to 0\n\
\n\
Lines not starting with a number are skipped as comment\n\
(To be exact: not TString::IsFloat())\n\
\n\
Select columns:\n\
   Select 2, 3 or 4 columns to be used as X, Y, Ex, Ey\n\
   This is not implemented for the case of assymetric\n\
   errors.\n\
   White space or comma are used as separators\n\
   Unused columns may contain any characters \n\
   Columns are counted from 1\n\
The graph can be drawn / overlayed in a selected pad.\n\
Default is to construct a new canvas\n\
";
static const Char_t helpText2d[] =
"Read values from ASCII file and construct a TGraph2d\n\
Input data have the format:\n\
X, Y, Z\n\
Select columns:\n\
   Select 3 columns to be used as X, Y, Z\n\
   White space or comma are used as separators\n\
   Unused columns may contain any characters \n\
   Columns are counted from 1\n\
";

   static void *valp[50];
   Int_t ind = 0;
   fGraphSerialNr = 0;
   fUseXaxisMax=0;
   fUseYaxisMax=0;
   fUseZaxisMax=0;
   fCanvas = NULL;
   fGraph1D = NULL;
   fGraph2D = NULL;
   fCommand = "Draw_The_Graph()";
   fCommandHead = "Show_Head_of_File()";
   fCommandTail = "Show_Tail_of_File()";
   fCommandSave = "WriteoutGraph()";
   RestoreDefaults();
   
   if ( fXaxisMax <= fXaxisMin || fMinMaxZero != 0) {
		fXaxisMax = fXaxisMin = 0;
	}
   fGraphSelPad = 0;    // start with new canvas as default
   TList *row_lab = new TList();
   if ( fDim == 1) {
		row_lab->Add(new TObjString("RadioButton_Simple:X,Y no Errs"));
		row_lab->Add(new TObjString("RadioButton+ErrorS:X,Y,(Ex),Ey"));
		row_lab->Add(new TObjString("RadioButton_Asym Error: X,Y,Exl,Exu,Eyl,Eyu "));
		row_lab->Add(new TObjString("RadioButton_Empty pad only"));
		row_lab->Add(new TObjString("CheckButton+Select cols,X,Y"));
//		row_lab->Add(new TObjString("RadioButton+Simple: X, Y draw as hist"));
	}
   if ( fDim == 2) 
		row_lab->Add(new TObjString("CheckButton_Select columns,X,Y,Z"));
   row_lab->Add(new TObjString("PlainIntVal_Col_X"));
   row_lab->Add(new TObjString("PlainIntVal+Col_Y"));
   if ( fDim == 1) {
		row_lab->Add(new TObjString("PlainIntVal+Col_Ex"));
		row_lab->Add(new TObjString("PlainIntVal+Col_Ey"));
   } else {
		row_lab->Add(new TObjString("PlainIntVal+Col_Z"));
	}
   row_lab->Add(new TObjString("FileRequest_Inputfile"));
   row_lab->Add(new TObjString("StringValue_GraphName"));
   row_lab->Add(new TObjString("CheckButton-Use Name as title"));
   row_lab->Add(new TObjString("StringValue_Title X"));
   row_lab->Add(new TObjString("StringValue+Title Y  "));
   if ( fDim == 2) 
		row_lab->Add(new TObjString("StringValue+Title Z"));
   row_lab->Add(new TObjString("DoubleValue_Xax min"));
   row_lab->Add(new TObjString("DoubleV+CbU+Xax max"));
   row_lab->Add(new TObjString("DoubleValue_Yax min"));
   row_lab->Add(new TObjString("DoubleV+CbU+Yax max"));
   if ( fDim == 2) {
		row_lab->Add(new TObjString("DoubleValue_Zax min"));
		row_lab->Add(new TObjString("DoubleV+CbU+Zax max"));
	}
//   row_lab->Add(new TObjString("CheckButton_Force auto Xmin/Xmax"));
   row_lab->Add(new TObjString("PlainIntVal_Xsize canvas"));
   row_lab->Add(new TObjString("PlainIntVal+Ysize canvas"));
   row_lab->Add(new TObjString("PlainIntVal_Div X canvas"));
   row_lab->Add(new TObjString("PlainIntVal+Div Y canvas"));
   row_lab->Add(new TObjString("CheckButton+Use sel pad"));
   if (fDim == 1) {
		row_lab->Add(new TObjString("CheckButton_Marker    "));
		row_lab->Add(new TObjString("CheckButton+SimpleLine"));
		row_lab->Add(new TObjString("CheckButton+SmoothLine"));
		row_lab->Add(new TObjString("Float_Value_MarkSize "));
		row_lab->Add(new TObjString("Mark_Select+MarkStyle"));
		row_lab->Add(new TObjString("ColorSelect+MarkColor"));
		row_lab->Add(new TObjString("PlainShtVal_LineWidth"));
		row_lab->Add(new TObjString("LineSSelect+LStyle   "));
		row_lab->Add(new TObjString("ColorSelect+LineColor"));
		row_lab->Add(new TObjString("CheckButton_Fill area"));
		row_lab->Add(new TObjString("Fill_Select+FillStyle"));
		row_lab->Add(new TObjString("ColorSelect+FillColor"));
		row_lab->Add(new TObjString("ComboSelect_ErrMod; (default);X(no Err);Z(no XErr);>;|>;||(only end);2;3;4"));
		row_lab->Add(new TObjString("Float_Value+EndErrS "));
		row_lab->Add(new TObjString("Float_Value+X ErrS"));
	}
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt+Show_Tail_of_File"));
   row_lab->Add(new TObjString("CommandButt_Draw_the_Graph"));
   row_lab->Add(new TObjString("CommandButt+Save_Graph_to_File"));
//   row_lab->Add(new TObjString("CommandButt_Cancel"));
//      Int_t nrows = row_lab->GetSize();

   if ( fDim == 1) {
		valp[ind++] = &fGraph_Simple;
		valp[ind++] = &fGraph_Error;
		valp[ind++] = &fGraph_AsymError;
		valp[ind++] = &fEmptyPad;
//		valp[ind++] = &fGraph_AsHist;
	}
   valp[ind++] = &fGraphColSelect;
   valp[ind++] = &fGraphColSelX;
   valp[ind++] = &fGraphColSelY;
   if ( fDim == 1) {
		valp[ind++] = &fGraphColSelEx;
		valp[ind++] = &fGraphColSelEy;
   } else  {
		valp[ind++] = &fGraphColSelZ;
	}
   valp[ind++] = &fGraphFileName;
   valp[ind++] = &fGraphName;
   valp[ind++] = &fGraphShowTitle;
   valp[ind++] = &fGraphXtitle;
   valp[ind++] = &fGraphYtitle;
   if ( fDim == 2) 
		valp[ind++] = &fGraphZtitle;
   valp[ind++] = &fXaxisMin;
   valp[ind++] = &fXaxisMax;
   valp[ind++] = &fUseXaxisMax;
   valp[ind++] = &fYaxisMin;
   valp[ind++] = &fYaxisMax;
   valp[ind++] = &fUseYaxisMax;
   if (fDim ==2) {
		valp[ind++] = &fZaxisMin;
		valp[ind++] = &fZaxisMax;
		valp[ind++] = &fUseZaxisMax;
	}
//   valp[ind++] = &fMinMaxZero;
   valp[ind++] = &fGraphXsize;
   valp[ind++] = &fGraphYsize;
   valp[ind++] = &fGraphXdiv;
   valp[ind++] = &fGraphYdiv;
   valp[ind++] = &fGraphSelPad;
   if (fDim == 1){
		valp[ind++] = &fGraphPolyMarker;
		valp[ind++] = &fGraphSimpleLine;
		valp[ind++] = &fGraphSmoothLine;
		valp[ind++] = &fGraphMarkerSize;
		valp[ind++] = &fGraphMarkerStyle;
		valp[ind++] = &fGraphMarkerColor;
		valp[ind++] = &fGraphLineWidth;
		valp[ind++] = &fGraphLineStyle;
		valp[ind++] = &fGraphLineColor;
		valp[ind++] = &fGraphFill;
		valp[ind++] = &fGraphFillStyle;
		valp[ind++] = &fGraphFillColor;
		valp[ind++] = &fErrorMode;
		valp[ind++] = &fEndErrorSize;
		valp[ind++] = &fErrorX;
	}
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fCommandTail;
   valp[ind++] = &fCommand;
   valp[ind++] = &fCommandSave;
   const char *ht = NULL;
   if (fDim == 1) {
		ht = &helpText[0];
	} else {
		ht = &helpText2d[0];
	}
   Int_t ok;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText("Graphs parameters", NULL, &ok,itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, ht, this, this->ClassName());
   fDialog->Move(100,100);
//	if ( ok ) ;
};
//_________________________________________________________________________

Ascii2GraphDialog::~Ascii2GraphDialog()
{
   SaveDefaults();
};
//_________________________________________________________________________

void Ascii2GraphDialog::Draw_The_Graph()
{
	if (fGraphColSelect && fGraph_AsymError) {
		cout << setred << "Column select not implemented for Asym Errors" 
			<< setblack << endl;
		return;
	}
	TEnv env(gHprLocalEnv);
	fGraphLogX        = env.GetValue("GraphAttDialog.fGraphLogX", 0);
	fGraphLogY        = env.GetValue("GraphAttDialog.fGraphLogY", 0);
	fGraphLogZ        = env.GetValue("GraphAttDialog.fGraphLogZ", 0);
	HistPresent *hpr = (HistPresent*)gROOT->GetList()->FindObject("mypres");
	cout << endl;
	if (fEmptyPad != 0) {
		cout << "Empty pad only" << endl;
		fGraph1D = new TGraph();
		fCanvas = new HTCanvas("Empty", "Empty", fWinx, fWiny,
							fGraphXsize, fGraphYsize, hpr, NULL, fGraph1D);
		if ( fGraphLogX )
			 fCanvas->SetLogx();
		else
			fCanvas->SetLogx(kFALSE);
		if ( fGraphLogY )
			fCanvas->SetLogy();
		else
			fCanvas->SetLogy(kFALSE);
      if (fGraphXdiv > 1 || fGraphYdiv > 1) {
         fCanvas->Divide(fGraphXdiv, fGraphYdiv);
         fCanvas->cd(1);
      }
      Double_t xmin = 0, xmax = 100;
      Double_t ymin = 0, ymax = 100;
      if (fUseXaxisMax > 0 && (fXaxisMin != 0 || fXaxisMax != 0)) {
         xmin = fXaxisMin;
         xmax = fXaxisMax;
         ymin = fYaxisMin;
         ymax = fYaxisMax;
      }
			
      gStyle->SetOptStat(0);
      TH1F * gh = new TH1F(fGraphName, fGraphName, 100, xmin, xmax);
      gh->Draw();
      gh->SetMinimum(ymin);
      gh->SetMaximum(ymax);
      if (fGraphXtitle.Length() > 0)
         gh->GetXaxis()->SetTitle(fGraphXtitle.Data());
      if (fGraphYtitle.Length() > 0)
         gh->GetYaxis()->SetTitle(fGraphYtitle.Data());
      fGraph1D->SetHistogram(gh);
      gPad->Update();
      return;
   }

   TArrayD xval(100), yval(100), zval(100), wval(100), eyl(100), eyh(100);
   ifstream infile;
   infile.open(fGraphFileName.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "Ascii2GraphDialog: "
			<< gSystem->GetError() << " - " << fGraphFileName.Data()
			<< endl;
		return;
	}
   Int_t n = 0;
   TArrayD x(6);

   TString line;
   TString del(" ,\t");
   TObjArray * oa;
   TString val; 
	Double_t xmin_val = 1e20, xmax_val = -1e20;
	Double_t ymin_val = 1e20, ymax_val = -1e20;
	Double_t zmin_val = 1e20, zmax_val = -1e20;
	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
//		if (line.BeginsWith("#") ) 
//			continue;
		// check for DOS format
		if ( line.EndsWith("\r") ) {
			line.Resize(line.Length() - 1);
		}
		oa = line.Tokenize(del);
		Int_t nent = oa->GetEntries();
		if (nent < 1)
			continue;
		val = ((TObjString*)oa->At(0))->String();
		if ( !val.IsFloat() ) {
			cout << "Comment: " << line << endl;
			continue;
		} 
      if (nent < 2) {
         cout << "Not enough entries at: " << n+1 << endl;
         break;
      }
      if (fDim == 2 && nent < 3) {
         cout << "Not enough entries at: " << n+1 << endl;
         break;
      }
      if ( nent > x.GetSize() ) { 
			x.Set(nent);
		}
		for (Int_t i = 0; i < nent; i++) {
			val = ((TObjString*)oa->At(i))->String();
			
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << n+1 << endl;
				x[i] = 0;
			   break;
         }
			x[i] = val.Atof();
		}
      if (fGraphColSelect) {
			if ( fGraphColSelX <= 0 || fGraphColSelY <= 0) {
				cout << "Columns are numbered from 1 " << n << endl;
				return;
         }
			if ( fDim == 2 && fGraphColSelZ <= 0) {
				cout << "Columns are numbered from 1 " << n << endl;
				return;
         }
			if ( fGraphColSelX > nent || fGraphColSelY > nent) {
				cout << "Not enough entries " << nent << " at: " << n << endl;
				break;
         }
			if (  fDim == 2 && fGraphColSelZ > nent) {
				cout << "Not enough entries " << nent << " at: " << n << endl;
				break;
         }
         xval.AddAt(x[fGraphColSelX-1], n);
			if (x[fGraphColSelX-1] < xmin_val) xmin_val = x[fGraphColSelX-1];
			if (x[fGraphColSelX-1] > xmax_val) xmax_val = x[fGraphColSelX-1];
         yval.AddAt(x[fGraphColSelY-1], n);
			if (x[fGraphColSelY-1] < ymin_val) ymin_val = x[fGraphColSelY-1];
			if (x[fGraphColSelY-1] > ymax_val) ymax_val = x[fGraphColSelY-1];
			if (fGraph_Error == 1 ) {
				if (fGraphColSelEx > 0)
					zval.AddAt(x[fGraphColSelEx-1], n);
				if (fGraphColSelEy > 0)
					wval.AddAt(x[fGraphColSelEy-1], n);
			}
			if (fDim == 2) {
				zval.AddAt(x[fGraphColSelZ-1], n);
				if (x[fGraphColSelZ-1] < zmin_val) zmin_val = x[fGraphColSelZ-1];
				if (x[fGraphColSelZ-1] > zmax_val) zmax_val = x[fGraphColSelZ-1];
			}
			n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	yval.Set(n+100);
         	zval.Set(n+100);
         	wval.Set(n+100);
         	eyl.Set(n+100);
         	eyh.Set(n+100);
         }
      } else {
         xval.AddAt(x[0], n);
			if (x[0] < xmin_val) xmin_val = x[0];
			if (x[0] > xmax_val) xmax_val = x[0];
         yval.AddAt(x[1], n);
			if (x[1] < ymin_val) ymin_val = x[1];
			if (x[1] > ymax_val) ymax_val = x[1];
         if (fDim == 2) {
				zval.AddAt(x[2], n);
				if (x[2] < zmin_val) zmin_val = x[2];
				if (x[2] > zmax_val) zmax_val = x[2];
			} else {	
	//       if only 3 values  assume x, y, ye
				if (nent == 3) {
					x[3] = x[2];
					x[2] = 0;
				}
				if (fGraph_Error == 1 || fGraph_AsymError == 1) {
					zval.AddAt(x[2], n);
					wval.AddAt(x[3], n);
					if ( fGraph_AsymError == 1 ) {
						eyl.AddAt(x[4], n);
						eyh.AddAt(x[5], n);
					}
				}
			}
      	n++;
      	if (n >= xval.GetSize()){
         	xval.Set(n+100);
         	yval.Set(n+100);
         	zval.Set(n+100);
         	wval.Set(n+100);
         	eyl.Set(n+100);
         	eyh.Set(n+100);
      	}
   	}
   }
   infile.close();

   if (n < 1) return;
//	if (fMinMaxZero != 0 ) {
//		fXaxisMax = fXaxisMin = 0;
//	}
   Double_t dx_low = TMath::Abs(xval[n-1] - xval[n-2]);
   Double_t dx_up  = TMath::Abs(xval[n-1] - xval[n-2]);
   if (fUseXaxisMax == 0) {
		fXaxisMax = xmax_val + 0.5 * dx_up;
		fXaxisMin = xmin_val - 0.5 * dx_low;
	}
   cout << "entries " << n << " fXaxisMin " <<fXaxisMin 
			<< " fXaxisMax " <<fXaxisMax <<   endl;
   TString hname = fGraphFileName;
   hname = gSystem->BaseName(hname);
   Int_t ip = hname.Index(".");
   if (ip > 0) hname.Resize(ip);
   TString htitle = hname;
   htitle.Prepend("Values from ");
   hname.Prepend("g_");
	TString cname = hname;
	cname.Prepend("C_");
	TObject * cc = gROOT->GetListOfCanvases()->FindObject(cname.Data());
	if (cc != NULL) {
		cname += "_";
		cname += fGraphSerialNr ++;
	}
//  TGraph part
	fGraph2D = NULL;
   TH1 * gh = NULL;
   if (fDim == 2 ) {
		fGraph2D = new TGraph2D(n, xval.GetArray(), yval.GetArray(),
												zval.GetArray());
		fCanvas = new HTCanvas(cname, htitle, fWinx, fWiny,
									fGraphXsize, fGraphYsize, hpr, NULL, fGraph2D);
		if ( fGraphLogX )
			fCanvas->SetLogx();
		else
			fCanvas->SetLogx(kFALSE);
		if ( fGraphLogY )
			fCanvas->SetLogy();
		else
			fCanvas->SetLogy(kFALSE);
		fGraph2D->Draw("TRI2");
//		TH2D *h2g = fGraph2D->GetHistogram();
		if (fUseXaxisMax) {
			fGraph2D->GetHistogram()->GetXaxis()->SetRangeUser(fXaxisMin, fXaxisMax);
		}
		if (fUseYaxisMax) {
			fGraph2D->GetHistogram()->GetYaxis()->SetRangeUser(fYaxisMin, fYaxisMax);
		}
		if (fUseZaxisMax) {
//			cout <<fZaxisMin << " " << fZaxisMax<< endl;
			fGraph2D->GetHistogram()->GetZaxis()->SetRangeUser(fZaxisMin, fZaxisMax);
		}
		gh = (TH1*)fGraph2D->GetHistogram();
		if (fGraphName.Length() <= 0) {
			fGraph2D->SetName(fGraphFileName.Data());
			fGraph2D->SetTitle(fGraphFileName.Data());
		} else  {
			fGraph2D->SetName(fGraphName.Data());
			fGraph2D->SetTitle(fGraphName.Data());
		}
		gPad->Modified();
		gPad->Update();
	} else {
		if (fGraph_Simple == 1){
			fGraph1D = new TGraph(n, xval.GetArray(), yval.GetArray());
		} else if (fGraph_Error == 1) {
			fGraph1D = new TGraphErrors(n, xval.GetArray(), yval.GetArray(),
												 zval.GetArray(), wval.GetArray());
		} else if (fGraph_AsymError == 1) {
			fGraph1D = new TGraphAsymmErrors(n, xval.GetArray(), yval.GetArray(),
														zval.GetArray(), wval.GetArray(),
														eyl.GetArray(),  eyh.GetArray());
		} 
		if (fGraph1D) {
			if (fGraphName.Length() <= 0) {
				fGraph1D->SetName(fGraphFileName.Data());
				fGraph1D->SetTitle(fGraphFileName.Data());
			} else  {
				fGraph1D->SetName(fGraphName.Data());
				fGraph1D->SetTitle(fGraphName.Data());
			}

			TString drawopt(fErrorMode);           // draw axis as default
			if (drawopt.Index("(") > 0)
				drawopt.Resize(drawopt.Index("("));
			drawopt += "A";
			if (fGraphPolyMarker == 0 && fGraphSmoothLine == 0
				&& fGraphSimpleLine == 0 ) {
					drawopt+= "L";
			} else {
				if (fGraphPolyMarker) drawopt+= "P";
				if (fGraphSmoothLine)
					drawopt+= "C";
				else if (fGraphSimpleLine)
					drawopt+= "L";
			}
	//		else if (fGraphBarChart)
	//			drawopt+= "B";
			if (fGraphFill && TMath::Abs(fGraphLineWidth) < 100) {
				drawopt+= "F";
				if ( fGraphFillStyle == 0 )
					fGraphFillStyle = 3001;
			}
			if ( fGraphShowTitle )
				gStyle->SetOptTitle(kTRUE);
			else
				gStyle->SetOptTitle(kFALSE);
	//         cout << "gPad->GetName() " <<gPad->GetName() << endl;
			if (fGraphSelPad) {
				Int_t ngr = FindGraphs(gPad, NULL, NULL);
				if (ngr > 0) {
					TString oo(drawopt);
					Int_t inda = drawopt.Index("A", 0, TString::kIgnoreCase);
					if (inda>=0) drawopt.Remove(inda,1);
	//            cout << "oo: " << oo<< endl;

					drawopt += "SAME";
					fGraph1D->Draw(drawopt);
					if (fGraphXtitle.Length() > 0)
						fGraph1D->GetHistogram()->GetXaxis()->SetTitle(fGraphXtitle.Data());
					if (fGraphYtitle.Length() > 0)
						fGraph1D->GetHistogram()->GetYaxis()->SetTitle(fGraphYtitle.Data());

				} else {
					fGraph1D->Draw(drawopt);
				}
	//            gPad->Modified();
	//            gPad->Update();
			} else {
				fCanvas = new HTCanvas(cname, htitle, fWinx, fWiny,
									fGraphXsize, fGraphYsize, hpr, NULL, fGraph1D);
				if ( fGraphLogX )
					fCanvas->SetLogx();
				else
					fCanvas->SetLogx(kFALSE);
				if ( fGraphLogY )
					fCanvas->SetLogy();
				else
					fCanvas->SetLogy(kFALSE);
				if (fGraphXdiv > 1 || fGraphYdiv > 1) {
					fCanvas->Divide(fGraphXdiv, fGraphYdiv);
					fCanvas->cd(1);
				}
				fDrawOpt = drawopt;
				fGraph1D->Draw(drawopt);
	//            gPad->Modified();
	//            gPad->Update();
			}
	//      TEnv env(gHprLocalEnv);

			gh = fGraph1D->GetHistogram();

			fGraph1D->SetMarkerStyle(fGraphMarkerStyle);
			fGraph1D->SetMarkerColor(fGraphMarkerColor);
			fGraph1D->SetMarkerSize(fGraphMarkerSize);
			fGraph1D->SetLineStyle(fGraphLineStyle);
			fGraph1D->SetLineColor(fGraphLineColor);
			fGraph1D->SetFillStyle(fGraphFillStyle);
			fGraph1D->SetFillColor(fGraphFillColor);
			fGraph1D->SetLineWidth(fGraphLineWidth);
			if (fUseXaxisMax != 0) fGraph1D->GetXaxis()->SetLimits(fXaxisMin, fXaxisMax);
			if (fUseYaxisMax != 0) fGraph1D->SetMinimum(fYaxisMin);
			if (fUseYaxisMax != 0) fGraph1D->SetMaximum(fYaxisMax);
			gPad->Modified();
			gPad->Update();
			cout << "TGraph *gr = (TGraph*)" << fGraph1D << "; << opt: " << fDrawOpt<< endl;
		}
	}
	if (gh != NULL) {
//		cout << "gh " << gh << endl;
//		gh->Print();
		if (fGraphXtitle.Length() > 0){
			gh->GetXaxis()->SetTitle(fGraphXtitle.Data());
			if ( env.GetValue("SetHistOptDialog.fTitleCenterX", 1))
				gh->GetXaxis()->CenterTitle(kTRUE);
		}
		if (fGraphYtitle.Length() > 0){
			gh->GetYaxis()->SetTitle(fGraphYtitle.Data());
			if ( env.GetValue("SetHistOptDialog.fTitleCenterY", 1))
				gh->GetYaxis()->CenterTitle(kTRUE);
		}
		if (fDim == 2 && fGraphZtitle.Length() > 0) {
			gh->GetZaxis()->SetTitle(fGraphZtitle.Data());
			if ( env.GetValue("SetHistOptDialog.fTitleCenterZ", 1))
				gh->GetZaxis()->CenterTitle(kTRUE);
		}
		gh->SetLineWidth(1);
		gh->SetLineColor(0);
		gPad->Modified();
		gPad->Update();
	}
   SaveDefaults();
   return;
};
//_______________________________________________________________________________________

Int_t Ascii2GraphDialog::FindGraphs(TVirtualPad * ca, TList * logr, 
												TList * pads)
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
          while (TObject * obj2 = next2()) {
             if (obj2->InheritsFrom("TGraph")) {
                ngr++;
                if (logr) logr->Add(obj2);
                if (pads) pads->Add(p);
             }
          }
      }
   }
   return ngr;
};

//_________________________________________________________________________

void Ascii2GraphDialog::SaveDefaults()
{
   TEnv env(gHprLocalEnv);
   env.SetValue("Ascii2GraphDialog.fEmptyPad"  	    , fEmptyPad        );
   env.SetValue("Ascii2GraphDialog.Graph_Simple"	 , fGraph_Simple    );
   env.SetValue("Ascii2GraphDialog.Graph_AsHist"	 , fGraph_AsHist    );
   env.SetValue("Ascii2GraphDialog.Graph_Error" 	 , fGraph_Error     );
   env.SetValue("Ascii2GraphDialog.Graph_AsymError" , fGraph_AsymError );
   env.SetValue("Ascii2GraphDialog.fGraphColSelect" , fGraphColSelect  );
   env.SetValue("Ascii2GraphDialog.fGraphColSelX"   , fGraphColSelX    );
   env.SetValue("Ascii2GraphDialog.fGraphColSelY"   , fGraphColSelY    );
   env.SetValue("Ascii2GraphDialog.fGraphColSelEx"  , fGraphColSelEx   );
   env.SetValue("Ascii2GraphDialog.fGraphColSelEy"  , fGraphColSelEy   );
   env.SetValue("Ascii2GraphDialog.fGraphColSelZ"   , fGraphColSelZ    );
	if ( !fGraphFileName.BeginsWith("/") ) {
		fGraphFileName.Prepend("/");
		fGraphFileName.Prepend(gSystem->pwd());
		cout << "fGraphFileName: " << fGraphFileName << endl;
	}
   env.SetValue("Ascii2GraphDialog.GraphFileName"   , fGraphFileName   );
   env.SetValue("Ascii2GraphDialog.GraphName"		 , fGraphName       );
   env.SetValue("Ascii2GraphDialog.GraphSelPad" 	 , fGraphSelPad     );
	env.SetValue("Ascii2GraphDialog.fGraphShowTitle" , fGraphShowTitle  );
	env.SetValue("Ascii2GraphDialog.GraphNewPad" 	 , fGraphNewPad     );
   env.SetValue("Ascii2GraphDialog.GraphXsize"  	 , fGraphXsize      );
   env.SetValue("Ascii2GraphDialog.GraphYsize"  	 , fGraphYsize      );
   env.SetValue("Ascii2GraphDialog.GraphXtitle" 	 , fGraphXtitle     );
   env.SetValue("Ascii2GraphDialog.GraphYtitle" 	 , fGraphYtitle     );
   env.SetValue("Ascii2GraphDialog.GraphZtitle" 	 , fGraphZtitle     );
   env.SetValue("Ascii2GraphDialog.fMinMaxZero"  	 , fMinMaxZero       );
   env.SetValue("Ascii2GraphDialog.XaxisMin"  		 , fXaxisMin        );
   env.SetValue("Ascii2GraphDialog.YaxisMin"  		 , fYaxisMin        );
   env.SetValue("Ascii2GraphDialog.ZaxisMin"  		 , fZaxisMin        );
   env.SetValue("Ascii2GraphDialog.XaxisMax"  		 , fXaxisMax        );
   env.SetValue("Ascii2GraphDialog.YaxisMax"  		 , fYaxisMax        );
   env.SetValue("Ascii2GraphDialog.ZaxisMax"  		 , fZaxisMax        );
   env.SetValue("Ascii2GraphDialog.GraphXdiv"		 , fGraphXdiv       );
   env.SetValue("Ascii2GraphDialog.GraphYdiv"		 , fGraphYdiv       );
   env.SetValue("Ascii2GraphDialog.fErrorMode"		 , fErrorMode       );
   env.SetValue("Ascii2GraphDialog.fErrorX"			 , fErrorX          );
   env.SetValue("Ascii2GraphDialog.fEndErrorSize"   , fEndErrorSize    );
	env.SetValue("Ascii2GraphAttDialog.fGraphFill"   , fGraphFill       );
	env.SetValue("GraphAttDialog.fDrawOpt"           , fDrawOpt         );
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Ascii2GraphDialog::RestoreDefaults()
{
   TEnv env(gHprLocalEnv);
   fEmptyPad         = env.GetValue("Ascii2GraphDialog.fEmptyPad"  	   , 0);
   fGraph_Simple     = env.GetValue("Ascii2GraphDialog.Graph_Simple"  	, 0);
   fGraph_AsHist     = env.GetValue("Ascii2GraphDialog.Graph_AsHist"  	, 0);
   fGraph_Error      = env.GetValue("Ascii2GraphDialog.Graph_Error"		, 1);
   fGraph_AsymError  = env.GetValue("Ascii2GraphDialog.Graph_AsymError" , 0);
   fGraphColSelect   = env.GetValue("Ascii2GraphDialog.fGraphColSelect" , 0);
   fGraphColSelX     = env.GetValue("Ascii2GraphDialog.fGraphColSelX"   , -1);
   fGraphColSelY     = env.GetValue("Ascii2GraphDialog.fGraphColSelY"   , -1);
   fGraphColSelEx    = env.GetValue("Ascii2GraphDialog.fGraphColSelEx"  , -1);
   fGraphColSelEy    = env.GetValue("Ascii2GraphDialog.fGraphColSelEy"  , -1);
   fGraphColSelZ     = env.GetValue("Ascii2GraphDialog.fGraphColSelZ"   , -1);
   fGraphFileName    = env.GetValue("Ascii2GraphDialog.GraphFileName" 	, "hs.dat");
   fGraphName        = env.GetValue("Ascii2GraphDialog.GraphName"  		, "hs");
   fGraphSelPad      = env.GetValue("Ascii2GraphDialog.GraphSelPad"		, 0);
	fGraphShowTitle   = env.GetValue("Ascii2GraphDialog.fGraphShowTitle" , 0);
	fGraphNewPad      = env.GetValue("Ascii2GraphDialog.GraphNewPad"		, 1);
   fGraphXsize       = env.GetValue("Ascii2GraphDialog.GraphXsize" 		, 800);
   fGraphYsize       = env.GetValue("Ascii2GraphDialog.GraphYsize" 		, 800);
   fGraphXtitle      = env.GetValue("Ascii2GraphDialog.GraphXtitle"	   , "Xval");
   fGraphZtitle      = env.GetValue("Ascii2GraphDialog.GraphYtitle"		, "Yval");
   fGraphZtitle      = env.GetValue("Ascii2GraphDialog.GraphZtitle"		, "Zval");
   fGraphXdiv        = env.GetValue("Ascii2GraphDialog.GraphXdiv"  		, 1);
   fMinMaxZero       = env.GetValue("Ascii2GraphDialog.fMinMaxZero"		, 0 );
   fXaxisMin         = env.GetValue("Ascii2GraphDialog.XaxisMin"  		, 0.);
   fYaxisMin         = env.GetValue("Ascii2GraphDialog.YaxisMin"  		, 0.);
   fZaxisMin         = env.GetValue("Ascii2GraphDialog.ZaxisMin"  		, 0.);
   fXaxisMax         = env.GetValue("Ascii2GraphDialog.XaxisMax"  		, 0.);
//   cout <<"fXaxisMin,fXaxisMax " << fXaxisMin << " " << fXaxisMax << endl;
   fYaxisMax         = env.GetValue("Ascii2GraphDialog.YaxisMax"  		, 0.);
   fZaxisMax         = env.GetValue("Ascii2GraphDialog.ZaxisMax"  		, 0.);
   fGraphYdiv        = env.GetValue("Ascii2GraphDialog.GraphYdiv"  		, 1);
	fDrawOpt          = env.GetValue("GraphAttDialog.fDrawOpt"           , "PA");
   fErrorMode        = env.GetValue("Ascii2GraphDialog.fErrorMode"		, " ");
   fErrorX           = env.GetValue("Ascii2GraphDialog.fErrorX"			, 0.5);
   fEndErrorSize     = env.GetValue("Ascii2GraphDialog.fEndErrorSize"   , 1);
	fGraphSmoothLine = 0;
	fGraphSimpleLine = 0;
	if (fDrawOpt.Contains("C")) {
		fGraphSmoothLine = 1;
	} else if (fDrawOpt.Contains("L")) {
		fGraphSimpleLine = 1;
	}
	fGraphPolyMarker = 0;
	if (fDrawOpt.Contains("P")) {
		fGraphPolyMarker= 1;
	}
	fGraphLineStyle   = env.GetValue("GraphAttDialog.fLineStyle",  1);
	fGraphLineWidth   = env.GetValue("GraphAttDialog.fLineWidth",  1);
	fGraphLineColor   = env.GetValue("GraphAttDialog.fLineColor",  1);
	fGraphMarkerStyle = env.GetValue("GraphAttDialog.fMarkerStyle",7);
	fGraphMarkerSize  = env.GetValue("GraphAttDialog.fMarkerSize", 1);
	fGraphMarkerColor = env.GetValue("GraphAttDialog.fMarkerColor",1);
	fGraphFillStyle   = env.GetValue("GraphAttDialog.fFillStyle",  0);
	fGraphFillColor   = env.GetValue("GraphAttDialog.fFillColor",  1);
	fGraphFill        = env.GetValue("GraphAttDialog.fGraphFill",  0);
}
//_________________________________________________________________________
void Ascii2GraphDialog::WriteoutGraph()
{
	TRootCanvas *cimp = NULL;
	if (fCanvas != NULL)
		cimp = (TRootCanvas *)fCanvas->GetCanvasImp();
	if (fGraph1D) {
		Save2FileDialog sfd(fGraph1D, NULL, cimp);
	} else if (fGraph2D) {
		Save2FileDialog sfd(fGraph2D, NULL, cimp);
	} else {
		cout << "No graph defined" << endl;
		return;
	}
}
//_________________________________________________________________________

void Ascii2GraphDialog::Show_Head_of_File()
{
   TString cmd(fGraphFileName.Data());
   cmd.Prepend("head ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2GraphDialog::Show_Tail_of_File()
{
   TString cmd(fGraphFileName.Data());
   cmd.Prepend("tail ");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________

void Ascii2GraphDialog::CloseDown(Int_t /*wid*/)
{
//    cout << "Ascii2GraphDialog::CloseDown() " << endl;
   delete this;
}

#include "TROOT.h"
#include "TButton.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TRootCanvas.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1.h"
#include "TH3.h"
#include "TView.h"
#include "TView3D.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPolyMarker3D.h"
#include "TVirtualHistPainter.h"
#include "TGLHistPainter.h"

#include "hprbase.h"
#include "support.h"
#include "Set3DimOptDialog.h"
#include <iostream>

extern Int_t gHprDebug;

Double_t gTranspThresh = 1;
Double_t gTranspAbove = 0.4;
Double_t gTranspBelow = 0.005;

Double_t my_transfer_function(const Double_t *x, const Double_t * /*param*/)
{
   // Bin values in our example range from -2 to 1.
   // Let's make values from -2. to -1.5 more transparent:
   if (*x < gTranspThresh) {
      return gTranspBelow;
	} else{
		return gTranspAbove;
	}
}


namespace std {} using namespace std;

//TString Set3DimOptDialog::fDrawOpt3Dim = "COLZ";
//_______________________________________________________________________
/*
#if ROOTVERSION > 60000

Set3DimOptDialog::Set3DimOptDialog()
{
	cout << "ctor Set2DimOptDialog, non interactive" <<endl;
	fDialog = NULL;
}
#endif
*/
//_______________________________________________________________________

Set3DimOptDialog::Set3DimOptDialog(TGWindow * win, TButton *b)
{
static const Char_t helptext[] =
"\n\
\"Scatter \"  : Draw a scatter-plot, swarm of points\n\
BOX   : a box is drawn for each cell with surface proportional to the\n\
          content's absolute value. \n\
\n\
OpenGL: Use Open Gl \n\
To enhance part of the plot the transparency of the bins may be adjusted\n\
according to their bin content. Typically the treshold is set to one\n\
the transparency \"below\" to a small value (transparent) and a \"above\"\n\
to e.g. 0.5. In this way non filled bin are invisible.\n\
\n\
PolyM:  Custom (HistPresent) method. A 3d PolyMarker is drawn\n\
        Colors are coded according to cell content. Ranges, min max\n\
        may be set and changed. Logarithmic color coding may be used\n\
        however - at least currently - no pallette axis is drawn\n\
        in this case since that would conflict with the Z-axis.\n\
\"View3D\":  ROOTs standard TView3D\n\
\n\
Caveat: When switching between display modes the histogram will be\n\
redisplayed.\n\
\n\
A first attempt to automatically rotate the picture is implemented\n\
by simulating the movement of the pressed mouse\n\
\n\
The histogram displayed may be saved with the current ranges from the\n\
File popup menu, however only when shown with Scatter or Box option\n\
since the other options dont have a real histogram anymore\n\
";
   const char *fDrawOpt3[kNopt3] =
   {"SCAT", "BOX0", "GLCOL", "PolyMHist"};
   const char *fDrawOpt3Title[kNopt3] =
   {"Scatter", "BOX", "OpenGL", "PolyM"};
   
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	fCmdButton = b;
//	cout << "fCmdButton " << fCmdButton<< endl;
	fApplyTranspCut = 0;
   fHist = NULL;
	fView3D = NULL;
	fRangeChanged = kFALSE;
	fContMin = 0;
	fContMax = 1.e10;

   Int_t nh1 = 0, nh2 = 0;
   TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TH1")) {
         fHist = (TH1*)obj;
         if (obj->InheritsFrom("TGraph")) {
            fHist = ((TGraph*)obj)->GetHistogram();
            if (!fHist) continue;
		   }
         if (fHist->GetDimension() == 1) nh1++;
         if (fHist->GetDimension() > 1)  nh2++;
		}
   }
	if ( fHist == NULL ) {
		fView3D = (TView3D*)fCanvas->GetView();

		if ( fView3D ) {
			Double_t *rmin = fView3D->GetRmin();
			Double_t *rmax = fView3D->GetRmax();
			fRangeX0 = rmin[0];
			fRangeX1 = rmax[0];
			fRangeY0 = rmin[1];
			fRangeY1 = rmax[1]; 
			fRangeZ0 = rmin[2];
			fRangeZ1 = rmax[2];
		} else {
	     cout << "No Hist nore TView3D in Canvas" << endl;
		  return;
		}
	} else {
		TAxis * xa = fHist->GetXaxis();
		TAxis * ya = fHist->GetYaxis();
		TAxis * za = fHist->GetZaxis();
		fRangeX0 = xa->GetBinLowEdge(xa->GetFirst());
		fRangeX1 = xa->GetBinLowEdge(xa->GetLast())+ xa->GetBinWidth(xa->GetLast());
		fRangeY0 = ya->GetBinLowEdge(ya->GetFirst());
		fRangeY1 = ya->GetBinLowEdge(ya->GetLast())+ ya->GetBinWidth(ya->GetLast());
		fRangeZ0 = za->GetBinLowEdge(za->GetFirst());
		fRangeZ1 = za->GetBinLowEdge(za->GetLast())+ za->GetBinWidth(za->GetLast());
		fContMin = fHist->GetMinimum();
		fContMax = fHist->GetMaximum();
	}
//	if ( fHist )
//		fHist->Dump();
   RestoreDefaults();
	fPhi3Dim = fCanvas->GetPhi();
	fTheta3Dim = fCanvas->GetTheta();
//   cout << "Set3DimOptDialog::ctor RestoreDefaults|"<<fDrawOpt3Dim<<"|" << endl;
//	GetValuesFromHist();
   Int_t selected = -1;
   for (Int_t i = 0; i < kNopt3; i++) {
      fDrawOpt3DimArray[i] = fDrawOpt3[i];
		TString temp(fDrawOpt3[i]);
		temp = temp.Strip(TString::kBoth);
//		cout << "Set3DimOptDialog::ctor temp|" << temp << "|"<< endl;
      if (fDrawOpt3Dim.Contains(temp.Data(), TString::kIgnoreCase) ) {
         if (selected < 0) {
				fOptRadio[i] = 1;
            selected = i;
         }
      } else {
         fOptRadio[i] = 0;
      }
   }
	
   fRow_lab = new TList();

   Int_t ind = 0;
   Int_t indopt = 0;
	static Int_t dummy;
   static TString stycmd("SaveDefaults()");
   static TString sadcmd("SetAllToDefault()");
   static TString rotcmd("Rotate()");
   static TString srgcmd("SetRanges()");

	fBidSCAT = 0;
	fBidBOX  = 1;
	fBidGL   = 2;
	fBidPolyM   = 3;
	fBidView3D  = 4;
	fRotDx = 1;
	fRotDy = 0;
	fRotSleep = 0;
	fRotCycles = 10;
   for (Int_t i = 0; i < kNopt3; i++) {
       TString text("RadioButton");
       if (i == 0)text += "_";
       else       text += "+";
       text += fDrawOpt3Title[i];
       fRow_lab->Add(new TObjString(text.Data()));
       fValp[ind++] = &fOptRadio[indopt++];
   }
//   cout << " indopt  " <<  indopt << endl;
	fDrawOptPrev = fDrawOpt3Dim;
   fRow_lab->Add(new TObjString("ColorSelect_BgColor"));
   fValp[ind++] = &f3DimBackgroundColor;
   fRow_lab->Add(new TObjString("ColorSelect+FillC"));
   fBidFillColor = ind; fValp[ind++] = &fHistFillColor3Dim;
   fRow_lab->Add(new TObjString("ColorSelect+LineC"));
   fBidLineColor = ind; fValp[ind++] = &fHistLineColor3Dim;

   fRow_lab->Add(new TObjString("ColorSelect_MColor"));
   fBidMarkerColor = ind; fValp[ind++] = &fMarkerColor3Dim;
   fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
   fBidMarkerStyle = ind; fValp[ind++] = &fMarkerStyle3Dim;
   fRow_lab->Add(new TObjString("Float_Value+MSize;0.01,9.99"));
   fBidMarkerSize = ind; fValp[ind++] = &fMarkerSize3Dim;
	
	fRow_lab->Add(new TObjString("CommentOnly_Display ranges"));
	fValp[ind++] = &dummy;
   fRow_lab->Add(new TObjString("CommandButt-Set ranges, min, max"));
   fValp[ind++] = &srgcmd;
   fRow_lab->Add(new TObjString("DoubleValue_X1"));
	fBidRangeX0 = ind;
   fValp[ind++] = &fRangeX0;
   fRow_lab->Add(new TObjString("DoubleValue+Y1"));
   fValp[ind++] = &fRangeY0;
   fRow_lab->Add(new TObjString("DoubleValue+Z1"));
   fValp[ind++] = &fRangeZ0;
   fRow_lab->Add(new TObjString("DoubleValue_X2"));
   fValp[ind++] = &fRangeX1;
   fRow_lab->Add(new TObjString("DoubleValue+Y2"));
   fValp[ind++] = &fRangeY1;
   fRow_lab->Add(new TObjString("DoubleValue+Z2"));
	fBidRangeZ1 = ind;
   fValp[ind++] = &fRangeZ1;
   fRow_lab->Add(new TObjString("DoubleValue_Cont_Min"));
	fBidContMin = ind;
   fValp[ind++] = &fContMin;
   fRow_lab->Add(new TObjString("DoubleValue-Max"));
	fBidContMax= ind;
   fValp[ind++] = &fContMax;
   fRow_lab->Add(new TObjString("CheckButton-Log"));
 	fBidContLog= ind;
	fValp[ind++] = &fContLog;
   fRow_lab->Add(new TObjString("CheckButton-ZScale"));
 	fBidShowZScale = ind;
	fValp[ind++] = &fShowZScale;
	
	fRow_lab->Add(new TObjString("CommentOnly_Parameters for OpenGL"));
	fValp[ind++] = &dummy;
	fRow_lab->Add(new TObjString("CheckButton_Apply Transpareny Thr"));
	fValp[ind++] = &fApplyTranspCut;
   fRow_lab->Add(new TObjString("DoubleValue+Threshold"));
   fValp[ind++] = &gTranspThresh;
   fRow_lab->Add(new TObjString("DoubleValue_Transp below"));
   fValp[ind++] = &gTranspBelow;
   fRow_lab->Add(new TObjString("DoubleValue+Transp above"));
	fBidTranspAbove = ind;
   fValp[ind++] = &gTranspAbove;
   fRow_lab->Add(new TObjString("DoubleValue_View Phi"));
	fBidPhi = ind;
   fValp[ind++] = &fPhi3Dim;
   fRow_lab->Add(new TObjString("DoubleValue+View Theta"));
	fBidTheta = ind;
   fValp[ind++] = &fTheta3Dim;
	
   fRow_lab->Add(new TObjString("PlainIntVal_Rot Dx"));
   fValp[ind++] = &fRotDx;
   fRow_lab->Add(new TObjString("PlainIntVal+Rot Dy"));
   fValp[ind++] = &fRotDy;
   fRow_lab->Add(new TObjString("PlainIntVal+Rot Cycles"));
   fValp[ind++] = &fRotCycles;
//   fRow_lab->Add(new TObjString("PlainIntVal+Rot Sleep"));
//   fValp[ind++] = &fRotSleep;
   fRow_lab->Add(new TObjString("CommandButt_Execute Rotate"));
   fValp[ind++] = &rotcmd;

   fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
   fValp[ind++] = &stycmd;
   fRow_lab->Add(new TObjString("CommandButt+Reset all to default"));
   fValp[ind++] = &sadcmd;

   static Int_t ok;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//_______________________________________________________________________

Set3DimOptDialog::~Set3DimOptDialog()
{
	if (fCanvas){
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
	}
}
//_______________________________________________________________________

void Set3DimOptDialog::CloseDialog()
{
//   cout << "Set3DimOptDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if (fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void Set3DimOptDialog::SetRanges()
{
	if (fRangeX0 >= fRangeX1 || fRangeY0 >= fRangeY1 
		|| fRangeZ0 >= fRangeZ1) {
		cout << "Illegal range:" << endl;
		cout << fRangeX0<< " " <<fRangeY0<< " " <<fRangeZ0
		<< " " <<fRangeX1<< " " <<fRangeY1<< " " <<fRangeZ1 << endl;
		return;
	}
	TString cname = fCanvas->GetName();
	cout << "Canvas name: " << cname << endl;
	if ( !cname.BeginsWith("C_F") ) {
		cout << "Illegal canvas name " << endl;
	} else {
		cname = cname(3,cname.Length()-3);
		cout << "Canvas name: " << cname << endl;
		TEnv * env = GetDefaults(cname, kFALSE);
		if (env) {
			env->SetValue("fRangeX0", fRangeX0);
			env->SetValue("fRangeX1", fRangeX1);
			env->SetValue("fRangeY0", fRangeY0);
			env->SetValue("fRangeY1", fRangeY1);
			env->SetValue("fRangeZ0", fRangeZ0);
			env->SetValue("fRangeZ1", fRangeZ1);
			env->SaveLevel(kEnvLocal);
		}
	}
	Double_t *rmin;
	Double_t *rmax;
	if ( fView3D ) {
		rmin = fView3D->GetRmin();
		rmax = fView3D->GetRmax();
		if (fRangeX0 >= rmin[0] && fRangeX1 <= rmax[0] &&
			fRangeY0 >= rmin[1] && fRangeY1 <= rmax[1] &&
			fRangeZ0 >= rmin[2] && fRangeZ1 <= rmax[2]) {
		} else {
			cout << "New ranges exceed current, must redisplay view" << endl;
			return;
		}
	}
	fRangeChanged = kTRUE;
	SetHistAtt(fCanvas);
}
//_______________________________________________________________________

void Set3DimOptDialog::SetHistAttNow(TCanvas *canvas)
{
	if (gHprDebug > 0) 
		cout << "SetHistAttNow fDrawOpt3Dim " << fDrawOpt3Dim << endl;
	
//	if (!fDrawOpt3Dim.Contains("GL")){
//		cout << "switching from non GL" << endl;
//		fDrawOpt3Dim.Prepend("GLCOL");
//		return;       // when switching from non GL redraw of canvas is needed
//	}
   if (!canvas) return;
   Set3DimOptDialog::SetHistAtt(canvas);
}
//_______________________________________________________________________

void Set3DimOptDialog::SetHistAtt(TCanvas *canvas)
{
   if (!canvas) return;
	canvas->cd();
	TList *lop = canvas->GetListOfPrimitives();
	TIter next(lop);
   TObject *obj;
   if (!fDrawOpt3Dim.Contains("Z", TString::kIgnoreCase) )
		fDrawOpt3Dim += "Z";
   fDrawOpt = fDrawOpt3Dim;
// 	if (fUseGL && ( fDrawOpt.Contains("BOX") ||fDrawOpt.Contains("ISO"))) {
// 		fDrawOpt.Prepend("GLCOL");
// 		gStyle->SetCanvasPreferGL(kTRUE);
// 
// 	} else {
// 		gStyle->SetCanvasPreferGL(kFALSE);
// 	}
		
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TGraph2D")) {
         ((TGraph2D*)obj)->SetDrawOption(fDrawOpt);
      } else if (obj->InheritsFrom("TH3")) {
			TList * lof = ((TH3*)obj)->GetListOfFunctions();
			TObject * trf= lof->FindObject("TransferFunction");
			if ( fDrawOpt.Contains("GL") && fApplyTranspCut ) {
				if ( !trf ) {
					TF1 * tf = new TF1("TransferFunction", my_transfer_function);
					lof->Add(tf);
				}
			} else {
				if (trf) {
					lof->Remove(trf);
					delete trf;
				}
			}
			if ( fHist ) {
				((TH3*)obj)->GetXaxis()->SetRangeUser(fRangeX0, fRangeX1);
				((TH3*)obj)->GetYaxis()->SetRangeUser(fRangeY0, fRangeY1);
				((TH3*)obj)->GetZaxis()->SetRangeUser(fRangeZ0, fRangeZ1);
			}
         ((TH3*)obj)->SetDrawOption(fDrawOpt);
         ((TH3*)obj)->SetOption(fDrawOpt);
			((TH3*)obj)->SetFillColor(fHistFillColor3Dim);
			((TH3*)obj)->SetLineColor(fHistLineColor3Dim);
			((TH3*)obj)->SetLineWidth(1);
			((TH3*)obj)->SetLineStyle(1);
			((TH3*)obj)->SetFillStyle(0);
			((TH3*)obj)->SetMarkerColor(fMarkerColor3Dim);  
			((TH3*)obj)->SetMarkerStyle(fMarkerStyle3Dim);  
			((TH3*)obj)->SetMarkerSize (fMarkerSize3Dim); 
// 			if (fDrawOpt.Contains("PolyM") || fDrawOpt.Contains("View")){
// 				TIter next2(canvas->GetListOfPrimitives());
// 				TObject *obj2;
// 				while ( (obj2 = next2()) ) {
// 					if (obj2->InheritsFrom("TPolyMarker3D")){
// 						((TPolyMarker3D*)obj2)->SetMarkerSize (fMarkerSize3Dim); 
// 						((TPolyMarker3D*)obj2)->SetMarkerStyle (fMarkerStyle3Dim); 
// 					}
// 				}
// 			}
		} else if (obj->InheritsFrom("TPolyMarker3D")){
			TPolyMarker3D* pm3 = (TPolyMarker3D*)obj;
			if (pm3->GetMarkerSize() > 0) {
				pm3->SetMarkerSize (fMarkerSize3Dim); 
				pm3->SetMarkerStyle (fMarkerStyle3Dim);
			}
      } else if ((obj->InheritsFrom("TPad"))) {
         TPad *pad = (TPad*)obj;
         TIter next1(pad->GetListOfPrimitives());
         TObject *obj1;
         pad->cd();
			while ( (obj1 = next1()) ) {
				if (obj1->InheritsFrom("TH3")) {
               ((TH3*)obj1)->SetDrawOption(fDrawOpt);
               ((TH3*)obj1)->SetOption(fDrawOpt);
            }
         }
	      pad->Modified();
	      pad->Update();
      }
   }
	if ( fRangeChanged ) {
		TPolyLine3D * pl3d = NULL;
		if (fView3D)
			fView3D->SetRange(fRangeX0, fRangeY0, fRangeZ0, fRangeX1, fRangeY1, fRangeZ1);
		TIter next1(canvas->GetListOfPrimitives());
		TList tempdel;
		TPolyMarker3D *pm;
		Double_t pmx=0, pmy=0, pmz=0;
		while ( (obj = next1()) ) {
			if (obj->InheritsFrom("TPolyMarker3D")) {
				pm = (TPolyMarker3D*)obj;
				if ( pm->GetN() == 1 ) {
					pm->GetPoint(0,pmx, pmy, pmz);
					Double_t cont = (Double_t)pm->GetUniqueID();
					if (  pmx < fRangeX0 || pmx > fRangeX1 
						|| pmy < fRangeY0 || pmy > fRangeX1
						|| pmz < fRangeZ0 || pmz > fRangeZ1
						|| cont < fContMin || cont > fContMax) {
						pm->SetMarkerSize(0);
					} else {
						pm->SetMarkerSize(fMarkerSize3Dim);
					}
//						tempdel.Add(pm);
//					}
				}
			}
			if (obj->InheritsFrom("TPolyLine3D")) {
				pl3d = (TPolyLine3D*)obj;
			}
		}
		if (tempdel.GetSize() > 0) {
			TIter next2(&tempdel);
			while ( (obj = next2()) ) {
				lop->Remove(obj);
				delete obj;
			}
		}
		if ( pl3d ) {
			fCanvas->cd();
			Hpr::BoundingB3D(pl3d, fRangeX0, fRangeY0, fRangeZ0, 
								  fRangeX1, fRangeY1, fRangeZ1);
		}
		
	}
   if (f3DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         canvas->GetFrame()->SetFillStyle(0);
      } else {
      	canvas->GetFrame()->SetFillStyle(1001);
      	canvas->GetFrame()->SetFillColor(10);
   	}
   } else {
      canvas->GetFrame()->SetFillStyle(1001);
      canvas->GetFrame()->SetFillColor(f3DimBackgroundColor);
   }
   fRangeChanged = kFALSE;
	canvas->Pop();
	canvas->Modified();
	canvas->Update();
}

//______________________________________________

//void Set3DimOptDialog::Rotate(Int_t dx, Int_t dy, Int_t sleep, Int_t ncycles)
void Set3DimOptDialog::Rotate()
{
// 0, 2 rotate about x axis
// 2, 0        about y axis
	Int_t cont_cycles = 1;
//	Int_t tot_cycles = fRotCycles;
	TGLHistPainter *hp = NULL;
	if ( fHist ) {
	TVirtualHistPainter* vp  = fHist->GetPainter();
		if (vp->InheritsFrom("TGLHistPainter") ) {
	//   if ( vp->IsA() == TGLHistPainter::Class() ) {
			hp = (TGLHistPainter *)fHist->GetPainter();
	//		rotate();
		}
	}
	TView *view = NULL;
	if ( !hp) {
		view=fCanvas->GetView();
//		tot_cycles = fRotCycles * 20;
	} else {
		// with OpenGL picture moves with mouse
		// otherwise picture is updated with kButton1Up only
		cont_cycles = 20;
	}
	if ( hp == NULL && view == NULL ) {
		cout << "No usable view found" << endl;
		return;
	}
	fCanvas->cd();
	for (Int_t i = 0; i < fRotCycles; i++) {
		// start in the middle of the plot
		Int_t ix = fCanvas->GetWw() / 2;
		Int_t iy = fCanvas->GetWh() / 2;
		if ( hp )
			hp->ExecuteEvent(kButton1Down, ix, iy);
		else if (view)
			view->ExecuteEvent(kButton1Down, ix, iy);
		fCanvas->Modified();
		fCanvas->Update();
		for (Int_t k = 0; k < cont_cycles; k++) {
			ix += fRotDx;
			iy += fRotDy;
			if ( hp )
				hp->ExecuteEvent(kButton1Motion, ix, iy);
			else if ( view ) {
				view->ExecuteEvent(kButton1Motion, ix, iy);
				fCanvas->Modified();
				fCanvas->Update();
			}
//			gSystem->Sleep(fRotSleep);
		}
		if ( hp )
			hp->ExecuteEvent(kButton1Up, ix, iy);
		else if ( view )
			view->ExecuteEvent(kButton1Up, ix, iy);
		fCanvas->Modified();
		fCanvas->Update();
	}
}
//______________________________________________________________________

void Set3DimOptDialog::SaveDefaults()
{
	if (gHprDebug > 0) 
		cout << "Set3DimOptDialog::SaveDefaults()  "<< endl;
   TEnv env(".hprrc");
   env.SetValue("Set3DimOptDialog.fDrawOpt3Dim",      fDrawOpt3Dim);
	env.SetValue("Set3DimOptDialog.f3DimBackgroundColor",f3DimBackgroundColor);
	env.SetValue("Set3DimOptDialog.fHistFillColor3Dim",fHistFillColor3Dim);
	env.SetValue("Set3DimOptDialog.fHistLineColor3Dim",fHistLineColor3Dim);
	env.SetValue("Set3DimOptDialog.fMarkerColor3Dim",  fMarkerColor3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerStyle3Dim",  fMarkerStyle3Dim  );
	env.SetValue("Set3DimOptDialog.fMarkerSize3Dim",   fMarkerSize3Dim   );
//	env.SetValue("Set3DimOptDialog.fUseGL",            fUseGL            );
	env.SetValue("Set3DimOptDialog.fApplyTranspCut",   fApplyTranspCut   );
	env.SetValue("Set3DimOptDialog.gTranspThresh",     gTranspThresh);
	env.SetValue("Set3DimOptDialog.gTranspBelow",      gTranspBelow );
	env.SetValue("Set3DimOptDialog.gTranspAbove",      gTranspAbove );
	env.SetValue("Set3DimOptDialog.fPhi3Dim",          fPhi3Dim          );
	env.SetValue("Set3DimOptDialog.fTheta3Dim",        fTheta3Dim        );
	env.SetValue("Set3DimOptDialog.fShowZScale",           fShowZScale           );
 	//env.SetValue("Set3DimOptDialog.fRangeX0",          fRangeX0);
 	//env.SetValue("Set3DimOptDialog.fRangeX1",          fRangeX1);	
 	//env.SetValue("Set3DimOptDialog.fRangeY0",          fRangeY0);
 	//env.SetValue("Set3DimOptDialog.fRangeY1",          fRangeY1);
 	//env.SetValue("Set3DimOptDialog.fRangeZ0",          fRangeZ0);
 	//env.SetValue("Set3DimOptDialog.fRangeZ1",          fRangeZ1);
	env.SaveLevel(kEnvLocal);
}

//______________________________________________________________________

void Set3DimOptDialog::SetAllToDefault()
{
   cout << "Set3DimOptDialog::SetAllToDefault()  "<< endl;
	RestoreDefaults(1);
}
//______________________________________________________________________

void Set3DimOptDialog::GetValuesFromHist()
{
   if ( !fHist ) return;

	fDrawOpt3Dim = fHist->GetOption();
}
//______________________________________________________________________

void Set3DimOptDialog::RestoreDefaults(Int_t resetall)
{
	if (gHprDebug > 0) 
		cout << "Set3DimOptDialog:: RestoreDefaults(resetall) " << resetall<< endl;
	TString envname;
	if (resetall == 0 ) {
		envname = ".hprrc";
	} else {
		// force use of default values by giving an empty resource file
		gSystem->TempFileName(envname);
	}
   TEnv env(envname);
	fDrawOpt3Dim       = env.GetValue("Set3DimOptDialog.fDrawOpt3Dim",     "BOX");
	f3DimBackgroundColor = env.GetValue("Set3DimOptDialog.f3DimBackgroundColor", 0);
	fHistFillColor3Dim = env.GetValue("Set3DimOptDialog.fHistFillColor3Dim", 1);
	fHistLineColor3Dim = env.GetValue("Set3DimOptDialog.fHistLineColor3Dim", 1);
	fMarkerColor3Dim   = env.GetValue("Set3DimOptDialog.fMarkerColor3Dim",   1);
	fMarkerStyle3Dim   = env.GetValue("Set3DimOptDialog.fMarkerStyle3Dim",   1);
	fMarkerSize3Dim    = env.GetValue("Set3DimOptDialog.fMarkerSize3Dim",    0.8);
//	fUseGL             = env.GetValue("Set3DimOptDialog.fUseGL",             0);
	fApplyTranspCut    = env.GetValue("Set3DimOptDialog.fApplyTranspCut",    1);
	gTranspThresh      = env.GetValue("Set3DimOptDialog.gTranspThresh",      1);
	gTranspBelow       = env.GetValue("Set3DimOptDialog.gTranspBelow",   0.005);
	gTranspAbove       = env.GetValue("Set3DimOptDialog.gTranspAbove",     0.9);
//	gAlphaOfPalette = (Int_t)(gTranspAbove*255.);
	fPhi3Dim           = env.GetValue("Set3DimOptDialog.fPhi3Dim", 135.);
	fTheta3Dim         = env.GetValue("Set3DimOptDialog.fTheta3Dim", 30);
	//fContMin           = env.GetValue("Set3DimOptDialog.fContMin", fContMin);
	//fContMax           = env.GetValue("Set3DimOptDialog.fContMax", fContMax);
	fContLog           = env.GetValue("Set3DimOptDialog.fContLog", 0);
	fShowZScale        = env.GetValue("Set3DimOptDialog.fShowZScale", 1);
 	//fRangeX0           = env.GetValue("Set3DimOptDialog.fRangeX0", fRangeX0);
 	//fRangeX1           = env.GetValue("Set3DimOptDialog.fRangeX1", fRangeX1);
 	//fRangeY0           = env.GetValue("Set3DimOptDialog.fRangeY0", fRangeY0);
 	//fRangeY1           = env.GetValue("Set3DimOptDialog.fRangeY1", fRangeY1);
 	//fRangeZ0           = env.GetValue("Set3DimOptDialog.fRangeZ0", fRangeZ0);
 	//fRangeZ1           = env.GetValue("Set3DimOptDialog.fRangeZ1", fRangeZ1);
}
//______________________________________________________________________

void Set3DimOptDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void Set3DimOptDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject *obj)
{
   TCanvas *canvas = (TCanvas *)obj;
   for (Int_t i = 0; i < kNopt3; i++) {
      if (fOptRadio[i] == 1) {
			fDrawOpt3Dim = fDrawOpt3DimArray[i];
		}
	}
	if (gHprDebug > 0) {
		cout << "CRButtonPressed fDrawOpt3Dim " << fDrawOpt3Dim << endl;
		if (obj) cout  << ", " << canvas->GetName() << ")";
		cout << endl;
	}
	
	if ( bid == fBidContMax ||bid == fBidContMin || bid == fBidContLog) {
		TEnv env(".hprrc");
		if ( bid == fBidContMin ) {
			env.SetValue("Set3DimOptDialog.fContMin", fContMin);
//			fHist->SetMinimum(fContMin);
//			cout << "CRButtonPressed: " << fHist << " " << fContMin << endl;
		}
		if ( bid == fBidContMax ) {
			env.SetValue("Set3DimOptDialog.fContMax", fContMax);
//			fHist->SetMaximum(fContMax);
		}
		if ( bid == fBidContLog ) {
			env.SetValue("Set3DimOptDialog.fContLog", fContLog);
		}
		if ( bid == fBidShowZScale ) {
			env.SetValue("Set3DimOptDialog.fShowZScale", fShowZScale);
		}
		SetHistAttNow(canvas);
		env.SaveLevel(kEnvLocal);
	}
	// make sure marker is scalable
	if ( bid == fBidMarkerStyle) {
		if (fMarkerStyle3Dim == 1 || fMarkerStyle3Dim == 6
			|| fMarkerStyle3Dim == 7 ) {
				fMarkerStyle3Dim = 21;
			fDialog->ReloadValues();
			fDialog->DoNeedRedraw();
			cout << "Markerstyle forced scalable: " << fMarkerStyle3Dim << endl;
		}
	}
	if ( bid >= fBidRangeX0 && bid <= fBidRangeZ1 )
		return;
	
//	if ( bid == fBidTranspAbove ) {
//		gAlphaOfPalette = (Int_t)(gTranspAbove*255.);
//	}
	if ( bid > fBidPolyM && bid <= fBidTranspAbove ) {
		SetHistAttNow(canvas);
	}
	// when changeing between GL and non GL option, the histogram
	// must be redisplayed
	if (bid == fBidPhi || bid == fBidTheta && ! fDrawOpt3Dim.Contains("OpenGL")) {
		fCanvas->SetPhi(fPhi3Dim);
		fCanvas->SetTheta(fTheta3Dim);
		fCanvas->Modified();
		fCanvas->Update();
	}
	
	if (bid >= fBidSCAT && bid <= fBidView3D /*&& fDrawOpt3Dim != fDrawOptPrev*/) {
		fDrawOptPrev = fDrawOpt3Dim;
		SaveDefaults();
//		delete fCanvas;
		SysInfo_t sysinf;
		gSystem->GetSysInfo(&sysinf);
		
		gSystem->ProcessEvents();
		if ( fCmdButton && sysinf.fOS == "Linux") {
//			cout << "CRButtonPressed exe: " << fCmdButton->GetMethod() << endl;
			gROOT->ProcessLine(fCmdButton->GetMethod());
			gSystem->ProcessEvents();
		} else {
			cout << "Cant auto redisplay, please redisplay manually" << endl;
		}
	} else {
		fDrawOptPrev = fDrawOpt3Dim;
	}
}


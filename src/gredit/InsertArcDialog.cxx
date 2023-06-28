#include "TROOT.h"
#include "THprArc.h"
#include "THprEllipse.h"
#include "TCurlyLine.h"
#include "GrCanvas.h"
#include "TMarker.h"
#include "TMath.h"
#include "TEnv.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
//#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "InsertArcDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertArcDialog)

InsertArcDialog::InsertArcDialog()
{
static const Char_t helptext[] =
"This widget helps to insert an arc or an\n\
ellipse. For an ellipse the ration R2/R1 must be\n\
> 0 and determines the shape. An arc may be defined\n\
giving the center and a point on the circumference\n\
or two points on the circumference. If the value of\n\
radius is not 0 only the center must be marked  \n\
";
   fCanvas = gPad->GetCanvas();
   TRootCanvas* win = NULL;
   if (fCanvas) {
      win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "InsertArcDialog", this, "CloseDialog()");
   }
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
   RestoreDefaults();
   static TString bycenter("ArcByCenterAndRadius()");
   static TString byponcf("ArcByPointsOnCF()");
   fX1 = fX2 = fY1 = fY2 = 0;
//   fPhi1 = 0;
//   fPhi2 = 360;
   fRow_lab->Add(new TObjString("DoubleValue_Xc"));
   fValp[ind++] = &fXcenter;
   fRow_lab->Add(new TObjString("DoubleValue+Yc"));
   fValp[ind++] = &fYcenter;
   fRow_lab->Add(new TObjString("DoubleValue_R1"));
   fValp[ind++] = &fR1;
   fRow_lab->Add(new TObjString("DoubleValue+Ell:R2/R1"));
   fValp[ind++] = &fR2;
   fRow_lab->Add(new TObjString("DoubleValue_X1 on Cf"));
   fValp[ind++] = &fX1;
   fRow_lab->Add(new TObjString("DoubleValue+Y1 on Cf"));
   fValp[ind++] = &fY1;
   fRow_lab->Add(new TObjString("DoubleValue_X2 on Cf"));
   fValp[ind++] = &fX2;
   fRow_lab->Add(new TObjString("DoubleValue+Y2 on Cf"));
   fValp[ind++] = &fY2;
   fRow_lab->Add(new TObjString("DoubleValue_Phi1 on Cf"));
   fValp[ind++] = &fPhi1;
   fRow_lab->Add(new TObjString("DoubleValue+Phi2 on Cf"));
   fValp[ind++] = &fPhi2;
   fRow_lab->Add(new TObjString("CheckButton_Sense Clock"));
   fValp[ind++] = &fSense;
   fRow_lab->Add(new TObjString("CheckButton+Show Edges"));
   fValp[ind++] = &fShowEdges;
   fRow_lab->Add(new TObjString("CheckButton+Keep Radius"));
   fValp[ind++] = &fKeepRadius;
   fRow_lab->Add(new TObjString("ColorSelect_Color"));
   fValp[ind++] = &fColor;
   fRow_lab->Add(new TObjString("LineSSelect+Style"));
   fValp[ind++] = &fStyle;
   fRow_lab->Add(new TObjString("PlainShtVal+Width"));
   fValp[ind++] = &fWidth;
   fRow_lab->Add(new TObjString("ColorSelect_FillColor"));
   fValp[ind++] = &fFillColor;
   fRow_lab->Add(new TObjString("Fill_Select+FillStyle"));
   fValp[ind++] = &fFillStyle;
   fRow_lab->Add(new TObjString("CommandButt_ArcByCenterAndRadius()"));
   fValp[ind++] = &bycenter;
   fRow_lab->Add(new TObjString("CommandButt_ArcByPointsOnCircumference()"));
   fValp[ind++] = &byponcf;
   static Int_t ok;
   Int_t itemwidth =  50 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText("Arc, Circle, Ellipse,", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
       GrCanvas* hc = (GrCanvas*)fCanvas;
       hc->Add2ConnectedClasses(this);
   }
}
//______________________________________________________________________________

void InsertArcDialog::ArcByCenterAndRadius()
{
   TMarker *m1 = NULL;
   TMarker *m2 = NULL;
	Bool_t clear_textposition = kFALSE;
	if (fXcenter == 0 && fYcenter == 0) {
      GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   	cout << "Mark center with left mouse" << endl;
   	m1  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
		if (m1 == NULL) {
			return;
		}
		clear_textposition = kTRUE;
		if (htc && htc->GetUseEditGrid()) {
			m1->SetX(htc->PutOnGridX(m1->GetX()));
			m1->SetY(htc->PutOnGridY(m1->GetY()));
		}
   	fXcenter = m1->GetX();
   	fYcenter = m1->GetY();
   	m1->SetMarkerStyle(4);
   	m1->Draw();
      if (fR1 <= 0) {
   	   cout << "Mark radius with left mouse" << endl;
      	m2  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
			if (m2 == NULL) {
            if (m1) delete m1;
				return;
			}
         if (!m2) {if (m1) delete m1; return;};
			if (htc && htc->GetUseEditGrid()) {
				m2->SetX(htc->PutOnGridX(m2->GetX()));
				m2->SetY(htc->PutOnGridY(m2->GetY()));
			}
  		   fR1 = TMath::Sqrt( (fXcenter - m2->GetX()) * (fXcenter - m2->GetX())
                  	 + (fYcenter - m2->GetY()) * (fYcenter - m2->GetY()));
      }
   }
   if (m1) delete m1;
   if (m2) delete m2;
   if (fR2 <= 0) {
      THprArc *a = new THprArc(fXcenter, fYcenter, fR1, fPhi1, fPhi2);
      a->SetMustAlign(1);
//      el = (THprEllipse *)a;
 //     el->SetR2(fR1* GetRatioXY());
		a->Draw();
		a->SetLineColor(fColor);
		a->SetLineStyle(fStyle);
		a->SetLineWidth(fWidth);
		a->SetFillColor(fFillColor);
		a->SetFillStyle(fFillStyle);
		a->SetNoEdges(!fShowEdges);
   } else {
      THprEllipse *el = new THprEllipse(fXcenter, fYcenter, fR1, fR1 * fR2, fPhi1, fPhi2);
      el->SetR2(fR1* fR2 * GetRatioXY());
		el->Draw();
		el->SetLineColor(fColor);
		el->SetLineStyle(fStyle);
		el->SetLineWidth(fWidth);
		el->SetFillColor(fFillColor);
		el->SetFillStyle(fFillStyle);
		el->SetNoEdges(!fShowEdges);
   }
	if ( clear_textposition ) {
		fXcenter = fYcenter = 0;
	}
   if (fKeepRadius == 0) fR1 = 0;
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void InsertArcDialog::ArcByPointsOnCF()
{
   if (fR1 <= 0) {
      cout << "Radius must be defined and > 0" << endl;
      return;
   }
	Bool_t clear_textposition = kFALSE;
	if (fX1 == 0 && fY1 == 0 && fX2 == 0 && fY2 == 0) {
   	cout << "Mark first point with left mouse" << endl;
   	TMarker * m1  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
   	m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
      if (!m1) return;
   	fX1 = m1->GetX();
   	fY1 = m1->GetY();
   	m1->SetMarkerStyle(4);
   	m1->Draw();
   	cout << "Mark second point with left mouse" << endl;
   	TMarker * m2  = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
      if (!m2) {delete m1; return;};
   	m2 = (TMarker *)gPad->GetListOfPrimitives()->Last();
   	fX2 = m2->GetX();
   	fY2 = m2->GetY();
   	delete m1;
   	delete m2;
		clear_textposition = kTRUE;
   }
   GrCanvas * htc = dynamic_cast<GrCanvas*>(gPad);
   if (htc && htc->GetUseEditGrid()) {
      fX1 = htc->PutOnGridX(fX1);
      fY1 = htc->PutOnGridY(fY1);
      fX2 = htc->PutOnGridX(fX2);
      fY2 = htc->PutOnGridY(fY2);
   }
   Int_t sense;
   if (fSense > 0) sense = 1;
   else           sense = -1;
   Bool_t ok =  PoCftoCenterPhi(fX1, fY1, fX2, fY2, fR1, sense,
                       &fXcenter, &fYcenter, &fPhi1, &fPhi2);
   if (ok) {
		THprArc * a = new THprArc(fXcenter, fYcenter, fR1, fPhi1, fPhi2);
		a->Draw();
		a->SetLineColor(fColor);
		a->SetLineStyle(fStyle);
		a->SetLineWidth(fWidth);
		a->SetFillColor(fFillColor);
		a->SetFillStyle(fFillStyle);
		a->SetNoEdges(!fShowEdges);
   }
	if ( clear_textposition ) {
		fXcenter = fYcenter = fX1 = fX2 = fY1 = fY2 = 0;
		fPhi1 = 0;
		fPhi2 = 360;
	}
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void InsertArcDialog::SaveDefaults()
{
   cout << "InsertArcDialog::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".hprrc");
   env.SetValue("InsertArcDialog.fColor" , fColor);
   env.SetValue("InsertArcDialog.fStyle" , fStyle);
   env.SetValue("InsertArcDialog.fWidth" , fWidth);
   env.SetValue("InsertArcDialog.fFillColor"  , fFillColor);
   env.SetValue("InsertArcDialog.fFillStyle"  , fFillStyle);
   env.SetValue("InsertArcDialog.fXcenter"    , fXcenter );
   env.SetValue("InsertArcDialog.fYcenter"    , fYcenter );
   env.SetValue("InsertArcDialog.fR2"         , fR2 );
   env.SetValue("InsertArcDialog.fR1"         , fR1 );
   env.SetValue("InsertArcDialog.fR2"         , fR2 );
   env.SetValue("InsertArcDialog.fPhi1"  , fPhi1 );
   env.SetValue("InsertArcDialog.fPhi2"  , fPhi2 );
   env.SetValue("InsertArcDialog.fSense" , fSense);
	env.SetValue("InsertArcDialog.fShowEdges" , fShowEdges);
	env.SetValue("InsertArcDialog.fKeepRadius" , fKeepRadius);
	env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void InsertArcDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fColor     = env.GetValue("InsertArcDialog.fColor" , 1);
   fStyle     = env.GetValue("InsertArcDialog.fStyle" , 1);
   fWidth     = env.GetValue("InsertArcDialog.fWidth" , 2);
   fFillColor = env.GetValue("InsertArcDialog.fFillColor" , 1);
   fFillStyle = env.GetValue("InsertArcDialog.fFillStyle" , 1);
   fXcenter   = env.GetValue("InsertArcDialog.fXcenter"   , 0.);
   fYcenter   = env.GetValue("InsertArcDialog.fYcenter"   , 0.);
   fR1		  = env.GetValue("InsertArcDialog.fR1"  , 30.);
   fR2  	      = env.GetValue("InsertArcDialog.fR2"  , -1.);
   fPhi1			= env.GetValue("InsertArcDialog.fPhi1"  , 0.);
   fPhi2			= env.GetValue("InsertArcDialog.fPhi2"  , 360.);
   fSense		= env.GetValue("InsertArcDialog.fSense" , 1);
	fShowEdges	= env.GetValue("InsertArcDialog.fShowEdges", 0);
	fKeepRadius = env.GetValue("InsertArcDialog.fKeepRadius" , 1);
}
//_________________________________________________________________________

Bool_t InsertArcDialog::PoCftoCenterPhi(Double_t x1, Double_t y1,
                       Double_t x2, Double_t y2, Double_t r, Int_t sense,
                       Double_t *xcenter, Double_t *ycenter,
                       Double_t *phimin, Double_t *phimax)
{
   // calculate center, given 2 points on circle and radius
/*
   Double_t x1, x2, y1, y2, xc, yc, r;

   x1 = 4;
   y1 = 4;
   x2 = -4;
   y2 = 4;

   r  = TMath::Sqrt(32.);
*/
   std::cout << "PoCftoCenterPhi: " << x1<<" " << y1<< " "<< x2 <<" " <<y2 << " " << r << std::endl;
   Double_t xc, yc;
   Bool_t ok = kTRUE;
   if (x1 == x2) {
   	Double_t arg = r*r - (y2-y1)*(y2-y1)/4;
   	if (arg <= 0) {
      	std::cout << "No real solution" << std::endl;
      	xc = yc = 0;
         ok = kFALSE;
   	} else {
      	xc = x1 - sense * TMath::Sqrt(arg);
      	yc = 0.5*(y2+y1);
   	}
   } else {
   	Double_t a = 0.5 * ( x1*x1 - x2*x2 + y1*y1  - y2*y2) / (x1 - x2);
   	Double_t b =  (y1-y2) / (x2-x1);
   	Double_t A = 1 + b*b;
   	Double_t B = -2*y1 + 2*a*b - 2*b*x1;
   	Double_t C = a*a - 2*x1*a + x1*x1 + y1*y1 - r*r;
//      Double_t dist = TMath::Sqrt((x1 - x2)*(x1 - x2) + (y1-y2)*(y1-y2));
//   	std::cout << dist << " "  << A << " " << B << " " << C << std::endl;
      Double_t arg = B*B/4 - A*C;
      if (arg <=0 ) {
         std::cout << "No real solution" << std::endl;
         xc = yc = 0;
         ok = kFALSE;
      } else {
         yc = (-0.5*B  - sense * TMath::Sqrt(arg)) / A;
   	   xc = a + b * yc;
      }
   }
   Double_t phi1 = 0;
   Double_t phi2 = 0;
   if (ok) {
      Double_t cosa = (x1 - xc)/r;
      Double_t sina = (y1 - yc)/r;
      phi1  = TMath::ATan2(sina,cosa);
      if(phi1 < 0) phi1 += 2 * TMath::Pi();
      phi1 = phi1 * 180 / TMath::Pi();
      cosa = (x2 - xc)/r;
      sina = (y2 - yc)/r;
      phi2  = TMath::ATan2(sina,cosa);
      if(phi2 < 0) phi2 += 2 * TMath::Pi();
      phi2 = phi2 * 180 / TMath::Pi();
      std::cout << xc << " " << yc<< " " << phi1<< " " << phi2 << std::endl;
   }
   *xcenter = xc;
   *ycenter = yc;
   *phimin = phi1;
   *phimax = phi2;
   return ok;
}

Double_t InsertArcDialog::GetRatioXY()
{
	Double_t ww = (Double_t)gPad->GetWw();
	Double_t wh = (Double_t)gPad->GetWh();
	Double_t pxrange = gPad->GetAbsWNDC()*ww;
	Double_t pyrange = gPad->GetAbsHNDC()*wh;
	Double_t xrange  = gPad->GetX2() - gPad->GetX1();
	Double_t yrange  = gPad->GetY2() - gPad->GetY1();
	Double_t pixeltoX  = xrange / pxrange;
	Double_t pixeltoY  = yrange/pyrange;
   return pixeltoY / pixeltoX;
}
//_________________________________________________________________________

InsertArcDialog::~InsertArcDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if (fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
};
//_______________________________________________________________________

void InsertArcDialog::CloseDialog()
{
//   cout << "InsertArcDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void InsertArcDialog::CloseDown(Int_t wid)
{
//   cout << "InsertArcDialog::CloseDown()" << endl;
   if (wid != -2) SaveDefaults();
   delete this;
}


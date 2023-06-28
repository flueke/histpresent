#include "TROOT.h"
#include "THprArrow.h"
#include "THprCurlyArc.h"
#include "THprCurlyLine.h"
#include "THprGraph.h"
#include "THprLine.h"
#include "THprPolyLine.h"
#include "TEnv.h"
#include "TGraph.h"
#include "TRootCanvas.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TMarker.h"
#include "THprMarker.h"
#include "MarkerLineDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(MarkerLineDialog)

MarkerLineDialog::MarkerLineDialog()
{
static const Char_t helptext[] =
"Insert primitives:\n\
Marker: just click at desired position\n\
SimpleLine and Arrow: Click and drag to endpoint\n\
PolyLine: Click at desired points\n\
end with click at same position or double click\n\
";
   fCanvas = gPad->GetCanvas();
   TRootCanvas* win = NULL;
   if (fCanvas) {
      win = (TRootCanvas*)fCanvas->GetCanvasImp();
		win->Connect("CloseWindow()", "MarkerLineDialog", this, "CloseDialog()");
	}
   Int_t ind = 0;
   RestoreDefaults();
   fRow_lab = new TList();
   static TString mar("Marker()");
   static TString dia("Diamond()");
   static TString pol("PolyLine()");
   static TString sil("SimpleLine()");
   static TString arr("Arrow()");

   fRow_lab->Add(new TObjString("Float_Value_ASize"));
   fValp[ind++] = &fArrowSize;
   fRow_lab->Add(new TObjString("ArrowSelect+AShape"));
   fValp[ind++] = &fArrowStyle;
   fRow_lab->Add(new TObjString("ColorSelect+AColor"));
   fValp[ind++] = &fArrowColor;

   fRow_lab->Add(new TObjString("PlainShtVal_LWidth"));
   fValp[ind++] = &fLineWidth;
   fRow_lab->Add(new TObjString("LineSSelect+LStyle"));
   fValp[ind++] = &fLineStyle;
   fRow_lab->Add(new TObjString("ColorSelect+LColor"));
   fValp[ind++] = &fLineColor;

   fRow_lab->Add(new TObjString("Float_Value_MSize"));
   fValp[ind++] = &fMarkerSize;
   fRow_lab->Add(new TObjString("Mark_Select+MStyle"));
   fValp[ind++] = &fMarkerStyle;
   fRow_lab->Add(new TObjString("ColorSelect+MColor"));
   fValp[ind++] = &fMarkerColor;

   fRow_lab->Add(new TObjString("CommandButt_Marker"));
   fValp[ind++] = &mar;
   fRow_lab->Add(new TObjString("CommandButt+Arrow()"));
   fValp[ind++] = &arr;
   fRow_lab->Add(new TObjString("CommandButt_SimpleLine"));
   fValp[ind++] = &sil;
   fRow_lab->Add(new TObjString("CommandButt+PolyLine"));
   fValp[ind++] = &pol;
//   fRow_lab->Add(new TObjString("CommandButt+Diamond"));
//   fValp[ind++] = &dia;
   Int_t itemwidth =  40 * TGMrbValuesAndText::LabelLetterWidth();
   static Int_t ok;
   fDialog =
      new TGMrbValuesAndText("Marker / Polyline", NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
	if (fCanvas) {
       GrCanvas* hc = (GrCanvas*)fCanvas;
       hc->Add2ConnectedClasses(this);
   }
}
//______________________________________________________________________________

void MarkerLineDialog::Arrow()
{
   const char * ArrowOption[] =
      {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
   TArrow * a = (TArrow*)GrCanvas::WaitForCreate("TArrow", &fPad);
   if (a == NULL) {
      return;
   }
   THprArrow * aa = new THprArrow(a->GetX1(), a->GetY1(),
                                  a->GetX2(), a->GetY2());
   delete a;
   aa->Draw();
   aa->SetFillColor(fArrowColor);
   aa->SetLineColor(fArrowColor);
   aa->SetLineWidth(fLineWidth);
   aa->SetArrowSize(fArrowSize);
   aa->SetDrawOption(ArrowOption[fArrowStyle]);
   aa->SetOption(ArrowOption[fArrowStyle]);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void MarkerLineDialog::SimpleLine()
{
   TLine * a = (TLine*)GrCanvas::WaitForCreate("TLine", &fPad);
   if (a == NULL) {
      return;
   }
   THprLine * ha =  new THprLine(a->GetX1(), a->GetY1(),
                         a->GetX2(), a->GetY2());
   delete a;
   ha->Draw();
   ha->SetLineColor(fLineColor);
   ha->SetLineWidth(fLineWidth);
   ha->SetLineStyle(fLineStyle);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________
void MarkerLineDialog::Marker()
{
   TMarker * a = (TMarker*)GrCanvas::WaitForCreate("TMarker", &fPad);
   if (a == NULL) {
      return;
   }
   THprMarker * ha =  new THprMarker(a->GetX(), a->GetY());
   delete a;
   ha->Draw();
   ha->SetMarkerColor(fMarkerColor);
   ha->SetMarkerSize(fMarkerSize);
   ha->SetMarkerStyle(fMarkerStyle);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void MarkerLineDialog::PolyLine()
{
  cout << "Input a Polyline defining the controlpoints" << endl;
  TIter next(gPad->GetListOfPrimitives());
  TObject * obj;
  while ( (obj = next()) ) {
     if ( obj->InheritsFrom("TGraph") ) {
        TGraph * g = (TGraph*)obj;
        if(!(strncmp(g->GetName(), "Graph", 5))) {
           cout << "Rename existing Graph" << endl;
           g->SetName("Hprgraph");
        }
     }
  }
  TGraph * gr = (TGraph*)GrCanvas::WaitForCreate("TGraph", &fPad);
  if (!gr) {
      cout << "Interrupted TPolyLine" << endl;
      return;
   }
   THprGraph * ha =  new THprGraph(gr->GetN(), gr->GetX(), gr->GetY());
   delete gr;
   ha->Draw("L");
   ha->SetLineColor(fLineColor);
   ha->SetLineWidth(fLineWidth);
   ha->SetLineStyle(fLineStyle);
   gPad->Modified();
   gPad->Update();
};
//______________________________________________________________________________

void MarkerLineDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("MarkerLineDialog.fMarkerColor"		, fMarkerColor  );
   env.SetValue("MarkerLineDialog.fMarkerSize"		, fMarkerSize   );
   env.SetValue("MarkerLineDialog.fMarkerStyle"		, fMarkerStyle  );
   env.SetValue("MarkerLineDialog.fLineColor"		, fLineColor 	 );
   env.SetValue("MarkerLineDialog.fLineWidth"		, fLineWidth 	 );
   env.SetValue("MarkerLineDialog.fLineStyle"		, fLineStyle 	 );
   env.SetValue("MarkerLineDialog.fArrowColor"     , fArrowColor   );
   env.SetValue("MarkerLineDialog.fArrowSize"      , fArrowSize    );
   env.SetValue("MarkerLineDialog.fArrowStyle"     , fArrowStyle   );
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void MarkerLineDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fMarkerColor  = env.GetValue("MarkerLineDialog.fMarkerColor"    , 1);
   fMarkerSize  = env.GetValue("MarkerLineDialog.fMarkerSize"      , 0.02);
   fMarkerStyle  = env.GetValue("MarkerLineDialog.fMarkerStyle"    , 20);
   fLineColor    = env.GetValue("MarkerLineDialog.fLineColor"      , 1);
   fLineWidth     = env.GetValue("MarkerLineDialog.fLineWidth"     , 1);
   fLineStyle    = env.GetValue("MarkerLineDialog.fLineStyle"      , 1);
   fArrowColor   = env.GetValue("MarkerLineDialog.fArrowColor"     , 1);
   fArrowSize    = env.GetValue("MarkerLineDialog.fArrowSize"      , 0.01);
   fArrowStyle   = env.GetValue("MarkerLineDialog.fArrowStyle"     , 1);
}
//______________________________________________________________________

void MarkerLineDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << " MarkerLineDialog::CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
}

//_________________________________________________________________________

MarkerLineDialog::~MarkerLineDialog()
{
	if (fCanvas) {
		GrCanvas* hc = (GrCanvas*)fCanvas;
		hc->RemoveFromConnectedClasses(this);
	}
	if (fRow_lab) {
		fRow_lab->Delete();
		delete fRow_lab;
	}
}
//_______________________________________________________________________
/*
void MarkerLineDialog::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
      cout << "MarkerLineDialog: RecursiveRemove CloseDialog " << fCanvas  << endl;
      CloseDialog();
   }
}
*/
//_______________________________________________________________________

void MarkerLineDialog::CloseDialog()
{
   cout << "MarkerLineDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   delete this;
}
//_________________________________________________________________________

void MarkerLineDialog::CloseDown(Int_t wid)
{
   cout << "MarkerLineDialog::CloseDown() " <<wid << endl;
   if (wid != -2 )
      SaveDefaults();
   delete this;
}


#include "TEnv.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TRootCanvas.h"
#include "TPaveStats.h"
#include "HTCanvas.h"
#include "HandleMenus.h"

#include "FitHist.h"
#include "HistPresent.h"
#include "SetColor.h"

extern Int_t gHprDebug;

ClassImp(HTCanvas)

//____________________________________________________________________________
HTCanvas::HTCanvas():GrCanvas()
{
   fHistPresent = NULL;
   fFitHist     = NULL;
   fObject      = NULL;
   fHandleMenus = NULL;
};

HTCanvas::HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr, FitHist * fh,
           TObject * obj, Int_t flag)
           : GrCanvas(name, title, wtopx, wtopy, ww,wh),
			    fHistPresent(hpr), fFitHist(fh),fObject(obj)
{

//*-*-*-*-*-*-*-*-*-*-*-*Canvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ==================
//  Create a new canvas.
//
//  wtopx,wtopy are the pixel coordinates of the top left corner of the canvas
//     if wtopx < 0) the menubar is not shown.
//  ww is the canvas size in pixels along X
//  wh is the canvas size in pixels along Y
//
// HTCanvas has the following extra facilities:
//
// Markers:   Pressing the middle mouse sets markers used by class FitHist
// Menus:     The class HandleMenus adds popup menus the standard menubar
//            to invoke  functions in classes HistPresent and FitHist
//--------------------------------------------------------------------------

   fHandleMenus = NULL;
   TRootCanvas *rc = (TRootCanvas *)fCanvas->GetCanvasImp();
   rc->Connect("CloseWindow()", "HTCanvas", this, "HTCanvasClosed()");
   if (TestBit(kMenuBar)) {
      BuildHprMenus(fHistPresent, fFitHist, fObject);
      if (flag & GrCanvas::kIsAEditorPage) {
			cout << "HTCanvas::kIsAEditorPage" << endl;
         SetBit(GrCanvas::kIsAEditorPage);
      }
   }
	if ( gHprDebug > 0 )
		cout << "ctor HTCanvas: " << this << " " << name
        << " Id " << GetRootCanvas()->GetId() << endl;
//	TTimer::SingleShot(20, "HTCanvas", this, "ConnectToModified()");
	ConnectToModified();
//   fTimer = new TTimer();
	fTimer.Connect("Timeout()", "HTCanvas", this, "DoSaveLegendStats()");
}
//______________________________________________________________________________________

void HTCanvas::HTCanvasClosed()
{
	if ( gHprDebug > 0 )
		cout << endl << "HTCanvas::HTCanvasClosed() " 
		<< this << " " << GetName() << endl << flush;
	if (this->TestBit(TObject::kNotDeleted)){
		if ( gHprDebug > 0 )
			cout << endl << "HTCanvas::HTCanvasClosed()  delete " 
			<< this << " " << GetName() << endl << flush;
		TRootCanvas *rc = (TRootCanvas *)fCanvas->GetCanvasImp();
		rc->Disconnect("CloseWindow()", this, "HTCanvasClosed()");
		rc->DontCallClose();
		delete this;
	}
}
//______________________________________________________________________________________

void HTCanvas::ConnectToModified() 
{
	TQObject::Connect((TPad*)this, "Modified()",
		  "HTCanvas", this, "HandlePadModified()");
};
//______________________________________________________________________________________

HTCanvas::~HTCanvas()
{
   TRootCanvas *rc = (TRootCanvas *)fCanvas->GetCanvasImp();
	if ( gHprDebug > 0 )
		cout << "dtor HTCanvas: " << this << " " << GetName()
		<< " FitHist: " << GetFitHist()
		<< " CanvasImp " <<rc << endl;
	if ( gHprClosing ) {
		if ( gHprDebug > 0 )
			cout << " hpr closing" << endl;
		return;
	}
	
//   TQObject::Disconnect((TPad*)this, "Modified()");
	if ( strcmp (GetName(), "FileList") == 0) {
		if (fHistPresent) fHistPresent->fFileList = NULL;
	}
   rc->Disconnect("CloseWindow()", this, "HTCanvasClosed()");
	rc->DontCallClose();
	fTimer.Stop();
   TQObject::Disconnect((TPad*)this, "Modified()", this, "HandlePadModified()");
	if (fHandleMenus) {
      delete fHandleMenus;
      fHandleMenus = 0;
   }
   if (fHistPresent) fHistPresent->HandleDeleteCanvas(this);
	if ( gHprDebug > 1 ) {
		cout << "dtor HTCanvas: after sleep " << endl;
		gSystem->Sleep(5000);
	}
/*	TList * lop = GetListOfPrimitives();
	TIter next(lop);
	TObject *obj;
	TList temp;
	while ( (obj = next()) ){
		if (obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph") )
			temp.Add(obj);
	}
	TIter next1(&temp);
	while ( (obj = next()) ){
		lop->Remove(obj);
	}
   if(fFitHist) {
      fFitHist->UpdateCut();
      fFitHist->SetCanvasIsDeleted();
		if ( gHprDebug > 0 )
			cout << "HTCanvas: " << this << " delete fFitHist " << fFitHist << endl<< flush;
      delete fFitHist;
   }*/
	Emit("HTCanvasClosed()");
};
//____________________________________________________________________________

void HTCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//

	if (gHprDebug > 3) {
		cout << "Event: " << event << endl;
	}
	if ( this->UseGL() ) {
		TCanvas::HandleInput(event, px, py);
		return;
	}
	// mouse wheel
	if ( event == kWheelUp || event ==  kWheelDown) {
		TCanvas::HandleInput(event, px, py);
		return;
	}
   TPad         *pad;
   TObject      *prevSelObj = 0;
   if ( event == kButton2Down ) {
       // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;
      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if(fFitHist) fFitHist->AddMark((TPad*)gPad,px,py);
	}
	if ( event == kButton1Down ) {
		// find pad in which input occured
		pad = Pick(px, py, prevSelObj);
		if (!pad) return;
		gPad = pad;   // don't use cd() because we won't draw in pad
		// we will only use its coordinate system
		if (gHprDebug > 0 && (fSelected->InheritsFrom("TH1") || fSelected->InheritsFrom("TGraph")) ) {
			cout << "Select: " << fSelected->ClassName()<< " " << fSelected->GetName() << endl;
		}
	}
	GrCanvas::HandleInput(event, px, py);
}
//______________________________________________________________________________

void HTCanvas::HandlePadModified()
{
	if ( TCanvas::fUpdating ) {
		if ( gHprDebug > 2 ) 
			cout << "TCanvas::fUpdating" << endl;
		return;
	}
	if ( gHprDebug > 2 ) 
		cout << "HTCanvas::HandlePadModified() " << this << endl;
	fTimer.Start(20, kTRUE);
//	TTimer::SingleShot(10, "HTCanvas", this, "DoSaveLegendStats()");
//	DoSaveLegendStats();
}	
//______________________________________________________________________________

void HTCanvas::DoSaveLegendStats()
{
	TEnv env(".hprrc");
	if ( TCanvas::fUpdating ) {
		cout << "TCanvas::fUpdating" << endl;
		return;
	}
	TList * lop = this->GetListOfPrimitives();
	if ( lop == NULL ) {
		cout << "HTCanvas:: lop == NULL " << endl;
		return;
	}
	TIter next(lop);
	TObject *obj;
	
	TPave * leg;
	TString envn;
	TString res;
	TString sdim;
	// look for statistics box	
	if ( env.GetValue("GeneralAttDialog.fRememberStatBox", 0) ) {
		while ( obj = next() ) {
			if ( obj->InheritsFrom("TH1") ) {
				leg = (TPave*)((TH1*)obj)->GetListOfFunctions()->FindObject("stats");
				if (leg ) {
					envn = "SetHistOptDialog.Stat";	
					if ( ((TH1*)obj)->GetDimension() == 1 ) {
						sdim =  "";
					} else if ( ((TH1*)obj)->GetDimension() == 2 ) {
						sdim = "2D";
					}
					res = envn + "X" + sdim;
					env.SetValue(res, leg->GetX2NDC());
					res = envn + "Y" + sdim;
					if (gHprDebug > 0) 
						cout <<"DoSaveLegendStats() " <<  res << " "  << leg->GetY1NDC() << endl;
					env.SetValue(res, leg->GetY2NDC());
				}
			}
		}
		env.SaveLevel(kEnvLocal);
	}
// look for legend box	
	if (  env.GetValue("GeneralAttDialog.fRememberLegendBox", 1) ) {
		leg = (TPave*)lop->FindObject("Legend_SuperImposeHist");
		if ( leg == NULL ) {
			envn = "Legend_SuperImposeGraph";
			leg = (TPave*)lop->FindObject(envn);
			if ( leg == NULL ) {
				leg = (TPave*)lop->FindObject("Legend_HprStack");
				if ( leg == NULL ) {
					if (gHprDebug > 0)
						cout <<"DoSaveLegendStats(): No legend found"<< endl;
					return;
				} else {
					envn = "HprStack.fLegend";
				}
			} else {
				envn = "SuperImposeGraph.fLegend";
			}
		} else {
			envn = "SuperImposeHist.fLegend";
		}
		res = envn + "X1";
		env.SetValue(res, leg->GetX1NDC());
		if (gHprDebug > 0) 
			cout << res << " " << leg->GetX1NDC() << endl;
		res = envn + "X2";
		env.SetValue(res, leg->GetX2NDC());
		if (gHprDebug > 0) 
			cout << res << " "  << leg->GetX2NDC() << endl;
		res = envn + "Y1";
		env.SetValue(res, leg->GetY1NDC());
		if (gHprDebug > 0) 
			cout <<"DoSaveLegendStats() " << res << " "  << leg->GetY1NDC() << endl;
		res = envn + "Y2";
		env.SetValue(res, leg->GetY2NDC());
		env.SaveLevel(kEnvLocal);
	}
}
//______________________________________________________________________________

void HTCanvas::BuildHprMenus(HistPresent *hpr, FitHist *fh, TObject *obj)
{
   fHistPresent = hpr;
   fFitHist     = fh;
   fObject      = obj;

   fHandleMenus = new HandleMenus(this, fHistPresent, fFitHist, fObject);
	if ( gHprDebug  > 1)
		cout << "fHandleMenus->GetId() " << fHandleMenus->GetId() << endl;
   fHandleMenus->BuildMenus();
}

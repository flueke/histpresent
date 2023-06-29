#include "TVirtualX.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TWbox.h"
#include "TGuiFactory.h"
#include "TStyle.h"
#include "TH1.h"
#include "TAxis.h"
#include "TDatime.h"
#include "TLatex.h"
#include "TMath.h"
#include "GrCanvas.h"
#include "TContextMenu.h"
#include "GEdit.h"
#include "SetColor.h"
#include "HprElement.h"
#include "HprEditBits.h"

const Size_t kDefaultCanvasSize   = 20;

//ClassImp(GrCanvas)

//____________________________________________________________________________
GrCanvas::GrCanvas():TCanvas()
{
   fRootCanvas  = NULL;
   fGEdit       = NULL;
   fConnections = 0;
   fHiddenPrimitives  = NULL;
   fEditGridX = 0;
   fEditGridY = 0;
   fUseEditGrid = 0;
   fEditorIsShown = kFALSE;
   fCurrentPlane      = 50;
//   fOrigWw = 0;
//  fOrigWh = 0;
   fButtonsEnabled = kTRUE;
   fOrigWw = GetWw();
   fOrigWh = GetWh();
};

GrCanvas::GrCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh) :  TCanvas(kFALSE)
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
// GrCanvas has the following extra facilities:
//
// Edit grid: if activated forces coordinates of mouse clicks on the grid
//--------------------------------------------------------------------------
	if (gDebug > 1) 
		cout << "enter ctor GrCanvas bits: " 
		  << hex << TestBits(0xffffffff)<< dec << endl;
   TObject *old = gROOT->GetListOfCanvases()->FindObject(name);
	if (old )
		cout << "GrCanvas ctor old " << old->GetName() << " " << old->ClassName() << endl;
   if (old && old->InheritsFrom("TCanvas") && old->IsOnHeap()) {
      Warning("Constructor","Deleting canvas with same name: %s",name);
      delete old;
   }
   SetBit(kMenuBar		  ,1);
   SetBit(kShowToolBar    ,0);
   SetBit(kShowEditor	  ,0);
   SetLogx(0);
   SetLogy(0);
   SetLogz(0);
	TCanvas::Constructor(name, title, wtopx, wtopy, ww, wh);
	fRootCanvas = (TRootCanvas*)fCanvasImp;
	fEditGridX = 0;
   fEditGridY = 0;
   fGEdit       = NULL;
   fHiddenPrimitives = new TList();
   fUseEditGrid = 0;
   fEditorIsShown = kFALSE;
   fConnections = 0;
   fCurrentPlane      = 50;
   fButtonsEnabled = kTRUE;
   if (TestBit(kIsAEditorPage)) {
//      InitEditCommands();
      fRootCanvas->DontCallClose();
      TQObject::Connect((TGMainFrame*)fRootCanvas, "CloseWindow()",
                        this->ClassName(), this, "MyClose()");
   }
   fOrigWw = GetWw();
   fOrigWh = GetWh();
	if (gDebug > 1) 
		cout << "ctor GrCanvas: " << this << " " << name
       << " fRootCanvas->GetHeight() " << fRootCanvas->GetHeight()
        << " GetWh " << GetWh()
		  << endl;
};
//______________________________________________________________________________________

void GrCanvas::MyClose()
{
   cout << "GrCanvas::MyClose() set ShowEditor(kFALSE) " << endl;
   fRootCanvas->ShowEditor(kFALSE);
   fRootCanvas->CloseWindow();
}
//______________________________________________________________________________________

GrCanvas::~GrCanvas()
{
	if (gDebug > 0)
		cout << "dtor GrCanvas: " << this << " " << GetName()<< endl;
};
//____________________________________________________________________________

void GrCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//
//  if (event > 0) return;
//OS start
   static Int_t pxB1down, pyB1down;
	static Bool_t logxState = 0, logyState = 0;
   static Bool_t in_edit = kFALSE;
   static Bool_t obj_moved = kFALSE;

   Double_t x = 0, y = 0;
   Int_t n;
   static TObject * pad_of_image = 0;
   static Bool_t in_image = kFALSE;
//OS end

   TPad         *pad;
   TObject      *prevSelObj = 0;
   TPad         *prevSelPad = 0;

   if ( !fButtonsEnabled ) {
		if (gDebug > 1 ) {
			cout << "fButtonsEnabled	false, return" << endl;
		}
		return;
	}
	if (gDebug == 6) {
		if (event == kButton1Down) 
			cout << px << " " << py << endl;
		return;
	}
//	if (gDebug > 3 && event != kMouseMotion && event != kButton1Motion)
	if (gDebug > 3)
		cout << "GrCanvas::HandleInput Event: " << event << endl;
   if (gROOT->GetEditorMode() != 0) {
      in_edit = kTRUE;
		if (gDebug > 1 && event != kMouseMotion && event != kButton1Motion)
		cout << "GrCanvas::HandleInput in_edit  Event: " << event << endl;
		
   }
   if (fSelected && fSelected->TestBit(kNotDeleted))
      prevSelObj = fSelected;
   if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
      prevSelPad = (TPad*) fSelectedPad;

   fPadSave = (TPad*)gPad;
   if (event == kButton1Down) {
		if (gDebug > 1 )
			cout << "kButton1Down: fSelected "<< fSelected << " "  << fSelected->ClassName() 
		     << " gPad "  << gPad << " "  << gPad->ClassName()
           << " fSelectedPad "  << fSelectedPad << " " << fSelectedPad->ClassName()  
		     <<  endl << flush;
      if (gROOT->GetEditorMode() != 0 && fSelectedPad != gPad) {
         if (gDebug > 1 ) cout << "Set gPad to selected pad" << endl;
			gPad = fSelectedPad;  
			FeedbackMode(kTRUE);

			fSelected->Pop();           // pop object to foreground
			fSelectedPad->cd();                  // and make its pad the current pad
			if (fSelected && fSelected->TestBit(kNotDeleted))
				prevSelObj = fSelected;
			if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
				prevSelPad = (TPad*) fSelectedPad;
			fPadSave = (TPad*)gPad;
			TIter next(gROOT->GetListOfCanvases());
			TCanvas *tc;
			while ((tc = (TCanvas *)next()))
				tc->Update();
      }
   }
   cd();        // make sure this canvas is the current canvas

//   if (event == kButton1Down)  cout << "gPad " << gPad << endl;

   fEvent  = event;
   fEventX = px;
   fEventY = py;
   obj_moved = kFALSE;

   if (fSelected) {
      if (event == kButton1Down && !strncmp(fSelected->ClassName(), "TASI", 4)) {
//         cout << "GrCanvas: " << fSelected->GetName()<< " gPad " <<
//              gPad << endl;
//         gPad->Dump();
         in_image = kTRUE;
      }
   }

//   if (gROOT->GetEditorMode() && fEvent == kMouseMotion) fEvent = kButton1Motion;

   switch (fEvent) {

   case kMouseMotion:
      // highlight object tracked over
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      fSelected->ExecuteEvent(event, px, py);

      if (GetAutoExec()) RunAutoExec();

      break;

   case kMouseEnter:
      // mouse enters canvas
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
      break;
   case kMouseLeave:
      // mouse leaves canvas
      {
//         cout << " kMouseLeave " << endl;
         // force popdown of tooltips
         TObject *sobj = fSelected;
         TPad    *spad = fSelectedPad;
         fSelected    = 0;
         fSelectedPad = 0;
         EnterLeave(prevSelPad, prevSelObj);
         fSelected    = sobj;
         fSelectedPad = spad;
         if (!fDoubleBuffer) FeedbackMode(kFALSE);
      }
      break;

   case kButton1Double:
      // triggered on the second button down within 350ms and within
      // 3x3 pixels of the first button down, button up finishes action
//      break;

   case kButton1Down:

//      cout << "kButton1Down "  << px << " " << py << " " << fSelected << endl;
     // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
		if (gDebug > 1 )
			cout << "kButton1Down: pad " << pad << " prevSelObj " << prevSelObj<< endl;
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if (in_image) {
         pad_of_image = pad;
         fSelected = pad_of_image;
      }

//OS start
      pxB1down = px;
      pyB1down = py;

      if (!fSelected->InheritsFrom("TCanvas") && !fSelected->InheritsFrom("TAxis") &&
			fSelected->TestBit(kIsBound)) break;
      x = gPad->AbsPixeltoX(px);
      y = gPad->AbsPixeltoY(py);
      if(fUseEditGrid && fSelectedPad == this &&
       !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() 
		 || fSelected->InheritsFrom("TPave"))
        ){
//         cout << "x y  " << gPad->AbsPixeltoX(px) << " " << gPad->AbsPixeltoY(py) << endl;
         if(fEditGridX !=0){
            n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
            x = n * fEditGridX;
         }
         if(fEditGridY !=0){
            n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
            y = n * fEditGridY;
         }
            if(fEditGridX !=0){
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw()) px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
      }

//OS end
      if (fSelected) {
         FeedbackMode(kTRUE);   // to draw in rubberband mode
         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0) {
            fSelected->ExecuteEvent(event, px, py);
         }
         if (GetAutoExec()) RunAutoExec();
      }
		if (gDebug > 1 )
			cout << "Exit kButton1Down: pad " << pad << " prevSelObj " << prevSelObj<< endl;

      break;

   case kButton1Motion:
   case kButton1ShiftMotion: //8 == kButton1Motion + shift modifier
      if (fSelected) {
         gPad = fSelectedPad;

         fSelected->ExecuteEvent(event, px, py);
         gVirtualX->Update();

         if (!fSelected->InheritsFrom(TAxis::Class())) {
            Bool_t resize = kFALSE;
            if (fSelected->InheritsFrom(TBox::Class()))
               resize = ((TBox*)fSelected)->IsBeingResized();
            if (fSelected->InheritsFrom(TVirtualPad::Class()))
               resize = ((TVirtualPad*)fSelected)->IsBeingResized();

            if ((!resize && TestBit(kMoveOpaque)) || (resize && TestBit(kResizeOpaque))) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         }
//OS start
			if (!fSelected->InheritsFrom("TCanvas") && !fSelected->InheritsFrom("TAxis") &&
				fSelected->TestBit(kIsBound)) {
				if (gDebug > 1 )
				cout << "fSelected->TestBit(kIsBound), break "<< fSelected->GetName() << endl;
				break;
			}
         if (in_image) {
            fSelected = pad_of_image;
//            fSelected = gPad;
         }
//         if(fUseEditGrid && !(fSelected->IsA() == TPad::Class())){
         if(fUseEditGrid && fSelectedPad == this &&
          !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class()  
			 || fSelected->InheritsFrom("TPave"))
           ){
            if(fEditGridX !=0){
               x = gPad->AbsPixeltoX(px);
               n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
               x = n * fEditGridX;
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw())px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               y = gPad->AbsPixeltoY(py);
               n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
               y = n * fEditGridY;
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
         }

//OS end

         RunAutoExec();
      }
/*
//      break;
//   case kMouseMotion:

      if (fSelected) {
         gPad = fSelectedPad;
//OS start
			if (fSelected->TestBit(kIsBound)) {
				if (gDebug > 1 )
				cout << "fSelected->TestBit(kIsBound), break " << endl;
				break;
			}
         if (in_image) {
            fSelected = pad_of_image;
//            fSelected = gPad;
         }
//         if(fUseEditGrid && !(fSelected->IsA() == TPad::Class())){
         if(fUseEditGrid && fSelectedPad == this &&
          !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() )
           ){
            if(fEditGridX !=0){
               x = gPad->AbsPixeltoX(px);
               n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
               x = n * fEditGridX;
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw())px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               y = gPad->AbsPixeltoY(py);
               n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
               y = n * fEditGridY;
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
         }

//OS end
         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0)
            fSelected->ExecuteEvent(event, px, py);
            gVirtualX->Update();

         {
            Bool_t resize = kFALSE;
            if (fSelected->InheritsFrom(TBox::Class()))
               resize = ((TBox*)fSelected)->IsBeingResized();
            if (fSelected->InheritsFrom(TVirtualPad::Class()))
               resize = ((TVirtualPad*)fSelected)->IsBeingResized();

            if ((!resize && OpaqueMoving()) || (resize && OpaqueResizing())) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         }
          if (GetAutoExec()) RunAutoExec();
//         if (GetShowEventStatus()) DrawEventStatus(event, px, py, fSelected);
      }
*/
      break;

   case kButton1Up:
      if (fSelected) {
         gPad = fSelectedPad;
         if (gDebug > 1 )
				cout << "kButton1Up: this " << this
              << " fSelected " << fSelected->ClassName() << " gPad " << gPad << endl;
         if (!fSelected->InheritsFrom("TCanvas") && !fSelected->InheritsFrom("TAxis") &&
				fSelected->TestBit(kIsBound)) break;
         if (in_image) {
//            cout << "setting: " << fSelected << endl;
            fSelected = pad_of_image;
            in_image = kFALSE;
         }
//OS start
//         cout << "name, px, py, bef " << fSelected->ClassName() << " " << pxB1down << " " << pyB1down
//                 << " "  << px << " " << py << endl;

         if (px != pxB1down || py != pyB1down) {
				obj_moved = kTRUE;
				if (gDebug > 1 )
					cout << "kButton1Up:  obj_moved = kTRUE" << endl;
			}
         if (fUseEditGrid && fSelectedPad == this &&
          !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() 
				 || fSelected->InheritsFrom("TPave")) )
           {
            if(fEditGridX !=0){
               x = gPad->AbsPixeltoX(px);
               n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
               x = n * fEditGridX;
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw())px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               y = gPad->AbsPixeltoY(py);
               n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
               y = n * fEditGridY;
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
         }
//OS end

         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0)
            fSelected->ExecuteEvent(event, px, py);

          if (GetAutoExec()) RunAutoExec();
         if (fPadSave->TestBit(kNotDeleted))
            gPad = fPadSave;
         else {
            gPad     = this;
            fPadSave = this;
         }

         Update();    // before calling update make sure gPad is reset
      }

      break;

//*-*----------------------------------------------------------------------

   case kButton2Down:
      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      FeedbackMode(kTRUE);

      fSelected->Pop();           // pop object to foreground
      pad->cd();                  // and make its pad the current pad
      if (gDebug) {
         printf("Current Pad: %s / %s \n", pad->GetName(), pad->GetTitle());
         cout << "fSelected: " << fSelected << " pad " << pad << endl;
      }
      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }

      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }

      return;   // don't want fPadSave->cd() to be executed at the end

   case kButton2Motion:
      break;

   case kButton2Up:
      break;

   case kButton2Double:
      break;

//*-*----------------------------------------------------------------------

   case kButton3Down:
   {
 //     cout << "kButton3Down "  << px << " " << py << " " << fSelected << endl;
      // popup context menu
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;
      if (!fDoubleBuffer) FeedbackMode(kFALSE);
		logxState = pad->GetLogx();
		logyState = pad->GetLogy();
		if (gDebug > 1) {
			cout << "kButton3Down fSelected " << fSelected;
			if (logxState)
				cout <<  " logx ";
			if (logyState)
				cout << " logy ";
			cout << endl;
		}
      if (fContextMenu && !fSelected->TestBit(kNoContextMenu) &&
          !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu)) {
 //         fSelected->ExecuteEvent(event, px, py);
          fContextMenu->Popup(px, py, fSelected, this, pad);
      }
		if (gDebug > 1) {
			cout << "kButton3Down gPad " << gPad;
			if (gPad->GetLogx())
				cout << " logx ";
			if (gPad->GetLogy() )
				cout << " logy ";
			cout << endl;
		}

      break;
   }
   case kButton3Motion:
      break;

   case kButton3Up:
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
		if (gDebug > 1) {
			cout << "kButton3Up gPad " << gPad;
			if (gPad->GetLogx())
				cout << " logx ";
			if (gPad->GetLogy() )
				cout << " logy ";
			cout << endl;
		}
      if ( gPad && (gPad->GetLogx() != logxState ||
			 gPad->GetLogy() != logyState ) )
			obj_moved = kTRUE;
		
      break;

   case kButton3Double:
      break;
   case kKeyPress:
      if (!fSelectedPad || !fSelected) return;
      gPad = fSelectedPad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      fSelected->ExecuteEvent(event, px, py);

      RunAutoExec();

      break;
#if 0
   case kButton1Shift:
      // Try to select
      pad = Pick(px, py, prevSelObj);

      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas
      if (gDebug) {
         printf("Current Pad: %s / %s \n", pad->GetName(), pad->GetTitle());
         cout << "1Shift fSelected: " << fSelected->GetName() << " pad " << pad << endl;
      }
      fSelected->ExecuteEvent(event, px, py);
      RunAutoExec();

      break;
   case kWheelUp:
   case kWheelDown:
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;
      fSelected->ExecuteEvent(event, px, py);
		break;
#endif

#if 0
   case kKeyPress:

      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      fSelected->ExecuteEvent(event, px, py);

//      if (GetShowEventStatus()) DrawEventStatus(event, px, py, fSelected);
      if (GetAutoExec())        RunAutoExec();

      break;
#endif

   default:
      break;
   }

//   if (fPadSave) fPadSave->cd();
   if (fPadSave && event != kButton2Down)
      fPadSave->cd();

   if (event != kMouseLeave) { // signal was already emitted for this event
      ProcessedEvent(event, px, py, fSelected);  // emit signal
      DrawEventStatus(event, px, py, fSelected);
   }
   if (in_edit && gROOT->GetEditorMode() == 0) {
      ObjectCreated(px, py, fSelected);
      in_edit = kFALSE;
   }
   if (gDebug > 1 && obj_moved) {
		cout << "fSelected, this " << fSelected << " " << this;
		if (obj_moved)
			cout << " obj_moved ";
		cout << endl;
	}
   if (obj_moved && fSelected && fSelected != this) {
      ObjectMoved(px, py, fSelected);
   }
}
//______________________________________________________________________________
void GrCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//
//    This function is called when the option "Event Status"
//    in the canvas menu "Options" is selected.
//

   const Int_t kTMAX=256;
   static char atext[kTMAX];

   if (!GetShowEventStatus() || !selected) return;

   if (!fCanvasImp) return; //this may happen when closing a TAttCanvas

   TVirtualPad* savepad;
   savepad = gPad;
   gPad = GetSelectedPad();

   fCanvasImp->SetStatusText(selected->GetTitle(),0);
   fCanvasImp->SetStatusText(selected->GetName(),1);
   if (event == kKeyPress)
      sprintf(atext, "%c", (char) px);
   else
      sprintf(atext, "%d,%d", px, py);
   fCanvasImp->SetStatusText(atext,2);
//   printf("Sel %s\n", selected->GetName());
	Bool_t use_td = kFALSE;
	
   if ( (selected->InheritsFrom("TH1") &&
		((TH1*)selected)->GetXaxis()->GetTimeDisplay()) ) use_td = kTRUE;
   if ( (selected->InheritsFrom("TGraph") &&
		((TGraph*)selected)->GetXaxis()->GetTimeDisplay()) ) use_td = kTRUE;
	if (selected->InheritsFrom("TAxis") && ((TAxis*)selected)->GetTimeDisplay())
		use_td = kTRUE;
	if ( use_td ) {
		UInt_t tt=(UInt_t)gPad->AbsPixeltoX(px);
		TDatime dt(tt);
		Double_t yy =(Double_t)gPad->AbsPixeltoY(py);
		TString stt(dt.AsSQLString());
		stt += " Val: ";
		stt += Form("%5.1f", yy);
//		dt.Print();
		fCanvasImp->SetStatusText(stt,3);
	} else {
		fCanvasImp->SetStatusText(selected->GetObjectInfo(px,py),3);
	}
   gPad = savepad;
}
//______________________________________________________________________________
void GrCanvas::RunAutoExec()
{
   // Execute the list of TExecs in the current pad.

   if (!gPad) return;
   ((TPad*)gPad)->AutoExec();
}
//______________________________________________________________________________

void GrCanvas::Add2ConnectedClasses(TObject *obj)
{
	if ( gDebug > 1 )
		cout << "Add2ConnectedClasses " <<obj->ClassName() << " gPad " << gPad << endl;
   this->Connect("ObjectCreated(Int_t, Int_t, TObject*)", obj->ClassName(), obj,
                 "ObjCreated(Int_t, Int_t, TObject*)");
   this->Connect("ObjectMoved(Int_t, Int_t, TObject*)", obj->ClassName(), obj,
                 "ObjMoved(Int_t, Int_t, TObject*)");
   fConnections++;
}
//______________________________________________________________________________

void GrCanvas::RemoveFromConnectedClasses(TObject *obj)
{
	if ( gDebug > 1 )
		cout << "RemoveFromConnectedClasses " <<obj->ClassName() << " gPad " << gPad << endl;
	this->Disconnect("ObjectCreated(Int_t, Int_t, TObject*)", obj,
					  "ObjCreated(Int_t, Int_t, TObject*)");
	this->Disconnect("ObjectMoved(Int_t, Int_t, TObject*)", obj,
					  "ObjMoved(Int_t, Int_t, TObject*)");
	if ( this->NumberOfConnections() == 0 )
		fConnections--;
}
//______________________________________________________________________________

void GrCanvas::ObjectCreated(Int_t px, Int_t py, TObject *obj)
{
   Long_t args[3];
   if (fConnections) {
      args[0] = (Long_t)px;
      args[1] = (Long_t)py;
      args[2] = (Long_t)obj;
      if (gDebug > 1 )
			cout << "GrCanvas::Emit(ObjectCreated: " << obj << endl;
      Emit("ObjectCreated(Int_t, Int_t, TObject*)", args );
   }
}
//______________________________________________________________________________

void GrCanvas::ObjectMoved(Int_t px, Int_t py, TObject *obj)
{
   Long_t args[3];
   if (fConnections) {
      args[0] = (Long_t)px;
      args[1] = (Long_t)py;
      args[2] = (Long_t)obj;
      if (gDebug > 1 )
			cout << "GrCanvas::Emit(ObjectMoved: " << obj << endl;
      Emit("ObjectMoved(Int_t, Int_t, TObject*)", args );
   }
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridX(Double_t x)
{
   if (fEditGridX ==  0) return x;
   Int_t n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
   return (Double_t)n * fEditGridX;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridY(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Int_t n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
   return (Double_t)n * fEditGridY;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridX_NDC(Double_t x)
{
   if (fEditGridX <=  0) return x;
   Double_t px1 = gPad->GetX1();
   Double_t px2 = gPad->GetX2();
	Double_t gNDC = fEditGridX /(px2 - px1);

   Int_t n = (Int_t)((x + TMath::Sign(0.5*gNDC, x)) / gNDC);
   return (Double_t)n * gNDC;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridY_NDC(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Double_t py1 = gPad->GetY1();
   Double_t py2 = gPad->GetY2();
	Double_t gNDC = fEditGridY /(py2 - py1);

   Int_t n = (Int_t)((y + TMath::Sign(0.5*gNDC, y)) / gNDC);
   return (Double_t)n * gNDC;
}
//______________________________________________________________________________

void GrCanvas::HideObject(TObject *obj)
{
   if (!obj || obj == this || GetListOfPrimitives()->GetEntries()  <= 0 ) return;
   if (gDebug > 1 )
		std::cout << "HideObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, GetListOfPrimitives(), fHiddenPrimitives);
}
//______________________________________________________________________________

void GrCanvas::ViewObject(TObject *obj)
{
   if (gDebug > 1 )
		std::cout << "ViewObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, fHiddenPrimitives, GetListOfPrimitives());
}

//______________________________________________________________________________

void GrCanvas::ViewAllObjects()
{
   if (!fHiddenPrimitives || fHiddenPrimitives->GetEntries() <=0 ) return;
   HprElement::MoveAllObjects(fHiddenPrimitives, GetListOfPrimitives(), 0, 1);
}
//_________________________________________________________________________

TObject * GrCanvas::WaitForCreate(const char * what, TPad **pad)
{
	*pad = NULL;
	TString ws(what);
	if (ws == "TGraph") {
		gROOT->SetEditorMode("PolyLine");
	} else {
		if (gDebug > 1)
			cout << "GrCanvas::WaitForCreate: " << &what[1]<< endl;
		gROOT->SetEditorMode(&what[1]);
	}
	for (Int_t i = 0; i < 30000; i++) {
		gSystem->ProcessEvents();
		if (*pad != NULL) {
			gROOT->SetEditorMode();
			if (gDebug > 1 )
				cout << "exit GrCanvas::WaitForCreate: " << *pad << endl;
			break;
		}
		gSystem->Sleep(10);
	}
	if (*pad == NULL) {
		gROOT->SetEditorMode();
		cout << "Timeout while waiting for create " << what << endl;
		return NULL;
	}
	TObject * obj = gPad->GetListOfPrimitives()->Last();			
	if ( !obj->InheritsFrom(what) ) {
		cout << "No " << what << " found in " << gPad << endl;
		return NULL;
	}
	return obj;
}
//____________________________________________________________________________

void GrCanvas::ListPrimitives()  
{
	cout << endl << "HTCanvas* hc = (HTCanvas*)" << this << ";" << endl;
	TString cn;
	TObjLink * lnk = gPad->GetListOfPrimitives()->FirstLink();
	Int_t ind = 0;
	while ( lnk ) {
		TObject * obj = lnk->GetObject();
		cn = obj->ClassName();
		if ( !cn.Contains("Marker") ) {
			cout << "" << obj->ClassName()<< " * l" << ind << " = " 
			<< "(" << obj->ClassName()<< "*)" << obj << "; " << obj->GetName()
				<< " Option: \"" <<lnk->GetOption()<< "\"" << endl; 
			ind++;
		}
		lnk = lnk->Next();
	}
}; 


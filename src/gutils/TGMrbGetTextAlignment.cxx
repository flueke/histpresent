namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include "TRootHelpDialog.h"
#include <TList.h>
#include <TObjString.h>
#include <TRegexp.h>

#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGButton.h>
#include "TGMrbHelpWindow.h"
#include "TGMrbGetTextAlignment.h"
#include "SetColor.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget to select text alignment                               // 
///////////////////////////////////////////////////////////////////////////

//________________________________________________________________________________________


ClassImp(TGMrbGetTextAlignment)

TGMrbGetTextAlignment::TGMrbGetTextAlignment(Int_t * align, const TGWindow *win,
                         const char *helptext):
							    TGTransientFrame(gClient->GetRoot(), win, 10, 10)
{
   // Create  input dialog.

   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);

   fWidgets = new TList;
   fButtons = new TList;
   fHelpText = helptext;
   const TGWindow * main = gClient->GetRoot();
   if(win != 0)fMyWindow = win;
   else        fMyWindow = main;
   fAlign = align;

   TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX , 2, 2, 2, 2);
   fWidgets->AddFirst(lo1);

//  table part

   TGCompositeFrame * hframe;
   TGRadioButton * rbutton;
   for (Int_t i = 3; i >= 1; i -= 1) {
      hframe = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);  
      fWidgets->Add(hframe);
      for (Int_t j = 10; j <= 30; j+=10) {
         Int_t al = i + j;
         rbutton = new TGRadioButton(hframe, new TGHotString(""), i + j);
         fButtons->Add(rbutton);
         if (*fAlign == al) rbutton->SetState(kButtonDown);
         else               rbutton->SetState(kButtonUp);
         rbutton->Resize(rbutton->GetDefaultWidth(), rbutton->GetDefaultHeight());
         rbutton->Associate(this);
         fWidgets->Add(rbutton);
         hframe->AddFrame(rbutton, lo1);
      }
      this->AddFrame(hframe, lo1);
   }
   // create frame and layout hints for Ok and Cancel buttons

   TGHorizontalFrame *hf = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
   fWidgets->AddFirst(hf);
   // put hf as last in list to be deleted

   // create OK and Cancel buttons in their own frame (hf)

   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", 1);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, lo1);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   if(helptext){
      b = new TGTextButton(hf, "Help", 3);
      fWidgets->AddFirst(b);
      b->Associate(this);
      hf->AddFrame(b, lo1);
      height = b->GetDefaultHeight();
      width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   }
   // place button frame (hf) at the bottom
   fWidgets->AddFirst(lo1);

   this->AddFrame(hf, lo1);

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * nb, height);

   // set dialog title
   this->SetWindowName("Align");

   // map all widgets and calculate size of dialog
   this->MapSubwindows();

   width  = this->GetDefaultWidth();
   height = this->GetDefaultHeight();

   this->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
         ((TGFrame *) fMyWindow)->GetWidth() - width >> 1,
         ((TGFrame *) fMyWindow)->GetHeight() - height >> 1,
                          ax, ay, wdum);
   UInt_t ww, wh;
   Int_t  wwi,whi;
   Int_t screen_x, screen_y;
//  screen size in pixels
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
              screen_x, screen_y, ww, wh);
   wwi = ww;
   whi = wh;
   Int_t wi = width;
   Int_t hi = height;
//  make sure its inside physical screen
   if      (ax < 0)    ax = 10;
   else if (ax + wi > wwi)  ax = wwi - wi - 5;
   if      (ay + hi/2 < 0) ay =  hi/2 + 5;
   else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);

   // make the message box non-resizable
   this->SetWMSize(width, height);
   this->SetWMSizeHints(width, height, width, height, 0, 0);

   this->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);

   // popup dialog and wait till user replies
   this->MapWindow();
   this->ChangeBackground(brown);
   hf->ChangeBackground(brown);

   gClient->WaitFor(this);
};

Bool_t TGMrbGetTextAlignment::ProcessMessage(Long_t msg, Long_t parm1, Long_t /*parm2*/)
{
   // Handle button and text enter events

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
 //        cout << " ProcessMessage: " << GET_SUBMSG(msg) << " " << parm1 << " " <<parm2 << endl;
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON: 
                switch (parm1) {
                   case 1:
                      TGRadioButton *rb; 
                      for (Int_t i=0; i < 9; i++){
                         rb = (TGRadioButton *)fButtons->At(i);
                         if (rb->GetState() == kButtonDown) *fAlign = rb->WidgetId();
                      }
                      CloseWindow();
//                      delete this;
                      break;
                   case 3:
                      new TGMrbHelpWindow(this, "Alignment Help", fHelpText, 550, 500);
                      break;
                  }
				    break;
             case kCM_RADIOBUTTON:
                TGRadioButton *rb; 
                cout << "Rb: " << parm1 << endl;
                for (Int_t i=0; i < 9; i++){
                   rb = (TGRadioButton *)fButtons->At(i);
                   if (parm1 != rb->WidgetId()) rb->SetState(kButtonUp);
                }
                break;
             default:
                 break;
          }
          break;
       default:
          break;
   }
   return kTRUE;
}
//_______________________________________________________________________________________

TGMrbGetTextAlignment::~TGMrbGetTextAlignment()
{
// Cleanup dialog.
      fWidgets->Delete();
      delete fWidgets;
}
//_______________________________________________________________________________________

void TGMrbGetTextAlignment::CloseWindow()
{
   DeleteWindow();
}
//_______________________________________________________________________________________
Int_t  GetTextAlign(Int_t defalign, TGWindow *win, const char *helptext)
{ 
   // Prompt for string. The typed in string is returned.

//   static char answer[128];
   Int_t align = defalign;
   new TGMrbGetTextAlignment(&align, win, helptext);
   return align;
}

namespace std {} using namespace std;

#include "TGMrbHelpWindow.h"
#include "TVirtualX.h"
#include <iostream>

//ClassImp(TGMrbHelpWindow)

//______________________________________________________________________________
 TGMrbHelpWindow::TGMrbHelpWindow(const TGWindow *main,
    const char *title, const char *helpText, UInt_t width, UInt_t height) :
    TGTransientFrame(gClient->GetRoot(), main, width, height)
{
   ULong_t bgcol;
   gClient->GetColorByName("firebrick", bgcol);
   // Create a help text dialog.

   fView = new TGTextView(this, width, height, kSunkenFrame | kDoubleBorder);
   //fView = new TGTextView(this, 10, 10, kSunkenFrame | kDoubleBorder);
   fL1 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 3, 3, 3, 3);
   AddFrame(fView, fL1);

//   fOK = new TGTextButton(this, "  &OK  ");
//   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
//   AddFrame(fOK, fL2);

   SetWindowName(title);
   SetIconName(title);

   MapSubwindows();

   Resize(GetDefaultSize());

// position relative to the parent's window
// avoid going off screen
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                          ((TGFrame *) main)->GetWidth() - (fWidth >> 1),
                          (((TGFrame *) main)->GetHeight() + 0*fHeight),
                          ax, ay, wdum);
   Int_t  screen_x, screen_y;
   UInt_t wwu, whu;
   Int_t ww, wh;

//  get screen size in pixels

   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
              screen_x, screen_y, wwu, whu);
   ww = (Int_t)wwu;
   wh = (Int_t)whu;
 
//   cout << "ax, ay " << ax << " " << ay << endl;
//   cout << "w, h " << w << " " << h << endl;
 //  cout << "width, height " << width << " " << height << endl;
   if(ax < 0) ax = 5;
   Int_t ovl = ax + width - ww;
   if(ovl > 0) ax -= ovl;
   if(ay < 0) ay = 5;
   ovl = ay +  height + 30 - wh;
   if(ovl > 0) ay -= ovl;

   Move(ax, ay);
   SetWMPosition(ax, ay);
   ChangeBackground(bgcol);
   fView->LoadBuffer(helpText);
   MapWindow();
}

//______________________________________________________________________________
 TGMrbHelpWindow::~TGMrbHelpWindow()
{
   // Delete help text dialog.

   delete fView;
//   delete fOK;
   delete fL1;
//   delete fL2;

} 
//______________________________________________________________________________
void TGMrbHelpWindow::CloseWindow()
{
   // Called when closed via window manager action.
   cout << "TGMrbHelpWindow::CloseWindow() " << endl;
   DeleteWindow();
}

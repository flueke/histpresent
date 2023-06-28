#ifndef INC_HandleMenus
#define INC_HandleMenus

#include "TList.h"
#include "TCanvas.h"
#include "TGMenu.h"
#include "TGStatusBar.h"
#include <iostream>

namespace std {} using namespace std;

class FitHist;
class HistPresent;
class GrCanvas;
class GEdit;
class TGraph;
class TGraph2D;
class TRootCanvas;

class HandleMenus : public TGFrame {
private:
   GrCanvas       * fHCanvas;
   HistPresent    * fHistPresent; 
   FitHist        * fFitHist;
   TObject        * fObject;
   TGraph         * fGraph1D;
   TGraph2D       * fGraph2D;
	GEdit          * fEditor;
	TH1            * fHistInPad;
	TH1				* fSelHist;
	Int_t fNbinLiveSliceX;
	Int_t fNbinLiveSliceY;
//  from HTRootCanvas
   TRootCanvas        * fRootCanvas;
   TGMenuBar           *fRootsMenuBar;       // menubar
   TGPopupMenu         *fFileMenu;           // file menu
   TGPopupMenu         *fCutsMenu;           // edit menu
   TGPopupMenu         *fDisplayMenu;           // view menu
   TGPopupMenu         *fViewMenu;           // view menu
   TGPopupMenu         *fOptionMenu;         // option menu
   TGPopupMenu         *fAttrMenu;         // option menu
   TGPopupMenu         *fAttrMenuDef;         // option menu
   TGPopupMenu         *fFitMenu;            //
   TGPopupMenu         *fCascadeMenu1;        // classes menu
   TGPopupMenu         *fCascadeMenu2; 
   TGPopupMenu         *fEditMenu;            //
   TGPopupMenu         *fHelpMenu;           // help menu
   TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
   TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
   TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
   TGLayoutHints       *fCanvasLayout;       // layout for canvas widget
   TGStatusBar         *fStatusBar;          // statusbar widget
   TGLayoutHints       *fStatusBarLayout;    // layout hints for statusbar
public:
   HandleMenus(HTCanvas * c, HistPresent * hpr, FitHist * fh, TObject *obj=NULL);
   ~HandleMenus();
   Bool_t   ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void BuildMenus();
   void SetLog(Int_t state);
   void Canvas2RootFile();
   void AsTCanvas2RootFile();
	void SetHist(TH1 *hist) { fSelHist = hist; };
	TH1 *GetHist() { return fSelHist; };
	void SetLiveSliceX(Int_t state);
	void SetLiveSliceY(Int_t state);
	void UseAttrFileHelp();
   ClassDef(HandleMenus,0)  
}; 
 
#endif

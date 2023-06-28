#include "TG3DLine.h"
#include "TGToolBar.h"
#include "TGDockableFrame.h"
#include "TEnv.h"
#include "TSystem.h"
#include "GEdit.h"

#include "TDialogCanvas.h"
#include "TGMrbInputDialog.h"
#include "InsertArcDialog.h"
#include "InsertAxisDialog.h"
#include "InsertTextBoxDialog.h"
#include "InsertPadDialog.h"
#include "InsertTextDialog.h"
#include "ChangeTextDialog.h"
#include "TSplineXDialog.h"
#include "InsertFunctionDialog.h"
#include "InsertImageDialog.h"
#include "InsertHistGraphDialog.h"
#include "FeynmanDiagramDialog.h"
#include "CurlyLineWithArrowDialog.h"
#include "MarkerLineDialog.h"
#include "SetColor.h"

enum ERootCanvasCommands {
   kOptionRefresh,
   kToolLine,
   kToolArrow,
   kToolDiamond,
   kToolEllipse,
   kToolPolyLine,
   kToolAxis,
   kToolTSplineX,
   kToolPad,
   kToolPave,
   kToolGraph,
   kToolHist,
   kToolCurlyLine,
   kToolCurlyLineArrow,
   kToolLatex,
   kToolLatexFile,
   kToolFunction,
   kToolPicture,
   kToolMarker,
   kToolPregion,
   kToolRregion,
   kToolUseGrid,
   kToolRemoveGrid,
   kToolUnZoom,
   kToolZoomIn,
   kToolZoomOut
};
//______________________________________________________________________________
static ToolBarData_t gHprToolBarData[] = {
   // { filename,      tooltip,            staydown,  id,              button}
   { "refresh2.xpm",  "Refresh",          kFALSE,    kOptionRefresh,  0 },
   { "pregion.xpm",    "Define Polygon Region",     kFALSE,    kToolPregion,  0 },
   { "rregion.xpm",   "Define Rectangular Region", kFALSE,    kToolRregion,  0 },
   { "usegrid.xpm",   "Use / Draw Grid",           kFALSE,    kToolUseGrid,  0 },
   { "removegrid.xpm","Remove Grid",               kFALSE,    kToolRemoveGrid, 0 },
   { "zoom_in.xpm",   "Zoom in",           kFALSE,    kToolZoomIn,  0 },
   { "zoom_out.xpm",  "Zoom out",          kFALSE,    kToolZoomOut,  0 },
   { "unzoom.xpm",    "UnZoom",            kFALSE,    kToolUnZoom,  0 },
   { "",               "",                 kFALSE,    -1,              0 },
   { 0,                0,                  kFALSE,    0,               0 }
};
//______________________________________________________________________________

static ToolBarData_t gHprToolBarData1[] = {
   { "line.xpm",       "Line",             kFALSE,    kToolLine,       0 },
   { "arrow.xpm",      "Arrow",            kFALSE,    kToolArrow,      0 },
   { "marker.xpm",     "Marker",           kFALSE,    kToolMarker,     0 },
   { "ellipse.xpm",    "Circle / Arc / Ellipse",    kFALSE,    kToolEllipse,    0 },
   { "graph.xpm",      "Polyline",         kFALSE,    kToolPolyLine,   0 },
   { "gaxis.xpm",      "Axis",             kFALSE,    kToolAxis,   0 },
   { "curlyline.xpm",  "Feynman Diagram",  kFALSE,    kToolCurlyLine,  0 },
   { "curlyline_arrow.xpm","Curlyline with Arrow",  kFALSE,    kToolCurlyLineArrow,  0 },
   { "tsplinex.xpm",   "TSplineX",         kFALSE,    kToolTSplineX,  0 },
   { "pad.xpm",        "Graphics Pad",     kFALSE,    kToolPad,        0 },
   { "pave.xpm",       "TextBox",          kFALSE,    kToolPave,       0 },
   { "latex_keyboard.xpm", "Text/Latex form keyboard", kFALSE,  kToolLatex, 0 },
   { "latex_file.xpm",     "Text/Latex from file",    kFALSE,  kToolLatexFile,0 },
   { "h1_t.xpm",     "Histogram from file",           kFALSE,  kToolHist,      0 },
   { "graph_file.xpm",     "Graph from file",         kFALSE,  kToolGraph,      0 },
   { "function.xpm",       "Function",                kFALSE,  kToolFunction,0 },
   { "picture.xpm",        "Picture, JPEG, GIF, PNG", kFALSE,  kToolPicture,0 },
   { 0,                0,                  kFALSE,    0,               0 }
};

//______________________________________________________________________________
void GEdit::ShowToolBar(Bool_t show)
{
   // Show or hide toolbar.
//	static Int_t first_time = 1;
   fToolDock = fRootCanvas->GetToolDock();
   if ( !fToolDock ) {
      cout << "Cant GetToolDock()" << endl;
      return;
   }
	fRootCanvas->ForceUpdate();
	gSystem->ProcessEvents();
	TString icons_dir =   gEnv->GetValue("Gui.IconPath","");
	if ( !icons_dir.Contains("MARABOU",TString::kIgnoreCase) ) {
	   cout << setred << "Your ICONPATH does not contain $MARABOU/icons" << endl;
		cout << "Please add: \"Gui.IconPath: $MARABOU/icons:$ROOTSYS/icons\" to your $HOME/.rootrc" << endl << "and restart HistPresent"
		     << setblack << endl;
		return;
	}
   if (show && !fToolBar) {
      fHorizontal1Layout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
      fToolBarSep = new TGHorizontal3DLine(fRootCanvas);
		fToolBarLayout = new TGLayoutHints(kLHintsTop |  kLHintsExpandX);
		fToolDock->AddFrame(fToolBarSep, fToolBarLayout);
      fToolBar = new TGToolBar(fToolDock, 60, 20, kHorizontalFrame);
      fToolDock->AddFrame(fToolBar, fHorizontal1Layout);
/*
		TString pm;
		TString roots_icons_dir(gSystem->Getenv("ROOTSYS"));
#ifdef R__WIN32
        roots_icons_dir += "\\icons\\";
#else
        roots_icons_dir += "/icons/";
#endif
		TString marabous_icons_dir(gSystem->Getenv("MARABOU"));
#ifdef R__WIN32
        marabous_icons_dir += "\\icons\\";
#else
        marabous_icons_dir += "/icons/";
#endif
*/
      Int_t spacing = 6, i;
      for (i = 0; gHprToolBarData[i].fPixmap; i++) {
         if (strlen(gHprToolBarData[i].fPixmap) == 0) {
            spacing = 6;
            continue;
         }
		   /*
			if (first_time == 1) {
				pm = gHprToolBarData[i].fPixmap;
				pm.Prepend(marabous_icons_dir.Data());
				if ( !gSystem->AccessPathName(pm) ) {
					gHprToolBarData[i].fPixmap = pm.Data();
				} else {
					pm = gHprToolBarData[i].fPixmap;
					pm.Prepend(roots_icons_dir.Data());
					if ( !gSystem->AccessPathName(pm) ) {
						gHprToolBarData[i].fPixmap = pm.Data();
						cout << "Pm0: " << gHprToolBarData[i].fPixmap << endl;
					} else {
						gHprToolBarData[i].fPixmap = "";
						spacing = 6;
						continue;
					}
				}
			}
		   cout << "Pm: " << gHprToolBarData[i].fPixmap << endl;
			*/
         TGButton * button =
         fToolBar->AddButton(fRootCanvas, &gHprToolBarData[i], spacing);
         button->Associate(this);
         spacing = 0;
      }
      fVertical1 = new TGVertical3DLine(fToolBar);
      fVertical2 = new TGVertical3DLine(fToolBar);
      fVertical1Layout = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 4,2,0,0);
      fVertical2Layout = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
      fToolBar->AddFrame(fVertical1, fVertical1Layout);
      fToolBar->AddFrame(fVertical2, fVertical2Layout);

      spacing = 6;
      for (i = 0; gHprToolBarData1[i].fPixmap; i++) {
         if (strlen(gHprToolBarData1[i].fPixmap) == 0) {
            spacing = 6;
            continue;
         }
			/*
			if (first_time == 1) {
				pm = gHprToolBarData1[i].fPixmap;
				pm.Prepend(marabous_icons_dir.Data());
				if ( !gSystem->AccessPathName(pm) ) {
					gHprToolBarData1[i].fPixmap = pm.Data();
				} else {
					pm = gHprToolBarData1[i].fPixmap;
					pm.Prepend(roots_icons_dir.Data());
					if ( !gSystem->AccessPathName(pm) ) {
						gHprToolBarData1[i].fPixmap = pm.Data();
					} else {
						gHprToolBarData1[i].fPixmap = "";
						spacing = 6;
						continue;
					}
				}
			}
		   cout << "Pm: " << gHprToolBarData1[i].fPixmap << endl;
			*/
         TGButton * button =
         fToolBar->AddButton(fRootCanvas, &gHprToolBarData1[i], spacing);
         button->Associate(this);
         spacing = 0;
      }
      fToolDock->MapSubwindows();
      fToolDock->Layout();
      fToolDock->SetWindowName(Form("ToolBar: %s", fRootCanvas->GetWindowName()));
      fToolDock->Connect("Docked()", "GEdit", this, "AdjustSize()");
      fToolDock->Connect("Undocked()", "GEdit", this, "AdjustSize()");
//		first_time = 0;
   }

   if (!fToolBar) return;

   UInt_t h  = fRootCanvas->GetHeight();
   UInt_t sh = fToolBarSep->GetHeight();
   UInt_t dh = fToolBar->GetHeight();
	fRootCanvas->ForceUpdate();
	gSystem->ProcessEvents();

//	cout << "h, dh, sh " << h<< " " <<dh  << " "  <<sh << endl;

   if (show) {
      fRootCanvas->ShowFrame(fToolDock);
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         fRootCanvas->ShowFrame(fHorizontal1);
//         h = h + sh;
//      }
      fRootCanvas->ShowFrame(fToolBarSep);
//      fViewMenu->CheckEntry(kViewToolbar);
      h = h + dh + sh;
   } else {
      if (fToolDock->IsUndocked()) {
         fToolDock->DockContainer();
         h = h + 2*sh;
      } else h = h - dh;

      fRootCanvas->HideFrame(fToolDock);
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         HideFrame(fHorizontal1);
//         h = h - sh;
//      }
      fRootCanvas->HideFrame(fToolBarSep);
      h = h - sh;
//      fViewMenu->UnCheckEntry(kViewToolbar);
   }
   fRootCanvas->Resize(fRootCanvas->GetWidth(), h);
	fRootCanvas->ForceUpdate();
	gSystem->ProcessEvents();
}

//______________________________________________________________________________
void GEdit::AdjustSize()
{
   // Keep the same canvas size while docking/undocking toolbar.

   if (fToolDock->IsUndocked()) {
      ExecuteAdjustSize(-1);   //smaller
   } else {
      ExecuteAdjustSize( 1);   //bigger
   }
}
//______________________________________________________________________________
void GEdit::ExecuteAdjustSize(Int_t how)
{
   // Keep the same canvas size while docking/undocking toolbar.
   if (how == 0 ) return;
   UInt_t h  = fRootCanvas->GetHeight();
   UInt_t dh = fToolBar->GetHeight();
   UInt_t sh = fToolBarSep->GetHeight();
   if ( how < 0) {
      fRootCanvas->HideFrame(fToolBarSep);
      h = h - dh - sh;
   } else {
      fRootCanvas->ShowFrame(fToolBarSep);
      h = h + dh + sh;
   }
   fRootCanvas->Resize(fRootCanvas->GetWidth(), h);
}
//______________________________________________________________________________
Bool_t GEdit::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.
	if (gDebug > 0)
		cout << "GEdit::ProcessMessage: " << parm1 << " gPad " << gPad->GetName()<< endl;
   fParent->cd(); 
	fParent->Pop(); 
   switch (GET_MSG(msg)) {

      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:

               switch (parm1) {
                  // Handle toolbar items...
                  case kOptionRefresh:
                     fParent->Paint();
                     fParent->Update();
                     break;

                  case kToolPregion:
                     DefinePolygone();
                     break;
                  case kToolRregion:
                     DefineBox();
                     break;
                  case kToolUseGrid:
                     SetUseEditGrid(1);
                     DrawEditGrid(kTRUE);
                     break;
                  case kToolRemoveGrid:
                     SetUseEditGrid(0);
                     RemoveEditGrid();
                     break;

                   case kToolZoomIn:
                     ZoomIn();
                     break;

                 case kToolZoomOut:
                     ZoomOut();
                     break;

                  case kToolUnZoom:
                     UnZoom();
                     break;

                  case kToolPolyLine:
                  case kToolLine:
					   case kToolArrow:
					   case kToolMarker:
                    new MarkerLineDialog();
                     break;
                  case kToolAxis:
                    new InsertAxisDialog();
                     break;
                  case kToolEllipse:
                     new InsertArcDialog();
                     break;
                  case kToolPad:
                     new InsertPadDialog();
                     break;
                  case kToolPave:
                     new InsertTextBoxDialog();
                     break;

                  case kToolHist:
                     new InsertHistGraphDialog(0);
                     break;
                   case kToolGraph:
                     new InsertHistGraphDialog(1);
                     break;
                 case kToolTSplineX:
                     new TSplineXDialog();
                     break;
                  case kToolCurlyLine:
                     new FeynmanDiagramDialog();
                     break;
                  case kToolCurlyLineArrow:
                     new CurlyLineWithArrowDialog();
                     break;
                  case kToolLatex:
                     new InsertTextDialog();
                     break;
                   case kToolLatexFile:
                     new InsertTextDialog(kTRUE);
                     break;
                 case kToolFunction:
                     new InsertFunctionDialog();
                     break;
                  case kToolPicture:
                     new InsertImageDialog();
                    break;
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}


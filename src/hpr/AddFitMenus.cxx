#include "TROOT.h"
#include "TEnv.h"
#include "TH1.h"
#include "TCanvas.h"
#include "AddFitMenus.h"
#include "CalibrationDialog.h"
#include "FindPeakDialog.h"
#include "FitOneDimDialog.h"
#include "Save2FileDialog.h"
#include <fstream>
//______________________________________________________________________________

ClassImp(AddFitMenus)

enum Ecmds {kFHFitGausLBg, kFHFitExp, kFHFitPol, kFHFitForm,
            kFHCalibrate, kFHFindPeaks,
            M_Save2File=1002};

AddFitMenus::AddFitMenus(TCanvas *canvas, TH1 *hist)
                         : fCanvas(canvas), fHist(hist)
{
//   cout << "AddFitMenus::BuildMenu() " <<this << endl;
   gROOT->GetListOfCleanups()->Add(this);
   fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
   TGMenuBar * menubar = fRootCanvas->GetMenuBar();
   TGLayoutHints * layoh_right = new TGLayoutHints(kLHintsTop | kLHintsLeft);

   fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
   menubar->AddPopup("Fit", fMenu, layoh_right, menubar->GetPopup("Inspect"));
	fMenu->AddEntry("Fit Gaussians (with tail)",   kFHFitGausLBg);
	fMenu->AddEntry("Fit Exponential",             kFHFitExp    );
	fMenu->AddEntry("Fit Polynomial",              kFHFitPol    );
	fMenu->AddEntry("Fit User formula",            kFHFitForm   );
	fMenu->AddEntry("Calibration Dialog",       kFHCalibrate);
	fMenu->AddEntry("FindPeaks",                kFHFindPeaks);

   TGPopupMenu * filemenu = menubar->GetPopup("File");
   if (filemenu) {
      const TList * el = filemenu->GetListOfEntries();
      TGMenuEntry *en = (TGMenuEntry*)el->First();
      filemenu->AddEntry("Save hist to rootfile", M_Save2File, NULL, NULL, en);
      filemenu->Connect("Activated(Int_t)", "AddFitMenus", this,
                      "HandleMenu(Int_t)");
   }
//   fMenu->AddEntry("Save hist to rootfile", M_Save2File);

   fMenu->Connect("Activated(Int_t)", "AddFitMenus", this,
                      "HandleMenu(Int_t)");

   menubar->MapSubwindows();
   menubar->Layout();
}
//____________________________________________________________________________
AddFitMenus::~AddFitMenus()
{
   gROOT->GetListOfCleanups()->Remove(this);
};
//__________________________________________________________________________

void AddFitMenus::RecursiveRemove(TObject * obj)
{
//   cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "
//        << obj << " fSelHist " <<  fSelHist <<  endl;
   if (obj == fCanvas) {
 //      cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "
 //       << obj << " fSelHist " <<  fSelHist <<  endl;
      delete this;
   }
}

//________________________________________________________________________

void AddFitMenus::HandleMenu(Int_t id)
{
   switch (id) {

		case kFHFitGausLBg:
		   new FitOneDimDialog(fHist, 1);
		   break;

		case kFHFitExp:
		   new FitOneDimDialog(fHist, 2);
		   break;

		case kFHFitPol:
		   new FitOneDimDialog(fHist, 3);
	   	break;

		case kFHFitForm:
		   new FitOneDimDialog(fHist, 4);
		   break;

		case kFHCalibrate:
		   new CalibrationDialog(fHist, 1);
		   break;

		case  kFHFindPeaks:
		   new FindPeakDialog(fHist, 1);
		   break;

      case M_Save2File:
         Save2FileDialog sfd(fHist);
         break;
   }
}

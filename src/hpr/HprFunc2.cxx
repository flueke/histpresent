#include <TROOT.h>
#include <TEnv.h>
#include <TStyle.h>
#include "SetColor.h"
#include "TGMrbValuesAndText.h"
#include "Set2DimOptDialog.h"
#include "SetColorModeDialog.h"
#include "SetCanvasAttDialog.h"
#include "WindowSizeDialog.h"
#include "Fit2DimDialog.h"

#include "HprFunc2.h"
extern Int_t gHprDebug;

ClassImp (HprFunc2)

enum EGoHCommandIds {
	M_Option2Dim,
	M_Option2DimCol,
	M_DrawFit
};
//________________________________________________________________________

HprFunc2::HprFunc2(TF2 *f2)
				: fFunc2(f2)
{
//	static const Char_t helptext[] =
//	"Handle 2 dim functions";
	if (gHprDebug > 0)
   cout << " ctor HprFunc2::" << this << endl;
	fCanvas = NULL;
//   TH1* hist = 0;
	if (fFunc2== 0) {
		cout << setred << "No Function" << setblack << endl;
		return;

	} 		if (WindowSizeDialog::fNwindows>0) {       // not the 1. time
			if (WindowSizeDialog::WindowSizeDialog::fWinshiftx != 0 && WindowSizeDialog::WindowSizeDialog::fNwindows%2 != 0) WindowSizeDialog::WindowSizeDialog::fWincurx += WindowSizeDialog::fWinshiftx;
			else   {WindowSizeDialog::fWincurx = WindowSizeDialog::fWintopx; WindowSizeDialog::fWincury += WindowSizeDialog::fWinshifty;}
		}
		WindowSizeDialog::fNwindows++;
		TString name=fFunc2->GetName();
		TEnv env(".hprrc");
		TString opt = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "SURF2");
		if ( opt.Contains("GL") )
			gStyle->SetCanvasPreferGL();
		fCanvas = new HTCanvas(name,name, WindowSizeDialog::fWincurx, WindowSizeDialog::fWincury,
				WindowSizeDialog::fWinwidx_2dim, WindowSizeDialog::fWinwidy_2dim);
//      fCanvasList->Add(ccc);
		BuildMenu();
		fFunc2->Draw(opt);
		fCanvas->Modified();
		fCanvas->Update();
		cout << "Parameter values of: " << name << endl;
		for (Int_t i = 0; i < fFunc2->GetNpar(); i++) {
			cout << fFunc2->GetParName(i) << ": " << fFunc2->GetParameter(i) << endl;
		}
		cout << "-----------------------------------------------" << endl;
}
		
//________________________________________________________________________

HprFunc2::~HprFunc2()
{
//   cout <<"dtor HprFunc2, fDialog " << fDialog << endl;
}
//________________________________________________________________________

void HprFunc2::BuildMenu()
{
//   cout << "HprFunc2::BuildMenu() " <<this << endl;
	fRootCanvas = (TRootCanvas*)fCanvas->GetCanvas()->GetCanvasImp();
	TGMenuBar * menubar = fRootCanvas->GetMenuBar();
	TGLayoutHints * layoh_left = new TGLayoutHints(kLHintsTop | kLHintsLeft);
	fMenu     = new TGPopupMenu(fRootCanvas->GetParent());
	menubar->AddPopup("Display", fMenu, layoh_left, menubar->GetPopup("Help"));
	fMenu->AddEntry("How to display a 2 dim function", M_Option2Dim);
	fMenu->AddEntry("Color Palette selection", M_Option2DimCol);
	fMenu->AddEntry("Change params, Redraw", M_DrawFit);
	
	fMenu->Connect("Activated(Int_t)", "HprFunc2", this,
								"HandleMenu(Int_t)");
	menubar->MapSubwindows();
	menubar->Layout();
}
//________________________________________________________________________

void HprFunc2::HandleMenu(Int_t id)
{
	switch (id) {

//		case M_OptionHist:
//			if ( GeneralAttDialog::fStackedPads ) {
//				new SetHistOptDialog(fRootCanvas, fStack->GetHists());
//			} else {
//				TList *temp = new TList();
//				temp->Add(fStack->GetHistogram());
//				new SetHistOptDialog(fRootCanvas, temp);
//			}
		case M_Option2Dim:
			new Set2DimOptDialog(fRootCanvas);
			break;
		case M_Option2DimCol:
			new SetColorModeDialog(fRootCanvas);
			break;
		case M_DrawFit:
//			cout << "  HprFunc2::new Fit2DimDialog(fFunc2);" << endl;
			new Fit2DimDialog(fFunc2);
			break;
	}
}


#ifndef __AddFitMenus__
#define __AddFitMenus__
#include "TH1.h"
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMenu.h"
#include "TString.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class AddFitMenus : public TObject {

private:
   TCanvas *fCanvas;
   TH1     *fHist;
   TRootCanvas *fRootCanvas;
   TGPopupMenu *fMenu;

public:
   AddFitMenus(TCanvas *canvas, TH1 *hist);
   AddFitMenus(){ fCanvas = NULL; };
   ~AddFitMenus();
   void RecursiveRemove(TObject*);
   void HandleMenu(Int_t id);

ClassDef(AddFitMenus,0)
};
#endif

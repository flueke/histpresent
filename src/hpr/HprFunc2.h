#ifndef HPRFUNC2
#define HPRFUNC2
#include <TVirtualPad.h>
#include <TGWindow.h>
#include <HTCanvas.h>
#include <TRootCanvas.h>
#include <TF2.h>
#include <TGMenu.h>
#include <iostream>
//#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


namespace std {} using namespace std;

class TGMrbValuesAndText;

class HprFunc2 : public TNamed {

private:
	HTCanvas    *fCanvas;
	TF2         *fFunc2;
	TGPopupMenu *fMenu;
	TRootCanvas *fRootCanvas;
	TString fDrawOpt;
public:
	HprFunc2(TF2 * f2);
	HprFunc2(){};
	~HprFunc2();
	void   RecursiveRemove(TObject* /* obj */) {};
	void   BuildMenu();
	void   HandleMenu(Int_t id);
	void   SetOptions(){};
ClassDef(HprFunc2,0)
};
#endif

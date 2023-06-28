#ifndef HPRPLANEMANAGER
#define HPRPLANEMANAGER
#include "TGWindow.h"
#include "GrCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


class HprPlaneManager : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TGWindow    *fWindow;
   TCanvas     *fCanvas;
   GrCanvas    *fGrCanvas;
   Int_t       fCurrentActive;
   Int_t       fBidCurrentActive;
   Int_t       fVisibility[100];
   Int_t       fUsedPlanes[100];
   Int_t       fMoveToPlane[100];
   Int_t       fPlaneIndexVis[100];
   Int_t       fPlaneIndexMove[100];
public:
   HprPlaneManager(TGWindow * win = NULL);
   ~HprPlaneManager() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void OpenWidget();
   void CloseWidget();
   void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
   Int_t FillPlaneList();
   Int_t MovePlaneNumber(Int_t from, Int_t to);
ClassDef(HprPlaneManager,0)
};
#endif

#include "TGMrbValuesAndText.h"
#include "TH3.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TString.h"

class HprTh3Dialog : public TObject {
private:
   enum { kMaxHists = 10};
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   Int_t fNhists;
   TList fHistList;
   TH3 *fHist;
   Int_t fSetIndex[kMaxHists * 3 + 10];

   Int_t fMarkerStyle[kMaxHists];
   Color_t fMarkerColor[kMaxHists];
   Size_t  fMarkerSize[kMaxHists];
   TString fProjectPlane;
public:
   HprTh3Dialog(TH3 *hist, TGWindow *win);
   HprTh3Dialog() {fRow_lab = NULL; fHist=NULL;};
   virtual ~HprTh3Dialog();
   void    ExecuteProject();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(HprTh3Dialog, 0)

};

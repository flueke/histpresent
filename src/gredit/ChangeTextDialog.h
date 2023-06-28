#ifndef CHANGETEXTDIALOG
#define CHANGETEXTDIALOG
#include "TObject.h"
//#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
//_____________________________________________________________________________________


class TGMrbValuesAndText;

class ChangeTextDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   Size_t         fChangeTextSizeMin;
   Size_t         fChangeTextSizeMax;
   Size_t         fChangeTextSize;
   Float_t        fChangeTextAngle;
   Short_t        fChangeTextAlign;
   Color_t        fChangeTextColor;
   Font_t         fChangeTextFont;
   Int_t          fChangeTextPrec;

   Int_t          fDoChangeTextSize;
   Int_t          fDoChangeTextAngle;
   Int_t          fDoChangeTextAlign;
   Int_t          fDoChangeTextColor;
   Int_t          fDoChangeTextFont;
   Int_t          fDoChangeTextPrec;
public:
   ChangeTextDialog();
   virtual ~ChangeTextDialog();
   void ChangeTextExecute();
   void SaveDefaults();
   void RestoreDefaults();
//   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(ChangeTextDialog,0)
};
#endif

#ifndef INSERTTEXTDIALOG
#define INSERTTEXTDIALOG
#include "TObject.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
//_____________________________________________________________________________________


class InsertTextDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas        *fCanvas;
	TPad           * fPad;
   TObject        *fCaller;
   TString        *fEditTextPointer;
   TString        fEditTextFileName;
   Int_t          fEditTextFromFile;
   Double_t       fEditTextRadius;
   Double_t       fEditTextStartAngle;
   Double_t       fEditTextX0;
   Double_t       fEditTextY0;
   Double_t       fEditTextDy;
   Double_t       fEditTextCharSep;
   Size_t         fEditTextSize;
   Float_t        fEditTextAngle;
   Short_t        fEditTextAlign;
   Color_t        fEditTextColor;
   Font_t         fEditTextFont;
   Int_t          fEditTextPrec;
   Int_t          fEditTextMarkCompound;
   Int_t          fEditTextLatexFilter;
   Int_t          fEditTextSeqNr;
public:
   InsertTextDialog(Bool_t from_file = kFALSE, TObject *caller = NULL);
   virtual ~InsertTextDialog();
   void InsertTextExecute(Int_t onarc = 0);
   void InsertTextOnArc();
   TString        *GetTextPointer() { return fEditTextPointer; } ;
   Size_t GetEditTextSize() { return fEditTextSize; };
   Float_t GetEditTextAngle() { return fEditTextAngle; };
   Short_t GetEditTextAlign() { return fEditTextAlign; };
   Color_t GetEditTextColor() { return fEditTextColor; };
   Font_t GetEditTextFont() { return fEditTextFont; };
   Int_t GetEditTextPrec()  { return fEditTextPrec; };
   Double_t GetEditTextX0() { return fEditTextX0; };
   Double_t GetEditTextY0() { return fEditTextY0; };

	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
   void SaveDefaults();
   void RestoreDefaults();
   void Show_Head_of_File();
   void    CloseDialog();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(InsertTextDialog,0)
};
#endif

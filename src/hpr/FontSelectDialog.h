#ifndef FONTSELECTDIALOG
#define FONTSELECTDIALOG
#include "TEnv.h"

class TGMrbValuesAndText;
class TCanvas;
class TList;
class TObject;
class TGWindow;
class TString;

//_____________________________________________________________________________________
/*
Gui.DefaultFont:            -*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1
Gui.MenuFont:               -*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1
Gui.MenuHiFont:             -*-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1
Gui.DocFixedFont:           -*-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1
Gui.DocPropFont:            -*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1
Gui.IconFont:               -*-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1
Gui.StatusFont:             -*-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1
*/

class FontSelectDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   
   TString fDefaultFont;  
   TString fMenuFont;     
   TString fMenuHiFont;   
   TString fDocFixedFont; 
   TString fDocPropFont;  
   TString fIconFont;     
   TString fStatusFont;
   
   TString fDefaultFontFamily;  
   TString fMenuFontFamily;     
   TString fMenuHiFontFamily;   
   TString fDocFixedFontFamily; 
   TString fDocPropFontFamily;  
   TString fIconFontFamily;     
   TString fStatusFontFamily;
      
   TString fDefaultFontWeight;  
   TString fMenuFontWeight;     
   TString fMenuHiFontWeight;   
   TString fDocFixedFontWeight; 
   TString fDocPropFontWeight;  
   TString fIconFontWeight;     
   TString fStatusFontWeight;   
      
   Int_t   fDefaultFontSize;  
   Int_t   fMenuFontSize;     
   Int_t   fMenuHiFontSize;   
   Int_t   fDocFixedFontSize; 
   Int_t   fDocPropFontSize;  
   Int_t   fIconFontSize;     
   Int_t   fStatusFontSize;   
   
public:

   FontSelectDialog(TGWindow * win = NULL);
   ~FontSelectDialog();
   void Apply();
   void CloseDialog();
   void SaveDefaultsCwd();
   void SaveDefaultsHome();
   void SaveDefaults(EEnvLevel level);
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(FontSelectDialog,0)
};
#endif

#ifndef HPR_TGMRBGETTEXTALIGNMENT
#define HPR_TGMRBGETTEXTALIGNMENT
#include <TGWindow.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include "TOrdCollection.h"
#include "TArrayI.h"
//_________________________________________________________________________________________

Int_t  GetTextAlign(Int_t defalign, TGWindow *win = 0, const char *helptext = 0);
//_________________________________________________________________________________________

class TGMrbGetTextAlignment : public TGTransientFrame {

private:
   Int_t * fAlign;
   const TGWindow   *fMyWindow;
   TList            *fWidgets;   // keep track of widgets to be deleted in dtor
   const char       *fHelpText;
   TList				  *fButtons;

public:
   TGMrbGetTextAlignment(Int_t * align, const TGWindow *win=0, const char *helptext=0);
   virtual ~TGMrbGetTextAlignment();
   void CloseWindow();
   virtual Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

ClassDef(TGMrbGetTextAlignment,0)		// [GraphUtils] A dialog box
};   
#endif

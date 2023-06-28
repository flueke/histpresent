
#ifndef HPR_HelpWindow
#define HPR_HelpWindow
#include "TGTextView.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HelpWindow                                                           //
//                                                                      //
// A HelpWindow is used to display help text (or any text in a          //
// dialog window). There is on OK button to popdown the dialog.         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif

class TGMrbHelpWindow : public TGTransientFrame {

private:
   TGTextView       *fView;   // text view
   TGLayoutHints    *fL1;     // layout of TGTextView

public:
   TGMrbHelpWindow(const TGWindow *main, const char *title, 
              const char *helpText, UInt_t w, UInt_t h);
   virtual ~TGMrbHelpWindow();

   void   CloseWindow();

   ClassDef(TGMrbHelpWindow,0)  // [GraphUtils] Dialog to display help text
};

#endif

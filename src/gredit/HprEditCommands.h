#ifndef HPR_EDITCOMMANDS
#define HPR_EDITCOMMANDS

#include <TGWindow.h>
#include <TGButton.h>
#include <TList.h>

class  HprEditCommands : public TGTransientFrame {

private:
   const TGWindow   *fMyWindow;
   TList            *fButtons;
   TObject          *fCalling;
   TList            *fWidgets;   // keep track of widgets to be deleted in dtor

public:
   HprEditCommands(const TGWindow *Win, Int_t win_width, 
                 TObject * calling_class, const char * cname,
                 TList * labels, TList * methods);

   virtual ~HprEditCommands();
ClassDef(HprEditCommands,0)		
};   
#endif

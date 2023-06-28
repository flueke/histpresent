#ifndef HPR_DIALOGS
#define HPR_DIALOGS
#include <TGWindow.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
//_________________________________________________________________________________________

const char *GetString(const char *prompt, const char *defval, 
                             Bool_t * Ok=0, TGWindow *win=0, 
                             const char *ynprompt=0,Bool_t * yn=0,
                             const char *helptext=0,
                             const char *ynprompt1=0,Bool_t * yn1=0,
                             const char * FileName=0, TList * complist=0);
Int_t GetInteger(const char *prompt, Int_t defval, 
                        Bool_t * Ok=0, TGWindow *win=0, 
                        const char *ynprompt=0, Bool_t * yn=0,
                        const char *helptext=0,
                        const char *ynprompt1=0,Bool_t * yn1=0);
Float_t GetFloat(const char *prompt, Float_t defval, 
                        Bool_t * Ok=0, TGWindow *win=0, 
                        const char *ynprompt=0, Bool_t * yn=0,
                        const char *helptext=0,
                        const char *ynprompt1=0,Bool_t * yn1=0);
//_________________________________________________________________________________________

class TGMrbInputDialog : public TGTransientFrame{

private:
   const TGWindow   *fMyWindow;
   TGTextEntry      *fTE;      // text entry widget containing
   TList            *fWidgets; // keep track of widgets to be deleted in dtor
   char             *fRetStr;  // address to store return string
   const char       *fDefVal;  // address to store return string
   const char       *fHelpText;  // address to store return string
   TGCheckButton    *fCheckYesNo; //
   TGCheckButton    *fCheckYesNo1; //
   Bool_t           *fRetYesNo;    // address to store return boolean
   Bool_t           *fRetYesNo1;   // address to store return boolean
   Int_t            *fReturn;      // address to store cancel = -1 / ok =0 
   TGListBox        *fListBox;
   TString          fFileName;
   TList            *fCompList;
public:
   TGMrbInputDialog(const char *Prompt, const char *DefVal,Int_t * Return,
                    char *retstr, const TGWindow *Win = 0, 
                    const char *YNPrompt = 0, Bool_t * YesNo = 0 ,
                    const char *HelpText = 0,
                    const char *YNPrompt1 = 0, Bool_t * YesNo1 = 0,
                    const char * FileName = 0, TList * complist=0);
   virtual ~TGMrbInputDialog();
   void CloseWindow();
   void ReallyDelete(){delete this;};
   virtual Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void SaveList();

ClassDef(TGMrbInputDialog,0)		// [GraphUtils] A dialog box
};   
#endif

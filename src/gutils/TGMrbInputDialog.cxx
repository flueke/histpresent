namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include "TRootHelpDialog.h"
#include <TList.h>
#include <TObjString.h>
#include <TRegexp.h>

#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include "TGMrbInputDialog.h"
#include "TGMrbHelpWindow.h"
#include "SetColor.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget to request input of a string                            //    
// optionally a Checkbutton is displayed to provide a yes-no selction    //
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const char *Prompt: the Prompt string                                //
//  const char *DefVal: the default value of the string                  //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //                  
//  char *retstr      : the returned string                              //
//                                                                       //
// optional arguments:                                                   //
//                                                                       //
// TGWindow *win       : pointer to parent window                        //  
// const char *YNPrompt: Prompt for Checkbutton                          //
// Bool_t * YesNo:       truth value of Checkbutton                      //
// const char *HelpText: help text                                       //
// const char *YNPrompt1:Prompt for second Checkbutton                   //   
// Bool_t * YesNo1:      truth value of second Checkbutton               //
// const char *FileName: a file containing lines from which a selection  //
//                       may be made, a TGListBox is used in this case   //
// TList * Complist:     list containing TObjStrings used for            //
//                       command completion                              //
//                                                                       //
// This class is normally accessed via the following utility functions:  //
//                                                                       //
// const char * GetString(const char *Prompt, const char * DefVal,       //
// 							 Bool_t * Ok = 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0, Bool_t * yn=0,			 //
// 							 const char *helptext=0);								 //
// 																							 //
// Int_t  	  GetInteger(const char *Prompt, Int_t *DefVal, 				 //
// 							 Bool_t * Ok= 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0,Bool_t * yn=0, 			 //
// 							 const char *helptext=0);								 //
// Float_t		 GetFloat(const char *Prompt, Float_t *DefVal,				 //
// 							 Bool_t * Ok = 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0, Bool_t * yn=0,			 //
// 							 const char *helptext=0);								 //
//  Note: most arguments have defaults, ok is true if OK buttom pressed  //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

//________________________________________________________________________________________
/*
Ssiz_t IndexOfLastSpecial(TString &str) 
{
   // return index  of not (letter, number or _ ) in str searching backwards
   // Skip ::
   TRegexp spc("[^a-zA-Z0-9_]", kFALSE);
   if (str.Index(spc) < 0) return -1;    // no occurence
   Int_t len = str.Length();
   Int_t ind = len - 1;
   Int_t indspc;
   TString onec;
   const char *sp = str.Data();
   char c = {':'};
   while (ind >= 0) {
//      cout << ind << " " << str.Index(spc, ind) << endl;
      onec = str(ind, 1);
      indspc = onec.Index(spc);
      if (indspc == 0) {
         if (sp[ind] != c) return ind;
         if (sp[ind] == c && (sp[ind -1] != c || ind == 0)) {
            if (ind == 0) cout << setred 
                         << "Warning: Expression begins with ':' "
                         << setblack << endl;
            return ind;
         }
         ind--;
      }
      ind--;
   }
   return -1;
} 
//________________________________________________________________________________________

Ssiz_t IndexOfLast(TString &str, char &c) 
{
   // return index of detached char c in str searching backwards
   if (str.Index(c) < 0) return -1;    // no occurence
   Int_t len = str.Length();
   const char *sp = str.Data();
   cout << sp << endl;
   Int_t ind = len - 1;
   while (ind > 0) {
      if (sp[ind] == c && sp[ind -1] != c) return ind;
      ind--;
      if (sp[ind+1] == c)ind--;
   }
   return -1;
} 
//________________________________________________________________________________________

Int_t Matches(TList * list, const char * s, Int_t * matchlength)
{
   Int_t pos = -1;
   Bool_t unique = kTRUE;
   TObjString * obs;
   Int_t lmax = 1000000;
   for (Int_t i = 0; i < list->GetSize(); i++) {
      obs = (TObjString *)list->At(i);
      TString var = obs->GetString();
      if (var.Index(s, 0) == 0) {
         cout << "Input: " << s << " matches: " << var << endl;
         if (pos >=0) unique = kFALSE;
         pos = i;
         if (var.Length() < lmax) lmax = var.Length(); 
         *matchlength = var.Length();
      }
   }
   if (unique) {
      return pos;
   } else {
      cout << setred << "Not unique, find  longest match: lmax = " << lmax << setblack << endl;
      Int_t lm = 0;
      for (Int_t l = 0; l < lmax; l++) {
         Bool_t fm = kTRUE;
         char cm ={' '};
         for (Int_t i = 0; i < list->GetSize(); i++) {
            obs = (TObjString *)list->At(i);
            TString var = obs->GetString();
            if (var.Index(s, 0) != 0) continue;
            if (fm) {
               cm = var[l];
               fm = kFALSE;
            } else {
               if (var[l] != cm) {
                  *matchlength = lm;
//                  cout << "pso, lm: " << pos << " " << lm << endl;
                  return pos;
               } 
            }
         }
         lm++;    
      }
   }
   *matchlength = lmax;
   return pos;
}
;
*/
extern Ssiz_t IndexOfLastSpecial(TString &str);
extern Ssiz_t IndexOfLast(TString &str, char &c);
extern Int_t Matches(TList * list, const char * s, Int_t * matchlength);

//________________________________________________________________________________________


ClassImp(TGMrbInputDialog)

TGMrbInputDialog::TGMrbInputDialog(const char *Prompt, const char *DefVal, 
                         Int_t * Return, char *retstr,const TGWindow *Win,
                         const char *YNPrompt, Bool_t * YesNo,
                         const char *HelpText,
                         const char *YNPrompt1, Bool_t * YesNo1,
                         const char * FileName, TList * Complist):
							    TGTransientFrame(gClient->GetRoot(), Win, 10, 10)
{
   // Create  input dialog.

   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);

   fWidgets = new TList;
   fListBox = NULL;
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;
   fCompList = Complist;
   // create Prompt label and textentry widget
   TGLabel *label = new TGLabel(this, Prompt);
   fWidgets->AddFirst(label);

   TGTextBuffer *tbuf = new TGTextBuffer(256);  // will be deleted by TGtextEntry
   tbuf->AddText(0, DefVal);

   fTE = new TGTextEntry(this, tbuf);
   Int_t wid = 8 * strlen(DefVal);
   fTE->Resize(TMath::Max(wid,320), fTE->GetDefaultHeight());
   fTE->Associate(this);
   fWidgets->AddFirst(fTE);

   TGLayoutHints *l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
   TGLayoutHints *l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fWidgets->AddFirst(l1);
   fWidgets->AddFirst(l2);

   this->AddFrame(label, l1);
   this->AddFrame(fTE, l2);

   // optionally create a ListBox from which items can be selected

   fFileName = FileName;         // remember
   if (FileName != NULL) {
      if (gSystem->AccessPathName(FileName)) {
         cout << "Warning: File with selections not found: " << FileName << endl;
      } else {
         ifstream selections(FileName);
         TString line;
         Int_t id = 0;
         fListBox = new TGListBox(this, 99);  
         while (1) {
            line.ReadLine(selections);
            if (selections.eof()) break;
            fListBox->AddEntry(line.Data(), id);
            if (id == 00 && (Int_t)fTE->GetBuffer()->GetTextLength() == 0) {
               fTE->GetBuffer()->AddText(0, line.Data());
               gClient->NeedRedraw(fTE);
            }
            id++;
         }
         selections.close();
         cout << "Entries: " << id << endl;
         wid = 8 * fTE->GetBuffer()->GetTextLength();
         fListBox->Resize(TMath::Max(wid,320), TMath::Min(200, id*20));
         fListBox->Layout();
         fListBox->Associate(this);
         fWidgets->AddFirst(fListBox);
         this->AddFrame(fListBox, l2);
      }
   }
   wid = 8 * fTE->GetBuffer()->GetTextLength();
   fTE->Resize(TMath::Max(wid,320), fTE->GetDefaultHeight());
   // create frame and layout hints for Ok and Cancel buttons

   TGHorizontalFrame *hf = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
   fWidgets->AddFirst(hf);
   TGLayoutHints     *l3 = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);

   // put hf as last in list to be deleted
   fWidgets->AddFirst(l3);

   if(YNPrompt && YesNo){
      fCheckYesNo = new TGCheckButton(this, YNPrompt, 0);
      if(*YesNo)fCheckYesNo->SetState(kButtonDown);
      fWidgets->AddFirst(fCheckYesNo);
      this->AddFrame(fCheckYesNo, l2);
   }
   if(YNPrompt1 && YesNo1){
      fCheckYesNo1 = new TGCheckButton(this, YNPrompt1, 0);
      if(*YesNo1)fCheckYesNo1->SetState(kButtonDown);
      fWidgets->AddFirst(fCheckYesNo1);
      this->AddFrame(fCheckYesNo1, l2);
   }
   // create OK and Cancel buttons in their own frame (hf)

   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", 1);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   b = new TGTextButton(hf, "&Cancel", 2);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   if(HelpText){
      b = new TGTextButton(hf, "Help", 3);
      fWidgets->AddFirst(b);
      b->Associate(this);
      hf->AddFrame(b, l3);
      height = b->GetDefaultHeight();
      width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   }
   // place button frame (hf) at the bottom
   TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
   fWidgets->AddFirst(l4);

   this->AddFrame(hf, l4);

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * nb, height);

   // set dialog title
   this->SetWindowName("Get Input");

   // map all widgets and calculate size of dialog
   this->MapSubwindows();

   width  = this->GetDefaultWidth();
   height = this->GetDefaultHeight();

   this->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
         ((TGFrame *) fMyWindow)->GetWidth() - width >> 1,
         ((TGFrame *) fMyWindow)->GetHeight() - height >> 1,
                          ax, ay, wdum);
   UInt_t ww, wh;
   Int_t  wwi,whi;
   Int_t screen_x, screen_y;
//  screen size in pixels
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
              screen_x, screen_y, ww, wh);
   wwi = ww;
   whi = wh;
   Int_t wi = width;
   Int_t hi = height;
//  make sure its inside physical screen
   if      (ax < 0)    ax = 10;
   else if (ax + wi > wwi)  ax = wwi - wi - 5;
   if      (ay + hi/2 < 0) ay =  hi/2 + 5;
   else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);

   // make the message box non-resizable
   this->SetWMSize(width, height);
   this->SetWMSizeHints(width, height, width, height, 0, 0);

   this->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);

   // popup dialog and wait till user replies
   this->MapWindow();
   this->ChangeBackground(brown);
   hf->ChangeBackground(brown);

   fRetStr = retstr;
   fDefVal = DefVal;
   fReturn = Return;
   fRetYesNo = YesNo;
   fRetYesNo1 = YesNo1;
   fHelpText = HelpText;
   gClient->WaitFor(this);
};
//____________________________________________________________________________

TGMrbInputDialog::~TGMrbInputDialog()
{
// Cleanup dialog.
      fWidgets->Delete();
      fWidgets->Clear();
      delete fWidgets;
}
//____________________________________________________________________________

void TGMrbInputDialog::CloseWindow()
{
   DeleteWindow();
}
//____________________________________________________________________________

Bool_t TGMrbInputDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON:
                switch (parm1) {
                   case 1:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fTE->GetBuffer()->GetString());
                      if (fFileName.Length() > 0) this->SaveList();
                      if(fRetYesNo){
                         if(fCheckYesNo->GetState() == kButtonUp) *fRetYesNo = kFALSE;
                         else                                     *fRetYesNo = kTRUE;
                      }
                      if(fRetYesNo1){
                         if(fCheckYesNo1->GetState() == kButtonUp) *fRetYesNo1 = kFALSE;
                         else                                      *fRetYesNo1 = kTRUE;
                      }
                      *fReturn = 0;
                      CloseWindow();
//                      delete this;
                      break;

                    case 2:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fTE->GetBuffer()->GetString());
                      if (fFileName.Length() > 0) this->SaveList();
                      *fReturn = -1;
                      CloseWindow();
//                      delete this;
                      break;
                  case 3:
                      new TGMrbHelpWindow(this, "HistPresent Help", fHelpText, 550, 500);
                      break;
                 }
             case kCM_LISTBOX:
                {
//                cout << "LISTBOX: " << parm1 << " " << parm2 << endl;
                if (parm1 != 99) break;
                TGTextLBEntry * tge = (TGTextLBEntry *)fListBox->GetEntry(parm2);
                TString text = tge->GetText()->GetString();
 //               fTE->GetBuffer()->RemoveText(0, (Int_t)fTE->GetBuffer()->GetTextLength());
 //               fTE->GetBuffer()->AddText(0, text.Data());
                fTE->SetText(text.Data());
                gClient->NeedRedraw(fTE);
                break;
                }
             default:
                 break;
          }
          break;

       case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
             case kTE_TAB:
//               cout << "Tab " << endl;
                if (fCompList) {
                   TString temp = fTE->GetBuffer()->GetString();
//                   cout << "temp: " << temp << endl;
//                   char colon = ':';
                   Int_t indcol = IndexOfLastSpecial(temp);
                   TString var = temp(indcol+1, temp.Length() - indcol - 1);
//                   cout << "var: " << var << endl;
                   Int_t matchlength;
                   Int_t pos = Matches(fCompList, var.Data(), &matchlength);
                   if (pos >= 0) {
                      TObjString * obs = (TObjString *)fCompList->At(pos);
                      TString result = (obs->GetString()).Data();
                      if (matchlength != result.Length())result = result(0,matchlength);
                      temp.Replace(indcol + 1, temp.Length() - indcol - 1, result.Data());
                      fTE->SetText(temp.Data());
                   }
                }
                break;
             case kTE_ENTER:
                // here copy the string from text buffer to return variable
                strcpy(fRetStr, fTE->GetBuffer()->GetString());
                if (fFileName.Length() > 0) this->SaveList();
                if(fRetYesNo){
                   if(fCheckYesNo->GetState() == kButtonUp) *fRetYesNo = kFALSE;
                   else                                     *fRetYesNo = kTRUE;
                }
                if(fRetYesNo1){
                   if(fCheckYesNo1->GetState() == kButtonUp) *fRetYesNo1 = kFALSE;
                   else                                      *fRetYesNo1 = kTRUE;
                }
                *fReturn = 0;
                CloseWindow();
//                delete this;
                break;
             default:
                break;
          }
          break;

       default:
          break;
   }
   return kTRUE;
}

//_______________________________________________________________________________________

void TGMrbInputDialog::SaveList()
{
   if (fListBox == NULL) {
      fListBox = new TGListBox(this, 99);           
      fListBox->AddEntry(fTE->GetBuffer()->GetString(), 0);
      fWidgets->AddFirst(fListBox);
   }
   if (!(gSystem->AccessPathName(fFileName.Data()))) {
      TString bak(fFileName);
      bak += ".bak";
      gSystem->Rename(fFileName.Data(), bak.Data());
   }
   ofstream outfile(fFileName.Data());
   TString sel(fTE->GetBuffer()->GetString());
   Int_t ne = fListBox->GetNumberOfEntries();
//   cout << "Selected: " << sel << endl;
   outfile << sel << endl;
   if (ne > 0) {
      for (Int_t i =0; i < ne; i++) {
         TGLBEntry * tle = fListBox->GetEntry(i);
         TGTextLBEntry * tge = (TGTextLBEntry *)tle;
         TString text = tge->GetText()->GetString();
//         cout << "Ent: " << text.Data() << endl;
         if (text != sel) outfile <<text.Data() << endl;
      }
   }
   outfile.close();
}

//_______________________________________________________________________________________

const char *GetString(const char *Prompt, const char *DefVal, Bool_t * Ok,
                      TGWindow *Win,  
                      const char *YNPrompt, Bool_t * YesNo,
                      const char *HelpText, 
                      const char *YNPrompt1, Bool_t * YesNo1, 
                      const char *FileName, TList * Complist)
{
   // Prompt for string. The typed in string is returned.

   static char answer[128];
   Int_t ret;
   new TGMrbInputDialog(Prompt, DefVal, &ret, answer,Win, YNPrompt, YesNo, 
       HelpText, YNPrompt1, YesNo1, FileName, Complist);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }
   return answer;
}
//_______________________________________________________________________________________

Int_t GetInteger(const char *Prompt, Int_t DefVal, Bool_t * Ok,
                 TGWindow *Win,  const char *YNPrompt, Bool_t * YesNo,
                 const char *HelpText, const char *YNPrompt1, Bool_t * YesNo1)
{
   // Prompt for integer. The typed in integer is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%d", DefVal);

   Int_t ret;
   new TGMrbInputDialog(Prompt, defv, &ret, answer, Win, YNPrompt, YesNo, HelpText, YNPrompt1, YesNo1);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }

   return atoi(answer);
}
//_______________________________________________________________________________________

Float_t GetFloat(const char *Prompt, Float_t DefVal, Bool_t * Ok,
                 TGWindow *Win,  const char *YNPrompt, Bool_t * YesNo,
                 const char *HelpText, const char *YNPrompt1, Bool_t * YesNo1)
{
   // Prompt for float. The typed in float is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%f", DefVal);

   Int_t ret;
   new TGMrbInputDialog(Prompt, defv, &ret, answer, Win, YNPrompt, YesNo, HelpText, YNPrompt1, YesNo1);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }
   return atof(answer);
}

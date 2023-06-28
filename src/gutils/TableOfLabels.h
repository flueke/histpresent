#ifndef HPR_FHTABLE
#define HPR_FHTABLE
#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>

#include <TGWindow.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>

#include "TString.h"
#include "TGString.h"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TGLabel.h"
enum FhTableCommandIdentifiers {
   M_CANCEL,M_OK
};
//______________________________________________________________________

//class TableOfLabels : public TGTransientFrame {
class TableOfLabels : public TGMainFrame 
{
private:
   TList *fWidgets;
//   TGMainFrame            *fMainFrame;  // transient frame, main dialog window
   TGCompositeFrame       *fRowFrame;
   TGCompositeFrame       *fLabelFrame;
   TList                  *fEntries;
   TGCompositeFrame       *fFrame;
   TGLabel                *fItemLabel;
   TGLabel                *fLabel;
   TGLabel                *fRowLabel;
   TGLayoutHints          *fLO1, *fLO2, *fLO3, *fLO4;
   TOrdCollection         *fValues;
   TGTextButton           *fCancelButton;
   Int_t fNrows, fNcols, fNent;
public:
//   TableOfLabels(const TGWindow *p,const TGWindow *main, UInt_t w, UInt_t h, 
   TableOfLabels(const TGWindow *main,TString *title,
                 Int_t ncols, Int_t nrows,
                 TOrdCollection *entries,
                 TOrdCollection *col_lab,
                 TOrdCollection *row_lab, Int_t xpos = -1, Int_t ypos = -1);
   virtual ~TableOfLabels();
   virtual void CloseWindow();
   TList  *GetListOfLabels(){return fEntries;};
   void SetLabelText(Int_t col, Int_t row, const Char_t * text){
     ((TGLabel*)fEntries->At(fNcols*row + col))->SetText(new TGString(text));};
   void SetLabelColor(Int_t col, Int_t row, ULong_t color){
      TGLabel * la = (TGLabel*)fEntries->At(fNcols*row + col);
      la->ChangeBackground(color);
      la->SetText(new TGString(la->GetText()->GetString()));
  };
//  TGMainFrame * GetFrame(){return fMainFrame;};
//   virtual void CloseWindow();
   virtual Bool_t ProcessMessage (Long_t msg, Long_t parm1, Long_t);

ClassDef(TableOfLabels,0)		// [Utils] A table of labels

};

#endif

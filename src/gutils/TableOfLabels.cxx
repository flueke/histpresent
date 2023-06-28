namespace std {} using namespace std;

#include <iostream>
#include "TEnv.h"
#include "TableOfLabels.h"
ClassImp(TableOfLabels)
TableOfLabels::TableOfLabels(const TGWindow *win, TString *title,
                               Int_t nc, Int_t nr, 
                               TOrdCollection *values, 
                               TOrdCollection *col_labels,
                               TOrdCollection *row_labels,
                               Int_t xpos, Int_t ypos) :
                TGMainFrame(win, 10, 10){
   ULong_t grey, white, brown, antiquewhite1, antiquewhite2;
   gClient->GetColorByName("grey",   grey);
   gClient->GetColorByName("brown",   brown);
   gClient->GetColorByName("antiquewhite1",     antiquewhite1);
   gClient->GetColorByName("antiquewhite2", antiquewhite2);
   gClient->GetColorByName("white", white);

   const TGWindow *main = gClient->GetRoot();
   const TGWindow *mywin = main;
   if(win != 0)mywin=win;
//   fMainFrame = new TGMainFrame(main, 10, 10);
   fWidgets = new TList;
   fEntries = new TList;
   // command to be executed by buttons and text entry widget
//   char cmd[128];
 //  sprintf(cmd, 
 //  "{long r__ptr=0x%x; ((TableOfLabels*)r__ptr)->ProcessMessage($MSG,$PARM1,$PARM2);}", this);

   fNcols = nc; 
   fNrows = nr;
   fValues = values;
   if(fNrows < 0){
      cout << "Error: fNrows <0, set = 0 " << endl;
      fNrows = 0;
   }  
   if(fNcols < 0){
      cout << "Error: fNcols <0, set = 0 " << endl;
      fNcols = 0;
   }  
   if(fNrows == 0 && fNcols == 0){
      cout << "Error: fNrows <= 0 && fNcols <= 0, setting fNrows=1 " << endl;
      fNrows = 1;
      fNcols = 0;
   } 
   Int_t nvalues = values->GetSize();
   if(nvalues <= 0){
       cout << "Error: s <=0 " << endl;
       return;
   }
   if(fNrows != 0 && fNcols != 0 && fNrows*fNcols != nvalues){
      cout << "Warning: Not all " << nvalues<< " values will be shown " << endl;
   }
   if(fNrows != 0 && fNcols == 0) fNcols = nvalues/fNrows;
   if(fNcols != 0 && fNrows == 0) fNrows = nvalues/fNcols;

   fLO1 = new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2);
   fLO2 = new TGLayoutHints(kLHintsExpandX |kLHintsCenterX | kLHintsExpandY, 2, 2, 2, 2);
   fLO3 = new TGLayoutHints(kLHintsExpandX | kLHintsRight, 2, 2, 2, 2);
   fLO4 = new TGLayoutHints(kLHintsExpandY |kLHintsCenterY| kLHintsCenterX, 1, 1, 1, 1);
   fWidgets->AddFirst(fLO1);
   fWidgets->AddFirst(fLO2);
   fWidgets->AddFirst(fLO3);
   fWidgets->AddFirst(fLO4);

   TObjString *objs;
   TString s;
   Int_t rl =0;
   Int_t longest_line = 0;
   if(row_labels)rl=1;
   Int_t itemwidth;
   if(fNcols == 1) itemwidth = 240;
   else            itemwidth = 220;
   Int_t istart = 1;
   if(row_labels) istart = 0;
   if(col_labels){
      fRowFrame      = new TGCompositeFrame(this, itemwidth*(fNcols+rl), 20, kHorizontalFrame);  
      fWidgets->AddFirst(fRowFrame);
      for(Int_t i=istart; i <= fNcols; i++){
         fLabelFrame = new TGCompositeFrame(fRowFrame,itemwidth,20,kVerticalFrame | kFixedWidth |kRaisedFrame);
         fWidgets->AddFirst(fLabelFrame);
         if(i > 0){
            objs = (TObjString *)col_labels->At(i-1);
            s = objs->String();
            if (s.Length() > longest_line)
					longest_line = s.Length();
            fLabel    = new TGLabel(fLabelFrame, new TGString((const char *)s));
            fLabelFrame->AddFrame(fLabel, fLO4);
            fWidgets->AddFirst(fLabel);
            fLabel->ChangeBackground(white);
//            gVirtualX->ChangeWindowAttributes(fLabel->GetId(), &wattawhite1);
         }
         fRowFrame->AddFrame(fLabelFrame,fLO2);
      }
      this->AddFrame(fRowFrame,fLO1);                // frame into main frame
   }

   TIter next(values);
   for(Int_t j = 0; j < fNrows; j++){
      fRowFrame      = new TGCompositeFrame(this, itemwidth*(fNcols+rl), 20, kFixedWidth |kHorizontalFrame);  
      fWidgets->AddFirst(fRowFrame);
      if(row_labels){
         objs = (TObjString *)row_labels->At(j);
         s = objs->String();
			if (s.Length() > longest_line)
				longest_line = s.Length();
         fLabelFrame = new TGCompositeFrame(fRowFrame,itemwidth,20,kVerticalFrame |kRaisedFrame);
         fWidgets->AddFirst(fLabelFrame);
         fLabel    = new TGLabel(fLabelFrame, new TGString((const char *)s));
         fLabel->SetTextFont(gEnv->GetValue("Gui.MenuHiFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
         fLabelFrame->AddFrame(fLabel, fLO2);
         fWidgets->AddFirst(fLabel);
         fRowFrame->AddFrame(fLabelFrame,fLO2);
         fLabel->ChangeBackground(grey);
//         gVirtualX->ChangeWindowAttributes(fLabel->GetId(), &wattabrown);
      }
      for(Int_t i=0; i < fNcols; i++){
         objs = (TObjString*)next();
         s = objs->String();
			if (s.Length() > longest_line)
				longest_line = s.Length();
//         cout << s << endl;
         fLabelFrame = new TGCompositeFrame(fRowFrame,itemwidth-5,20,kFixedWidth |kVerticalFrame);
         fWidgets->AddFirst(fLabelFrame);
         fLabel    = new TGLabel(fLabelFrame, new TGString((const char *)s));
         fLabel->SetTextFont(gEnv->GetValue("Gui.MenuHiFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1"));
         fLabelFrame->AddFrame(fLabel, fLO2);
         fWidgets->AddFirst(fLabel);
         fEntries->Add(fLabel);
         fRowFrame->AddFrame(fLabelFrame,fLO2);
         fLabel->ChangeBackground(white);
//         gVirtualX->ChangeWindowAttributes(fLabel->GetId(), &wattawhite1);
      }
      this->AddFrame(fRowFrame,fLO1);                // frame into main frame
   }

//   fRowFrame      = new TGCompositeFrame(this, itemwidth*(fNcols+rl), 20, kHorizontalFrame);  
//   fWidgets->AddFirst(fRowFrame);
//   fActionFrame  = new TGCompositeFrame(fMainFrame, 100, 20, kHorizontalFrame);  
//   fCancelButton = new TGTextButton
//                         (fRowFrame, "Cancel", M_CANCEL);
//   fWidgets->AddFirst(fCancelButton);
//   fCancelButton->Resize(100,fCancelButton->GetDefaultHeight());
   
//   fRowFrame->AddFrame(fCancelButton,fLO1);
//   fCancelButton->SetCommand(cmd);
//   this->AddFrame(fRowFrame,fLO1);                // frame into main frame

   if(title)this->SetWindowName((const char *)*title);
   this->MapSubwindows();

   UInt_t width  = this->GetDefaultWidth();
//   UInt_t width  = itemwidth*(fNcols+rl) +30;
   UInt_t height = this->GetDefaultHeight();
//   cout << "Width " << width << " longest_line " << longest_line << endl;
//   cout << fLabel << " text: |" << fLabel->GetText() << endl;
   this->Resize(width+20, height);
   this->ChangeBackground(brown);

//   gVirtualX->ChangeWindowAttributes(fMainFrame->GetId(), &wattbrown);
   // position relative to the parent window (which is the root window)
   Window_t wdum;
   Int_t      ax, ay;
   if (xpos == -1) {
      gVirtualX->TranslateCoordinates(mywin->GetId(), this->GetParent()->GetId(),
         ((TGFrame *) mywin)->GetWidth() - width >> 1,
         ((TGFrame *) mywin)->GetHeight() - height >> 1,
                          ax, ay, wdum);
   } else {
      ax = xpos;
      ay = ypos;
   }
   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);
   this->MapWindow();
//   this->Resize(width, height);
//   gClient->NeedRedraw(this);
//   gClient->WaitFor(fMainFrame);
};

//_______________________________________________________________________

void TableOfLabels::CloseWindow()
{
//   cout << "TableOfLabels::CloseWindow()" << endl;
   DeleteWindow();
   TQObject::Destroyed();  
}
//_______________________________________________________________________

TableOfLabels::~TableOfLabels(){
//   cout << "dtor ~TableOfLabels" << endl;
   fEntries->Clear();
   fWidgets->Delete();
   delete fWidgets;
   delete fEntries;

};

Bool_t TableOfLabels::ProcessMessage (Long_t msg, Long_t parm1, Long_t){
  switch(GET_MSG(msg)) {
      case kC_COMMAND:
         switch(GET_SUBMSG(msg)) {
           case kCM_BUTTON:
               switch(parm1) {
                  case M_CANCEL:
                     CloseWindow();
//                     delete this;
                   break;
               }
               break;

            case kCM_RADIOBUTTON:
            case kCM_CHECKBUTTON:
               break;
            case kCM_COMBOBOX:
               break;

            default:
               break;
         }
         break;

      default:
         break;
   }
   return kTRUE;
};

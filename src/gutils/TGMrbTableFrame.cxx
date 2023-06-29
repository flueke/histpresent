/*__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:            gutils/src/TGMrbTableFrame.cxx
// Purpose:        A matrix of N * M text entries + check buttons
// Description:    
// Author:         O. Schaile
// Mailto:         <a href=mailto:otto.schaile@physik.uni-muenchen.de>O. Schaile</a>
// Revision:       
// Date:           
// URL:            
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TObjString.h"
#include "TVirtualX.h"
#ifdef MARABOUVERS
#include "TMrbLogger.h"
#endif
#include "TGMrbTableFrame.h"
#include "TRootHelpDialog.h"

#include "SetColor.h"

//ClassImp(TGMrbTableFrame)
//ClassImp(TGMrbTableOfInts)
//ClassImp(TGMrbTableOfDoubles)

#ifdef MARABOUVERS
extern TMrbLogger * gMrbLog;			// access to message logging
#endif
TGMrbTableFrame::TGMrbTableFrame(const TGWindow * Window, Int_t * RetValue, const Char_t * Title,
													Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
													TOrdCollection * Values, 
													TOrdCollection * ColumnLabels,
													TOrdCollection * RowLabels,
													TArrayI * Flags, 
													Int_t Nradio, 
													const Char_t * HelpText, 
													const Char_t * ActionText_1,  
													const Char_t * ActionText_2) :
											TGTransientFrame(gClient->GetRoot(), Window, 10, 10) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableFrame
// Purpose:        Provide a matrix of text entries
// Arguments:      TGWindow * Window               -- parent window
//                 Int_t * RetValue                -- return value
//                                                 -- on entry: < 0 no edit allowed
//                 Char_t * Title                  -- title
//                 Int_t ItemWidth                 -- item width (in pixels 120-240)
//                 Int_t Ncols                     -- number of columns
//                 Int_t Nrows                     -- number of Rows
//                 TOrdCollection * Values         -- collection of values (strings)
//                 TOrdCollection * ColumnLabels   -- column headings
//                 TOrdCollection * RowLabels      -- row labels
//                 TArrayI * Flags                 -- checkbutton flags
//                                                 -- if number of Flags > 2 * Nrows  show 2 columns 
//                 Int_t Nradio                    -- first Nradio flags are RadioButtons
//                                                 -- if < 0 TGColorSelect
//                                                 -- if 2 Flag columns 2nd group of flags triggers
//                                                 --  0: TGColorSelect, 1 FillStyle, 2 LineStyle, 3: Markerstyle
//                 Char_t *HelpText                -- display a help button 
//                 Char_t *ActionText_1            -- replace text in Ok Button 
//                 Char_t *ActionText_2            -- put extra Button, RetValue = 1 
// Exceptions:     
// Description:    A matrix of Ncols * Nrows text entries together with checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#ifdef MARABOUVERS
//	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
#endif	
	fWidgets = new TList;
	fEntries = new TList;

   ULong_t brown, lgrey, linen, green, blue, yellow;
   gClient->GetColorByName("firebrick", brown);
   gClient->GetColorByName("snow", lgrey);
   gClient->GetColorByName("linen", linen);
   gClient->GetColorByName("green", green);
   gClient->GetColorByName("blue", blue);
   gClient->GetColorByName("yellow", yellow);

	if(Flags)	fFlags = new TList;
	else		fFlags = NULL;
	fFlagsReturn = Flags;
//  Otto 20/11/00
   fHelpText = HelpText;
   TString t_ok;
   if(ActionText_1) t_ok = ActionText_1;
   else 			t_ok = "Ok";

   fNcols = Ncols; 
   fNrows = Nrows;
   fRet = RetValue;
   fValues = Values;
   if (Nradio >= 0) {
      fRadio = Nradio;
      fColorSelect = 0;
   } else {
      fRadio = 0;
      fColorSelect = TMath::Abs(Nradio);
   }
//   fCheck = fNrows - fRadio;
	fCheck = 0;
		
#ifdef MARABOUVERS
   if(fNrows < 0) {
      gMrbLog->Err() << "Number of rows < 0, set to 0" << endl;
	  gMrbLog->Flush("TGMrbTableFrame");
      fNrows = 0;
   }  
   if(fNcols < 0) {
      gMrbLog->Err() << "Number of columns < 0, set to 0" << endl;
	  gMrbLog->Flush("TGMrbTableFrame");
      fNcols = 0;
   }  
   if(fNrows == 0 && fNcols == 0) {
      gMrbLog->Err() << "Number of rows and columns, setting fNrows=1" << endl;
	  gMrbLog->Flush("TGMrbTableFrame");
      fNrows = 1;
      fNcols = 0;
   } 
   Int_t nvalues = Values->GetSize();
   if(nvalues <= 0) {
      gMrbLog->Err() << "Value array has size <= 0" << endl;
	  gMrbLog->Flush("TGMrbTableFrame");
      return;
   }
   if(fNrows != 0 && fNcols != 0 && fNrows*fNcols != nvalues) {
      gMrbLog->Err() << "WARNING: not all values will be shown" << endl;
	  gMrbLog->Flush("TGMrbTableFrame");
   }
#else
   if(fNrows < 0) {
      cout << "Number of rows < 0, set to 0" << endl;
      fNrows = 0;
   }  
   if(fNcols < 0) {
      cout << "Number of columns < 0, set to 0" << endl;
      fNcols = 0;
   }  
   if(fNrows == 0 && fNcols == 0) {
      cout << "Number of rows and columns, setting fNrows=1" << endl;
      fNrows = 1;
      fNcols = 0;
   } 
   Int_t nvalues = Values->GetSize();
   if(nvalues <= 0) {
      cout << "Value array has size <= 0" << endl;
      return;
   }
   if(fNrows != 0 && fNcols != 0 && fNrows*fNcols != nvalues) {
      cout << "WARNING: not all values will be shown" << endl;
   }
#endif

   if(fNrows != 0 && fNcols == 0) fNcols = nvalues/fNrows;
   if(fNcols != 0 && fNrows == 0) fNrows = nvalues/fNcols;

//   cout << Ncols  << " " << Nrows  << " "<< Nradio  << " " << nvalues << endl;

   TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX , 2, 2, 2, 2);
   fWidgets->AddFirst(lo1);
   TGLayoutHints * lo2 = new TGLayoutHints(kLHintsExpandY | kLHintsLeft, 2, 2, 2, 2);
   fWidgets->AddFirst(lo2);
   TGLayoutHints * lo3 = new TGLayoutHints(kLHintsExpandX | kLHintsRight, 2, 2, 2, 2);
   fWidgets->AddFirst(lo3);
   TGLayoutHints * lo4 = new TGLayoutHints(kLHintsExpandY | kLHintsCenterY | kLHintsCenterX, 2, 2, 2, 2);
   fWidgets->AddFirst(lo4);
   TGLayoutHints * lo5 = new TGLayoutHints(kLHintsExpandY | kLHintsCenterY | kLHintsCenterX, 1, 1, 1, 1);
   fWidgets->AddFirst(lo5);
/*
     --------------------------------------------------
     |          fColFrame                              |
     | ----- -|-----------------------------------------
     | fRow   |                               |  opt   |
     | Frame  |   fTableFrame                 | check  |
     |        |                               | buttons|
     |        |                               |        |
     ---------------------------------------------------
*/
   Int_t rl =0;
   if (RowLabels) rl = 1;
   Int_t itemwidth;
   Int_t fl1 = 0;
   Int_t fl2 = 0;
   Int_t flmod = 0;
   if (Flags) {
      fl1 = 1;
      if(Flags->GetSize() < fNrows) {
#ifdef MARABOUVERS
		gMrbLog->Err() << "Not enough flags in checkbutton array" << endl;
	  	gMrbLog->Flush("TGMrbTableFrame");
#else
      cout << "Not enough flags in checkbutton array" << endl;
#endif
         fl1 = 0;
      }
	   if(Flags->GetSize() >= 2*fNrows) {
         if (fColorSelect == 0) fl2 = 1;
         else                   flmod = 1;
      }
   }
   if (ItemWidth > 0) {
		itemwidth = ItemWidth;
   } else {
		if(fNcols == 1) itemwidth = 240;
		else            itemwidth = 120;
   }
   fColFrame      = new TGCompositeFrame(this, (Int_t) (itemwidth*(fNcols+rl+.5*(fl1+fl2))), 20, kHorizontalFrame);  
   fColFrame->ChangeBackground(brown);
   fWidgets->AddFirst(fColFrame);
   fTableRowFrame = new TGCompositeFrame(this, (Int_t) (itemwidth*(fNcols+rl+.5*(fl1+fl2))), fNrows*20, kHorizontalFrame);  
   fTableRowFrame->ChangeBackground(brown);
   fWidgets->AddFirst(fTableRowFrame);

	if (RowLabels) {
		fRowFrame = new TGCompositeFrame(fTableRowFrame, itemwidth, fNrows*20, kVerticalFrame| kFixedWidth);  
		fWidgets->AddFirst(fRowFrame);
	}

	fTableFrame = new TGCompositeFrame(fTableRowFrame, fNcols*itemwidth, fNrows*20,kHorizontalFrame| kFixedWidth);  

   fTableFrame->ChangeBackground(brown);
	if (fl1 > 0) {
		fFlagFrame1  = new TGCompositeFrame(fTableRowFrame, (Int_t) (0.5*itemwidth), fNrows*20, kVerticalFrame| kFixedWidth);  
//      fFlagFrame1->ChangeBackground(green);
		fWidgets->AddFirst(fFlagFrame1);
	}

	if (fl2 > 0) {
		fFlagFrame2  = new TGCompositeFrame(fTableRowFrame, (Int_t) (0.5*itemwidth), fNrows*20, kVerticalFrame| kFixedWidth);  
		fWidgets->AddFirst(fFlagFrame2);
	}

   fLMMatrix = new TGMatrixLayout(fTableFrame, fNrows, 0);
   fWidgets->AddFirst(fLMMatrix);
   fLMMatrix->fSep = 2;
   fTableFrame->SetLayoutManager(fLMMatrix);

//   cout << "fNrows " << fNrows<< endl;
   Int_t ind = 0;
   TIter next(Values);
   TObjString * objs;
   TString s;
   while((objs = (TObjString *) next())) {
      s = objs->String();
      if (s.BeginsWith("CheckButton")) {
         fFlagButton = new TGCheckButton(fTableFrame, new TGHotString(""), ind);
         if (s.EndsWith("Down")) fFlagButton->SetState(kButtonDown);
         else                    fFlagButton->SetState(kButtonUp);
         fFlagButton->Resize(itemwidth, fFlagButton->GetDefaultHeight());
         fWidgets->Add(fFlagButton);
         fEntries->Add(fFlagButton);
         fTableFrame->AddFrame(fFlagButton, lo4);
      } else {
         if (*RetValue >= 0) {
            fTEItem = new TGTextEntry(fTableFrame, fTBItem = new TGTextBuffer(100));
            fWidgets->Add(fTEItem);
            fEntries->Add(fTEItem);
            fTBItem->AddText(0, (const char  *) s);
//            cout << "s: " << (const char  *) s << endl;
            fTEItem->Resize(itemwidth, fTEItem->GetDefaultHeight());
            fTableFrame->AddFrame(fTEItem, lo1);
         } else {
            fColLabFrame = new TGCompositeFrame(fTableFrame,itemwidth,20,kVerticalFrame | kFixedWidth |kRaisedFrame);
		      fWidgets->Add(fColLabFrame);
            fColLabel = new TGLabel(fColLabFrame, new TGString((const char *) s));
            fColLabFrame->AddFrame(fColLabel, lo4);
            fTableFrame->AddFrame(fColLabFrame, lo1);
			   fWidgets->Add(fColLabel);
         }
      }
      ind++;
   }

   if (ColumnLabels) {
      Int_t ioff;                       // if rowlabels defined skip first field
      if (RowLabels == NULL) ioff = 0;
      else                   ioff = -1;
      for(Int_t i=0; i < fNcols - ioff; i++) {
         fColLabFrame = new TGCompositeFrame(fColFrame,itemwidth,20,kVerticalFrame | kFixedWidth |kRaisedFrame);
		   fWidgets->Add(fColLabFrame);
         if(i > 0 || (i == 0 && RowLabels == NULL)) {
            objs = (TObjString *) ColumnLabels->At(i+ioff);
            s = objs->String();
            fColLabel    = new TGLabel(fColLabFrame, new TGString((const char *) s));
			   fWidgets->Add(fColLabel);
            fColLabFrame->AddFrame(fColLabel, lo4);
         }
         fColFrame->AddFrame(fColLabFrame,lo2);
      }
      if(fl1 > 0) {
         fColLabFrame = new TGCompositeFrame(fColFrame,(Int_t) (0.5*itemwidth),20,kVerticalFrame | kFixedWidth |kRaisedFrame);
	   	fWidgets->Add(fColLabFrame);
         if (ColumnLabels->GetSize() > fNcols) {
            objs = (TObjString *) ColumnLabels->At(fNcols);
            s = objs->String();
         } else s = "Flags";
         fColLabel    = new TGLabel(fColLabFrame, new TGString((const char *) s));
		   fWidgets->Add(fColLabel);
         fColLabFrame->AddFrame(fColLabel, lo2);
         fColFrame->AddFrame(fColLabFrame,lo2);
      }
      if(fl2 > 0) {
         fColLabFrame = new TGCompositeFrame(fColFrame, (Int_t) (0.5*itemwidth),20,kVerticalFrame | kFixedWidth |kRaisedFrame);
		   fWidgets->Add(fColLabFrame);
         if(ColumnLabels->GetSize() > fNcols+1) {
            objs = (TObjString *) ColumnLabels->At(fNcols+1);
            s = objs->String();
         } else {
            s = "Flags";
         }
         fColLabel    = new TGLabel(fColLabFrame, new TGString((const char *) s));
	      fWidgets->Add(fColLabel);
         fColLabFrame->AddFrame(fColLabel, lo2);
         fColFrame->AddFrame(fColLabFrame,lo2);
      }
   }
   if (RowLabels) {
      for (Int_t i = 0; i < fNrows; i++) {
         fRowLabFrame = new TGCompositeFrame(fRowFrame,itemwidth,20,kHorizontalFrame);
		   fWidgets->Add(fRowLabFrame);
         objs = (TObjString *) RowLabels->At(i);
         s = objs->String();
         fRowLabel    = new TGLabel(fRowLabFrame, new TGString((const char *) s));
			fWidgets->Add(fRowLabel);
         fRowLabFrame->AddFrame(fRowLabel, lo1);
         fRowFrame->AddFrame(fRowLabFrame,lo1);
      }
   }
   if (fl1 > 0) {
      for(Int_t i=0; i < fNrows; i++) {
         fFlagButton = 0;
         if (fColorSelect > 0) {
            if (flmod == 0 || (flmod != 0 && (*Flags)[i + fNrows] == kFlagColor)) {
               fFlagButton = new TGColorSelect(fFlagFrame1, (*Flags)[i], i);
            } else if (flmod != 0) {
               if ((*Flags)[i + fNrows] == kFlagFillStyle) 
                  fFlagButton = new TGPictureButton(fFlagFrame1,fClient->GetPicture("selection_t.xpm"), i);
               else if ((*Flags)[i + fNrows] == kFlagLineStyle) 
                  fFlagButton = new TGPictureButton(fFlagFrame1,fClient->GetPicture("selection_t.xpm"), i);
               else if ((*Flags)[i + fNrows] == kFlagFillStyle) 
                  fFlagButton = new TGPictureButton(fFlagFrame1,fClient->GetPicture("selection_t.xpm"), i);
               else  
                  fFlagFrame1->AddFrame(new TGLabel(fFlagFrame1,new TGString("noop")),lo1); 
            } else {
               fFlagFrame1->AddFrame(new TGLabel(fFlagFrame1,new TGString("noop")),lo1); 
            }
         } else  {
            if (fRadio > 0 && i < fRadio)	fFlagButton = new TGRadioButton(fFlagFrame1, new TGHotString(""), i);
            else							      fFlagButton = new TGCheckButton(fFlagFrame1, new TGHotString(""), i);
         }
         if (fFlagButton) {
            fWidgets->AddFirst(fFlagButton);
            if (fColorSelect > 0)fFlagFrame1->AddFrame(fFlagButton,lo5); 
            else                 fFlagFrame1->AddFrame(fFlagButton,lo4);
            fFlags->Add(fFlagButton);

            if (fColorSelect <= 0 && (*Flags)[i]) fFlagButton->SetState(kButtonDown);
            fFlagButton->Associate(this);
         }
      }
      if (fCheck > 0) {
         fBtnAll1 = new TGPictureButton(fFlagFrame1, gClient->GetPicture("cbutton_all.xpm"), kTableFrameAll_1);
         fWidgets->AddFirst(fBtnAll1);
         fFlagFrame1->AddFrame(fBtnAll1,lo4);
         fBtnAll1->Associate(this);
         fBtnNone1 = new TGPictureButton(fFlagFrame1, gClient->GetPicture("cbutton_none.xpm"), kTableFrameNone_1);
         fWidgets->AddFirst(fBtnNone1);
         fFlagFrame1->AddFrame(fBtnNone1,lo4);
         fBtnNone1->Associate(this);
	  }
   }
 
   if (fl2 > 0) {
      for(Int_t i=0; i < fNrows; i++) {
         if (fColorSelect > 0)               fFlagButton = new TGColorSelect(fFlagFrame1, (*Flags)[i], i+fNrows);
         else if(fRadio >= fNrows && i < fRadio-fNrows)	fFlagButton = new TGRadioButton(fFlagFrame2, new TGHotString(""), i+fNrows);
         else										fFlagButton = new TGCheckButton(fFlagFrame2, new TGHotString(""), i+fNrows);
         fWidgets->AddFirst(fFlagButton);
         fFlagFrame2->AddFrame(fFlagButton,lo4);
         fFlags->Add(fFlagButton);
         if (fColorSelect <= 0 && (*Flags)[i+fNrows]) fFlagButton->SetState(kButtonDown);
         if (fColorSelect <= 0) fFlagButton->Associate(this);
      }
      if (fCheck > 0) {
         fBtnAll2 = new TGPictureButton(fFlagFrame2, gClient->GetPicture("cbutton_all.xpm"), kTableFrameAll_2);
         fWidgets->AddFirst(fBtnAll2);
         fFlagFrame2->AddFrame(fBtnAll2,lo4);
         fBtnAll2->Associate(this);
         fBtnNone2 = new TGPictureButton(fFlagFrame2, gClient->GetPicture("cbutton_none.xpm"), kTableFrameNone_2);
         fWidgets->AddFirst(fBtnNone2);
         fFlagFrame2->AddFrame(fBtnNone2,lo4);
         fBtnNone2->Associate(this);
      }
   }

   fActionFrame  = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);  
   fWidgets->AddFirst(fActionFrame);
   fCancelButton = new TGTextButton(fActionFrame, "Cancel" , TGMrbTableFrame::kTableFrameCancel);
   fCancelButton->Associate(this);
   fWidgets->AddFirst(fCancelButton);
   fOkButton     = new TGTextButton(fActionFrame, t_ok.Data(), TGMrbTableFrame::kTableFrameOk);
   fWidgets->AddFirst(fOkButton);
   fOkButton->Associate(this);
   if(ActionText_2){
      fAct_2Button = new TGTextButton(fActionFrame, ActionText_2, TGMrbTableFrame::kTableFrameAct_2);
      fWidgets->AddFirst(fAct_2Button);
      fAct_2Button->Associate(this);
      fAct_2Button->ChangeBackground(linen);
   }
   if(fHelpText){
      fHelpButton = new TGTextButton(fActionFrame, "Help", TGMrbTableFrame::kTableFrameHelp);
      fWidgets->AddFirst(fHelpButton);
      fHelpButton->Associate(this);
      fHelpButton->ChangeBackground(linen);
   }
   fCancelButton->ChangeBackground(linen);
   fOkButton->ChangeBackground(linen);

   fCancelButton->Resize(100,fCancelButton->GetDefaultHeight());
   fOkButton->Resize(100,fOkButton->GetDefaultHeight());
   if(fHelpText)fHelpButton->Resize(100,fHelpButton->GetDefaultHeight());
   if(ActionText_2)fAct_2Button->Resize(100,fAct_2Button->GetDefaultHeight());
   fActionFrame->AddFrame(fCancelButton,lo1);
   fActionFrame->AddFrame(fOkButton,lo1);

   if(ActionText_2)fActionFrame->AddFrame(fAct_2Button,lo1); 
   if(fHelpText)fActionFrame->AddFrame(fHelpButton,lo1); 

   if (RowLabels) {
      fRowFrame->Resize(itemwidth,fRowFrame->GetDefaultHeight());
      fTableRowFrame->AddFrame(fRowFrame,lo2);
   }
   fTableFrame->Resize(fNcols*itemwidth,fTableFrame->GetDefaultHeight());
   fTableRowFrame->AddFrame(fTableFrame,lo2);

   if (fl1 > 0) {
      fFlagFrame1->Resize((Int_t)0.5 * itemwidth,fFlagFrame1->GetDefaultHeight());
      fTableRowFrame->AddFrame(fFlagFrame1,lo2);
   }
   if (fl2 > 0) {
      fFlagFrame2->Resize((Int_t)0.5 * itemwidth,fFlagFrame2->GetDefaultHeight());
      fTableRowFrame->AddFrame(fFlagFrame2,lo2);
   }
   if ( *fRet == 2 )
		this->AddFrame(fActionFrame,lo1);                // frame into main frame
   this->AddFrame(fColFrame,lo1);                // frame into main frame
   this->AddFrame(fTableRowFrame,lo1);                // frame into main frame
   if ( *fRet != 2 )
		this->AddFrame(fActionFrame,lo1);                // frame into main frame
	
   if (Title) this->SetWindowName(Title);
   this->MapSubwindows();

   Int_t width  = (Int_t) (itemwidth*(fNcols+rl+0.5*(fl1+fl2)) +30);
   Int_t height = this->GetDefaultHeight();
   this->Resize(width, height);

// position relative to the parent's window
// avoid going off screen

   if(Window){
   	Window_t wdum;
   	Int_t      ax, ay;
//   	gVirtualX->TranslateCoordinates(Window->GetId(), this->GetParent()->GetId(),
//         	((TGFrame *) Window)->GetWidth() - width >> 1,
//         	((TGFrame *) Window)->GetHeight(),
//                       	  ax, ay, wdum);
//      cout << "height, ay raw" << height<< " " << ay << endl; 
   	gVirtualX->TranslateCoordinates(Window->GetId(), this->GetParent()->GetId(),
         	((TGFrame *) Window)->GetWidth() - width >> 1,
         	((TGFrame *) Window)->GetHeight() - height,
                        	  ax, ay, wdum);
   	Int_t  screen_x, screen_y;
   	UInt_t wwu, whu;
   	Int_t ww, wh;

	//  get screen size in pixels

   	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
            	  screen_x, screen_y, wwu, whu);
   	ww = (Int_t)wwu;
   	wh = (Int_t)whu;
 //     cout << "height, ay, wh " << height<< " " << ay << " " << wh<< endl; 
   	if(ax < 0) ax = 5;
   	Int_t ovl = ax + width - ww;
   	if(ovl > 0) ax -= ovl;
      ay += height;
   	if(ay < 0) ay = 5;
      ovl = ay +  height + 30 - wh;
      if(ovl > 0) ay -= ovl;

      this->Move(ax, ay);
      this->SetWMPosition(ax, ay);
   }
   this->MapWindow();
   this->ChangeBackground(brown);
   gClient->WaitFor(this);
};

//_____________________________________________________________________________
TGMrbTableFrame::~TGMrbTableFrame()		
{
   fEntries->Clear();
	fWidgets->Delete();
	delete fWidgets;
	delete fEntries;
};
//_____________________________________________________________________________
void TGMrbTableFrame::CloseWindow()
{
   DeleteWindow();
}
//_____________________________________________________________________________

void TGMrbTableFrame::StoreValues(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableFrame::StoreValues()
// Purpose:        Store return values
// Arguments:  
//             
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
   TIter nextent(fEntries);
   TIter next(fValues);
   TObjString *objs;
   Int_t i=0;
   if (*fRet >= 0) {
   	while((objs = (TObjString*)next())){
      	fTEItem=(TGTextEntry*)nextent();
      	if (fTEItem->InheritsFrom("TGCheckButton")) {
         	fFlagButton = (TGCheckButton*)fTEItem;
         	if (fFlagButton->GetState() == kButtonDown)
            	objs->SetString("CheckButton_Down"); 
         	else
            	objs->SetString("CheckButton_Up");
      	} else { 
         	const char * te = fTEItem->GetBuffer()->GetString();
         	objs->SetString((char *)te);
      	} 
      	i++; 
   	}
   }
//   cout << "StoreValues(), fColorSelect " << fColorSelect << endl;
   if(fFlags){
      TIter nflag(fFlags);
      if (fColorSelect <= 0) {
      	TGCheckButton *chkb;
      	Int_t j = 0;
      	while( (chkb = (TGCheckButton*)nflag()) ) {
      	  (*fFlagsReturn)[j] = (chkb->GetState() == kButtonDown) ? 1 : 0;
      	  j++;
      	}
      } else {
      	TGColorSelect *cols;
      	Int_t j = 0;
      	while( (cols = (TGColorSelect*)nflag()) ) {
 //          cout << cols->GetColor() << endl;
      	  (*fFlagsReturn)[j] = cols->GetColor();
      	  j++;
      	}
      }
   }
}

Bool_t TGMrbTableFrame::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t /*Param2*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableFrame::ProcessMessage
// Purpose:        Handle X events
// Arguments:      Long_t MsgId    -- message id
//                 Long_t ParamX   -- params
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   switch(GET_MSG(MsgId)) {
      case kC_COMMAND:
         switch(GET_SUBMSG(MsgId)) {
           case kCM_BUTTON:
               switch(Param1) {
                  case kTableFrameCancel:
                     *fRet = -1;
                     CloseWindow();
//                     delete this;
                     break;
                  case kTableFrameOk:
                     StoreValues();
                     *fRet = 0;
                     CloseWindow();
//                     delete this;
                     break;
                  case kTableFrameAct_2:
                     StoreValues();
                     *fRet = 1;
                     CloseWindow();
//                     delete this;
                     break;
                  case kTableFrameHelp:
                     {
                     TString temp(fHelpText);
                     Int_t nl = temp.CountChar('\n');
                     nl *= 15;
                     TRootHelpDialog * hd = new TRootHelpDialog(this, "HelpDialog", 450, nl+20);
                     hd->SetText(fHelpText);
                     hd->Popup();
                     break;
                    }
                  case kTableFrameAll_1:
					{
						TGCheckButton *chkb;
						Int_t n = (fRadio > 0) ? fRadio : fNrows;
						for (Int_t i=0; i < n; i++){
                     		chkb = (TGCheckButton *)fFlags->At(i);
                     		chkb->SetState(kButtonDown);
                  		}
					}
					break;
                  case kTableFrameNone_1:
					{
						TGCheckButton *chkb;
						Int_t n = (fRadio > 0) ? fRadio : fNrows;
						for (Int_t i=0; i < n; i++){
                     		chkb = (TGCheckButton *)fFlags->At(i);
                     		chkb->SetState(kButtonUp);
                  		}
					}
					break;
                  case kTableFrameAll_2:
					{
						TGCheckButton *chkb;
						Int_t n = (fRadio > 0) ? fRadio : fNrows;
						for (Int_t i=0; i < n; i++){
                     		chkb = (TGCheckButton *)fFlags->At(i + fNrows);
                     		chkb->SetState(kButtonDown);
                  		}
					}
					break;
                  case kTableFrameNone_2:
					{
						TGCheckButton *chkb;
						Int_t n = (fRadio > 0) ? fRadio : fNrows;
						for (Int_t i=0; i < n; i++){
                     		chkb = (TGCheckButton *)fFlags->At(i + fNrows);
                     		chkb->SetState(kButtonUp);
                  		}
					}
					break;
				}
				break;

            case kCM_RADIOBUTTON:
            case kCM_CHECKBUTTON:
               if(fRadio > 0){
                  TGCheckButton *chkb; 
                  TIter nflag(fFlags);
                  Int_t button_down = -1;
                  Int_t button_pressed = -1;
                  for (Int_t i=0; i < fRadio; i++){
                     chkb = (TGCheckButton *)fFlags->At(i);
                     if     (Param1 == chkb->WidgetId())      button_pressed=i;
                     else if(chkb->GetState() == kButtonDown)button_down=i;
                  }
                  if(button_down >= 0 && button_pressed>= 0)
                      ((TGCheckButton *)fFlags->At(button_down))->SetState(kButtonUp);
               }
               break;

            default:
               break;
         }
      case kC_COLORSEL:
 //        cout << "C_COLORSEL: " << Param1 << " " << Param2 << endl;   
      break;

      default:
         break;
   }
	return(kTRUE);
};

TGMrbTableOfInts::TGMrbTableOfInts(const TGWindow * Window, Int_t * RetValue, const Char_t * Title,
													Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
													TArrayI & Values, Int_t /*Base*/,
													TOrdCollection * ColumnLabels,
													TOrdCollection * RowLabels,
													TArrayI * Flags, 
                                       Int_t Nradio, 
                                       const Char_t *HelpText, 
                                       const Char_t *ActionText_1,  
                                       const Char_t *ActionText_2) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableOfInts
// Purpose:        Provide a matrix of integer entries
// Arguments:      TGWindow * Window               -- parent window
//                 Int_t * RetValue                -- return value -1 cancel, 0 ok, 1 Action_2
//                 Char_t * Title                  -- title
//                 Int_t ItemWidth                 -- item width (pixels: 120 - 240)
//                 Int_t Ncols                     -- number of columns
//                 Int_t Nrows                     -- number of Rows
//                 TArrayI & Values                -- integer array of values
//                 Int_t Base                      -- numerical base (2, 8, 10, 16)
//                 TOrdCollection * ColumnLabels   -- column headings
//                 TOrdCollection * RowLabels      -- row labels
//                 TArrayI * Flags                 -- checkbutton flags
//                 Int_t Nradio                    -- first Nradio flags are RadioButtons
//                 Char_t *HelpText                -- display a help button 
//                 Char_t *ActionText_1            -- replace text in Ok Button 
//                 Char_t *ActionText_2            -- put extra Button, RetValue = 1 
// Exceptions:     
// Description:    A matrix of Ncols * Nrows integer entries together with checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TObjString * os;

	TOrdCollection * entries = new TOrdCollection();		// TGMrbTableFrame needs a collection of strings

	Int_t idx = 0;
	for (Int_t col = 0; col < Ncols; col++) {
		for (Int_t row = 0; row < Nrows; row++) {
			os = new TObjString(Form("%d", Values[idx]));	// wrap TObject around it
			entries->Add((TObject *) os);					   // add to collection of strings
			idx++;
		}
	}

	// pop up the dialog window now
	new TGMrbTableFrame(Window, RetValue, Title, ItemWidth, Ncols, Nrows, 
                       entries, ColumnLabels, RowLabels, Flags,
                       Nradio, HelpText, ActionText_1, ActionText_2);

	idx = 0;												// reverse operation
	for (Int_t col = 0; col < Ncols; col++) {
		for (Int_t row = 0; row < Nrows; row++) {
			os = (TObjString *) entries->At(idx);			// get (modified) string
         Values[idx] = atoi(os->GetString().Data());
			idx++;
		}
	}
	entries->Delete();										// remove temp alloc objects
}

TGMrbTableOfDoubles::TGMrbTableOfDoubles(const TGWindow * Window, Int_t * RetValue, const Char_t * Title,
													Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
													TArrayD & Values, Int_t /*Precision*/,
													TOrdCollection * ColumnLabels,
													TOrdCollection * RowLabels,
													TArrayI * Flags, 
                                       Int_t Nradio, 
                                       const Char_t *HelpText, 
                                       const Char_t *ActionText_1,  
                                       const Char_t *ActionText_2) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableOfDoubles
// Purpose:        Provide a matrix of double entries
// Arguments:      TGWindow * Window               -- parent window
//                 Int_t * RetValue                -- return value (button id)
//                 Char_t * Title                  -- title
//                 Int_t ItemWidth                 -- item width
//                 Int_t Ncols                     -- number of columns
//                 Int_t Nrows                     -- number of Rows
//                 TArrayD & Values                -- integer array of values
//                 Int_t Precision                 -- floating precision
//                 TOrdCollection * ColumnLabels   -- column headings
//                 TOrdCollection * RowLabels      -- row labels
//                 TArrayI * Flags                 -- checkbutton flags
//                 Int_t Nradio                    -- first Nradio flags are RadioButtons
//                 Char_t *HelpText                -- display a help button 
//                 Char_t *ActionText_1            -- replace text in Ok Button 
//                 Char_t *ActionText_2            -- put extra Button, RetValue = 1 
// Exceptions:     
// Description:    A matrix of Ncols * Nrows double entries together with checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TObjString * os;

	TOrdCollection * entries = new TOrdCollection();	// TGMrbTableFrame needs a collection of strings
	Int_t idx = 0;
	for (Int_t col = 0; col < Ncols; col++) {
		for (Int_t row = 0; row < Nrows; row++) {
			os = new TObjString(Form("%g", Values[idx]));				// wrap TObject around it
			entries->Add((TObject *) os);				// add to collection of strings
			idx++;
		}
	}

	// pop up the dialog window now
	new TGMrbTableFrame(Window, RetValue, Title, ItemWidth, Ncols, Nrows, 
                       entries, ColumnLabels, RowLabels, Flags,
                       Nradio, HelpText, ActionText_1, ActionText_2);

	idx = 0;											// reverse operation
	for (Int_t col = 0; col < Ncols; col++) {
		for (Int_t row = 0; row < Nrows; row++) {
			os = (TObjString *) entries->At(idx);		// get (modified) string
         Values[idx] = atof(os->GetString().Data());
			idx++;
		}
	}
	entries->Delete();									// remove temp alloc objects
}

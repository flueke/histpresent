namespace std {} using namespace std;

#include <iostream>

#include <TROOT.h>
#include <TVirtualX.h>

#include <TGClient.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TObjString.h>
#include "TOrdCollection.h"
#include <stdlib.h>
// #include "MyTimer.h"
#include "TGMrbSliders.h"

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget providing n sliders.                                    //
//                                                                       //
// After call the routine returns immediatly keeping up the widget       //

// when a slider changes its value but at a maximum repitition rate      //
// of 20 Hz. This allows the application to update canvases in a smooth  // 
// manner.                                                               //
//                                                                       //
// Usage:                                                                //
//                                                                       //
// somewhere in calling class: ColorPlayer                                       //
//                                                                       //
// TGMrbSliders * sl = new TGMrbSliders("Set HLS", NVAL,                 // 
//                     min, max, val, lab, flags,                        //
//                     (TRootCanvas*)fCanvas->GetCanvasImp());           //
//  sl->Connect("SliderEvent(Int_t, Int_t, Int_t)",this->ClassName(),    //
//                this, "AdjustHLS(Int_t, Int_t, Int_t)");               //
//  .....                                                                //
//                                                                       //
//  void ColorPlayer::AdjustHLS(Int_t id, Int_t row , Int_t val)         //
//                                                                       //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const Char_t *  Title                                                //
//  Int_t NValues     : number of entries                                //
//  Double_t * Min, Max, Val : min, max, value                           //
//                                                                       //
// optional arguments:                                                   //
//                                                                       //
//  TOrdCollection * RowLabels                                           //
//  Int_t * flags   : if bit 1 set TextEntry is colored with value = hue //
//                  : if bit 2 set, move sliders with bit 2 combined     //
//  TGWindow *win   : pointer to parent window                           //  
//  Int_t identifier : unique number passed in emit signal               //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


//________________________________________________________________________________________
enum buttonId {kCOMBINED = 101};

Int_t GetFreeColorIndex()
{
	// Search for the highest color index not used in ROOT:
	// We do not want to overwrite some colors...
	Int_t highestIndex = 0;
	TColor * color;
	TSeqCollection *colorTable = gROOT->GetListOfColors();
	if ((color = (TColor *) colorTable->Last()) != 0) {
		if (color->GetNumber() > highestIndex) {
			highestIndex = color->GetNumber();
		}
		while ((color = (TColor *) (colorTable->Before(color))) != 0) {
			if (color->GetNumber() > highestIndex) {
				highestIndex = color->GetNumber();
			}
		}
	}
	return highestIndex + 1;
}
//________________________________________________________________________________________

ClassImp(MyTimer)
ClassImp(TGMrbSliders)

//____________________________________________________________________________
MyTimer::MyTimer(Long_t ms, Bool_t synch,  TGMrbSliders* slider):TTimer(ms,synch)
{
	fSlider = slider;
//     cout << "init mytimer" << endl;
	gSystem->AddTimer(this);
};
//_____________________________________________________________________________
//MyTimer::~MyTimer() {}
//_____________________________________________________________________________

Bool_t MyTimer::Notify() {
 //  cout << " Notify reached" << endl;
	fSlider->Wakeup();
	Reset();
	return kTRUE;
};
//________________________________________________________________________________

TGMrbSliders::TGMrbSliders(const char *Title,  const Int_t NValues,
								 const Int_t * min, const Int_t * max, Int_t * val, 
								 TOrdCollection * Row_Labels, const Int_t * flags, 
								 const TGWindow *Win, const Int_t Identifier)
								 : TGTransientFrame(gClient->GetRoot(), Win, 100, 100)
{
//   ChangeOptions(kHorizontalFrame);
	static Int_t color_index = 10000;
	fIdentifier = Identifier;
	fNValues = NValues; 
	
	fVal = new Int_t[NValues];
	fValPrev = new Int_t[NValues];
	for (Int_t i = 0; i < NValues; i++ ) {
	  fValPrev[i] = -1;
	  fVal[i] = val[i];
	}
	if (flags) {
		fFlags = new Int_t[NValues];
		for (Int_t i = 0; i < NValues; i++ ) fFlags[i] = flags[i];
	} else {
		fFlags = NULL;
	}
	fWidgetList = new TList();
//   fStopwatch = new TStopwatch();
	ULong_t brown;
	gClient->GetColorByName("firebrick", brown);
	color_index = GetFreeColorIndex();
	fTeColor = new TColor(color_index, 1, 0, 0);
 //  cout << "TGMrbSliders,color_index: " <<  color_index << endl;
	const TGWindow * main = gClient->GetRoot();
	if(Win != 0)fMyWindow = Win;
	else        fMyWindow = main;

	//--- layout for buttons: top align, equally expand horizontally

	fBly = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 3, 0);

	//--- layout for the frame: place at bottom, right aligned
	fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsRight, 20, 10, 15, 0);
	fLO4 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY 
									 |kLHintsCenterY| kLHintsCenterX, 1, 1, 1, 1);

	fTePointers  = new TGTextEntry * [NValues];
	fTbPointers  = new TGTextBuffer * [NValues];
	fSlPointers  = new TGHSlider * [NValues];
	TGCompositeFrame *vframe = new TGCompositeFrame(this, 100, 200, kVerticalFrame);

	TGHorizontalFrame *hframe = new TGHorizontalFrame(vframe, 0, 0, 0);
//   fSliderFrame = new TGVerticalFrame(vframe, 0, 0, 0);
//   fValueFrame  = new TGVerticalFrame(vframe, 0, 0, 0);
	for (Int_t i = 0; i < NValues; i++) {
		hframe = new TGHorizontalFrame(vframe, 0, 0, 0);
		fWidgetList->AddFirst(hframe);

		TObjString *objs = (TObjString *)Row_Labels->At(i);
		TString s = objs->String();
		TGCompositeFrame * label_fr = new TGCompositeFrame(hframe,100,20,
										kVerticalFrame | kFixedWidth |kRaisedFrame);
		fWidgetList->AddFirst(label_fr);
		TGLabel * label = new TGLabel(label_fr, new TGString((const char *)s));
		fWidgetList->AddFirst(label);
		label_fr->AddFrame(label, fLO4);
		hframe->AddFrame(label_fr, fBly);
		fSlPointers[i] = new TGHSlider(hframe, 100, kSlider1 | kScaleBoth, i);
		fTePointers[i] = new TGTextEntry(hframe, 
							 fTbPointers[i] = new TGTextBuffer(10), i + 1000);
		fTePointers[i]->Resize(80, fTePointers[i]->GetDefaultHeight());
		fTbPointers[i]->AddText(0, Form("%d", val[i])); 
		fSlPointers[i]->Associate(this);
		fTePointers[i]->Associate(this);
		fSlPointers[i]->SetRange(min[i], max[i]);
		fSlPointers[i]->SetPosition(val[i]);
		hframe->AddFrame(fSlPointers[i], fBly);
		hframe->AddFrame(fTePointers[i], fBly);
		vframe->AddFrame(hframe, fLO4);
	}
//   vframe->AddFrame(fValueFrame, fBfly1);
//   vframe->AddFrame(fSliderFrame, fBfly1);
//   vframe->AddFrame(fLabelFrame, fBfly1);
	AddFrame(vframe, new TGLayoutHints(kLHintsTop | kLHintsCenterX , 2, 2, 2, 2));
//  is combined change forseen
	fCombined = kFALSE;
	if (fFlags) { 
		for (Int_t row = 0; row < fNValues; row++) { 
			if (fFlags[row] & 2) fCombined = kTRUE; 
		}
	}
	if (fCombined) {
		hframe = new TGHorizontalFrame(this,150, 20, 0);
		fWidgetList->AddFirst(hframe);
		fCombinedButton = new TGCheckButton(hframe, 
						  new TGHotString("Change combined"), kCOMBINED);
		fCombinedButton->SetState(kButtonUp);
		fWidgetList->Add(fCombinedButton);
		fCombinedButton->Associate(this);
		hframe->AddFrame(fCombinedButton, new TGLayoutHints( 0 , 2, 2, 2, 2));
		AddFrame(hframe, new TGLayoutHints(kLHintsBottom | kLHintsCenterX , 2, 2, 2, 2));
		fCombined = kFALSE;
	}

	// set dialog title
	this->SetWindowName(Title);
	// map all widgets and calculate size of dialog
	this->MapSubwindows();
	Int_t  width  = this->GetDefaultWidth();
	Int_t  height = this->GetDefaultHeight();

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
// left of parent window
//   Int_t parents_width = ((TGFrame *) fMyWindow)->GetWidth();
//   cout << " ax, ay " << ax << " " <<ay << endl;
//   ax = ax -  parents_width / 2 -  width / 3;
	ax = ax - width / 2;

//  make sure its inside physical sceen
	if      (ax < 0)    ax = 10;
	else if (ax + wi > wwi)  ax = wwi - wi - 5;
	if      (ay + hi/2 < 0) ay =  hi/2 + 5;
	else if (ay + hi/2 > whi) ay = whi - hi/2 -5;
 //  cout << " ax, ay " << ax << " " <<ay << endl;
	this->Move(ax, ay);
	this->SetWMPosition(ax, ay);
	// map all widgets and calculate size of dialog
	this->MapSubwindows();

/*
	// make the message box non-resizable
	this->SetWMSize(width, height);
	this->SetWMSizeHints(width, height, width, height, 0, 0);

	this->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
													kMWMDecorMinimize | kMWMDecorMenu,
								kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
													kMWMFuncMinimize,
								kMWMInputModeless);
*/
	// popup dialog and wait till user replies
	this->MapWindow();
	this->ChangeBackground(brown);
	fTimer = new MyTimer(50, kTRUE, this);
 
//   gClient->WaitFor(this);
};

TGMrbSliders::~TGMrbSliders()
{
	// Delete dialog.
//   cout << "dtor ~TGMrbSliders()" << endl;
	fWidgetList->Delete();
	delete fWidgetList;
	for (Int_t i = 0; i < fNValues; i++) {
		delete fSlPointers[i];
		delete fTePointers[i];
	}

	delete [] fSlPointers;
	delete [] fTePointers;                    
	delete [] fTbPointers;
	delete [] fVal;
	delete [] fValPrev;
	if (fFlags) delete [] fFlags;

	delete fTeColor;
//   delete fStopwatch;
	delete fTimer;
	delete fBfly1; delete fBly; delete fLO4;
}

void TGMrbSliders::CloseWindow()
{
	// Called when window is closed via the window manager.
//   cout << "TGMrbSliders::CloseWindow() " << endl;
	SliderClosed(this);
	DeleteWindow();
}

Bool_t TGMrbSliders::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//    cout << "ProcessMessage : " << parm1 << endl;
//    if (fCombined ) cout << " fCombined true" << endl;
//    else  cout << " fCombined false" << endl;
	// Process slider messages.
//   Float_t r, g, b;   
	Int_t row;
	switch (GET_MSG(msg)) {
		case kC_COMMAND:
//         switch (GET_SUBMSG(msg)) { 
//            case kCM_BUTTON:
				if (parm1 == kCOMBINED) {
					if (fCombinedButton->GetState() == kButtonDown) {
						fCombined = kTRUE;
					} else {
						fCombined = kFALSE;
					}
				}
//            if (fCombined ) cout << " fCombined true" << endl;
//            else  cout << " fCombined false" << endl;
				break;
//         }
		case kC_TEXTENTRY:
			switch (GET_SUBMSG(msg)) {
				case kTE_ENTER:
					row = parm1%1000;
//               cout << parm1 << " " << row << endl;
//               cout << fSlPointers[row] << endl;
					fSlPointers[row]->SetPosition(atoi(fTbPointers[row]->GetString()));
					fVal[row] = fSlPointers[row]->GetPosition();
//               SliderEvent(fIdentifier, row, fVal[row]);
					break;
			}
			break;
		case kC_VSLIDER:
		case kC_HSLIDER:
			switch (GET_SUBMSG(msg)) {
				case kSL_RELEASE:
					row = parm1%1000;
					SliderEvent(row, fVal[row]);
					break;
				case kSL_POS:
					row = parm1%1000;
					fVal[row] = parm2;
					fTbPointers[row]->Clear();
					fTbPointers[row]->AddText(0, Form("%ld", parm2));
					fClient->NeedRedraw(fTePointers[row]);
					break;
			}
			break;
		default:
			break;
	}
	return kTRUE;
}
//______________________________________________________________

void  TGMrbSliders::Wakeup()
{
	if (fCombined) cout << "Wakeup()" << endl;
	Float_t r, g, b; 
//  did a value change which should be correlated with others
	Int_t common_val = 0;
	Bool_t changed = kFALSE;
	for (Int_t row = 0; row < fNValues; row++) { 
		if (fFlags && fFlags[row] & 2 &&  fVal[row] != fValPrev[row]) {
			common_val = fVal[row];
			changed = kTRUE; 
		}
	}
	if (fCombined && changed) {
		for (Int_t row = 0; row < fNValues; row++) { 
			if (fFlags && fFlags[row] & 2) {
				fSlPointers[row]->SetPosition(common_val);
				fVal[row] = common_val;
				fTbPointers[row]->Clear();
				fTbPointers[row]->AddText(0, Form("%d", common_val));
			}
		}
	}     
	for (Int_t row = 0; row < fNValues; row++) { 
		if (fVal[row] != fValPrev[row]) {
//  optionally set color of text widget
			if (fFlags && fFlags[row] & 1){
				TColor::HLS2RGB(fVal[row], 0.5, 1, r, g, b);
				fTeColor->SetRGB(r, g, b);
				Pixel_t p = fTeColor->GetPixel();
				fTePointers[row]->SetBackgroundColor(p);
//         	 cout << "Wakeup() set pixel color " << fVal[row] << " " <<
 //        	 r<<" "<<g<<" "<<b << " " << p << endl;
 //           fClient->NeedRedraw(fTePointers[row]);
			}
			fClient->NeedRedraw(fTePointers[row]);
			fValPrev[row] = fVal[row];
			SliderEvent(row, fVal[row]);
		}
	}
}
//______________________________________________________________

void  TGMrbSliders::SliderEvent(Int_t row, Int_t val)
{
	Long_t args[2];   // row, value
	args[0] = (Long_t)row;
	args[1] = (Long_t)val;
//   cout << "SliderEvent: Emit " << id << " " << row << " " << val << endl;
 
	Emit("SliderEvent(Int_t, Int_t)", args);
};
//______________________________________________________________

void  TGMrbSliders::SliderClosed(TObject* obj)
{
	Long_t args[1];   // row, value
	args[0] = (Long_t)obj;
//   cout << "SliderEvent: Emit " << id << " " << row << " " << val << endl;
 
	Emit("SliderClosed(TObject*)", args);
} 

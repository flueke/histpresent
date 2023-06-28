#ifndef __TGMrbSliders_h__
#define __TGMrbSliders_h__

#include "Rtypes.h"
#include "TSystem.h"
#include <TQObject.h>
#include "TGWindow.h"
#include <TGButton.h>
#include "TGFrame.h"
#include "TGSlider.h"
#include "TGTextEntry.h"
#include <TList.h>
#include "TColor.h"
#include "TTimer.h"
//#include "MyTimer.h"


class TGMrbSliders; 

class MyTimer : public TTimer 
{
private:
  TGMrbSliders * fSlider;
public:  
   MyTimer(Long_t ms, Bool_t synch, TGMrbSliders * slider);
   virtual ~MyTimer(){};
   Bool_t Notify();

ClassDef(MyTimer,0)
};

class TGMrbSliders : public TGTransientFrame {

private:
   Int_t fNValues;
   Int_t *fVal;
   Int_t *fValPrev;
   Int_t *fFlags;
   Int_t fIdentifier;
   const TGWindow * fMyWindow;
   TList *fWidgetList;
   Bool_t fCombined;
   TGCheckButton *fCombinedButton;
   TGTextEntry  ** fTePointers;
   TGTextBuffer ** fTbPointers;
   TGHSlider    ** fSlPointers;
   TGLayoutHints *fBly, *fBfly1, *fLO4; 
//   TStopwatch * fStopwatch;
   MyTimer * fTimer;
   TColor * fTeColor;

public:
   TGMrbSliders(const char *Title,  const Int_t NValues,
                const Int_t * min, const Int_t * max, Int_t * val, 
                TOrdCollection * Row_Labels = 0, const Int_t * flags = 0, 
                const TGWindow *Win = 0, const Int_t Identifier = 0);
   virtual ~TGMrbSliders();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void SliderEvent(Int_t, Int_t);
   void SliderClosed(TObject*);
   void Wakeup();
ClassDef(TGMrbSliders,0)		// [GraphUtils] A box with sliders
};
#endif

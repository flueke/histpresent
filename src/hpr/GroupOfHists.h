#ifndef GROUPOFHISTS
#define GROUPOFHISTS
#include <TVirtualPad.h>
#include <TGWindow.h>
#include <HTCanvas.h>
#include <TRootCanvas.h>
#include <TList.h>
#include <TGMenu.h>
#include <TTimer.h>
#include <iostream>
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


namespace std {} using namespace std;

class GroupOfHists;

class GoHTimer : public TTimer 
{
private:
   GroupOfHists *fGoH;
public:  
   GoHTimer(Long_t ms, Bool_t synch, GroupOfHists *goh);
   virtual ~GoHTimer(){};
   Bool_t Notify();

ClassDef(GoHTimer,0)
};
//________________________________________________________________________

class HistPresent;

class GroupOfHists : public TNamed {

private:
   void *fValp[100];
   TList *fRow_lab;
   TList *fHList;
   TList *fPadList;
   TGMrbValuesAndText *fDialog;
   HistPresent *fHistPresent;
   HTCanvas    *fCanvas;
   TList       fHistList;
   GoHTimer    *fTimer;
   TGPopupMenu *fMenu;
   TGPopupMenu *fOptionMenu;
   TRootCanvas *fRootCanvas;   
   Bool_t      fAnyFromSocket;

   Int_t       fWindowXWidth;
   Int_t       fWindowYWidth;
   Int_t       fNx;
   Int_t       fNy;

   Double_t    fMagFac;
   Int_t       fMagFacButton;
   Int_t			fDisplayLogX;			// Log scale for all hists X
   Int_t			fDisplayLogY;			// Log scale for all hists Y
   Int_t			fDisplayLogZ;			// Log scale for all hists z
   Int_t			fTwoDimLogX;			// Log scale for all hists X
   Int_t			fTwoDimLogY;			// Log scale for all hists Y
   Int_t			fTwoDimLogZ;			// Log scale for all hists z
   Double_t		fDisplayLowX;		// Display range for all hists Low X
   Double_t		fDisplayLowY;		// Display range for all hists Low Y
   Double_t		fDisplayUpX;		// Display range for all hists upper X
   Double_t		fDisplayUpY;		// Display range for all hists upper Y
/*
   Double_t    fMarginX;
   Double_t    fMarginY;
   Double_t    fLeftMargin;
   Double_t    fRightMargin;
   Double_t    fTopMargin; 
   Double_t    fBotMargin;  
*/
   Int_t       fAutoUpdateDelay;
   Int_t       fCommonRotate;
   Int_t       fNoSpace;
   Int_t       fNoSpaceButton;
   Int_t       fShowAllAsFirst;
   Int_t       fArrangeOnTop;
   Int_t       fArrangeSideBySide;
   Int_t       fArrangeAsTiles;
   Int_t   fFill1Dim;      
   Color_t fHistFillColor ;
   Style_t fHistFillStyle; 
	Color_t fHistLineColor1Dim; 
	Color_t fHistLineColor2Dim; 
	Style_t fHistLineStyle;
	Width_t fHistLineWidth1Dim;
	Width_t fHistLineWidth2Dim;
	Int_t   fShowContour; 
	Int_t   fShowZScale;
	Int_t   fOptStat1Dim;
	Int_t   fOptStat2Dim;
	Int_t   fShowStatBox1Dim;
	Int_t   fShowStatBox2Dim;
	Int_t   fNh1Dim;
	Int_t   fNh2Dim;
	Double_t   fPadBottomMargin;
	Double_t   fPadTopMargin;  
	Double_t   fPadLeftMargin;
	Double_t   fPadRightMargin;
   TString fErrorMode;    
   TString fDrawOpt2Dim;
  
public:
   GroupOfHists(TList * hlist, HistPresent * hpr, const Char_t *title = NULL);
   GroupOfHists();
   ~GroupOfHists();
   void   BuildCanvas();
   void   RecursiveRemove(TObject *obj);
   void   ActivateTimer(Int_t delay);    // start timer, delay(milli seconds)
   void   UpdateHists();
   void   BuildMenu();
   void   HandleMenu(Int_t id);
   void   auto_exec();
   void   ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, 
          Int_t mode = 0, TGWindow * win = NULL);
   void   RebinAll(TCanvas * canvas);
//   void   WarnBox(const char *message);
   TH1    *GetTheHist(TVirtualPad * pad);
   TList  *GetPadList(){ return fPadList;};
   void   SetOptions();
   void   SaveDefaults();
   void   RestoreDefaults();
   void   CloseDown(Int_t wid);
   void   CRButtonPressed(Int_t, Int_t, TObject*);

ClassDef(GroupOfHists,0)
};
#endif

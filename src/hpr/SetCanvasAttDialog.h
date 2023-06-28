#ifndef SETCANVASATTDIALOG
#define SETCANVASATTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class SetCanvasAttDialog : public TObject {

private:
   TGMrbValuesAndText *fDialog;
   void       *fValp[100];
   TList      *fRow_lab;
   TCanvas    *fCanvas;
   Color_t     fPadColor;       
   Width_t     fPadBorderSize;  
   Int_t       fPadBorderMode;  
   Float_t     fPadBottomMargin;
   Float_t     fPadTopMargin; 
   Float_t     fPadLeftMargin;
   Float_t     fPadRightMargin; 
   Int_t       fPadGridX;
   Int_t       fPadGridY;
   Int_t       fPadTickX;
   Int_t       fPadTickY;
	Color_t     fFrameFillColor;    //pad frame fill color
	Color_t     fFrameLineColor;    //pad frame line color
	Style_t     fFrameFillStyle;    //pad frame fill style
	Style_t     fFrameLineStyle;    //pad frame line style
	Width_t     fFrameLineWidth;    //pad frame line width
	Width_t     fFrameBorderSize;   //pad frame border size
	Short_t     fFrameBorderMode;   //pad frame border mode
	Color_t     fCanvasColor;       //canvas color
	Width_t     fCanvasBorderSize;  //canvas border size
   Short_t     fCanvasBorderMode; //canvas border mode
	Int_t       fCanvasDefH;        //default canvas height
	Int_t       fCanvasDefW;        //default canvas width
	Int_t       fCanvasDefX;        //default canvas top X position
	Int_t       fCanvasDefY;        //default canvas top Y position
	Int_t       fPadLabelX;
	Int_t       fPadLabelY;
	Int_t       fCanvasFillStyle; 
	Int_t       fOk;
	Int_t       fDummy;
	
public:

   SetCanvasAttDialog(TGWindow * win = NULL);
	SetCanvasAttDialog(Int_t batch);
   ~SetCanvasAttDialog();
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SetCanvasAtt(TCanvas *canvas);
   void SetPadAtt(TPad *pad);
   void SetCanvasAttPerm();
   void SaveDefaults();
   void RestoreDefaults(Int_t resetall=0);
   void SetAllToDefault();
   static void SetDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(SetCanvasAttDialog,0)
};
#endif

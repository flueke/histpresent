#ifndef SETHISTOPTHISTDIALOG
#define SETHISTOPTHISTDIALOG
#include "TH1.h"
#include "TGWindow.h"
#include "TStyle.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class TPavesText;
class TLegend;

class SetHistOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TCollection  *fHistList;
   TGWindow     *fWindow;
	TH1          *fHist;
	TObject      *fParent;
	TPad         *fPad;
	TAxis        *fAxisX;
	TAxis        *fAxisY;
	TAxis        *fAxisZ;
	TPavesText   *fStatBox;
	TPavesText   *fTitleBox;
	TLegend      *fLegendBox;
	TString      fObjTitle;
	Int_t        fNSubPads;
	Int_t        fPdivX;
	Int_t        fSdivX;
	Int_t        fTdivX;
	Int_t        fPdivY;
	Int_t        fSdivY;
	Int_t        fTdivY;
	Int_t        fPdivZ;
	Int_t        fSdivZ;
	Int_t        fTdivZ;
	Int_t        fOptimizedivX;
	Int_t        fOptimizedivY;
	Int_t        fOptimizedivZ;
   Int_t        fNdivisionsX;
   Int_t        fNdivisionsY;
   Int_t        fNdivisionsZ;
   Color_t      fAxisColor;
   Color_t      fLabelColor;
   Style_t      fLabelFont;
   Float_t      fLabelOffsetX;
   Float_t      fLabelOffsetY;
   Float_t      fLabelOffsetZ;
   Float_t      fLabelSize;
   Int_t        fLabelMaxDigits;
   Float_t      fTickLength;
   TString      fTickSideX;
   TString      fTickSideY;
   TString      fTickSideZ;
   Float_t      fTitleOffsetX;
   Float_t      fTitleOffsetY;
   Float_t      fTitleOffsetZ;
   Float_t      fTitleSize;
   Color_t      fTitleColorA;
   Style_t      fTitleFontA;
   Float_t      fLabelOffsetX3Dim;
   Float_t      fLabelOffsetY3Dim;
   Float_t      fLabelOffsetZ3Dim;
   Float_t      fLabelSize3Dim;
   Float_t      fTickLength3Dim;
   TString      fTickSideX3Dim;
   TString      fTickSideY3Dim;
   TString      fTickSideZ3Dim;
   Float_t      fTitleOffsetX3Dim;
   Float_t      fTitleOffsetY3Dim;
   Float_t      fTitleOffsetZ3Dim;
   Color_t      fTitleLineColor;
	Color_t      fTitleFillColor;
	Color_t      fTitleTextColor;
   Width_t      fTitleBorderSize;
   Style_t      fTitleFont;
   Float_t      fHistTitleSize;
   Style_t      fTitleStyle;
   Float_t      fTitleX;
   Float_t      fTitleY;
   Float_t      fTitleW;
   Float_t      fTitleH;
   Short_t      fTitleAlign;        //Text alignment
   Int_t        fTitleCmd1;
   Int_t        fTitleCmd2;
   Color_t      fStatLineColor;
	Color_t      fStatFillColor;
	Color_t      fStatTextColor;
   Width_t      fStatBorderSize;
   Style_t      fStatFont;
   TString      fStatFormat;
   Float_t      fStatFontSize;
   Style_t      fStatStyle;
   Float_t      fStatX;
   Float_t      fStatY;
   Float_t      fStatW;
   Float_t      fStatH;
	Float_t      fStatX2D;
	Float_t      fStatY2D;
	Float_t      fStatW2D;
	Float_t      fStatH2D;
	Int_t        fStatCmd1;
   Int_t        fStatCmd2;
	Int_t        fBidReselect;
   TString      fCustomStyleName;
   Int_t        fNewStyle;
	Int_t			 fDummy;
	Int_t        fOk;
public:
	static Int_t fTitleCenterX;
	static Int_t fTitleCenterY;
	static Int_t fTitleCenterZ;
	
public:
		
   SetHistOptDialog(TGWindow * win = NULL, TCollection * hlist = NULL);
	SetHistOptDialog(Int_t batch);
   ~SetHistOptDialog();
   void CloseDialog();
	Bool_t SetPointers();
   void SetHistAttNow(Int_t bid = 0);
   void SetHistAtt(Int_t bid = 0);
   void SetHistAttPermLocal();
   void SetHistAttPerm(TStyle * style = NULL);
   void SetStatBoxAttr(TStyle * style = NULL);
	void SetLegendBoxAttr(TStyle * style = NULL);
	void SetTitleBoxAttr(TStyle * style = NULL);
   void SetCustomStyle();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
   static void SetDefaults(Int_t ndim =1);
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(SetHistOptDialog,0)
};
#endif

#ifndef SETCOLORMODEDIALOG
#define SETCOLORMODEDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include "TGMrbSliders.h"
#include <iostream>

//_____________________________________________________________________________________

class SetColorModeDialog : public TObject {

private:
   void *fValp[100];
   Int_t fFlags[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TGMrbSliders *fHlsSlider;
   TGMrbSliders *fRgbSlider;
   TCanvas      *fCanvas;
   TGWindow     *fWindow;
   static TColor** fTransColorsRGB;
   static TColor** fTransColorsHLS;
   static TString fPalName;
	static Int_t fMtransRGB;
	static Int_t fMtransHLS;
	static Int_t fMbw;
	static Int_t fMbwinv;
	static Int_t fMrbow;
//	static Int_t fMBird; 
//	static Int_t fMDeepSea; 
//	static Int_t fMBlackBody;
//	static Int_t fMTwoColHue;	
	static Int_t fByNumber;
	static Int_t fPaletteNumber;
   static Int_t fStartColorIndex;
   static Color_t fStartColor;
   static Color_t fEndColor;
   static Int_t fNofTransLevels;
   static Int_t fNofGreyLevels;
   static Int_t *fGreyPaletteInv;
   static Int_t *fTransPaletteRGB;
   static Int_t *fTransPaletteHLS;

   static Float_t fEnhenceRed;
   static Float_t fEnhenceGreen;
   static Float_t fEnhenceBlue;
   static Int_t   fStartHue;
   static Int_t   fEndHue;
   static Float_t fLightness;
   static Float_t fSaturation;
public:
   SetColorModeDialog(TGWindow * win = NULL, Int_t batch = 0);
//#if ROOTVERSION > 60000
//   SetColorModeDialog() {};
//#endif
   ~SetColorModeDialog();
//   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SliderClosed(TObject * obj);
   static void SetColorMode();
   void UpdateCanvas();
   void SetBrightness();
   void AdjustBrightness(Int_t row , Int_t val);
   void SetHLS();
   void AdjustHLS(Int_t row , Int_t val);
   static void SetTransLevelsRGB();
   static void SetTransLevelsHLS();
   static void SetGreyLevels();

   static void SetStartColorIndex( Int_t val) { fStartColorIndex = val; };
   static void SetStartColor( Int_t val) { fStartColor = val; };
   static void SetEndColor( Int_t val) { fEndColor = val; };
   static void SetNofTransLevels( Int_t val) { fNofTransLevels = val; };
   static void SetNofGreyLevels( Int_t val) { fNofGreyLevels = val; };
   static void SetEnhenceRed( Float_t val) { fEnhenceRed = val; };
   static void SetEnhenceGreen( Float_t val) { fEnhenceGreen = val; };
   static void SetEnhenceBlue( Float_t val) { fEnhenceBlue = val; };
   static void SetStartHue( Int_t val) { fStartHue = val; };
   static void SetEndHue( Int_t val) { fEndHue = val; };
   static void SetLightness( Float_t val) { fLightness = val; };
   static void SetSaturation( Float_t val) { fSaturation = val; };
   static Int_t GetStartColorIndex() { return fStartColorIndex; };
   static Int_t GetStartColor() { return fStartColor; };
   static Int_t GetEndColor() { return fEndColor; };
   static Int_t GetNofTransLevels() { return fNofTransLevels; };
   static Int_t GetNofGreyLevels() { return fNofGreyLevels; };
   static Float_t GetEnhenceRed() { return fEnhenceRed; };
   static Float_t GetEnhenceGreen() { return fEnhenceGreen; };
   static Float_t GetEnhenceBlue() { return fEnhenceBlue; };
   static Int_t GetStartHue() { return fStartHue; };
   static Int_t GetEndHue() { return fEndHue; };
   static Float_t GetLightness() { return fLightness; };
   static Float_t GetSaturation() { return fSaturation; };

   static void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(SetColorModeDialog, 0)
};
#endif

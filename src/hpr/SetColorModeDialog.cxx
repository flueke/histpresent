#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TMath.h"
#include "TColor.h"
#include "TOrdCollection.h"
#include "TObjString.h"
#include "TObject.h"
#include "TEnv.h"
#include "TH1.h"
#include "TStyle.h"
#include "SetColorModeDialog.h"
#include "hprbase.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

namespace std {} using namespace std;

typedef std::map<string, Int_t> palmap;

//TString SetColorModeDialog::fDrawOpt2Dim = "COLZ";
Color_t SetColorModeDialog::fStartColor;
Color_t SetColorModeDialog::fEndColor;

Int_t   SetColorModeDialog::fStartColorIndex = 1;
Int_t   SetColorModeDialog::fNofTransLevels = 20;
Int_t   SetColorModeDialog::fNofGreyLevels = 255;
Int_t   SetColorModeDialog::fPaletteNumber = 57;
Int_t   SetColorModeDialog::fByNumber = 0;
Int_t * SetColorModeDialog::fGreyPaletteInv = NULL;
Int_t * SetColorModeDialog::fTransPaletteRGB = NULL;
Int_t * SetColorModeDialog::fTransPaletteHLS = NULL;
Float_t SetColorModeDialog::fEnhenceRed;
Float_t SetColorModeDialog::fEnhenceGreen;
Float_t SetColorModeDialog::fEnhenceBlue;
Int_t   SetColorModeDialog::fStartHue;
Int_t   SetColorModeDialog::fEndHue;
Float_t SetColorModeDialog::fLightness;
Float_t SetColorModeDialog::fSaturation;
Int_t   SetColorModeDialog::fMtransRGB;
Int_t   SetColorModeDialog::fMtransHLS;
Int_t   SetColorModeDialog::fMbw;
Int_t   SetColorModeDialog::fMbwinv;
Int_t   SetColorModeDialog::fMrbow;
//Int_t   SetColorModeDialog::fMDeepSea;
//Int_t   SetColorModeDialog::fMBird;
//Int_t   SetColorModeDialog::fMBlackBody;
//Int_t   SetColorModeDialog::fMTwoColHue;
TColor** SetColorModeDialog::fTransColorsRGB=NULL;
TColor** SetColorModeDialog::fTransColorsHLS=NULL;
TString SetColorModeDialog::fPalName = "Bird";
palmap  fPals;

extern Int_t gHprDebug;

//__________________________________________________________________

ClassImp(SetColorModeDialog)
//__________________________________________________________________

SetColorModeDialog::SetColorModeDialog(TGWindow * win, Int_t batch)
{
static const Char_t helptext[] =
"This determines which color palette should be used.\n\
Nice effects may be produced playing with RGB (Red/Green/Blue)\n\
or HLS (Hue/Lightness/Saturation) transitions\n\
\n\
As of Root Vers 6.04.00 the following palettes are predefined:\n\
With Root Vers 5.34 only 51 - 55 are meaningful.\n\
\n\
	kDeepSea=51,          kGreyScale=52,    kDarkBodyRadiator=53,\n\
	kBlueYellow= 54,      kRainBow=55,      kInvertedDarkBodyRadiator=56,\n\
	kBird=57,             kCubehelix=58,    kGreenRedViolet=59,\n\
	kBlueRedYellow=60,    kOcean=61,        kColorPrintableOnGrey=62,\n\
	kAlpine=63,           kAquamarine=64,   kArmy=65,\n\
	kAtlantic=66,         kAurora=67,       kAvocado=68,\n\
	kBeach=69,            kBlackBody=70,    kBlueGreenYellow=71,\n\
	kBrownCyan=72,        kCMYK=73,         kCandy=74,\n\
	kCherry=75,           kCoffee=76,       kDarkRainBow=77,\n\
	kDarkTerrain=78,      kFall=79,         kFruitPunch=80,\n\
	kFuchsia=81,          kGreyYellow=82,   kGreenBrownTerrain=83,\n\
	kGreenPink=84,        kIsland=85,       kLake=86,\n\
	kLightTemperature=87, kLightTerrain=88, kMint=89\n\
	kNeon=90,             kPastel=91,       kPearl=92,\n\
	kPigeon=93,           kPlum=94,         kRedBlue=95,\n\
	kRose=96,             kRust=97,         kSandyTerrain=98,\n\
	kSienna=99,           kSolar=100,       kSouthWest=101,\n\
	kStarryNight=102,     kSunset=103,      kTemperatureMap=104,\n\
	kThermometer=105,     kValentine=106,   kVisibleSpectrum=107,\n\
	kWaterMelon=108,      kCool=109,        kCopper=110,\n\
	kGistEarth=111,       kViridis\n\
	\n\
For more look at:\n\
https://root.cern.ch/root/html604/TColor.html#TColor:SetPalette\n\
";
	const Int_t Ncol = 62;
//	const Char_t * pal_names[Ncol] = {"DeepSea", "GreyScale","DarkBodyRadiator",
//BlueYellow", "RainBow", "InvertedDarkBodyRadiator"};
	const Char_t * pal_names[Ncol] = {
   "DeepSea",          "GreyScale",    "DarkBodyRadiator",
   "BlueYellow",       "RainBow",      "InvertedDarkBodyRadiator",
   "Bird",             "Cubehelix",    "GreenRedViolet",
   "BlueRedYellow",    "Ocean",        "ColorPrintableOnGrey",
   "Alpine",           "Aquamarine",   "Army",
   "Atlantic",         "Aurora",       "Avocado",
   "Beach",            "BlackBody",    "BlueGreenYellow",
   "BrownCyan",        "CMYK",         "Candy",
   "Cherry",           "Coffee",       "DarkRainBow",
   "DarkTerrain",      "Fall",         "FruitPunch",
   "Fuchsia",          "GreyYellow",   "GreenBrownTerrain",
   "GreenPink",        "Island",       "Lake",
   "LightTemperature", "LightTerrain", "Mint",
   "Neon",             "Pastel",       "Pearl",
   "Pigeon",           "Plum",         "RedBlue",
   "Rose",             "Rust",         "SandyTerrain",
   "Sienna",           "Solar",        "SouthWest",
   "StarryNight",      "Sunset",       "TemperatureMap",
   "Thermometer",      "Valentine",    "VisibleSpectrum",
   "WaterMelon",       "Cool",         "Copper",
   "GistEarth",        "kViridis"
	};
	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
	if (gHprDebug > 0)
		cout << "SetColorModeDialog ctor " << batch << endl;
	RestoreDefaults();
	SetGreyLevels();
	SetTransLevelsHLS();
	SetTransLevelsRGB();
	for (Int_t i=0; i < Ncol; i++) {
		fPals[pal_names[i]] = 51 + i;
	}
	SetColorMode();

	if ( batch )
		return;
	fRgbSlider = NULL;
	fHlsSlider = NULL;
	fWindow = win;

	fRow_lab = new TList();
	Int_t ind = 0;
	static Int_t dummy;
	static TString stycmd("SaveDefaults()");
	static TString hlscmd("SetHLS()");
	static TString rgbcmd("SetBrightness()");
	fRow_lab = new TList();
	Int_t selected_bid = 0;
	fRow_lab->Add(new TObjString("RadioButton_Select one of these"));
	if (fByNumber != 0) selected_bid = ind;
	fValp[ind++] = &fByNumber;
	TString tmp("ComboSelect-Palettes");
#if ROOTVERSION < 60000
	Int_t ncol_used = 6;
#else
	Int_t ncol_used = Ncol;
#endif	
	for (Int_t i=0; i < ncol_used; i++) {
		tmp += ";";
		tmp += pal_names[i];
	}

	fRow_lab->Add(new TObjString(tmp));
	fValp[ind++] = &fPalName;
	fRow_lab->Add(new TObjString("RadioButton_Grey,high=white"));
	if (fMbw != 0) selected_bid = ind;
	fValp[ind++] = &fMbw;
	fRow_lab->Add(new TObjString("RadioButton+Grey,high=black"));
	if (fMbwinv != 0) selected_bid = ind;
	fValp[ind++] = &fMbwinv;
//	fRow_lab->Add(new TObjString("RadioButton+     Two Col Hue"));
//	if (fMTwoColHue != 0) selected_bid = ind;
//	fValp[ind++] = &fMTwoColHue;
	
	fRow_lab->Add(new TObjString("RadioButton+Rainbow(classic)"));
	if (fMrbow != 0) selected_bid = ind;
	fValp[ind++] = &fMrbow;
//	fRow_lab->Add(new TObjString("RadioButton+                  Bird"));
//	if (fMBird != 0) selected_bid = ind;
//	fValp[ind++] = &fMBird;
//	fRow_lab->Add(new TObjString("RadioButton+Black Body Rad"));
//	if (fMBlackBody != 0) selected_bid = ind;
//	fValp[ind++] = &fMBlackBody;
	
	fRow_lab->Add(new TObjString("CommentOnly_User defined Palettes"));
	fValp[ind++] = &dummy;
	fRow_lab->Add(new TObjString("RadioButton_Color transition HLS"));
	if (fMtransHLS != 0) selected_bid = ind;
	fValp[ind++] = &fMtransHLS;
	fRow_lab->Add(new TObjString("CommandButt+Set HLS"));
	fValp[ind++] = &hlscmd;
	fRow_lab->Add(new TObjString("RadioButton_Color transition RGB"));
	if (fMtransRGB != 0) selected_bid = ind;
	fValp[ind++] = &fMtransRGB;
	fRow_lab->Add(new TObjString("CommandButt+Set RGB"));
	fValp[ind++] = &rgbcmd;
	fRow_lab->Add(new TObjString("ColorSelect_Start color RGB "));
	fRow_lab->Add(new TObjString("ColorSelect+End color RGB   "));
	fValp[ind++] = &fStartColor;
	fValp[ind++] = &fEndColor;

	fRow_lab->Add(new TObjString("CommandButt_Set as global default"));
	fValp[ind++] = &stycmd;

	static Int_t ok;
   Int_t itemwidth =  58 * TGMrbValuesAndText::LabelLetterWidth();
	fDialog =
		new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
							 NULL, NULL, fRow_lab, fValp,
							 NULL, NULL, helptext, this, this->ClassName());
	fDialog->SetCheckButton(selected_bid, 1);
}
//______________________________________________________

SetColorModeDialog::~SetColorModeDialog()
{
	if (gHprDebug > 0) {
		cout << "SetColorModeDialog dtor" << endl;
	}
	if (fCanvas)
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
}
//______________________________________________________

void SetColorModeDialog::SetColorMode()
{	
	if (gHprDebug > 0) {
		cout << "SetColorMod: ";
		if (fByNumber != 0 )cout << "fByNumber ";
		if (fMbw != 0 )cout << "fMbw ";
		if (fMbwinv != 0 )cout << "fMbwinv ";
//		if (fMBird != 0 )cout << "fMBird ";
//		if (fMBlackBody != 0 )cout << "fMBlackBody ";
//		if (fMTwoColHue != 0 )cout << "fMTwoColHue ";
		if (fMtransRGB != 0 )cout << "fMtransRGB ";
		if (fMtransHLS != 0 )cout << "fMtransHLS ";
		if (fMrbow != 0 )cout << "fMrbow ";
		if (fByNumber != 0 ) 
			cout<< " " << fPalName.Data() << " " <<fPals[fPalName.Data()];
		cout << endl;
	}
	if        ( fMrbow ) {
		gStyle->SetPalette(1, NULL);
	} else if ( fMbw) {
		gStyle->SetPalette(52);
//	} else if ( fMBird ) {
//		gStyle->SetPalette(57);
//	} else if ( fMBlackBody ) {
//		gStyle->SetPalette(53);
//	} else if ( fMTwoColHue ) {
//		gStyle->SetPalette(54);
	} else if ( fByNumber ) {
		gStyle->SetPalette(fPals[fPalName.Data()]);
	} else if (fMbwinv) {
		SetGreyLevels();
		gStyle->SetPalette(fNofGreyLevels, fGreyPaletteInv);
	} else if (fMtransRGB) {
		SetTransLevelsRGB();
		gStyle->SetPalette(fNofTransLevels, fTransPaletteRGB);
	} else if (fMtransHLS) {
		SetTransLevelsHLS();
		gStyle->SetPalette(fNofTransLevels, fTransPaletteHLS);
	} else {
		gStyle->SetPalette(1, NULL);
	}
}
//__________________________________________________________________

void SetColorModeDialog::SetBrightness()
{
	TOrdCollection * lab = new TOrdCollection();
	lab->Add(new TObjString("red"));
	lab->Add(new TObjString("green"));
	lab->Add(new TObjString("blue"));
	static Int_t val[3];
	static Int_t max[3];
	static Int_t min[3];
	min[0] = 0;
	min[1] = 0;
	min[2] = 0;
	max[0] = 200;
	max[1] = 200;
	max[2] = 200;

	val[0] = (Int_t)(100 * SetColorModeDialog::fEnhenceRed);
	val[1] = (Int_t)(100 * SetColorModeDialog::fEnhenceGreen);
	val[2] = (Int_t)(100 * SetColorModeDialog::fEnhenceBlue);

	Int_t id  = 1;
	fRgbSlider = new TGMrbSliders("Set RGB", 3, min, max, val, lab, NULL,
	fWindow, id);
	fRgbSlider->Connect("SliderEvent(Int_t, Int_t)",this->ClassName() , this,
					"AdjustBrightness(Int_t, Int_t)");
	fRgbSlider->Connect("SliderClosed(TObject*)",this->ClassName() , this,
					"SliderClosed(TObject*)");
}
//___________________________________________________________________

void SetColorModeDialog::AdjustBrightness(Int_t row , Int_t val)
{
//   cout << "AdjustBrightness: " << id << " " << row << " " << val << endl;
	switch (row) {
		case 0:
		  SetColorModeDialog::fEnhenceRed   = (Float_t)val / 100.;
		  break;
		case 1:
		  SetColorModeDialog::fEnhenceGreen = (Float_t)val / 100.;
		  break;
		case 2:
		  SetColorModeDialog::fEnhenceBlue  = (Float_t)val / 100.;
		  break;
		default:
			return;
	}
//   cout << "AdjustBrightness: " <<SetColorModeDialog::fEnhenceRed
//        << " " << SetColorModeDialog::fEnhenceGreen << " " << SetColorModeDialog::fEnhenceBlue << endl;
	SetColorModeDialog::SetTransLevelsRGB();
//   hp->SetColorPalette(mycanvas);
	UpdateCanvas();
}
//___________________________________________________________________

void SetColorModeDialog::SetHLS()
{
	const Int_t NVAL = 4;
	TOrdCollection * lab = new TOrdCollection();
	lab->Add(new TObjString("Hue Start"));
	lab->Add(new TObjString("Hue End"));
	lab->Add(new TObjString("Lightness"));
	lab->Add(new TObjString("Saturation"));
	Int_t val[NVAL];
	const Int_t max[NVAL] = {360, 360, 100, 100};
	Int_t min[NVAL] = {0, 0, 0, 0};
	Int_t flags[NVAL] = {1, 1, 0, 0};
	val[0] = (Int_t)SetColorModeDialog::fStartHue;
	val[1] = (Int_t)SetColorModeDialog::fEndHue;
	val[2] = (Int_t)(100 * SetColorModeDialog::fLightness);
	val[3] = (Int_t)(100 * SetColorModeDialog::fSaturation);
	const Int_t id  = 1;
	fHlsSlider = new TGMrbSliders("Set HLS", NVAL,
						  min, max, val, lab, flags,
						  fWindow, id);
	fHlsSlider->Connect("SliderEvent(Int_t, Int_t)",this->ClassName() , this,
					"AdjustHLS(Int_t, Int_t)");
	fHlsSlider->Connect("SliderClosed(TObject*)",this->ClassName() , this,
					"SliderClosed(TObject*)");
}
//___________________________________________________________________

void SetColorModeDialog::AdjustHLS(Int_t row , Int_t val)
{
//   cout << "AdjustBrightness: " << id << " " << row << " " << val << endl;
	switch (row) {
		case 0:
		  SetColorModeDialog::fStartHue = val;
		  break;
		case 1:
		  SetColorModeDialog::fEndHue = val;
		  break;
		case 2:
		  SetColorModeDialog::fLightness  = (Float_t)val / 100.;
		  break;
		case 3:
		  SetColorModeDialog::fSaturation = (Float_t)val / 100.;
		  break;
		default:
			return;
	}
//   cout << "AdjustBrightness: " <<SetColorModeDialog::fEnhenceRed
//        << " " << SetColorModeDialog::fEnhenceGreen << " " << SetColorModeDialog::fEnhenceBlue << endl;
	SetColorModeDialog::SetTransLevelsHLS();
	UpdateCanvas();
}
//_______________________________________________________________

void SetColorModeDialog::UpdateCanvas()
{
	if (fCanvas) {
		TIter next(fCanvas->GetListOfPrimitives());
		TObject *obj;
		while ( (obj = next()) ) {
			if ((obj->InheritsFrom("TPad"))) {
				TPad *pad = (TPad*)obj;
				pad->Modified();
				pad->Update();
			}
		}
		fCanvas->Modified();
		fCanvas->Update();
	}
}
//_______________________________________________________________

void SetColorModeDialog::SetGreyLevels()
{
//	static Int_t mono = 0;  // only once 
	if (fGreyPaletteInv == NULL) {
		fGreyPaletteInv = new Int_t[fNofGreyLevels];
//		mono = 1;
		// grey: white high
      Double_t r[9] = { 0./255., 32./255., 64./255., 96./255., 128./255., 160./255., 192./255., 224./255., 255./255.};
      Double_t g[9] = { 0./255., 32./255., 64./255., 96./255., 128./255., 160./255., 192./255., 224./255., 255./255.};
      Double_t b[9] = { 0./255., 32./255., 64./255., 96./255., 128./255., 160./255., 192./255., 224./255., 255./255.};
      Double_t s[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000};
      // invert black high
      Double_t ri[9], gi[9],bi[9];
      for (Int_t i=0; i < 9; i++) {
			ri[i] = r[8-i];
			gi[i] = g[8-i];
			bi[i] = b[8-i];
		}
		Int_t FI = TColor::CreateGradientColorTable(9, s, ri, gi, bi, fNofGreyLevels);
		if (gHprDebug > 0)
			cout << " SetColorModeDialog:: Create GreyPalette startindex: " <<  FI << endl;
		for (int i=0;i<fNofGreyLevels;i++) fGreyPaletteInv[i] = FI+i;
	}
}
//___________________________________________________________________

void SetColorModeDialog::SetTransLevelsRGB()
{
	if (fNofTransLevels < 2) fNofTransLevels = 2;
	if (fTransPaletteRGB == NULL) {
		fTransPaletteRGB  = new Int_t[fNofTransLevels];
		Int_t colind = Hpr::GetFreeColorIndex();
		if (gHprDebug > 0)
			cout << "SetTransLevelsRGB() colind: " << colind << endl; 
		fTransColorsRGB = new TColor*[fNofTransLevels];
		for (Int_t i = 0; i < fNofTransLevels; i++) {
			fTransColorsRGB[i] = new TColor(colind, 1, 0, 0);
			colind++;
		}
	}
//	TColor * color;
	TColor * sc = gROOT->GetColor(fStartColor);
	if ( !sc ) {
		cout << "SetColorModeDialog::SetTransLevelsRGB Cant get color for index: " << fStartColor;
		return;
	}
	TColor * ec = gROOT->GetColor(fEndColor);
	if ( !ec ) {
		cout << "SetColorModeDialog::SetTransLevelsRGB Cant get color for index: " << fStartColor;
		return;
	}
	Float_t start_r = sc->GetRed();
	Float_t step_r = (ec->GetRed() - start_r) / (Float_t)(fNofTransLevels - 1);
	Float_t frac_r = 0;
	Float_t start_g = sc->GetGreen();
	Float_t step_g = (ec->GetGreen() - start_g) / (Float_t)(fNofTransLevels - 1);
	Float_t frac_g = 0;
	Float_t start_b = sc->GetBlue();
	Float_t step_b = (ec->GetBlue() - start_b) / (Float_t)(fNofTransLevels - 1);
	Float_t frac_b = 0;
	
	for (Int_t i = 0; i < fNofTransLevels; i++) {
		Float_t rgb[3];
		Float_t h, l, s;
		rgb[0] = start_r + frac_r;
		rgb[1] = start_g + frac_g;
		rgb[2] = start_b + frac_b;
		TColor::RGB2HLS(rgb[0], rgb[1], rgb[2], h, l, s);
		rgb[0] *= fEnhenceRed;
		rgb[1] *= fEnhenceGreen;
		rgb[2] *= fEnhenceBlue;
		if (rgb[0] > 1.) {
			rgb[1] += 0.5 * (rgb[0] - 1.);
			rgb[2] += 0.5 * (rgb[0] - 1.);
			rgb[0] = 1.;
		} else {
//         rgb[0] = 0;
		}
		if (rgb[1] > 1.) {
			rgb[0] += 0.5 * (rgb[1] - 1.);
			rgb[2] += 0.5 * (rgb[1] - 1.);
			rgb[1] = 1.;
		}else {
//         rgb[1] = 0;
		}
		if (rgb[2] > 1.) {
			rgb[0] += 0.5 * (rgb[2] - 1.);
			rgb[1] += 0.5 * (rgb[2] - 1.);
			rgb[2] = 1.;
		}else {
//         rgb[2] = 0;
		}
//      cout << "o rgb " << rgb[0] << " "  << rgb[1] << " "  << rgb[2] << endl; 
		fTransColorsRGB[i]->SetRGB( rgb[0], rgb[1], rgb[2]);
		fTransPaletteRGB[i] = fTransColorsRGB[i]->GetNumber();
		frac_r += step_r;
		frac_g += step_g;
		frac_b += step_b;
	}
}
//___________________________________________________________________

void SetColorModeDialog::SetTransLevelsHLS()
{
	if (fNofTransLevels < 2) fNofTransLevels = 2;
	if (fTransPaletteHLS == NULL) {
		fTransPaletteHLS  = new Int_t[fNofTransLevels];
		fTransColorsHLS = new TColor*[fNofTransLevels];
		Int_t colind = Hpr::GetFreeColorIndex();
		if (gHprDebug > 0)
			cout << "SetTransLevelsHLS() colind: " << colind << endl; 
		for (Int_t i = 0; i < fNofTransLevels; i++) {
			fTransColorsHLS[i] = new TColor(colind, 1, 0, 0);
			colind++;
		}
	}
//	TColor * color;

	Float_t step_h = (fEndHue - fStartHue) / (Float_t)(fNofTransLevels - 1);
	Float_t rgb[3];
	Float_t h, l, s;

	h = fStartHue;
	l = fLightness;
	s = fSaturation;

	for (Int_t i = 0; i < fNofTransLevels; i++) {
		TColor::HLS2RGB(h, l, s, rgb[0], rgb[1], rgb[2]);
		fTransColorsHLS[i]->SetRGB(rgb[0], rgb[1], rgb[2]);
		fTransPaletteHLS[i] = fTransColorsHLS[i]->GetNumber();
		h += step_h;
	}
}
//___________________________________________________________________

void SetColorModeDialog::SaveDefaults()
{
	TEnv env(".hprrc");
	env.SetValue("SetColorModeDialog.fPaletteNumber", fPaletteNumber);
	env.SetValue("SetColorModeDialog.fPalName", fPalName);
	env.SetValue("SetColorModeDialog.fByNumber", fByNumber);
	env.SetValue("SetColorModeDialog.fMtransRGB", fMtransRGB);
	env.SetValue("SetColorModeDialog.fMtransHLS", fMtransHLS);
	env.SetValue("SetColorModeDialog.fMbw", fMbw);
	env.SetValue("SetColorModeDialog.fMbwinv", fMbwinv);
	env.SetValue("SetColorModeDialog.fMrbow", fMrbow);
//	env.SetValue("SetColorModeDialog.fMDeepSea", fMDeepSea);
//	env.SetValue("SetColorModeDialog.fMBird", fMBird);
//	env.SetValue("SetColorModeDialog.fMBlackBody", fMBlackBody);
//	env.SetValue("SetColorModeDialog.fMTwoColHue", fMTwoColHue);
	env.SetValue("SetColorModeDialog.NofTransLevels", fNofTransLevels);
	env.SetValue("SetColorModeDialog.StartColor", fStartColor);
	env.SetValue("SetColorModeDialog.EndColor", fEndColor);
	env.SetValue("SetColorModeDialog.EnhenceRed",fEnhenceRed);
	env.SetValue("SetColorModeDialog.EnhenceGreen",fEnhenceGreen);
	env.SetValue("SetColorModeDialog.EnhenceBlue",fEnhenceBlue);
	env.SetValue("SetColorModeDialog.StartHue",fStartHue);
	env.SetValue("SetColorModeDialog.EndHue",fEndHue);
	env.SetValue("SetColorModeDialog.Lightness", fLightness);
	env.SetValue("SetColorModeDialog.Saturation",fSaturation);
	env.SaveLevel(kEnvLocal);
}

//___________________________________________________________________
/*
void SetColorModeDialog::RecursiveRemove(TObject * obj)
{
	if (obj == fCanvas) {
      cout << "SetColorModeDialog: RecursiveRemove "  << endl;
		CloseDialog();
	}
}
*/
//___________________________________________________________________

void SetColorModeDialog::CloseDialog()
{
//   cout << "SetColorModeDialog::CloseDialog() "
//        << "fHlsSlider " << fHlsSlider <<" fRgbSlider "
//        <<  fRgbSlider << endl;
	if (fHlsSlider) {

		fHlsSlider->DeleteWindow();
	}
	if (fRgbSlider) fRgbSlider->DeleteWindow();
	if (fDialog)    fDialog->CloseWindowExt();
	if (fRow_lab){
		fRow_lab->Delete();
		delete fRow_lab;
	}
	delete this;
}
//___________________________________________________________________

void SetColorModeDialog::RestoreDefaults()
{
	TEnv env(".hprrc");
	fPaletteNumber   = env.GetValue("SetColorModeDialog.fPaletteNumber", 57);
	
	fByNumber   = env.GetValue("SetColorModeDialog.fByNumber", 0);
	fPalName    = env.GetValue("SetColorModeDialog.fPalName", "Bird");
	fMtransRGB  = env.GetValue("SetColorModeDialog.fMtransRGB", 0);
	fMtransHLS  = env.GetValue("SetColorModeDialog.fMtransHLS", 0);
	fMbw        = env.GetValue("SetColorModeDialog.fMbw", 0);
	fMbwinv     = env.GetValue("SetColorModeDialog.fMbwinv", 0);
	fMrbow      = env.GetValue("SetColorModeDialog.fMrbow", 1);
//	fMDeepSea   = env.GetValue("SetColorModeDialog.fMDeepSea", 0);
//	fMBird      = env.GetValue("SetColorModeDialog.fMBird", 0);
//	fMBlackBody = env.GetValue("SetColorModeDialog.fMBlackBody", 0);
//	fMTwoColHue = env.GetValue("SetColorModeDialog.fMTwoColHue", 0);
	fNofTransLevels   = env.GetValue("SetColorModeDialog.NofTransLevels", 20);
	fStartColor       = env.GetValue("SetColorModeDialog.StartColor", 2);
	fEndColor         = env.GetValue("SetColorModeDialog.EndColor", 3);
	fEnhenceRed       = env.GetValue("SetColorModeDialog.EnhenceRed", 1.);
	fEnhenceGreen     = env.GetValue("SetColorModeDialog.EnhenceGreen", 1.);
	fEnhenceBlue      = env.GetValue("SetColorModeDialog.EnhenceBlue", 1.);
	fStartHue         = env.GetValue("SetColorModeDialog.StartHue", 120);
	fEndHue           = env.GetValue("SetColorModeDialog.EndHue", 1);
	fLightness        = env.GetValue("SetColorModeDialog.Lightness", 0.5);
	fSaturation       = env.GetValue("SetColorModeDialog.Saturation ", 1.);
}
//__________________________________________________________________

void SetColorModeDialog::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
	fDialog = NULL;
	if (wid == -1)
		SaveDefaults();
	CloseDialog();
}
//___________________________________________________________________

void SetColorModeDialog::SliderClosed(TObject *obj)
{
	if (obj == fHlsSlider)
		fHlsSlider = NULL;
	if (obj == fRgbSlider)
		fRgbSlider = NULL;
//   cout << "SliderClosed " << obj << " " << fHlsSlider << endl;
}
//___________________________________________________________________

void SetColorModeDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << " SetColorModeDialog::CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
	if ( fMrbow ) {
		TH1* hh = Hpr::FindHistInPad((TVirtualPad *)fCanvas);
		if (hh && hh->GetDimension() == 2) {
			hh->SetContour(fNofTransLevels);
		}
	}
	SetColorMode();
	UpdateCanvas();
}

//___________________________________________________________________

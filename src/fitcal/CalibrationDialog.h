#ifndef CALIBRATIONDIALOG
#define CALIBRATIONDIALOG
#include "TArrayD.h"
#include "TGraph.h"
#include "TList.h"
#include "TH1.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"

//_____________________________________________________________________________________

class CalibrationDialog : public TObject {

static const Int_t MAXPEAKS = 100;
static const Int_t MAXNOMPEAKS = 100;
enum EMenuIds {M_Save2File = 1001};
private:
   TRootCanvas        *fParentWindow;
   TGMrbValuesAndText *fDialog;
   TGMrbValuesAndText *fDialogSetNominal;
   TGMrbValuesAndText *fAutoSelectDialog;
   TCanvas            *fCalValCanvas;
   TCanvas            *fScanCanvas;
   TCanvas            *fEffCanvas;
   TString     fFuncName;
   TString     fFuncFromFile;
   TString     fFormula;
   TCanvas     *fSelCanvas;
   TH1         *fSelHist;
   TH1         *fCalHist;
   TF1         *fCalFunc;
   TList    fPeakList;
   Double_t fX[MAXPEAKS];
   Double_t fY[MAXPEAKS];
   Double_t fXE[MAXPEAKS];
   Double_t fYE[MAXPEAKS];
   Double_t fCont[MAXPEAKS];
   Int_t    fUse[MAXPEAKS];
   Int_t    fAssigned[MAXPEAKS];
   Int_t    fMaxPeaks;
   Int_t    fNpeaks;
   Int_t    fSetFlag[MAXNOMPEAKS];
   Int_t    fGaugeNpeaks;
   TList    fGaugeName;
   TArrayD  fGaugeEnergy;
   TArrayD  fGaugeError;
   TArrayD  fGaugeIntensity;
   Int_t    fAutoAssigned;

   Int_t    fInteractive;     // Flag for user interaction, 0: no Dialog
   Int_t    fVerbose;         // Verbose printout
   Int_t    fCalibratedNbinsX;// Params for calibrated histogram, Nbins
   Double_t fCalibratedXlow;  // Xlow
   Double_t fCalibratedXup;   // Xup
   Int_t    fEu152Gauge;      // Use Eu 152 gauge source
   Int_t    fCo60Gauge;       // Use Co60 gauge source
   Int_t    fY88Gauge;        // Use Y88gauge source
   Int_t    fCm244Gauge;      // Use Curium 244
	Int_t    fCustomGauge;     // Use user provided gauge peaks
	Int_t    fForceConst0;     // Force calibration function through origin (p0 = 0)
   TString  fCustomGaugeFile; // File name for user provided gauge peaks
   Int_t    fMatchNbins;      // Number of bins for peak match histogram
   Double_t fMatchMin;        // Low edge for peak match histogram
   Double_t fMatchMax;        // Upper edge for peak match histogram
   Double_t fGainMin;         // Min gain value  for scanning
   Double_t fGainMax;         // Max gain value  for scanning
   Double_t fGainStep;        // Step size gain for scanning
   Double_t fOffMin;          // Min offset value for scanning
   Double_t fOffMax;          // Max offset value for scanning
   Double_t fOffStep;         // Step size for offset for scanning
   Double_t fContThresh;      // Threshol on peak content relative to max
   Double_t fAccept;          // Acceptance limit: Abs (gauge - measured)
   Int_t    fUpdatePeakListDone;
//#ifdef MARABOUVERS
//   HistPresent *fHistPresent;
//#endif
public:
   CalibrationDialog(TH1 * hist, Int_t interactive = 1);
   CalibrationDialog();
   virtual ~CalibrationDialog();
   void RecursiveRemove(TObject * obj);
   TF1 * CalculateFunction();
   void SaveFunction();
   void GetFunction();
   void ExecuteGetFunction();
   void FillCalibratedHist();
   TList * UpdatePeakList();
   void SetNominalValues();
   void SetBuiltinGaugeValues();
   void QueryReadGaugeFile();
   Int_t ReadGaugeFile();
   void SetValues();
   void AutoSelectDialog();
   Bool_t ExecuteAutoSelect();
   void LabelPeaks();
   Int_t FindNumberOfPeaks();
//   void ClearHistPeakList();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown(Int_t wid);
   void DisableDialogs();
   void EnableDialogs();
   void CRButtonPressed(Int_t, Int_t, TObject *){};
   void AddMenu(TCanvas *canvas);
   void HandleMenu(Int_t id);

   inline void SetInteractive( Int_t interactive) { fInteractive = interactive; };
   inline void SetVerbose(Int_t val) { fVerbose = val; };
   inline void SetCalibratedNbinsX( Int_t calibratednbinsx) { fCalibratedNbinsX = calibratednbinsx; };
   inline void SetCalibratedXlow( Double_t calibratedxlow) { fCalibratedXlow = calibratedxlow; };
   inline void SetCalibratedXup( Double_t calibratedxup) { fCalibratedXup = calibratedxup; };
   inline void SetCustomGauge( Int_t customgauge) { fCustomGauge = customgauge; };
   inline void SetCustomGaugeFile(const Char_t * customgaugefile) { fCustomGaugeFile = customgaugefile; };
   inline void SetFormula(const Char_t * formula) { fFormula = formula; };
   inline void SetMatchNbins( Int_t matchnbins) { fMatchNbins = matchnbins; };
   inline void SetMatchMin( Double_t matchmin) { fMatchMin = matchmin; };
   inline void SetMatchMax( Double_t matchmax) { fMatchMax = matchmax; };
   inline void SetGainMin( Double_t gainmin) { fGainMin = gainmin; };
   inline void SetGainMax( Double_t gainmax) { fGainMax = gainmax; };
   inline void SetGainStep( Double_t gainstep) { fGainStep = gainstep; };
   inline void SetOffMin( Double_t offmin) { fOffMin = offmin; };
   inline void SetOffMax( Double_t offmax) { fOffMax = offmax; };
   inline void SetOffStep( Double_t offstep) { fOffStep = offstep; };
   inline void SetAccept( Double_t val) { fAccept = val; };
   inline void SetContThreshold( Double_t val) { fContThresh = val; };
   inline void SetEu152Gauge( Int_t eu152gauge) { fEu152Gauge = eu152gauge; };
   inline void SetCo60Gauge( Int_t co60gauge) { fCo60Gauge = co60gauge; };
   inline void SetY88Gauge( Int_t y88gauge) { fY88Gauge = y88gauge; };

   void SetGaugePoint(Int_t N, Int_t Use, Double_t X = 0, Double_t Y = 0, Double_t Xerr = 0, Double_t Yerr = 0);

   inline Int_t GetVerbose()          { return fVerbose; };
   inline Int_t GetInteractive()      { return fInteractive; };
   inline Int_t GetCalibratedNbinsX() { return fCalibratedNbinsX; };
   inline Double_t GetCalibratedXlow(){ return fCalibratedXlow; };
   inline Double_t GetCalibratedXup() { return fCalibratedXup; };
   inline Int_t GetCustomGauge()      { return fCustomGauge; };
   inline const Char_t *GetCustomGaugeFile(){ return fCustomGaugeFile.Data(); };
   inline const Char_t *GetFormula()    { return fFormula.Data(); };
   inline Int_t GetMatchNbins() { return fMatchNbins; };
   inline Double_t GetMatchMin(){ return fMatchMin; };
   inline Double_t GetMatchMax(){ return fMatchMax; };
   inline Double_t GetGainMin() { return fGainMin; };
   inline Double_t GetGainMax() { return fGainMax; };
   inline Double_t GetGainStep(){ return fGainStep; };
   inline Double_t GetOffMin()  { return fOffMin; };
   inline Double_t GetOffMax()  { return fOffMax; };
   inline Double_t GetOffStep() { return fOffStep; };
   inline Double_t GetAccept()  { return fAccept; };
   inline Double_t GetContThreshold() { return fContThresh; };
   inline Int_t GetEu152Gauge() { return fEu152Gauge; };
   inline Int_t GetCo60Gauge()  { return fCo60Gauge; };
   inline Int_t GetY88Gauge()   { return fY88Gauge; };

   inline TList * GetPeakList()   { return &fPeakList; };

ClassDef(CalibrationDialog,0)
};
#endif

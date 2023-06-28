#ifndef ASCII2HISTDIALOG
#define ASCII2HISTDIALOG
#include "TArrayD.h"
#include "TGWindow.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class TH1;
class HTCanvas;
namespace std {} using namespace std;
class Ascii2HistDialog : public TObject {

private:
   TArrayD  fXval;
   TArrayD  fYval;
   TArrayD  fZval;
   TArrayD  fWval;

	TH1  *fHist;
	HTCanvas *fCanvas;
   TGMrbValuesAndText *fDialog;
   TString  fCommand;
   TString  fSaveCommand;
   TString  fCommandHead;
   TString  fReadCommand;
   TString  fHistFileName;
   TString  fHistName;
   TString  fHistTitle;
   Int_t    fNvalues;
   Int_t		fDim;					// dimension of histogram
   Int_t    fSpectrum;        // Channel Contents only, no X values
   Int_t    f1Dim;            // X values to be filled
   Int_t    f1DimWithWeight;  // X, Weight
   Int_t    f1DimWithErrors;  // X, Weight, Error
   Int_t    f2Dim;            // X, Y values to be filled
   Int_t    f2DimWithWeight;  // X,Y,  Weight
   Int_t    f3Dim;            // X, Y, Z values to be filled
   Int_t    f3DimWithWeight;  // X,Y,Z,  Weight
   Int_t    fBidSelection;
   Int_t    fNskip;
   Double_t fError;  
   Int_t    fNbinsX; 
   Double_t fXlow;  
   Double_t fXup;   
   Int_t    fNbinsY; 
   Double_t fYlow;  
   Double_t fYup;   
   Int_t    fNbinsZ; 
   Double_t fZlow;  
   Double_t fZup;   
   Int_t    f1DimNbinsX; 
   Int_t    f2DimNbinsX; 
   Int_t    f3DimNbinsX; 
   Int_t    f2DimNbinsY; 
   Int_t    f3DimNbinsY; 
   Int_t    fKeepLimits;
 
public:
   Ascii2HistDialog(TGWindow * win = NULL);
   ~Ascii2HistDialog();
   void Draw_The_Hist();
   void Read_Input();
   void Show_Head_of_File();
   void WriteoutHist();
   void SaveDefaults();
   void GetDim();
   void SetDim();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/);

ClassDef(Ascii2HistDialog,0)
};
#endif

#include "Rtypes.h"
#include "TASImage.h"
#include "TEnv.h"
#include "GrCanvas.h"
#include "THprArc.h"
#include "TGMrbValuesAndText.h"

using std::cout;
using std::endl;

static const Int_t MAXL = 10;
class CDisc : public THprArc
{
private:
	Double_t fInnerRadiusRatio;
	TArc  *fInnerArc;
public:
	CDisc(Double_t x, Double_t y, Double_t r, Double_t ratio);
	virtual ~CDisc(){};
	void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
	void Paint(const Option_t *opt);
	ClassDef(CDisc, 0)
};

class CreateCDlabel : public TObject
{
private:
	GrCanvas * fCanvas;
	GrCanvas * fImgCanvas;
	GEdit    * fEditor;
	CDisc    * fCDP;
   TImage   *fImage;
   Double_t fImageWidth;
   Double_t fImageHeight;
	Int_t    fRedFac;
	
	Double_t fXRange;
	Double_t fYRange;
	Double_t fInnerRadius;
	Double_t fOuterRadius;
	Int_t    fSmallInnerHole;
	Int_t    fBigInnerHole;
	Int_t    fUserWindowX;
	Int_t    fUserWindowY;
	Double_t fUserRX;
	TString  fProjectName;   // 
	TString	fPsSuffix;
	TString  fBGImage;       // name of background iomage file (gif, jpg..
	TString  fRootFileName;
	TString  fCanvasFromFile;
	Int_t    fLoadFileBid;
	Int_t    fSelectBgIBid;
	Double_t fPaperSizeX;
	Double_t fPaperSizeY;
	Int_t    fDrawCircles;
public:
	CreateCDlabel();
	~CreateCDlabel(){};
	void RecursiveRemove(TObject * obj);
	void Leave();
	void MakePS();
	void SetName();
	void NewProject(Int_t ww, Int_t wh, Double_t xr, Double_t yr);
	void NewCDlabel();
	void NewPortrait();
	void NewLandscape();
	void NewUserDef();
	void SelectBackgroundPicture();
	void CreateBackgroundPicture();
	void InsertBackgroundPicture();
	void CloseProject();
	void ExecuteGetProject();
	void ExecuteStoreProject();
	void CoverDeadArea();
	void SetPaperSizeX(Double_t ps) { fPaperSizeX = ps; };
	Double_t GetPaperSizeX() { return fPaperSizeX; };
	void SetPaperSizeY(Double_t ps) { fPaperSizeY = ps; };
	Double_t GetPaperSizeY() { return fPaperSizeY; };
	void SaveDefaults();
	void RestoreDefaults();
	void CloseDown(Int_t i) {if ((i)){};};
	void CRButtonPressed(Int_t wid, Int_t bid,TObject* obj);
	ClassDef(CreateCDlabel,0)
};

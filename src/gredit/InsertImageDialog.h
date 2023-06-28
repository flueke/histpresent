#ifndef INSERTIMAGEDIALOG
#define INSERTIMAGEDIALOG
#include "TASImage.h"
#include "TString.h"
#include "TList.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________

class TCanvas;
class TRootCanvas;
class TObject;

class InsertImageDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	TCanvas      *fCanvas;
	TPad       * fPad;
	TRootCanvas *fWindow;
	TImage *fImage;
	Int_t fEntirePad;
	Int_t fFix_no;
	Int_t fFix_h;
	Int_t fFix_w;
	Int_t fFix_wh;
	Int_t fOffset_x;
	Int_t fOffset_y;
	Int_t fWidth_x;
	Int_t fWidth_y;
	Double_t fBlur_x;
	Double_t fBlur_y;
	Double_t fImageWidth;
	Double_t fImageHeight;

	TString fPname;
	TString fMergeMode;

public:
	InsertImageDialog();
	virtual ~InsertImageDialog();
	void    ExecuteInsert();
	void    ExecuteMerge();
	void    ExecuteBlur();
	void    ExecuteCrop();
	TImage *FindActiveImage();
	void    ImageToFile();
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/) {};

ClassDef(InsertImageDialog,0)
};
#endif

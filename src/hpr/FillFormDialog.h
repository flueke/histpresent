#ifndef FILLFORMDIALOG
#define FILLFORMDIALOG
#include "TASImage.h"
#include "TString.h"
#include "TList.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________

class HTCanvas;
class TRootCanvas;
class TObject;

class FillFormDialog : public TObject {

private:
	void *fValp[100];
	TList *fRow_lab;
	TGMrbValuesAndText *fDialog;
	HTCanvas     *fHCanvas;
	TPad        *fPad;
	TRootCanvas *fWindow;
	TImage *fImage;
	Double_t fImageWidth;
	Double_t fImageHeight;
	Int_t   fRedFactor;
	TString fBaseName;
	TString fInputName;
	TString fOutputName;
	TString fCommandName;
	Int_t   fBidOpen;
	Int_t   fOutputWritten;
public:
	FillFormDialog();
	virtual ~FillFormDialog();
	void    OpenInput();
	void    WriteOutput();
	void    EditCommandFile();
	void    ComposeFileNames();
	void    SaveDefaults();
	void    RestoreDefaults();
	void    CloseDialog();
	void    CloseDown(Int_t wid);
	void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) ;

ClassDef(FillFormDialog,0)
};
#endif

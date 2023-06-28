#ifndef HPR_HPR
#define HPR_HPR
//#define MAXHIST 10
#include "TROOT.h"
#include "TQObject.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TControlBar.h>
#include <TTree.h>
#include <TList.h>
#include <TSocket.h>
#include "TRootCanvas.h"
#include <iostream>

#include "FitHist.h"
#include "TMrbHelpBrowser.h"
#include "TGMrbValuesAndText.h"
#include <list>

namespace std {} using namespace std;


const Int_t MAXCAN=10;
class FitHist;
class GroupOfHists;

class HistPresent : public TNamed , public TQObject {

friend class FitHist;
friend class GEdit;
friend class HTCanvas;
friend class HandleMenus;
friend class GroupOfHists;

protected:
   TCanvas *cHPr;                 // the main canvas
   HTCanvas *fFileList;                 // the main canvas
   TFile       *fRootFile;        // root file
   TH1 * fCurrentHist;
   TRootCanvas *fRootCanvas;
//   TRootCanvas *lastcanvas;
   TGMrbValuesAndText *fDialogShowTree;
   Bool_t fByTitle;
   Int_t fRebin, fRMethod;
   TVirtualPad *activeFile;
   TVirtualPad *activeHist;
   Float_t fOpfac;
   TString *fLeafCut;
   TString *fGraphCut;
   TString *fCutVarX;
   TString *fCutVarY;
   TString *fExpression;
	Int_t   fUseFileSelFromRun;
	Int_t   fFileSelFromRun;
	Int_t   fFileSelToRun;
   TString fFileSelMask;
   Int_t   fUseFileSelMask;
   TString fHistSelMask;
   Int_t   fUseHistSelMask;
	TString fLeafSelMask;
   Int_t   fUseLeafSelMask;
	TString fCanvasSelMask;
   Int_t   fUseCanvasSelMask;
	Int_t fFileUseRegexp;
	Int_t fHistUseRegexp;
	Int_t fLeafUseRegexp;
	Int_t fCanvasUseRegexp;
	Bool_t fApplyLeafCut;
   Bool_t fUseHist;
   Bool_t fApplyGraphCut;
   Bool_t fApplyExpression;
   TControlBar * fControlBar;
//   TList *fCanvasList;
   TList *fAllWindows;
   TList *fAllFunctions;
   TList *fAllCuts;
   TList *fHistLists;
   TList *fSelectHist;
	TList *fSelectCanvas;
	TList *fHistListList;
   TList *fSelectLeaf;
   TList *fSelectCut;
   TList *fSelectWindow;
   TList *fCmdLine;
   TList *fSelectContour;
   TList *fSelectGraph;
   TList *fAllContours;

   TMrbHelpBrowser * fHelpBrowser;
   Int_t fSeqNumberMany;
	Int_t fSeqNumberGraph;
	Int_t fPageNumber;

   TString  fGraphFile;

   Int_t fRememberTreeHists;
   Int_t fAlwaysFindLimits;
   Int_t fAlwaysRequestLimits;
   Int_t fNtupleVersioning;
   Int_t fNtupleSeqNr;
	Int_t fNtuplePrependFN;
	Int_t fNtuplePrependTN;
	Int_t fAppendSelections;
	Int_t f2dimAsGraph;
	Int_t fWriteLeafMinMaxtoFile;
	Int_t fWriteLeafNamesToFile;
   Style_t fMarkStyle;
   Size_t  fMarkSize;
   Color_t fMarkColor;
   Int_t fRealStack;
   TString * fHelpDir;
   Double_t fLogScaleMin;
   Double_t fLinScaleMin;
   Int_t    fAutoUpdateDelay;

   TButton * fRebin2;
   TButton * fRebin4;
   TButton * fRebinOth;
   TButton * fRebinSumAvg;
   TButton * fValButton;
   TButton * fCloseWindowsButton;
   Bool_t    fCanvasClosing;
   TString * fHostToConnect;
   Bool_t    fConnectedOnce;
   Int_t     fSocketToConnect;
   TSocket * fComSocket;
//   Bool_t  fSocketIsOpen;
   Bool_t  fAnyFromSocket;
   TGWindow * fLastWindow;
	TButton * fHfromM_aButton;
   Int_t         fLabelMaxDigits;

   Color_t       fFuncColor;
   Style_t       fFuncStyle;
   Width_t       fFuncWidth;

   Int_t      fEditUsXlow  ;
   Int_t      fEditUsYlow  ;
   Int_t      fEditUsXwidth;
   Int_t      fEditUsYwidth;
   Double_t   fEditUsXRange;

   Int_t      fEditLsXlow  ;
   Int_t      fEditLsYlow  ;
   Int_t      fEditLsXwidth;
   Int_t      fEditLsYwidth;
   Double_t   fEditLsXRange;

   Int_t      fEditPoXlow  ;
   Int_t      fEditPoYlow  ;
   Int_t      fEditPoXwidth;
   Int_t      fEditPoYwidth;
   Double_t   fEditPoXRange;
	TString 		fEditor;

public:
enum EHfromASCIImode { kNotDefined, kSpectrum, kSpectrumError, k1dimHist,
                       k1dimHistWeight, k2dimHist, k2dimHistWeight,
                       kGraph, kGraphError, kGraphAsymmError,
                       k3dimHist, k3dimHistWeight,};

   HistPresent(const Text_t *name, const Text_t *title);
   HistPresent();
   ~HistPresent();
	void HandleTerminate(Int_t status);
   void RecursiveRemove(TObject *);
   TRootCanvas* GetRootCanvas(){return fRootCanvas;};
//   void  SetMyCanvas(TRootCanvas *myc){lastcanvas = myc;};
   void SaveOptions();
   void RestoreOptions();
   void EditAttrFile();
   void Editrootrc();
   void CloseAllCanvases();
   void CloseHistLists();
   void HandleDeleteCanvas(HTCanvas * htc);
   void SetRebinValue(Int_t);               //
   void SetRebinMethod();               //
   void DiffHist();                 //
   void RebinHist();               //
   void OperateHist(Int_t);               //
   void SetOperateVal();               //
   void ListSelect();
   void SetShowTreeOptions(TGWindow * win = 0);
   void SetShowTreeOptionsCint(const char *pointer = 0);

   void CRButtonPressed(Int_t, Int_t, TObject*);
   void CloseDown(Int_t wid);
//   void CheckAutoExecFiles();
   void ClearSelect();
   void ShowMain();               //
//   Bool_t IsSelected(const char * name);
   void ListMacros(const char* bp =0);               //
   void GetFileSelMask(const char* bp =0);               //
   void GetHistSelMask(const char* bp =0);               //
//   void SetHistSelMask();               //
   void SelectCut(const char* , const char*, const char* bp =0);
   void SelectContour(const char* , const char*, const char* bp =0);
   void SelectGraph(const char*  , const char* dir, const char*, const char* bp =0);
   void SelectLeaf(const char*, const char* bp =0);
   void EditLeafCut(const char* vl =0, const char* bp = 0);
   void ToggleLeafCut(const char* bp = 0);
   void UseHist(const char* bp =0);
   void ToggleUseHist(const char* bp = 0);
//   void DefineGraphCut(const char* bp =0);
//   void ToggleGraphCut(const char* bp =0);
   void EditExpression(const char* vl =0, const char* bp =0);
   void ToggleExpression(const char* bp =0);
   void ShowFiles(const char* , const char* bp =0);               //
   void SelectFromOtherDir();
	void SelectdCache();
	void ShowContents(const char* fname, const char* dir = 0,const char* bp =0);    //
   void ShowFunction(const char*, const char*, const char*, const char* bp =0);
   void LoadFunction(const char*, const char*, const char*, const char* bp =0);
   void ShowCanvas(const char* , const char*, const char*, const char* bp =0);
   void ShowContour(const char*, const char*, const char*, const char* bp =0);
   void ShowGraph(const char*, const char* , const char*, const char* bp =0);
	void HistsFromProof(const char* bp = NULL);
   void ComposeList(const char* bp =0);
   void ShowList(const char* , const char*, const char* bp =0);
   void PrintCut(const char* , const char*, const char* bp =0);
   void LoadCut(const char* , const char*, const char* bp =0);
   void CutsToASCII(const char *, const char* bp =0);
   void PrintWindow(const char* , const char*, const char* bp =0);
   void LoadWindow(const char* , const char*, const char* bp =0);
   void ShowHist(const char * fname , const char* dir, const char  * hname, const char* bp =0);
   void SelectHist(const char*  fname , const char* dir , const char * hname, const char* bp =0);
	void SelectCanvas(const char*  fname , const char* dir , const char * hname, const char* bp =0);
	void ShowStatOfAll(const char* , const char* , const char* bp =0);
   void PurgeEntries(const char* , const char* bp =0);
   void DeleteSelectedEntries(const char* , const char* bp =0);
   void ShowSelectedHists(const char* bp =0);
   void ShowSelectedHists(TList *, const char* title =0);
   void StackSelectedHists(const char* bp =0);
//	void StackSelectedHistsScaled(const char* bp =0);
	void StackSelectedHists(TList *, const char* title =0);
   void ShowInOneCanvas(const char* bp =0);
   void StackInOneCanvas(const char* bp =0);
   void ShowTree(const char*, const char*, const char*, const char* bp =0);
   void ShowLeaf(const char*, const char*, const char*, const char* l=0, const char* bp =0);  // display hist of leaf
   void MkClass(const char*, const char*, const char*, const char* l=0, const char* bp =0);  // display hist of leaf
   void HandleRemoveAllCuts();
   void CutsFromASCII(TGWindow * win = 0);
   void HistFromASCII(TGWindow * win = 0);
   void GraphFromASCII(TGWindow * win = 0);
   void Graph2DFromASCII(TGWindow * win = 0);
   void NtupleFromASCII(TGWindow * win = 0);
   void SaveFromSocket(const char*, const char* bp =0);
   TH1*  GetHist(const char*, const char*, const char*);
   FitHist * ShowHist(TH1*, const char* origname=NULL, TButton *b = NULL);
//   TList* GetCanvasList(){return fCanvasList;};
   TList* GetHistList(){return fHistLists;};
   TList* GetSelectedHist(){return fSelectHist;};
	TList* GetSelectedCanvas(){return fSelectCanvas;};
	TList* GetWindowList(){return fAllWindows;};
   TList* GetFunctionList(){return fAllFunctions;};
   TList* GetCutList(){return fAllCuts;};
   TList* GetGraphList(){return fSelectGraph;};
	TH1*   GetSelHistAt(Int_t pos = 0, TList * hl = NULL, Bool_t try_memory = kFALSE,
							  const char * hsuffix = NULL);
   TObject* GetSelGraphAt(Int_t pos = 0);
   void TurnButtonGreen(TVirtualPad **);
   TList* GetSelections(){return fSelectHist;};

   TH1 *  GetCurrentHist(){return fCurrentHist;};
   void   SetCurrentHist(TH1 * hist){fCurrentHist = hist;};
   TMrbHelpBrowser * GetHelpBrowser(){return fHelpBrowser;};
   void WarnBox(const char *);
   void CleanWindowLists(TH1* hist);
//   void RemoveFromLists(TObject * obj);
   void DinA4Page(Int_t form);
   Int_t GetWindowPosition(Int_t * winx, Int_t * winy);
   void SuperimposeGraph(TCanvas * = NULL, Int_t mode = 0);

ClassDef(HistPresent,0)      // A histogram presenter
};

R__EXTERN HistPresent *gHpr;
R__EXTERN Int_t gHprDebug;
R__EXTERN Int_t gHprClosing;
R__EXTERN TString gHprWorkDir;
R__EXTERN TString gHprLocalEnv;
;
#endif

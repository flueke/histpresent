#include "TROOT.h"
//#include "TApplication.h"
//#include "TWbox.h"
//#include "TButton.h"
//#include "TGClient.h"
//#include "TGuiFactory.h"
#include "TRootHelpDialog.h"
#include "TGMenu.h"
#include "TGWindow.h"
#include "TStyle.h"
#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TList.h"
#include "TMarker.h"
#include "TMath.h"
#include "TImage.h"
//#include "TError.h"
#include "TContextMenu.h"
#include "TRegexp.h"

#include "GEdit.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "support.h"
#include "SetColor.h"
#include "TGMrbInputDialog.h"
#include "EmptyHistDialog.h"
#include "SetHistOptDialog.h"
#include "Set1DimOptDialog.h"
#include "Set2DimOptDialog.h"
#include "Set2DimGraphDialog.h"
#include "Set3DimOptDialog.h"
#include "SetColorModeDialog.h"
#include "SetCanvasAttDialog.h"
#include "WhatToShowDialog.h"
#include "WindowSizeDialog.h"
#include "FontSelectDialog.h"
#include "GeneralAttDialog.h"
#include "GraphAttDialog.h"
#include "HprTh3Dialog.h"
#include "Rebin2DimDialog.h"
#include "FillFormDialog.h"
#include "hprbase.h"
#include "ShiftScale.h"

extern Int_t gHprDebug;
extern Int_t gHprClosing;

void EditFitMacroG(TGWindow * win);
void ExecFitMacroG(TGraph * graph, TGWindow * win);
void ExecGausFitG(TGraph * graph, Int_t type);


enum ERootCanvasCommands {
	kFileNewCanvas,
	kFileOpen,
	kFileSaveAs,
	kFileSaveAsRoot,
	kFileSaveAsC,
	kFileSaveAsPS,
	kFileSaveAsEPS,
	kFileSaveAsTEX,
	kFileSaveAsPDF,
	kFileSaveAsGIF,
	kFileSaveAsJPG,
	kFilePrint,
	kFileCloseCanvas,
	kFileQuit,

	kEditEditor,
	kEditUndo,
	kEditClearPad,
	kEditClearCanvas,

	kViewEventStatus,
	kViewColors,
	kViewFonts,
	kViewMarkers,
	kViewFillStyles,
	kViewLineStyles,
	kViewIconify,
	kViewX3D,
	kViewOpenGL,
	kInterrupt,

	kOptionEventStatus,
	kOptionAutoExec,
	kOptionAutoResize,
	kOptionResizeCanvas,
	kOptionMoveOpaque,
	kOptionResizeOpaque,
	kOptionRefresh,
	kOptionStatistics,
	kOptionHistTitle,
	kOptionFitParams,
	kOptionCanEdit,

	kInspectRoot,
	kInspectBrowser,

	kClassesTree,

	kHelpAbout,
	kHelpOnCanvas,
	kHelpOnMenus,
	kHelpOnGraphicsEd,
	kHelpOnBrowser,
	kHelpOnObjects,
	kHelpOnPS,

//   Ottos stuff

//   kOptionFit,
	kOptionDisp,
	kOption1Dim,
	kOption2Dim,
	kOption2DimGraph,
	kOption3Dim,
	kOption2DimCol,
	kOptionGraph,
	kOptionHpr,
	kOptionResetAll,
	kOptionWin,
	kOptionFont,
	kOptionTree,
	kFHMarks,
	kFHMarkon,
	kFHMarkoff,
	kFHClearMarks,
	kFHPrintMarks,
	kFHDistMarks,
	kFHSet2Marks,
	kFHColorMarked,
	kFHSelectInside,
	kFHDisplayS2FD,
	kFHKeepPar,
	kFHCallMinos,

	kFHDisplay,
	kFHExpand,
	kFHEntire,
	kFHAxisRange,
	kFHXaxisRange,
	kFHYaxisRange,
	kFHZaxisRange,
	kFHMagnify,
	kFHEnlarge,
	kFHSuperimpose,
	kFHTh3Dialog,
	kFHRebinDimDialog,
	kFHSuperimposeGraph,
	kFHSuperimposeGraphScale,
	kFHSuperimposeGraph2Dim,
	kFHSetAxisGraphX,
	kFHSetAxisGraphY,
	kFHKolmogorov,
	kFHfft,
	kFHSuperimposeScale,
	kFHGetRange,
//   kFHSetDrawMode,
//   kFHSetLabAtt,
	kFHLogY,
	kFHRotateClock,
	kFHRotateCClock,
	kFHTranspose,
	kFHTransposeInv,
	kFHProfileX,
	kFHProfileY,
	kFHProjectX,
	kFHProjectX_func,
	kFHProjectY,
	kFHProjectF,
	kFHLiveSliceX,
	kFHLiveSliceY,
	kFHProjectB,
	kFHProjectOnAnyAxis,
	kFHStackMax,
	kFHStackMin,
	kFHStack,
	kFHCanvas2LP,
	kFHRebinOne,
	kFHUserCont,
	kFHUserContClear,
	kFHUserContSave,
	kFHUserContUse,
//   kFHProjectY,
	kFHOutputStat,
	kFHContHist,
	kFHHistToFile,
	kFHGraphToFile,
	kFHGraphToASCII,
	kFHSelAnyDir,
	kFHSeldCache,
	kFHCanvasToFile,
	kFHAsTCanvasToFile,
	kFHHistToASCII,
	kFHHistToGraph,
	kFHPictToPS,
	kFHPictToLP,

	kFHASCIIToHist,
	kFHNtuple,
	kFHGraph,
	kFHGraph2D,
	kFHEmptyHist,

	kFHCut,
	kFHInitCut,
	kFHClearCut,
	kFHRemoveAllCuts,
	kFHListCuts,
	kFHListWindows,
	kFHDrawCut,
	kFHDrawCutName,
	kFHMarksToCut,
	kFHClearRegion,
	kFHWriteOutCut,
	kFHTerminate,

	kFHFit,
	kFHFitGausLBg,
	kFHFit2DimGaus1,
	kFHFit2DimGaus2,
	kFHFit2DimGaus3,
	kFHFit2DimGaus4,
	kFHFit2DimGaus5,
	kFHFitExp,
	kFHFitLandau,
	kFHFitPol,
	kFHFitForm,
	kFHFillRandom,
	kFHFitUserG,
	kFHGausFitG,
	kFHExpFitG,
	kFHPolFitG,
	kFHFormFitG,
	kFHEditUserG,
	kFHFitUser,
	kFHEditUser,
	kFHEditSlicesYUser,
	kFHFitSlicesYUser,
	kFHMacroBreitW,
	kFHMacroExpG,
	kFHMacroLandau,
	kFHMacroTwoG,
	kFHFuncsToHist,
	kFHWriteFunc,
//   kFHWriteFuncList,
	kFHFitPolyHist,
	kFHFitPolyMarks,
	kFHFindPeaks,
	kFHCalibrate,
	kFHShiftScale,
	kFHCalDialog,
	kFHCalibrateNew,
	kFHDeleteCal,
	kFHShowPeaks,
	kFHSetRangeAxis,
	kFHRedefineAxis,
	kFHRestoreAxis,
	kFHAddAxisX,
	kFHAddAxisY,
	kFHReDoAxis,
	kFHMarksToWindow,
	kFHDrawWindows,
	kFHDrawFunctions,
	kFHSubtractFunction,
	kFHClearWindows,
	kFHWindowsToHist,
	kFHWindowsFromHist,
	kFHCutsToHist,
	kFHCutsFromHist,
	kFHWriteOutWindows,
	kFHCutsFromASCII,
	kFH_CASCADE1_0,
	kFH_CASCADE1_1,
	kFH_CASCADE1_2,
	kFH_CASCADE1_3,
	kFH_CASCADE1_4,
	kFH_CASCADE1_5,
	kFH_CASCADE1_6,
	kFH_CASCADE1_7,
	kFH_CASCADE1_8,
	kFH_CASCADE1_U,
	kFH_CASCADE1_H,
 	kFH_CASCADE2_0,
	kFH_CASCADE2_1,
	kFH_CASCADE2_2,
	kFH_CASCADE2_3,
	kFH_CASCADE2_4,
	kFH_CASCADE2_5,
	kFH_CASCADE2_6,
	kFH_CASCADE2_7,
	kFH_CASCADE2_8,
	kFH_CASCADE2_U,
	kFH_Portrait,
	kFH_UserEdit,
	kFH_FillForm,
	kFH_Landscape,
	kFH_WritePrim,
	kFH_DrawHist,
	kFH_DrawGraph,
	kFH_InsertImage,
	kFH_InsertAxis,
	kFH_InsertLatex,
	kFH_InsertLatexF,
	kFH_SetGrid,
	kFH_UseGrid,
	kFH_DrawGrid,
	kFH_DrawGridVis,
	kFH_RemoveGrid,
	kFH_RemoveCGraph,
	kFH_DrawCGraph,
	kFH_SetVisEnclosingCut,
	kFH_PutObjectsOnGrid,
	kFH_ExtractGObjects,
	kFH_MarkGObjects,
	kFH_InsertGObjectsG,
	kFH_InsertGObjects,
	kFH_WriteGObjects,
	kFH_DeleteObjects,
	kFH_ReadGObjects,
	kFH_ShowGallery,
	kOptionPad,
	kOptionHist,
	kOptionUseAttrFile,
	kOptionUseCurrentAttr,
	kOptionUseAttrFileHelp,
	kFH_NoAction
};
static const char *gOpenTypes[] = { "ROOT files",   "*.root",
												"All files",    "*",
												0,              0 };

static const char *gSaveAsTypes[] = { "PostScript",   "*.ps",
												  "Encapsulated PostScript", "*.eps",
												  "PDF",          "*.pdf",
												  "TeX",          "*.tex",
												  "SVG",          "*.svg",
												  "GIF",          "*.gif",
												  "ROOT macros",  "*.C",
												  "ROOT files",   "*.root",
												  "XML",          "*.xml",
												  "PNG",          "*.png",
												  "XPM",          "*.xpm",
												  "JPEG",         "*.jpg",
												  "TIFF",         "*.tiff",
												  "All files",    "*",
												  0,              0 };

ClassImp (HandleMenus)

HandleMenus::HandleMenus(HTCanvas * c, HistPresent * hpr, FitHist * fh, TObject *obj)
				  : TGFrame(gClient->GetRoot(), 10 ,10),
					 fHCanvas(c),fHistPresent(hpr), fFitHist(fh), fObject(obj)
{
	if ( fHCanvas->IsBatch() ) {
		cout << setred << "Cannot open canvas, did you forget option -X with ssh "
		<< endl << "HistPresent exits now" << setblack << endl;
		exit(-1);
	}
	fRootCanvas = (TRootCanvas*)fHCanvas->GetCanvasImp();
	fRootsMenuBar = fRootCanvas->GetMenuBar();
	fMenuBarItemLayout = fRootCanvas->GetMenuBarItemLayout();
	fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
	fEditor = NULL;
	fHistInPad = NULL;
	fGraph1D = NULL;
	fGraph2D = NULL;
	if (obj && obj->InheritsFrom("TGraph"))
		fGraph1D =(TGraph*)obj;
	if (obj && obj->InheritsFrom("TGraph2D"))
		fGraph2D =(TGraph2D*)obj;
	
	//   MapWindow();
//   UnMapWindow();
};

//______________________________________________________________________________
HandleMenus::~HandleMenus(){};

//______________________________________________________________________________
Bool_t HandleMenus::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
	// Handle menu and other command generated by the user.

//  find the corresponding FitHist Object
//   cout << " enter HandleMenus::ProcessMessage" << endl;
	TMrbHelpBrowser * hbrowser = NULL;
	if(fHistPresent){
		hbrowser=fHistPresent->GetHelpBrowser();
	}
	switch (GET_MSG(msg)) {

		case kC_COMMAND:

			switch (GET_SUBMSG(msg)) {

				case kCM_BUTTON:
				case kCM_MENU:

					if(parm1 >= 100000){
						Int_t ind = parm1 - 100000;
						if( hbrowser ) {
							hbrowser->SetOrigin(WindowSizeDialog::fMainWidth, 20);
							if ( ind >= 0 && ind < hbrowser->GetNofItems() )
								hbrowser->DrawText(ind);
						}
						break;
					}
					switch (parm1) {
						// Handle File menu items...
						case kFileNewCanvas:
//                     gROOT->GetMakeDefCanvas();
							break;
						case kFileOpen:
							{
								TGFileInfo fi;
								fi.fFileTypes = (const char **) gOpenTypes;
								new TGFileDialog(fRootCanvas->GetParent(), fRootCanvas, kFDOpen,&fi);
								if (!fi.fFilename) return kTRUE;
								new TFile(fi.fFilename, "update");
								delete [] fi.fFilename;
							}
							break;
						case kFileSaveAs:
							{
								TString CurDir;
								CurDir = gSystem->pwd();
								static TString dir(".");
								static Int_t typeidx = 0;
								TGFileInfo fi;
								fi.fFileTypes   = gSaveAsTypes;
								fi.fIniDir      = StrDup(dir);
								fi.fFileTypeIdx = typeidx;
								new TGFileDialog(fClient->GetDefaultRoot(), fRootCanvas, kFDSave, &fi);
								if (!fi.fFilename) return kTRUE;
								Bool_t  appendedType = kFALSE;
								TString fn = fi.fFilename;
								TString ft = fi.fFileTypes[fi.fFileTypeIdx+1];
								dir     = fi.fIniDir;
								typeidx = fi.fFileTypeIdx;
again:
								if (fn.EndsWith(".root") ||
									 fn.EndsWith(".ps")   ||
									 fn.EndsWith(".eps")  ||
									 fn.EndsWith(".pdf")  ||
									 fn.EndsWith(".tex")  ||
									 fn.EndsWith(".svg")  ||
									 fn.EndsWith(".gif")  ||
									 fn.EndsWith(".xml")  ||
									 fn.EndsWith(".xpm")  ||
									 fn.EndsWith(".jpg")  ||
									 fn.EndsWith(".png")  ||
									 fn.EndsWith(".tiff")) {
//                           fHCanvas->RemoveEditGrid();
									fHCanvas->cd();
									gSystem->Sleep(60); // give X server time to refresh
									fHCanvas->SaveAs(fn);
									if ( fn.EndsWith(".tex")) {
										Hpr::ReplaceUS(fn);
									}
								} else if (fn.EndsWith(".C"))
									fHCanvas->SaveSource(fn);
								else {
									if (!appendedType) {
										if (ft.Index(".") != kNPOS) {
											fn += ft(ft.Index("."), ft.Length());
											appendedType = kTRUE;
											goto again;
										}
									}
									Warning("ProcessMessage", "file %s cannot be saved with this extension", fi.fFilename);
								}
								gSystem->cd(CurDir);
								cout << "Restore dir: " << CurDir << endl;
							}
							break;
						case kFileSaveAsRoot:
							fHCanvas->SaveAs(".root");
							break;
						case kFileSaveAsC:
							{
							TString nname = fHCanvas->GetName();
							nname += "_save.C";
							fHCanvas->SaveSource(nname);
							}
							break;
						case kFileSaveAsPS:
//                    fHCanvas->SetFillStyle(0);
						  fHCanvas->SaveAs();
							break;
						case kFileSaveAsEPS:
							fHCanvas->SaveAs(".eps");
							break;
						case kFileSaveAsTEX:
							{
							TString fname(fHCanvas->GetName());
							if (fname.BeginsWith("C_F")) {
								fname = fname(3,fname.Length());
							}
							fname+=".tex";
							fHCanvas->SaveAs(fname);
							Int_t latex_header = 1;
							Hpr::ReplaceUS(fname, latex_header);
							cout << "Canvas saved as: " << fname << endl;
							}
							break;
						case kFileSaveAsPDF:
							fHCanvas->SaveAs(".pdf");
							break;
						case kFileSaveAsGIF:
//                    fHCanvas->SetFillStyle(0);
							fHCanvas->SaveAs(".gif");
							break;
						case kFileSaveAsJPG:
							fHCanvas->SaveAs(".jpg");
							break;
						case kFilePrint:
						  fHCanvas->Print();
//                    fHCanvas->SetFillStyle(0);
							break;
						case kFileCloseCanvas:
//                     cout << "TVirtualPadEditor::GetPadEditor(kFALSE) " <<TVirtualPadEditor::GetPadEditor(kFALSE) << endl;
//                     if ( (TVirtualPadEditor::GetPadEditor(kFALSE) != 0) )
//                        TVirtualPadEditor::Terminate();
							fHCanvas->Disconnect(((TPad*)fHCanvas), "Modified()");
							fRootCanvas->ShowEditor(kFALSE);

							fRootCanvas->SendCloseMessage();
							break;
						case kFHCutsFromASCII:
							{
//                     HistPresent *fHistPresent = (HistPresent*)gROOT->FindObject("mypres");
							if(fHistPresent)fHistPresent->CutsFromASCII(fRootCanvas);
							}
							break;
						case kFileQuit:
							{
							Int_t buttons= kMBYes | kMBNo, retval=0;
							EMsgBoxIcon icontype = kMBIconStop;
							new TGMsgBox(gClient->GetRoot(), fRootCanvas,
							 "Do you really want to quit?",
							  "Do you really want to quit?",
					//       gClient->GetPicture("/home/rg/schaileo/myroot/xpm/rquit.xpm"),
							 icontype, buttons, &retval);
							if(retval == kMBYes){
								gHprClosing = 1;
								if( fHistPresent ) {
//									fHistPresent->CloseAllCanvases();
//									fHistPresent->SaveOptions();
									if (fHistPresent->cHPr) {
										cout << endl << setblue << "Deleting main window cHpr " 
										<< fHistPresent->cHPr << setblack << endl;
										delete fHistPresent->cHPr;
										fHistPresent->cHPr = NULL;
									}
								}
								gApplication->Terminate(0);
							}
							}
							break;

						case kFHStackMax:
							{
							THStack * st = (THStack *)fHCanvas->GetListOfPrimitives()
															  ->FindObject("hstack");
							if (st) {
								fHCanvas->cd();
								Bool_t ok;
								Double_t val = GetFloat("Yscale Max Val",st->GetMaximum(),
																 &ok, fRootCanvas);
								if (ok) {
									st->SetMaximum(val);
									fHCanvas->Modified();
									fHCanvas->Update();
								}
							}
							}
							break;

						case kFHStackMin:
							{
							THStack * st = (THStack *)fHCanvas->GetListOfPrimitives()
															  ->FindObject("hstack");
							if (st) {
								fHCanvas->cd();
								Bool_t ok;
								Double_t val = GetFloat("Yscale Min Val",st->GetMinimum(),
																 &ok, fRootCanvas);
								if (ok) {
									st->SetMinimum(val);
									fHCanvas->Modified();
									fHCanvas->Update();
								}
							}
							}
							break;
						case kFHStack:
							{
							THStack * st = (THStack *)fHCanvas->GetListOfPrimitives()
															  ->FindObject("hstack");
							fHCanvas->cd();
							if (fHistPresent->fRealStack) {
								fHistPresent->fRealStack = 0;
								fDisplayMenu->UnCheckEntry(kFHStack);
								if (st) {
									st->SetDrawOption("nostack");
//                           cout << "nostack" << endl;
								}
							} else {
								fHistPresent->fRealStack = 1;
								fDisplayMenu->CheckEntry(kFHStack);
								if (st) {
								  st->SetDrawOption("");
 //                         cout << "stack" << endl;
								}
							}
							fHCanvas->Modified();
							fHCanvas->Update();
							}
							break;

						case kFHCanvas2LP:
							Canvas2LP((TCanvas*)fHCanvas, kTRUE, fRootCanvas);
							break;
						case kFHPictToPS:
							Canvas2LP((TCanvas*)fHCanvas, kFALSE, fRootCanvas);
							break;

						// Handle Edit menu items...
						case kEditEditor:
//                     fHCanvas->EditorBar();
							  if (fHCanvas->GetAutoExec()) fHCanvas->ToggleAutoExec();
							  fRootCanvas->ShowEditor();
//                       fRootCanvas->ShowToolBar();
//                       fHCanvas->SetBit(HTCanvas::kIsAEditorPage);
							  if ( !fEditor ) {
								  fEditor = new GEdit(fHCanvas);
							  }
//                     if (!fEditor) CreateEditor();
							break;
						case kEditUndo:
							// noop
							break;
						case kEditClearPad:
							gPad->Clear();
							gPad->Modified();
							gPad->Update();
							break;
						case kEditClearCanvas:
							fHCanvas->Clear();
							fHCanvas->Modified();
							fHCanvas->Update();
							break;
						case kOptionRefresh:
							fHCanvas->Paint();
							fHCanvas->Update();
							break;
						case    kFH_Portrait:
							if(fHistPresent)
								fHistPresent->DinA4Page(0);
							break;
						case    kFH_Landscape:
							if(fHistPresent)
								fHistPresent->DinA4Page(1);
							break;
						case    kFH_UserEdit:
							if(fHistPresent)
								fHistPresent->DinA4Page(2);
							break;
						case    kFH_FillForm:
								new FillFormDialog();
							break;

						// Handle View menu items...
						case kViewEventStatus:
							fHCanvas->ToggleEventStatus();
//                     fRootCanvas->ShowStatusBar(fHCanvas->GetShowEventStatus());
							break;
						case kViewOpenGL:
							{
							if ( gPad )
								if ( gPad->GetView() == NULL ) {
									cout << "Not applicable for this view" << endl;
								} else {
									gPad->GetViewer3D("ogl");
								}
							}
							break;
						case kViewColors:
							DrawColors();
							break;
						 case kViewFillStyles:
							DrawFillStyles();
							break;
						 case kViewLineStyles:
							DrawLineStyles();
							break;
					  case kViewFonts:
							Show_Fonts();
							break;
						case kViewMarkers:
							{
								TVirtualPad *padsav = gPad->GetCanvas();
								TCanvas *m = new TCanvas("markers","Marker Types",600,200);
								TMarker::DisplayMarkerTypes();
								m->Update();
								padsav->cd();
							}
							break;
						case kOptionDisp:
							{
							new WhatToShowDialog(fRootCanvas);
							}
							break;
						case kOption1Dim:
							{
							new Set1DimOptDialog(fRootCanvas);
							}
							break;
						case kOption2Dim:
							{
							new Set2DimOptDialog(fRootCanvas);
							}
							break;
						case kOption2DimGraph:
							{
							new Set2DimGraphDialog(fRootCanvas);
							}
							break;
						case kOption3Dim:
							{
							TButton *b = 0;
							if ( fFitHist )
								b=fFitHist->GetCmdButton();
							new Set3DimOptDialog(fRootCanvas, b);
							}
							break;
						case kOption2DimCol:
							{
							new SetColorModeDialog(fRootCanvas);
							}
							break;
						case kOptionGraph:
							{
							new GraphAttDialog(fRootCanvas);
							}
							break;
						case kOptionHpr:
							{
							new GeneralAttDialog(fRootCanvas);
							}
							break;
						case kOptionWin:
							{
							new WindowSizeDialog(fRootCanvas);
							}
							break;
						case kOptionFont:
							{
							new FontSelectDialog(fRootCanvas);
							}
							break;
						case kOptionTree:
						{
							if ( gHpr )
								gHpr->SetShowTreeOptions(fRootCanvas);
						}
						break;
						case kOptionResetAll:
							{
							SetHistOptDialog * shod = new SetHistOptDialog(1);
							shod->RestoreDefaults(1);  // force defaults
							shod->SaveDefaults();
							shod->SetDefaults();
							delete shod;
							SetCanvasAttDialog * scad = new SetCanvasAttDialog(1);
							scad->RestoreDefaults(1);  // force defaults
							scad->SaveDefaults();
							scad->SetDefaults();
							delete scad;
							}
							break;
						case kOptionPad:
							{
								new SetCanvasAttDialog(fRootCanvas);
							}
							break;
						case kOptionHist:
							{
							new SetHistOptDialog(fRootCanvas);
							}
							break;
						case kOptionUseAttrFile:
							if(gROOT->GetForceStyle()) {
								gROOT->ForceStyle(kFALSE) ;
								fAttrMenu->CheckEntry(kOptionUseAttrFile);
								fAttrMenu->UnCheckEntry(kOptionUseCurrentAttr);
							} else {
								gROOT->ForceStyle(kTRUE) ;
								fAttrMenu->UnCheckEntry(kOptionUseAttrFile);
								fAttrMenu->CheckEntry(kOptionUseCurrentAttr);
							}
							break;
						case kOptionUseCurrentAttr:
							if(gROOT->GetForceStyle()) {
								gROOT->ForceStyle(kFALSE) ;
								fAttrMenu->CheckEntry(kOptionUseCurrentAttr);
								fAttrMenu->UnCheckEntry(kOptionUseAttrFile);
							} else {
								gROOT->ForceStyle(kTRUE) ;
								fAttrMenu->UnCheckEntry(kOptionUseCurrentAttr);
								fAttrMenu->CheckEntry(kOptionUseAttrFile);
							}
							break;
						case kOptionUseAttrFileHelp:
							UseAttrFileHelp();
							break;
						case kFHClearMarks:
							fFitHist->ClearMarks();
							break;
						case kFHPrintMarks:
							fFitHist->PrintMarks();
							break;
						case kFHDistMarks:
							fFitHist->DistBetween2Marks();
							break;
							
						case kFHSet2Marks:
							fFitHist->Set2Marks();
							break;
						case kFHColorMarked:
							fFitHist->ColorMarked();
							break;
						case kFHSelectInside:
							if(fFitHist->InsideState()) {
								 fFitHist->SetInside(kFALSE);
								 fCutsMenu->UnCheckEntry(kFHSelectInside);
							} else {
								 fFitHist->SetInside(kTRUE);
								 fCutsMenu->CheckEntry(kFHSelectInside);
							}
							break;
						case kFHDisplayS2FD:
							if(fFitHist->GetAutoDisplayS2FD()) {
								 fFitHist->SetAutoDisplayS2FD(kFALSE);
								 fFitMenu->UnCheckEntry(kFHDisplayS2FD);
							} else {
								 fFitHist->SetAutoDisplayS2FD(kTRUE);
								 fFitMenu->CheckEntry(kFHDisplayS2FD);
							}
							break;
						case  kFHMarksToWindow:
							fFitHist->MarksToWindow();
							break;
						case  kFHDrawWindows:
							fFitHist->DrawWindows();
							break;
						case  kFHDrawFunctions:
							fFitHist->DrawSelectedFunctions();
							break;
						case  kFHSubtractFunction:
							fFitHist->SubtractFunction();
							break;
						case  kFHWindowsToHist:
							fFitHist->AddWindowsToHist();
							break;
						case  kFHWindowsFromHist:
							fFitHist->RemoveWindowsFromHist();
							break;
						case  kFHClearWindows:
							fFitHist->ClearWindows();
							break;
						case  kFHWriteOutWindows:
							fFitHist->WriteOutWindows();
							break;
						case kFHExpand:
							fFitHist->Expand();
							break;
						case kFHEntire:
							fFitHist->Entire();
							break;
						case kFHXaxisRange:
							fFitHist->SetXaxisRange();
							break;
						case kFHYaxisRange:
							fFitHist->SetYaxisRange();
							break;
						case kFHZaxisRange:
							fFitHist->SetZaxisRange();
							break;
						case kFHRebinOne:
							fFitHist->RebinOne();
							break;
						case kFHUserContUse:
							fFitHist->UseSelectedContour();
							break;
						case kFHUserCont:
							fFitHist->SetUserContours();
							break;
						case kFHUserContClear:
							fFitHist->ClearUserContours();
							break;
						case kFHUserContSave:
							fFitHist->SaveUserContours();
							break;
						case kFHMagnify:
							fFitHist->Magnify();
							break;
						case kFHEnlarge:
							fFitHist->Enlarge();
							break;
						case kFHTh3Dialog:
							new HprTh3Dialog((TH3*)fFitHist->GetSelHist(), fRootCanvas);
							break;
						case kFHRebinDimDialog:
							new Rebin2DimDialog((TH2*)fFitHist->GetSelHist(), fRootCanvas);
							break;
						case kFHSuperimpose:
							fFitHist->SuperImpose(0);
							break;
						case kFHSuperimposeScale:
							fFitHist->SuperImpose(1);
							break;
						case kFHSuperimposeGraph:
							fHistPresent->SuperimposeGraph((TCanvas*)fHCanvas, 0);
							break;
						case kFHSuperimposeGraph2Dim:
							Hpr::SuperimposeGraph2Dim((TCanvas*)fHCanvas);
							break;
						case kFHSuperimposeGraphScale:
							fHistPresent->SuperimposeGraph((TCanvas*)fHCanvas, 1);
							break;
						case kFHSetAxisGraphX:
							SetAxisGraphX((TCanvas*)fHCanvas, fGraph1D);
							break;
						case kFHSetAxisGraphY:
							SetAxisGraphY((TCanvas*)fHCanvas, fGraph1D);
							break;
						case kFHKolmogorov:
							fFitHist->KolmogorovTest();
							break;
						case kFHfft:
							fFitHist->FastFT();
							break;
						case kFHGetRange:
							fFitHist->GetRange();
							break;
						case kFHLogY:
							if ( fSelHist && fSelHist->GetDimension() == 2) {
								if (fHCanvas->GetLogz()) {
									 fFitHist->SetLogz(0);
									 fDisplayMenu->UnCheckEntry(kFHLogY);
								} else {
									 fFitHist->SetLogz(1);
									 fDisplayMenu->CheckEntry(kFHLogY);
								}
							} else {
								if (fHCanvas->GetLogy()) {
									 fFitHist->SetLogy(0);
									 fDisplayMenu->UnCheckEntry(kFHLogY);
								} else {
									 fFitHist->SetLogy(1);
									 fDisplayMenu->CheckEntry(kFHLogY);
								}
							}
							fFitHist->SaveDefaults();
							fFitHist->GetCanvas()->Modified(kTRUE);
							fFitHist->GetCanvas()->Update();
							break;
						case kFHRotateClock:
							fFitHist->Rotate(0);
							break;
						case kFHRotateCClock:
							fFitHist->Rotate(1);
							break;
						case kFHTranspose:
							fFitHist->Transpose(0);
							break;
						case kFHTransposeInv:
							fFitHist->Transpose(1);
							break;
						case kFHProfileX:
							fFitHist->ProfileX();
							break;
						case kFHProfileY:
							fFitHist->ProfileY();
							break;
						case kFHProjectX:
							fFitHist->ProjectX();
							break;
						case kFHProjectX_func:
							fFitHist->ProjectX_Func();
							break;
						case kFHProjectY:

							fFitHist->ProjectY();
							break;
						case kFHProjectF:
							fFitHist->ProjectF();
							break;
						case kFHProjectB:
							fFitHist->ProjectBoth();
							break;
						case kFHProjectOnAnyAxis:
							fFitHist->ProjectOnAnyAxis();
							break;
						case kFHLiveSliceX:
							{
							TEnv env(".hprrc");
							if (fNbinLiveSliceX) {
								env.SetValue("Set2DimOptDialog.fNbinLiveSliceX", 0);
								env.SaveLevel(kEnvLocal);
								fDisplayMenu->UnCheckEntry(kFHLiveSliceX);
								fNbinLiveSliceX = 0;
								TCanvas * canpro = Hpr::FindCanvas("_projection_");
								if ( canpro ) {
									TString cname = canpro->GetName();
									// hide canvas from root
									if (cname.EndsWith("1")) {
										TRegexp pr("_projection_");
										cname(pr) = "_Projection_";
										canpro->SetName(cname);
									}
								}
							} else {
								Int_t nb= env.GetValue("Set2DimOptDialog.fNbinSliceX", 1);
								env.SetValue("Set2DimOptDialog.fNbinLiveSliceX", nb);
								env.SaveLevel(kEnvLocal);
								fDisplayMenu->CheckEntry(kFHLiveSliceX);
								fNbinLiveSliceX = 1;
							}
							
//							((TH2*)fSelHist)->SetShowProjectionX(fNbinLiveSliceX);
							break;
							}
						 case kFHLiveSliceY:
							{
							TEnv env(".hprrc");
							if (fNbinLiveSliceY) {
								env.SetValue("Set2DimOptDialog.fNbinLiveSliceY", 0);
								env.SaveLevel(kEnvLocal);
								fDisplayMenu->UnCheckEntry(kFHLiveSliceY);
								fNbinLiveSliceY = 0;
								TCanvas * canpro = Hpr::FindCanvas("_projection_");
								if ( canpro ) {
									TString cname = canpro->GetName();
									// hide canvas from root
									if (cname.EndsWith("2")) {
										TRegexp pr("_projection_");
										cname(pr) = "_Projection_";
										canpro->SetName(cname);
									}
								}
							} else {
								Int_t nb= env.GetValue("Set2DimOptDialog.fNbinSliceY", 1);
								env.SetValue("Set2DimOptDialog.fNbinLiveSliceY", nb);
								env.SaveLevel(kEnvLocal);
								fDisplayMenu->CheckEntry(kFHLiveSliceY);
								fNbinLiveSliceY = 1;
							}
							break;
							}
					  case kFHOutputStat:
							fFitHist->OutputStat(0);
							break;
						case kFHContHist:
							fFitHist->OutputStat(1);
							break;
						case kFHHistToFile:
							fHistInPad = Hpr::FindHistInPad(fHCanvas);
							if ( fHistInPad ) {
								if ( fHistInPad->GetEntries() < 1 ) {
									TGraph *gr =  Hpr::FindGraphInPad(fHCanvas);
									if ( gr ) {
										Save2FileDialog sfd(gr, NULL, fRootCanvas);
										break;
									}
								}
								Save2FileDialog sfd(fHistInPad, NULL, fRootCanvas);
							} else {
								cout << setblue << "No hist in pad, " << endl
								<< "In case of 3Dim hist: use Scatter or Box option"
								<< setblack << endl;
							}
							break;
							
						case kFHGraphToFile:
						
							if (fGraph1D)
								Save2FileDialog(fGraph1D,NULL, fRootCanvas);
							if (fGraph2D)
								Save2FileDialog(fGraph2D,NULL, fRootCanvas);
							break;
						case kFHSelAnyDir:
							{
							TString CurDir = gSystem->pwd();
							fHistPresent->SelectFromOtherDir();
							gSystem->cd(CurDir);
							}
							break;
						case kFHSeldCache:
							fHistPresent->SelectdCache();
							break;
						case kFHGraphToASCII:
							if (fGraph1D)
								Hpr::WriteGraphasASCII(fGraph1D, fRootCanvas);
							break;
						case kFHCanvasToFile:
							Canvas2RootFile();
							break;
						case kFHAsTCanvasToFile:
							AsTCanvas2RootFile();
							break;
						case kFHHistToASCII:
							fHistInPad = Hpr::FindHistInPad(fHCanvas);
							if ( fHistInPad )
								Hpr::WriteHistasASCII(fHistInPad, fRootCanvas, 0);
//							Hpr::WriteHistasASCII(fFitHist->GetSelHist(), fRootCanvas);
							else
								cout << "No hist in pad" << endl;
							break;
						case kFHHistToGraph:
							fHistInPad = Hpr::FindHistInPad(fHCanvas);
							if ( fHistInPad )
								Hpr::WriteHistasASCII(fHistInPad, fRootCanvas, 1);
//							Hpr::WriteHistasASCII(fFitHist->GetSelHist(), fRootCanvas);
							else
								cout << "No hist in pad" << endl;
							break;

						case kFHASCIIToHist:
							fHistPresent->HistFromASCII(fRootCanvas);
							break;
						case kFHNtuple:
							fHistPresent->NtupleFromASCII(fRootCanvas);
							break;
						case kFHGraph:
							fHistPresent->GraphFromASCII(fRootCanvas);
							break;
						case kFHGraph2D:
							fHistPresent->Graph2DFromASCII(fRootCanvas);
							break;
						case kFHEmptyHist:
							new EmptyHistDialog(fRootCanvas, WindowSizeDialog::fWincurx,
													  WindowSizeDialog::fWincury); 
							break;

						case  kFHMarksToCut:
							fFitHist->MarksToCut();
							break;
						case  kFHClearRegion:
							fFitHist->ClearRegion();
							break;
						case kFHInitCut:
							fFitHist->InitCut();
							break;
						case kFHClearCut:
							fFitHist->ClearCut();
							break;
						case kFHRemoveAllCuts:
							fFitHist->RemoveAllCuts();
							break;
						case kFHListCuts:
							fFitHist->ListCuts();
							break;
						case kFHListWindows:
							fFitHist->ListWindows();
							break;
						case kFHDrawCut:
							fFitHist->DrawCut();
							break;
						case kFHDrawCutName:
							fFitHist->DrawCutName();
							break;
						case kFHWriteOutCut:
							fFitHist->WriteOutCut();
							break;
						case  kFHCutsToHist:
							fFitHist->AddCutsToHist();
							break;
						case  kFHCutsFromHist:
							fFitHist->RemoveWindowsFromHist();
							break;
						case kFHTerminate:
							gApplication->Terminate(0);
							break;
						 case kFHFit2DimGaus1:
							fFitHist->Fit2DimD(1);
							break;
						  case kFHFit2DimGaus2:
							fFitHist->Fit2DimD(2);
							break;
						 case kFHFit2DimGaus3:
							fFitHist->Fit2DimD(3);
							break;
						 case kFHFit2DimGaus4:
							fFitHist->Fit2DimD(4);
							break;
						  case kFHFit2DimGaus5:
							fFitHist->Fit2DimD(5);
							break;
					  case kFHFitGausLBg:
							 new FitOneDimDialog(fSelHist, 1);
//                     fFitHist->Fit1DimDialog(1);
							break;
						 case kFHFitExp:
							 new FitOneDimDialog(fSelHist, 2);
//							 fFitHist->Fit1DimDialog(2);
							break;
						 case kFHFitLandau:
							 new FitOneDimDialog(fSelHist, 5);
//							 fFitHist->Fit1DimDialog(2);
							break;
						 case kFHFitPol:
							 new FitOneDimDialog(fSelHist, 3);
//							 fFitHist->Fit1DimDialog(3);
							break;
						 case kFHFitForm:
							 new FitOneDimDialog(fSelHist, 4);
//							 fFitHist->Fit1DimDialog(4);
							break;
						 case kFHFillRandom:
							 Hpr::FillHistRandom(fHCanvas);
//							 fFitHist->Fit1DimDialog(4);
							break;
						case kFHFitUserG:
							if (fGraph1D) 
								ExecFitMacroG(fGraph1D, fRootCanvas);   // global function !!
							break;
						case kFHGausFitG:
							if (fGraph1D)
								ExecGausFitG(fGraph1D, 1);                 // global function !!
							break;
						case kFHExpFitG:
							if (fGraph1D)
								ExecGausFitG(fGraph1D, 2);                 // global function !!
							break;
						 case kFHPolFitG:
							if (fGraph1D)
								ExecGausFitG(fGraph1D, 3);                 // global function !!
							break;
						case kFHFormFitG:
							if (fGraph1D)
								ExecGausFitG(fGraph1D, 4);                 // global function !!
							break;
					  case kFHFitUser:
							fFitHist->ExecFitMacro();
							break;
						case kFHFitSlicesYUser:
							fFitHist->ExecFitSliceYMacro();
							break;
						case kFHEditSlicesYUser:
							fFitHist->EditFitSliceYMacro();
							break;
						case kFHEditUserG:
							EditFitMacroG(fRootCanvas);    // global function !!
							break;
						case kFHEditUser:
							fFitHist->EditFitMacro();
							break;
						case kFHMacroBreitW:
							fFitHist->SetTemplateMacro("BreitWigner");
							break;
						case kFHMacroTwoG:
							fFitHist->SetTemplateMacro("TwoGauss");
							break;
						case kFHMacroExpG:
							fFitHist->SetTemplateMacro("ExpGauss");
							break;
						case kFHMacroLandau:
							fFitHist->SetTemplateMacro("Landau");
							break;
						case kFHKeepPar:
							if(fFitHist->GetKeepPar()){
								fFitHist->SetKeepPar(kFALSE);
								fFitMenu->UnCheckEntry(kFHKeepPar);
							} else  {
								fFitHist->SetKeepPar(kTRUE);
								fFitMenu->CheckEntry(kFHKeepPar);
							}
							break;
						case kFHCallMinos:
							if(fFitHist->GetCallMinos()){
								fFitHist->SetCallMinos(kFALSE);
								fFitMenu->UnCheckEntry(kFHCallMinos);
							} else  {
								fFitHist->SetCallMinos(kTRUE);
								fFitMenu->CheckEntry(kFHCallMinos);
							}
							break;
						case kFHFuncsToHist:
							fFitHist->AddFunctionsToHist();
							break;
						case kFHWriteFunc:
							fFitHist->WriteFunctions();
							break;
 //                 case kFHWriteFuncList:
 //                    fFitHist->WriteFunctionList();
 //                    break;
						case kFH_CASCADE1_0:
							fFitHist->FitPolyHist(0);
							break;
						case kFH_CASCADE1_1:
							fFitHist->FitPolyHist(1);
							break;
						case kFH_CASCADE1_2:
							fFitHist->FitPolyHist(2);
							break;
						case kFH_CASCADE1_3:
							fFitHist->FitPolyHist(3);
							break;
						case kFH_CASCADE1_4:
							fFitHist->FitPolyHist(4);
							break;
						case kFH_CASCADE1_5:
							fFitHist->FitPolyHist(5);
							break;
						case kFH_CASCADE1_6:
							fFitHist->FitPolyHist(6);
							break;
						case kFH_CASCADE1_7:
							fFitHist->FitPolyHist(7);
							break;
						case kFH_CASCADE1_8:
							fFitHist->FitPolyHist(8);
							break;
						case kFH_CASCADE1_U:
							fFitHist->FitPolyHist(-1);
							break;
						case kFH_CASCADE1_H:
							fFitHist->HelpFit2dim();
							break;

						case kFH_CASCADE2_0:
							fFitHist->FitPolyMarks(0);
							break;
						case kFH_CASCADE2_1:
							fFitHist->FitPolyMarks(1);
							break;
						case kFH_CASCADE2_2:
							fFitHist->FitPolyMarks(2);
							break;
						case kFH_CASCADE2_3:
							fFitHist->FitPolyMarks(3);
							break;
						case kFH_CASCADE2_4:
							fFitHist->FitPolyMarks(4);
							break;
						case kFH_CASCADE2_5:
							fFitHist->FitPolyMarks(5);
							break;
						case kFH_CASCADE2_6:
							fFitHist->FitPolyMarks(6);
							break;
						case kFH_CASCADE2_7:
							fFitHist->FitPolyMarks(7);
							break;
						case kFH_CASCADE2_8:
							fFitHist->FitPolyMarks(8);
							break;
						case kFH_CASCADE2_U:
							fFitHist->FitPolyMarks(-1);
							break;
						case kFHShiftScale:
						{
							if ( fFitHist && fFitHist->GetSelHist())
								new ShiftScale(fFitHist->GetSelHist(), (HTCanvas*)fHCanvas);
							break;
						}
						case kFHCalibrate:
							fFitHist->Calibrate();
							break;
						case kFHFindPeaks:
							fFitHist->FindPeaks();
							break;
						case kFHRedefineAxis:
							fFitHist->RedefineAxis();
							break;
						case kFHSetRangeAxis:
							fFitHist->SetRangeAxis();
							break;
						case kFHRestoreAxis:
							fFitHist->RestoreAxis();
							break;
						case kFHAddAxisX:
							fFitHist->AddAxis(1);
							break;
						case kFHAddAxisY:
							fFitHist->AddAxis(2);
							break;
//                  case kFHReDoAxis:
//                     fFitHist->ReDoAxis();
//                     break;
// Otto end
					}
				default:
					break;
			}
		default:
			break;
	}
	return kTRUE;
}

//______________________________________________________________________________

void HandleMenus::BuildMenus()
{
	fRootsMenuBar = ((TRootCanvas*)fHCanvas->GetCanvasImp())->GetMenuBar();
/*
	cout << "fRootsMenuBar: " << fRootsMenuBar->GetId()<< endl;
	cout << "fMenuBarItemLayout: " << fMenuBarItemLayout<< endl;
	cout << "fRootCanvas: " << fRootCanvas->GetId()<< endl;
	cout << "fHistPresent: " << fHistPresent<< endl;
	cout << "fFitHist: " << fFitHist<< endl;
*/
	TMrbHelpBrowser * hbrowser = NULL;
	Bool_t fh_menus = kFALSE;
	Bool_t graph1d = kFALSE;
	Bool_t graph2d = kFALSE;
	Bool_t its_start_window = kFALSE;
// turn of auto slicing	
	fNbinLiveSliceX = fNbinLiveSliceY = 0;
	TEnv env(".hprrc");
	env.SetValue("Set2DimOptDialog.fNbinLiveSliceX", 0);
	env.SetValue("Set2DimOptDialog.fNbinLiveSliceY", 0);
	env.SaveLevel(kEnvLocal);
	
	TString cn(fHCanvas->GetName());
	if ( cn == "cHPr" )
		its_start_window = kTRUE;
	if ( fGraph2D )
		graph2d = kTRUE;
	if ( fGraph1D )
		graph1d = kTRUE;
	
	if(fHistPresent){
		hbrowser=fHistPresent->GetHelpBrowser();
	}
	Int_t nDim = 0;
//	cout << "HandleMenu " << " " << fHCanvas << " " <<fFitHist << endl;
	if ( fFitHist ) {
		fh_menus = kTRUE;
		nDim = fFitHist->GetSelHist()->GetDimension();
		fSelHist = fFitHist->GetSelHist();
	} else {
		fSelHist = Hpr::FindHistInPad(fHCanvas);
	}

	TGPopupMenu * pm;
	const TList * l;
	TGMenuEntry * e;

	pm = fRootsMenuBar->GetPopup("Tools");
	if (pm) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("Tools");
		}
	}
	pm = fRootsMenuBar->GetPopup("Help");
	if (pm) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("Help");
		}
	}
	pm = fRootsMenuBar->GetPopup("File");
	if (pm) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("File");
		}
	}

	pm = fRootsMenuBar->GetPopup("Edit");
	if (pm ) {
//   if (pm && fFitHist == 0 ) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("Edit");
		}
	}

	pm = fRootsMenuBar->GetPopup("Classes");
	if (pm) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("Classes");
		}
	}
	pm = fRootsMenuBar->GetPopup("View");
	if (pm) {
		l = pm->GetListOfEntries();
		if (l) {
			TIter next(l);
			while ( (e = (TGMenuEntry *)next()) ) {
				pm->DeleteEntry(e);
			}
			fRootsMenuBar->RemovePopup("View");
		}
	}
	if (!fFitHist) {
/*
		pm = fRootsMenuBar->GetPopup("Options");
		if (pm) {
			l = pm->GetListOfEntries();
			if (l) {
				TIter next(l);
				while ( (e = (TGMenuEntry *)next()) ) {
					pm->DeleteEntry(e);
				}
				fRootsMenuBar->RemovePopup("Options");
			}
		}
*/
	} else {

		pm = fRootsMenuBar->GetPopup("Options");
		if (pm) {
			l = pm->GetListOfEntries();
			if (l) {
				TIter next(l);
				while ( (e = (TGMenuEntry *)next()) ) {
					TString en = e->GetName();
					if (en == "Statistics" || en == "Histogram Title"
						 || en == "Fit Parameters")
						  pm->DeleteEntry(e);
				}
//            fRootsMenuBar->RemovePopup("Options");
			}
			pm->AddEntry("To change: \"Statistics / title display\" use: Hpr-Options", kFH_NoAction);
			pm->DisableEntry(kFH_NoAction);
		}

	}
	if (fFitHist) {
			  pm = fRootsMenuBar->GetPopup("Inspect");
		if (pm) {
			l = pm->GetListOfEntries();
			if (l) {
				TIter next(l);
				while ( (e = (TGMenuEntry *)next()) ) {
					pm->DeleteEntry(e);
				}
				fRootsMenuBar->RemovePopup("Inspect");
			}
		}
	}
	fCascadeMenu1 = NULL;
	fCascadeMenu2 = NULL;
	fCutsMenu     = NULL;
	fViewMenu     = NULL;
	fDisplayMenu  = NULL;
	fFitMenu      = NULL;
	fOptionMenu   = NULL;
	fAttrMenu     = NULL;
	fHistInPad = Hpr::FindHistInPad(fHCanvas);
//	Bool_t is2dim = kFALSE;
	TGPopupMenu * pmo = fRootsMenuBar->GetPopup("Options");
	if (pmo) {
		pmo = fRootsMenuBar->RemovePopup("Options");
		fRootsMenuBar->AddPopup("ROOT-Options", pmo, fMenuBarItemLayout);
		e = pmo->GetEntry("Pad Auto Exec");
		if (e && fHCanvas->GetAutoExec())pmo->CheckEntry(e->GetEntryId());
	}
	TGPopupMenu * pmi = fRootsMenuBar->GetPopup("Inspect");
	if (pmi == 0) pmi = pmo;
  // Create menus
	fFileMenu = new TGPopupMenu(fRootCanvas->GetParent());
//   fFileMenu->AddEntry("&New Canvas",         kFileNewCanvas);
	if (graph1d ||graph2d) {
		fFileMenu->AddEntry("Graph_to_ROOT-File",      kFHGraphToFile);
		if ( graph1d )
			fFileMenu->AddEntry("Graph_to_ASCII-File",     kFHGraphToASCII);
	}
	if ( its_start_window ) {
		fFileMenu->AddEntry("Select ROOT file from any dir",  kFHSelAnyDir);
		fFileMenu->AddEntry("dCache File-List",  kFHSeldCache);
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("ASCII data to Ntuple", kFHNtuple);
		fFileMenu->AddEntry("ASCII data to histogram ",  kFHASCIIToHist);
		fFileMenu->AddEntry("ASCII data to 1D graph",  kFHGraph);
		fFileMenu->AddEntry("ASCII data to 2D graph",  kFHGraph2D);
		fFileMenu->AddEntry("Create empty histogram",  kFHEmptyHist);
	} else {
		fFileMenu->AddEntry("Canvas_to_ROOT-File",     kFHCanvasToFile);
		fFileMenu->AddEntry("As TCanvas_to_ROOT-File", kFHAsTCanvasToFile);
			fFileMenu->AddEntry("Hist_to_ROOT-File",             kFHHistToFile);
			fFileMenu->AddEntry("Hist_to_ASCII-File", kFHHistToASCII);
			fFileMenu->AddEntry("Hist_as_Graph_to_ROOT-File", kFHHistToGraph);
		if (fFitHist != NULL ) {
			if ( nDim < 3)
				fFileMenu->AddEntry("Histogram bin content",  kFHContHist );
		}
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("Picture to Printer",  kFHCanvas2LP);
		fFileMenu->AddEntry("Picture to PS-File",  kFHPictToPS);
		fFileMenu->AddEntry("Save As....",   kFileSaveAs);
		fFileMenu->AddEntry("Save As .eps",  kFileSaveAsEPS);
		fFileMenu->AddEntry("Save As LaTeX incl header",  kFileSaveAsTEX);
		fFileMenu->AddEntry("Save As .pdf",  kFileSaveAsPDF);
		fFileMenu->AddEntry("Save As .gif",  kFileSaveAsGIF);
		fFileMenu->AddEntry("Save As .C",    kFileSaveAsC);
	}
	if (  its_start_window ) {
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("Read Cuts (Window2D) from ASCII file", kFHCutsFromASCII);
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("Open Edit canvas (A4 portrait)",    kFH_Portrait);
		fFileMenu->AddEntry("Open Edit canvas (A4 landscape)",   kFH_Landscape);
		fFileMenu->AddEntry("Open Edit canvas (user defined)",   kFH_UserEdit);
		fFileMenu->AddEntry("Fill in Form",   kFH_FillForm);
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("Terminate program",          kFileQuit);
	} else {
		fFileMenu->AddSeparator();
		fFileMenu->AddEntry("&Close Canvas",       kFileCloseCanvas);
	}

	fOptionMenu = new TGPopupMenu(fRootCanvas->GetParent());
	if(!fObject && !fFitHist) {
		fOptionMenu->AddEntry("Various HistPresent Options", kOptionHpr);
		fOptionMenu->AddEntry("Default window sizes", kOptionWin);
		fOptionMenu->AddEntry("Fonts used in GUI widgets", kOptionFont);
		fOptionMenu->AddEntry("Options when showing trees", kOptionTree);
		fOptionMenu->AddEntry("Reset options/values to factory defaults", kOptionResetAll);

	}
	if (!fObject && fFitHist)
		fOptionMenu->AddEntry("What to display for a histgram", kOptionDisp);
	if ( nDim == 1)
		fOptionMenu->AddEntry("How to display a 1-dim histogram", kOption1Dim);
	if ( graph2d || (fFitHist && fFitHist->Its2dim())) {
		if ( graph2d ) 
			fOptionMenu->AddEntry("How to display a Graph2D ", kOption2DimGraph);
		else 
			fOptionMenu->AddEntry("How to display a 2-dim histogram ", kOption2Dim);
		fOptionMenu->AddEntry("Color Palette", kOption2DimCol);
	}
	if ( nDim == 3) {
		fOptionMenu->AddEntry("How to display a 3-dim histogram ", kOption3Dim);
		fOptionMenu->AddEntry("Color Palette", kOption2DimCol);
	}
	if ( graph1d ) {
		fOptionMenu->AddEntry("How to display a graph", kOptionGraph);
//		if (!fGraph1) {
//			fOptionMenu->AddEntry("Various HistPresent Options", kOptionHpr);
//			fOptionMenu->AddEntry("Default window sizes", kOptionWin);
//			fOptionMenu->AddEntry("Reset options/values to factory defaults", kOptionResetAll);
//		}
	}
	if (!fObject && fFitHist)
		fOptionMenu->AddEntry("Default window sizes", kOptionWin);

	fAttrMenu = new TGPopupMenu(fRootCanvas->GetParent());
	fAttrMenu->AddEntry("Axis / Title / StatBox Attributes", kOptionHist);
	fAttrMenu->AddEntry("Canvas, Pad, Frame", kOptionPad);
	fAttrMenu->AddEntry("Use graphics attributes stored on file", kOptionUseAttrFile);
	if (gROOT->GetForceStyle()) {
		fAttrMenu->UnCheckEntry(kOptionUseAttrFile);
	} else {
		fAttrMenu->CheckEntry(kOptionUseAttrFile);
	}
	fAttrMenu->AddEntry("Use currently selected graphics attributes ", kOptionUseCurrentAttr);
	if (gROOT->GetForceStyle()) {
		fAttrMenu->CheckEntry(kOptionUseCurrentAttr);
	} else {
		fAttrMenu->UnCheckEntry(kOptionUseCurrentAttr);
	}
	fAttrMenu->AddEntry("Help on Attributes stored on file", kOptionUseAttrFileHelp);
//	fAttrMenu->AddEntry("How to display a graph", kOptionGraph);
	
//   fAttrMenuDef = new TGPopupMenu(fRootCanvas->GetParent());
//   fAttrMenuDef->AddEntry("Take and set Stat box defaults", kOptionStatDef);
//   fOptionMenu->AddPopup(" Take and set defaults",  fAttrMenuDef);

	fOptionMenu->CheckEntry(kOptionAutoResize);
	fOptionMenu->CheckEntry(kOptionStatistics);
	fOptionMenu->CheckEntry(kOptionHistTitle);
	if (fHCanvas->GetAutoExec()) {
		fOptionMenu->CheckEntry(kOptionAutoExec);
	} else {
		fOptionMenu->UnCheckEntry(kOptionAutoExec);
	}
	fViewMenu = new TGPopupMenu(fRootCanvas->GetParent());
	if ( ! its_start_window ) {
		fViewMenu->AddEntry("Launch Graphics Editor",  kEditEditor);
		fViewMenu->AddEntry("Event &Status",           kViewEventStatus);
		fViewMenu->AddEntry("SetRangeUser, Min/Max 1 dim",kFHSetRangeAxis);
		fViewMenu->AddEntry("Draw selected Functions", kFHDrawFunctions);
		fViewMenu->AddEntry("Subtract Function",       kFHSubtractFunction);
		fViewMenu->AddSeparator();
	}
	fViewMenu->AddEntry("Show Colors",             kViewColors);
	fViewMenu->AddEntry("Show Fonts",              kViewFonts);
	fViewMenu->AddEntry("Show Markers",            kViewMarkers);
	fViewMenu->AddEntry("Show Fillstyles",         kViewFillStyles);
	fViewMenu->AddEntry("Show Line Attr",          kViewLineStyles);
	fViewMenu->AddSeparator();
	fViewMenu->AddEntry("View with OpenGL",        kViewOpenGL);

	if ( graph2d ) {
		fDisplayMenu = new TGPopupMenu(fRootCanvas->GetParent());
		fDisplayMenu->AddEntry("Superimpose selected graph", kFHSuperimposeGraph2Dim);
	}
	if ( graph1d ) {
		fDisplayMenu = new TGPopupMenu(fRootCanvas->GetParent());
		fDisplayMenu->AddEntry("Superimpose selected graph", kFHSuperimposeGraph);
		fDisplayMenu->AddEntry("Superimpose graph scaled", kFHSuperimposeGraphScale);
		fDisplayMenu->AddEntry("Set range of X-axis", kFHSetAxisGraphX);
		fDisplayMenu->AddEntry("Set range of Y-axis", kFHSetAxisGraphY);
	}
	if (fh_menus) {
		fDisplayMenu = new TGPopupMenu(fRootCanvas->GetParent());
		fFitHist->SetMyCanvas(fRootCanvas);
		if (nDim < 3) {
			fDisplayMenu->AddEntry("Expand / Apply cuts",      kFHExpand     );
			fDisplayMenu->AddEntry("Entire / Ignore cuts",      kFHEntire     );
			fDisplayMenu->AddSeparator();
			fDisplayMenu->AddEntry("ClearMarks",   kFHClearMarks);
			fDisplayMenu->AddEntry("PrintMarks",   kFHPrintMarks);
			fDisplayMenu->AddEntry("Set2Marks",    kFHSet2Marks);
			fDisplayMenu->AddEntry("DistBetween2Marks",    kFHDistMarks);
			fDisplayMenu->AddEntry("Highlight marked area", kFHColorMarked);
			fDisplayMenu->AddSeparator();
		}
		if ( nDim == 2 ) {
			fDisplayMenu->AddEntry("ProjectX",     kFHProjectX   );
			fDisplayMenu->AddEntry("ProjectY",     kFHProjectY   );
			fDisplayMenu->AddEntry("ProjectBoth",  kFHProjectB   );
			fDisplayMenu->AddEntry("Project on any Axis",  kFHProjectOnAnyAxis);
			fDisplayMenu->AddEntry("Project_X_WithinFunction", kFHProjectX_func);
			fDisplayMenu->AddEntry("Project_Y_AlongFunction", kFHProjectF);
			fDisplayMenu->AddEntry("Live slice X", kFHLiveSliceX);
			fDisplayMenu->UnCheckEntry(kFHLiveSliceX);
			fDisplayMenu->AddEntry("Live slice Y", kFHLiveSliceY);
			fDisplayMenu->UnCheckEntry(kFHLiveSliceY);
			fDisplayMenu->AddEntry("ProfileX",     kFHProfileX   );
			fDisplayMenu->AddEntry("ProfileY",     kFHProfileY   );
			fDisplayMenu->AddSeparator();
			fDisplayMenu->AddEntry("Transpose",   kFHTranspose  );
			fDisplayMenu->AddEntry("Rotate by 90 deg Clockwise",   kFHTransposeInv);
			fDisplayMenu->AddEntry("Rotate Clockwise", kFHRotateClock);
			fDisplayMenu->AddEntry("Rotate Counter Clockwise", kFHRotateCClock);
			fDisplayMenu->AddSeparator();
		} 
		// for all types
		fDisplayMenu->AddEntry("Superimpose", kFHSuperimpose);
		if ( nDim == 1 ){
			fDisplayMenu->AddEntry("Superimpose scaled", kFHSuperimposeScale);
		}
		fDisplayMenu->AddEntry("Show in same Range", kFHGetRange);
		if ( nDim == 1 ) {
			fDisplayMenu->AddEntry("Rebin",       kFHRebinOne);
		} else if ( nDim == 2 ) {
			fDisplayMenu->AddEntry("Rebin/Clip",   kFHRebinDimDialog  );
		}
		if ( nDim == 3 ) {
		 	fDisplayMenu->AddEntry("TH3 Dialog", kFHTh3Dialog);
		}

		if ( nDim < 3 ) {
			fDisplayMenu->AddSeparator();
			fDisplayMenu->AddEntry("Show Statistics only",  kFHOutputStat );
			fDisplayMenu->AddSeparator();
			// Axes
			TGPopupMenu  * casc_axis= new TGPopupMenu(fRootCanvas->GetParent());
			casc_axis->AddEntry("Enlarge, add extra bins", kFHEnlarge);
			casc_axis->AddEntry("Set Display Range X", kFHXaxisRange);
			if ( nDim > 1)
				casc_axis->AddEntry("Set Display Range Y", kFHYaxisRange);
			if ( nDim > 2)
				casc_axis->AddEntry("Set Display Range Z", kFHZaxisRange);
			casc_axis->AddEntry("Set Min/Max 1 dim",     kFHSetRangeAxis);
			if ( nDim == 1)	
				casc_axis->AddEntry("Magnify",     kFHMagnify    );
			casc_axis->AddEntry("Redefine Axis",     kFHRedefineAxis);
			casc_axis->AddEntry("Restore orig Axis",     kFHRestoreAxis);
			casc_axis->AddEntry("Add new X axis",     kFHAddAxisX);
			casc_axis->AddEntry("Add new Y axis",     kFHAddAxisY);
			casc_axis->AddEntry("Repaint extra axis", kFHReDoAxis);
			fDisplayMenu->AddPopup("Manipulate Axes (new, ranges etc.)", casc_axis);
			// user contours				
			if ( nDim == 2 ) {
				TGPopupMenu  * casc_cont= new TGPopupMenu(fRootCanvas->GetParent());
				casc_cont->AddEntry("Modify Contours",   kFHUserCont);
				casc_cont->AddEntry("Use Selected Contours",   kFHUserContUse);
				casc_cont->AddEntry("Clear User Contours",   kFHUserContClear);
				casc_cont->AddEntry("Save User Contours",   kFHUserContSave);
				fDisplayMenu->AddPopup("Handle Contours", casc_cont);
			}
			
			if (fFitHist->GetSelHist()->GetDimension() == 1) {
				fDisplayMenu->AddEntry("Log Y scale",  kFHLogY);
				if (fHCanvas->GetLogy()) fDisplayMenu->CheckEntry(kFHLogY);
				else                   fDisplayMenu->UnCheckEntry(kFHLogY);
			} else if (fFitHist->GetSelHist()->GetDimension() == 2) {
				fDisplayMenu->AddEntry("Log Z scale",  kFHLogY);
				if (fHCanvas->GetLogz()) fDisplayMenu->CheckEntry(kFHLogY);
				else                   fDisplayMenu->UnCheckEntry(kFHLogY);
			}


			if(hbrowser)hbrowser->DisplayMenu(fDisplayMenu, "display.html");
			if (!strncmp(fHCanvas->GetName(), "cstack", 5)){
				fDisplayMenu->AddEntry("Real stack", kFHStack);
				if (fHistPresent->fRealStack) fDisplayMenu->CheckEntry(kFHStack);
				else                          fDisplayMenu->UnCheckEntry(kFHStack);

				fDisplayMenu->AddEntry("Set Minimum of Yscale", kFHStackMin);
				fDisplayMenu->AddEntry("Set Maximum of Yscale", kFHStackMax);
			}
			fDisplayMenu->AddSeparator();
		}
	}
	if ( ! its_start_window && nDim < 3) {
		if(fh_menus && nDim < 3){
			fCutsMenu     = new TGPopupMenu(fRootCanvas->GetParent());
			if(nDim == 2){
				fCutsMenu->AddEntry("Init Cut",      kFHInitCut    );
				fCutsMenu->AddEntry("Marks To Cut",   kFHMarksToCut);
				fCutsMenu->AddEntry("Clear Active Cuts",kFHClearCut   );
				fCutsMenu->AddEntry("Remove All Cuts",kFHRemoveAllCuts   );
				fCutsMenu->AddEntry("List Cuts",     kFHListCuts    );
				fCutsMenu->AddEntry("Draw Cuts",      kFHDrawCut    );
				fCutsMenu->AddEntry("DrawCutName",  kFHDrawCutName    );
				fCutsMenu->AddEntry("Add Cuts to Hist",   kFHCutsToHist   );
				fCutsMenu->AddEntry("Remove Cuts from Hist",   kFHCutsFromHist   );
				fCutsMenu->AddEntry("Write out Cuts", kFHWriteOutCut);
			} else {
				fCutsMenu->AddEntry("Marks to Window",   kFHMarksToWindow   );
				fCutsMenu->AddEntry("List Windows",     kFHListWindows     );
				fCutsMenu->AddEntry("Draw Windows",     kFHDrawWindows     );
				fCutsMenu->AddEntry("Set Windows inactive",    kFHClearWindows    );
				fCutsMenu->AddEntry("Add Windows to Hist",   kFHWindowsToHist   );
				fCutsMenu->AddEntry("Remove Windows from Hist",   kFHWindowsFromHist);
				fCutsMenu->AddEntry("Writeout Windows", kFHWriteOutWindows);
			}
			fCutsMenu->AddSeparator();
			fCutsMenu->AddEntry("SelectInside",  kFHSelectInside);
			if(fFitHist->InsideState()) fCutsMenu->CheckEntry(kFHSelectInside);
			else                        fCutsMenu->UnCheckEntry(kFHSelectInside);
			fCutsMenu->AddEntry("ClearMarks",   kFHClearMarks);
			fCutsMenu->AddEntry("PrintMarks",   kFHPrintMarks);
			fCutsMenu->AddEntry("Set2Marks",    kFHSet2Marks);
			fCutsMenu->AddSeparator();
			fCutsMenu->AddEntry("Clear (rectangular) Region",    kFHClearRegion);
			if(hbrowser)hbrowser->DisplayMenu(fCutsMenu, "cuts.html");
	//      fCutsMenu->AddEntry("Help On Marks",         kFH_Help_Mark);
	//      fCutsMenu->AddEntry("Help On Cuts/Windows",  kFH_Help_Cuts);

			fCascadeMenu1 = new TGPopupMenu(fRootCanvas->GetParent());
			fCascadeMenu1->AddEntry("Pol 0", kFH_CASCADE1_0);
			fCascadeMenu1->AddEntry("Pol 1", kFH_CASCADE1_1);
			fCascadeMenu1->AddEntry("Pol 2", kFH_CASCADE1_2);
			fCascadeMenu1->AddEntry("Pol 3", kFH_CASCADE1_3);
			fCascadeMenu1->AddEntry("Pol 4", kFH_CASCADE1_4);
			fCascadeMenu1->AddEntry("Pol 5", kFH_CASCADE1_5);
			fCascadeMenu1->AddEntry("Pol 6", kFH_CASCADE1_6);
			fCascadeMenu1->AddEntry("Pol 7", kFH_CASCADE1_7);
			fCascadeMenu1->AddEntry("Pol 8", kFH_CASCADE1_8);
			fCascadeMenu1->AddEntry("User Formula", kFH_CASCADE1_U);
			fCascadeMenu1->AddEntry("Help", kFH_CASCADE1_H);

			fCascadeMenu2 = new TGPopupMenu(fRootCanvas->GetParent());
			fCascadeMenu2->AddEntry("Pol 0", kFH_CASCADE2_0);
			fCascadeMenu2->AddEntry("Pol 1", kFH_CASCADE2_1);
			fCascadeMenu2->AddEntry("Pol 2", kFH_CASCADE2_2);
			fCascadeMenu2->AddEntry("Pol 3", kFH_CASCADE2_3);
			fCascadeMenu2->AddEntry("Pol 4", kFH_CASCADE2_4);
			fCascadeMenu2->AddEntry("Pol 5", kFH_CASCADE2_5);
			fCascadeMenu2->AddEntry("Pol 6", kFH_CASCADE2_6);
			fCascadeMenu2->AddEntry("Pol 7", kFH_CASCADE2_7);
			fCascadeMenu2->AddEntry("Pol 8", kFH_CASCADE2_8);
			fCascadeMenu2->AddEntry("User Formula", kFH_CASCADE2_U);
			fCascadeMenu2->AddEntry("Help", kFH_CASCADE1_H);
		}
//		if ( fFitHist ) {
		fFitMenu     = new TGPopupMenu(fRootCanvas->GetParent());
			if(nDim == 2){
				TGPopupMenu *g2p = new TGPopupMenu(fRootCanvas->GetParent());
				g2p->AddEntry("1 peak", kFHFit2DimGaus1);
				g2p->AddEntry("2 peak", kFHFit2DimGaus2);
				g2p->AddEntry("3 peak", kFHFit2DimGaus3);
				g2p->AddEntry("4 peak", kFHFit2DimGaus4);
				g2p->AddEntry("5 peak", kFHFit2DimGaus5);
				fFitMenu->AddPopup("Fit 2 dim Gaussian", g2p);
				fFitMenu->AddPopup("FitPolyHist",  fCascadeMenu1);
				fFitMenu->AddPopup("FitPolyMarks", fCascadeMenu2);
				fFitMenu->AddEntry("Auto display Save2FileDialog after fit",  kFHDisplayS2FD);
				if(fFitHist->GetAutoDisplayS2FD()) fFitMenu->CheckEntry(kFHDisplayS2FD);
				else                            fFitMenu->UnCheckEntry(kFHDisplayS2FD);
				
				fFitMenu->AddSeparator();
				fFitMenu->AddEntry("Edit User Fit Slices Y Macro", kFHEditSlicesYUser);
				fFitMenu->AddEntry("Execute User FitSlices Y Macro", kFHFitSlicesYUser);
				fFitMenu->AddSeparator();
			} else {
//				if ( fFitHist )
//  also in case of EmptyHistogram
					fFitMenu->AddEntry("Gaussians (with tail)",   kFHFitGausLBg);
				fFitMenu->AddEntry("Exponential",     kFHFitExp);
				fFitMenu->AddEntry("Landau",     kFHFitLandau);
				fFitMenu->AddEntry("Polynomial",      kFHFitPol);
				fFitMenu->AddEntry("User formula",    kFHFitForm);
				fFitMenu->AddEntry("Fill Random acc Function",    kFHFillRandom);
			}
//	}
		if ( fFitHist ) {
			fFitMenu->AddSeparator();
			fFitMenu->AddEntry("Edit User Fit Macro", kFHEditUser);
			fFitMenu->AddEntry("Execute User Fit Macro", kFHFitUser);
			fFitMenu->AddSeparator();

			fFitMenu->AddEntry("Add Functions to Hist", kFHFuncsToHist);
			fFitMenu->AddEntry("Write Functions to File",     kFHWriteFunc);
			fFitMenu->AddEntry("Draw selected Functions",     kFHDrawFunctions);
			fFitMenu->AddEntry("Subtract Function",       kFHSubtractFunction);
			fFitMenu->AddSeparator();
			fFitMenu->AddEntry("Shift / Scale histogram",     kFHShiftScale);
			if( nDim == 1 ){
				fFitMenu->AddEntry("Calibration Dialog",            kFHCalibrate);
			}
			fFitMenu->AddEntry("FindPeaks",         kFHFindPeaks);
			fFitMenu->AddSeparator();
			fFitMenu->AddEntry("Kolmogorov Test",         kFHKolmogorov);
			fFitMenu->AddEntry("Fast Fourier Transform",         kFHfft);
			if(hbrowser)hbrowser->DisplayMenu(fFitMenu, "fitting.html");
		}
//      fCascadeMenu1->Associate((TGWindow*)this);
		if ( fCascadeMenu2 )
			fCascadeMenu2->Associate((TGWindow*)this);
//      fCascadeMenu1->Associate(this);
	}

	if ( graph1d ) {
		fFitMenu     = new TGPopupMenu(fRootCanvas->GetParent());
		fFitMenu->AddEntry("Edit User Fit Macro", kFHEditUserG);
		fFitMenu->AddEntry("Execute User Fit Macro", kFHFitUserG);
		fFitMenu->AddEntry("Gauss Fit Dialog", kFHGausFitG);
		fFitMenu->AddEntry("Fit Exponential",  kFHExpFitG);
		fFitMenu->AddEntry("Fit Polynomial",   kFHPolFitG);
		fFitMenu->AddEntry("Fit User formula", kFHFormFitG);
		fFitMenu->AddSeparator();
	}

	if (fDisplayMenu) fDisplayMenu->Associate((TGWindow*)this);
	if (fViewMenu) fViewMenu->Associate((TGWindow*)this);
// this main frame will process the menu commands
	fFileMenu->Associate((TGWindow*)this);
	fOptionMenu->Associate((TGWindow*)this);
	fAttrMenu->Associate((TGWindow*)this);
	fAttrMenu->Associate((TGWindow*)this);
//   if (fFitHist)
 //     fRootsMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout, fRootsMenuBar->GetPopup("Edit"));
//   else
		fRootsMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout, pmi);

	if (fHistPresent) 
		fRootsMenuBar->AddPopup("&Hpr-Options", fOptionMenu,  fMenuBarItemLayout, pmi);
//	if ( fHistPresent || graph1d ) 
		fRootsMenuBar->AddPopup("GraphicsAtt", fAttrMenu,  fMenuBarItemLayout, pmi);
	
	if (fViewMenu) fRootsMenuBar->AddPopup("&View", fViewMenu,  fMenuBarItemLayout, pmi);
	if (fDisplayMenu) fRootsMenuBar->AddPopup("&Display", fDisplayMenu,  fMenuBarItemLayout, pmi);
	if( fCutsMenu ){
		fRootsMenuBar->AddPopup("Cuts/Windows",    fCutsMenu,  fMenuBarItemLayout, pmi);
		fCutsMenu->Associate((TGWindow*)this);
	}
	if ( fFitMenu ) {
		fRootsMenuBar->AddPopup("Draw/Fit/Calib", fFitMenu,   fMenuBarItemLayout, pmi);
		fFitMenu->Associate((TGWindow*)this);
	}
//   if(edit_menus){
//         fRootsMenuBar->AddPopup("Hpr-Edit",            fEditMenu,  fMenuBarItemLayout, pmi);
//   }
	if ( graph1d ) {
		fRootsMenuBar->AddPopup("Fit", fFitMenu,   fMenuBarItemLayout, pmi);
	}
	if(hbrowser) {
		fHelpMenu     = new TGPopupMenu(fRootCanvas->GetParent());
//      hbrowser->DisplayMenu(fHelpMenu, "*.html");
		hbrowser->DisplayMenu(fHelpMenu, "index.html");
		hbrowser->DisplayMenu(fHelpMenu, "overview.html");
		hbrowser->DisplayMenu(fHelpMenu, "showlists.html");
		fHelpMenu->Associate((TGWindow*)this);
		fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
		fRootsMenuBar->AddPopup("&Help on HistPresent",    fHelpMenu,    fMenuBarHelpLayout);
	}
	fRootCanvas->ForceUpdate();
	gSystem->ProcessEvents();
	return;
}
//______________________________________________________________________________
void HandleMenus::SetLog(Int_t state)
{
	 if(state > 0) fDisplayMenu->CheckEntry(kFHLogY);
	 else          fDisplayMenu->UnCheckEntry(kFHLogY);
//	 cout << " HandleMenus::SetLog " << state << endl;
}
//______________________________________________________________________________

void HandleMenus::SetLiveSliceX(Int_t state)
{
	fNbinLiveSliceX = state;
	if (state > 0)
		fDisplayMenu->CheckEntry(kFHLiveSliceX);
	else 
		fDisplayMenu->UnCheckEntry(kFHLiveSliceX);
}
//______________________________________________________________________________

void HandleMenus::SetLiveSliceY(Int_t state)
{
	fNbinLiveSliceY = state;
	if (state > 0)
		fDisplayMenu->CheckEntry(kFHLiveSliceY);
	else 
		fDisplayMenu->UnCheckEntry(kFHLiveSliceY);
}

//_______________________________________________________________________________________
void HandleMenus::AsTCanvas2RootFile()
{
	TRootCanvas * trc = (TRootCanvas*)fHCanvas->GetCanvasImp();
	Bool_t se = fHCanvas->GetShowEditor();
	if ( se && fEditor != NULL) {
		trc->ShowEditor(kFALSE);
		fEditor->ShowToolBar(kFALSE);
		fEditor->ExecuteAdjustSize(-1);
	}
   TCanvas *can =  Hpr::HT2TC(fHCanvas);
	Save2FileDialog sfd(can, NULL, trc);
	if ( se  && fEditor != NULL) {
		trc->ShowEditor(kTRUE);
		fEditor->ExecuteAdjustSize( 1);
		fEditor->ShowToolBar(kTRUE);
	}
   delete can;
}
//_______________________________________________________________________________________
void HandleMenus::Canvas2RootFile()
{
	TRootCanvas * trc = (TRootCanvas*)fHCanvas->GetCanvasImp();
	Bool_t se = fHCanvas->GetShowEditor();
	if ( se && fEditor != NULL) {
		trc->ShowEditor(kFALSE);
		fEditor->ShowToolBar(kFALSE);
		fEditor->ExecuteAdjustSize(-1);
	}
	Save2FileDialog sfd(fHCanvas, NULL, trc);
	if ( se  && fEditor != NULL) {
		trc->ShowEditor(kTRUE);
		fEditor->ExecuteAdjustSize( 1);
		fEditor->ShowToolBar(kTRUE);
	}
}
//_______________________________________________________________________________________
void HandleMenus::UseAttrFileHelp()
{
	static const Char_t helptext[] =
"Histograms and graphs can be displayed\n\
with graphics attributes (colors, markerstyle,\n\
linestyle etc.) as stored with these objects\n\
on the ROOT file.\n\
This corresponds to: \"gROOT->ForceStyle(kFALSE)\"\n\
Otherwise attributues as currently defined\n\
by \"Set as global default\" in the above menus\n\
are used:  \"gROOT->ForceStyle(kTRUE)\"";
	TRootHelpDialog * hd = new TRootHelpDialog(fRootCanvas,
		"Help on gROOT->ForceStyle", 350, 150);
	hd->SetText(helptext);
	hd->Popup();
}

#ifndef HPRBASE
#define HPRBASE
#include "TCanvas.h"
#include "TGWindow.h"
#include "TF1.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TPaveStats.h"
#include "HprGaxis.h"
#include "HprLegend.h"

class TPolyLine3D;
class TRootCanvas;
class GrCanvas;

namespace Hpr
{
	void WriteGraphasASCII(TGraph * g, TRootCanvas * mycanvas = 0);
	void WriteOutGraph(TGraph * g, TRootCanvas * mycanvas = 0);
	void WriteHistasASCII(TH1 *hist, TGWindow *window = NULL, Int_t a_g = 0);
	void WarnBox(const char *, TGWindow *window = NULL);
	Bool_t QuestionBox(const char *message, TGWindow *window);
	TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t pop_push);
	TH1 * FindHistInPad(TVirtualPad * ca, Int_t lfgraph = 0, Int_t lfstack = 0, TObject ** parent=0);
	TGraph * FindGraphInPad(TVirtualPad * ca);
	TGraph2D* FindGraph2D(TVirtualPad *ca);
	void SuperimposeGraph2Dim(TCanvas * current);
	TLegend * FindLegendInPad(TVirtualPad * ca);
	Bool_t HistLimitsMatch(TH1* h1, TH1* h2);
	HprGaxis * DoAddAxis(TCanvas * canvas, TH1 *hist, Int_t where, 
		Double_t ledge, Double_t uedge, Double_t axis_offset=0, Color_t col=0);
	Int_t SuperImpose(TCanvas * canvas, TH1 * selhist, Int_t mode);
	TH1 * GetOneHist(TH1 * selhist, TCanvas * canvas = NULL);
	void ResizeStatBox(TPaveStats * st, Int_t ndim); 
	Bool_t IsSelected(const char * name, TString * mask, Int_t use_regexp);
	Bool_t IsSelected(const char * name, Int_t from_run, Int_t to_run);
	void BoundingB3D(TPolyLine3D * pl,  Double_t x0, Double_t y0, Double_t z0, 
						  Double_t x1, Double_t y1, Double_t z1);
	TF1 * FindFunctionInPad(TVirtualPad * ca);
	void FillHistRandom(TVirtualPad * ca);
	void ReplaceUS(const char * fname, Int_t latexheader=0);
	Int_t NofLines(TString& str);
	Int_t MixPointsInGraph2D(TGraph2D * grin, TGraph2D * grout, Int_t npoints = 0);
	TCanvas * FindCanvas(const Char_t * pat);
	void SetUserPalette(Int_t startindex, TArrayI * pixels);
	Int_t GetFreeColorIndex();
	Double_t DtoL(Double_t px, Double_t py, Double_t x1, Double_t y1, Double_t x2, Double_t y2 );
	TH1F* projectany(TH2* hin , TH1F* hp, Double_t co, Double_t sl);
	TCanvas * HT2TC(GrCanvas *htc);
	TObject *FindObjectByWildcard(const char *s, TList *list);
	Int_t PrintList(TList *list);

}
#endif

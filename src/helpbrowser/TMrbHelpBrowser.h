#ifndef __TMrbHelpBrowser_h__
#define __TMrbHelpBrowser_h__

#include "TROOT.h"
#include "TString.h"
#include "TList.h"
#include "TFile.h"
#include "TGMenu.h"
#include <iostream>

namespace std {} using namespace std;

class TMrbHelpBrowser : public TNamed {
private:
   TFile * fRootFile;
   TString fHtmlDir;
   TList * fHelpList;
   TList * fCanvasList;
   Int_t fWwX, fWwY;
   Int_t fX0;
   Int_t fY0;
   Int_t fScreen_w, fScreen_h;
   Int_t fTextSize;                  // size in pixels
   Int_t fTextFont;                  // 
   Int_t fMaxLineLength; 
   TString fGifViewer;

public:

   ~TMrbHelpBrowser();
//#if ROOTVERSION > 60000
//	TMrbHelpBrowser();
//#endif
   TMrbHelpBrowser(const char * InputFile = 0);
	void RecursiveRemove(TObject*);
   void SetTextSize(Int_t size);
   
   void SetMaxLineLength(Int_t len);

   void SetTextFont(Int_t font);
   
   void SetGifViewer(const char * viewer);
   
   void SetOrigin(Int_t x0, Int_t y0){fX0=x0; fY0=y0;};

//________________________________________________________________________________

	Int_t AddHtml(const char * InputFile);

	Int_t AddHtmlFile(const char * HtmlFile, Bool_t keep_references = kTRUE);
	Int_t AddHtmlAsString(TString & htext, TString & anchor, TString & dir, Bool_t keep_references = kTRUE);

	Int_t AddHtmlDir(const char * HtmlDir);
	Int_t AddGifToRoot(const char * Input);

	void AddGifFile(const char * GifFile);
	void AddCanvas(const char * RootFile);
	void Clear(Option_t * Option) { TObject::Clear(Option); };
	void CanvasClosed();
	void Clear();

	TList * GetHelpList() {return fHelpList;};
	TList * GetCanvasList() {return fCanvasList;};
	TFile * SetRootFile(const char * RootFile){
   fRootFile = new TFile(RootFile);
   return fRootFile;
}

	static void HandleMouseClicks();
	//________________________________________________________________________________

	Bool_t ToRootFile(const char * Input, const char * RootFile, 
							const char * option = "UPDATE");

	TFile * GetRootFile(){return fRootFile;}

	void Print(Option_t * Option) const { TObject::Print(Option); };
	void Print(TString * Option = NULL) const;

	void DrawText(const char * hname, Int_t xoff = -1, Int_t yoff = -1);
	void DrawText(const Int_t  ind, Int_t xoff = -1, Int_t yoff = -1);
	void DisplayImage(TString & img_name);

	Int_t DisplayMenu(TGPopupMenu * menu, const char * pattern = "");

	Int_t GetNofItems(){return fHelpList->GetSize();};
//________________________________________________________________________________

private:

	Bool_t Squeeze_White_Space(TString & s);

	Bool_t Squeeze_EOL(TString & s);

	Int_t NextLine(TString & text, TString & line, Bool_t start);

	Int_t NextTag(TString & text, TString & tag, Int_t search_from);

	Int_t NextAnchor(TString & text, TString & anchor, Int_t search_from);

	Int_t NextHeading(TString & text, TString & heading, Int_t search_from);

	Int_t AddTagAtEOL(TString & s);


	Int_t LineCount(TString & str, Int_t * longest_line, Int_t * nof_heads);

	void MoveOrigin();

	void PreScan(TString & text, Bool_t keep_references = kTRUE);


	ClassDef(TMrbHelpBrowser, 0)   // [HelpBrowser] Help browser
};

#endif

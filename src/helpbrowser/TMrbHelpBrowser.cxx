#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TText.h"
#include "TLatex.h"
#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TNamed.h"
#include "TArrayC.h"
#include "TKey.h"
#include "TList.h"
#include "TFile.h"
#include "TH1.h"
#include "TBox.h"
#include "TImage.h"
#include "TVirtualX.h"
#include "TGClient.h"
#include "TGWindow.h"
#include "TRegexp.h"
#include <fstream>
#include <iostream>
#include <iomanip>

#include "TNamedString.h"
#include "TNamedBytes.h"
#include "TMrbHelpBrowser.h"

enum ERefTypes {kTextOnly, kTextRef, kCanvasRef, kGifRef, kPsRef, kJpegRef};

ClassImp(TMrbHelpBrowser)

//_______________________________________________________________________________
//
// This class implements a simple HelpBrowser typically used in an interactive
// application where a standard browser like Netscape appears to be too clumpsy
// and takes away too much resources especially space on the screen.
// It does n o t support internet references (e.g. http:) but only file refs.
// It does n o t support display of the Web documentation of ROOT.
//
//It accepts input in HTML format but does not recognize all possible tags. 
// currently only the following tags are recognized (others are just ignored):
//  
//<Hn> 					  Heading i.e. highlight line
//<B>, <I> 				  bold, italic
//<a NAME=xxx> 			  an anchor
//<a href=yyy>text</a> 	  a reference (may not be http:)
//<img SRC=pict.canvas.gif>  insert a gif picture
//
//The constructor is normally called with the name of a directory containing 
//html-files to be included into the documention. 
//NB: A file index.html containing the entry level hrefs should be
//    part of this directory.
//If the complete documentation is in one htmlfile the name of
//this file may be given instead.
//Also a ROOT-file containing ready made documentation can be given
//(see method ToRootFile below);
//More files / directories may be added using the method: AddHtml
//
//It chops text between references (<a href=..) into help items. 
//Text before the the first anchor gets the name of the the file
//as name of the help item. This is useful to construct an index which
//works also for a normal browser like netscape.
//If a reference is clicked on only the text for this item is displayed. 
//The window size is adjusted to the number of lines.
//It is assumed that the help text is preformatted, so no formatting
//is done except optionally breaking lines at fMaxLineLength.
//A possible image is displayed in a separate window.
//
//To display the helptext the method: DrawText(anchor_name, xpos, ypos)
//should be used.
//When help items should be added to a popup menu the method
//DrawText(index, xpos, ypos)
//
//As an extension ROOT-files containing canvases may be 
//defined. If an image with name "pict.canvas.gif" appears
//in the documentation a root-file with name "pict.canvas.root"
//is searched for in the same directory.
//i.e. the convention is to replace the suffix .gif by .root 
//If not found xv is invoked for the gif-file.
//Otherwise the the canvas contained in the root-file is drawn.
//This allows a standard browser to be used and still have the possiblity
//to exploit the interactive features of root when showing a canvas.
//Begin_Html
//An example (for HistPresent) can be found <A HREF="hpr/index.html">here</A>. 
//End_Html
//Use View Page source to see how it is done.

//#if ROOTVERSION > 60000
//TMrbHelpBrowser::TMrbHelpBrowser()
//{
//	cout << "TMrbHelpBrowser default ctor " << this << endl<<flush;
//}
//#endif
//___________________________________________________________

TMrbHelpBrowser::TMrbHelpBrowser(const char * InputFile)
				:TNamed("TMrbHelpBrowser", "TMrbHelpBrowser")
{
	if (gDebug>0) cout << "TMrbHelpBrowser normal ctor " << this << endl;
	UInt_t w, h;
	Int_t screen_x, screen_y;
	//  screen size in pixels
	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
					screen_x, screen_y, w, h);
	fScreen_w = (Int_t)w;
	fScreen_h = (Int_t)h;
	fWwX = 720;
	fX0 = fScreen_w - fWwX  - 150;
	fY0 = 50;
	fTextSize = 18;
	fTextFont = 100;         // courier
//      fDefaultFont = 100;        // courier bold (+10 italic)
//      fDefaultFont = 40;         // helvetica
//      fDefaultFont = 60;         // helvetica bold
	fMaxLineLength = 75;    // -1: dont wrap lines as default
	fHelpList = new TList();
	fCanvasList = new TList();
	fRootFile = 0;
	fGifViewer = "xv";
	gROOT->GetList()->Add(this);
	if(!InputFile){
		cout << "WARNING: No input file given" << endl;
	} else {
		TRegexp endwithroot("\\.root$");   
		TString infile(InputFile);

		if(infile.Index(endwithroot) < 0){
			AddHtml(InputFile); 
		} else {
			fRootFile = new TFile(InputFile);
		}
	}
	gROOT->GetListOfCleanups()->Add(this);
	cout << "Init TMrbHelpBrowser from: " << InputFile << endl;
}
//________________________________________________________________________________
TMrbHelpBrowser::~TMrbHelpBrowser()
{
//	cout << "TMrbHelpBrowser dtor " << this << endl<<flush;
	gROOT->GetList()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
}

//________________________________________________________________________________

void TMrbHelpBrowser::SetTextSize(Int_t size){
//  set character size (in pixels), supported are 8, 10, 12, 14, 16, 18, 24
   fTextSize = size;
}
//________________________________________________________________________________

void TMrbHelpBrowser::SetMaxLineLength(Int_t len){
//  if > 0 lines will be broken at fMaxLineLength
//  however the result is nomally quite mediocre
   fMaxLineLength = len;
}

void TMrbHelpBrowser::SetTextFont(Int_t font){
//  set text font (see root doc) recommended are only courier fonts (80, 100),
//  proportional fonts like helvetica are not very useful
   fTextFont = font;
}
//________________________________________________________________________________

void TMrbHelpBrowser::SetGifViewer(const char * viewer){
// set a viewer for gif files, the default is xv
	fGifViewer = viewer;
};

//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddHtml(const char * InputFile){

//   add a single file or html files contained in dir depending on 
//   if InputFile is a file or directory

   Long_t id, flags, modtime;
   Long64_t size;
   Int_t nfiles = 0;
   gSystem->GetPathInfo(InputFile,&id, &size, &flags, &modtime);
   if((flags & 2) == 0)nfiles = AddHtmlFile(InputFile);  
   else               nfiles = AddHtmlDir(InputFile);
   if ( gDebug > 0 ) 
		cout << nfiles << " help files found" << endl;
   return nfiles;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddHtmlFile(const char * HtmlFile, Bool_t keep_references){

//  scan file and chop text following anchors into help items.
//  Text before the first anchor gets as name the name of the file.
//  With keep_references = kFALSE references are removed from the
//  text, this is useful if the file is imported from a external
//  source (e.g. the class description part of TAttText from the 
//  documentation of ROOT)

//   cout << "Look for " << HtmlFile << endl;
   ifstream infile(HtmlFile);
   if(infile.fail()){
      cout << "Cant open " << HtmlFile << endl;
      infile.close();
      return -1;
   }
   TString dir(HtmlFile);
//  directory name is inserted into picture names
   if(dir.Index("/") >= 0){     
      dir.Resize(dir.Length() - strlen(gSystem->BaseName(dir.Data())));
//      cout << "dir " << dir << endl;
   } else {
      dir = "";
   }
   TString anchor(gSystem->BaseName(HtmlFile));
   TString rawtext;   
   rawtext.ReadFile(infile);
   Int_t res = AddHtmlAsString(rawtext, anchor, dir, keep_references );
   infile.close();
   return res;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddHtmlAsString(TString & rawtext, TString & anchor, TString & dir, 
													Bool_t keep_references){

   TString helptext;
   TString line;
   TString tag;
	Bool_t start = kTRUE;
   PreScan(rawtext, keep_references);   // remove unknown tags and EOL in tags
   while(NextLine(rawtext, line, start) >=0){
      start = kFALSE;
      Int_t tstart = 0;
      Int_t tag_start = NextTag(line, tag, tstart);
      if(tag_start >= 0) {
         if((tag.Index("<imgSRC=", 8, 0,  TString::kIgnoreCase)) >= 0){
            if(dir.Length() > 1 && tag(9) != '/'){
//               line.Insert(tag_start+8,"/");
               line.Insert(tag_start+9,dir.Data());
//               cout << "Inserted dir: " << line << endl;
            }
         } else if(tag.Index("<aNAME", 6, 0,  TString::kIgnoreCase) >= 0){
            if(helptext.Length() > 1){
               if (anchor.BeginsWith("index"))
                  fHelpList->AddFirst(new TNamedString(anchor, helptext));
               else
                  fHelpList->Add(new TNamedString(anchor, helptext));
               helptext = "";
            }
            line.Remove(tag_start, tag.Length());
            anchor = tag;
            anchor.Remove(tag.Length()-1,1);   // >
            anchor.Remove(0,7);                // <aNAME=
         }
      }
      if(helptext.Length() > 1 || line.Length() > 1){
//         if line longer 64 chars try to chop it
 //        cout << "in AddHtmlFile " << line.Length() << " " << fMaxLineLength<< endl;
          
         if(fMaxLineLength > 0){
         	TString subline;
         	Int_t fnb = -1;       // first non blank
         	Int_t lblank = 0;       // last  blank
         	Int_t from = 0;
         	Bool_t intag = kFALSE;
         	Int_t len = 0;
         	Int_t nl = 0;
 //           cout << "in chop line, len: " << line.Length() << endl;
         	for(Int_t i=0; i < line.Length(); i++){
            	if     (line[i] == '<')intag = kTRUE;
            	else if(line[i] == '>')intag = kFALSE;
            	else if(!intag){
               	if(fnb < 0 && line[i] != ' ')fnb = i;
               	if(line[i] == ' ')lblank = i;      // place to wrap line
               	len ++;
               	subline = line(from, lblank);
               	if(len >= fMaxLineLength && lblank > from){
                  	subline = line(from, lblank);
                  	if(nl > 0 && fnb > -1){
                     	for(Int_t j=0; j< fnb; j++)subline.Prepend(" ");
                  	}
                  	from = lblank+1;
                  	helptext += subline; helptext += "\n";
 //                 	cout << "subline:" <<  subline << endl;      
                  	nl++;
                  	len = 0;
               	}
            	}
         	}
         	if(from < line.Length()){
            	subline = line(from, line.Length());
            	if(nl > 0 && fnb > -1){
               	for(Int_t j=0; j< fnb; j++)subline.Prepend(" ");
            	}
            	helptext += subline; helptext += "\n";          
 //           	if(nl > 0)cout << "Lastline:" << subline << endl;
         	}
      	} else {
            helptext += line; helptext += "\n";          
         }
      } 
   }  
//  anything left over?  
   if(anchor.Length() > 0) { 
   	if (anchor.BeginsWith("index"))
      	fHelpList->AddFirst(new TNamedString(anchor, helptext));
   	else
      	fHelpList->Add(new TNamedString(anchor, helptext));
   }
   return 1; 
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddHtmlDir(const char * HtmlDir){

//  look in HtmlDir for files ending with .html and call
//  AddHtmlFile for matching files

//   cout << "htmldir " << HtmlDir << endl;
   if(!HtmlDir)return -1;
   fHtmlDir = HtmlDir; 
   fHtmlDir += "/";
   const char *fname;
   Int_t nfiles = 0;
   void* dirp=gSystem->OpenDirectory(HtmlDir);
   TRegexp endwith_html("\\.html$");
   while ((fname=gSystem->GetDirEntry(dirp))) {
      TString sname(fname);
      if(sname.Index(endwith_html) >= 0){
         sname.Prepend("/");
         sname.Prepend(HtmlDir);
//         cout << "file-----------> " << fname << endl;
         Int_t ret = AddHtmlFile(sname.Data());
         if(ret > 0)nfiles++;
      }
   }
   gSystem->FreeDirectory(dirp);
   return nfiles;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddGifToRoot(const char * Input){
//
//  look in GifDir for files ending with .gif  or .root
//  and add to fRootFile

   if(!Input || !fRootFile)return -1;
   fRootFile->cd();
   TString sname;
   const char * fname;
   Int_t nfiles = 0;
   TRegexp endwithgif("\\.gif$");
   TRegexp endwithroot("\\.root$");

   Long_t id, flags, modtime;
   Long64_t size;
   gSystem->GetPathInfo(Input,&id, &size, &flags, &modtime);
   if(flags && 2 == 0){                            // real file 
      sname = Input;
      if(sname.Index(endwithgif) > 0){
         AddGifFile(sname.Data());
         nfiles++;
      } else if(sname.Index(endwithroot) >= 0){
         AddCanvas(sname.Data());
         nfiles++;
      } else {
         cout << "Illegal file type " << Input << endl;
      }
   } else {  
      void * dirp = gSystem->OpenDirectory(Input);
      while ((fname=gSystem->GetDirEntry(dirp))) {
         sname = fname;
         sname.Prepend("/");
         sname.Prepend(Input);
         if(sname.Index(endwithgif) >= 0){
            AddGifFile(sname.Data());
            nfiles++;
         }
         if(sname.Index(endwithroot) >= 0){
            AddCanvas(sname.Data());
            nfiles++;
         }
      }
      gSystem->FreeDirectory(dirp);
   }
   gDirectory = gROOT;
   return nfiles;
}
//________________________________________________________________________________

void TMrbHelpBrowser::AddGifFile(const char * GifFile){
    cout << " AddGifFile(" << GifFile<< ")" << endl;
   TNamedBytes * gifdata = new TNamedBytes(GifFile);
   fRootFile->cd();
   fRootFile->SetCompressionLevel(0);
   gifdata->Write();
   delete gifdata;
}
//________________________________________________________________________________

void TMrbHelpBrowser::AddCanvas(const char * RootFile){
//
//  extract canvas object from a ROOT file and add to doc file
//
   TFile * rfile = new TFile(RootFile);
   rfile->ls();
   TString cname(gSystem->BaseName(RootFile));
   TRegexp endwithroot("\\.root$");
   cname(endwithroot) = ""; 
   TCanvas * c1 = (TCanvas*)rfile->Get(cname.Data());
   if(c1){
      fRootFile->cd();
      c1->Write();
   } else {
      cout << "Canvas not found in " << cname.Data() << endl;
   }
   rfile->Close();
}
//______________________________________________________________________________

void TMrbHelpBrowser::CanvasClosed()
{
	if (gDebug > 0) {
		cout << "TMrbHelpBrowser::CanvasClosed()" << endl;
	}
//	if ( fCanvasList->FindObject(obj) ) {
//		cout << "TMrbHelpBrowser::Remove " << obj << endl;
//		fCanvasList->Remove(obj);
//	}
}
//______________________________________________________________________________

void TMrbHelpBrowser::RecursiveRemove(TObject * obj)
{
//	if (gDebug > 2) {
//		cout << "TMrbHelpBrowser::RecursiveRemove " << obj << endl;
//		if (obj) cout << " " << obj->GetName();
//		cout  << endl;
//	}
	if ( fCanvasList->FindObject(obj) ) {
		cout << "TMrbHelpBrowser::Remove " << obj << endl;
		fCanvasList->Remove(obj);
	}
}
//________________________________________________________________________________

void TMrbHelpBrowser::Clear(){
//
// remove all help - canvases from screen
	TIter next(fCanvasList);
	TCanvas * htc;
	while ( (htc =(TCanvas *)next()) ) {
		TRootCanvas *rc = (TRootCanvas*)htc->GetCanvasImp();
		if (rc != NULL)
			rc->SendCloseMessage();
	}
	fCanvasList->Clear("nodelete");
}
//________________________________________________________________________________

Bool_t TMrbHelpBrowser::ToRootFile(const char * InputDir, const char * RootFile, 
                  const char * option){
//
//  add html and gif files in directory InputDir to a ROOT file.
//  In this way a single root file containing a complete documentation
//  may be constructed

   if(InputDir && RootFile){
      fRootFile = new TFile(RootFile, option);
      AddHtml(InputDir);
      fRootFile->SetCompressionLevel(1);
      fHelpList->Write();
      AddGifToRoot(InputDir);
      fRootFile->Close();
      fRootFile = 0;
      return kTRUE;
   } else {
      cout << "No Input or RootFile given to ToRootFile" << endl;
      return kFALSE;
   }
}
//________________________________________________________________________________

void TMrbHelpBrowser::Print(TString * option) const {
//
//  print a list of currently known help entries
   Int_t ind = 0;
   cout << "-----------------------------------------------" << endl;
   cout << "Browser name: " << GetName() << endl;
   if(fRootFile){
      cout << "Input from RootFile: " << endl;
      fRootFile->ls();
   } else {
      cout << "Help Entries: " << fHelpList->GetSize() << endl;
      if(option)cout << "option " << *option << endl;
      TIter next(fHelpList);
      while(TNamedString * ns = (TNamedString *)next()){
         cout << ind++ << " - " << ns->GetName() << endl;
         if(option && option->Index("v", 1, 0, TString::kIgnoreCase) >= 0){
           cout << " -----------------------------------------------" << endl;
           cout << ns->GetText() << endl;
           cout << " -----------------------------------------------" << endl;
         }         
      }
   }
   return;
}
//________________________________________________________________________________

Bool_t  TMrbHelpBrowser::Squeeze_White_Space(TString & s){
//  Squeeze out tabs and blanks from TString
   Bool_t foundone = kFALSE;
   while(1){
      Int_t ind_space = s.Index(" ", 1, 0, TString::kExact);
      if(ind_space < 0)break;
      s.Remove(ind_space,1);
      foundone = kTRUE;
   }
   while(1){
      Int_t ind_tab = s.Index("\t", 1, 0, TString::kExact);
      if(ind_tab < 0)break;
      s.Remove(ind_tab,1);
      foundone = kTRUE;
   }
   return foundone;
};
//________________________________________________________________________________

Bool_t TMrbHelpBrowser::Squeeze_EOL(TString & s){
//  Squeeze out end_of_lines from TString
   Bool_t foundone = kFALSE;
   while(1){
      Int_t ind_EOL = s.Index("\n", 1, 0, TString::kExact);
      if(ind_EOL < 0)break;
//      cout << "EOL in " << s << endl;
      s.Remove(ind_EOL,1);
      foundone = kTRUE;
   }
   return foundone;
};
//_________________________________________________________________________

Int_t TMrbHelpBrowser::NextLine(TString & text,  TString & line, Bool_t start){

// find next line in text, return length, if index = 0 start at begin,
// return -1 if at end of text

   Int_t len = text.Length();
   static Int_t sind, ind;
   if(start){ind = -1; sind = 0;};
   if(ind >= len)return -1;
   sind = ind + 1;
   ind = text.Index("\n", 1, sind, TString::kExact);
   if(ind == sind){
      line = "";
      return 0;
   } 
   if(ind < 0)ind = len;
   line = text(sind,ind-sind);
   return ind-sind;
}  
//________________________________________________________________________________

Int_t TMrbHelpBrowser::NextTag(TString & text,  TString & tag, Int_t search_from){

// return next tag <xxx> in text , if start == true at begin,
// return -1 if at end of text
   Int_t len = text.Length();
   if(search_from >= len)return -1;
   Int_t tag_start = text.Index("<", 1, search_from, TString::kExact);
   if(tag_start >= 0) {

      Int_t tag_end = text.Index(">", 1, tag_start + 1, TString::kExact); 
      if(tag_end < 0){
         cout << "tag end not found " << endl;
         return -2;
      }
      tag = text(tag_start, tag_end - tag_start + 1);
   }
   return tag_start;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::NextAnchor(TString & text,  TString & anchor, Int_t search_from){

// return next anchor <a xxx>yyy</a> in text,
// return -1 if at end of text

   Int_t len = text.Length();
   if(search_from >= len)return -1;
   Int_t tag_start = text.Index("<a", 2, search_from, TString::kExact);
   if(tag_start >= 0) {

      Int_t tag_end = text.Index("</a>", 2, tag_start + 1, TString::kExact); 
      if(tag_end < 0){
         cout << "anchor end not found starting at: " <<  
         tag_start <<  " in:"<< endl;
         TString tt = text(tag_start, 40);
         cout << tt << endl;
         return -2;
      }
      anchor = text(tag_start, tag_end - tag_start + 4);
   }
   return tag_start;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::NextHeading(TString & text,  TString & heading, Int_t search_from){

// return next anchor <a xxx>yyy</a> in text , if start == true at begin,
// return -1 if at end of text

   Int_t len = text.Length();
   if(search_from >= len)return -1;
   Int_t tag_start = text.Index("<H", 2, search_from, TString::kExact);
   if(tag_start >= 0) {

      Int_t tag_end = text.Index("</H", 2, tag_start + 1, TString::kExact); 
      if(tag_end < 0){
         cout << "Heading end not found " << endl;
         return -2;
      }
      heading = text(tag_start, tag_end - tag_start + 5);
   }
   return tag_start;
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::AddTagAtEOL(TString & s){

//  if text enclosed is <t>  ... </t> extends over more than one line
//  insert extra start and end tags (used with <H1> ... </H1> 
   Int_t added = 0;
//  minimal tag - tag end  is <t>xEOLy</t>
   cout << "AddTagAtEOL " << s << endl;
   if(s.Length() < 10) return added;
   TString starttag;
   TString endtag;
   Int_t search_from = 0;
   Int_t start_tag = NextTag(s, starttag, search_from);
   if(start_tag != 0){
      cout << "AddTagAtEOL, start_tag not a 0: " << start_tag << endl;
      return -1;
   }
   cout << "starttag " << starttag<< endl;
   search_from = starttag.Length() + 1;
   NextTag(s, endtag, search_from);
   cout << "endtag " << endtag<< endl;

   while(1){
      Int_t ind_EOL = s.Index("\n", 1, 0, TString::kExact);
      if(ind_EOL < 0)break;
      cout << "EOL in " << s << " ind " << ind_EOL<< endl;
      s.Insert(ind_EOL-1, starttag.Data());
      s.Insert(ind_EOL+1, endtag.Data());
      added = added + starttag.Length() + endtag.Length();
      return added;

    }
   return added;
};
//________________________________________________________________________________

Int_t TMrbHelpBrowser::LineCount(TString & str,
                      Int_t * longest_line, Int_t * nof_heads){
//  find Number of lines and length of longest line
   Int_t nol = 0,  ll = 0, noh = 0;
   Bool_t start = kTRUE;
   TString line;
//   TRegexp tag("<.*>");       // matches things like <Iimg bbb>
   
   while((NextLine(str, line, start)) >=0){
     start = kFALSE;
//     cout << "LineCount: " << line << endl;
     Bool_t intag = kFALSE;
     Int_t len = 0;
     for(Int_t i=0; i < line.Length(); i++){
        if     (line[i] == '<')intag = kTRUE;
        else if(line[i] == '>')intag = kFALSE;
        else if(!intag) len ++;
     }
     if(len > ll) ll = len;
     if (line.Index("<H" , 2, 0, TString::kIgnoreCase) >= 0)noh ++;
     nol++;
   }
   *longest_line = ll;
   *nof_heads = noh;
   return nol;
};
//________________________________________________________________________________

void TMrbHelpBrowser::MoveOrigin(){
   fX0 += 30;
   fY0 += 30;
   if(fX0 + fWwX > fScreen_w)fX0=600;
   if(fY0 > 600)  fY0=50;
}
//________________________________________________________________________________

void TMrbHelpBrowser::PreScan(TString & text, Bool_t keep_references){

//  first remove blanks and EOL in tags <xxx>
//  remove unknown tags
//  remove EOL in anchors <a xxx>yyy</a>
//  remove EOL in <Hn>... </Hn>

   TString tag;
   Int_t start = 0;
   Int_t tag_start;
//  first go: remove blanks and EOL in tags <xxx>
   while(1){
//      cout << "PreScan, text(start,10)" << text(start, 10) << endl;
      tag_start = NextTag(text, tag, start);
      if(tag_start < 0)break;
//      cout << "PreScan, tag" << tag << endl;
      text.Remove(tag_start, tag.Length());
      Squeeze_White_Space(tag);
      Squeeze_EOL(tag);
//    remove style stuff
      if (tag.Index("<style"  , 6, 0,  TString::kIgnoreCase) >= 0 ||
          tag.Index("<title"  , 6, 0,  TString::kIgnoreCase) >= 0)  {
         tag_start = NextTag(text, tag, start);
//        cout << "Prescan: " << tag.Data() << endl;
//        cout << "<style: start, tag_start, len  " << start << " " <<  tag_start << " " << tag.Length() << endl;
//         cout << "Remove: " << text(start,  tag_start - start + tag.Length()) << endl;
         text.Remove(start, tag_start - start + tag.Length());
         tag_start = start;
      }     
//     if tag is known put it back
      if(tag.Index("<aNAME"  , 6, 0,  TString::kIgnoreCase) >= 0 ||
         tag.Index("<aname"  , 6, 0,  TString::kIgnoreCase) >= 0 ||
        (tag.Index("<ahref=" , 7, 0,  TString::kIgnoreCase) >= 0
                                              && keep_references)||
         tag.Index("</a"     , 3, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("<imgSRC=", 8, 0,  TString::kIgnoreCase) >= 0 ||
         tag.Index("<B>"     , 3, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("<I>"     , 3, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("</B>"    , 4, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("</I>"    , 4, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("<H1>"    , 4, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("<H2>"    , 4, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("<H3>"    , 4, 0,  TString::kIgnoreCase) >= 0 ||       
         tag.Index("</H1>"   , 5, 0,  TString::kIgnoreCase) >= 0 || 
         tag.Index("</H2>"   , 5, 0,  TString::kIgnoreCase) >= 0 || 
         tag.Index("</H3>"   , 5, 0,  TString::kIgnoreCase) >= 0  ) {
//           cout << "Prescan: " << tag.Data() << endl;
           text.Insert(tag_start, tag.Data());     
           start = tag_start + tag.Length();
      } else {          
         start = tag_start;
      }
   }
   start = 0;

   while(1){
      tag_start = NextAnchor(text, tag, start);
      if(tag_start < 0)break;
      Int_t tag_length = tag.Length();
      if(Squeeze_EOL(tag)){
         text.Remove(tag_start, tag_length);
//         cout << "anchor |" << tag << "|" << endl;
          text.Insert(tag_start, tag.Data());     
      }
      start = tag_start + tag.Length() + 1;
   }
   start = 0;

   while(1){
      tag_start = NextHeading(text, tag, start);
      if(tag_start < 0)break;
//      Int_t tag_length = tag.Length();
//      cout << "Heading |" << tag << "|" << endl;
//      Int_t added = AddTagAtEOL(tag);
      Int_t tag_length = tag.Length();
      if(Squeeze_EOL(tag)){
         text.Remove(tag_start, tag_length);
//         cout <<  "sqHeading |" << tag << "|" << endl;
          text.Insert(tag_start, tag.Data());     
      }
       start = tag_start + tag.Length() + 1;
   }
}
//________________________________________________________________________________

void TMrbHelpBrowser::DisplayImage(TString & img_name){

//  Display an image, first look if a RootFile is provided
//  with the same name if suffix gif is replaced by root
//  otherwise call xv for a gif file

   TFile * rfile = 0;
   TCanvas * c1 =  0;
   TNamedBytes * gifdata = 0;
   TRegexp endwithgif("\\.gif$");
   TRegexp endwithroot("\\.root$");
   Bool_t good_gif = kFALSE;
   if(img_name.Index(endwithgif) > 0){
      TString gifname(img_name);
      TString rfname(img_name);
      rfname(endwithgif) = ".root";
      TString cname(gSystem->BaseName(rfname.Data()));
      cname(endwithroot) = "";                      
//     if document source is a root file look in it
      if(fRootFile){
         c1 = (TCanvas*)fRootFile->Get(cname.Data());
         if(!c1){
//             not found as canvas, look if it is there as gif
            gifdata = (TNamedBytes*)fRootFile->Get(img_name.Data());
            if(gifdata){
               gifname = "xxx_HelpB_tmp.gif";
               gifdata->ToFile(gifname.Data());
               good_gif = kTRUE;
            } 
         } 
//      else look in normal files      
      } else {      
         if(!gSystem->AccessPathName(rfname.Data())){
    //        cout << "rf: |" << rfname.Data() << "|" << endl;
            rfile = new TFile(rfname.Data());
            c1 = (TCanvas*)rfile->Get(cname.Data());
         } else {
            if(!gSystem->AccessPathName(gifname.Data())) good_gif = kTRUE;
         }
      }
//     first check if a canvas then if a good gif file was found
      if(c1){
         c1->Draw(); 
         fCanvasList->Add(c1);
      } else {
         if(good_gif){
				/*
            TString xv_cmd(img_name);
            xv_cmd += Form(" -geometry +%d+%d &", fX0, fY0);
            xv_cmd.Prepend(" ");
            xv_cmd.Prepend(fGifViewer.Data());
            gSystem->Exec(xv_cmd.Data());
				*/
				TImage * img = TImage::Open(img_name);
				c1 = new TCanvas("helpc", img_name, -fX0, fY0,
									   img->GetWidth()+4, img->GetHeight() + 30);
			   fCanvasList->Add(c1);
				img->Draw("xxx");
            MoveOrigin();
         }
      }
   }
}
//________________________________________________________________________________

void TMrbHelpBrowser::DrawText(const Int_t ind,  Int_t xoff, Int_t yoff)
{
//Retrieve and display helptext by index, this is used when the help item are
//automatically added to a popup menu
   if(!fHelpList){
      cout << "Retrieve by index not possible (yet) with root file" <<endl;
      return;
   }
   if(ind < 0 || ind >= fHelpList->GetSize()){
      cout << "Index out of range: " << ind <<endl;
      return;
   }
   TNamedString * hs = 0;
   hs = (TNamedString *)fHelpList->At(ind);
   if(hs)DrawText(hs->GetName(), xoff, yoff);
} 

//________________________________________________________________________________

void TMrbHelpBrowser::DrawText(const char * hname, Int_t xoff, Int_t yoff)
{

//   This method produces the canvas with the help text:
//
//   Format the help text
//   Insert anchors  
//   Display embedded images in separate windows
	if (gDebug > 0)
		cout << "Enter DrawText(" << hname << ")" << xoff << " " << yoff << endl;
	TString url(hname);
	if ( url.BeginsWith("http") ) {
		url.Prepend("firefox -new-window ");
//		url.Append("&");
		gSystem->Exec(url);
		return;
	}
   TObject * obj = gROOT->GetListOfCanvases()->FindObject(hname);
   if(obj && obj->IsA() == TCanvas::Class()){
       cout << hname << " already on screen " << endl;
       TRootCanvas * rc = (TRootCanvas *)((TCanvas*)obj)->GetCanvasImp();
       rc->RaiseWindow();
       return;
   }
   TCanvas * ca;
   TString address(gSystem->BaseName(hname));
   Int_t hash = address.Index("#");
   if(hash >= 0)address.Remove(0,hash+1); 
//  search in memory resident list
   TNamedString * hs = 0;
   if(fRootFile)hs = (TNamedString *)fRootFile->Get(address);
   else         hs = (TNamedString *)fHelpList->FindObject(address);
   if(!hs){
      cout <<"Cant find help for: " << address << endl;
      return;
   }
//
   Int_t save_optdate = gStyle->GetOptDate();
   gStyle->SetOptDate(0);

   gStyle->SetTextFont(fTextFont + 3);   // char height in pixel
   gStyle->SetTextSize(fTextSize);
   
   Int_t char_width = 5;
   if(fTextSize >= 8)  char_width = 5;
   if(fTextSize >= 10) char_width = 6;
   if(fTextSize >= 12) char_width = 7;
   if(fTextSize >= 14) char_width = 9;
   if(fTextSize >= 18) char_width = 11;
   if(fTextSize >= 24) char_width = 15;

   TString text(hs->GetText());

   Int_t nl = 0;
   Int_t longest_line = 0;
   Int_t nof_heads = 0;
   Int_t line_length = 65;
   Int_t extralines = 1 ;

// find number of lines

   nl = LineCount(text, &longest_line, &nof_heads);
//   max window size (in pixel);
   fWwX = 720;
   fWwY = 720;


   Int_t wwy = (extralines + nl)*(fTextSize + 2) + 4;
//   optimized for char height 18 x 11
//   Int_t wwx_max = (Int_t)((Float_t) fWwX * (Float_t)char_width / 11.);
//   cout << "nl: " << nl << " longest_line " << longest_line 
//        << " wwy " << wwy << endl;

   Int_t wwx_max;
   if (longest_line > line_length) wwx_max = line_length * char_width;
   else                            wwx_max = longest_line * char_width;

   TString canvas_title(hname);
   if (canvas_title.EndsWith(".html"))
     canvas_title.Resize(canvas_title.Length() - 5);
   Int_t xpos, ypos;
   if (xoff >=0 )
		xpos = xoff;
	else 
		xpos = fX0;
   if (yoff >=0 )
		ypos = yoff;
	else 
		ypos = fY0;
	if (gDebug > 0) {
		cout << "xpos, ypos " << xpos<< " " << ypos << endl;
	}
   if(nl <= 36 && longest_line <= line_length){
//     default size, no scroll bar in x  and y required
      ca = new TCanvas(hname, canvas_title, -xpos, ypos, wwx_max, wwy + 60);
   } else { 
      if(wwy < fWwY)fWwY = wwy;
      Int_t wwx =wwx_max;
      
//      if(longest_line > line_length) {
//         wwx = (Int_t) (fWwX * (Float_t)longest_line / line_length);
         wwx = longest_line * char_width;
//      }
      ca = new TCanvas(hname, canvas_title, -xpos, ypos, wwx_max, fWwY + 60);

      if(longest_line <= line_length) wwx = ca->GetWw() - 16;
//      cout << "wwx  " << wwx << " wwx_max  " << wwx_max <<  endl;
      ca->SetCanvasSize(wwx, wwy);
   }
   TRootCanvas *rc = (TRootCanvas *)ca->GetCanvasImp();
   rc->Connect("CloseWindow()", this->ClassName(), this, "CanvasClosed()");
   fCanvasList->Add(ca);
   MoveOrigin();
   ca->AddExec("ex1","TMrbHelpBrowser::HandleMouseClicks()");
   TText * tt = new TText(0,0,"xxxxxxxxxx");
   tt->Draw();
   ca->Modified();
   ca->Update();
   UInt_t tw, th;
   tt->GetBoundingBox(tw, th);
   Float_t clength = 0.1 * gPad->AbsPixeltoX(tw);
//   cout << tw << " " << th << " " << clength << endl;
   delete tt;

   Float_t lspace = 1 / (Float_t)(nl + nof_heads + extralines);   // line spacing 
//   Float_t wh = (Float_t)gPad->XtoPixel(gPad->GetX2());
//   cout << "wh " << gPad->XtoPixel(gPad->GetX2())<<  endl;
//   Float_t clength = char_width / (Float_t)(wh);     // in NDC

   Float_t x0 = 2 * clength;      // left margin
   Float_t yt0 = 1 - lspace;       // top margin, starting y
   Float_t yt  = yt0;
   Float_t xt = x0;               // starting  x 

   TString tag;
   TString reference;
   TString anchortext;
   TString helptext;
   TString restofline;

   Bool_t first_line = kTRUE;
   Bool_t heading = kFALSE;
   Bool_t bold    = kFALSE;
   Bool_t italic  = kFALSE;
   Bool_t its_ref = kFALSE;
   TString line;

//  loop on lines

   while((NextLine(text, line, first_line)) >=0){
      first_line= kFALSE;
//      cout << "yt " << yt  << " line: " << line<< endl;
      xt = x0;
      Int_t pointer = 0;
      Int_t  search_from = 0;
      Int_t tag_start;
//     look fot next tag
      while((tag_start = NextTag(line, tag, search_from)) >=0){
         search_from = tag_start + tag.Length() + 1; 
//         text before tag start
         if(tag_start > pointer){
            restofline = line(pointer, tag_start - pointer);   
//            cout << "Rol |" <<  restofline << "|" << endl;
            tt = new TText(xt, yt, restofline.Data()); 
//            tt->SetText(xt, yt, restofline.Data()); 
//            tt = latex.DrawLatex(xt, yt, restofline.Data()); 
            if(heading){
               tt->SetTextFont(63);  tt->SetTextColor(46);
               if (yt < yt0) {
                  yt -= lspace;
                  tt->SetY(yt);
               }
            }
            if(italic) {tt->SetTextFont(113); tt->SetTextColor(46);}
            if(bold)   {tt->SetTextFont(103); tt->SetTextColor(46);}
            tt->Draw();
            if(its_ref){ 
               tt->SetName(reference.Data());
               tt->SetUniqueID(kTextRef);
               tt->SetTextColor(4);
            } else {
               tt->SetUniqueID(kTextOnly);
            }

//            ca->Modified();
//            ca->Update();
//            tt->GetTextExtent(tw, th, restofline.Data());
//            cout << restofline.Length() << " " << tw << " " << th << endl;
            xt += restofline.Length() * clength;
            its_ref   = kFALSE;
         }
         pointer = tag_start + tag.Length();
//         start of heading / highlight
         if((tag.Index("<H", 2, 0,  TString::kIgnoreCase)) >= 0 ){
            heading = kTRUE;
            continue;
         }
         if((tag.Index("<B>", 3, 0,  TString::kIgnoreCase)) >= 0 ){
            bold = kTRUE;
            continue;
         }
         if((tag.Index("<I>", 3, 0,  TString::kIgnoreCase)) >= 0 ){
            italic = kTRUE;
            continue;
         }
//         end of heading / highlight
         if((tag.Index("</H", 3, 0,  TString::kIgnoreCase)) >= 0 ){
            heading = kFALSE;
            continue;
         }         
         if((tag.Index("</B>", 4, 0,  TString::kIgnoreCase)) >= 0 ){
            bold = kFALSE;
            continue;
         }         
         if((tag.Index("</I>", 4, 0,  TString::kIgnoreCase)) >= 0 ){
            italic = kFALSE;
            continue;
         }         
//         image
         if((tag.Index("<imgSRC=", 8, 0,  TString::kIgnoreCase)) >= 0){
            Int_t img_name_end =  tag.Index("\"",1,9,TString::kExact);
            TString img_name = tag(9, img_name_end - 9);
            DisplayImage(img_name);
            ca->cd();
            continue;
         }         
//         reference
         if((tag.Index("<ahref=",  7, 0,  TString::kIgnoreCase)) >= 0){
            reference = tag(7, tag.Length()-8);
            if(reference.First('#')==0)reference.Remove(0,1);
//            cout << "ar |"<< reference.Data()<< "|" << endl;
            its_ref   = kTRUE;
            continue;
         }         
      } 
//     is there text left
      if(pointer < line.Length()){
         restofline = line(pointer, line.Length() - pointer);   
//         cout << "TextLeft |" << restofline  << "|" << endl;
         tt = new TText(xt, yt, restofline.Data()); 
//         tt->SetText(xt, yt, restofline.Data()); 
//         tt = latex.DrawLatex(xt, yt, restofline.Data()); 
         tt->Draw();
         tt->SetUniqueID(kTextOnly);
         if(heading){tt->SetTextFont(63); tt->SetTextColor(46);}
         if(italic) {tt->SetTextFont(113); tt->SetTextColor(46);}
         if(bold)   {tt->SetTextFont(103); tt->SetTextColor(46);}
      }
      yt -= lspace;
   }
   ca->Modified(kTRUE);
//   TRootCanvas * rc = (TRootCanvas *)ca->GetCanvasImp();
//   rc->RaiseWindow();
   ca->Update(); 
   ca->SetEditable(kFALSE);
   gStyle->SetOptDate(save_optdate);

//   cout << "Exit DrawText ---------------------------" << endl;
}

//________________________________________________________________________________

void TMrbHelpBrowser::HandleMouseClicks()
{
   TText * tt;
   gPad->SetCursor(kPointer);
   int event = gPad->GetEvent();
//   if (event != 1) return;
   
   TObject *select = gPad->GetSelected();
   if (!select) return;
   if (select->InheritsFrom("TText")) {
      tt = (TText*)select;

      if(tt->GetUniqueID() == kTextOnly) return;
      gPad->SetCursor(kHand);
      if(event != 1) return;
      TMrbHelpBrowser * hb = (TMrbHelpBrowser *)gROOT->GetList()->FindObject("TMrbHelpBrowser");
      if(!hb) return;

      TString hname(tt->GetName());
//      cout << "Reference: " << hname << endl;
      if (tt->GetUniqueID() == kTextRef) {     
         hb->DrawText(hname.Data());
         tt->SetTextColor(9);
      }
   }
}
//________________________________________________________________________________

Int_t TMrbHelpBrowser::DisplayMenu(TGPopupMenu * menu, const char * pattern)
{
//Add an entry to a popup menu, pattern may contain wildcards,
//The menu text is taken from a <TITLE> entry in the helptext if found
//otherwise it is the anchor name with a possible extension .html replaced
//by _help
//in the ProcessMessage method for the TGPopupMenu the corresponding code
//should be as e.g. follows (assume hbrowser points to a TMrbHelpBrowser:
//
//Bool_t HTRootCanvas::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
//...
//   switch (GET_MSG(msg)) {
//      case kC_COMMAND:
//         switch (GET_SUBMSG(msg)) {
//            case kCM_MENU:
//
//               if(parm1 >= 100000){
//                  Int_t ind = parm1 - 100000;
//                  if( hbrowser && ind >= 0 && ind < hbrowser->GetNofItems())
//                     hbrowser->DrawText(ind);
//                  break;
//               }

   const Int_t offset = 100000;
   Int_t ind = -1;   
   TRegexp pat(pattern, kTRUE);
   TRegexp html(".html");
   Int_t patlen = strlen(pattern);
   TString sname;
   ifstream * infile;
   TIter next(fHelpList);
   TString * text;
   while(TNamedString * ns = (TNamedString *)next()){
      ind++;
      sname = ns->GetName();
    
      if(patlen > 0){
         if(sname.Index(pat) < 0)continue;
      }  
//      Int_t is = -1;
      Int_t ie = -1;
      TString htmlfile(fHtmlDir.Data());
      htmlfile += "/";
      htmlfile += sname.Data();
     
//      cout << htmlfile.Data() << endl;
      infile = new ifstream(htmlfile.Data());
      if (infile->good()){
         text = new TString();
         text->ReadFile(*infile);
         Int_t is = text->Index("<TITLE>", 0, TString::kIgnoreCase);
//         cout << " is " << is << endl;
         if (is >=0) {
            ie = text->Index("</TITLE>", 0, TString::kIgnoreCase);
            is += 7;
            if (is < ie - 1) sname = (*text)(is, ie - is);
            else ie = -1;
         }
         delete text;
      }
      infile->close();
      if (ie == -1) 
         sname(html) = "_help";
      menu->AddEntry(sname.Data(), offset + ind);
//      cout << ind << " - " <<sname << endl;
      
   }
   return ind+1;
}

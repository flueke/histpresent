#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TObjString.h"
#include "TObject.h"
#include "TStyle.h"
#include "FontSelectDialog.h"
#include "TGMrbValuesAndText.h"
#include "hprbase.h"
#include <iostream>

namespace std {} using namespace std;
//___________________________________________________________________

FontSelectDialog::FontSelectDialog(TGWindow * win)
{
// *INDENT-OFF*
   static const char helptext[] =
"\n\
Set fonts used for GUI elements.\n\
All widgets used in HistPresent are scaled by the\n\
size of the font used (to be precise TextWidth(\"A\"))\n\
This is useful with high resolution displays\n\
With normal displays (1920x1200) widgets are optimized\n\
for \"courier bold 12\"\n\
Family, weights (Bold/Medium) and sizes can be\n\
modified, \"Apply settings\" only effects\n\
the text font in newly created widgets in the current\n\
session. \n\
Fonts used in the menubars are not affected directly.\n\
\n\
\"Save as def Cwd\" writes them into \".rootrc\" in the\n\
current working directory and will be used at the\n\
next start of HistPresent in this directory only.\n\
\n\
\"Save as def Home\" writes them into \".rootrc\" in users\n\
homedirectory. This makes them default for all sessions\n\
for this user\n\
";
// *INDENT-ON*

	if (win) {
		fCanvas = ((TRootCanvas*)win)->Canvas();
		fCanvas->Connect("HTCanvasClosed()", this->ClassName(), this, "CloseDialog()");
	} else {
		fCanvas = NULL;
	}
   fRow_lab = new TList();
   RestoreDefaults();
   Int_t ind = 0;
   Int_t dummy;
   TString savecmd_cwd("SaveDefaultsCwd()");
   TString savecmd_home("SaveDefaultsHome()");
   TString applycmd("Apply()");
   
	fRow_lab->Add(new TObjString("CommentOnly_    Family           Weight           Size"));
	fValp[ind++] = &dummy;
	
   fRow_lab->Add(new TObjString("ComboSelect_Default;courier;helvetica&Used in TextEntry fields"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_Menu   ;courier;helvetica&Used in PopupMenus"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_MenuHi ;courier;helvetica&Used in Labels"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_DocFix ;courier;helvetica&Used in Help widgets"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_DocPro ;courier;helvetica&Used in Help widgets"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_Icon   ;courier;helvetica&Used in Icons"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   fRow_lab->Add(new TObjString("ComboSelect_Status ;courier;helvetica&Used in Event status display"));
   fRow_lab->Add(new TObjString("ComboSelect- ;bold;medium"));
   fRow_lab->Add(new TObjString("PlainIntVal- "));
   

   fValp[ind++] = &fDefaultFontFamily ;fValp[ind++] =&fDefaultFontWeight ;fValp[ind++] =&fDefaultFontSize ;
   fValp[ind++] = &fMenuFontFamily    ;fValp[ind++] =&fMenuFontWeight    ;fValp[ind++] =&fMenuFontSize    ;
   fValp[ind++] = &fMenuHiFontFamily  ;fValp[ind++] =&fMenuHiFontWeight  ;fValp[ind++] =&fMenuHiFontSize  ;
   fValp[ind++] = &fDocFixedFontFamily;fValp[ind++] =&fDocFixedFontWeight;fValp[ind++] =&fDocFixedFontSize;
   fValp[ind++] = &fDocPropFontFamily ;fValp[ind++] =&fDocPropFontWeight ;fValp[ind++] =&fDocPropFontSize ;
   fValp[ind++] = &fIconFontFamily    ;fValp[ind++] =&fIconFontWeight    ;fValp[ind++] =&fIconFontSize    ;
   fValp[ind++] = &fStatusFontFamily  ;fValp[ind++] =&fStatusFontWeight  ;fValp[ind++] =&fStatusFontSize  ;
   
   fRow_lab->Add(new TObjString("CommandButt_Save as def Cwd"));
   fValp[ind++] = &savecmd_cwd;
   fRow_lab->Add(new TObjString("CommandButt+Save as def Home"));
   fValp[ind++] = &savecmd_home;
   fRow_lab->Add(new TObjString("CommandButt+Apply settings"));
   fValp[ind++] = &applycmd;
   

   Int_t ok = -1;
   Int_t itemwidth =  53 * TGMrbValuesAndText::LabelLetterWidth();
   fDialog =
      new TGMrbValuesAndText(fCanvas->GetName(), NULL, &ok,itemwidth, win,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//___________________________________________________________________

FontSelectDialog::~FontSelectDialog() {
if (fCanvas)
		fCanvas->Disconnect("HTCanvasClosed()", this,  "CloseDialog()");
};
//___________________________________________________________________

void FontSelectDialog::Apply()
{
	TString fn;
	
	fn = "-*-";
	fn += fDefaultFontFamily;
	fn += "-";
	fn += fDefaultFontWeight;
	fn += "-r-*-*-";
	fn += fDefaultFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fDefaultFont = fn;
	gEnv->SetValue("Gui.DefaultFont", fn);
	
	fn = "-*-";
	fn += fMenuFontFamily;
	fn += "-";
	fn += fMenuFontWeight;
	fn += "-r-*-*-";
	fn += fMenuFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fMenuFont = fn;
	gEnv->SetValue("Gui.MenuFont", fn);
	
	fn = "-*-";
	fn += fMenuHiFontFamily;
	fn += "-";
	fn += fMenuHiFontWeight;
	fn += "-r-*-*-";
	fn += fMenuHiFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fMenuHiFont = fn;
	gEnv->SetValue("Gui.MenuHiFont", fn);
	
	fn = "-*-";
	fn += fDocFixedFontFamily;
	fn += "-";
	fn += fDocFixedFontWeight;
	fn += "-r-*-*-";
	fn += fDocFixedFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fDocFixedFont = fn;
	gEnv->SetValue("Gui.DocFixedFont", fn);
	
	fn = "-*-";
	fn += fDocPropFontFamily;
	fn += "-";
	fn += fDocPropFontWeight;
	fn += "-r-*-*-";
	fn += fDocPropFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fDocPropFont =fn;
	gEnv->SetValue("Gui.DocPropFont", fn);
	
	fn = "-*-";
	fn += fIconFontFamily;
	fn += "-";
	fn += fIconFontWeight;
	fn += "-r-*-*-";
	fn += fIconFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fIconFont = fn;
	gEnv->SetValue("Gui.IconFont", fn);
	
	fn = "-*-";
	fn += fStatusFontFamily;
	fn += "-";
	fn += fStatusFontWeight;
	fn += "-r-*-*-";
	fn += fStatusFontSize;
	fn += "-*-*-*-*-*-iso8859-1";
	fStatusFont = fn;
	gEnv->SetValue("Gui.StatusFont", fn);	
}
//___________________________________________________________________

void FontSelectDialog::CloseDialog()
{
//   cout << "FontSelectDialog::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   if ( fRow_lab ){
		fRow_lab->Delete();
		delete fRow_lab;
	}
   delete this;
}
//_______________________________________________________________________

void FontSelectDialog::SaveDefaultsCwd()
{
	SaveDefaults(kEnvLocal);
}
//_______________________________________________________________________

void FontSelectDialog::SaveDefaultsHome()
{
	SaveDefaults(kEnvUser);
}
//_______________________________________________________________________

void FontSelectDialog::SaveDefaults(EEnvLevel level)
{
	Apply();		// generate font names
	/*
   TEnv env(".hprrc");
   env.SetValue("FontSelectDialog.fDefaultFontFamily", fDefaultFontFamily );  
   env.SetValue("FontSelectDialog.fMenuFontFamily",    fMenuFontFamily    );    
   env.SetValue("FontSelectDialog.fMenuHiFontFamily",  fMenuHiFontFamily  );  
   env.SetValue("FontSelectDialog.fDocFixedFontFamily",fDocFixedFontFamily);
   env.SetValue("FontSelectDialog.fDocPropFontFamily", fDocPropFontFamily ); 
   env.SetValue("FontSelectDialog.fIconFontFamily",    fIconFontFamily    );    
   env.SetValue("FontSelectDialog.fStatusFontFamily",  fStatusFontFamily  );
                     
   env.SetValue("FontSelectDialog.fDefaultFontWeight", fDefaultFontWeight ); 
   env.SetValue("FontSelectDialog.fMenuFontWeight",    fMenuFontWeight    );    
   env.SetValue("FontSelectDialog.fMenuHiFontWeight",  fMenuHiFontWeight  );  
   env.SetValue("FontSelectDialog.fDocFixedFontWeight",fDocFixedFontWeight);
   env.SetValue("FontSelectDialog.fDocPropFontWeight", fDocPropFontWeight ); 
   env.SetValue("FontSelectDialog.fIconFontWeight",    fIconFontWeight    );    
   env.SetValue("FontSelectDialog.fStatusFontWeight",  fStatusFontWeight  );  
                     
   env.SetValue("FontSelectDialog.fDefaultFontSize", fDefaultFontSize );  
   env.SetValue("FontSelectDialog.fMenuFontSize",    fMenuFontSize    );     
   env.SetValue("FontSelectDialog.fMenuHiFontSize",  fMenuHiFontSize  );   
   env.SetValue("FontSelectDialog.fDocFixedFontSize",fDocFixedFontSize); 
   env.SetValue("FontSelectDialog.fDocPropFontSize", fDocPropFontSize );  
   env.SetValue("FontSelectDialog.fIconFontSize",    fIconFontSize    );     
   env.SetValue("FontSelectDialog.fStatusFontSize",  fStatusFontSize  );   
   env.SaveLevel(kEnvLocal);
 */  
   TEnv er(".rootrc");
   cout << "SaveDefaults: ";
   if (level  == kEnvLocal)
		cout << "cwd";
   else
 		cout << "home";
   cout << endl;
   er.SetValue("Gui.DefaultFont",  fDefaultFont);   
   er.SetValue("Gui.MenuFont",     fMenuFont);    
   er.SetValue("Gui.MenuHiFont",   fMenuHiFont);  
   er.SetValue("Gui.DocFixedFont", fDocFixedFont);
   er.SetValue("Gui.DocPropFont",  fDocPropFont); 
   er.SetValue("Gui.IconFont",     fIconFont);    
   er.SetValue("Gui.StatusFont",   fStatusFont);
   er.SaveLevel(level);
}
//___________________________________________________________________

void FontSelectDialog::RestoreDefaults()
{
	TObjArray * oo;
	TString fn;
	fn = gEnv->GetValue("Gui.DefaultFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fDefaultFont = fn;
	oo = fn.Tokenize("-");
	fDefaultFontFamily = ((TObjString*)oo->At(1))->String();
//	cout << "Gui.DefaultFont " << fDefaultFontFamily<< endl;
	fDefaultFontWeight = ((TObjString*)oo->At(2))->String();
	fDefaultFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
//	cout << "Gui.DefaultFont Size " << fDefaultFontSize<< endl;
	fn = gEnv->GetValue("Gui.MenuFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fMenuFont = fn;
	oo = fn.Tokenize("-");
	fMenuFontFamily = ((TObjString*)oo->At(1))->String();
	fMenuFontWeight = ((TObjString*)oo->At(2))->String();
	fMenuFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
	fn = gEnv->GetValue("Gui.MenuHiFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fMenuHiFont = fn;
	oo = fn.Tokenize("-");
	fMenuHiFontFamily = ((TObjString*)oo->At(1))->String();
	fMenuHiFontWeight = ((TObjString*)oo->At(2))->String();
	fMenuHiFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
	fn = gEnv->GetValue("Gui.DocFixedFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fDocFixedFont = fn;
	oo = fn.Tokenize("-");
	fDocFixedFontFamily = ((TObjString*)oo->At(1))->String();
	fDocFixedFontWeight = ((TObjString*)oo->At(2))->String();
	fDocFixedFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
	fn = gEnv->GetValue("Gui.DocPropFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fDocPropFont = fn;
	oo = fn.Tokenize("-");
	fDocPropFontFamily = ((TObjString*)oo->At(1))->String();
	fDocPropFontWeight = ((TObjString*)oo->At(2))->String();
	fDocPropFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
	fn = gEnv->GetValue("Gui.IconFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fIconFont = fn;
	oo = fn.Tokenize("-");
	fIconFontFamily = ((TObjString*)oo->At(1))->String();
	fIconFontWeight = ((TObjString*)oo->At(2))->String();
	fIconFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
	fn = gEnv->GetValue("Gui.StatusFont",
						"-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fStatusFont = fn;
	oo = fn.Tokenize("-");
	fStatusFontFamily = ((TObjString*)oo->At(1))->String();
	fStatusFontWeight = ((TObjString*)oo->At(2))->String();
	fStatusFontSize   =	((TObjString*)oo->At(6))->String().Atoi();
/*	
   TEnv env(".hprrc");
	fDefaultFontFamily  = env.GetValue("FontSelectDialog.fDefaultFontFamily", fDefaultFontFamily );  
	fMenuFontFamily     = env.GetValue("FontSelectDialog.fMenuFontFamily",    fMenuFontFamily    );    
	fMenuHiFontFamily   = env.GetValue("FontSelectDialog.fMenuHiFontFamily",  fMenuHiFontFamily  );  
	fDocFixedFontFamily = env.GetValue("FontSelectDialog.fDocFixedFontFamily",fDocFixedFontFamily);
	fDocPropFontFamily  = env.GetValue("FontSelectDialog.fDocPropFontFamily", fDocPropFontFamily ); 
	fIconFontFamily     = env.GetValue("FontSelectDialog.fIconFontFamily",    fIconFontFamily    );    
	fStatusFontFamily   = env.GetValue("FontSelectDialog.fStatusFontFamily",  fStatusFontFamily  );
								
	fDefaultFontWeight  = env.GetValue("FontSelectDialog.fDefaultFontWeight", fDefaultFontWeight ); 
	fMenuFontWeight     = env.GetValue("FontSelectDialog.fMenuFontWeight",    fMenuFontWeight    );    
	fMenuHiFontWeight   = env.GetValue("FontSelectDialog.fMenuHiFontWeight",  fMenuHiFontWeight  );  
	fDocFixedFontWeight = env.GetValue("FontSelectDialog.fDocFixedFontWeight",fDocFixedFontWeight);
	fDocPropFontWeight  = env.GetValue("FontSelectDialog.fDocPropFontWeight", fDocPropFontWeight ); 
	fIconFontWeight     = env.GetValue("FontSelectDialog.fIconFontWeight",    fIconFontWeight    );    
	fStatusFontWeight   = env.GetValue("FontSelectDialog.fStatusFontWeight",  fStatusFontWeight  );  
								
	fDefaultFontSize    = env.GetValue("FontSelectDialog.fDefaultFontSize", fDefaultFontSize );  
	fMenuFontSize       = env.GetValue("FontSelectDialog.fMenuFontSize",    fMenuFontSize    );     
	fMenuHiFontSize     = env.GetValue("FontSelectDialog.fMenuHiFontSize",  fMenuHiFontSize  );   
	fDocFixedFontSize   = env.GetValue("FontSelectDialog.fDocFixedFontSize",fDocFixedFontSize); 
	fDocPropFontSize    = env.GetValue("FontSelectDialog.fDocPropFontSize", fDocPropFontSize );  
	fIconFontSize       = env.GetValue("FontSelectDialog.fIconFontSize",    fIconFontSize    );     
	fStatusFontSize     = env.GetValue("FontSelectDialog.fStatusFontSize",  fStatusFontSize  );   
 */
}
//___________________________________________________________________

void FontSelectDialog::CloseDown(Int_t /*wid*/)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
//   if (wid == -1)
 //     SaveDefaults();
}
//______________________________________________________________________

void FontSelectDialog::CRButtonPressed(Int_t /*wid*/, Int_t /*bid*/, TObject */*obj*/)
{
//   TCanvas *canvas = (TCanvas *)obj;
//  cout << "CRButtonPressed(" << wid<< ", " <<bid;
//   if (obj) cout  << ", " << canvas->GetName() << ")";
//   cout << endl;
 //  SetWindowSizeNow(canvas);
}


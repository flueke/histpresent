
#define ENABLE_CURLYDEFAULTS 40304

#include "TROOT.h"
#include "TEnv.h"
#include "TColor.h"
#include "TRootCanvas.h"
#include "TGaxis.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TFrame.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "TArc.h"
#include "TMarker.h"
#include "TLatex.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "Buttons.h"

#include "TGMrbValuesAndText.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "TMrbHelpBrowser.h"
#include "support.h"
#include "SetHistOptDialog.h"
#include "Set1DimOptDialog.h"
#include "Set2DimOptDialog.h"
#include "SetColorModeDialog.h"
#include "SetCanvasAttDialog.h"
#include "WhatToShowDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "GraphAttDialog.h"
#include <iostream>
#include <fstream>
#include <sstream>

//_______________________________________________________________________

void HistPresent::RestoreOptions()
{
   TEnv env(".hprrc");
   fGraphFile =
       env.GetValue("HistPresent.GraphFile", "gdata.asc");
   
   const char hist_file[] = {"ntupleCuts.txt"};
	fstream ntc(hist_file);
	if (ntc.is_open()) ntc >> *fLeafCut;
   const char cmd_file[] = {"ntupleCmds.txt"};
	fstream cmds(cmd_file);
	if (cmds.is_open()) cmds >> *fExpression;
	
   fRememberTreeHists  = env.GetValue("HistPresent.RememberTreeHists", 0);
   fNtupleVersioning   = env.GetValue("HistPresent.NtupleVersioning", 1);
	fNtuplePrependTN    = env.GetValue("HistPresent.fNtuplePrependTN", 0);
	fNtuplePrependFN    = env.GetValue("HistPresent.fNtuplePrependFN", 0);
	fAppendSelections   = env.GetValue("HistPresent.fAppendSelections", 1);
	fAlwaysRequestLimits= env.GetValue("HistPresent.AlwaysRequestLimits", 1);
   fAlwaysFindLimits   = env.GetValue("HistPresent.AlwaysFindLimits", 1);
	fWriteLeafMinMaxtoFile = env.GetValue("HistPresent.fWriteLeafMinMaxtoFile", 0);
	fWriteLeafNamesToFile  = env.GetValue("HistPresent.fWriteLeafNamesToFile", 0);
   f2dimAsGraph        = env.GetValue("HistPresent.f2dimAsGraph", 0);
   fMarkStyle          = env.GetValue("HistPresent.fMarkStyle", 20);
   fMarkSize           = env.GetValue("HistPresent.fMarkSize", 1);
   fMarkColor          = env.GetValue("HistPresent.fMarkColor", 1);
   fHistSelMask        = env.GetValue("HistPresent.fHistSelMask", "");
   fFileSelMask        = env.GetValue("HistPresent.fFileSelMask", "");
   fUseFileSelMask     = env.GetValue("HistPresent.fUseFileSelMask", 0);
   fUseHistSelMask     = env.GetValue("HistPresent.fUseHistSelMask", 0);
   fUseLeafSelMask     = env.GetValue("HistPresent.fUseLeafSelMask", 0);
   fUseCanvasSelMask   = env.GetValue("HistPresent.fUseCanvasSelMask", 0);
   fUseFileSelFromRun  = env.GetValue("HistPresent.fUseFileSelFromRun", 0);
   fFileSelFromRun     = env.GetValue("HistPresent.fFileSelFromRun", -1);
   fFileSelToRun       = env.GetValue("HistPresent.fFileSelToRun", -1);
	fLeafSelMask        = env.GetValue("HistPresent.fLeafSelMask", "");
	fCanvasSelMask      = env.GetValue("HistPresent.fCanvasSelMask", "");
	fHistUseRegexp      = env.GetValue("HistPresent.fHistUseRegexp", 0);
	fFileUseRegexp      = env.GetValue("HistPresent.fFileUseRegexp", 0);
	fLeafUseRegexp      = env.GetValue("HistPresent.fLeafUseRegexp", 0);
	fCanvasUseRegexp    = env.GetValue("HistPresent.fCanvasUseRegexp", 0);
	fRealStack          = env.GetValue("HistPresent.fRealStack", 1);
   fLogScaleMin        = atof(env.GetValue("HistPresent.LogScaleMin", "0.1"));
   fLinScaleMin		  = atof(env.GetValue("HistPresent.LinScaleMin", "0"));

   *fHostToConnect =
       env.GetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   fSocketToConnect =
       env.GetValue("HistPresent.SocketToConnect", fSocketToConnect);

   fFuncColor=    env.GetValue("HistPresent.FuncColor", 4);
   fFuncStyle=    env.GetValue("HistPresent.FuncStyle", 1);
   fFuncWidth=    env.GetValue("HistPresent.FuncWidth", 2);


   fEditUsXlow   = env.GetValue("HistPresent.EditUsXlow",    550);
   fEditUsYlow   = env.GetValue("HistPresent.EditUsYlow",    5  );
   fEditUsXwidth = env.GetValue("HistPresent.EditUsXwidth",  500);
   fEditUsYwidth = env.GetValue("HistPresent.EditUsYwidth",  524);
   fEditUsXRange = env.GetValue("HistPresent.EditUsXRange",  100.);
//  numbers below are for 1600 x 1200 screens
//   Int_t  screen_x, screen_y;
//   UInt_t wwu, whu;
//   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
//            	  screen_x, screen_y, wwu, whu);
//   Float_t fac = (Float_t)wwu / 1600.;
	
   fEditLsXlow   = env.GetValue("HistPresent.EditLsXlow",   550);
   fEditLsYlow   = env.GetValue("HistPresent.EditLsYlow",   5   );
	fEditLsXwidth = env.GetValue("HistPresent.EditLsXwidth", 936);
	fEditLsYwidth = env.GetValue("HistPresent.EditLsYwidth", 646);
   fEditLsXRange = env.GetValue("HistPresent.EditLsXRange", 297. );

   fEditPoXlow   = env.GetValue("HistPresent.EditPoXlow",   550);
   fEditPoYlow   = env.GetValue("HistPresent.EditPoYlow",   5  );
	fEditPoXwidth = env.GetValue("HistPresent.EditPoXwidth", 646);
	fEditPoYwidth = env.GetValue("HistPresent.EditPoYwidth", 936);
   fEditPoXRange = env.GetValue("HistPresent.EditPoXRange", 210.);
	
//   CheckAutoExecFiles();
//

   SetCanvasAttDialog::SetDefaults();
   SetHistOptDialog::SetDefaults();
   Set1DimOptDialog::SetDefaults();
   WhatToShowDialog::SetDefaults();
   WindowSizeDialog::RestoreDefaults();
   WindowSizeDialog::fNwindows = 0;
//   GraphAttDialog::RestoreDefaults();
   SetColorModeDialog::RestoreDefaults();
   SetColorModeDialog::SetGreyLevels();
   SetColorModeDialog::SetTransLevelsRGB();
   SetColorModeDialog::SetTransLevelsHLS();
   SetColorModeDialog::SetColorMode();
   GeneralAttDialog::RestoreDefaults();
   if (GeneralAttDialog::fForceStyle > 0) gROOT->ForceStyle();
   else                 gROOT->ForceStyle(kFALSE);

   gEnv->SetValue("Root.Stacktrace", 0);
	gStyle->SetFrameBorderMode(0);    // magic, otherwise superimpose 3dim fails
}

//_______________________________________________________________________

void HistPresent::SaveOptions()
{
   TEnv env(".hprrc");
   env.SetValue("HistPresent.fRealStack", fRealStack);
   env.SetValue("HistPresent.RememberTreeHists", fRememberTreeHists);
   env.SetValue("HistPresent.AlwaysRequestLimits", fAlwaysRequestLimits);
   env.SetValue("HistPresent.AlwaysFindLimits", fAlwaysFindLimits);
   env.SetValue("HistPresent.f2dimAsGraph", f2dimAsGraph);
	env.SetValue("HistPresent.fWriteLeafMinMaxtoFile", fWriteLeafMinMaxtoFile);
	env.SetValue("HistPresent.fWriteLeafNamesToFile", fWriteLeafNamesToFile);
	env.SetValue("HistPresent.fMarkStyle",fMarkStyle);
   env.SetValue("HistPresent.fMarkSize", fMarkSize );
   env.SetValue("HistPresent.fMarkColor",fMarkColor);
   env.SetValue("HistPresent.fHistSelMask",fHistSelMask);
	env.SetValue("HistPresent.fLeafSelMask",fLeafSelMask);
	env.SetValue("HistPresent.fFileSelMask",fFileSelMask);
   env.SetValue("HistPresent.fUseFileSelFromRun", fUseFileSelFromRun);
   env.SetValue("HistPresent.fFileSelFromRun", fFileSelFromRun);
   env.SetValue("HistPresent.fFileSelToRun", fFileSelToRun);
	env.SetValue("HistPresent.fCanvasSelMask",fCanvasSelMask);
   env.SetValue("HistPresent.fUseFileSelMask",   fUseFileSelMask  );
   env.SetValue("HistPresent.fUseHistSelMask",   fUseHistSelMask  );
   env.SetValue("HistPresent.fUseLeafSelMask",   fUseLeafSelMask  );
   env.SetValue("HistPresent.fUseCanvasSelMask", fUseCanvasSelMask);
	
	env.SetValue("HistPresent.fHistUseRegexp",fHistUseRegexp);
	env.SetValue("HistPresent.fLeafUseRegexp",fLeafUseRegexp);
	env.SetValue("HistPresent.fFileUseRegexp",fFileUseRegexp);
	env.SetValue("HistPresent.fCanvasUseRegexp",fCanvasUseRegexp);
	
	env.SetValue("HistPresent.NtupleVersioning", fNtupleVersioning);
	env.SetValue("HistPresent.fNtuplePrependTN", fNtuplePrependTN);
	env.SetValue("HistPresent.fNtuplePrependFN", fNtuplePrependFN);
	env.SetValue("HistPresent.fAppendSelections", fAppendSelections);
	env.SetValue("HistPresent.GraphFile", fGraphFile.Data());
   env.SetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   env.SetValue("HistPresent.SocketToConnect", fSocketToConnect);
   env.SetValue("HistPresent.LinScaleMin", fLinScaleMin);
   env.SetValue("HistPresent.LogScaleMin", fLogScaleMin);

   env.SetValue("HistPresent.FuncColor",     fFuncColor    );
   env.SetValue("HistPresent.FuncStyle",     fFuncStyle    );
   env.SetValue("HistPresent.FuncWidth",     fFuncWidth    );

   env.SetValue("HistPresent.EditUsXlow",   fEditUsXlow  );
   env.SetValue("HistPresent.EditUsYlow",   fEditUsYlow  );
   env.SetValue("HistPresent.EditUsXwidth", fEditUsXwidth);
   env.SetValue("HistPresent.EditUsYwidth", fEditUsYwidth);
   env.SetValue("HistPresent.EditUsXRange", fEditUsXRange);

   env.SetValue("HistPresent.EditLsXlow",   fEditLsXlow  );
   env.SetValue("HistPresent.EditLsYlow",   fEditLsYlow  );
   env.SetValue("HistPresent.EditLsXwidth", fEditLsXwidth);
   env.SetValue("HistPresent.EditLsYwidth", fEditLsYwidth);
   env.SetValue("HistPresent.EditLsXRange", fEditLsXRange);

   env.SetValue("HistPresent.EditPoXlow",   fEditPoXlow  );
   env.SetValue("HistPresent.EditPoYlow",   fEditPoYlow  );
   env.SetValue("HistPresent.EditPoXwidth", fEditPoXwidth);
   env.SetValue("HistPresent.EditPoYwidth", fEditPoYwidth);
   env.SetValue("HistPresent.EditPoXRange", fEditPoXRange);
   env.SaveLevel(kEnvLocal);
   GeneralAttDialog::SaveDefaults();
//   cout << "env.SaveLevel(kEnvLocal) " << endl;
}


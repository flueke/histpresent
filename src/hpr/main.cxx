#include "TROOT.h"
#include "TRint.h"
#include "TEnv.h"
#include <iostream>
#include "HistPresent.h"
#include "SetColor.h"

int main(int argc, char **argv)
{
   gSystem->Load("libHistPainter.so");
   HistPresent *hp;
   gEnv->SetValue("Gui.IconPath", ".:$HOME/icons:$MARABOU/icons:$ROOTSYS/icons:");
   TRint *theApp = new TRint("App", &argc, argv);
   TDatime dt;
   
   cout << "Starting HistPresent at: " << dt.AsSQLString()<< endl;
   hp = new HistPresent("mypres","mypres");
   hp->ShowMain();
   if(!gSystem->AccessPathName(attrname, kFileExists))
      gROOT->LoadMacro(attrname);
//loop:
   theApp->Run(kFALSE);
//	gSystem->Exit(0);
}


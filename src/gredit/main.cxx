#include "TROOT.h"
#include "TRint.h"
#include "TEnv.h"
#include <iostream>
#include "CreateCDlabel.h"

int main(int argc, char **argv)
{
   TRint *theApp = new TRint("App", &argc, argv);
   new CreateCDlabel();
   theApp->Run(kTRUE);
   return 0;
}


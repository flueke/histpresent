#include "TBuffer.h"
#include "TAttText.h"
#include "TCutG.h"
#include "TGraph.h"

#include "TMrbWdw.h"

void TMrbWindow2D::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMrbWindow2D.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); 
      if (R__v > 1) { 
         cout << "TMrbWindow2D TCutG streamer " << endl;
         TCutG::Streamer(R__b);
      } else {
         cout << "TMrbWindow2D TGraph streamer " << endl;
         R__v = R__b.ReadVersion(); if (R__v) { }
         TGraph::Streamer(R__b);
         fVarX.Streamer(R__b);
         fVarY.Streamer(R__b);
      }
      TAttText::Streamer(R__b);
      R__b >> fXtext;
      R__b >> fYtext;
   } else {
      R__b.WriteVersion(TMrbWindow2D::IsA());
      TCutG::Streamer(R__b);
//      cout << "TMrbWindow2D TCutG streamer, writing" << endl;
//      TGraph::Streamer(R__b);
//      fVarX.Streamer(R__b);
//      fVarY.Streamer(R__b);
      TAttText::Streamer(R__b);
      R__b << fXtext;
      R__b << fYtext;
   }
}
//______________________________________________________________________________
void TMbsWindow2d::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMbsWindow2d.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      cout << "TMbsWindow2d  new  streamer " << endl;
//      TCutG::Streamer(R__b);
      TGraph::Streamer(R__b);
      fVarX.Streamer(R__b);
      fVarY.Streamer(R__b);
      TAttText::Streamer(R__b);
      R__b >> fXtext;
      R__b >> fYtext;
   } else {
      R__b.WriteVersion(TMbsWindow2d::IsA());
//      TCutG::Streamer(R__b);
      TGraph::Streamer(R__b);
      fVarX.Streamer(R__b);
      fVarY.Streamer(R__b);
      TAttText::Streamer(R__b);
      R__b << fXtext;
      R__b << fYtext;
   }
}

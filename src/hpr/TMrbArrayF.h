#ifndef TMRBARRAYF
#define  TMRBARRAYF
#include "TArrayF.h"

class TMrbArrayF : public TArrayF {
private:
   Float_t fLowEdge;
   Float_t fBinWidth;

public:
   TMrbArrayF(Int_t n);
   TMrbArrayF(Int_t n, const Float_t * array) ;
   ~TMrbArrayF(){};

   void SetLowEdge(Float_t le){fLowEdge = le;};
   void SetBinWidth(Float_t bw){fBinWidth = bw;};
   Stat_t GetMean();
   Stat_t GetRMS();

ClassDef(TMrbArrayF,0)
};      
#endif

#ifndef TMRBARRAYD
#define  TMRBARRAYD
#include "TArrayD.h"


class TMrbArrayD : public TArrayD {
private:
   Double_t fLowEdge;
   Double_t fBinWidth;
public:
   TMrbArrayD(Int_t n);
   TMrbArrayD(Int_t n, const Double_t * array);
   ~TMrbArrayD(){};
   void SetLowEdge(Double_t le){fLowEdge = le;};
   void SetBinWidth(Double_t bw){fBinWidth = bw;};
   Stat_t GetMean();
   Stat_t GetRMS();

ClassDef(TMrbArrayD,0)
};      
#endif

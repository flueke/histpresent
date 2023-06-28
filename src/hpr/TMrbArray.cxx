#include "TMrbArrayF.h"
#include "TMrbArrayD.h"
#include "TMath.h"

ClassImp(TMrbArrayF)
ClassImp(TMrbArrayD)

TMrbArrayF::TMrbArrayF(Int_t n) : TArrayF(n), fLowEdge(0), fBinWidth(1) {};
TMrbArrayF::TMrbArrayF(Int_t n, const Float_t * array) 
          : TArrayF(n, array), fLowEdge(0), fBinWidth(1){};

Stat_t TMrbArrayF::GetMean()
{
   Stat_t sum = GetSum();
   if (sum == 0)return 0;
   Stat_t sumx = 0;
   Stat_t bincenter =  0.5 * fBinWidth + fLowEdge;
   for (Int_t i= 0; i < GetSize(); i++, bincenter += fBinWidth) {
      sumx += bincenter * At(i);
   }
   return sumx / sum;
}

Stat_t TMrbArrayF::GetRMS()
{
   Stat_t sum = GetSum();
   if (sum == 0)return 0;
   Stat_t sumx2  = 0;
   Int_t non_zero = 0;
   Stat_t bincenter =  0.5 * fBinWidth + fLowEdge;
   for (Int_t i= 0; i <  GetSize(); i++, bincenter += fBinWidth) {
      if (At(i) != 0) non_zero++;
      sumx2 += bincenter * bincenter * At(i);

   }
   Stat_t mean = GetMean();
   Stat_t diff = sumx2 / sum -  mean * mean;
   if (non_zero <= 1 || diff <= 0) return 0;
   else           return TMath::Sqrt(diff);
}
//______________________________________________________________________________

TMrbArrayD::TMrbArrayD(Int_t n) : TArrayD(n), fLowEdge(0), fBinWidth(1) {};
TMrbArrayD::TMrbArrayD(Int_t n, const Double_t * array) 
           : TArrayD(n, array), fLowEdge(0), fBinWidth(1) {};
Stat_t TMrbArrayD::GetMean()
{
   Stat_t sum = GetSum();
   if (sum == 0)return 0;
   Stat_t sumx = 0;
   Stat_t bincenter =  0.5 * fBinWidth + fLowEdge;
   for (Int_t i= 0; i < GetSize(); i++, bincenter += fBinWidth) {
      sumx += bincenter * At(i);
   }
   return sumx / sum;
}

Stat_t TMrbArrayD::GetRMS()
{
   Stat_t sum = GetSum();
   if (sum == 0)return 0;
   Stat_t sumx2  = 0;
   Int_t non_zero = 0;
   Stat_t bincenter =  0.5 * fBinWidth + fLowEdge;
   for (Int_t i= 0; i <  GetSize(); i++, bincenter += fBinWidth) {
      if (At(i) != 0) non_zero++;
      sumx2 += bincenter * bincenter * At(i);
   }
   Stat_t mean = GetMean(); 
   Stat_t diff = sumx2 / sum -  mean * mean;
   if (non_zero <= 1 || diff <= 0) return 0;
   else           return TMath::Sqrt(diff);
}

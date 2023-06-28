#include "TArrow.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TMarker.h"
#include "TPolyMarker.h"

#include "PeakFinder.h"
#include "FhPeak.h"

#include <iostream>

// Find peaks
Int_t FoldSqareWave(Int_t i, Int_t j,Int_t m){
   if      (i <= j-1 && i >= j-m)     return -1;
   else if (i <= j+m-1 && i >= j)     return  2;
   else if (i <= j+2*m-1 && i >= j+m) return -1; 
   else return 0;
}
//_____________________________________________________________________

PeakFinder::PeakFinder(TH1 * hist, Double_t pwidth, Double_t thresh)
               : fSelHist(hist), fPeakThreshold(thresh)
{
   fPeakMwidth = (Int_t) (pwidth / hist->GetBinWidth(1));
   static Int_t LENINC = 50;
   fPositions.Set(LENINC);
   fWidths.Set(LENINC);
   fHeights.Set(LENINC);
   TPolyMarker *pm;
   pm = (TPolyMarker *) fSelHist->GetListOfFunctions()->FindObject("TPolyMarker");
   if ( pm ) {
      fSelHist->GetListOfFunctions()->Remove(pm);
      delete pm;
   }
   pm = new TPolyMarker();
	pm->SetMarkerColor(4);
	pm->SetMarkerStyle(23);
   fSelHist->GetListOfFunctions()->Add(pm);
   if(fSelHist->GetDimension() != 1){
      cout << "FindPeaks in 2 or 3 dim not yet supported " << endl;
      fPositions.Set(0);
      return;
   }

   Int_t bin, i, L, start=0, wid;
   Stat_t fold, var;
   fNpeaks = 0;
//   cout << "PeakFinder: fPeakMwidth fPeakThreshold: " << fPeakMwidth << " " << fPeakThreshold<< endl;
   Int_t nbins   = fSelHist->GetNbinsX();
   for (bin = 1;bin<=nbins-1;bin++) {
      if(bin > fPeakMwidth+1 &&  bin <= nbins-2*fPeakMwidth-1) {
         fold = 0.;
         var  = 0.;
         for(i=bin-fPeakMwidth; i <= bin+2*fPeakMwidth-1; i++){
            L = FoldSqareWave(i,bin,fPeakMwidth);
            fold += L*fSelHist->GetBinContent(i);
            var += L * L * fSelHist->GetBinContent(i);
         }
         if (var) 
            fold = fold /TMath::Sqrt(var);
         else 
            fold = 0.;
         if(fold < fPeakThreshold) {
            fold=0.;
            if(start){
               wid=bin-start;
//              h_width->Fill(wid);
 //              cout << " bin " << bin << " width " << wid << endl;
               start += (Int_t)(0.5 * fPeakMwidth);
               Double_t maxa = 0;
               Int_t maxp = 0;
               for (Int_t b = start; b <= TMath::Min(start+2 * fPeakMwidth, nbins); b++) {
                  if (fSelHist->GetBinContent(b) > maxa ) {
                     maxa = fSelHist->GetBinContent(b);
                     maxp = b;
                  }
               }
               if(maxp > 0 && wid > 0.5 * fPeakMwidth){
                  Float_t x = fSelHist->GetBinCenter(maxp);
                  Int_t l = fPositions.GetSize();
                  if (fNpeaks >= l) {
                     cout << "PeakFinder fNpeaks" << fNpeaks <<endl;
							fPositions.Set(l + LENINC);
							fWidths.Set(l + LENINC);
							fHeights.Set(l + LENINC);
                     cout << "PeakFinderL  " << fPositions.GetSize() << endl;
                  }   
                  fPositions[fNpeaks] = x;
                  fWidths[fNpeaks] = wid;
                  fHeights[fNpeaks] = maxa;
                  pm->SetPoint(fNpeaks, x, maxa);
                  pm->SetMarkerColor(4);
                  pm->SetMarkerStyle(23);
 //                 pm->Draw();
                  fNpeaks++;
               }
               start = 0;
            }
         } else {
             if(!start) start = bin;
         }
      }
   }
   fPositions.Set(fNpeaks);
}

#ifndef PEAK_FINDER
#define PEAK_FINDER
#include <iostream>
#include <iomanip>
#include "TObject.h"
#include "TArrayD.h"
#include "TArrayF.h"

namespace std {} using namespace std;

class  PeakFinder : public TObject {

private:
   TH1      *fSelHist;
   Int_t    fPeakMwidth;
   Double_t fPeakThreshold;
   Int_t    fNpeaks;
   TArrayF  fPositions;
   TArrayD  fWidths;
   TArrayD  fHeights;
public:
   PeakFinder(){};
   PeakFinder(TH1 * hist, Double_t mwidth, Double_t thresh);

   ~PeakFinder() {};
   Float_t  *GetPositionX() { return fPositions.GetArray(); };
   Double_t *GetWidths() { return fWidths.GetArray(); };
   Double_t *GetHeights() { return fHeights.GetArray(); };
   Int_t     GetNpeaks() { return fNpeaks; };

ClassDef(PeakFinder,0)
};

#endif

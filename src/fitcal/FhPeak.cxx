#include "FhPeak.h"
ClassImp(FhPeak)

FhPeak::FhPeak(Double_t pos) : fEstimate(pos), fMean(pos){
   fNominalEnergy = 0;
   fCalibratedEnergy = 0;
//   fEstimate = 0;
   fContent = 0;
//   fMean = 0;
   fWidth = 0;
   fNominalEnergyError = 0;
   fContentError = 0;
   fMeanError = 0;
   fWidthError = 0;
   fChi2oNdf = 0;
   fTailContent = 0;
   fTailWidth = 0;
   fIntensity = 0;
   fRelEfficiency = 0;
   fUsed = kFALSE;
};
void FhPeak::PrintPeak(ostream & ostr) const {
	ostr << fMean << " " << fWidth << " " << fContent << " " 
	     << fMeanError << " " << fChi2oNdf << endl;
};
void FhPeak::Print(Option_t * /*opt*/) const {
	printf("%10.2f %10.2f %10.2f %10.2f %10.2f", 
			 fMean, fWidth, fContent, fMeanError, fChi2oNdf);
	//    cout << setw(12)<< fContent << setw(12)<< fMean 
	//         << setw(12)<< fMeanError << setw(12)<< fChi2oNdf;
	if (fUsed) cout << "  in use";
 	cout << endl;
};
void FhPeak::PrintHeadLine(){
   cout << "      Mean" 
	     << "      Sigma" << "    Content" << "  MeanError" <<  "   Chi2/Ndf";
   if (0) {
       cout << " Used";
   }
   cout << endl;
}

Int_t FhPeak::Compare(const TObject *obj) const { 
//   Double_t mthis = GetMean();
//   Double_t mobj  = (( FhPeak*)obj)->GetMean();
//   cout << " mthis << " " <<  mobj" <<  mthis " " <<  mobj << endl;
   if(GetMean() == ((FhPeak *)obj)->GetMean()) return  0; 
   if(GetMean() <  ((FhPeak *)obj)->GetMean()) return -1;
   else                                       return +1;
};

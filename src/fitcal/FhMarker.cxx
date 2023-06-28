#include "FhMarker.h"

ClassImp(FhMarkerList)
ClassImp(FhMarker)
/*
Bool_t FhMarker::IsEqual(const TObject *obj) const 
{ 
   if (obj == this) 
      return kTRUE;
   else if ( obj->IsA() == FhMarker::Class() )
      return GetX() == ((FhMarker*)obj)->GetX();
   else
      return kFALSE;
};   
*/
void FhMarker::Print(Option_t *) const
{
   // Dump this marker with its attributes.

   printf("Marker  X=%f",fX);
   if (fErrX != 0 || fErrY != 0)
		printf(" ErrX=%f",fErrX);
   printf(" Y=%f", fY);
   if (fErrX != 0 || fErrY != 0)
		printf(" ErrY=%f",fErrY);
   if (GetMarkerColor() != 1) printf(" Color=%d",GetMarkerColor());
   if (GetMarkerStyle() != 1) printf(" Style=%d",GetMarkerStyle());
   if (GetMarkerSize()  != 1) printf(" Size=%f",GetMarkerSize());
   printf("\n");
}
//___________________________________________________________________________

Int_t FhMarker::Compare(const TObject *obj) const
{ 
   if (GetX() == ((FhMarker*)obj)->GetX())
      return 0;
   else if  (GetX() < (( FhMarker*)obj)->GetX()) 
      return -1;
   else
      return +1;
};

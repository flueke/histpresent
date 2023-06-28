#include "CmdListEntry.h"
#include <sstream>

//Bool_t CmdListEntry::IsEqual(const TObject *obj)const  {
//   if(fModDate > 0) {
//      return ((CmdListEntry*)obj)->fModDate == fModDate;
//   } else             return fNam == ((CmdListEntry*)obj)->fNam;
//}

Int_t CmdListEntry::Compare(const TObject *obj) const {
   if(fModDate > 0){
//      Int_t iok = -1;
      if(((CmdListEntry*)obj)->fModDate > fModDate) return 1;
      else                                          return -1;
   } else {
      Int_t hislength = ((CmdListEntry*)obj)->fNam.Length();
      Int_t mylength =   fNam.Length();
      if(hislength == 0 && mylength == 0) return 0;
      if(hislength == 0) return -1;
      if(mylength == 0) return 1;
      if(hislength < mylength)mylength = hislength;
      for(Int_t i=0; i<mylength; i++){
         if(fNam[i] < ((CmdListEntry*)obj)->fNam[i]){
            if(fReverse) return +1;
            else         return -1;
         }
         if(fNam[i] > ((CmdListEntry*)obj)->fNam[i]){
            if(fReverse) return -1;
            else         return +1;
         }

      }
      return 0;
   } 
} 


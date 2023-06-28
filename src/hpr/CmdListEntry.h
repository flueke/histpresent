#ifndef CMDLISTENTRY
#define CMDLISTENTRY
#include "TObject.h"
#include "TString.h"

namespace std {} using namespace std;

class CmdListEntry : public TObject {
public:
   TString fCmd;
   TString fNam;
   TString fTit;
   TString fSel;
   Bool_t  fReverse;
   Long_t fModDate;
public:
   CmdListEntry (TString cmd, TString nam, TString tit,TString sel):
                 fCmd(cmd), fNam(nam), fTit(tit), fSel(sel){
      fReverse=kFALSE;
      fModDate=0;
   };
   CmdListEntry (TString cmd, TString nam, TString tit,TString sel, 
                 Bool_t reverse):
                 fCmd(cmd), fNam(nam), fTit(tit), fSel(sel), 
                  fReverse(reverse){
      fModDate=0;
   };

   CmdListEntry (TString cmd, TString nam, TString tit,TString sel, 
                 Long_t fmdate):
                 fCmd(cmd), fNam(nam), fTit(tit), fSel(sel), fModDate(fmdate){
      fReverse=kFALSE;
   };

   ~CmdListEntry(){};

//   Bool_t IsEqual(const TObject *obj) const;

   Bool_t IsSortable() const {return kTRUE;}
   Long_t GetDate(){return fModDate;};
   Int_t Compare( const TObject *obj) const;
};
#endif

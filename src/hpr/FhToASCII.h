#ifndef HPR_FHTOASCII
#define HPR_FHTOASCII
//class FitHist;

namespace std {} using namespace std;

class  FhToASCII : public TGTransientFrame {

private:
   TGCompositeFrame   *fOpenWorkfileFrame;
   TGButton           *fOpenWorkfileButton;
   TGCompositeFrame   *fLFrame1;
   TGLabel            *fLname;
   TGTextEntry        *fName;
   TGTextBuffer       *fTbName;
   TGLayoutHints      *fLO1,*fLO2,*fLO3, *fLO4;
   TGCompositeFrame   *fCommandFrame1;
   TGButton           *fCancelButton;
   TGCompositeFrame   *fRBFrame;
   TGRadioButton      *fRCOnly, *fRCC;
   Bool_t             fContentOnly;
   TGWindow           *fMymain;
//   FitHist            *fFh;
public:
   FhToASCII(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~FhToASCII();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
};
#endif

#include "TROOT.h"
#include "TArrayC.h"
#include "TASImage.h"
#include "HprElement.h"
#include <iostream>
namespace std {} using namespace std;

class HprImage : public TNamed, public HprElement
{
// using TNamed::Print;
private:
   TArrayC * fBuffer;
   TImage * fImage; //! dont stream
   TPad * fPad;    //! dont stream
   Bool_t fIsGood; //! dont stream
   Bool_t fIsPainted; //! dont stream

public:

   HprImage(){fImage = 0; fPad = 0; fBuffer = 0; fIsGood = kFALSE;};
   HprImage(const Char_t * fname, TPad * pad);
   virtual ~HprImage();
   void RecursiveRemove(TObject * obj) ;
   TImage * GetImage() {return fImage;};
   Bool_t IsGood(){return fIsGood;};
   Int_t FileExists();
   void Paint(Option_t * opt = 0);
   void Print(Option_t *option="") const;
//   void Draw(Option_t * opt = NULL) const;
   Bool_t IsSortable() const {return kTRUE;}
   Int_t Compare( const TObject *obj) const
   {
      const HprElement* hpre = dynamic_cast<const HprElement*>(obj);
      if (GetPlane()== hpre->GetPlane())
         return 0;
      if (GetPlane() > hpre->GetPlane())
         return 1;
      else
         return -1;
   }
//
   Int_t ToFile(const Char_t * fname);
   ClassDef(HprImage,1)   // A TNamed object containing a sequence of bytes
};

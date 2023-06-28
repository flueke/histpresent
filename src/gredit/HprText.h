#ifndef HPRTEXT
#define HPRTEXT
#include "TAttText.h"
#include "TObject.h"
#include "TString.h"

class HprText : public TObject, public TAttText
{
private:
   TString   fText;
   Double_t  fOffset;
public:
   HprText(const char * text = NULL);
   virtual ~HprText(){};
   void SetText(const char * text) { fText = text; };
   void SetOffset(Double_t offset) { fOffset = offset; };
   Double_t GetOffset() { return fOffset; };
   const char *GetText() { return fText.Data(); };

ClassDef(HprText,1)
};
#endif

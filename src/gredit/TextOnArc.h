#include "GrCanvas.h"
#include "TArc.h"
#include "TMath.h"
#include "TPad.h"
#include "HprElement.h"

class TextOnArc : public TArc, public TAttText, public HprElement
{
private:
	TString  fText;
	Short_t  fAlign;
	Double_t fAngle;
	Double_t fSeperator;
	
public:
	TextOnArc(){SetDaughter(this);};
	TextOnArc(Double_t x, Double_t y, Double_t r, const char * text,
				 Double_t stangle = 90, Short_t align = 2);
	virtual ~TextOnArc();
	Short_t GetAlign() { return fAlign; };
	Double_t GetAngle() { return fAngle; };
	const char * GetText(){ return fText.Data(); };
	void Paint(Option_t * option = " ");
	Int_t DistancetoPrimitive(Int_t px, Int_t py);
	void SetAlign(Short_t al)   { fAlign = al; };  // *MENU*
	void SetAngle(Double_t an)  { fAngle = an; };  // *MENU*
	void SetSeperator(Double_t an)  { fSeperator = an; };  // *MENU*
	Double_t GetSeperator()  { return fSeperator; };
	void SetText(const char * t){ fText = t; };    // *MENU*
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
	ClassDef (TextOnArc, 1)
};

#ifndef THPRSPLINEX
#define THPRSPLINEX
#include "HprElement.h"
#include "TSplineX.h"
#include "InsertTextDialog.h"

class THprSplineX : public TSplineX, public HprElement
{
private:
  InsertTextDialog *fTextDialog;   //! dont stream
public:
	THprSplineX(){ fTextDialog = NULL; SetDaughter(this);};
   THprSplineX(Int_t npoints, Double_t *x = NULL, Double_t *y = NULL,
           Float_t *sf = NULL, Float_t prec = 0.2, Bool_t closed = kFALSE);
   ~THprSplineX(){};
   void AddTextDialog();    // *MENU*
   void InsertTextExecute();
	void InsertTextOnArc() {};
	void CloseDown(Int_t flag);
   void CRButtonPressed(int,int,TObject* /*obj*/) {};
   void Paint(const Option_t *opt);
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
// #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
//   void SavePrimitive(ostream &, Option_t *);
// #else
//   void SavePrimitive(ofstream &, Option_t *);
// #endif
ClassDef(THprSplineX,1)
};
#endif

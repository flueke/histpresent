#ifndef HTPAD
#define HTPAD
#include "TPad.h"
#include "HprElement.h"

class HprElement;

class HTPad : public TPad, public HprElement
{
private:
   TList *fHiddenPrimitives;
   Color_t fLabelColor();
   Int_t  fCurrentPlane;
public:
   HTPad();
   HTPad(const char *name, const char *title,
           Double_t x1, Double_t y1, Double_t x2, Double_t y2,
           Color_t color=-1, Short_t bordersize=-1, Short_t bordermode=-2);
   ~HTPad();
   void Paint(Option_t *option="");
   void PaintModified();
   virtual void PrintAddress()  {std::cout << ClassName() << "* aa = "
                         << "(" << ClassName() << "*)" << this
                         << std::endl; }; // *MENU*
   void     RemoveImage(); // *MENU*
   void     HideObject(TObject*);
   void     ViewObject(TObject*);
   void     ViewAllObjects(); // *MENU*
   Int_t    GetCurrentPlane() {return fCurrentPlane;};
   void     SetCurrentPlane(Int_t p) {fCurrentPlane = p;};
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
ClassDef(HTPad,1)
};

#endif

#include "HprElement.h"
#include "HTPad.h"
#include "GrCanvas.h"

ClassImp(HprElement)

HprElement::HprElement(TObject * daughter) :
            fDaughter(daughter)
{
   fPlane = 0;
   fVisibilityFlag = 1;
   fMustAlign = 0;
   fHTPad = NULL;
   fGrCanvas = NULL;
   if (gPad->TestBit(GrCanvas::kIsAEditorPage) || gPad->InheritsFrom("HTPad")) {
		if (gPad && gPad->GetMother() == gPad) {
			fGrCanvas = (GrCanvas*)gPad;
			fPlane = fGrCanvas->GetCurrentPlane();
		} else {
			fHTPad  = (HTPad*)gPad;
			fPlane   = fHTPad->GetCurrentPlane();
		}
   }
//   std::cout << "HprElement this: " << this  << " fGrCanvas " << fGrCanvas<< " fHTPad " << fHTPad << " fPlane " << fPlane << std::endl;
};
//____________________________________________________________________________
HprElement::~HprElement(){};
//____________________________________________________________________________
void HprElement::SetVisibility(Int_t vis)
{
   fVisibilityFlag = vis;

   if ( fGrCanvas) {
      if ( vis )
      fGrCanvas->ViewObject(fDaughter);
      else
      fGrCanvas->HideObject(fDaughter);
   }
   if ( fHTPad ) {
      if ( vis )
         fHTPad->ViewObject(fDaughter);
      else
         fHTPad->HideObject(fDaughter);
   }
   gPad->Modified();
   gPad->Update();
}
//____________________________________________________________________________
void HprElement::MoveObject(TObject *obj, TList *from, TList *to)
{
   if ( !obj || !from || from->GetEntries() <=0 ) return;
   TObject * o = NULL;
   Option_t * opt;
   TObjOptLink *objlnk = NULL;
   TObjOptLink *lnk = (TObjOptLink*)from->FirstLink();
   while (lnk) {
      o = lnk->GetObject();
      if (o == obj) {
         opt = lnk->GetOption();
         objlnk = lnk;
         break;
      } else {
         objlnk = NULL;
      }
      lnk = (TObjOptLink*)lnk->Next();
   }
   if (objlnk) {
      from->Remove(objlnk);
      to->Add(o, opt);
   }
}
//____________________________________________________________________________

void HprElement::MoveAllObjects(TList *from, TList *to, Int_t plane, Int_t vis)
{
   TObject * o = NULL;
   Option_t * opt;
   Int_t nel = GetNofElementsInPlane(from, plane);
   std::cout << "MoveAll nel: " << nel << std::endl;
   do {
		TObjOptLink *lnk = (TObjOptLink*)from->FirstLink();
		while (lnk) {
			o = lnk->GetObject();
		   opt = lnk->GetOption();
         if (o->InheritsFrom("HprElement")) {
            HprElement *hpre = dynamic_cast<HprElement*>(o);
//           std::cout << "cast from obj to hpre: " << o << " " << hpre << std::endl;

            if (plane == 0 || hpre->GetPlane() == plane) {
 		   	   from->Remove(lnk);
			      to->Add(o, opt);
               nel--;
               std::cout << "----Move  " << o->GetName()<< std::endl;
               hpre->SetVisibilityFlag(vis);
               break;
            }
         }
         lnk = (TObjOptLink*)lnk->Next();
		}
   } while (nel > 0);

}
//____________________________________________________________________________
Int_t HprElement::GetNofElementsInPlane(TList *list, Int_t plane)
{
   if (plane == 0)
      return list->GetEntries();
   TIter next(list);
   TObject *obj;
   Int_t nel = 0;
   while ( (obj = next()) ) {
//      cout << "GetNofElementsInPlane: "<< obj->ClassName() << endl;
      if (obj->InheritsFrom("HprElement")) {
         HprElement* hpre = dynamic_cast<HprElement*>(obj);
         if (plane == hpre->GetPlane()) {
            nel++;
         }
      }
   }
   return nel;
}
//____________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void HprElement::SavePrimitive(ostream &, Option_t *){}; // dont write to .C file
#else
void HprElement::SavePrimitive(ofstream &, Option_t *){};
#endif

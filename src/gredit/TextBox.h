#ifndef TEXTBOX
#define TEXTBOX
#include "TPave.h"
#include "TList.h"
#include "TTimer.h"
#include "HprEditBits.h"
#include <iostream>

namespace std {} using namespace std;
//______________________________________________________________________

class TextBox;

class TbTimer : public TTimer
{
private:
   TextBox *fTextBox;
public:
   TbTimer(Long_t ms, Bool_t synch,  TextBox *textbox);
   virtual ~TbTimer(){};
   Bool_t Notify();

ClassDef(TbTimer, 0)
};
//______________________________________________________________________

class TextBoxMember : public TObject {

private:
   TObject * fObject;
   Short_t   fAlign;
   Bool_t    fFirst;
   Int_t fAlignType;    // 0 no align, 1 on edges only, 2 center
public:
   TextBoxMember(){};
   TextBoxMember(TObject *obj, Short_t align, Int_t align_type)
   : fObject(obj), fAlign(align),  fAlignType(align_type)
{
}
   ~TextBoxMember(){};
   TObject *GetObject(){return fObject;};
   Short_t GetAlign(){return fAlign;};
   Int_t   GetAlignType(){return fAlignType;};
   void    SetAlign(Short_t al){fAlign = al;};
   void SetFirst(Bool_t first) {fFirst = first;};
   Bool_t GetFirst() {return fFirst;};
ClassDef(TextBoxMember, 1)

};
//______________________________________________________________________

class TextBox : public TPave {

private:
   TbTimer  *fTimer;  //!
   Int_t    fPrimitives; //!
   Double_t fX1Save;
   Double_t fY1Save;
   Double_t fX2Save;
   Double_t fY2Save;
   TList    fMembers;
   Bool_t   fOwnMembers;
   Double_t fTextMargin;
   Double_t fSmall;
   Int_t    fAlignType;    // 0 no align, 1 on edges only, 2 center
public:
   TextBox(Double_t x1, Double_t y1, Double_t x2, Double_t y2,const char *name = NULL);
   TextBox();
   ~TextBox();
   void RecursiveRemove(TObject * obj);
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void AlignEntries(Double_t dX1, Double_t dY1, Double_t dX2, Double_t dY2);
   void Paint(Option_t * option = " ");

//   void PaintPaveArc(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
//                      Int_t, Option_t *option);
   void AdoptAll() ;                       // *MENU*
   void AdoptMember(TObject* obj) ;                    //
   void AddMember(TObject* obj, Double_t x, Double_t y);
   TextBoxMember *FindMember(TObject *obj);
   void      PrintMembers();                 // *MENU*
   Int_t   IsInside(Double_t x, Double_t y) const {
      return (x > GetX1()-fSmall && x < GetX2() + fSmall
           && y > GetY1()-fSmall && y < GetY2() + fSmall);
   }
   Double_t PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t align);

   Double_t GetXbyAlign(Short_t align, Double_t ts = 0);
   Double_t GetYbyAlign(Short_t align, Double_t ts = 0);
   Short_t  GetAlignbyXY(Double_t x, Double_t y, Bool_t its_line = kFALSE);
   Int_t GetAlignType() { return fAlignType;};
   void  SetAlignType(Int_t at) {fAlignType = at;};  // *MENU*
   TList *GetListOfMembers() {return &fMembers;};
   void TransformPave(TPave *p, Short_t align);
   void SetPaveNDC(TPave *p);
   void Wakeup();
   void ObjCreated(Int_t px, Int_t py, TObject*);
   void ObjMoved(Int_t px, Int_t py, TObject*);
ClassDef(TextBox, 1)
};
#endif


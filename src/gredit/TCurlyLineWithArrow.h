#ifndef TCURLYLINEWITHARROW
#define TCURLYLINEWITHARROW
#include "TArrow.h"
#include "TCurlyLine.h"

class TCurlyLineWithArrow;
//_________________________________________________________________

class TCurlyLineArrow : public TArrow {
private:
   TCurlyLineWithArrow *fParent;
   Int_t fWhere;
public:
   TCurlyLineArrow(){};
   TCurlyLineArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
              Float_t arrowsize=0.05,Option_t *option="|>");
   ~TCurlyLineArrow(){};
//   void Paint(const Option_t *opt) {};
   void  SetWhere(Int_t where) {fWhere = where;}
   Int_t GetWhere() {return fWhere;}
   void SetParent(TCurlyLineWithArrow *p) {fParent = p;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(std::ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(std::ofstream &, Option_t *){};     // dont write to .C file
 #endif
ClassDef(TCurlyLineArrow,1)
};

class TCurlyLineWithArrow : public TCurlyLine {

private:
   Double_t fX1;
   Double_t fY1;
   Double_t fX2;
   Double_t fY2;
   Int_t    fWhere;
   Float_t  fDefaultArrowSize;
   TCurlyLineArrow * fArrowStart;
   TCurlyLineArrow * fArrowEnd;
public:
   TCurlyLineWithArrow(){};
   TCurlyLineWithArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
              Double_t wl = .02, Double_t amp = .01,
              Int_t where = 2, Float_t arrowsize=0);
   ~TCurlyLineWithArrow(){};
   void SetX1(Double_t v) {fX1 = v;}
   void SetX2(Double_t v) {fX2 = v;}
   void SetY1(Double_t v) {fY1 = v;}
   void SetY2(Double_t v) {fY2 = v;}
   Double_t GetX1() {return fX1;}
   Double_t GetX2() {return fX2;}
   Double_t GetY1() {return fY1;}
   Double_t GetY2() {return fY2;}
//   void AddArrow(Int_t where, Float_t arrowsize=0) {}; // *MENU*
   void SetArrowSize(Int_t where = 2, Float_t arrowsize = 0);  // *MENU*
   Double_t GetDistToArrow(Int_t where);
   TCurlyLineArrow * GetArrowStart() { return fArrowStart;}
   TCurlyLineArrow * GetArrowEnd() { return fArrowEnd;}
   Float_t GetDefaultArrowSize () { return fDefaultArrowSize; };
   void  SetWhere(Int_t where) {fWhere = where;}
   Int_t GetWhere() {return fWhere;}
   void Paint(const Option_t *option="|>");
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(std::ostream &, Option_t *);
 #else
   void SavePrimitive(std::ofstream &, Option_t *);
 #endif
ClassDef(TCurlyLineWithArrow,1)
};
#endif

#ifndef GROUPOFGOBJECTS
#define GROUPOFGOBJECTS

#include "TNamed.h"
#include "TList.h"
#include "TPad.h"
#include "TCutG.h"
#include "HprElement.h"

//___________________________________________________________________

class GroupOfGObjects : public TCutG {
//friend class HTCanvas;
friend class GEdit;
protected:
   Double_t fXorigin;
   Double_t fYorigin;
   Double_t fXLowEdge;      // Bounding Box X Low Edge
   Double_t fYLowEdge;      // Bounding Box Y Low Edge
   Double_t fXUpEdge;       // Bounding Box X Upper Edge
   Double_t fYUpEdge;       // Bounding Box Y Upper Edge
   Bool_t   fIsVisible;
   Bool_t   fForceVerticalShiftOnly;
   Bool_t   fForceHorizontalShiftOnly;
   TList    fMembers;       // Members
   Double_t fAngle;
	Int_t    fAlign;
	Int_t    fMirror;
public:
   GroupOfGObjects(){};

   GroupOfGObjects(const char * name,
                   Double_t x, Double_t y,  TCutG* cut);
   virtual ~GroupOfGObjects();
   void    RecursiveRemove(TObject * obj);
   void    SetOrigin(Double_t x, Double_t y){fXorigin = x; fYorigin = y;}
   Double_t GetXorigin(){return fXorigin;}
   Double_t GetYorigin(){return fYorigin;}
   TList * GetMemberList() {return & fMembers;};
   void    SetEnclosingCut(TCutG *cut);
   void    AddMember(TObject * obj, Option_t *option);
   void    RemoveMember(TObject * obj) {fMembers.Remove(obj);};
   Bool_t  BindMember(TObject * obj);
   Bool_t  FreeMember(TObject * obj);
   Int_t   GetNMembers() {return fMembers.GetSize();};
   Int_t   AddMembersToList(TPad * pad, Double_t xoff = 0, Double_t yoff = 0,
                            Double_t scaleNDC = 1,Double_t scaleG = 1,
                            Double_t angle = 0, Int_t align = 11, Int_t mirror = 0, Int_t draw_cut=0, Int_t plane = -1);
   void Transform(Double_t x, Double_t y, Double_t xoff,Double_t yoff,
                    Double_t scale, Double_t* xt, Double_t* yt);
   void Print(Option_t *chopt="") const;                                                // *MENU*
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void Paint(Option_t* opt) {if (fIsVisible) TGraph::Paint(opt);};
   void SetVisible(Bool_t vis) {fIsVisible = vis;};          // *MENU*
   Bool_t GetVisible() {return fIsVisible;};
   void BindObjects() {BindReleaseObjects(kTRUE);};            // *MENU*
   void ReleaseObjects() {BindReleaseObjects(kFALSE);};        // *MENU*
   void DeleteObjects();                                       // *MENU*
   void ListObjects();                                         // *MENU*
   void ForceVerticalShiftOnly(Bool_t fs) {fForceVerticalShiftOnly = fs;};   // *MENU*
   void ForceHorizontalShiftOnly(Bool_t fs) {fForceHorizontalShiftOnly = fs;};   // *MENU*
   void ShiftObjects(Double_t xshift, Double_t yshift, Bool_t shiftcut = kTRUE);        // *MENU*
   void BindReleaseObjects(Bool_t bind) ;
   Bool_t SloppyInside(Double_t x, Double_t y);
   void SetPlane(Int_t plane);                    // *MENU*

ClassDef(GroupOfGObjects,2)
};
#endif

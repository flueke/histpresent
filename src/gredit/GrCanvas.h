
#ifndef ROOT_GrCanvas
#define ROOT_GrCanvas

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// GrCanvas                                                              //
//                                                                      //
// Graphics canvas .                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TList.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include <iostream>

namespace std {} using namespace std;


class HprElement;
class GEdit;

class GrCanvas : public TCanvas {


private:
   TRootCanvas    * fRootCanvas;      //! dont stream
   GEdit          * fGEdit;           //! dont stream
   Int_t          fConnections;     //! dont stream
   Bool_t         fButtonsEnabled;    //! dont stream
   Bool_t         fEditorIsShown;     //! dont stream
   TList          *fHiddenPrimitives; //

   Double_t       fEditGridX;         //
   Double_t       fEditGridY;         //
   Int_t          fUseEditGrid;       //
   UInt_t         fOrigWw;
   UInt_t         fOrigWh;
   Int_t          fCurrentPlane;      //

//   void Build();
//
public:
   enum {
      kIsObjectList  = BIT(22),
      kIsAEditorPage = BIT(23)
   };
   GrCanvas();
   GrCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh);
   virtual ~GrCanvas();
   virtual void HandleInput(EEventType button, Int_t x, Int_t y);
   virtual void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
   void   RunAutoExec();
//   void   SetLog(Int_t state);
   void   MyClose();
	TRootCanvas * GetRootCanvas() { return fRootCanvas; } 
   Double_t GetEditGridX() {return fEditGridX;};
   Double_t GetEditGridY() {return fEditGridY;};
   void     SetEditGrid(Double_t x, Double_t y) {fEditGridX = x, fEditGridY = y;};
   void    SetUseEditGrid(Int_t use = 1) {fUseEditGrid = use;};
   Int_t   GetUseEditGrid() {return fUseEditGrid;};
   void    Add2ConnectedClasses(TObject *obj);
	void    RemoveFromConnectedClasses(TObject *obj);
	void    ObjectCreated(Int_t, Int_t, TObject*);
   void    ObjectMoved(Int_t, Int_t, TObject*);
   static  TObject * WaitForCreate(const char * what, TPad **pad);
   Double_t PutOnGridX(Double_t);
   Double_t PutOnGridY(Double_t);
   Double_t PutOnGridX_NDC(Double_t);
   Double_t PutOnGridY_NDC(Double_t);
   void     HideObject(TObject*);
   void     ViewObject(TObject*);
   void     ViewAllObjects();       // *MENU*
   Int_t    GetCurrentPlane() {return fCurrentPlane;};
   void     SetCurrentPlane(Int_t p) {fCurrentPlane = p;};
   TList    *GetHiddenPrimitives() {return fHiddenPrimitives;};
	void 		ListPrimitives();             // *MENU*
//______________________________________________________________________________

	void RemovePicture()
	{
		TList * lop = GetListOfPrimitives();
		TIter next(lop );
		TObject * obj;
		while ( (obj = next()) ) {
			if (obj->InheritsFrom("HprImage")) {
				lop->Remove(obj);
				break;
			}
		}
		Modified();
		Update();
	}                // *MENU*

   void     SetEnableButtons(Bool_t enable = kTRUE) {fButtonsEnabled = enable;};
   Bool_t   GetEnableButtons() {return fButtonsEnabled ;};
	UInt_t   GetOrigWw() { return fOrigWw; };
	UInt_t   GetOrigWh() { return fOrigWh; };
   void     SetShowEditor(Bool_t y = kTRUE) { fEditorIsShown = y; };
   Bool_t   GetShowEditor() { return fEditorIsShown; };
   GEdit    *GetGEdit() { return fGEdit; };
   void     SetGEdit(GEdit *ed) { fGEdit = ed; };
//   TList   *GetConnectedClasses() { return &fConnectedClasses;};
   ClassDef(GrCanvas,1)            //Graphics canvas
};

#endif


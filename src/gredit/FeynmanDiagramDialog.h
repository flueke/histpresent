#ifndef FEYNMANDIAGRAMDIALOG
#define FEYNMANDIAGRAMDIALOG
#include "TObject.h"
#include "TGMrbValuesAndText.h"
#include "GrCanvas.h"
//_____________________________________________________________________________________


class FeynmanDiagramDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TPad       * fPad;
   Double_t   fWaveLength; // wavelength of sinusoid in percent of pad height
   Double_t   fAmplitude;  // amplitude of sinusoid in percent of pad height
   Color_t    fColor ;  	// Feynman line color
   Width_t    fWidth ;  	// Feynman line width
   Int_t      fPhi1; 		// Phi1 angle for curly arcs
   Int_t      fPhi2; 		// Phi2 angle for curly arcs
   Float_t    fArrowSize;  //
   Int_t      fArrowStyle;  //
public:
   FeynmanDiagramDialog();
   virtual ~FeynmanDiagramDialog();
   void    FeynmanArrow();
   void    FeynmanCurlyWavyLine(Int_t curly);
   void    FeynmanWavyLine();
   void    FeynmanCurlyLine();
   void    FeynmanCurlyWavyArc(Int_t curly);
   void    FeynmanWavyArc();
   void    FeynmanCurlyArc();
   void    FeynmanSolidLine();
   void    FeynmanDashedLine();
   void    FeynmanLine(Style_t);
	void    ObjCreated(Int_t /*px*/, Int_t /*py*/, TObject *obj) {fPad = (TPad*)obj;};
	void    ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject */*obj*/) { };
   void    SaveDefaults();
   void    RestoreDefaults();
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
ClassDef(FeynmanDiagramDialog,0)
};
#endif

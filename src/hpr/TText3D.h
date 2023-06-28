#ifndef TTEXT3D
#define TTEXT3D
#include "TPolyLine3D.h"
#include "TAttText.h"

class TNamed;
class TText3D;
class TPolyLine3D;

class TPol3Dtext : public TPolyLine3D {
	
	private:
		TText3D *fText;
		TString fTitle;
	public:
		TPol3Dtext() {};
		virtual ~TPol3Dtext();
		TPol3Dtext(Int_t n, TText3D *text);
		const char * GetTitle() const {return fTitle.Data();};
		const char * GetText() const ;     //*MENU*            
		void SetText(const char * text); //*MENU*
		
		ClassDef (TPol3Dtext, 1)
};

class TText3D : public TPolyLine3D, public TAttText {
	
private:
	TList * fListOfLetters;
	TString fText;
	Double_t fX;
	Double_t fY;
	Double_t fZ;
	Double_t fDirX;
	Double_t fDirY;
	Double_t fDirZ;
	TString fName;
	TString fTitle;
	
//	Width_t fLineWidthText;
	
public:
	TText3D() {};
	virtual ~TText3D();
	TText3D(Double_t x, Double_t y, Double_t z, 
	        Double_t dx, Double_t dy, Double_t dz, 
			  const char * text);
	void Compute();
	void Draw(const char * option="");
	void Paint(const char * option="");
//	Width_t GetLineWidthText() const {return fLineWidthText;};                 
//	void SetLineWidthText(const Width_t wid) {fLineWidthText = wid; Paint();}; //*MENU*
	Width_t GetLineWidth() const {return fLineWidth;};                 
	void SetLineWidth(const Width_t wid) {fLineWidth = wid; Paint();}; //*MENU*
	const char * GetText() const {return fText.Data();};                 
	void SetText(const char * text) {fText = text; Compute(); Paint();}; //*MENU*
	Color_t  GetTextColor() const {return fTextColor;}
	void SetTextColor(Color_t col) {fTextColor = col; Paint();}; //*MENU*
//	Width_t GetLineWidth() {return fLineWidth;};                 
//	void SetLineWidth(Width_t wid) {fLineWidth = wid; Paint();}; //*MENU*
	Size_t GetTextSize() const {return fTextSize;};                 
	void SetTextSize(Size_t siz) {fTextSize =siz; Compute(); Paint();}; //*MENU*
	void SetName(const char * name) {fName = name;};      
	const char * GetName() const {return fName.Data();};      
	void SetTitle(const char * name) {fTitle = name;};      
	const char * GetTitle() const {return fTitle.Data();};      
	Int_t DistancetoPrimitive(Int_t px, Int_t py);
	void ExecuteEvent(Int_t event, Int_t px, Int_t py);
	ClassDef (TText3D, 1)
};
#endif

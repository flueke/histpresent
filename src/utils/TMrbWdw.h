#ifndef __TMrbWdw_h__
#define __TMrbWdw_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbWdw.h
// Purpose:        Define named windows
// Classes:        TMrbWindowI              -- a one-dim window, integer
//                 TMrbWindowF              -- ... double
//                 TMrbWindow2D             -- 2-dim windows and cuts, double only
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbWdw.h,v 1.7 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TNamed.h"
#include "TString.h"
#include "TLine.h"
#include "TAttText.h"
#include "TPad.h"
#include "TCutG.h"
#include "TH1.h"
#include "TSystem.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVarWdwCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow
// Purpose:        Base class for user-defined windows
// Description:    Defines a variable or window and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbWindow: public TLine, public TAttText {

	public:
		TMrbWindow();			 						// default ctor

		TMrbWindow(Double_t Xlower, Double_t Xupper);	// ctor

		~TMrbWindow();	         						// dtor

		inline void SetParent(TObject * Parent) { fParent = Parent; };

		inline UInt_t GetType() const { return(GetUniqueID() & kVarOrWindow); };
		inline UInt_t GetStatus() const { return(GetUniqueID() & kStatus); };

		void Initialize();								// initialize values
		virtual void Print(Option_t * Option = "") const;
		void SetRange(Bool_t Flag = kTRUE) {
			UInt_t wdwType = this->GetUniqueID();
			if (Flag) wdwType |= kIsRangeChecked; else wdwType &= ~kIsRangeChecked;
			this->SetUniqueID(wdwType);
		};

		inline Bool_t HasInitValues() const { return((GetUniqueID() & kHasInitValues) != 0); };
		inline Bool_t IsRangeChecked() const { return((GetUniqueID() & kIsRangeChecked) != 0); };

		Bool_t AddToHist(TObject * Histogram);	// add window to histogram

// we have to imitate a TNamed object here ...
		virtual void SetName(const Char_t * WdwName);
		virtual const Text_t * GetName() const { return(fName.Data()); };
		virtual Bool_t IsSortable() const { return kTRUE; }
		virtual Int_t Compare(const TObject * Object) const {
			if (this == Object) return(0);
			return(fName.CompareTo(Object->GetName()));
		};
		virtual ULong_t Hash() const { return fName.Hash(); }

// some methods to visualize a window
		virtual void Draw(Option_t * Option = "");
		virtual void Paint(Option_t * Option = "");

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void SetInitialType(UInt_t WdwType);

	protected:
		TString fName;				// name linke in TNamed
		TObject * fParent;			//! dont stream  addr of parent histogram

	ClassDef(TMrbWindow, 1) 	// [Utils] Base class for user-defined windows
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI
// Purpose:        Base class for a simple (1-dim) window (integer)
// Description:    Defines a 1-dim window and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbWindowI: public TMrbWindow {

	public:
		TMrbWindowI() {};														// default ctor
		TMrbWindowI(const Char_t * Name, Int_t Xlower = 0, Int_t Xupper = 0);	// ctor

		~TMrbWindowI() {};														// dtor

		inline Int_t Clip(Int_t Value) const {						// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		void SetLowerLimit(Int_t Xlow);   // *MENU* *ARGS={Xlow=>fX1}

		void SetUpperLimit(Int_t Xup);    // *MENU* *ARGS={Xup=>fX2}

		void Set(Int_t Xlow, Int_t Xup);  // *MENU* *ARGS={Xlow=>fX1,Xup=>fX2}

		inline Bool_t IsInside(Int_t X) const { return(X >= (Int_t) (fX1) && X <= (Int_t) (fX2)); }; // test limits

		Int_t GetLowerLimit() const { return((Int_t) (fX1)); };				// return current lower limit
		Int_t GetUpperLimit() const { return((Int_t) (fX2)); };				// return current upper limit

		void Initialize() {								// reset to initial limits
			fX1 = (Double_t) this->Clip(fLowerInit);
			fX2 = (Double_t) this->Clip(fUpperInit);
			if (gPad) {
				this->Paint();
				gPad->Modified();
			}
		};

		void SetRange(Int_t Xlower, Int_t Xupper) {		// define range
			this->SetUniqueID(this->GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		virtual void PaintLine(Double_t, Double_t, Double_t, Double_t);

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fXlower;							// current limits
		Int_t fXupper;
		Int_t fLowerInit; 						// initial limits
		Int_t fUpperInit;
		Int_t fLowerRange;						// range
		Int_t fUpperRange;

	ClassDef(TMrbWindowI, 1)		// [Utils] A 1-dim window with integer limits
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF
// Purpose:        Base class for a simple (1-dim) window (double)
// Description:    Defines a 1-dim window and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbWindowF: public TMrbWindow {

	public:
		TMrbWindowF() {};																// default ctor

		TMrbWindowF(const Char_t * Name, Double_t Xlower = 0., Double_t Xupper = 0.);	// ctor

		~TMrbWindowF() {};									    						// dtor

		inline Double_t Clip(Double_t Value) const {						// clip value to range boundaries
			if (this->IsRangeChecked()) {
				if (Value < fLowerRange) return(fLowerRange);
				else if (Value > fUpperRange) return(fUpperRange);
				else return(Value);
			} else return(Value);
		};

		void SetLowerLimit(Double_t Xlow);		// *MENU* *ARGS={Xlow=>fX1}
		void SetUpperLimit(Double_t Xup);		// *MENU* *ARGS={Xup=>fX2}
		void Set(Double_t Xlow, Double_t Xup);	// *MENU* *ARGS={Xlow=>fX1,Xup=>fX2}

		Double_t GetLowerLimit() const { return(fX1); };		// return current lower limit

		Double_t GetUpperLimit() const { return(fX2); };		// return current upper limit

		inline Bool_t IsInside(Double_t X) const { return(X >= fX1 && X <= fX2); }; // test limits

		void Initialize() {								// reset to initial limits
			fX1 = this->Clip(fLowerInit);
			fX2 = this->Clip(fUpperInit);
			if (gPad) {
				this->Paint();
				gPad->Modified();
			}
		};

		void SetRange(Double_t Xlower, Double_t Xupper) {		// define range
			this->SetUniqueID(this->GetUniqueID() | kIsRangeChecked);
			fLowerRange = Xlower;
			fUpperRange = Xupper;
		};

		virtual void PaintLine(Double_t, Double_t, Double_t, Double_t);

		virtual void Print(Option_t * Option = "") const;

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

 	protected:
		Double_t fXlower;							// current limits
		Double_t fXupper;
		Double_t fLowerInit; 						// initial limits
		Double_t fUpperInit;
		Double_t fLowerRange;						// range
		Double_t fUpperRange;

	ClassDef(TMrbWindowF, 1)		// [Utils] A 1-dim window with float limits
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D
// Purpose:        Base class for a 2-dim windows and cuts
// Description:    Defines a 2-dim window/cut and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbWindow2D : public TCutG, public TAttText {

	public:
		TMrbWindow2D(); 				// default ctor

		TMrbWindow2D(const Char_t * WdwName, Int_t N, Double_t * X = NULL, Double_t * Y = NULL);

		TMrbWindow2D(TCutG Cut);

		~TMrbWindow2D();					// dtor         

		inline void SetParent(TObject * Parent) { fParent = Parent; };

		virtual void Draw(Option_t * Option = "");
		virtual void Print(Option_t * Option = "") const;
		Double_t GetXtext() const { return(fXtext); }
		Double_t GetYtext() const { return(fYtext); }
		void SetXtext(Double_t Xt){ fXtext = Xt; }
		void SetYtext(Double_t Yt){ fYtext = Yt; }

		Bool_t AddToHist(TObject * Histogram);			// add window to histogram

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void SetInitialType(UInt_t WdwType);

	protected:
		Double_t fXtext;
		Double_t fYtext;
		TObject *fParent;				//! don't stream

	ClassDef(TMrbWindow2D, 2)			// [Utils] A 2-dim window
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsWindowI, TMbsWIndowF, TMbsWindow2d
// Purpose:        Obsolete defs, for compatibility only
//////////////////////////////////////////////////////////////////////////////

class TMbsWindowI : public TMrbWindow {
	public:
		TMbsWindowI() {};
		~TMbsWindowI() {};
	protected:
		Int_t fXlower;							// current limits
		Int_t fXupper;
		Int_t fLowerInit; 						// initial limits
		Int_t fUpperInit;
		Int_t fLowerRange;						// range
		Int_t fUpperRange;

	ClassDef(TMbsWindowI, 1)
};

class TMbsWindowF : public TMrbWindow {
	public:
		TMbsWindowF() {};
		~TMbsWindowF() {};
	protected:
		Float_t fXlower;							// current limits
		Float_t fXupper;
		Float_t fLowerInit; 						// initial limits
		Float_t fUpperInit;
		Float_t fLowerRange;						// range
		Float_t fUpperRange;

	ClassDef(TMbsWindowF, 1)
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsWindow2d
// Purpose:        Base class for a 2-dim windows and cuts
//                 this is only for backward compatibility, 
//                 for new windows use: TMrbWindow2D
// Methods:        GetXtext        -- get text position X
//                 GetYtext        -- ... Y
// Description:    Defines a 2-dim window/cut and its methods.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMbsWindow2d : public TCutG, public TAttText {

	public:
		TMbsWindow2d(){}; 				// default ctor

		~TMbsWindow2d();					// dtor         


		virtual void Draw(Option_t * Option = "");
		Float_t GetXtext() { return(fXtext); }
		Float_t GetYtext() { return(fYtext); }

	protected:
		Float_t fXtext;
		Float_t fYtext;
		TObject *fParent;			//! don't stream it

	ClassDef(TMbsWindow2d, 1)
};

#endif

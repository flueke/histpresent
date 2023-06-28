//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbWindows.cxx
// Purpose:        MARaBOU utilities: Provide user-defined variables and windows
// Description:    Implements class methods to handle variables and 1-dim windows
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbWindows.cxx,v 1.9 2010-01-28 13:33:40 Rudolf.Lutter Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TROOT.h"

#include "TMrbLogger.h"
#include "TMrbWdw.h"
#include "TMrbLofUserVars.h"

#include "TMrbVarWdwCommon.h"

#include "SetColor.h"

ClassImp(TMrbWindow)
ClassImp(TMrbWindowI)
ClassImp(TMrbWindowF)
ClassImp(TMrbWindow2D)
ClassImp(TMbsWindowI)
ClassImp(TMbsWindowF)
ClassImp(TMbsWindow2d)

extern TMrbLogger * gMrbLog;					// MARaBOU's message logger
extern TMrbLofUserVars * gMrbLofUserVars;		// a list of all vars and windows defined so far

TMrbWindow::TMrbWindow() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow
// Purpose:        Default constructor
// Description:    Creates list of vars/wdws.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
	fParent = NULL;
}

TMrbWindow::TMrbWindow(Double_t Xlower, Double_t Xupper) : TLine(Xlower, 0.0, Xupper, 0.0) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow
// Purpose:        Default constructor
// Arguments:      Double_t Xlower    -- lower limit
//                 Double_t Xupper    -- upper limit
// Description:    Creates list of vars/wdws.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
	fParent = NULL;
}


TMrbWindow::~TMrbWindow() {
//__________________________________________________________________[C++ DTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           ~TMrbWindow
// Purpose:        Destructor
// Description:    Remove window from lists.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gDirectory->GetList()->Remove(this);
	gMrbLofUserVars->Remove(this);
	if (gPad) gPad->GetListOfPrimitives()->Remove(this);
	if (fParent && fParent->InheritsFrom(TH1::Class())) ((TH1*)fParent)->GetListOfFunctions()->Remove(this);
	fParent = NULL;
}

void TMrbWindow::SetName(const Char_t * WdwName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::SetName
// Purpose:        Set window name
// Arguments:      Char_t * WdwName  -- window name
// Results:
// Exceptions:
// Description:    Sets the name as done in TNamed.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fName = WdwName;
	if (gPad) gPad->Modified();
};                                   // *MENU*

void TMrbWindow::SetInitialType(UInt_t WdwType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::SetInitialType
// Purpose:        Set variable type
// Arguments:      UInt_t WdwType    -- type bits
// Results:
// Exceptions:
// Description:    Defines type bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetUniqueID() != 0) return;

	this->SetUniqueID(WdwType);
	gMrbLofUserVars->Append(this);
//	if (gDirectory != NULL) gDirectory->Append(this);
}

Bool_t TMrbWindow::AddToHist(TObject * Histogram) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::AddToHist
// Purpose:        Connect window to Histogram
// Arguments:      TObject * Histogram   -- object inherited from TH1
// Results:
// Exceptions:
// Description:    Add a window to the list of function of a histo.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Histogram->InheritsFrom("TH1")) {
		TList * lofFunct = ((TH1 *) Histogram)->GetListOfFunctions();
		if (lofFunct->FindObject(this) == NULL) lofFunct->Add(this);
		this->SetLineColor(1);
		this->SetLineStyle(1);
		this->SetLineWidth(1);
		this->SetTextAlign(11);
		this->SetTextAngle(0);
		this->SetTextColor(1);
		this->SetTextFont(62);
		this->SetTextSize(0.04);
		fParent = Histogram;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Not a histogram - " << Histogram->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "AddToHist");
		return(kFALSE);
	}
}

void TMrbWindow::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::Print
// Purpose:        Print current value
// Arguments:      Char_t * Option   -- option
// Results:
// Exceptions:
// Description:    Outputs current value to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (((TMrbWindow *) this)->GetType()) {
		case kWindowI:	((TMrbWindowI *) this)->Print(Option); break;
		case kWindowF:	((TMrbWindowF *) this)->Print(Option); break;
	}
}

void TMrbWindow::Initialize() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::Initialize
// Purpose:        Reset to initial value(s)
// Arguments:
// Results:
// Exceptions:
// Description:    Resets to initial value(s).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (this->GetType()) {
		case kWindowI:	((TMrbWindowI *) this)->Initialize(); break;
		case kWindowF:	((TMrbWindowF *) this)->Initialize(); break;
	}
}


TMrbWindowI::TMrbWindowI(const Char_t * Name, Int_t Xlower, Int_t Xupper) : TMrbWindow((Double_t) Xlower, (Double_t) Xupper) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI
// Purpose:        Define an integer window
// Arguments:      Char_t * Name           -- name of window
//                 Int_t * Xlower          -- initial lower limit
//                 Int_t * Xupper          -- initial upper limit
// Results:
// Exceptions:
// Description:    Creates a window with integer limits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t hasInit;

	SetName(Name);
	fLowerInit = Xlower;
	fUpperInit = Xupper;
	hasInit = ((Xlower != 0) || (Xupper != 0)) ? kHasInitValues : 0;
	this->SetInitialType(hasInit | kWindowI);
};

void TMrbWindowI::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI::Print
// Purpose:        Print current window limits
// Arguments:      Char_t * Option  -- option
// Results:
// Exceptions:
// Description:    Outputs window limits to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = [" << (Int_t) fX1 << "," << (Int_t) fX2 << "]" << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "int"
				<< setw(11) << (Int_t) (fX1);
		cout	<< " - "
				<< setw(5) << (Int_t) (fX2)
				<< setw(8) << fLowerInit;
		cout	<< " - "
				<< setw(5) << fUpperInit;
		if (((TMrbWindow *) this)->IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " - "
					<< setw(5) << fUpperRange;
		}
		cout	<< endl;
	}
}

void TMrbWindowI::SetLowerLimit(Int_t Xlower) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI::SetLowerLimit
// Purpose:        Set lower limit of this window
// Arguments:      Int_t Xlower   -- lower limit
// Results:
// Exceptions:
// Description:    Sets lower limit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fXlower = Clip(Xlower);
	fX1 = (Double_t) fXlower;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};

void TMrbWindowI::SetUpperLimit(Int_t Xupper) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI::SetUpperLimit
// Purpose:        Set upper limit of this window
// Arguments:      Int_t Xupper   -- upper limit
// Results:
// Exceptions:
// Description:    Sets upper limit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fXupper = Clip(Xupper);
	fX2 = (Double_t) fXupper;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};

void TMrbWindowI::Set(Int_t Xlower, Int_t Xupper) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI::Set
// Purpose:        Set both limits of this window
// Arguments:      Int_t Xlower    -- limit
//                 Int_t Xupper    -- upper limit
// Results:
// Exceptions:
// Description:    Sets both limits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fXlower = Clip(Xlower);
	fX1 = (Double_t) fXlower;
	fXupper = Clip(Xupper);
	fX2 = (Double_t) fXupper;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};

TMrbWindowF::TMrbWindowF(const Char_t * Name, Double_t Xlower, Double_t Xupper) : TMrbWindow(Xlower, Xupper) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF
// Purpose:        Define a float window
// Arguments:      Char_t * Name           -- name of window
//                 Double_t * Xlower        -- initial lower limit
//                 Double_t * Xupper        -- initial upper limit
// Results:
// Exceptions:
// Description:    Creates a window with float limits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t hasInit;

	SetName(Name);
	fLowerInit = Xlower;
	fUpperInit = Xupper;
	hasInit = ((Xlower != 0.) || (Xupper != 0.)) ? kHasInitValues : 0;
	this->SetInitialType(hasInit | kWindowF);
};

void TMrbWindowF::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF::Print
// Purpose:        Print current window limits
// Arguments:      Char_t * Option  -- option
// Results:
// Exceptions:
// Description:    Outputs window limits to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< this->ClassName() << " " << GetName() << " = [" << fX1 << "," << fX2 << "]" << endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "float"
				<< setw(11) << fX1;
		cout	<< " - "
				<< setw(5) << fX2
				<< setw(8) << fLowerInit;
		cout	<< " - "
				<< setw(5) << fUpperInit;
		if (((TMrbWindow *) this)->IsRangeChecked()) {
			cout	<< setw(8) << fLowerRange;
			cout	<< " - "
					<< setw(5) << fUpperRange;
		}
		cout	<< endl;
	}
}

void TMrbWindowF::SetLowerLimit(Double_t Xlower) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF::SetLowerLimit
// Purpose:        Set lower limit of this window
// Arguments:      Double_t Xlower   -- lower limit
// Results:
// Exceptions:
// Description:    Sets lower limit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fX1 = Clip(Xlower);
	fXlower = fX1;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};                                    // *MENU*

void TMrbWindowF::SetUpperLimit(Double_t Xupper) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF::SetUpperLimit
// Purpose:        Set upper limit of this window
// Arguments:      Double_t Xupper   -- upper limit
// Results:
// Exceptions:
// Description:    Sets upper limit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fX2 = Clip(Xupper);
	fXupper = fX2;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};									// *MENU*

void TMrbWindowF::Set(Double_t Xlower, Double_t Xupper) {			// set both limits
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF::Set
// Purpose:        Set both limits of this window
// Arguments:      Int_t Xlower    -- limit
//                 Int_t Xupper    -- upper limit
// Results:
// Exceptions:
// Description:    Sets both limits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fX1 = Clip(Xlower);
	fXlower = fX1;
	fX2 = Clip(Xupper);
	fXupper = fX2;
	if (gPad) {
		Paint();
		gPad->Modified();
	}
};

void TMrbWindow::Draw(Option_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::Draw
// Purpose:        Draw a line representing this window
// Arguments:      Option_t * Option       -- option(s)
// Results:
// Exceptions:
// Description:    Draws a line with current attr represeinting this window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Option_t * opt;
	if (strlen(Option) == 0)	opt = Option;
	else 						opt = (Char_t *) GetOption();
	AppendPad(opt);
}

void TMrbWindowI::PaintLine(Double_t Xlower, Double_t /*Ylower*/, Double_t Xupper, Double_t /*Yupper*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowI::PaintLine
// Purpose:        Draw window with new coordinates
// Arguments:      Double_t Xlower      -- lower edge
//                 Double_t Ylower      -- (discarded)
//                 Double_t Xupper      -- upper edge
//                 Double_t Yupper      -- (discarded)
// Results:
// Exceptions:
// Description:    Draws window with new coordinates.
//                 Values Ylower/Yupper will be set to 1/4 pad height.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fX1 = (Double_t) ((Int_t) (Xlower + .5));
	fX2 = (Double_t) ((Int_t) (Xupper + .5));
	Paint();
}

void TMrbWindow::Paint(Option_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow::Paint
// Purpose:        Draw window with current coordinates
// Arguments:      Option_t * Option   -- option
// Results:
// Exceptions:
// Description:    Draws window with current coordinates.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t x, dx, y, dy;
	TString wdwName;

	if(gPad == NULL) {
		gMrbLog->Err() << "No pad defined" << endl;
		gMrbLog->Flush(this->ClassName(), "Paint");
		return;
	}

	TString opt = Option;
	opt.ToLower();
	TAttLine::Modify();
	TAttText::Modify();

	y = gPad->GetUymax() - gPad->GetUymin();
	dy = 0.02 * y;
	if(fY1 == 0 && fY2 == 0){
		y = gPad->GetUymin() + 0.25 * y;
	} else y=fY1;

	if (fX1 > fX2) {
		x = fX1;
		fX1 = fX2;
		fX2 = x;
	};

	fY1 = y;
	fY2 = y;
	gPad->PaintLine(fX1, y, fX2, y);
	gPad->PaintLine(fX1, y-dy, fX1, y+dy);
	gPad->PaintLine(fX2, y-dy, fX2, y+dy);

	x = gPad->GetUxmax() - gPad->GetUxmin();
	dx = 0.02 * x;
	wdwName = GetName();
	gPad->PaintText(fX2 + dx, fY1 + dy, wdwName.Data());
}

void TMrbWindowF::PaintLine(Double_t Xlower, Double_t /*Ylower*/,
                            Double_t Xupper, Double_t /*Yupper*/) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindowF::PaintLine
// Purpose:        Draw window with new coordinates
// Arguments:      Double_t Xlower      -- lower edge
//                 Double_t Ylower      -- (discarded)
//                 Double_t Xupper      -- upper edge
//                 Double_t Yupper      -- (discarded)
// Results:
// Exceptions:
// Description:    Draws window with new coordinates.
//                 Values Ylower/Yupper will be set to 1/4 pad height.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fX1 = Xlower;
	fX2 = Xupper;
	Paint();
};

TMrbWindow2D::TMrbWindow2D() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D
// Purpose:        Default constructor
// Description:    Creates list of vars/wdws.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
	fParent = NULL;
}


TMrbWindow2D::TMrbWindow2D(const Char_t * WdwName, Int_t N, Double_t * X, Double_t * Y) :
														TCutG(WdwName, N, (Double_t *) X, (Double_t *) Y) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D
// Purpose:        Default constructor
// Description:    Creates list of vars/wdws.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
	fParent = NULL;
	fXtext = 0;
	fYtext = 0;
	SetInitialType(kWindow2D);
};

TMrbWindow2D::TMrbWindow2D(TCutG cut) : TCutG(cut) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D
// Purpose:        Default constructor
// Description:    Creates list of vars/wdws.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLofUserVars == NULL) gMrbLofUserVars = new TMrbLofUserVars("SystemVars");
	fParent = NULL;
}

TMrbWindow2D::~TMrbWindow2D() {
//__________________________________________________________________[C++ DTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           ~TMrbWindow2D
// Purpose:        Destructor
// Description:    Remove window from lists.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gROOT->GetListOfSpecials()->Remove(this);
	gDirectory->GetList()->Remove(this);
	gMrbLofUserVars->Remove(this);
	if (gPad) gPad->GetListOfPrimitives()->Remove(this);
	if (fParent && fParent->InheritsFrom(TH1::Class())) ((TH1*)fParent)->GetListOfFunctions()->Remove(this);
	fParent = NULL;
}

void TMrbWindow2D::SetInitialType(UInt_t WdwType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D::SetInitialType
// Purpose:        Set variable type
// Arguments:      UInt_t WdwType    -- type bits
// Results:
// Exceptions:
// Description:    Defines type bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->GetUniqueID() != 0) return;

	this->SetUniqueID(WdwType);
	gMrbLofUserVars->Append(this);
//	if (gDirectory != NULL) gDirectory->Append(this);
}

Bool_t TMrbWindow2D::AddToHist(TObject * Histogram) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D::AddToHist
// Purpose:        Connect window to Histogram
// Arguments:      TObject * Histogram   -- object inherited from TH2
// Results:
// Exceptions:
// Description:    Add a window to the list of function of a histo.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Histogram->InheritsFrom("TH2")) {
		((TH1 *) Histogram)->GetListOfFunctions()->Add(this);
		this->SetLineColor(1);
		this->SetLineStyle(1);
		this->SetLineWidth(1);
		this->SetTextAlign(11);
		this->SetTextAngle(0);
		this->SetTextColor(1);
		this->SetTextFont(62);
		this->SetTextSize(0.04);
		fParent = Histogram;
		return(kTRUE);
	} else {
		gMrbLog->Err() << "Not a 2-dim histogram - " << Histogram->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "AddToHist");
		return(kFALSE);
	}

}

void TMrbWindow2D::Draw(Option_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D::Draw
// Purpose:        Draw a polygon representing this window
// Arguments:      Option_t * Option       -- option(s)
// Results:
// Exceptions:
// Description:    Draws a polycon with current attr represeinting this window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Option_t * opt;
	if (strlen(Option) == 0)	opt = Option;
	else 						opt = (Char_t *) GetOption();
/*
   TText *t = (TText*)gPad->GetListOfPrimitives()->FindObject(GetName());
   if(t){
      SetXtext(t->GetX());
      SetYtext(t->GetX());
      gPad->GetListOfPrimitives()->Remove(t);
      delete t;
   }
   t = new TText(fXtext+fX[0], fYtext+fY[0], GetName());
   t->SetName(GetName());
   t->Draw();
*/
	AppendPad(opt);
}

void TMrbWindow2D::Print(Option_t * Option) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWindow2D::Print
// Purpose:        Print current window limits
// Arguments:      Char_t * Option  -- option
// Results:
// Exceptions:
// Description:    Outputs window limits to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t vOpt;

	TMrbLofNamedX varOptions;

	vOpt = varOptions.CheckPatternShort(this->ClassName(), "Print", Option, kMrbVarWdwOutput);
	if (vOpt == TMrbLofNamedX::kIllIndexBit) return;

	if (vOpt & kOutputShort) {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "cut";
		cout	<< endl;
	} else {
		cout	<< "   "
				<< setiosflags(ios::left) << setw(15) << GetName();
		cout	<< resetiosflags(ios::left)
				<< setw(9) << "cut";
		for (Int_t i = 0; i < this->GetN(); i++) {
			Double_t x, y;
			this->GetPoint(i, x, y);
			if (i == 0) {
				cout	<< setw(19) << x
						<< setw(18) << y;
			} else {
				cout	<< setw(46) << x
						<< setw(18) << y;
			}
			cout	<< endl;
		}
	}
}

TMbsWindow2d::~TMbsWindow2d() {
//__________________________________________________________________[C++ DTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           ~TMbsWindow2d
// Purpose:        Destructor
// Description:    Remove window from lists.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gDirectory->GetList()->Remove(this);
//	gMrbLofUserVars->Remove(this);
	if (gPad) gPad->GetListOfPrimitives()->Remove(this);
	if (fParent && fParent->InheritsFrom(TH1::Class())) ((TH1*)fParent)->GetListOfFunctions()->Remove(this);
	fParent = NULL;
}

void TMbsWindow2d::Draw(Option_t * Option) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsWindow2d::Draw
// Purpose:        Draw a polygon representing this window
// Arguments:      Option_t * Option       -- option(s)
// Results:
// Exceptions:
// Description:    Draws a polycon with current attr represeinting this window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Option_t * opt;
	if (strlen(Option) == 0)	opt = Option;
	else 						opt = (Char_t *) GetOption();
	AppendPad(opt);
}

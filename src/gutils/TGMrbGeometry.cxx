//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbGeometry.cxx
// Purpose:        MARaBOU utilities: Window geometry
// Description:    Implements utilities to specify window geometry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbGeometry.cxx,v 1.4 2009-09-23 10:42:51 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TROOT.h"
#include "TEnv.h"
#include "TGMrbGeometry.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TGMrbGeometry)

extern TMrbLogger * gMrbLog;			// access to message lgging

TGMrbGeometry * gMrbGeometry = NULL;	// global instance

TGMrbGeometry::TGMrbGeometry(TGWindow * Parent, Double_t ScaleFactor) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGeometry
// Purpose:        Define window size
// Arguments:      TGWindow * Parent     -- parent window
//                 Double_t ScaleFactor  -- scale factor
// Description:    Defines window scale (<= 1).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (gMrbGeometry && Parent == NULL) {
		gMrbLog->Err()	<< gROOT->GetName() << ": Application geometry already defined. Ignored." << endl;
		gMrbLog->Flush("TGMrbGeometry");
		this->MakeZombie();
	} else {
		fApplication = gROOT->GetName();
		fParent = Parent;

		ULong_t wdwId = Parent ? Parent->GetId() : gClient->GetRoot()->GetId();
		gVirtualX->GetWindowSize(wdwId, fParentX, fParentX, fParentWidth, fParentHeight);
		if (!this->SetScaleFactor(ScaleFactor)) this->MakeZombie();
		gMrbGeometry = this;
	}
}

TGMrbGeometry::TGMrbGeometry(const Char_t * Application, Double_t ScaleFactor) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGeometry
// Purpose:        Define window size
// Arguments:      Char_t * Application  -- name of application
//                 Double_t ScaleFactor  -- scale factor
// Description:    Defines window scale (<= 1).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (gMrbGeometry) {
		gMrbLog->Err()	<< Application << ": Application geometry already defined. Ignored." << endl;
		gMrbLog->Flush("TGMrbGeometry");
		this->MakeZombie();
	} else {
		fApplication = Application;
		fParent = NULL;

		gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(), fParentX, fParentY, fParentWidth, fParentHeight);
		if (!this->SetScaleFactor(ScaleFactor)) this->MakeZombie();
		gMrbGeometry = this;
	}
}

Bool_t TGMrbGeometry::SetScaleFactor(Double_t ScaleFactor) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGeometry::SetScaleFactor
// Purpose:        Set window scale factor
// Arguments:      Double_t ScaleFactor   -- scale factor < 1.
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines window scale.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ScaleFactor <= 0.) {
		TString res = fApplication;
		res += ".ScaleFactor";
		ScaleFactor = gEnv->GetValue(res.Data(), (Double_t) kGMrbScalePerc / 100.);
	}
	fScaleFactor = ScaleFactor;
	if (fScaleFactor > 1.) {
		gMrbLog->Err()	<< "Wrong scale factor - " << fScaleFactor << " (should be <= 1.)" << endl;
		gMrbLog->Flush("TGMrbGeometry");
		return(kFALSE);
	}
	fX = fParentX;
	fY = fParentY;
	fWidth = (UInt_t) (fParentWidth * fScaleFactor +.5);
	fHeight = (UInt_t) (fParentHeight * fScaleFactor +.5);
	return(kTRUE);
}

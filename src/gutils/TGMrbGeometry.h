#ifndef __TGMrbGeometry_h__
#define __TGMrbGeometry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbGeometry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbGeometry                  -- window geometry
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbGeometry.h,v 1.1 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"

#include "TSystem.h"
#include "TVirtualX.h"
#include "TGWindow.h"
#include "TGClient.h"
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGeometry
// Purpose:        A utility to ease use of ROOT's GC mechanisms
// Description:    Defines window geometry
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbGeometry : public TObject {

	public:
		enum	{	kGMrbScalePerc	=	80	};

	public:
		TGMrbGeometry(const Char_t * Application, Double_t ScaleFactor = 0.); // ctor
		TGMrbGeometry(TGWindow * Parent = NULL, Double_t ScaleFactor = 0.);
		~TGMrbGeometry() {};						// dtor

		inline const Char_t * GetApplication() { return(fApplication.Data()); };

		Bool_t SetScaleFactor(Double_t ScaleFactor = 0.);
		inline Double_t GetScaleFactor() { return(fScaleFactor); };

		inline Int_t GetParentX() { return(fParentX); };
		inline Int_t GetParentY() { return(fParentY); };
		inline Int_t GetParentWidth() { return((Int_t) fParentWidth); };
		inline Int_t GetParentHeight() { return((Int_t) fParentHeight); };
		inline Int_t GetX() { return(fX); };
		inline Int_t GetY() { return(fY); };
		inline Int_t GetWidth() { return((Int_t) fWidth); };
		inline Int_t GetHeight() { return((Int_t) fHeight); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TString fApplication;
		TGWindow * fParent;
		Double_t fScaleFactor;
		Int_t fParentX;
		Int_t fParentY;
		UInt_t fParentWidth;
		UInt_t fParentHeight;
		Int_t fX;
		Int_t fY;
		UInt_t fWidth;
		UInt_t fHeight;

	ClassDef(TGMrbGeometry, 0)		// [GraphUtils] Window geometry
};

#endif

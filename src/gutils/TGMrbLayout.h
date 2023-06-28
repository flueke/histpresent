#ifndef __TGMrbLayout_h__
#define __TGMrbLayout_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLayout.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLayout                  -- layout & graphic context
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLayout.h,v 1.5 2007-09-26 07:42:42 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"
#include "TGGC.h"
#include "TGFont.h"
#include "TGResourcePool.h"

#include "TGLayout.h"
#include "TSystem.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        A utility to ease use of ROOT's GC mechanisms
// Description:    Handles graphic contexts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLayout : public TObject {

	public:
		TGMrbLayout(const Char_t * Font = "Gui.NormalFont",
					const Char_t * Foreground = "black",
					const Char_t * Background = "white",
					TGLayoutHints * Hints = NULL);

		TGMrbLayout(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, TGLayoutHints * Hints = NULL);

		~TGMrbLayout() {};						// dtor

		inline GContext_t GC() const { return(fGC.GetGC()); };

		Bool_t SetFont(const Char_t * Font);
		inline FontStruct_t Font() const { return(fFont->GetFontStruct()); };
		inline FontH_t FontHandle() const { return(fFont->GetFontHandle()); };

		Bool_t SetFG(const Char_t * Foreground);
		inline Pixel_t FG() const { return(fGC.GetForeground()); };

		Bool_t SetBG(const Char_t * Background);
		inline Pixel_t BG() const { return(fGC.GetBackground()); };

		inline void SetLH(TGLayoutHints * Hints) { fLayoutHints = Hints; };
		inline TGLayoutHints * LH() const { return(fLayoutHints); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void CreateGC(TGFont * Font, Pixel_t Foreground, Pixel_t Background, TGLayoutHints * Hints = NULL);

	protected:
		TGGC fGC;
		TGFont * fFont;
		TGLayoutHints * fLayoutHints;

	ClassDef(TGMrbLayout, 0)		// [GraphUtils] Describe a graphical layout (colors, fonts, layout hints)
};

#endif

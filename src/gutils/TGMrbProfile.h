#ifndef __TGMrbProfile_h__
#define __TGMrbProfile_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbProfile.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbProfile.h,v 1.2 2007-07-30 12:25:32 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"

#include "TGClient.h"
#include "TGGC.h"
#include "TGLayout.h"
#include "TGFont.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbGC
// Purpose:        A wrapper around TGGC object
// Description:    Handles graphics contexts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbGC : public TObject {

	public:
		enum EGMrbButtonType		{	kGMrbTextButton 			= BIT(0),
										kGMrbRadioButton			= BIT(1),
										kGMrbCheckButton			= BIT(2),
										kGMrbPictureButton			= BIT(3)
									};

		enum EGMrbButtonFrameType 	{	kGMrbButtonList 			= BIT(4),
										kGMrbButtonGroup			= BIT(5),
										kGMrbButtonExpand			= BIT(6)
									};

		enum EGMrbGCType			{	kGMrbGCFrame				= BIT(7),
										kGMrbGCLabel				= BIT(8),
										kGMrbGCButton				= BIT(9),
										kGMrbGCEntry				= BIT(10),
										kGMrbGCTextButton			= kGMrbGCButton | kGMrbTextButton,
										kGMrbGCRadioButton			= kGMrbGCButton | kGMrbRadioButton,
										kGMrbGCCheckButton			= kGMrbGCButton | kGMrbCheckButton,
										kGMrbGCPictureButton		= kGMrbGCButton | kGMrbPictureButton,
										kGMrbGCTextEntry			= kGMrbGCEntry | (kGMrbPictureButton << 1),
										kGMrbGCLBEntry				= kGMrbGCEntry | (kGMrbPictureButton << 2)
							};

	public:
		TGMrbGC(const Char_t * Font = "Gui.NormalFont",
					const Char_t * Foreground = "Gui.Foreground",
					const Char_t * Background = "Gui.Background",
					UInt_t Options = 0);

		TGMrbGC(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, UInt_t Options = 0);

		TGMrbGC(TGMrbGC::EGMrbGCType Type, UInt_t Options = 0);

		~TGMrbGC() {};						// dtor

		inline TGGC * GCAddr() const { return(fGC); };
		inline GContext_t GC() const { return(fGC->GetGC()); };

		Bool_t SetFont(const Char_t * Font);
		inline TGFont * Font() const { return(fFont); };
		inline const Char_t * GetFontName() { return(fFontName.Data()); };

		Bool_t SetFG(const Char_t * Foreground);
		inline void SetFG(Pixel_t Foreground) { fForeground = Foreground; };
		inline Pixel_t FG() const { return(fForeground); };
		inline const Char_t * GetForegroundName() { return(fForegroundName.Data()); };

		Bool_t SetBG(const Char_t * Background);
		inline void SetBG(Pixel_t Background) { fBackground = Background; };
		inline Pixel_t BG() const { return(fBackground); };
		inline const Char_t * GetBackgroundName() { return(fBackgroundName.Data()); };

		inline void SetOptions(UInt_t Options) { fOptions = Options; };
		inline UInt_t GetOptions() { return(fOptions); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbGC.html&"); };

	protected:
		TGGC * fGC;
		TString fFontName;
		TString fForegroundName;
		TString fBackgroundName;
		TGFont * fFont;
		Pixel_t fForeground;
		Pixel_t fBackground;
		UInt_t fOptions;

	ClassDef(TGMrbGC, 0)		// [GraphUtils] Graphics context (colors, fonts)
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbProfile
// Purpose:        Graphics profile
// Description:    Handles a list of TGMrbGC objects
//                 together with global graphics options.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbProfile : public TNamed {

	public:
		TGMrbProfile(const Char_t * Name, const Char_t * Title, TEnv * Env = NULL);

		~TGMrbProfile() {};						// dtor

		Bool_t SetGC(TGMrbGC::EGMrbGCType Type, TGMrbGC * GC);
		Bool_t SetGC(TGMrbGC::EGMrbGCType Type,	const Char_t * Font = "Gui.NormalFont",
										const Char_t * Foreground = "Gui.Foreground",
										const Char_t * Background = "Gui.Background");
		Bool_t SetGC(TGMrbGC::EGMrbGCType Type, const Char_t * Font, Pixel_t Foreground, Pixel_t Background);

		TGMrbGC * GetGC(TGMrbGC::EGMrbGCType Type);

		void SetOptions(TGMrbGC::EGMrbGCType Type, UInt_t Options);
		void SetOptions(const Char_t * Type, UInt_t Options);
		UInt_t GetOptions(TGMrbGC::EGMrbGCType Type);
		UInt_t GetOptions(const Char_t * Type);

		inline UInt_t GetFrameOptions() {
				return(((TGMrbGC *) ((TMrbNamedX *) fLofGCs[TGMrbGC::kGMrbGCFrame])->GetAssignedObject())->GetOptions());
		};

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, const Char_t * Type = NULL) const;
		inline void Print() const { Print(cout); };

	protected:
		TGMrbGC * AddGC(TMrbNamedX * GCSpecs, TEnv * Env = NULL, TGMrbGC * DefaultGC = NULL);

	protected:
		TMrbLofNamedX fLofGCs;			// graphics contexts

	ClassDef(TGMrbProfile, 0)		// [GraphUtils] A graphics profile: a set of graphics contexts and options
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLofProfiles
// Purpose:        A list of graphics profiles
// Description:    Handles a list of graphics profiles.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLofProfiles : public TMrbLofNamedX {

	public:
		TGMrbLofProfiles(const Char_t * Name = "LofGraphProfiles", const Char_t * Title = "");

		~TGMrbLofProfiles() {};						// dtor

		TGMrbProfile * AddProfile(const Char_t * Name, const Char_t * Title);
		TGMrbProfile * FindProfile(const Char_t * Name, Bool_t FallBack = kFALSE);

		TGMrbProfile * SetDefault(const Char_t * DefaultProfile = "standard");
		inline TGMrbProfile * GetDefault() { return(fDefault); };

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, const Char_t * ProfileName = NULL) const;
		inline void Print() const { Print(cout); };

	protected:

		TString fRcFile;			// where graph envs are defined
		TEnv * fEnv;				// ptr to environment data
		TGMrbProfile * fDefault;	// default profile

	ClassDef(TGMrbLofProfiles, 0)		// [GraphUtils] A list of graphics profiles
};
#endif

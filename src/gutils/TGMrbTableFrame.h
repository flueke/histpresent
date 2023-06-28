#ifndef __TGMrbTableFrame_h__
#define __TGMrbTableFrame_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbTableFrame.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbTableFrame       -- a dialog frame containing a N * M matrix
//                 TGMrbTableOfInts      -- ... pure int elements
//                 TGMrbTableOfDoubles   -- ... pure double elements
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbTableFrame.h,v 1.8 2006-10-02 13:28:45 Otto.Schaile Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TOrdCollection.h"
#include "TArrayI.h"
#include "TArrayD.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TList.h"

#include "TGTextEntry.h"
#include "TGTextBuffer.h"
#include "TGLayout.h"
#include "TGLabel.h"
#include "TGColorSelect.h"

//#include "TMrbString.h"
//#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableFrame
// Purpose:        Provide matrix of editable entry widgets
// Methods:        ProcessMessage   -- handle X events
// Description:    A transient window showing N * M entries and checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbTableFrame : public TGTransientFrame {

	public:
		enum EGMrbTableFrameCmdId		{	kTableFrameCancel, kTableFrameOk,
                                        	kTableFrameHelp, kTableFrameAct_2,
											kTableFrameAll_1, kTableFrameNone_1,
											kTableFrameAll_2, kTableFrameNone_2
										};
		enum EGMrbTableFrameActions		{	kFlagNoop, kFlagColor,
                                  kFlagFillStyle, kFlagLineStyle
										};

	public:
		TGMrbTableFrame(const TGWindow * MainFrame, Int_t * RetValue, const Char_t * Title,
								Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
								TOrdCollection * Values,
								TOrdCollection * ColumnLabels = NULL,
								TOrdCollection * RowLabels = NULL,
								TArrayI * Flags = NULL,
                        		Int_t Nradio = 0,
                        		const Char_t * HelpText = NULL,
                        		const Char_t * ActionText_1 = NULL,
                        		const Char_t * ActionText_2 = NULL);

		virtual ~TGMrbTableFrame();

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);
		void StoreValues();
		void CloseWindow();
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	private:
		TList *fWidgets;					// list of widgets on heap
		TGCompositeFrame	   *fColFrame;
		TGCompositeFrame	   *fTableRowFrame;
		TGCompositeFrame	   *fRowFrame;
		TGCompositeFrame	   *fTableFrame;
		TGCompositeFrame	   *fFlagFrame1;
		TGCompositeFrame	   *fFlagFrame2;
		TGButton		         *fFlagButton;
		TList				      *fEntries;
		TList				      *fFlags;
		TGTextEntry 		   *fTEItem;
		TGTextBuffer		   *fTBItem;
		TGMatrixLayout  	   *fLMMatrix;
		TGCompositeFrame	   *fActionFrame;
		TGCompositeFrame	   *fColLabFrame;
		TGLabel 			      *fColLabel;
		TGCompositeFrame	   *fRowLabFrame;
		TGLabel 			      *fRowLabel;
		TGTextButton		   *fCancelButton;
		TGTextButton		   *fOkButton;
		TGTextButton		   *fAct_2Button;
		TGTextButton		   *fHelpButton;
		TOrdCollection  	   *fValues;
		TGPictureButton 		*fBtnAll1, *fBtnAll2, *fBtnNone1, *fBtnNone2;
		Int_t fNrows;
		Int_t fNcols;
		Int_t fNent;
		Int_t *fRet;
		TArrayI *fFlagsReturn;
      Int_t fRadio;
      Int_t fCheck;
      Int_t fColorSelect;
      Bool_t fCancelOk;
      const Char_t * fHelpText;

	ClassDef(TGMrbTableFrame, 0)	// [GraphUtils] A rows x cols table
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableOfInts
// Purpose:        Provide matrix of integer entries
// Description:    A transient window showing integer entries and checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbTableOfInts {

	public:
		TGMrbTableOfInts(const TGWindow * MainFrame, Int_t * RetValue, const Char_t * Title,
											Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
											TArrayI & Values, Int_t Base = 0,
											TOrdCollection * ColumnLabels = NULL,
											TOrdCollection * RowLabels = NULL,
											TArrayI * Flags = NULL,
                        			Int_t Nradio = 0,
                        			const Char_t * HelpText = NULL,
                                 const Char_t * ActionText_1 = NULL,
                                 const Char_t * ActionText_2 = NULL);

		virtual ~TGMrbTableOfInts() {};

		TGMrbTableOfInts(const TGMrbTableOfInts &) {};				// default copy ctor

		inline void Help() { gSystem->Exec("mrbHelp TGMrbTableOfInts"); };

	ClassDef(TGMrbTableOfInts,0)
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTableOfDoubles
// Purpose:        Provide matrix of double entries
// Description:    A transient window showing double entries and checkbuttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TGMrbTableOfDoubles {

	public:
		TGMrbTableOfDoubles(const TGWindow * MainFrame, Int_t * RetValue, const Char_t * Title,
											Int_t ItemWidth, Int_t Ncols, Int_t Nrows,
											TArrayD & Values, Int_t Precision = 0,
											TOrdCollection * ColumnLabels = NULL,
											TOrdCollection * RowLabels = NULL,
											TArrayI * Flags = NULL,
                        			Int_t Nradio = 0,
                        			const Char_t * HelpText = NULL,
                                 const Char_t * ActionText_1 = NULL,
                                 const Char_t * ActionText_2 = NULL);

		virtual ~TGMrbTableOfDoubles() {};

		TGMrbTableOfDoubles(const TGMrbTableOfDoubles &) {};				// default copy ctor

		inline void Help() { gSystem->Exec("mrbHelp TGMrbTableOfDoubles"); };

	ClassDef(TGMrbTableOfDoubles,0)
};

#endif

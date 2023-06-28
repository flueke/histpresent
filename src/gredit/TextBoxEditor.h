// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TextBoxEditor.h
// Author: Ilka  Antcheva, Otto Schaile 15/12/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TextBoxEditor
#define ROOT_TextBoxEditor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TextBoxEditor                                                          //
//                                                                      //
//  Implements GUI for editing Arc attributes: radius, phi1, phi2.      //                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGButton
#include "TGWidget.h"
#endif
#ifndef ROOT_TGedFrame
#include "TGedFrame.h"
#endif

class TGComboBox;
class TGNumberEntry;
class TGRadioButton;
class TGCheckButton;
class TextBox;

class TextBoxEditor : public TGedFrame {

protected:
   TextBox              *fTextBox; 
   TGNumberEntry        *fX1Entry; 
   TGNumberEntry        *fY1Entry; 
   TGNumberEntry        *fX2Entry; 
   TGNumberEntry        *fY2Entry;
   TGNumberEntry        *fCRadEntry;   
   virtual void   ConnectSignals2Slots();
   TGComboBox    *BuildOptionComboBox(TGFrame* parent, Int_t id);
   
public:
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   TextBoxEditor(const TGWindow *p = 0,
#else
   TextBoxEditor(const TGWindow *p, Int_t id,
#endif
                Int_t width = 140, Int_t height = 30,
                UInt_t options = kChildFrame,
                Pixel_t back = GetDefaultFrameBackground());
   virtual ~TextBoxEditor();

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   virtual void   SetModel(TObject *obj);
   void           ActivateBaseClassEditors(TClass* cl);
#else
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
#endif
   virtual void   Redraw();
	virtual void   SetCRadius();

   ClassDef(TextBoxEditor,0)  // GUI for editing arrow attributes
};

#endif

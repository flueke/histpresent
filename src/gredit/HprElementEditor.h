// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: HprElementEditor.h
// Author: Ilka  Antcheva, Otto Schaile 15/12/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_HprElementEditor
#define ROOT_HprElementEditor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  HprElementEditor                                                          //
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

class TGNumberEntry;
class HprElement;

class HprElementEditor : public TGedFrame {

protected:
   HprElement           *fHprElement;     // pointer to the object
   TGNumberEntry        *fPlaneEntry;     // plane entry
   TGNumberEntry        *fVisEntry;       // visibility entry
   virtual void   ConnectSignals2Slots();

public:
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   HprElementEditor(const TGWindow *p = 0,
#else
   HprElementEditor(const TGWindow *p, Int_t id,
#endif
                Int_t width = 140, Int_t height = 30,
                UInt_t options = kChildFrame,
                Pixel_t back = GetDefaultFrameBackground());
   virtual ~HprElementEditor();
   virtual void DoSetPlane();
   virtual void DoSetVisibility();

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   virtual void   SetModel(TObject *obj);
   void           ActivateBaseClassEditors(TClass* cl);
#else
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
#endif
   ClassDef(HprElementEditor,0)  // GUI for editing arrow attributes
};

#endif

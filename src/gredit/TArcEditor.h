// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TArcEditor.h
// Author: Ilka  Antcheva, Otto Schaile 15/12/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TArcEditor
#define ROOT_TArcEditor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TArcEditor                                                          //
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
class TArc;

class TArcEditor : public TGedFrame {

protected:
   TArc                 *fArc;             // pointer to the Arc object
   TGNumberEntry        *fRadiusEntry;     // radius entry
   TGNumberEntry        *fPhiminEntry;     // Phimin entry
   TGNumberEntry        *fPhimaxEntry;     // Phimax entry
   TGNumberEntry        *fCenterXEntry;    // center x entry
   TGNumberEntry        *fCenterYEntry;    // center y entry
   TGCheckButton        *fDrawEdgesEntry; 
/*
   TGNumberEntry        *fPoCfX1Entry;     // center x entry
   TGNumberEntry        *fPoCfY1Entry;     // center y entry
   TGNumberEntry        *fPoCfX2Entry;     // center x entry
   TGNumberEntry        *fPoCfY2Entry;     // center y entry
   TGRadioButton        *fUseCenterPhiEntry; 
   TGCheckButton        *fSenseEntry; 
   TGRadioButton        *fUsePoCfEntry; 
   Bool_t               fUseCenterPhi;   // use radius, center, phimin, phimax
                                           // otherwise radius + 2 points on
                                           // circumference
*/
   virtual void   ConnectSignals2Slots();
   TGComboBox    *BuildOptionComboBox(TGFrame* parent, Int_t id);
   
public:
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   TArcEditor(const TGWindow *p = 0,
#else
   TArcEditor(const TGWindow *p, Int_t id,
#endif
                Int_t width = 140, Int_t height = 30,
                UInt_t options = kChildFrame,
                Pixel_t back = GetDefaultFrameBackground());
   virtual ~TArcEditor();

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   virtual void   SetModel(TObject *obj);
   void           ActivateBaseClassEditors(TClass* cl);
#else
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
#endif
   virtual void   DoRedraw();
   virtual void   Redraw();
   virtual void   DoCenterPhi();
//   virtual void   DoPointsOnCf();
//   virtual void   DoUseCenterPhi();
//   virtual void   DoUsePointsOnCf();

   ClassDef(TArcEditor,0)  // GUI for editing arrow attributes
};

#endif

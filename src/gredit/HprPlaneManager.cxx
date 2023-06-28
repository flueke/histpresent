#include "TROOT.h"
#include "TEnv.h"
#include "HprElement.h"
#include "HprPlaneManager.h"
#include <iostream>

namespace std {} using namespace std;
//___________________________________________________________________

HprPlaneManager::HprPlaneManager(TGWindow * win)
{

//   TRootCanvas *rc = (TRootCanvas*)win;
//   fCanvas = rc->Canvas();
   fCanvas = gPad->GetCanvas();
   fWindow = win;
   fDialog = NULL;
   gROOT->GetListOfCleanups()->Add(this);
   OpenWidget();
}
//__________________________________________________________________
void HprPlaneManager::OpenWidget()
{
// *INDENT-OFF*
   static const char helptext[] =
"\n\
Each graphic element is associated with a drawing plane[1-100].\n\
This determines the order of drawing, planes with higher numbers\n\
are drawn at last. Elements of one plane can be made invisible\n\
together and shifted to a different plane individually or together\n\
In the latter case the target plane must be empty.\n\
\n\
";
// *INDENT-ON*
   if (fDialog != NULL) {
      cout << "Widget already open" << endl;
      return;
   }
   fRow_lab = new TList();
//   RestoreDefaults();
   static Int_t dummy;
   Int_t ind = 0;
   fCurrentActive = 0;
   fGrCanvas = dynamic_cast<GrCanvas*>(gPad);
   if (!fGrCanvas) {
		fGrCanvas = dynamic_cast<GrCanvas*>(gPad->GetMother());
		if (!fGrCanvas) {
			cout << "Cant get active canvas" << endl;
			return;
		}
   }
   if (!fGrCanvas->TestBit(GrCanvas::kIsAEditorPage)) {
      cout << "Active canvas is not an editor page" << endl;
      return;
   }

   fCurrentActive = fGrCanvas->GetCurrentPlane();
   fRow_lab->Add(new TObjString("PlainIntVal_Current active plane"));
   fBidCurrentActive = ind;
   fValp[ind++] = &fCurrentActive;
   TString label;
   Int_t nElements = FillPlaneList();
//   cout << "nElements " <<nElements  << endl;

   if (nElements > 0) {
      for (Int_t i =0; i < 100; i++) {
			fPlaneIndexMove[i] = 0;
			fPlaneIndexVis[i] = 0;
         if (fUsedPlanes[i] > 0) {
             label = "CommentOnly_Plane ";
             label += i;
             label += " NofElts ";
             label += fUsedPlanes[i];
             label += " ",
             fRow_lab->Add(new TObjString(label.Data()));
             fValp[ind++] = &dummy;
             fRow_lab->Add(new TObjString("CheckButton- Visible "));
             fPlaneIndexVis[i] = ind;
             fValp[ind++] = &fVisibility[i];
             fRow_lab->Add(new TObjString("Exec_Button-Move to"));
             fPlaneIndexMove[i] = ind;
             fValp[ind++] = &dummy;
             fRow_lab->Add(new TObjString("PlainIntVal-"));
             fMoveToPlane[i] = i;
             fValp[ind++] = &fMoveToPlane[i];
         }
      }
   }
   static Int_t ok;
   Int_t itemwidth =  55 * TGMrbValuesAndText::LabelLetterWidth();
//   cout << this->ClassName()<< endl;
   fDialog =
      new TGMrbValuesAndText("PlaneManager", NULL, &ok,itemwidth, fWindow,
                      NULL, NULL, fRow_lab, fValp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//__________________________________________________________________
void HprPlaneManager::RecursiveRemove(TObject * obj)
{
   if (obj == fCanvas) {
 //     cout << "HprPlaneManager: CloseDialog "  << endl;
      CloseDialog();
   }
}
//__________________________________________________________________
void HprPlaneManager::CloseDialog()
{
//   cout << "HprPlaneManager::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   fRow_lab->Delete();
   delete fRow_lab;
   delete this;
}
//__________________________________________________________________
void HprPlaneManager::CloseWidget()
{
//   cout << "HprPlaneManager::CloseDialog() " << endl;
   if (fDialog) fDialog->CloseWindowExt();
   fDialog = NULL;
   fRow_lab->Delete();
   delete fRow_lab;
}
//_______________________________________________________________________

void HprPlaneManager::SaveDefaults()
{
//   TEnv env(".hprrc");
//   env.SetValue("HprPlaneManager.fMainWidth", fMainWidth);
//   env.SaveLevel(kEnvLocal);
}
//___________________________________________________________________

void HprPlaneManager::RestoreDefaults()
{
//   TEnv env(".hprrc");
//   fMainWidth = env.GetValue("HprPlaneManager.fMainWidth", 250);
}
//___________________________________________________________________

void HprPlaneManager::CloseDown(Int_t wid)
{
//   cout << "CloseDown(" << wid<< ")" <<endl;
   fDialog = NULL;
   if (wid == -1)
      SaveDefaults();
}
//______________________________________________________________________

void HprPlaneManager::CRButtonPressed(Int_t wid, Int_t bid, TObject */*obj*/)
{
   cout << "CRButtonPressed(" << wid<< ", " <<bid  << " " 
	<< fBidCurrentActive << " " <<fCurrentActive << endl;
   if (fGrCanvas) {
      if (bid == fBidCurrentActive) {
         if (fCurrentActive < 1 || fCurrentActive > 100) {
            cout << " Plane must be > 0 , <= 100 " << endl;
            fCurrentActive = 50;
         } else {
            fGrCanvas->SetCurrentPlane(fCurrentActive);
         }
      } else {
			
        for (Int_t plane =0; plane < 100; plane++) {
//			 cout << plane << " "<< fPlaneIndexVis[plane] << " " <<
//			 fVisibility[plane] << endl;
          if (bid == fPlaneIndexVis[plane]) {
             if (fVisibility[plane] == 1) {
                HprElement::MoveAllObjects(fGrCanvas->GetHiddenPrimitives(),
                               fGrCanvas->GetListOfPrimitives(),
                               plane, 1);
             } else {
                HprElement::MoveAllObjects(fGrCanvas->GetListOfPrimitives(),
                               fGrCanvas->GetHiddenPrimitives(),
                               plane, 0);
             }
             fGrCanvas->Modified();
             fGrCanvas->Update();
          } else {
             if (bid == fPlaneIndexMove[plane]) {
                if (fUsedPlanes[fMoveToPlane[plane]] != 0) {
                    cout << "plane: " << plane << " Targetplane: " << fMoveToPlane[plane] <<
                         " not empty" << endl;
                   } else {
                      MovePlaneNumber(plane, fMoveToPlane[plane]);
                      CloseWidget();
                      OpenWidget();
							 break;
                   }
                }
             }
          }
      }
   }
}
//___________________________________________________________________

Int_t HprPlaneManager::FillPlaneList()
{
   for (Int_t i =0; i < 100; i++) {
      fUsedPlanes[i] = 0;
      fVisibility[i] = 0;
   }
   Int_t nel = 0;
   TIter next(gPad->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("HprElement")) {
 //        cout << obj->ClassName() << endl;
         HprElement* hpre = dynamic_cast<HprElement*>(obj);
         Int_t plane = hpre->GetPlane();
         if (plane >=0 && plane < 100) {
            fUsedPlanes[plane] += 1;
            fVisibility[plane] = 1;
            nel++;
         }
      }
   }
   TIter next1(fGrCanvas->GetHiddenPrimitives());
   while ( (obj = next1()) ) {
      if (obj->InheritsFrom("HprElement")) {
         HprElement* hpre = dynamic_cast<HprElement*>(obj);
         Int_t plane = hpre->GetPlane();
         if (plane >=0 && plane < 100) {
            fUsedPlanes[plane] += 1;
            nel++;
         }
      }
   }
   return nel;
}
//___________________________________________________________________

Int_t HprPlaneManager::MovePlaneNumber(Int_t from, Int_t to)
{
   Int_t nel = 0;
   TIter next(fGrCanvas->GetListOfPrimitives());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("HprElement")) {
         HprElement* hpre = dynamic_cast<HprElement*>(obj);
         Int_t plane = hpre->GetPlane();
         if (plane == from) {
				cout << "Move " << obj->ClassName() << " from " <<
				from <<  " to " << to << endl;
            hpre->SetPlane(to);
            nel++;
         }
      }
   }
   TIter next1(fGrCanvas->GetHiddenPrimitives());
   while ( (obj = next1()) ) {
      if (obj->InheritsFrom("HprElement")) {
         HprElement* hpre = dynamic_cast<HprElement*>(obj);
         Int_t plane = hpre->GetPlane();
         if (plane == from) {
            hpre->SetPlane(to);
            nel++;
         }
      }
   }
   fUsedPlanes[to]   = nel;
   fUsedPlanes[from] = 0;
   fVisibility[to]   = fVisibility[from];
   CloseWidget();
   OpenWidget();
   return nel;
}

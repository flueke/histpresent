#include <TList.h>
#include "TArrayF.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjectTable.h"

#include "FhMarker.h"
#include "FitHist.h"
#include "CmdListEntry.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "support.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "Save2FileDialog.h"
#include "WindowSizeDialog.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

extern HistPresent *gHpr;
//______________________________________________________________________________________

void FitHist::MarksToWindow(){
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL ||  fMarkers->GetEntries() <= 0) return;
   int nval = fMarkers->GetEntries();
   FhMarker *ti;
   TIter next(fMarkers);
   if(is2dim(fSelHist)){
      MarksToCut();
   } else {
      if(nval != 2){
         WarnBox("Please set exactly 2 marks");
         return;
      } else {
         ti = (FhMarker*)next();
         Double_t x1 = ti->GetX();
//         Float_t x1 = ti->GetX();
         ti = (FhMarker*)next();
         Double_t x2 = ti->GetX();
//         Float_t x2 = ti->GetX();
         TString cname=this->GetName();
         Int_t us = cname.Index("E_");
         if(us >= 0)cname.Remove(0,2);
         us = cname.Index("_");
         if(us >= 0)cname.Remove(0,us+1);
         fCutNumber++;
         cname.Prepend(Form("%d_", fCutNumber));
         cname.Prepend("Wdw");
         Bool_t ok;
         cname = GetString("Window will be saved with name",cname.Data(),
                              &ok, mycanvas);
         if(!ok)return;
         CheckList(fAllWindows);
         TMrbWindow * wdw_old = (TMrbWindow *)fAllWindows->FindObject(cname.Data());
         if(wdw_old) {
            if (QuestionBox("Window with this name\n\
already exists, delete it? ", mycanvas)) {
               fAllWindows->Remove(wdw_old);
               fActiveWindows->Remove(wdw_old);
               delete wdw_old;
            } else {
               return;
            }
         }
         TMrbWindowF * wdw = new TMrbWindowF(cname.Data(), x1, x2);
         wdw->Draw();
         fCanvas->Update();
         fActiveWindows->Add(wdw);
         fAllWindows->Add(wdw);
      }
   }
}
//______________________________________________________________________________________

void FitHist::ClearRegion()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers &&  fMarkers->GetEntries() == 2)  {
		FhMarker *ti;
		if (is2dim(fSelHist)) {
         ti = (FhMarker*)fMarkers->At(0);
         Int_t binlx = XBinNumber(fSelHist, ti->GetX());
         Int_t binly = YBinNumber(fSelHist, ti->GetY());
         ti = (FhMarker*)fMarkers->At(1);
         Int_t binux = XBinNumber(fSelHist, ti->GetX());
         Int_t binuy = YBinNumber(fSelHist, ti->GetY());
         cout <<binlx << " " <<binux << " "<<binly << " "<<binuy << endl;
         for (Int_t binx = binlx; binx <= binux; binx++) {
            for (Int_t biny = binly; biny <= binuy; biny++) {
               fSelHist->SetBinContent(binx, biny, 0);
               fSelHist->SetBinError(binx, biny, 0);
            }
         }
         Draw2Dim();
		} else {
         ti = (FhMarker*)fMarkers->At(0);
         Int_t binl = fSelHist->FindBin(ti->GetX());
         ti = (FhMarker*)fMarkers->At(1);
         Int_t binu = fSelHist->FindBin(ti->GetX());
         for (Int_t bin = binl; bin <= binu; bin++) {
            fSelHist->SetBinContent(bin, 0);
            fSelHist->SetBinError(bin, 0);
         }
      }
      gPad->Modified();
      gPad->Update();
   } else {
      WarnBox("Please set exactly 2 marks");
      return;
   }
}
//______________________________________________________________________________________
Bool_t FitHist::UseWindow(TMrbWindow * wdw){
   if(!Nwindows())return kFALSE;
   return fActiveWindows->Contains(wdw);
}
//______________________________________________________________________________________

void FitHist::RemoveWindowsFromHist() {
   TList temp;
   TIter next(fSelHist->GetListOfFunctions());
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TMrbWindow") | obj->InheritsFrom("TMrbWindow2D") ) {
         temp.Add(obj);
      }
   }
   cout<< "Remove " << temp.GetSize()<< " window" ;
   if ( temp.GetSize() != 1 )
      cout << "s";
   cout << endl;
   if (temp.GetSize() > 0) { 
      if (fSelHist->GetDimension() == 1) 
         ClearWindows();
      else 
         ClearCut();
      TIter next1(&temp);
      while ( (obj = next1()) ) {
//         cout<< obj << endl;
         fSelHist->GetListOfFunctions()->Remove(obj);
         fSelPad->GetListOfPrimitives()->Remove(obj);
      }
      fSelPad->Modified();
      fSelPad->Update();
   }  
}
//______________________________________________________________________________________

void FitHist::AddWindowsToHist(){
   Int_t nval = CheckList(fActiveWindows);
   if(nval>0){
      TMrbWindow *wdw;
      TIter next(fActiveWindows);
      while( (wdw = (TMrbWindow*)next()) ){
         TMrbWindow * wdwcp;
         wdwcp = (TMrbWindow*)wdw->Clone();
         fSelHist->GetListOfFunctions()->Add(wdwcp);
         TMrbWindow * w = (TMrbWindow*)wdwcp;
         w->SetParent(fSelHist);
      }
   }
}
//______________________________________________________________________________________

void FitHist::ClearWindows(){
   Int_t nval = CheckList(fActiveWindows);
   if(nval>0){
      TMrbWindow *wdw;
      TIter next(fActiveWindows);
      while( (wdw = (TMrbWindow*)next()) ){
         gDirectory->GetList()->Remove(wdw);
      }
      fActiveWindows->Clear();
   }
}
//______________________________________________________________________________________

void FitHist::DrawWindows(){
   Int_t nval = CheckList(fActiveWindows);
   if(nval>0){
      fCanvas->cd();
      cout << "-- Current active Windows " << nval << " -----------" << endl;
      cout <<setw(9)<<setw(9)<< "Name"<<setw(9)<< "xlow" <<setw(9)<< "xup"
         <<setw(9)<< "Sum" <<setw(9)<< "Mean" <<setw(9)<< "Sigma" << endl;
      TMrbWindow *wdwf;
      TIter next(fActiveWindows);
      while( (wdwf = (TMrbWindow*)next()) ){
         Axis_t xlow, xup;
         Stat_t sum = 0;
         Stat_t mean = 0;
         Stat_t sigma = 0;
         xlow= (Axis_t)wdwf->GetX1();
         xup = (Axis_t)wdwf->GetX2();
         sum = Content(fSelHist,xlow, xup, &mean, &sigma);
         cout <<setw(9)<< wdwf->GetName()
              <<setw(9)<< xlow <<setw(9) << xup <<setw(9)
              << sum <<setw(9)<< mean <<setw(9) << sigma << endl;
         TMrbWindow *wdw = (TMrbWindow *)wdwf;
         wdw->SetTextSize(0.03);
         wdw->SetTextColor(1);
         wdw->SetLineColor(1);
         wdw->SetLineWidth(3);
         wdw->Draw();
         fCanvas->Update();
      }
   } else {
     WarnBox("No Windows set");
   }
}
//______________________________________________________________________________________

void  FitHist::WriteOutWindows(){
   Int_t nval = CheckList(fActiveWindows);
   if(nval>0){
      if (nval == 1) {
         TMrbWindow* wdw = (TMrbWindow*)fActiveWindows->At(0);
         Save2FileDialog sfd(wdw);
      } else {
         Save2FileDialog sfd(fActiveWindows);
      }
   } else {
     WarnBox("No Windows active");
   }
}
//______________________________________________________________________________________

void FitHist::ListWindows(){
   if(CheckList(fAllWindows) < 0){WarnBox("No windows active");return;}
   CheckList(fActiveWindows);
   Int_t xp, yp;
   xp =  fCanvas->GetWindowTopX() +  ((Int_t)(fCanvas->GetWw()) / 2);
   yp =  fCanvas->GetWindowTopY() +  ((Int_t)(fCanvas->GetWh()) / 2);
   TIter next(fAllWindows);
   TMrbWindow* wdw;
   while( (wdw=(TMrbWindow*)next()) ){
      TString nam =  wdw->GetName();
      TString cmd = this->GetName();
      TString sel = cmd;
      cmd = cmd + "->EditWindow(\"" + nam.Data() + "\")";
      TString tit;
      sel = sel + "->AddWindow(\"" + nam.Data() + "\")";
      if(fActiveWindows->Contains(wdw)) sel = sel + " YES";
      fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel));
   }
   if(fCmdLine->IsEmpty()){
      WarnBox("No windows active");
      return;
   }
   Int_t xwidth = 0;
   xwidth = WindowSizeDialog::fWinwidx_hlist;
   fCutPanel = CommandPanel("Windows", fCmdLine, xp, yp, NULL, xwidth);
   fCmdLine->Delete();
}
//______________________________________________________________________________________

void FitHist::AddWindow(const char * name, const char * bp){
   TMrbWindow * wdw;
   wdw = (TMrbWindow *)fAllWindows->FindObject(name);
   TButton * b;
   b = (TButton *)strtoul(bp, 0, 16);
   if(b->TestBit(kSelected)){
      fActiveWindows->Remove(wdw);
      b->SetFillColor(16);
      b->ResetBit(kSelected);
   } else {
      fActiveWindows->Add(wdw);
      b->SetFillColor(3);
      b->SetBit(kSelected);
   }
}
//______________________________________________________________________________________

void FitHist::EditWindow(const char * name, const char */*bp*/)
{
   if(CheckList(fAllWindows)<= 0) return;
   TObject* obj=fAllWindows->FindObject((char *)name);
   if(obj){
      TMrbWindow* wdw = (TMrbWindow*)obj;
      TArrayD xyvals(4);
//      Double_t *xyvals = new Double_t[4];
      TOrdCollection *row_lab = new TOrdCollection();

      row_lab->Add(new TObjString("x,y left"));
      row_lab->Add(new TObjString("x,y right"));
      xyvals[0] = wdw->GetX1();
      xyvals[1] = wdw->GetX2();
      xyvals[2] = wdw->GetY1();
      xyvals[3] = wdw->GetY2();
// show values to caller and let edit
      TString title="Window: ";
      title += wdw->GetName();
      Int_t ret = 0,  itemwidth=120, precission = 5;
      TGMrbTableOfDoubles(mycanvas, &ret,title.Data(),itemwidth, 2, 2, xyvals,
      precission, NULL,row_lab);
      if(ret >= 0){
         wdw->SetX1(xyvals[0]);
         wdw->SetX2(xyvals[1]);
         wdw->SetY1(xyvals[2]);
         wdw->SetY2(xyvals[3]);
			fCanvas->Modified();
			fCanvas->Update();
      }
//      if (xyvals) delete [] xyvals;
      if(row_lab){ row_lab->Delete(); delete row_lab;}
   }
}
//______________________________________________________________________________________

void FitHist::UpdateCut(){
//   ClearCut();
   CheckList(fActiveCuts, "TMrbWindow2D");
   CheckList(fAllCuts, "TMrbWindow2D");
   TCutG *cut = (TCutG*)gROOT->FindObject("CUTG");

   if(cut)
   {
      if (fCutname.Length() < 1) {
         fCutname = fHname;
         fCutNumber++;
         fCutname.Prepend(Form("%d_", fCutNumber));
         fCutname.Prepend("Cut");
      }
      cout << "Create cut: " << fCutname.Data() << endl;
      TMrbWindow2D * wdw2d =
      new TMrbWindow2D(fCutname.Data(), cut->GetN(), cut->GetX(), cut->GetY());
		if ( strlen(fSelHist->GetXaxis()->GetTitle()) > 0 )
			wdw2d->SetVarX(fSelHist->GetXaxis()->GetTitle());
		else 
			wdw2d->SetVarX("X");
		if ( strlen(fSelHist->GetYaxis()->GetTitle()) > 0 )
			wdw2d->SetVarY(fSelHist->GetYaxis()->GetTitle());
		else
			wdw2d->SetVarY("Y");
      fActiveCuts->Add(wdw2d);
      fAllCuts->Add(wdw2d);
      delete cut;
      DrawCut();
      fCutname = "";
   }
   CloseCuts();
}
//______________________________________________________________________________________

void  FitHist::WriteOutCut(){
   UpdateCut();
   if(Ncuts() > 0){
      if (Ncuts() == 1) {
         TMrbWindow* wdw = (TMrbWindow*)fActiveCuts->At(0);
         Save2FileDialog sfd(wdw, NULL, mycanvas);
      } else {
         Save2FileDialog sfd(fActiveCuts, NULL, mycanvas);
      }
   } else {
     WarnBox("No cuts active");
   }
}
//______________________________________________________________________________________

Bool_t FitHist::UseCut(TMrbWindow2D * cut){
//   TObjString newcut = (const char*)(name);
   UpdateCut();
   if(!Ncuts())return kFALSE;
   Int_t pos = fActiveCuts->IndexOf(cut);
   if(pos >= 0)return kTRUE;
   else        return kFALSE;
}
//______________________________________________________________________________________

void FitHist::AddCutsToHist(){
   Int_t nval = CheckList(fActiveCuts, "TMrbWindow2D");
   if(nval>0){
      TMrbWindow2D *wdw;
      TIter next(fActiveCuts);
      while( (wdw = (TMrbWindow2D*)next()) ){
         TMrbWindow2D * wdwcp= new TMrbWindow2D(*wdw);
         TIter nextobj(fCanvas->GetListOfPrimitives());
         TObject *obj;
         while( (obj=nextobj()) ){
//            cout << obj->GetName() << endl;
            if(obj->InheritsFrom(TText::Class()) &&
               !strcmp(obj->GetName(), wdw->GetName())){
               TText*t = (TText*)obj;
               wdwcp->SetXtext(t->GetX() - wdw->GetX()[0]);
               wdwcp->SetYtext(t->GetY() - wdw->GetY()[0]);
            }
         }
         fSelHist->GetListOfFunctions()->Add(wdwcp);
         TMrbWindow2D * w = (TMrbWindow2D*)wdwcp;
         w->SetParent(fSelHist);
      }
   }
}
//______________________________________________________________________________________

void FitHist::InitCut(){
   fCanvas->cd();
   UpdateCut();
//   CheckList(fActiveCuts);
   TString cname = fHname;
	Int_t p = cname.Index(";");
	if (p>0) cname.Resize(p);
   fCutNumber++;
   cname.Prepend(Form("%d_", fCutNumber));
   cname.Prepend("Cut");
   Bool_t ok;
   fCutname = GetString("Cut will be saved with name",cname.Data(), &ok, mycanvas);
   if(!ok)  return;
   TMrbWindow2D * wdw_old = (TMrbWindow2D *)fAllCuts->FindObject(fCutname.Data());
   if(wdw_old) {
      if (QuestionBox("Window with this name\n\
already exists, delete it? ", mycanvas)) {
         fAllCuts->Remove(wdw_old);
         fActiveCuts->Remove(wdw_old);
         delete wdw_old;
      } else {
         return;
      }
   }
   gROOT->SetEditorMode("CutG");
}
//______________________________________________________________________________________

Bool_t FitHist::InsideCut(Float_t x, Float_t y){
   if ((Ncuts() == 0)) return kTRUE;
   TMrbWindow2D *cut;
   TIter next(fActiveCuts);
// if inside selected: return true if point is within any cut
// if outside:         point must be outside all cuts
   Int_t atleastone=kFALSE;
   Bool_t result = !fSelInside;
   while( (cut= (TMrbWindow2D*)next()) ){
      if(cut->TestBit(TObject::kNotDeleted)){
         Bool_t temp = cut->IsInside(x,y);
         if(fSelInside)result = result || temp;
         else          result = result && !temp;
         atleastone = kTRUE;
      }
   }
   if(atleastone) return result;
   else           return kTRUE;
}
//______________________________________________________________________________________

void FitHist::DrawCutName(){
//   UpdateCut();
   if(CheckList(fActiveCuts, "TMrbWindow2D") <=0) return;
   TMrbWindow2D * wdw;
   TIter next(fActiveCuts);
   while( (wdw= (TMrbWindow2D*)next()) ){
      TIter nextobj(fCanvas->GetListOfPrimitives());
      TObject *obj;
      while( (obj=nextobj()) ){
//         cout << obj->GetName() << endl;
         if(obj->InheritsFrom(TText::Class()) &&
            !strcmp(obj->GetName(), wdw->GetName())){
            fCanvas->GetListOfPrimitives()->Remove(obj);
            delete obj;
         }
      }
      TText *t = new TText
      (wdw->GetX()[0]+wdw->GetXtext(),wdw->GetY()[0]+wdw->GetYtext(),
      wdw->GetName());
      t->SetName(wdw->GetName());
      t->SetTextSize(0.02);
      fCanvas->cd();
      t->Draw();
   }
   fCanvas->Update();
}
//_____________________________________________________________________________________

void FitHist::RemoveAllCuts(){
   UpdateCut();
//   gObjectTable->Print();
   fActiveCuts->Clear();
   fAllCuts->SetOwner();
   fAllCuts->Clear();
   if (gHpr) 
     gHpr->HandleRemoveAllCuts();
   fCanvas->Modified();
   fCanvas->Update();
   
//   gObjectTable->Print();
}
//_____________________________________________________________________________________

void FitHist::ClearCut(){
   UpdateCut();
//   gObjectTable->Print();
   fActiveCuts->Clear();
//   gObjectTable->Print();
}
//______________________________________________________________________________________

void FitHist::CloseCuts(){
   if(CheckList(fActiveCuts) <=0) return;
   TMrbWindow2D *cut;
   TIter next(fActiveCuts);
   while( (cut= (TMrbWindow2D*)next()) ){
      Int_t last = cut->GetN()-1;
      cut->GetX()[last] = cut->GetX()[0];
      cut->GetY()[last] = cut->GetY()[0];
   }
}
//______________________________________________________________________________________

void FitHist::ListCuts(){
   Int_t xp, yp;
   xp =  fCanvas->GetWindowTopX() +  (Int_t)(fCanvas->GetWw() >> 1);
   yp =  fCanvas->GetWindowTopY() +  (Int_t)(fCanvas->GetWh() >> 1);
   UpdateCut();
//   CheckList(fActiveCuts);
   TIter next(fAllCuts);
   while(TMrbWindow2D* cut = (TMrbWindow2D*)next()){
      TString cmd = this->GetName();
      cmd = cmd + "->EditCut(\"" + cut->GetName() + "\")";
      TString nam =  cut->GetName();
      TString tit;
      TString sel = this->GetName();
      sel = sel + "->AddCut(\"" + cut->GetName() + "\")";
      if(UseCut(cut)) sel = sel + " YES";
      fCmdLine->Add(new CmdListEntry(cmd, nam, tit, sel));
   }
   if(fCmdLine->IsEmpty()){
      WarnBox("No cuts active");
      return;
   }
   Int_t xwidth = 0;
   xwidth = WindowSizeDialog::fWinwidx_hlist;
   fCutPanel= CommandPanel("Cuts", fCmdLine, xp, yp, NULL, xwidth);
   fCmdLine->Delete();
}
//______________________________________________________________________________________

void FitHist::AddCut(const char * name, const char * bp){
   UpdateCut();
   TMrbWindow2D* cut = (TMrbWindow2D*)fAllCuts->FindObject((char *)name);
   if(cut && bp){
       TButton * b;
       b = (TButton *)strtoul(bp, 0, 16);
       if(b->TestBit(kSelected)){
          fActiveCuts->Remove(cut);
          b->SetFillColor(16);
          b->ResetBit(kSelected);
       } else {
          if(!UseCut(cut)) fActiveCuts->Add(cut);
          b->SetFillColor(3);
          b->SetBit(kSelected);
       }
   } else { cout << "Cut not found " << name << endl;}
}
//______________________________________________________________________________________

void FitHist::EditCut(const char * name, const char */*bp*/)
{
   UpdateCut();
   TMrbWindow2D* cut = (TMrbWindow2D*)fActiveCuts->FindObject((char *)name);
   if(cut){
//      PrintOneCut(cut);
      int n = cut->GetN();
//      Double_t *xyvals = new Double_t[2*n + 2];
      TArrayD xyvals(2*n + 2);
      TOrdCollection *row_lab = new TOrdCollection();

      for(int i=0; i<n; i++)  {
//         float x,y;
        Double_t x,y;
        cut->GetPoint(i,x,y);
         xyvals[i] = x;
         xyvals[i+n+1] = y;
         row_lab->Add(new TObjString("x,y value"));
        cout << "x: " << x << " \ty: " << y << endl;
//        cout << "x: " << xyvals[i] << " \ty: " << xyvals[i+n+1] << endl;
      }
      xyvals[n] = 0;
      xyvals[2*n+1] = 0;
      row_lab->Add(new TObjString("x,y shift"));
// show values to caller and let edit
      TString title("Cut: ");
      title += cut->GetName();
      Int_t ret = 0, itemwidth=240, precission = 5;
      TGMrbTableOfDoubles(mycanvas, &ret, title.Data(), itemwidth, 2, n+1,
                         xyvals, precission,NULL,row_lab);
      cout << ret << endl;
      if(ret >= 0){
         for(int i=0; i<n; i++)  {
            cut->SetPoint(i,xyvals[i] + xyvals[n],xyvals[i+n+1] + xyvals[2*n+1]);
         }
//         make sure contour is closed
         xyvals[n-1]   =  xyvals[0];
         xyvals[2*n] =  xyvals[n+1];
			fCanvas->Modified();
			fCanvas->Update();
      }
//      if (xyvals) delete [] xyvals;
      if(row_lab){ row_lab->Delete(); delete row_lab;}
   }
}
//------------------------------------------------------

void FitHist::PrintOneCut(TMrbWindow2D * cut){
   cout << cut->GetName() << ":"<<endl;
   int n = cut->GetN();
   for(int i=0; i<n; i++)  {
      Double_t x,y;
      cut->GetPoint(i,x,y);
      cout << "x: " << x << " \ty: " << y << endl;
   }
}
//______________________________________________________________________________________

Float_t FitHist::DrawCut(){

   UpdateCut();
   if(Ncuts() <= 0){
//       WarnBox("No cuts active");
      return 0;
   }
   if(!is2dim(fSelHist)) return 0;
   cout << "--- Entries inside cut(s) -----------" << endl;
   fCanvas->cd();
   TMrbWindow2D *cut;
   TIter next(fActiveCuts);
   TAxis* xaxis = fSelHist->GetXaxis();
   TAxis* yaxis = fSelHist->GetYaxis();
   Float_t ent = 0;

   while( (cut= (TMrbWindow2D*)next()) ){
      cut->Draw();
      ent = 0;
      for(int i = 1; i <= fSelHist->GetNbinsX(); i++){
         for(int j = 1; j <= fSelHist->GetNbinsY(); j++){
            Axis_t xcent = xaxis->GetBinCenter(i);
            Axis_t ycent = yaxis->GetBinCenter(j);
            if(cut->IsInside((float)xcent,(float)ycent))
            ent += fSelHist->GetBinContent(i, j);
         }
      }
      cout << cut->GetName() << ": Content:" << ent << endl;
   }
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
   return ent;

}
//______________________________________________________________________________________

void FitHist::MarksToCut(){
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL ||  fMarkers->GetEntries() <= 0) return;
   Int_t nmarks = fMarkers->GetEntries();
   if(nmarks < 2){
      WarnBox("Need at least 2 marks");
      return;
   }
   FhMarker *mark;
   TIter next(fMarkers);
   Int_t nval;
   if(nmarks == 2)nval = 5;
   else           nval = nmarks+1;
   TArrayD x(nval);            // need this for 2.25-03
   TArrayD y(nval);
//   TArrayF x(nval);
//   TArrayF y(nval);
   for(Int_t i=0; i<nmarks; i++){
      mark = (FhMarker*)next();
      x[i] = mark->GetX();
      y[i] = mark->GetY();
   }
   x[nval-1] = x[0];
   y[nval-1] = y[0];

   if(nmarks == 2){
      x[3] = x[0];
      y[3] = y[1];
      x[2] = x[1];
      y[2] = y[3];
      x[1] = x[2];
      y[1] = y[0];
   }
   TString cname = fHname;
   fCutNumber++;
   cname.Prepend(Form("%d_", fCutNumber));
   cname.Prepend("Cut");
   Bool_t ok;
   cname = GetString("Cut will be saved with name",cname.Data(), &ok, mycanvas);
   if(!ok)  return;
   CheckList(fActiveCuts);
   CheckList(fAllCuts);
   if(fAllCuts->FindObject(cname.Data())) {
      WarnBox("Cut already exists");
      return;
   }
   TMrbWindow2D * wdw = new TMrbWindow2D(cname.Data(), nval, &x[0], &y[0]);
   wdw->Draw();
   fCanvas->Update();
   fActiveCuts->Add(wdw);
   fAllCuts->Add(wdw);
   ClearMarks();
}

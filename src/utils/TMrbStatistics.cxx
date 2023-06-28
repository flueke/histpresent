//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbStatistics.cxx
// Purpose:        MARaBOU utilities: histogram statistics
// Description:    Implements class methods to handle histogram statistics & contents
// Keywords:
// Author:         O. Schaile
// Mailto:         <a href=mailto:otto.schaile@physik.uni-muenchen.de>O. Schaile</a>
// Revision:       $Id: TMrbStatistics.cxx,v 1.8 2006-10-17 06:59:29 Otto.Schaile Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbStatistics.h"

Int_t check_list_of_functions(TH1 * h)
{
   Int_t n_removed = 0;
   if (!h) return n_removed;
   TObject * obj;
   TList * lof = h->GetListOfFunctions();
repeat:
   Int_t nent = lof->GetSize();
   if (nent < 2) return n_removed;
   for (Int_t i = 0; i < nent - 1; i++) {
      obj = lof->At(i);
      for (Int_t k = i+1; k < nent; k++) {
         if (obj == lof->At(k)) {
            cout << "Removing duplicate: " << obj->GetName()
                 << " from: " << h->GetName() << endl;
            lof->Remove(obj);
            n_removed++;
            goto repeat;
         }
      }
   }
   return n_removed;
}
      

ClassImp(TMrbStatistics)
/////////////////////////////////////////////////////////////////////////////////////////
//  TMrbStatistics                                                                     //
//  This class is useful if many histograms reside in a memory mapped file to speed up //
//  the access when displaying contents and statistics only. Otherwise the complete    //
//  histograms would have to be streamed in to get e.g. the number of entries.         //
/////////////////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________________

TMrbStatistics::TMrbStatistics(const char * title)
   :TNamed("TMrbStatistics",title){
// TMrbStatistics normal constructor
   fStatEntries = new TList(this);
};
//____________________________________________________________________________

TMrbStatistics::~TMrbStatistics(){
// TMrbStatistics  destructor
   if(fStatEntries){ 
      fStatEntries->Delete(); 
      delete fStatEntries;
   }
};
//____________________________________________________________________________

Int_t TMrbStatistics::Init(){
//
//  Look for all histograms in current directory and make an entry in statistics list
//
   Int_t nhists = 0;
   TIter next(gDirectory->GetList());
   TH1 *h;
   while( (h = (TH1*)next()) ){
      if(h->InheritsFrom(TH1::Class()))
      fStatEntries->Add(new TMrbStatEntry(h, h->GetName(), h->GetTitle())); 
      nhists++;
   }
   return nhists;
}
//____________________________________________________________________________

Int_t TMrbStatistics::Update(){
//
//  Walk through list of  histograms and update statistics entries
//
   Int_t nhists = 0;
   TIter next(fStatEntries);
   TH1* h;
   TMrbStatEntry *ent;
   while( (ent = (TMrbStatEntry*)next()) ){
      h = ent->GetHist();
      if(!h) {cout << "TMrbStatistics: Null pointer " << endl; break;}
      if(h->InheritsFrom(TH2::Class())){
         ent->Set(h->GetEntries(), h->GetSumOfWeights()); 
//         cout << "2 dim" << endl;
      } else {
         ent->Set(h->GetEntries(), h->GetSumOfWeights(),
                  h->GetMean(), h->GetRMS()); 
//         cout << "1 dim" << endl;
      }
      nhists++;
   }
   return nhists;
}
//____________________________________________________________________________

Int_t TMrbStatistics::Fill(){
//
//  Look for all histograms in current directory and make an entry in statistics list
//
   Int_t nhists = 0;
   TIter next(gROOT->GetList());
   TMrbStatEntry * ent;
   TH1 *h;
   TObject* obj;
   while( (obj= (TObject*)next()) ){
      if(obj->InheritsFrom(TH1::Class())){
         h = (TH1*)obj;
         ent = new TMrbStatEntry(h, h->GetName(), h->GetTitle());
         fStatEntries->Add(ent); 
         if(h->InheritsFrom(TH2::Class())){
            ent->Set(h->GetEntries(), h->GetSumOfWeights()); 
         } else {
            ent->Set(h->GetEntries(), h->GetSumOfWeights(),
                     h->GetMean(), h->GetRMS()); 
         }
         nhists++;
      }
   }
   return nhists;
}
//____________________________________________________________________________

Int_t TMrbStatistics::Fill(TDirectory * file){
//
//  Look for all histograms in file and make an entry in statistics list
//
   Int_t nhists = 0;
   TIter next(file->GetListOfKeys());
   TMrbStatEntry * ent;
   TH1 *h;
   TKey* key;
   TObject* obj;
   while( (key = (TKey*)next()) ){
      if(!strncmp(key->GetClassName(),"TH1",3) ||
         !strncmp(key->GetClassName(),"TProfile",8) ||
         !strncmp(key->GetClassName(),"TH2",3) ||
         !strncmp(key->GetClassName(),"TH3",3)){
         obj = (TObject*)key->ReadObj(); 
//      if(obj->InheritsFrom(TH1::Class())){
         h = (TH1*)obj;
         check_list_of_functions(h);
         ent = new TMrbStatEntry(h, h->GetName(), h->GetTitle());
         fStatEntries->Add(ent); 
         ent->SetCycle(key->GetCycle());
         if(h->InheritsFrom(TH2::Class())){ 
            ent->Set(h->GetEntries(), h->GetSumOfWeights()); 
         } else {
            ent->Set(h->GetEntries(), h->GetSumOfWeights(),
                     h->GetMean(), h->GetRMS()); 
         }
         nhists++;
         delete obj;
      }
   }
   return nhists;
}
//____________________________________________________________________________

void TMrbStatistics::Print(ostream & output) const{
//
   TString sep("------------------------------------------------------------------");
   output << sep.Data() << endl;
   output << GetName() << ": " << GetTitle() << endl;
   output << sep.Data() << endl;
     output << setw(25)<<setiosflags(ios::left)<< "Name" 
            << resetiosflags(ios::left)<< setw(10) <<  "Entries" 
           << setw(10) << "Sumofw"
           << setw(10)<<  "Mean" <<setw(10) << "Sigma" 
           << setw(10) << "NbinsX" << setw(10) << "NbinsY" <<  setw(10) << "Size[bytes]"<< endl;
   output << sep.Data() << endl;
//
//     fStatEntries->Print();
   Int_t size = 0;
   TIter next(fStatEntries);
   TMrbStatEntry *ent;
   while( (ent = (TMrbStatEntry*)next()) ){
      size += ent->Print(output);
   }
   output << endl << "Total size used: " << (size + 999999) / 1000000 << " Mbytes" << endl; 
}

ClassImp(TMrbStatEntry)
/////////////////////////////////////////////////////////////////////////////////////////
//  TMrbStatEntry                                                                      //
//  This class is used by class TMrbStatistics                                         //
/////////////////////////////////////////////////////////////////////////////////////////


TMrbStatEntry::TMrbStatEntry(TH1* hist, const char * name, const char * title)
               :TNamed(name, title){
// TMrbStatEntry normal constructor
   fHist = hist;
   fNbinsX = fNbinsY = fNbinsZ = 0;
   if      (hist->InheritsFrom(TH3::Class())) {
     fDim = 3;
   } else if (hist->InheritsFrom(TH2::Class())) {
     fDim = 2; 
   } else if (hist->InheritsFrom(TH1::Class())) {
      fDim = 1;
   } else {cout << "ctor TMrbStatEntry illegal hist dim " << endl; fDim=0;}
   fNbinsX = hist->GetNbinsX();
   if (fDim > 1) fNbinsY = hist->GetNbinsY();
   if (fDim > 2) fNbinsZ = hist->GetNbinsZ();
   TString cn = hist->ClassName();
   if      (cn[3] == 'C') fBytesPerCell = 1;
   else if (cn[3] == 'S') fBytesPerCell = 2; 
   else if (cn[3] == 'F') fBytesPerCell = 4; 
   else if (cn[3] == 'D') fBytesPerCell = 8; 
   fNofUpdates =0;
   fEntries = 0;
   fSumofw  = 0;
   fMean    = 0;
   fSigma   = 0;
   fCycle   = 0;
}
//____________________________________________________________________________

TMrbStatEntry::TMrbStatEntry(){
// TMrbStatEntry default constructor 
   fHist=0; fNofUpdates=0, fCycle=0;
};

void TMrbStatEntry::Set(const Stat_t ent, const Stat_t sofw, const Stat_t mx,
         const Stat_t sx){
    fEntries = ent; 
    fSumofw  = sofw;
    fMean   = mx;
    fSigma  = sx;
    fNofUpdates ++;
};
//____________________________________________________________________________
void TMrbStatEntry::Set(const Stat_t ent, const Stat_t sofw){
    fEntries = ent; 
    fSumofw  = sofw;
    fNofUpdates ++;
};
//____________________________________________________________________________
Int_t TMrbStatEntry::Get(Stat_t *ent, Stat_t *sofw, Stat_t *mx, Stat_t *sx) const {
    *ent  = fEntries;
    *sofw = fSumofw ;
    *mx   = fMean  ;
    *sx   = fSigma ;
    return fNofUpdates;
};
//____________________________________________________________________________
Int_t TMrbStatEntry::Get(Stat_t *ent, Stat_t *sofw) const {
    *ent  = fEntries;
    *sofw = fSumofw ;
    return fNofUpdates;
};
//____________________________________________________________________________

Int_t TMrbStatEntry::Print(ostream & output) const {
   Int_t size = 0;
   if(fDim == 1) {
        size = (fNbinsX + 2) * fBytesPerCell;
      output << setw(25)<<setiosflags(ios::left)<< GetName() 
        << resetiosflags(ios::left)
        << setw(10)<< fEntries << " "
        << setw(9) << fSumofw << " " << setw(9)<< fMean 
        << setw(10) << fSigma << setw(9) << fNbinsX << setw(10)<< 0 << setw(10)<< size << endl;
   } else {
      size = (fNbinsX + 2) * (fNbinsY + 2) * fBytesPerCell;;
      output << setw(25)<<setiosflags(ios::left)<< GetName() 
        << resetiosflags(ios::left)
        << setw(10)<<  fEntries << " " << setw(9) << fSumofw
        << setw(10)<< " " << setw(10)<< " " 
        << setw(10) << fNbinsX << setw(10) << fNbinsY << setw(10)<< size << endl;
   }
   return size;
}
//____________________________________________________________________________

// #include "TMrbStatistics.h"

ClassImp(TMrbStatCheck)
/////////////////////////////////////////////////////////////////////////////////////////
//  TMrbStatCheck                                                                      //
//  This class serves to implement check on histgram parameters                        //
/////////////////////////////////////////////////////////////////////////////////////////


TMrbStatCheck::TMrbStatCheck(const char * name, const char * title)
               :TNamed(name, title){
// TMrbStatCheck normal constructor
   fAnyCheck = kFALSE;
   fEntriesLow = 0;
   fSumofwLow  = 0;
   fMeanLow   = 0;
   fSigmaLow  = 0;
   fEntriesUp = 0;
   fSumofwUp  = 0;
   fMeanUp   = 0;
   fSigmaUp  = 0;
};
//____________________________________________________________________________

TMrbStatCheck::TMrbStatCheck(){
// TMrbStatCheck default constructor 
};
//____________________________________________________________________________

Bool_t TMrbStatCheck::InsideMean(Stat_t val){
   if(fMeanUp  != 0 && val > fMeanUp) return kFALSE; 
   if(fMeanLow != 0 && val < fMeanUp) return kFALSE; 
   return kTRUE;
}
//____________________________________________________________________________

Bool_t TMrbStatCheck::InsideSigma(Stat_t val){
   if(fSigmaUp  != 0 && val > fSigmaUp) return kFALSE; 
   if(fSigmaLow != 0 && val < fSigmaLow) return kFALSE; 
   return kTRUE;
}

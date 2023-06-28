#include "TPad.h"
#include "TSystem.h"
#include <fstream>

#include "HprImage.h"

ClassImp(HprImage)
//_________________________________________________________________________
//
// This class wraps a TImage object. It stores
// the content of the image file in a byte buffer
// The image itself is only painted, i.e. not added
// to the ListOfPrimitives. In this way the image file
// itself is stored on file not its pixmap
// The pixmap may be saved via 
//_________________________________________________________________________
HprImage::HprImage(const Char_t * fname, TPad * pad) :
          TNamed(gSystem->BaseName(fname), ""), fPad(pad){

   Long_t id, size = 0, flags, modtime;
//   cout << "norm ctor HprImage: " << fname << endl;
   gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);
   fIsPainted = kFALSE;
   if(size <= 0){
//      cout << "File: " << fname << " does not exist of empty" << endl;
      fIsGood = kFALSE;
      fBuffer = NULL;
      fImage = NULL;
   } else {
      cout << "ctor HprImage this: " << this << "Pad: " << pad << endl;
      fImage = TImage::Open(fname);
      if (!fImage) {
         cout << "Could not create an image... exit" << endl;
         return;
      }
      fBuffer = new TArrayC(size);
      ifstream inf(fname);
      inf.read(fBuffer->fArray,size);
      fIsGood = kTRUE;
      gPad->Modified(kTRUE);
   }
   gROOT->GetListOfCleanups()->Add(this);
};
//______________________________________________

HprImage::~HprImage()
{
//   cout << "HprImage:: dtor" << endl;
   fPad = 0;
   if(fBuffer)delete fBuffer;
   fBuffer = 0;
};
//______________________________________________

void HprImage:: RecursiveRemove(TObject * obj) {
//   cout << "HprImage::RecursiveRemove: "
//        << this << " " << fPad << " " << fImage << " " << obj <<  endl;
   if (obj == fImage) {
      gROOT->GetListOfCleanups()->Remove(this);
      if (fPad) {
        fPad->GetListOfPrimitives()->Remove(this);
      }
      delete this;
   }
};
//_________________________________________________________________________

Int_t HprImage::FileExists()
{
   if ( gSystem->AccessPathName( GetName() ) )
      return -1;
   Long_t id, size, flags, modtime;
   gSystem->GetPathInfo(GetName(), &id, &size, &flags, &modtime);
   if ( fBuffer->GetSize() != size ) {
      cout << "File exists, but size differ, file: " << size
           << " buffer " << fBuffer->GetSize()  << endl;
      return 0;
   } else {
      return 1;
   }
}
//_________________________________________________________________________

void HprImage::Print(Option_t * /*opt*/) const
{
   cout << "HprImage:: " << GetName() << endl;
}
//_________________________________________________________________________
//void  HprImage::Draw(Option_t * opt)const
//{
//  if (opt);
//  cout << "HprImage::Draw() " << endl;
//}
//_________________________________________________________________________

Int_t HprImage::ToFile(const Char_t * fname)
{
// Write buffer contents to a file with name fname
// for later reading by TImage::Open(
// cout << "HprImage::ToFile(): " << fname << endl;
   if(!fBuffer){
      cerr << "Buffer is empty"  << endl;
      return -1;
   }

   ofstream of(fname);
   if(of.fail()){
      cerr << "Cant open " << fname << endl;
      return -2;
   }
   of.write(fBuffer->fArray, fBuffer->GetSize());
   of.close();
   return fBuffer->GetSize();
}
//_________________________________________________________________________

void HprImage::Paint(Option_t * /*opt*/)
{
   if ( GetVisibility() == 0 )
      return;
//   cout << "HprImage::Paint(): "<< this  << " Pad: " << gPad  << endl;
   TList * lop = gPad->GetListOfPrimitives();
   TObject *obj;
   TIter next_img(lop);
   TObject *hpri = NULL;
   TObject *aimg = NULL;
//   Bool_t found = kFALSE;
   while ( (obj = next_img()) ) {
      if (obj->InheritsFrom("TASImage")) {
         cout << "TASImage GetHeight(): " << ((TImage*)obj)->GetHeight() << endl;
         if (((TImage*)obj)->GetHeight() > 5) aimg = obj;
      }
      if (obj->InheritsFrom("HprImage")) hpri = obj;
   }
   if ( aimg ) {
/*
      if (aimg && aimg != lop->First()) {
         cout << "HprImage::Paint:  Remove aimg: " << aimg << endl;
         lop->Remove(aimg);
         lop->AddFirst(aimg,"X");
      }
*/
      if (hpri && hpri != lop->First()) {
         cout << "HprImage::Paint: move to top " << hpri << endl;
         lop->Remove(hpri);
         lop->AddFirst(hpri,"X");
         gPad->Modified(kTRUE);
         gPad->Update();
      }
      return;
   }
//   cout << "HprImage::Paint(): " << name << endl;
//   Dump();
//   if (fImage) fImage->Dump();
   TObjOptLink *lnk = (TObjOptLink*)lop->FirstLink();
//   cout << "HprImage::Paint(): bef Remove TASImage" << endl;
   while (lnk) {
      obj = lnk->GetObject();
      if (obj->InheritsFrom("TASImage")) {
         lop->Remove(lnk);
         cout << "Remove TASImage" << endl;
      }
      lnk = (TObjOptLink*)lnk->Next();
   }
//   cout << "HprImage::Paint(): after Remove TASImage" << endl;

//   lnk = (TObjOptLink*)lop->FirstLink();
//   while (lnk) {
//      obj = lnk->GetObject();
//      if (obj->InheritsFrom("TFrame")) {

//      }
//      lnk = (TObjOptLink*)lnk->Next();
//   }

   if (!fImage || !fIsGood) {
      TString name(GetName());
// if file exits and has correct length skip reading
      Int_t fex = FileExists();
      if ( fex <= 0) {
         if (fex == 0) name.Prepend("temp_"); // exists, dont overwrite
         ToFile(name.Data());
      }
//       cout << "HprImage::ToFile " << endl;

      fImage = TImage::Open(name.Data());
      if (!fImage) {
         cout << "Could not create an image... exit" << endl;
         return;
      }
   }
   fImage->SetConstRatio(kTRUE);
   fImage->SetEditable(kTRUE);
   fImage->SetImageQuality(TAttImage::kImgBest);
   fImage->Paint("X");
//   lop->ls();
// make sure image is drawn first
   if (fImage != lop->First()) {
//      lop->Remove((TObject*)fImage);
//      lop->AddFirst(fImage,"X");
//       cout << "HprImage:: AddFirst" << endl;
   } else {
       cout << "HprImage:: is 1. "<<fImage<< endl;
   }
//   lop->ls();
   gPad->Modified(kTRUE);
   fIsPainted = kTRUE;
}

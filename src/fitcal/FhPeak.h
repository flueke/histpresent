#ifndef HPR_FHPEAK
#define HPR_FHPEAK
#include <iostream>
#include <iomanip>
#include "TObject.h"
#include "TList.h"

namespace std {} using namespace std;

class FhPeak : public TObject {

private:
   TString fGaugeName;
   Double_t fNominalEnergy;
   Double_t fCalibratedEnergy;
   Double_t fEstimate;
   Double_t fContent;
   Double_t fMean;
   Double_t fWidth;
   Double_t fNominalEnergyError;
   Double_t fContentError;
   Double_t fMeanError;
   Double_t fWidthError;
   Double_t fChi2oNdf;
   Double_t fTailContent;
   Double_t fTailWidth;
   Double_t fIntensity;
   Double_t fRelEfficiency;
   Bool_t   fUsed;
public:
   FhPeak(){};
   FhPeak(Double_t pos);

   ~FhPeak() {};
   const Char_t * GetGaugeName() const {return fGaugeName.Data();};
   Double_t GetNominalEnergy() const {return fNominalEnergy;};
   Double_t GetCalibratedEnergy() const {return fCalibratedEnergy;};
   Double_t GetContent()             {return fContent;};
   Double_t GetMean() const          {return fMean;};
   Double_t GetWidth() const         {return fWidth;};
   Double_t GetNominalEnergyError() const {return fNominalEnergyError;};
   Double_t GetContentError() const  {return fContentError;};
   Double_t GetMeanError() const     {return fMeanError;};
   Double_t GetWidthError() const    {return fWidthError;};
   Double_t GetChi2oNdf() const      {return fChi2oNdf;};
   Double_t GetTailContent() const   {return fTailContent;};
   Double_t GetTailWidth() const     {return fTailWidth;};
   Double_t GetIntensity() const     {return fIntensity;};
   Double_t GetRelEfficiency() const {return fRelEfficiency;};
   Bool_t   GetUsed() const          {return fUsed;};


   void SetGaugeName(const Char_t * val){fGaugeName=val;};
   void SetNominalEnergy(Double_t val){fNominalEnergy=val;};
   void SetCalibratedEnergy(Double_t val){fCalibratedEnergy=val;};
   void SetContent(Double_t val)      {fContent=val;};
   void SetMean(Double_t val)         {fMean=val;};
   void SetWidth(Double_t val)        {fWidth=val;};
   void SetNominalEnergyError(Double_t val)       {fNominalEnergyError=val;};
   void SetContentError(Double_t val) {fContentError=val;};
   void SetMeanError(Double_t val)    {fMeanError=val;};
   void SetWidthError(Double_t val)   {fWidthError=val;};
   void SetChi2oNdf(Double_t val)     {fChi2oNdf=val;};
   void SetTailContent(Double_t val)  {fTailContent=val;};
   void SetTailWidth(Double_t val)    {fTailWidth=val;};
   void SetIntensity(Double_t val)    {fIntensity=val;};
   void SetRelEfficiency(Double_t val) {fRelEfficiency=val;};
   void SetUsed(Bool_t  used)         {fUsed = used;};

   Bool_t IsSortable() const {return kTRUE;};

   Int_t Compare(const TObject *obj) const;
	void PrintPeak(ostream & ostr = cout) const ;
   void Print(Option_t * Option) const;
//   void Print();
   void PrintHeadLine();
ClassDef(FhPeak,1)
};

#endif

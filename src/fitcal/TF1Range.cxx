#include "TF1Range.h"

TF1Range::TF1Range(const char* name, const char* formula, Double_t xmin, Double_t xmax)
   : TF1(name, formula,xmin, xmax)
{
}
Double_t TF1Range::Eval(Double_t x, Double_t /*y*/, Double_t /*z*/ , Double_t /*t*/) const
{
   if ( x < GetXmin() || x > GetXmax() ) {
      return 0;
   } else {
      return TF1::Eval(x);
   }
}

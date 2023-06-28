#ifndef TF1RANGE
#define TF1RANGE
#include "TF1.h"
//_____________________________________________________________________________________

class TF1Range : public TF1 {
public:
   TF1Range(const char* name, const char* formula, Double_t xmin = 0, Double_t xmax = 1);
   Double_t Eval(Double_t x, Double_t y = 0, Double_t z = 0, Double_t t = 0) const;
ClassDef(TF1Range,1)
};
#endif

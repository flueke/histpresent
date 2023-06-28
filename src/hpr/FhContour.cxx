#include "TObjString.h"
#include "TOrdCollection.h"
#include "FhContour.h"
#include "TGMrbTableFrame.h"
#include <iostream>
#include <iomanip>

ClassImp(FhContour)

//__________________________________________________________________________

FhContour::FhContour(const char* name, const char* title, const Int_t len, 
							const Double_t * levels , const Int_t * colors):
					 TNamed(name, title) 
{
  if (len > 0) {
	  if (levels) fLevels.Set(len, levels);
	  else        fLevels.Set(len);
	  if (colors) fColors.Set(len, colors);
	  else        fColors.Set(len);
  }
}; 
//__________________________________________________________________________

void FhContour::Print()
{
	cout << "Contourname: " << GetName() << endl
		  << "Number of levels " << fLevels.GetSize() << endl;
	if (fLevels.GetSize() > 0){
		for (Int_t i = 0; i < fLevels.GetSize(); i++){
			cout << setw(3) << i << " "<< setw(12) << fLevels[i] << " " 
				  << setbase(16) << setw(10) << fColors[i] << endl;
		}
	}
}
//__________________________________________________________________________

Int_t FhContour::Edit( TGWindow * canvas)
{
	Int_t ncont = TMath::Max(fLevels.GetSize(),fColors.GetSize());
	if (ncont <= 0) return -1;
//   TArrayD xyvals(ncont);
//   TArrayI colors(ncont);
	TOrdCollection *col_lab = new TOrdCollection();
	TOrdCollection *row_lab = new TOrdCollection();
	col_lab->Add(new TObjString("Level"));
	col_lab->Add(new TObjString("Color"));
	TString rowl;
	for (Int_t i=0; i < ncont; i++) {
		rowl = "Level #: ";
		rowl += i;
		row_lab->Add(new TObjString(rowl.Data()));
	}
	Int_t ret = 0, ncols = 1, itemwidth=120, precission = 5; 
	TGMrbTableOfDoubles(canvas, &ret, "Contour values", 
								 itemwidth, ncols, ncont, fLevels, precission,
								  col_lab, row_lab, &fColors, -ncont);
	return ret;
}   

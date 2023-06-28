#include "TPad.h"
#include "TRegexp.h"
#include "Buttons.h"
#include "HprLegend.h"
#include <iostream>

ClassImp(HprLegend)

//________________________________________________________________________

HprLegend::HprLegend(Double_t x1, Double_t y1,Double_t x2, Double_t y2,
                  const char *header, Option_t *option)
           : TLegend(x1, y1, x2, y2, header, option)
{
//	std::cout << "ctor HprLegend x1,y1, x2,y2" << x1 << " " 
//	            << y1 << " "<< x2 << " "  <<y2 << std::endl;
}

void   HprLegend::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	TLegendEntry *entry = GetEntry();
	if ( !entry ) return;
	if ( event == kButton1Down ) {
		TObject *obj = entry->GetObject();
		if (obj) {
			obj->Pop();
			this->Pop();
		}
	}
	TLegend::ExecuteEvent(event, px, py);
}

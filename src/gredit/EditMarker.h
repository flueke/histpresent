#ifndef EDITMARKER
#define EDITMARKER
#include "TMarker.h"

namespace std {} using namespace std;

//#endif
class EditMarker : public TMarker {

public:
   EditMarker(){};
   EditMarker(Float_t x1, Float_t y1);
   EditMarker(Float_t x1, Float_t y1, Float_t sz,  Int_t mstyle = 2);
   ~EditMarker(){};
   Int_t DistancetoPrimitive(Int_t /*px*/, Int_t /*py*/){ return 9999; };
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(ofstream &, Option_t *){};     // dont write to .C file
 #endif 
ClassDef(EditMarker,0)
};
#endif

#include "EditMarker.h"
ClassImp(EditMarker)

EditMarker::EditMarker(Float_t x, Float_t y) : TMarker(x,y,2)
{
  TMarker::SetUniqueID(0xaffec0c0);
}
              
EditMarker::EditMarker(Float_t x, Float_t y, Float_t sz, Int_t mstyle) : TMarker(x,y, mstyle)
{
  TMarker::SetUniqueID(0xaffec0c0);
  TMarker::SetMarkerSize(sz);
};

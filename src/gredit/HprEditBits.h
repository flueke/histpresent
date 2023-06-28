#ifndef HPREDITBITS
#define HPREDITBITS
#include "Rtypes.h"

enum {
   kCantEdit       = BIT(20),
   kIsBound        = BIT(21),
   kIsEnclosingCut = BIT(22),
   kIsAEditorPage  = BIT(23)
};
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldUtil
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/v3d.h"
#include "../game/Constants.h"

class WorldUtil {
public:
  // i don't know that this is a great place for this, but ...
  // i need it in both WorldMap and OverdrawManager, and some other places that
  // used to go through WorldMap
  static v3di_t getRegionIndex(const v3d_t &pos);
  static v3di_t getRegionIndex(const v3di_t &pos);
};

#include "WorldUtil.h"

v3di_t WorldUtil::getRegionIndex(const v3d_t &pos) {
  v3di_t ri;
  ri.x = (int)(floor((pos.x / (double)(WORLD_CHUNK_SIDE))));
  ri.y = (int)(floor((pos.y / (double)(WORLD_CHUNK_SIDE))));
  ri.z = (int)(floor((pos.z / (double)(WORLD_CHUNK_SIDE))));
  return ri;
}

v3di_t WorldUtil::getRegionIndex(const v3di_t &pos) {
  v3di_t ri;
  // i might need to cast these to doubles and then back to ints again...
  ri.x = (int)(floor((double)pos.x / (double)WORLD_CHUNK_SIDE));
  ri.y = (int)(floor((double)pos.y / (double)WORLD_CHUNK_SIDE));
  ri.z = (int)(floor((double)pos.z / (double)WORLD_CHUNK_SIDE));
  return ri;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BiomeMap
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/MathUtil.h"
#include "../math/PseudoRandom.h"
#include "../world/Terrain.h"
#include "../world/BlockTypeData.h"


struct BiomeInfo {
  int type;
  float value;
};


struct BiomeType {
  double terrainHeightOffset;
  double terrainHeightMultiplier;
  int noise3dType;
  double noise3dValue;
  int blockTypeGround;
  int blockTypeSubterran;
  int blockTypeLiquid;
  int treeScheme;

  int groundCoverType;
  int groundCoverMaxHeight;
  double groundCoverHighPass; // [0, 1.0) must get above this number to have ground cover

  v3d_t planetMapColor;
};


enum {
  BIOME_TYPE_DESERT,
  BIOME_TYPE_FIELD,
  BIOME_TYPE_SNOW,
  BIOME_TYPE_MARS,
  BIOME_TYPE_METAL,
  BIOME_TYPE_LIGHT_PURPLE,
  BIOME_TYPE_GLASS,
  NUM_BIOME_TYPES
};


extern BiomeType gBiomeTypes[];


class BiomeMap {
public:
  BiomeMap();
  ~BiomeMap();

  void randomize(PseudoRandom prng, int typesW, int typesH);

  BiomeInfo getBiomeInfo(int x, int z) const;

private:
  Terrain mTerrain1;
  Terrain mTerrain2;

  int mTypesW;
  int mTypesH;

  BiomeType* mBiomeTypes;

  double mInterval1;
  double mInterval2;
};


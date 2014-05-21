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

#include "PseudoRandom.h"
#include "Terrain.h"
#include "BlockTypeData.h"

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

BiomeType gBiomeTypes[];

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


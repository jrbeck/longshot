#include "BiomeMap.h"

// dunno that this is really the place for this ...
// or that this shouldn't be in an external config file somewehere
BiomeType gBiomeTypes[] = {
  { // BIOME_TYPE_DESERT
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_SAND, //  int groundType;
    BLOCK_TYPE_SAND, //  int blockTypeSubterran;
    BLOCK_TYPE_WATER, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.9, 0.8, 0.5 }, // float planetMapColor[4];
  },

  { // BIOME_TYPE_FIELD
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_GRASS, //  int groundType;
    BLOCK_TYPE_DIRT, //  int blockTypeSubterran;
    BLOCK_TYPE_SLUDGE, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.1, 0.6, 0.2 }, // float planetMapColor[4];
  },

  { // BIOME_TYPE_SNOW
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_SNOW, //  int groundType;
    BLOCK_TYPE_DIRT, //  int blockTypeSubterran;
    BLOCK_TYPE_WATER, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.9, 0.8, 0.9 }, // float planetMapColor[4];
  },
  { // BIOME_TYPE_MARS
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_MARS, //  int groundType;
    BLOCK_TYPE_MARS, //  int blockTypeSubterran;
    BLOCK_TYPE_BLOOD, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.6, 0.3, 0.2 }, // float planetMapColor[4];
  },
  { // BIOME_TYPE_METAL
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_METAL_TILE_GREY, //  int groundType;
    BLOCK_TYPE_METAL_TILE_GREY, //  int blockTypeSubterran;
    BLOCK_TYPE_LAVA, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.7, 0.7, 0.7 }, // float planetMapColor[4];
  },
  { // BIOME_TYPE_LIGHT_PURPLE
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_LIGHT_PURPLE, //  int groundType;
    BLOCK_TYPE_METAL_TILE_GREY, //  int blockTypeSubterran;
    BLOCK_TYPE_WATER, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.42, 0.35, 0.47 }, // float planetMapColor[4];
  },
  { // BIOME_TYPE_GLASS
    0.0, //  double terrainHeightOffset;
    0.0, //  double terrainHeightMultiplier;
    0, //  int noise3dType;
    0.0, //  double noise3dValue;
    BLOCK_TYPE_GLASS, //  int groundType;
    BLOCK_TYPE_GLASS,  //  int blockTypeSubterran;
    BLOCK_TYPE_WATER, //  int blockTypeLiquid;
    0, //  int treeScheme;
    { 0.0, 0.0, 0.0 }, // float planetMapColor[4];
  },
};




BiomeMap::BiomeMap() {
}

BiomeMap::~BiomeMap() {
}

void BiomeMap::randomize(PseudoRandom prng, int typesW, int typesH) {
  mTypesW = typesW;
  mTypesH = typesH;
  mInterval1 = 1.0 / typesW;
  mInterval2 = 1.0 / typesH;


  mTerrain1.resize(1024);
  mTerrain1.generateTilable(64.0, prng);
  mTerrain1.normalize(0.0, 1.0);

  mTerrain2.resize(1024);
  mTerrain2.generateTilable(64.0, prng);
  mTerrain2.normalize(0.0, 1.0);

}

BiomeInfo BiomeMap::getBiomeInfo(int i, int j) const {

  double iScale = 0.075;
  double jScale = 0.075;

//	double a = mTerrain1.get_value(i, j);
//	double b = mTerrain2.get_value(i, j);
  double a = mTerrain1.getValueBilerp((double)i * iScale, (double)j * jScale);
  double b = mTerrain2.getValueBilerp((double)i * iScale, (double)j * jScale);

  int aa = min(floor(a / mInterval1), mTypesW - 1);
  int bb = min(floor(b / mInterval2), mTypesH - 1);

  double cutoff = 0.05;

  double x = (a - ((double)aa * mInterval1)) / mInterval1;
  if (x > 0.5) {
    x = 1.0 - x;
  }

  x = 2.0 * x;

  if (x < cutoff) {
    x = x / cutoff;
//		x = 0.0;
  }
  else {
    x = 1.0;
  }


  double y = (b - ((double)bb * mInterval2)) / mInterval2;
  if (y > 0.5) {
    y = 1.0 - y;
  }

  y = 2.0 * y;

  if (y < cutoff) {
    y = y / cutoff;
//		y = 0.0;
  }
  else {
    y = 1.0;
  }

  BiomeInfo ret;
  ret.type = aa + (bb * mTypesW);
  ret.value = x * y;

  return ret;
}

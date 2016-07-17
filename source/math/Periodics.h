// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Periodics
// *
// * this creates the random landscapes and much more!
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "AssetManager.h"

#include "Simplex.h"
#include "Terrain.h"
#include "BiomeMap.h"
#include "PseudoRandom.h"


#define WATER_LEVEL            (-1)
#define SEA_FLOOR_HEIGHT       (-15.0)
#define SNOW_LEVEL             (90)
#define DESERT_MOISTURE_LEVEL  (0.4)

#define NUM_TERRAINS           (3)


struct GroundCoverInfo {
  int height;
  BYTE blockType;
};


class Periodics {
private:
  // copy constructor guard
  Periodics (const Periodics& periodics) { }
  // assignment operator guard
  Periodics& operator=(const Periodics& periodics) { return *this; }

public:
  Periodics();
  ~Periodics();

  int saveToDisk(FILE* file);
  int loadFromDisk(FILE* file);

  void randomize(int seed);

  int getTerrainHeight(int x, int z) const;

  // does this really need to be accessible? it's here now for testing...
  BiomeInfo getBiomeInfo(int x, int z) const;

  GroundCoverInfo getGroundCoverInfo(v3di_t worldPosition, BYTE groundType);

  double getPrecipitationLevel(double worldX, double worldZ) const;

  BYTE generateBlockAtWorldPosition(v3di_t worldPosition);
  BYTE generateBlockAtWorldPosition(v3di_t worldPosition, int terrainHeight);
  BYTE generateBlockAtWorldPosition(v3di_t worldPosition, int terrainHeight, BiomeInfo& biomeInfo);

  BYTE generateDesertBlock(v3di_t worldPosition, int terrainHeight);

//private:
  int mSeed;
  PseudoRandom mPrng;

  // FIXME: might as well make these private
  Terrain mTerrains[NUM_TERRAINS];
  Terrain mRandomMap;

  BiomeMap mBiomeMap;
};

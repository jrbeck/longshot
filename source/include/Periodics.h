// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Periodics
// *
// * this creates the random landscapes and much more!
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Periodics_h_
#define Periodics_h_


#include "AssetManager.h"

#include "Simplex.h"
#include "Terrain.h"
#include "BiomeMap.h"
#include "PseudoRandom.h"



#define WATER_LEVEL			(-1)
#define SEA_FLOOR_HEIGHT	(-15.0)
#define SNOW_LEVEL			(90)


//#define BIOME_TYPE_W (2)
//#define BIOME_TYPE_H (1)




#define NUM_TERRAINS				(3)

#define DESERT_MOISTURE_LEVEL		(0.4)


class Periodics {
private:
  // copy constructor guard
  Periodics (const Periodics &periodics) { }
  // assignment operator guard
  Periodics & operator=(const Periodics &periodics) { return *this; }


public:
  Periodics(void);
  ~Periodics(void);

  int saveToDisk(FILE *file);
  int loadFromDisk(FILE *file);

  void randomize(int seed);

  int getTerrainHeight(int x, int z) const;

  // does this really need to be accessible? it's here now for testing...
  BiomeInfo getBiomeInfo(int x, int z) const;

  double getPrecipitationLevel(double worldX, double worldZ) const;

  BYTE generateBlockAtWorldPosition(v3di_t worldPosition);
  BYTE generateBlockAtWorldPosition(v3di_t worldPosition, int terrainHeight);
  BYTE generateBlockAtWorldPosition(v3di_t worldPosition, int terrainHeight, BiomeInfo &biomeInfo);

  BYTE generateDesertBlock(v3di_t worldPosition, int terrainHeight);

//private:
  int mSeed;
  PseudoRandom mPrng;

  // FIXME: might as well make these private
  Terrain mTerrains[NUM_TERRAINS];
  Terrain mRandomMap;

  BiomeMap mBiomeMap;
};



#endif // Periodics_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * World
// *
// * this class is basically the instantiation of a Planet object
// *
// * this is the root of the World->WorldMap->WorldColumn->WorldChunk hierachy.
// *
// * it implements the things like sun/moon/stars. it also grows trees for some reason.
// * the game_c uses this as an interface to load/save
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

// #include <omp.h>
#include <SDL2/SDL_opengl.h>

#include "../world/Location.h"
#include "../math/v3d.h"
#include "../world/WorldMap.h"
#include "../world/Terrain.h"
#include "../assets/AssetManager.h"
#include "../world/SkySim.h"
#include "../math/Periodics.h"
#include "../game/LoadScreen.h"


#define NUM_PRELOADED_COLUMNS  100


class World : public Location {
public:
  World();
  virtual ~World();

  // these are inherited from Location
  int initialize(FILE *file, Galaxy *galaxy, int planetHandle);
  void save(FILE *file);
  void load(FILE *file);
  // updates the columns that are loaded in a WorldMap
  int update(v3d_t playerPosition);
  void drawEnvironment(GlCamera &cam);
  // finds a start position that is conducive to the player within a
  // certain area of land
  v3d_t getStartPosition(void);

  // World-only methods * * * * * * * * *
  void setStartPosition(v3d_t pos);

  void clear(bool clearInactive);

  int preloadColumns(int numColumns, v3d_t pos, LoadScreen* loadScreen);
  int loadSurroundingColumns(v3d_t pos);

  int loadColumn(int xIndex, int zIndex, bool doOutcroppings);
  int loadColumn(int xIndex, int zIndex, const int *heightMap);
  int loadColumn(WorldColumn &worldColumn, int xIndex, int zIndex, const int *heightMap, bool doOutCroppings);

  void applyOverdrawBlocks(WorldColumn &wc);

  // these should probably be put into FeatureGenerator?
  void growTree(v3di_t worldPosition, int floorBlockType, int numWaterInColumn);
  void growBlockTree(v3di_t position);
  void growRandomTree(v3di_t position);
  void growSpiralTree(v3di_t position);
  void growCactusTree(v3di_t position);
  void growPalmTree(v3di_t position);
  void growGroundCover(v3di_t position);

  int getTerrainHeight(int x, int z);

  static const int WORLD_MAP_SIDE = 32;

private:
  Galaxy *mGalaxy;
  int mPlanetHandle;

  SkySim *mSkySim;

  Periodics mPeriodics;

  v3d_t mPlayerPosition;

  // HACK
  bool mIsPlayerStartPosSet;
  v3d_t mPlayerStartPos;
};

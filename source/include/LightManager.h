// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * LightManager
// *
// * manages the WorldLights
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "WorldMap.h"
#include "WorldLight.h"

using namespace std;

class LightManager {
public:
  LightManager();
  ~LightManager();

  size_t addLight(const v3d_t& position, double radius, const IntColor& color, WorldMap& worldMap);
  // tell the worldMap what columns need their lighting updated
  void updateWorldColumns(const WorldLight& light, WorldMap& worldMap) const;
  void removeLight(size_t handle);

  // this could be sped up by doing some sort of pre-column test to
  // get just the possible lights to test against at each block
  // this will be a lot more expensive when depth/height increases
  IntColor getLightLevel(const v3di_t& position) const;

  // this puts lights that aren't contained in a loaded
  // WorldColumn onto the mInactiveLights, and the opposite
  void update(WorldMap& worldMap);

  void clear();

  void save(FILE* file);
  void load(FILE* file);

  //private:
  size_t mNextHandle;

  vector<WorldLight*> mLights;
  vector<WorldLight*> mInactiveLights;
};


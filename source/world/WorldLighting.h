// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldLighting
// *
// * handles the player aboard a starship
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/v3d.h"
#include "../world/WorldMap.h"
#include "../world/LightManager.h"
#include "../world/Terrain.h"

using namespace std;

class WorldLighting {
public:
  // this method should be called: propagateSunlight()...
  static void createShadowVolume( int columnIndex, WorldMap& worldMap );
  static int getNumSolidNeighbors( v3di_t worldPosition, const WorldMap& worldMap );
  static void applyLighting(
    int columnIndex,
    WorldMap& worldMap,
    const LightManager& lightManager,
    IntColor& sunColor );

};

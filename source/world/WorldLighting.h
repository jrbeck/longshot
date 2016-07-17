// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldLighting
// *
// * handles the player aboard a starship
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef WorldLighting_h_
#define WorldLighting_h_


#include "v3d.h"
//#include "Constants.h"
#include "WorldMap.h"
#include "LightManager.h"
#include "Terrain.h"

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





#endif // WorldLighting_h_

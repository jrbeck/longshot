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

#ifndef World_h_
#define World_h_

#include <omp.h>

#include "Location.h"
#include "v3d.h"
#include "WorldMap.h"
#include "Terrain.h"
#include "AssetManager.h"
#include "SkySim.h"
#include "Periodics.h"
#include "SdlInterface.h"
#include "LoadScreen.h"


#define NUM_PRELOADED_COLUMNS	100


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
		// basically draw the stars'n stuff
	void draw(gl_camera_c &cam);
		// finds a start position that is conducive to the player within a
		// certain area of land
	v3d_t getStartPosition(void);

	// World-only methods * * * * * * * * *
	void setStartPosition(v3d_t pos);

	void clear (bool clearInactive);

	// loads the set-pieces around some location
	void loadFeaturesAroundPlayer (v3d_t playerPos);

	int preloadColumns (int numColumns, v3d_t pos);
	int loadSurroundingColumns (v3d_t pos);

	int loadColumn (int xIndex, int zIndex, bool doOutcroppings);
	int loadColumn (int xIndex, int zIndex, const int *heightMap);
	int loadColumn (WorldColumn &wc, int xIndex, int zIndex, const int *heightMap, bool doOutCroppings);

	// these should probably be put into FeatureGenerator?
	void growTree (v3di_t worldPosition, int floorBlockType, int numWaterInColumn);
	void growBlockTree (v3di_t position);
	void growRandomTree (v3di_t position);
	void growSpiralTree (v3di_t position);
	void growCactusTree (v3di_t position);
	void growPalmTree (v3di_t position);
	void growGrass(v3di_t position);

	// mostly here for FeatureGenerator
	int getTerrainHeight (int x, int z);

	static const int WORLD_MAP_SIDE = 24;

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




#endif // World_h_
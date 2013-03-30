// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldMap
// *
// * part of World->WorldMap->WorldColumn->WorldChunk
// *
// * the WorldMap stores and manages the WorldColumns. it also manages loading to
// * and saving from the inactive column manager
// *
// * the lighting is performed by the mWorldLighting object
// *
// * rendering is ...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef WorldMap_h_
#define WorldMap_h_

#include <vector>

#include "BlockTypeData.h"
#include "WorldColumn.h"
#include "InactiveColumnManager.h"
#include "MathUtil.h"



class WorldMap {
private:
	// copy constructor guard
	WorldMap (const WorldMap &worldMap) : mInactiveColumnManager(WORLD_CHUNK_SIDE) { }
	// assignment operator guard
	WorldMap &operator=(const WorldMap &worldMap) { return *this; }

public:
	WorldMap ();
	~WorldMap();

	void resize (int xWidth, int zWidth);
	void clear (bool clearInactive);

	v3di_t getRegionIndex (const v3d_t &pos) const;
	int getColumnIndexByRegionCoords (int xIndex, int zIndex) const;

	int getColumnIndex (v3di_t worldPosition) const;
	int getColumnIndex (v3d_t worldPosition) const;

	int pickColumn (v3di_t pos) const;
	bool isColumnLoaded (int xIndex, int zIndex);
	void clearColumn (int xIndex, int zIndex);
	void clearColumn (size_t columnIndex);

	int getBlockType (v3di_t position) const;
	void setBlockType (v3di_t position, BYTE type);

	void setBlockVisibility (v3di_t position, BYTE visibility);

	block_t *getBlock (const v3di_t &position) const;
	block_t *getBlock (const v3d_t &position) const;
	int setBlock (const v3di_t &position, const block_t &block);

	BYTE getUniqueLighting (const v3di_t &position);
	void setUniqueLighting (const v3di_t &position, BYTE level);

	bool isSolidBlock (v3di_t position);
	bool isBoundingBoxEmpty (const BoundingBox &boundingBox);
	bool isBoundingBoxInLiquid (const BoundingBox &boundingBox);
	double getViscosity (const BoundingBox &boundingBox);
	double getHealthEffects (const BoundingBox &boundingBox);

	void clearBlock (v3di_t position);
	void fillVolume (v3di_t a, v3di_t b, int blockType);
	int fillSphere (v3d_t pos, double radius, int blockType, BYTE uniqueLighting);
	int clearSphere (v3d_t pos, double radius);

//	void generateChunkContaining (v3di_t position);
	void generateChunkContaining (WorldColumn &worldColumn, v3di_t position);
	bool columnHasFourNeighbors (int columnIndex);

	// this is really part of the viewing procees and should be extracted to WorldMapView
	void updateBlockVisibility (int columnIndex);

	// this all has to do with the fluid sim
	// FIXME: get this outta here!
	void addToChangedList (int columnIndex);
	void removeFromChangedList (int columnIndex);
	void updateFluids (void);
	bool updateFluid (int columnIndex);
	bool updateLiquidBlock (int blockType, int columnIndex, v3di_t worldPosition);
	int countLiquidNeighbors (int blockType, v3di_t worldPosition);


	// TODO: should this be merged with the InactiveColumnManager?
	int save(FILE *file);
	int load(FILE *file);
	void swapOutToInactive(void);
	void saveToInactive(void);

	// these are really utility functions...I imagine they should
	// be moved out of this class?
	bool lineOfSight (const v3d_t &a, const v3d_t &b);
	bool rayCastSolidBlock (const v3d_t &a, const v3d_t &b, v3di_t &hitPosition, int &face);



// MEMBERS * * * * * * * * * * *

	int mXWidth;
	int mZWidth;

	int mNumColumns;

	WorldColumn *mColumns;

	BYTE mWorldLightingFloor;
	BYTE mWorldLightingCeiling;

	// this is used to track which columns have changed
	// so that the fluid sim can be updated...yuck!
	vector <int> mChangedList;

	// this stores all the chunks that have ever been loaded in memory,
	// it is used both for saving the data as well as allowing it to be
	// loaded into memory very quickly if it was already loaded before
	InactiveColumnManager mInactiveColumnManager;

	// if this is set, periodics will be consulted when creating a new
	// WorldChunk. This is to account for digging, etc...
	Periodics *mPeriodics;
};



#endif // WorldMap_h_
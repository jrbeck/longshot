// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * FeatureGenerator
// *
// * this is responsible for the procedurally generated world features
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef FeatureGenerator_h_
#define FeatureGenerator_h_

#include "LoadScreen.h"
#include "World.h"
#include "RogueMap.h"
#include "dungeon/DungeonUtil.h"
#include "dungeon/MoleculeDungeon.h"
#include "InactiveList.h"


struct height_info_t {
	int high;
	int low;
	int avg;
};


// this is for the worldRogueMap
// i.e. numColumns x numColumns
#define ROGUE_W		64
#define ROGUE_H		64


enum {
	FG_CORNER_SW,
	FG_CORNER_NW,
	FG_CORNER_NE,
	FG_CORNER_SE,

	FG_NUM_CORNERS
};


class FeatureGenerator {
public:

	static void createSetPieces (int xIndex, int zIndex, World& world);

	static void loadWorldRegion(v2di_t cornerIndex, int sideLength, World &world, bool doOutcroppings);
	static height_info_t getHeightInfo (int worldX, int worldZ, int sideX, int sideZ, World &world);

	static void createPyramid (int side, v2di_t cornerIndex, World &world);
	static void create1by1Tower (v2di_t cornerIndex, int baseHeight, WorldMap &worldMap);
	static void buildSpiralStaircase (v3di_t southwestInsideCorner, int stairsPerLevel, int topHeight,
		int startCorner, WorldMap &worldMap);

	static void drillSpiral (v3d_t top, double radius, double height,
		double totalRotation, int numSteps, WorldMap &worldMap);

	static void drillCavern (v2di_t cornerIndex, int sideX, int sideZ, World &world);

	static void growSpiralGarden (v2di_t cornerIndex, int sideX, int sideZ, World &world);

	static void createCastle8x8 (v2di_t cornerIndex, World &world);

	static void createPlain (v2di_t cornerIndex, World &world);
	
	static void createVillage (int side, v2di_t cornerIndex, World &world);

	static void drawPyramid (v3di_t baseNearPoint, int sideLength, char blockType, World &world);
	
	static void createHouse (v2di_t cornerIndex, World &world);
	static void createBuilding2x2 (v2di_t cornerIndex, World &world);

	static void createPlain (v2di_t cornerIndex, int sideX, int sideZ, World &world);

	static void createForViewer (v3d_t startPos, int type, World &world);

	static void clearDungeonBlock (int worldX, int worldY, int worldZ, WorldMap &worldMap);
	static void draw2x2Block( v3di_t worldPos, char blockType, WorldMap& worldMap );
	static void createDungeon (v2di_t cornerIndex, int depth, World &world);
	static v2di_t createRogueRec (v2di_t cornerIndex, v2di_t stairsUp, int height, int levelsRemaining, WorldMap &worldMap);
	static v2di_t createDungeonRec (v2di_t cornerIndex, v2di_t stairsUp, int height, int levelsRemaining, WorldMap &worldMap);

	static void hollowOutDungeon2x( DungeonUtil* dungeon, WorldMap& worldMap, const v2di_t& cornerIndex, int dungeonSideChunks, int height );
};



#endif // FeatureGenerator_h_

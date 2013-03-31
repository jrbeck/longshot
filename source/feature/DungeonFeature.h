// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * DungeonFeature
// *
// * desc: hollow out Dungeons in the World
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once


#include "v2d.h"
#include "v3d.h"

#include "FeatureUtil.h"
#include "../dungeon/DungeonUtil.h"
#include "../dungeon/MoleculeDungeon.h"


class DungeonFeature {
public:
	static void createDungeon (v2di_t cornerIndex, int levels, World &world);
	
	static v2di_t createDungeonRec (v2di_t cornerIndex, v2di_t stairsUp, int height, int levelsRemaining, World &world);

	static void hollowOutDungeon2x( DungeonUtil* dungeon, WorldMap& worldMap, const v2di_t& cornerIndex, int dungeonSideChunks, int height );

private:
	static void clearDungeonBlock (int worldX, int worldY, int worldZ, WorldMap &worldMap);
	static void draw2x2Block( v3di_t worldPos, char blockType, WorldMap& worldMap );
	static void addLights( World& world, DungeonUtil* dungeon, const v2di_t& cornerIndex, int dungeonSide, int ceilingHeight );

};


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

#include "InactiveList.h"


class DungeonFeature {
public:
  static void createDungeon(v2di_t cornerIndex, int levels, World& world);

  static v2di_t createDungeonRec(v2di_t cornerIndex, v2di_t stairsUp, int height, int levelsRemaining, World& world);

  // scale refers to how to scale the dungeon tiles into world coords on the x and z axes
  static void hollowOutDungeon(DungeonUtil* dungeon, WorldMap& worldMap, const v2di_t& cornerIndex, int dungeonSideChunks, int height, int scale);

private:
  static void clearDungeonBlock(int worldX, int worldY, int worldZ, WorldMap& worldMap, int scale);
  static void drawBlock(v3di_t worldPos, char blockType, WorldMap& worldMap, int scale);
  static void addLights(World& world, DungeonUtil* dungeon, const v2di_t& cornerIndex, int dungeonSide, int ceilingHeight, int scale);
  static void buildExitStaircase(v3di_t worldEntryPoint, World& world);
  static void addMonsterGenerators(World& world, DungeonUtil* dungeon, const v2di_t& cornerIndex, int dungeonSide, int ceilingHeight, int scale);
};

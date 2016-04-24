#include "DungeonFeature.h"

// cornerIndex = chunk index "bottom left"
// depth = dungeon levels
void DungeonFeature::createDungeon (v2di_t cornerIndex, int levels, World &world) {
  WorldMap &worldMap = *world.getWorldMap();

  // dungeon side length in chunks
  int dungeonSide = 8;

  int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
  int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

  int xSide = dungeonSide * WORLD_CHUNK_SIDE;
  int zSide = dungeonSide * WORLD_CHUNK_SIDE;

  height_info_t heightInfo = FeatureUtil::getHeightInfo(worldX, worldZ, xSide, xSide, world);

  FeatureUtil::loadWorldRegion(cornerIndex, dungeonSide, world, true);

  // this starts the dungeon creation process
  v2di_t stairDownRoomCorner = createDungeonRec (
    cornerIndex, v2di_v(0, 0), heightInfo.low, levels, world);

  // see if our dungeon creator failed
  if (stairDownRoomCorner.x == 0) {
    printf("DungeonFeature::createDungeon() failed!\n");
    return;
  }

  // now we need a connection to the outside world
  v3di_t worldEntryPoint = {
    worldX + stairDownRoomCorner.x,
    heightInfo.low - 6,
    worldZ + stairDownRoomCorner.y,
  };

  buildExitStaircase(worldEntryPoint, world);
}

// recursive dungeon burrower
// create a level of the dungeon
// returns the stairsUp location
//
v2di_t DungeonFeature::createDungeonRec(
  v2di_t cornerIndex,
  v2di_t stairsUp,
  int height,
  int levelsRemaining,
  World& world)
{
  WorldMap& worldMap = *world.getWorldMap();

  // this should be set according to the type of dungeon we're using
  int dungeonScale = 1;

  int dungeonSideChunks = 8; // the length of the dungeon side in chunks
  int dungeonSide = dungeonSideChunks * (WORLD_CHUNK_SIDE / dungeonScale); // the DungeonModel side length

  DungeonUtil *dungeon = new DungeonUtil();
  dungeon->createDungeonModel(dungeonSide, dungeonSide);

  // check to see if we are drawing the first level
  if (stairsUp.x == 0) {
    // TEMP: put the world -> dungeon staircase dead center
    stairsUp = v2di_v(dungeonSide / 2, dungeonSide / 2);
  }

  // if there's another level beneath this one, make sure we have a room for the
  // downward stairs
  v2di_t stairDownRoomCorner;
  if (levelsRemaining > 0) {
    //stairDownRoomCorner = rogueMap.random_room (500, 6, 6);

    // TEMP: this puts it in the same place as the last one ...
    stairDownRoomCorner = v2di_v(dungeonSide / 2, dungeonSide / 2);
  }

  stairsUp.x += 2;
  stairsUp.y += 2;

  v2di_t entryPoint = stairsUp;

  MoleculeDungeon::createDungeon(*dungeon, stairsUp);

  // this is kinda ugly, with the whole worldEntryPoint being 2/3 set in the hollowDungeon2x thing...
  hollowOutDungeon(dungeon, worldMap, cornerIndex, dungeonSide, height, dungeonScale);

  // this just blankets the non-wall area with lights
  addLights(world, dungeon, cornerIndex, dungeonSide, height - 3, dungeonScale);

  // build the staircase to the previous dungeon level
  v3di_t worldEntryPoint = v3di_v(
    cornerIndex.x * WORLD_CHUNK_SIDE + (entryPoint.x * dungeonScale),
    height - 6,
    cornerIndex.y * WORLD_CHUNK_SIDE + (entryPoint.y * dungeonScale));

  // clear out the shaft for the staircase
  v3di_t a = v3di_v(worldEntryPoint.x - 2, worldEntryPoint.y + 3, worldEntryPoint.z - 2);
  v3di_t b = v3di_v(worldEntryPoint.x + 3, height, worldEntryPoint.z + 3);
  worldMap.fillVolume(a, b, BLOCK_TYPE_AIR);

  // build the staircase
  v3di_t southwestInsideCorner = v3di_v(worldEntryPoint.x - 1, worldEntryPoint.y,  worldEntryPoint.z - 1);
  FeatureUtil::buildSpiralStaircase(southwestInsideCorner, 3, height, FG_CORNER_SW, worldMap);

//  addMonsterGenerators();

  delete dungeon;

  // do the next level
  if (levelsRemaining > 0 && stairDownRoomCorner.x != 0) {
    createDungeonRec (
      cornerIndex,
      // stair down on this level becomes stair up on next level
      stairDownRoomCorner,
      height - 7,
      levelsRemaining - 1,
      world);
  }

  return stairsUp;
}


// hollows out a DungeonModel, doubling the tiles (ie 2x2 blocks for each dungeon tile)
void DungeonFeature::hollowOutDungeon(
  DungeonUtil* dungeon,
  WorldMap& worldMap,
  const v2di_t& cornerIndex,
  int dungeonSide,
  int height,
  int scale)
{
  int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
  int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

  v3di_t worldPos = v3di_v(0, 0, 0);

  // iterate through the dungeon
  for (int dz = 0; dz < dungeonSide; dz++) {
    worldPos.z = worldZ + (dz * scale);
    for (int dx = 0; dx < dungeonSide; dx++) {
      worldPos.x = worldX + (dx * scale);

      int type = dungeon->getTile(dx, dz)->type;

      if (type != DUNGEON_TILE_WALL) {
        // clear the block
        clearDungeonBlock(worldPos.x, height - 6, worldPos.z, worldMap, scale);

        // lay down some flooring
        worldPos.y = height - 7;
        if (type == DUNGEON_TILE_WATER) {
          drawBlock(worldPos, BLOCK_TYPE_WATER, worldMap, scale);
        }
        else {
          drawBlock(worldPos, BLOCK_TYPE_GREEN_STAR_TILE, worldMap, scale);
        }

        // now the ceiling
        worldPos.y = height - 3;
        drawBlock(worldPos, BLOCK_TYPE_GREEN_STAR_TILE, worldMap, scale);
      }
    }
  }
}

void DungeonFeature::clearDungeonBlock(int worldX, int worldY, int worldZ, WorldMap& worldMap, int scale) {
  v3di_t nearCorner = {
    worldX,
    worldY,
    worldZ
  };

  v3di_t farCorner = {
    worldX + (scale - 1),
    worldY + 2,
    worldZ + (scale - 1)
  };

  worldMap.fillVolume(nearCorner, farCorner, BLOCK_TYPE_AIR);
}

void DungeonFeature::drawBlock(v3di_t worldPos, char blockType, WorldMap& worldMap, int scale) {
  v3di_t farCorner = {
    worldPos.x + (scale - 1),
    worldPos.y,
    worldPos.z + (scale - 1)
  };

  worldMap.fillVolume(worldPos, farCorner, blockType);
}

void DungeonFeature::addLights(
  World& world,
  DungeonUtil* dungeon,
  const v2di_t& cornerIndex,
  int dungeonSide,
  int ceilingHeight,
  int scale)
{
  LightManager* lightManager = world.getLightManager();
  WorldMap& worldMap = *world.getWorldMap();

  int red = r_numi(0, 2);
  int green = r_numi(0, 2);
  int blue = r_numi(0, 2);

  if (red + green + blue == 0) {
    return;
  }

  IntColor lightColor = { red * LIGHT_LEVEL_MAX, green * LIGHT_LEVEL_MAX, blue * LIGHT_LEVEL_MAX };
  double lightRadius = 7.0;

  int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
  int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

  v3di_t worldPos = v3di_v(0, 0, 0);
  v3d_t lightPos = v3d_v(0, ceilingHeight, 0);

  // iterate through the dungeon
  for (int dz = 0; dz < dungeonSide; dz += 5) {
    worldPos.z = worldZ + (dz * scale);
    lightPos.z = (double)worldPos.z + 0.5;
    for (int dx = 0; dx < dungeonSide; dx += 5) {
      worldPos.x = worldX + (dx * scale);
      lightPos.x = (double)worldPos.x + 0.5;

      int type = dungeon->getTile(dx, dz)->type;

      if (type != DUNGEON_TILE_WALL) {
        lightManager->addLight(lightPos, lightRadius, lightColor, worldMap);
      }
    }
  }

}


void DungeonFeature::buildExitStaircase(v3di_t worldEntryPoint, World& world) {
  WorldMap &worldMap = *world.getWorldMap();

  // ok, lets build the tower/staircase outta here!
  height_info_t heightInfo2 = FeatureUtil::getHeightInfo(worldEntryPoint.x - 3, worldEntryPoint.z - 3, 8, 8, world);
  v3di_t a, b;

  // fill the entrance volume
  a = v3di_v(worldEntryPoint.x - 3, worldEntryPoint.y + 3, worldEntryPoint.z - 3);
  b = v3di_v(worldEntryPoint.x + 4, heightInfo2.high + 25, worldEntryPoint.z + 4);
  worldMap.fillVolume(a, b, BLOCK_TYPE_OLD_BRICK);

  // clear it out now
  a = v3di_v(worldEntryPoint.x - 2, worldEntryPoint.y + 3, worldEntryPoint.z - 2);
  b = v3di_v(worldEntryPoint.x + 3, heightInfo2.high + 25, worldEntryPoint.z + 3);
  worldMap.fillVolume(a, b, BLOCK_TYPE_AIR);

  // hmm...need some way to see the staircase
  a = v3di_v(worldEntryPoint.x - 2, heightInfo2.high + 1, worldEntryPoint.z - 3);
  b = v3di_v(worldEntryPoint.x + 3, heightInfo2.high + 23, worldEntryPoint.z + 4);
  worldMap.fillVolume(a, b, BLOCK_TYPE_AIR);
  a = v3di_v(worldEntryPoint.x - 3, heightInfo2.high + 1, worldEntryPoint.z - 2);
  b = v3di_v(worldEntryPoint.x + 4, heightInfo2.high + 23, worldEntryPoint.z + 3);
  worldMap.fillVolume(a, b, BLOCK_TYPE_AIR);

  // and for the staircase
  v3di_t southwestInsideCorner = v3di_v(worldEntryPoint.x - 1, worldEntryPoint.y,  worldEntryPoint.z - 1);
  FeatureUtil::buildSpiralStaircase(southwestInsideCorner, 3, heightInfo2.high + 25,
    FG_CORNER_SW, worldMap);
}


void DungeonFeature::addMonsterGenerators(
  World& world,
  DungeonUtil* dungeon,
  const v2di_t& cornerIndex,
  int dungeonSide,
  int ceilingHeight,
  int scale)
{
  //// this stores any items that are generated
  //InactiveList inactiveList;

  //WorldMap& worldMap = *world.getWorldMap();

  //int red = r_numi( 0, 2 );
  //int green = r_numi( 0, 2 );
  //int blue = r_numi( 0, 2 );

  //if( red + green + blue == 0 ) {
  //  return;
  //}

  //IntColor lightColor = { red * LIGHT_LEVEL_MAX, green * LIGHT_LEVEL_MAX, blue * LIGHT_LEVEL_MAX };
  //double lightRadius = 7.0;

  //int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
  //int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

  //v3di_t worldPos = v3di_v( 0, 0, 0 );
  //v3d_t lightPos = v3d_v( 0, ceilingHeight, 0 );

  //// iterate through the dungeon
  //for( int dz = 0; dz < dungeonSide; dz += 5 ) {
  //  worldPos.z = worldZ + (dz * scale);
  //  lightPos.z = (double)worldPos.z + 0.5;
  //  for( int dx = 0; dx < dungeonSide; dx += 5 ) {
  //    worldPos.x = worldX + (dx * scale);
  //    lightPos.x = (double)worldPos.x + 0.5;

  //    int special = dungeon->getTile( dx, dz )->special;

  //    if (special == DUNGEON_TILE_SPECIAL_MONSTER_GENERATOR) {
  // place some mosnters while we here
  // THIS MAY ACTUALLY BE USED AT SOME POINT!!
  //v2di_t monsterPos;
  //int numSpawners = r_numi (8, 15);
  //for (int i = 0; i < numSpawners; i++) {

  //  monsterPos = rogueMap.placeRandomMonster ();

  //  if (monsterPos.x >= 0) {
  //    inactive_item_t monsterItem;

  //    monsterItem.type = INACTIVETYPE_ITEM;
  //    monsterItem.type2 = OBJTYPE_MONSTER_SPAWNER;

  //    monsterItem.pos.x = static_cast<double>(worldX + (monsterPos.x * 2)) + 1.0;
  //    monsterItem.pos.y = static_cast<double>(heightInfo.low - 6) + 1.5;
  //    monsterItem.pos.z = static_cast<double>(worldZ + (monsterPos.y * 2)) + 1.0;

  //    mInactiveList.addItem (monsterItem);
  //  }
  //}
  //    }
  //  }
  //}

  //inactiveList.saveToDisk ("save/inactive.list");
  //inactiveList.clear ();
}

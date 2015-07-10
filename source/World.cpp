#include "World.h"


World::World() :
  mGalaxy(NULL),
  mPlanetHandle(-1),
  mSkySim(NULL)
{
  mPlayerPosition = v3d_v (0.0, 0.0, 0.0);

  mType = LOCATION_WORLD;
  mWorldMap = NULL;
  mLightManager = NULL;

  // this is just here till i gettaround to something more long term!
  mIsPlayerStartPosSet = false;
}



World::~World(void) {
  printf("World::~World(): deleting\n");
  if (mWorldMap != NULL) {
    delete mWorldMap;
  }
  if (mLightManager != NULL) {
    delete mLightManager;
  }
  if (mSkySim != NULL) {
    delete mSkySim;
  }
}



void World::clear (bool clearInactive) {
  mWorldMap->resize(WORLD_MAP_SIDE, WORLD_MAP_SIDE);
  mWorldMap->clear(clearInactive);
}



int World::initialize(FILE *file, Galaxy *galaxy, int planetHandle) {
  if (mWorldMap != NULL) {
    delete mWorldMap;
  }
  mWorldMap = new WorldMap();
  mWorldMap->resize(WORLD_MAP_SIDE, WORLD_MAP_SIDE);
  mWorldMap->mUpdateLightingContinuously = true;

  if (mLightManager != NULL) {
    delete mLightManager;
  }
  mLightManager = new LightManager();

  if (file != NULL) {
    printf("World::initialize(): loading from file\n");
    load(file);
  }
  else {
    int seed = rand();
    printf("World::initialize(): loading from seed: %d\n", seed);
    mPeriodics.randomize(seed);
  }

  // these are used to set sun color, sky color, gravity, etc...
  mGalaxy = galaxy;
  mPlanetHandle = planetHandle;
  printf("World::initialize(): planetHandle: %d\n", planetHandle);
  StarSystem *starSystem = galaxy->getStarSystemByHandle(planetHandle);
  Planet *planet = galaxy->getPlanetByHandle(planetHandle);

  // FIXME: this seems a little wrong to be doing twice...
  printf("RESEEDING: %d\n", planet->mSeed);
  mPeriodics.randomize(planet->mSeed);
  mWorldMap->mPeriodics = &mPeriodics;

  mPlayerStartPos = getStartPosition();

  // give it some sky
  if (mSkySim != NULL) {
    delete mSkySim;
  }
  mSkySim = new SkySim();
  mSkySim->initialize(mPlayerStartPos);
  mSkySim->mSunColor[0] = starSystem->mStarColor[0];
  mSkySim->mSunColor[1] = starSystem->mStarColor[1];
  mSkySim->mSunColor[2] = starSystem->mStarColor[2];

  mSkySim->mSkyColor[0] = min(starSystem->mStarColor[2], 0.6f);
  mSkySim->mSkyColor[1] = min(starSystem->mStarColor[0], 0.6f);
  mSkySim->mSkyColor[2] = min(starSystem->mStarColor[1], 0.6f);

  return 0;
}


void World::save(FILE *file) {
  mWorldMap->save(file);
  mPeriodics.saveToDisk(file);
  mLightManager->save(file);
}


void World::load(FILE *file) {
  mWorldMap->load(file);
  mPeriodics.loadFromDisk(file);
  mLightManager->load(file);
}


v3d_t World::getStartPosition(void) {
  if (mIsPlayerStartPosSet) {
    return mPlayerStartPos;
  }

  bool foundAcceptableStartingLocation = false;
  int numAttempts = 0;

  double limit = 100.0;

  v2d_t searchPos;
  searchPos.x = mPeriodics.mPrng.getNextDouble(-10000.0, 10000.0);
  searchPos.y = mPeriodics.mPrng.getNextDouble(-10000.0, 10000.0);

  v3d_t pos;

  while (!foundAcceptableStartingLocation) {
    pos.x = mPeriodics.mPrng.getNextDouble(-limit, limit) + searchPos.x;
    pos.z = mPeriodics.mPrng.getNextDouble(-limit, limit) + searchPos.y;

    pos.y = getTerrainHeight (static_cast<int>(pos.x), static_cast<int>(pos.z)) + 2.0;

    if (pos.y > 1.0 && pos.y < 20.0) break;

    if (numAttempts++ > 10000) break;

    limit += 4.0;
  }

  pos.y += 30.0;

  v3d_print ("player start position", pos);
  printf ("numAttempts: %d\n", numAttempts);

  mPlayerStartPos = pos;
  mIsPlayerStartPosSet = true;

  return pos;
}


void World::setStartPosition(v3d_t pos) {
  mPlayerStartPos = pos;
  mIsPlayerStartPosSet = true;
}

void World::draw(gl_camera_c& cam) {
  mSkySim->draw(cam, mPlayerPosition);
}


int World::update(v3d_t playerPosition) {
  static Uint32 lastTick = -1000;
  static Uint32 lastChunkTick = -1000;

  Uint32 thisTick = SDL_GetTicks();

  // update fluid sim
  if (thisTick - lastTick > 500) {
    mWorldMap->updateFluids();
    lastTick = thisTick;
  }

  // load the columns around the player
  if (thisTick - lastChunkTick > 50) {
    loadSurroundingColumns(playerPosition);
    lastChunkTick = thisTick;
  }

  mPlayerPosition = playerPosition;

  mSkySim->update();

  // set the sunlight/moonlight level
  mWorldMap->mWorldLightingCeiling = mSkySim->getSkyLightLevel();

  // take care of any changes to the lights
  mLightManager->update(*mWorldMap);

  return 0;
}


int World::preloadColumns(int numColumns, v3d_t pos, LoadScreen* loadScreen) {
  int start_ticks = SDL_GetTicks ();
  printf ("%6d:  World::preloadColumns()", SDL_GetTicks ());

  // resize the WorldMap in case someone was monkeying with it
  mWorldMap->resize (WORLD_MAP_SIDE, WORLD_MAP_SIDE);

  // okay, let's actually load something now
  int numColumnsLoaded = 0;
  int numChunksLoaded = 0;

  for (int i = 0; i < numColumns; i++) {
    loadSurroundingColumns(pos);

    printf (".");
    loadScreen->draw(i, numColumns);
  }

  printf ("\n      columns loaded: %d, chunks: %d\n", numColumnsLoaded, numChunksLoaded);
  printf ("      time: %d\n\n", SDL_GetTicks () - start_ticks);

  return numColumnsLoaded;
}



int World::loadSurroundingColumns(v3d_t pos) {
  // this will follow a 'square spiral' pattern starting at the
  // given pos
  int numColumnsLoaded = 0;
  v3di_t regionIndex = WorldUtil::getRegionIndex (pos);

  int maxCount = mWorldMap->mXWidth * mWorldMap->mZWidth;
  static int nextAdd[4][2] = {
    { +1, 0 },
    { 0, +1 },
    { -1, 0 },
    { 0, -1 }};
  int direction = 0;
  int runLength = 1;
  int twosCounter = 0;

  // check under the player
  if (!mWorldMap->isColumnLoaded(regionIndex.x, regionIndex.z)) {
    loadColumn(regionIndex.x, regionIndex.z, true);
  }

  for (int count = 1; count < maxCount; /*count++*/ ) {
    for (int i = 0; i < runLength; i++) {
      // move to the next column region
      regionIndex.x += nextAdd[direction][0];
      regionIndex.z += nextAdd[direction][1];

      // check it
      if (!mWorldMap->isColumnLoaded(regionIndex.x, regionIndex.z)) {
        numColumnsLoaded += loadColumn(regionIndex.x, regionIndex.z, true);

//        v3di_print("loaded", regionIndex);

        if (++numColumnsLoaded > 1) {
          return numColumnsLoaded;
        }
      }
      count++;
      if (count == maxCount) {
        return 0;
      }
    }

    direction = (direction + 1) % 4;
    if (++twosCounter == 2) {
      runLength++;
      twosCounter = 0;
    }
  }

  return numColumnsLoaded;
}




int World::loadColumn(int xIndex, int zIndex, bool doOutcroppings) {
  int columnIndex = mWorldMap->getColumnIndexByRegionCoords(xIndex, zIndex);

  if (xIndex == mWorldMap->mColumns[columnIndex].mWorldIndex.x &&
    zIndex == mWorldMap->mColumns[columnIndex].mWorldIndex.z) {
    printf ("World::loadColumn() - tried to load a column that is already loaded\n");
    return 0;
  }

  mWorldMap->clearColumn(columnIndex);

  int numBlocks;

  // try to load from the inactive columns
  if (mWorldMap->mInactiveColumnManager.loadFromInactiveColumns(xIndex, zIndex, mWorldMap->mColumns[columnIndex]) == 0) {
//    printf ("loaded column from InactiveColumnManager\n");

    numBlocks = 0;
  }
  else {
    // generate this column fresh from the Periodics
    int worldX = xIndex * WORLD_CHUNK_SIDE;
    int worldZ = zIndex * WORLD_CHUNK_SIDE;

    int heightMap[(WORLD_CHUNK_SIDE + 2) * (WORLD_CHUNK_SIDE + 2)];

    for (int k = 0; k < (WORLD_CHUNK_SIDE + 2); k++) {
      for (int i = 0; i < (WORLD_CHUNK_SIDE + 2); i++) {
        heightMap[i + (k * (WORLD_CHUNK_SIDE + 2))] = mPeriodics.getTerrainHeight(worldX + i - 1, worldZ + k - 1);
      }
    }

    numBlocks = loadColumn(mWorldMap->mColumns[columnIndex], xIndex, zIndex, heightMap, doOutcroppings);
  }

  // prevent fluids from starting...
  // FIXME: this is a hack to compensate for a crappy
  // fluid sim
  mWorldMap->removeFromChangedList(columnIndex);

  return numBlocks;
}



int World::loadColumn(int xIndex, int zIndex, const int *heightMap) {
  int columnIndex = mWorldMap->getColumnIndexByRegionCoords (xIndex, zIndex);

  int numBlocks = loadColumn(mWorldMap->mColumns[columnIndex], xIndex, zIndex, heightMap, false);

  mWorldMap->mColumns[columnIndex].mNeedShadowVolume = true;
  mWorldMap->mColumns[columnIndex].mNeedLightingApplied = false;
  mWorldMap->mColumns[columnIndex].mNeedVisibility = false;
  mWorldMap->mColumns[columnIndex].mNeedDisplayList = false;

  // prevent fluids from starting...
  // FIXME: this is a hack to compensate for a crappy
  // fluid sim
  mWorldMap->removeFromChangedList(columnIndex);

  return numBlocks;
}



int World::loadColumn(WorldColumn &wc, int xIndex, int zIndex, const int *heightMap, bool doOutCroppings) {
  int worldX = xIndex * WORLD_CHUNK_SIDE;
  int worldZ = zIndex * WORLD_CHUNK_SIDE;

  wc.mWorldIndex.x = xIndex;
  wc.mWorldIndex.z = zIndex;

  wc.mWorldPosition.x = worldX;
  wc.mWorldPosition.z = worldZ;

  block_t block;
  block.faceVisibility = 0;
  block.uniqueLighting = LIGHT_LEVEL_NOT_SET;

  int floorBlockType = BLOCK_TYPE_DIRT;
  v3di_t worldPosition;
  worldPosition.z = worldZ;
  v3di_t relativePosition;

  for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++, worldPosition.z++) {
    worldPosition.x = worldX;
    for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++, worldPosition.x++) {
      // calculate the index for the heightmap: heightmap is an int[] which contains the height info
      // for the entire chunk 'floor'. It is apparently (WORLD_CHUNK_SIDE + 2)^2 in size. It has to be
      // because even the edges need to know their neighbors height to avoid gaps
      int terrainIndex = (relativePosition.x + 1) + ((relativePosition.z + 1) * (WORLD_CHUNK_SIDE + 2));
      int terrainHeight = heightMap[terrainIndex];

      if (terrainHeight > 5000 ||
        terrainHeight < -5000) {
          // this isn't really necessary...just kind of a warning
          printf ("World::loadColumn() - whoa boy\n");
      }

      int worldY = terrainHeight;

      // check neighbors to see if we need more blocks underneath
      int lowestNeighborHeight = 1000000;

      bool treePossible = true;
      int neighborHeight;

      // left
      int neighborIndex = terrainIndex - 1;
      neighborHeight = heightMap[neighborIndex];

      if (neighborHeight != worldY) treePossible = false;
      lowestNeighborHeight = min (lowestNeighborHeight, neighborHeight);

      // right
      neighborIndex = terrainIndex + 1;
      neighborHeight = heightMap[neighborIndex];

      if (neighborHeight != worldY) treePossible = false;
      lowestNeighborHeight = min (lowestNeighborHeight, neighborHeight);

      // backward
      neighborIndex = terrainIndex - (WORLD_CHUNK_SIDE + 2);
      neighborHeight = heightMap[neighborIndex];

      if (neighborHeight != worldY) treePossible = false;
      lowestNeighborHeight = min (lowestNeighborHeight, neighborHeight);

      // forward
      neighborIndex = terrainIndex + (WORLD_CHUNK_SIDE + 2);
      neighborHeight = heightMap[neighborIndex];

      if (neighborHeight != worldY) treePossible = false;
      lowestNeighborHeight = min (lowestNeighborHeight, neighborHeight);

      // now for the rest
      // this starts at 1 to include the ground floor
      int positiveNoiseHeight = 1;

      if (doOutCroppings) {
        positiveNoiseHeight += (int)floor(25.0 * mPeriodics.mRandomMap.getValueBilerp((double)worldPosition.x * 0.00321, (double)worldPosition.z * 0.00321));
      }

      if (worldY < WATER_LEVEL) {  // deal with the water
//        if (worldY < -20) worldY = -20;
        for (worldPosition.y = worldY; worldPosition.y < WATER_LEVEL; worldPosition.y++) {
          // this is where the actual block is figured out
          // this will generate the ground block and all the water above
          block.type = mPeriodics.generateBlockAtWorldPosition(worldPosition);
          if (block.type > BLOCK_TYPE_AIR) {
            wc.setBlockAtWorldPosition(worldPosition, block);

            if (gBlockData.get(block.type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
              wc.mNumUnderWater++;
            }
          }
        }
      }
      else { // finally, generate the stack of blocks for this x, z
        for (worldPosition.y = worldY; worldPosition.y < (worldY + positiveNoiseHeight); worldPosition.y++) {
          if (doOutCroppings) {
            block.type = mPeriodics.generateBlockAtWorldPosition(worldPosition);
          }
          else {
            block.type = mPeriodics.generateBlockAtWorldPosition(worldPosition, terrainHeight);
          }

          if (worldPosition.y == worldY) {
            floorBlockType = block.type;
          }
          if (block.type > BLOCK_TYPE_AIR) {
            wc.setBlockAtWorldPosition(worldPosition, block);
          }
        }

        if (doOutCroppings) {
          // this is the wrong place for this!!!!
          if (treePossible && r_numi(0, 10) >= 5) {
            // reset this to the ground level
            worldPosition.y = worldY;
            growTree(worldPosition, floorBlockType, wc.mNumUnderWater);
          }
          else {
            worldPosition.y = worldY;
            growGroundCover(worldPosition);
          }
        }
      }
    }
  }

  int lowest = wc.getLowestBlockHeight();

  // fill in the underground
/*  for (relativePosition.z = 0; relativePosition.z < DEFAULT_REGION_SIDE; relativePosition.z++) {
    for (relativePosition.x = 0; relativePosition.x < DEFAULT_REGION_SIDE; relativePosition.x++) {
      v3di_t worldPosition;
      worldPosition.x = worldX + relativePosition.x;
      worldPosition.z = worldZ + relativePosition.z;

      int terrainHeight = static_cast<int>(floor (mPeriodics.getTerrainHeight (worldPosition.x,
        worldPosition.z)));

      for (worldPosition.y = lowest; worldPosition.y < terrainHeight; worldPosition.y++) {
        block_t block = mPeriodics.generateBlockAtWorldPosition (worldPosition);

        wc.setBlockAtWorldPosition (worldPosition, block, mPeriodics);
      }

    }
  }*/

//  #pragma omp parallel for
  for (int rz = 0; rz < WORLD_CHUNK_SIDE; rz++) {
    for (int rx = 0; rx < WORLD_CHUNK_SIDE; rx++) {
      v3di_t worldPosition;
      worldPosition.x = worldX + rx;
      worldPosition.z = worldZ + rz;

      int terrainIndex = (rx + 1) + ((rz + 1) * (WORLD_CHUNK_SIDE + 2));
      int terrainHeight = heightMap[terrainIndex];

      for (worldPosition.y = lowest; worldPosition.y < terrainHeight; worldPosition.y++) {
        if (doOutCroppings) {
          block.type = mPeriodics.generateBlockAtWorldPosition(worldPosition);
        }
        else {
          block.type = mPeriodics.generateBlockAtWorldPosition(worldPosition, terrainHeight);
        }
        wc.setBlockAtWorldPosition(worldPosition, block);
      }
    }
  }

  applyOverdrawBlocks(wc);

  return 1;
}

void World::applyOverdrawBlocks(WorldColumn& wc) {
  vector<OverdrawBlock*>* overdrawBlocks = mWorldMap->mOverdrawManager.getBlocks(wc.mWorldIndex.x, wc.mWorldIndex.z);
  if (overdrawBlocks == NULL) {
    return;
  }
  int size = overdrawBlocks->size();
  for (int index = 0; index < size; index++) {
    OverdrawBlock *overdrawBlock = (*overdrawBlocks)[index];

    wc.setBlockType(v3di_v(overdrawBlock->x, overdrawBlock->y, overdrawBlock->z), overdrawBlock->blockType);
  }
  mWorldMap->mOverdrawManager.removeColumn(wc.mWorldIndex.x, wc.mWorldIndex.z);
}

void World::growTree(v3di_t worldPosition, int floorBlockType, int numWaterInColumn) {
//  if (r_numi (0, 10) >= 5) {
//    return;
//  }


  double i = static_cast<double>(worldPosition.x);
  double j = static_cast<double>(worldPosition.z);

  double val1 = mPeriodics.mRandomMap.getValueBilerp(i * 0.313, j * 0.313);
  double val2 = mPeriodics.mRandomMap.getValueBilerp(i * 2.13, j * 2.13);


  double precipitationLevel = mPeriodics.getPrecipitationLevel(worldPosition.x, worldPosition.z);

  if (floorBlockType == BLOCK_TYPE_SAND &&
    precipitationLevel < (DESERT_MOISTURE_LEVEL - 0.01) &&
    val1 < 0.4 &&
    val2 > 0.4 &&
    val2 < 0.5)
  {
    if (numWaterInColumn > 3) {
      growPalmTree(worldPosition);

    }
    else {
      growCactusTree(worldPosition);
    }
  }
  else if (val1 < 0.2 &&
    val2 > 0.6 &&
    val2 < 0.7)
  {
    growSpiralTree (worldPosition);
  }
  else if (precipitationLevel > (DESERT_MOISTURE_LEVEL - 0.03) &&
    val1 > 0.7 &&
    val2 > 0.5 &&
    val2 < 0.6)
  {
    growRandomTree(worldPosition);
//    growBlockTree(worldPosition);
  }


// poorly implemented cypress-like trees (dome generation)
/*
  v3di_t leafPosition;
//  block.type = BLOCK_TYPE_LEAVES;

  double r = r_num (0.0, 1.0);
  if (r < 0.5) {
    block.type = BLOCK_TYPE_DARK_GREEN;
  }
  else {
    block.type = BLOCK_TYPE_SLUDGE;
  }

  block.uniqueLighting = static_cast<GLfloat>(r_num (-0.2, 0.0));

  height = r_numi (1, 6);

  height = 15;

  double radius = r_num (2.0, 3.0);

  for (int j = 0; j < height; j++) {
    for (int i = -5; i <= 5; i++) {
      for (int k = -5; k <= 5; k++) {
        leafPosition.x = position.x + i;
        leafPosition.y = position.y + j;
        leafPosition.z = position.z + k;

        v3d_t a, b;

        a = v3d_v (position);
        b = v3d_v (leafPosition);

        b.y = static_cast<double>(position.y) + (static_cast<double>(j) * 0.2);

        double dist = v3d_dist (a, b);

        if (dist <= radius) {
          setBlockAtPosition (leafPosition, block);
        }

        int columnIndex = pickColumn (leafPosition);

        if (columnIndex > -1) {
          mColumn[columnIndex].mNeedDisplayList = true;
          mColumn[columnIndex].mNeedShadowVolume = true;
        }
      }
    }
  }
*/

}




void World::growBlockTree (v3di_t position) {
  // block trees * * * * * * * * * * * * * * * * * * *
  // -the standard
  int height;

  block_t block;

  position.y++;

  block_t *pBlock = mWorldMap->getBlock (position);
  if (pBlock != NULL &&
    pBlock->type != BLOCK_TYPE_AIR)
  {
    return;
  }

  block.type = BLOCK_TYPE_ALIEN_SKIN2; //BLOCK_TYPE_YELLOW_TRUNK;
  block.uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  height = r_numi (5, 9);

  // lay the trunk
  for (int j = 0; j < height; j++) {
    mWorldMap->setBlock(position, block);
    position.y++;
  }

  // now for the leaves
  block.type = BLOCK_TYPE_MED_BLUE;

  v3di_t leafPosition;
  block.uniqueLighting = 0; //static_cast<GLfloat>(r_num (-0.2, 0.0));

  height = r_numi (1, 6);

  for (int j = 0; j < height; j++) {
    for (int i = -2; i <= 2; i++) {
      for (int k = -2; k <= 2; k++) {
        leafPosition.x = position.x + i;
        leafPosition.y = position.y + j;
        leafPosition.z = position.z + k;
        mWorldMap->setBlock(leafPosition, block);
      }
    }
  }
}




int allowable[] = {
//  BLOCK_TYPE_DIRT,
  BLOCK_TYPE_DIRT_GRASS,
  BLOCK_TYPE_GRASS,
  BLOCK_TYPE_STONE_GRASS,
  BLOCK_TYPE_STONE,
  BLOCK_TYPE_WATER,
  BLOCK_TYPE_SAND,
  BLOCK_TYPE_SNOW,
  BLOCK_TYPE_DIRT_SNOW,
  BLOCK_TYPE_STONE_SNOW,
  BLOCK_TYPE_ALIEN_SKIN,
  BLOCK_TYPE_CACTUS,
  BLOCK_TYPE_LEAVES,
  BLOCK_TYPE_TRUNK,
  BLOCK_TYPE_MARS,
  BLOCK_TYPE_DARK_GREEN,
  BLOCK_TYPE_GRAY,
  BLOCK_TYPE_NAVY_BLUE,
  BLOCK_TYPE_BRICK_RED,
  BLOCK_TYPE_AQUA,
  BLOCK_TYPE_GLASS,
  BLOCK_TYPE_SLUDGE,
  BLOCK_TYPE_WHITE,
  BLOCK_TYPE_MUSTARD,
  BLOCK_TYPE_OBSIDIAN,
  BLOCK_TYPE_DARK_PURPLE,
  BLOCK_TYPE_TAN,
  BLOCK_TYPE_WHITE_MARBLE,
  BLOCK_TYPE_GREEN_STAR_TILE,
  BLOCK_TYPE_OLD_BRICK,
  BLOCK_TYPE_LIGHT_PURPLE,
  BLOCK_TYPE_LIGHT_BROWN,
  BLOCK_TYPE_COPPER,
  BLOCK_TYPE_MED_BLUE,
  BLOCK_TYPE_FUSCHIA,
  BLOCK_TYPE_LIGHT_BLUE,
  BLOCK_TYPE_YELLOW_TRUNK,
  BLOCK_TYPE_MED_STONE,
  BLOCK_TYPE_LIGHT_STONE,
  BLOCK_TYPE_ALIEN_SKIN2,
  BLOCK_TYPE_ALIEN_SKIN3,
  BLOCK_TYPE_CONSOLE,
  BLOCK_TYPE_METAL_TILE_GREY,
  BLOCK_TYPE_METAL_TILE_GREEN,
};




void World::growRandomTree(v3di_t position) {
  position.y++;

  block_t *pBlock = mWorldMap->getBlock(position);
  if (pBlock != NULL &&
    pBlock->type != BLOCK_TYPE_AIR)
  {
    return;
  }

  block_t block;
  block.type = BLOCK_TYPE_TRUNK; //BLOCK_TYPE_YELLOW_TRUNK; //BLOCK_TYPE_ALIEN_SKIN2;
  block.uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  // first the trunk
  int trunkHeight = r_numi(3, 7);

  for (int j = 0; j < trunkHeight; j++) {
    mWorldMap->setBlock(position, block);
    position.y++;
  }

  // now for the top
  int blockType = BLOCK_TYPE_LEAVES; //BLOCK_TYPE_ALIEN_SKIN3;

  v3d_t top;
  top.x = position.x;
  top.z = position.z;
  double bottom = position.y - 1.0;
//    getTerrainHeight (static_cast<int>(floor (top.x)), static_cast<int>(floor (top.z))) - 4.0;

  v3d_t pos;

  double percent;
  double angle;
  int numSteps = 50;

  double sphereRadiusStart = 1.5;
  double sphereRadiusEnd = 1.0;
  double sphereRadius;


  double radius = r_num(1.0, 2.0);
  double totalRotation = r_num(2.0, 4.0) * 1.5;
  double height = r_num(10.0, 20.0);

  top.y = bottom + height;

  BYTE uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  for (int step = 0; step < numSteps; step++) {
    percent = static_cast<double>(step) / static_cast<double>(numSteps - 1);
    angle = percent * totalRotation;

    pos.x = top.x + radius * cos(angle);
    pos.y = lerp(bottom, top.y, percent);
    pos.z = top.z + radius * sin(angle);

    sphereRadius = lerp(sphereRadiusStart, sphereRadiusEnd, percent);

    mWorldMap->fillSphere(pos, sphereRadius, blockType, uniqueLighting);
  }
}









void World::growSpiralTree (v3di_t position) {
  position.y++;

  block_t *pBlock = mWorldMap->getBlock (position);
  if (pBlock != NULL &&
    pBlock->type != BLOCK_TYPE_AIR)
  {
    return;
  }

  int blockType = BLOCK_TYPE_ALIEN_SKIN3;

  v3d_t top;

  top.x = position.x;
  top.z = position.z;
  double bottom = getTerrainHeight (static_cast<int>(floor (top.x)),
    static_cast<int>(floor (top.z))) - 4.0;

  v3d_t pos;

  double percent;
  double angle;
  int numSteps = 50;

  double sphereRadiusStart = 1.5;
  double sphereRadiusEnd = 1.0;
  double sphereRadius;


  double radius = r_num (1.0, 2.0);
  double totalRotation = r_num (2.0, 4.0) * 1.5;
  double height = r_num (10.0, 20.0);

  top.y = bottom + height;

  BYTE uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  for (int step = 0; step < numSteps; step++) {
    percent = static_cast<double> (step) / static_cast<double>(numSteps - 1);
    angle = percent * totalRotation;

    pos.x = top.x + radius * cos (angle);
    pos.y = lerp (bottom, top.y, percent);
    pos.z = top.z + radius * sin (angle);

    sphereRadius = lerp (sphereRadiusStart, sphereRadiusEnd, percent);

    mWorldMap->fillSphere (pos, sphereRadius, blockType, uniqueLighting);
  }
}




void World::growCactusTree (v3di_t position) {
  block_t *pBlock = mWorldMap->getBlock (position);
  if (pBlock != NULL &&
    pBlock->type != BLOCK_TYPE_AIR)
  {
    return;
  }

  block_t block;
  block.type = BLOCK_TYPE_CACTUS;
  block.uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  int height = r_numi (3, 6);

  for (int j = 0; j < height; j++) {
    position.y++;
    mWorldMap->setBlock (position, block);

//    if (position.y & 1) {
//
//    }

  }
}




void World::growPalmTree(v3di_t position) {

  position.y++;

  block_t *pBlock = mWorldMap->getBlock(position);
  if (pBlock != NULL && pBlock->type != BLOCK_TYPE_AIR) {
    return;
  }

  block_t block;
  block.type = BLOCK_TYPE_YELLOW_TRUNK; //BLOCK_TYPE_ALIEN_SKIN2;
  block.uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.3, 0.0));

  int height = r_numi(5, 9);

  // lay the trunk
  for (int j = 0; j < height; j++) {
    mWorldMap->setBlock(position, block);
    position.y++;
  }

  // 'palm' tree
  v3di_t leafPosition;
  block.type = BLOCK_TYPE_ALIEN_SKIN3;
  block.uniqueLighting = LIGHT_LEVEL_MIN; //static_cast<GLfloat>(r_num (-0.2, 0.0));

  leafPosition = position;

  mWorldMap->setBlock (leafPosition, block);

  int spread = r_numi (3, 5);

  // horizontal parts
  for (int i = 1; i < spread; i++) {
    leafPosition = position;
    leafPosition.x += i;
    mWorldMap->setBlock (leafPosition, block);

    leafPosition = position;
    leafPosition.x -= i;
    mWorldMap->setBlock (leafPosition, block);

    leafPosition = position;
    leafPosition.z += i;
    mWorldMap->setBlock (leafPosition, block);

    leafPosition = position;
    leafPosition.z -= i;
    mWorldMap->setBlock (leafPosition, block);
  }

  int drop = r_numi (3, 5);
  spread--;

  // vertical parts
  for (int i = 1; i < drop; i++) {
    leafPosition = position;
    leafPosition.x += spread;
    leafPosition.y -= i;
    mWorldMap->setBlock (leafPosition, block);

    leafPosition = position;
    leafPosition.x -= spread;
    leafPosition.y -= i;
    mWorldMap->setBlock (leafPosition, block);


    leafPosition = position;
    leafPosition.z += spread;
    leafPosition.y -= i;
    mWorldMap->setBlock (leafPosition, block);


    leafPosition = position;
    leafPosition.z -= spread;
    leafPosition.y -= i;
    mWorldMap->setBlock (leafPosition, block);
  }
}

void World::growGroundCover(v3di_t position) {
  BYTE groundType = mWorldMap->getBlockType(position);
  GroundCoverInfo gci = mPeriodics.getGroundCoverInfo(position, groundType);

  if (gci.height <= 0) {
    return;
  }

  block_t block;
  block.type = gci.blockType;
  for (int y = 0; y < gci.height; y++) {
    position.y++;
    mWorldMap->setBlock(position, block);
  }
}



int World::getTerrainHeight(int x, int z) {
  return mPeriodics.getTerrainHeight(x, z);
}




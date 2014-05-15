#include "WorldMap.h"

WorldMap::WorldMap() :
  mXWidth(0),
  mZWidth(0),
  mNumColumns(0),
  mColumns(NULL),
  mPeriodics(NULL)
{
  mWorldLightingFloor = LIGHT_LEVEL_MIN; // + 8;
  mWorldLightingCeiling = LIGHT_LEVEL_MAX - 8;
}


WorldMap::~WorldMap() {
  printf("WorldMap::~WorldMap(): deleting columns\n");
  if (mColumns != NULL) {
    delete [] mColumns;
  }
}


void WorldMap::resize(int xWidth, int zWidth) {
  clear( false );

  if (mColumns != NULL) {
    delete [] mColumns;
    mColumns = NULL;
  }

  mXWidth = xWidth;
  mZWidth = zWidth;

  mNumColumns = mXWidth * mZWidth;

  if (mNumColumns <= 0) {
    mXWidth = mZWidth = 0;
    mNumColumns = 0;
    return;
  }

  mColumns = new WorldColumn[mNumColumns];	

  if (mColumns == NULL) {
    printf("error: WorldMap::resize(): cannot allocate map!");
    mXWidth = mZWidth = 0;
    mNumColumns = 0;
  }
}


void WorldMap::clear(bool clearInactive) {
  if (!clearInactive) {
    saveToInactive();
  }
  for (int i = 0; i < mNumColumns; i++) {
    mColumns[i].clear();
  }
  if (clearInactive) {
    mInactiveColumnManager.clear();
  }

  mChangedList.clear();
}


// this returns the index according to the conversion between world coords
// and the index used to find the right place in the mColumns vector for
// those coords. this has nothing to do with whether or not columns are
// loaded into memory, etc...
// CANNOT (within the limits of the elementary variables) RETURN A 'BAD' VALUE,
// since the length of the index will only ever be: mXWidth*mZWidth (i.e. ~40*40)
int WorldMap::getColumnIndexByRegionCoords(int xIndex, int zIndex) const {
  if (xIndex >= mXWidth) {
    xIndex = xIndex % mXWidth;
  }
  while (xIndex < 0) {
    xIndex += mXWidth;
  }

  if (zIndex >= mZWidth) {
    zIndex = zIndex % mZWidth;
  }
  while (zIndex < 0) {
    zIndex += mZWidth;
  }

  int columnIndex = xIndex + (zIndex * mXWidth);

  return columnIndex;
}


int WorldMap::getColumnIndex(const v3di_t& worldPosition) const {
//	int xIndex = worldPosition.x / WORLD_CHUNK_SIDE;
//	int zIndex = worldPosition.z / WORLD_CHUNK_SIDE;

  // this is really the region index, and should probably be calling WorldUtil::getRegionIndex()
  int xIndex = static_cast<int>(floor(static_cast<double>(worldPosition.x) / static_cast<double>(WORLD_CHUNK_SIDE)));
  int zIndex = static_cast<int>(floor(static_cast<double>(worldPosition.z) / static_cast<double>(WORLD_CHUNK_SIDE)));

  return getColumnIndexByRegionCoords(xIndex, zIndex);
}


int WorldMap::getColumnIndex(const v3d_t& worldPosition) const {
  // this is really the region index, and should probably be calling WorldUtil::getRegionIndex()
  int xIndex = static_cast<int>(floor(worldPosition.x / static_cast<double>(WORLD_CHUNK_SIDE)));
  int zIndex = static_cast<int>(floor(worldPosition.z / static_cast<double>(WORLD_CHUNK_SIDE)));

  return getColumnIndexByRegionCoords(xIndex, zIndex);
}


int WorldMap::pickColumn(const v3di_t& pos) const {
  int columnIndex = getColumnIndex(pos);

  if (mColumns[columnIndex].isInColumn(pos)) {
    return columnIndex;
  }
  return -1;
}


bool WorldMap::isColumnLoaded(int xIndex, int zIndex) const {
  int columnIndex = getColumnIndexByRegionCoords(xIndex, zIndex);
  if (mColumns[columnIndex].mWorldIndex.x == xIndex && mColumns[columnIndex].mWorldIndex.z == zIndex) {
    return true;
  }
  return false;
}


// only clears/saves if it matches the indices
void WorldMap::clearColumn(int xIndex, int zIndex) {
  int columnIndex = getColumnIndexByRegionCoords(xIndex, zIndex);

  if (mColumns[columnIndex].mWorldIndex.x == xIndex && mColumns[columnIndex].mWorldIndex.z == zIndex) {
    clearColumn(columnIndex);
  }
}


// saves if necessary and clears regardless
void WorldMap::clearColumn(size_t columnIndex) {
  // if the column is already loaded, put it to disk
  if (//mColumns[columnIndex].mNeedsToBeSaved &&
    mColumns[columnIndex].mWorldChunks.size () > 0)
  {
    mInactiveColumnManager.saveToInactiveColumns(mColumns[columnIndex]);
  }

  mColumns[columnIndex].clear();
}


int WorldMap::getBlockType(const v3di_t& position) const {
  int column = pickColumn(position);

  if (column == -1) {
    return BLOCK_TYPE_INVALID;
  }
  return mColumns[column].getBlockType(position);
}


void WorldMap::setBlockType(const v3di_t& position, BYTE type) {
  int column = pickColumn(position);

  if (column == -1) {
    mOverdrawManager.setBlock(position, type);
    return;
  }

  if (mColumns[column].getBlockType(position) != type) {
    addToChangedList(column);
  }

  mColumns[column].setBlockType(position, type);
}


void WorldMap::setBlockVisibility(const v3di_t& position, BYTE visibility) {
  int column = pickColumn(position);

  if (column == -1) {
    return;
  }

  mColumns[column].setBlockVisibility(position, visibility);
}


block_t* WorldMap::getBlock(const v3di_t& position) const {
  int column = pickColumn(position);

  if (column == -1) {
    return NULL;
  }

  return mColumns[column].getBlockAtWorldPosition(position);
}


block_t* WorldMap::getBlock(const v3d_t& position) const {
  v3di_t pos = {
    static_cast<int>(floor(position.x)),
    static_cast<int>(floor(position.y)),
    static_cast<int>(floor(position.z))
  };

  return getBlock(pos);
}


int WorldMap::setBlock(const v3di_t& position, const block_t& block) {
  int column = pickColumn(position);

  if (column == -1) {
    mOverdrawManager.setBlock(position, block.type);
    return -1;
  }

  // if we're actually changing to a different block type, make note
  if (mColumns[column].getBlockType(position) != block.type) {
    addToChangedList(column);
  }
  
  mColumns[column].setBlockAtWorldPosition(position, block);

  return 0;
}


void WorldMap::clearBlock(const v3di_t& position) {
  int column = pickColumn(position);

  if (column == -1) {
    mOverdrawManager.setBlock(position, BLOCK_TYPE_AIR);
    return;
  }

  // just get out if there's no change
  block_t *block = mColumns[column].getBlockAtWorldPosition (position);
  if (block != NULL && block->type == BLOCK_TYPE_AIR) {
    return;
  }

  // FIXME: this will (re)generate rocky outcroppings...no good
  // but we need to do it to drill into the ground
  generateChunkContaining(mColumns[column], position);

  mColumns[column].clearBlockAtWorldPosition(position);

  // make note of that change...
  addToChangedList(column);

  // check if some plant is on top
  // FIXME: beware! this is recursive
  v3di_t neighborPosition = position;
  neighborPosition.y += 1;
  if (gBlockData.get(mColumns[column].getBlockType(neighborPosition))->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
    clearBlock(neighborPosition);
  }

  // now deal with the boundary issues
  for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
    neighborPosition = v3di_add(position, gBlockNeighborAddLookup[i]);

    int neighborColumn = pickColumn(neighborPosition);

    if (neighborColumn >= 0) {
      generateChunkContaining(mColumns[neighborColumn], neighborPosition);

      mColumns[neighborColumn].mNeedShadowVolume = true;

      addToChangedList(neighborColumn);
    }
  }
}


BYTE WorldMap::getUniqueLighting(const v3di_t& position) const {
  int column = pickColumn( position );

  if (column == -1) {
    return LIGHT_LEVEL_INVALID;
  }

  return mColumns[column].getUniqueLighting( position );
}


void WorldMap::setUniqueLighting( const v3di_t& position, BYTE level ) {
  int column = pickColumn( position );

  if (column == -1) {
    return;
  }

  mColumns[column].setUniqueLighting(position, level);
}


bool WorldMap::isSolidBlock(const v3di_t& position) const {
  int column = pickColumn(position);

  if (column == -1) {
    return false;
  }

  return mColumns[column].isSolidBlockAtWorldPosition(position);
}


bool WorldMap::isBoundingBoxEmpty( const BoundingBox& boundingBox ) const {
  for (int z = (int)(floor(boundingBox.mNearCorner.z)); z <= (int)(floor(boundingBox.mFarCorner.z)); z++) {
    for (int y = (int)(floor(boundingBox.mNearCorner.y)); y <= (int)(floor(boundingBox.mFarCorner.y)); y++) {
      for (int x = (int)(floor(boundingBox.mNearCorner.x)); x <= (int)(floor(boundingBox.mFarCorner.x)); x++) {
        if (isSolidBlock(v3di_v (x, y, z))) {
          return false;
        }
      }
    }
  }

  return true;
}


bool WorldMap::isBoundingBoxInLiquid(const BoundingBox& boundingBox) const {
  for (int z = (int)(floor(boundingBox.mNearCorner.z)); z <= (int)(floor(boundingBox.mFarCorner.z)); z++) {
    for (int y = (int)(floor(boundingBox.mNearCorner.y)); y <= (int)(floor(boundingBox.mFarCorner.y)); y++) {
      for (int x = (int)(floor(boundingBox.mNearCorner.x)); x <= (int)(floor(boundingBox.mFarCorner.x)); x++) {
        if (gBlockData.get(getBlockType(v3di_v(x, y, z)))->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
          return true;
        }
      }
    }
  }

  return false;
}


double WorldMap::getViscosity(const BoundingBox& boundingBox) const {
  double maxViscosity = 0.0;

  int startX = (int)(floor(boundingBox.mNearCorner.x));
  int endX = (int)(floor(boundingBox.mFarCorner.x));

  int startY = (int)(floor(boundingBox.mNearCorner.y));
  int endY = (int)(floor(boundingBox.mFarCorner.y));

  int startZ = (int)(floor(boundingBox.mNearCorner.z));
  int endZ = (int)(floor(boundingBox.mFarCorner.z));

  for (int z = startZ; z <= endZ; z++) {
    for (int y = startY; y <= endY; y++) {
      for (int x = startX; x <= endX; x++) {
        maxViscosity = max(maxViscosity, gBlockData.get(getBlockType(v3di_v(x, y, z)))->viscosity);
      }
    }
  }

  return maxViscosity;
}


// if boundingBox intersects any blocks with health altering properties
// (e.g. lava, poison, fairy water...) return the sum of damage/healing
double WorldMap::getHealthEffects(const BoundingBox& boundingBox) const {
  double totalHealthEffect = 0.0;

  int startX = (int)(floor(boundingBox.mNearCorner.x));
  int endX = (int)(floor(boundingBox.mFarCorner.x));

  int startY = (int)(floor(boundingBox.mNearCorner.y));
  int endY = (int)(floor(boundingBox.mFarCorner.y));

  int startZ = (int)(floor(boundingBox.mNearCorner.z));
  int endZ = (int)(floor(boundingBox.mFarCorner.z));

  for (int z = startZ; z <= endZ; z++) {
    for (int y = startY; y <= endY; y++) {
      for (int x = startX; x <= endX; x++) {
        totalHealthEffect += gBlockData.get(getBlockType(v3di_v(x, y, z)))->healthEffect;
      }
    }
  }

  return totalHealthEffect;
}


void WorldMap::fillVolume(v3di_t a, v3di_t b, int blockType) {
  if (a.x > b.x) {
    int temp = a.x;
    a.x = b.x;
    b.x = temp;
  }
  if (a.y > b.y) {
    int temp = a.y;
    a.y = b.y;
    b.y = temp;
  }
  if (a.z > b.z) {
    int temp = a.z;
    a.z = b.z;
    b.z = temp;
  }

  v3di_t pos;
  for (pos.z = a.z; pos.z <= b.z; pos.z++) {
    for (pos.y = a.y; pos.y <= b.y; pos.y++) {
      for (pos.x = a.x; pos.x <= b.x; pos.x++) {
        // clear first to ensure we have all the neighbors
        if (gBlockData.get(blockType)->solidityType != BLOCK_SOLIDITY_TYPE_ROCK || gBlockData.get(blockType)->alpha < 1.0) {
          clearBlock(pos);
        }
        setBlockType(pos, blockType);
      }
    }
  }
}


int WorldMap::fillSphere(const v3d_t& pos, double radius, int blockType, BYTE uniqueLighting) {
  BoundingSphere s(pos, radius);

  int blocksFilled = 0;

  v3di_t start = v3di_v(static_cast<int>(pos.x - (radius + 1)),
    static_cast<int>(pos.y - (radius + 1)),
    static_cast<int>(pos.z - (radius + 1)));

  v3di_t stop = v3di_v(static_cast<int>(pos.x + (radius + 1)),
    static_cast<int>(pos.y + (radius + 1)),
    static_cast<int>(pos.z + (radius + 1)));

  v3di_t i;
  block_t block;
  block.type = blockType;
  block.uniqueLighting = uniqueLighting;

  for (i.z = start.z; i.z <= stop.z; i.z++) {
    for (i.y = start.y; i.y <= stop.y; i.y++) {
      for (i.x = start.x; i.x <= stop.x; i.x++) {
        if (s.isPointInside(v3d_v (static_cast<int>(i.x + 0.5),
          static_cast<int>(i.y + 0.5),
          static_cast<int>(i.z + 0.5))))
        {
          setBlock(i, block);
        }
      }
    }
  }


  return blocksFilled;
  //	return 0;
}


int WorldMap::clearSphere(const v3d_t& pos, double radius) {
  BoundingSphere s(pos, radius);

  int blocksCleared = 0;

  v3di_t start = v3di_v(static_cast<int>(pos.x - (radius + 1)),
    static_cast<int>(pos.y - (radius + 1)),
    static_cast<int>(pos.z - (radius + 1)));

  v3di_t stop = v3di_v(static_cast<int>(pos.x + (radius + 1)),
    static_cast<int>(pos.y + (radius + 1)),
    static_cast<int>(pos.z + (radius + 1)));

  v3di_t i;

  for (i.z = start.z; i.z <= stop.z; i.z++) {
    for (i.y = start.y; i.y <= stop.y; i.y++) {
      for (i.x = start.x; i.x <= stop.x; i.x++) {
        if (s.isPointInside (v3d_v (static_cast<int>(i.x + 0.5),
          static_cast<int>(i.y + 0.5),
          static_cast<int>(i.z + 0.5))))
        {
          clearBlock(i);
        }
      }
    }
  }

  return blocksCleared;
  //	return 0;

}


void WorldMap::generateChunkContaining( WorldColumn &worldColumn, const v3di_t& position ) {
  // check to see if it has already been loaded
  if (worldColumn.pickChunkFromWorldHeight (position.y) >= 0) {
    return;
  }

  v3di_t relativePosition;
  v3di_t chunkPosition;

  chunkPosition.x = worldColumn.mWorldPosition.x;
  chunkPosition.y = static_cast<int>(floor (
    static_cast<double>(position.y) / static_cast<double>(WORLD_CHUNK_SIDE))) * WORLD_CHUNK_SIDE;
  chunkPosition.z = worldColumn.mWorldPosition.z;

//	v3di_print("chunk", chunkPosition);
  v3di_t blockPosition;
  block_t block;
  block.type = BLOCK_TYPE_AIR;
  block.faceVisibility = 0;
  block.uniqueLighting = LIGHT_LEVEL_NOT_SET;

  for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++) {
    for (relativePosition.y = 0; relativePosition.y < WORLD_CHUNK_SIDE; relativePosition.y++) {
      for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++) {
        blockPosition = v3di_add (chunkPosition, relativePosition);

        if (mPeriodics != NULL) {
          block.type = mPeriodics->generateBlockAtWorldPosition(blockPosition);
        }

        worldColumn.setBlockAtWorldPosition(blockPosition, block);
      }
    }
  }
}


bool WorldMap::columnHasFourNeighbors(int columnIndex) {
  v3di_t columnPosition = mColumns[columnIndex].mWorldPosition;
  columnPosition.y = 0;
  v3di_t neighborPosition;

  // LEFT
  neighborPosition = v3di_add(columnPosition, v3di_v(-WORLD_CHUNK_SIDE, 0, 0));
  if (pickColumn(neighborPosition) == -1) {
    return false;
  }

  // RIGHT
  neighborPosition = v3di_add(columnPosition, v3di_v(WORLD_CHUNK_SIDE, 0, 0));
  if (pickColumn(neighborPosition) == -1) {
    return false;
  }

  // BACK
  neighborPosition = v3di_add(columnPosition, v3di_v(0, 0, -WORLD_CHUNK_SIDE));
  if (pickColumn(neighborPosition) == -1) {
    return false;
  }

  // FRONT
  neighborPosition = v3di_add(columnPosition, v3di_v(0, 0, WORLD_CHUNK_SIDE));
  if (pickColumn(neighborPosition) == -1) {
    return false;
  }

  return true;
}


void WorldMap::updateBlockVisibility(int columnIndex) {
  v3di_t relativePosition;
  v3di_t worldPosition;
  v3di_t chunkPosition;
  v3di_t neighborPosition;
  v3di_t neighborAdd;

  BYTE type;
  BYTE faceVisibility;
  double blockAlpha;
  int solidityType;
  int neighborType;
  double neighborAlpha;

  for (size_t chunkIndex = 0; chunkIndex < mColumns[columnIndex].mWorldChunks.size(); chunkIndex++) {
    chunkPosition = mColumns[columnIndex].mWorldChunks[chunkIndex]->mWorldPosition;

    for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++) {
      for (relativePosition.y = 0; relativePosition.y < WORLD_CHUNK_SIDE; relativePosition.y++) {
        for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++) {
          worldPosition = v3di_add(chunkPosition, relativePosition);

          type = mColumns[columnIndex].getBlockType(worldPosition);
          blockAlpha = gBlockData.get(type)->alpha;
          solidityType = gBlockData.get(type)->solidityType;

          faceVisibility = 0;

          if (solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
            faceVisibility = 0xff;
            setBlockVisibility(worldPosition, faceVisibility);
          }
          // BLOCK_SOLIDITY_TYPE_GLASS
          else if (solidityType == BLOCK_SOLIDITY_TYPE_GLASS)
          {
            for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
              neighborAdd = gBlockNeighborAddLookup[i];
              neighborPosition = v3di_add(worldPosition, neighborAdd);
              neighborType = getBlockType(neighborPosition);
              neighborAlpha = gBlockData.get(neighborType)->alpha;

              if (neighborType != type &&
                neighborAlpha < 1.0)
              {
                faceVisibility |= gBlockSideBitmaskLookup[i];
              }
            }

            setBlockVisibility (worldPosition, faceVisibility);
          }
          // TRANSLUCENT
          else if (blockAlpha > 0.0 &&
            blockAlpha < 1.0)
          {
            for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
              neighborAdd = gBlockNeighborAddLookup[i];
              neighborPosition = v3di_add(worldPosition, neighborAdd);
              neighborType = getBlockType(neighborPosition);
              neighborAlpha = gBlockData.get(neighborType)->alpha;

              if (neighborType != type &&
                neighborAlpha < 1.0)
              {
                faceVisibility |= gBlockSideBitmaskLookup[i];
              }
            }

            setBlockVisibility(worldPosition, faceVisibility);
          }
          // NON-TRANSLUCENT
          else if (blockAlpha == 1.0) {
            for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
              neighborAdd = gBlockNeighborAddLookup[i];
              neighborPosition = v3di_add (worldPosition, neighborAdd);
              neighborType = getBlockType (neighborPosition);
              neighborAlpha = gBlockData.get(neighborType)->alpha;

              // ha ha...
//							if (neighborType == BLOCK_TYPE_INVALID) {
//								neighborAlpha = 1.0;
//							}

              if (neighborAlpha < 1.0 ||
                gBlockData.get(neighborType)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT ||
                gBlockData.get(neighborType)->solidityType == BLOCK_SOLIDITY_TYPE_GLASS)
              {
                faceVisibility |= gBlockSideBitmaskLookup[i];
              }
            }

            setBlockVisibility (worldPosition, faceVisibility);
          }

          if (faceVisibility == 0) {
            //						mNumHiddenBlocks++;
          }
        }
      }
    }
  }

  mColumns[columnIndex].mNeedShadowVolume = false;
  mColumns[columnIndex].mNeedVisibility = false;
  mColumns[columnIndex].mNeedLightingApplied = true;
  mColumns[columnIndex].mNeedDisplayList = false;
}


void WorldMap::addToChangedList( int columnIndex ) {
  for (size_t i = 0; i < mChangedList.size (); i++) {
    if (mChangedList[i] == columnIndex) {
      return;
    }
  }

//	printf ("added %d\n", columnIndex);
  mChangedList.push_back (columnIndex);
}


void WorldMap::removeFromChangedList( int columnIndex ) {
  for (size_t i = 0; i < mChangedList.size (); i++) {
    if (mChangedList[i] == columnIndex) {
      if (i != mChangedList.size () - 1) {
        swap (mChangedList[i], mChangedList[mChangedList.size () - 1]);
      }

      mChangedList.pop_back ();
//			printf ("removing %d\n", columnIndex);

      return;
    }
  }
}


void WorldMap::updateFluids() {
  vector <int> tempList = mChangedList;

  mChangedList.clear ();

  for (size_t i = 0; i < tempList.size (); i++) {
    updateFluid (tempList[i]);
//		printf ("updating fluid %d, %d\n", i, tempList[i]);
  }
}


bool WorldMap::updateFluid( int columnIndex ) {
  // need four neighbors?

  bool changesMade = false;

  v3di_t worldPosition;

  int highestBlockHeight = mColumns[columnIndex].getHighestBlockHeight ();
  int lowestBlockHeight = mColumns[columnIndex].getLowestBlockHeight ();

  char blockType;

  for (worldPosition.y = highestBlockHeight; worldPosition.y >= lowestBlockHeight; worldPosition.y--) {
    for (int z = 0; z < WORLD_CHUNK_SIDE; z++) {
      worldPosition.z = mColumns[columnIndex].mWorldPosition.z + z;

      for (int x = 0; x < WORLD_CHUNK_SIDE; x++) {
        worldPosition.x = mColumns[columnIndex].mWorldPosition.x + x;

        blockType = getBlockType (worldPosition);

        if (gBlockData.get(blockType)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
          changesMade |= updateLiquidBlock (blockType, columnIndex, worldPosition);
        }
      }
    }
  }

  if (changesMade) {
    // clean up the placeholders
    for (worldPosition.y = lowestBlockHeight; worldPosition.y <= highestBlockHeight; worldPosition.y++) {
      for (int z = -1; z <= WORLD_CHUNK_SIDE; z++) {
        worldPosition.z = mColumns[columnIndex].mWorldPosition.z + z;

        for (int x = -1; x <= WORLD_CHUNK_SIDE; x++) {
          worldPosition.x = mColumns[columnIndex].mWorldPosition.x + x;

          blockType = getBlockType (worldPosition);

          if (blockType >= BLOCK_TYPE_PLACEHOLDER) {
            setBlockType (worldPosition, blockType - BLOCK_TYPE_PLACEHOLDER);
          }
        }
      }
    }
  }

  return changesMade;
}


bool WorldMap::updateLiquidBlock( int blockType, int columnIndex, const v3di_t& worldPosition ) {
  char neighborType;

  char air = BLOCK_TYPE_AIR;
  char liquid = blockType + BLOCK_TYPE_PLACEHOLDER;

  // check relevant neigh-bros
  v3di_t neighborPosition;

  // down first
  neighborPosition = v3di_add (worldPosition, v3di_v (0, -1, 0));
  neighborType = getBlockType (neighborPosition);
  if (neighborType == BLOCK_TYPE_AIR ||
    neighborType == BLOCK_TYPE_INVALID) {
    setBlockType (worldPosition, air);
    setBlockType (neighborPosition, liquid);

    // all done for this time slice
    return true;
  }

  bool changesMade = false;

  int numWaterNeighbors = countLiquidNeighbors( blockType, worldPosition );

  if (numWaterNeighbors < 2) {
    setBlockType (worldPosition, air);

    // all done for this time slice
    return true;
  }
  else {
    // -x
    neighborPosition = v3di_add (worldPosition, v3di_v (-1, 0, 0));
    neighborType = getBlockType (neighborPosition);
    if (neighborType == BLOCK_TYPE_AIR) {
      setBlockType (neighborPosition, liquid);
      changesMade = true;
    }

    // +x
    neighborPosition = v3di_add (worldPosition, v3di_v (+1, 0, 0));
    neighborType = getBlockType (neighborPosition);
    if (neighborType == BLOCK_TYPE_AIR) {
      setBlockType (neighborPosition, liquid);
      changesMade = true;
    }

    // -z
    neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, -1));
    neighborType = getBlockType (neighborPosition);
    if (neighborType == BLOCK_TYPE_AIR) {
      setBlockType (neighborPosition, liquid);
      changesMade = true;
    }

    // +z
    neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, +1));
    neighborType = getBlockType (neighborPosition);
    if (neighborType == BLOCK_TYPE_AIR) {
      setBlockType (neighborPosition, liquid);
      changesMade = true;
    }
  }

  return changesMade;
}


int WorldMap::countLiquidNeighbors( int blockType, const v3di_t& worldPosition ) const {
  int numWaterNeighbors = 0;

  // -x
  v3di_t neighborPosition = v3di_add (worldPosition, v3di_v (-1, 0, 0));
  char neighborType = getBlockType (neighborPosition);
  if (neighborType == blockType) {
    numWaterNeighbors++;

    neighborPosition = v3di_add (worldPosition, v3di_v (-2, 0, 0));
    char neighborType = getBlockType (neighborPosition);
    if (neighborType == blockType) {
      numWaterNeighbors++;
    }
  }

  // +x
  neighborPosition = v3di_add (worldPosition, v3di_v (+1, 0, 0));
  neighborType = getBlockType (neighborPosition);
  if (neighborType == blockType) {
    numWaterNeighbors++;

    neighborPosition = v3di_add (worldPosition, v3di_v (+2, 0, 0));
    char neighborType = getBlockType (neighborPosition);
    if (neighborType == blockType) {
      numWaterNeighbors++;
    }
  }

  // -y
//	neighborPosition = v3di_add (worldPosition, v3di_v (0, -1, 0));
//	neighborType = getBlockType (neighborPosition);
//	if (neighborType == blockType) {
//		numWaterNeighbors++;
//	}

  // +y
  neighborPosition = v3di_add (worldPosition, v3di_v (0, +1, 0));
  neighborType = getBlockType (neighborPosition);
  if (neighborType == blockType) {
    numWaterNeighbors += 2;

    neighborPosition = v3di_add (worldPosition, v3di_v (0, +2, 0));
    char neighborType = getBlockType (neighborPosition);
    if (neighborType == blockType) {
      numWaterNeighbors++;
    }
  }

  // -z
  neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, -1));
  neighborType = getBlockType (neighborPosition);
  if (neighborType == blockType) {
    numWaterNeighbors += 1;

    neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, -2));
    char neighborType = getBlockType (neighborPosition);
    if (neighborType == blockType) {
      numWaterNeighbors++;
    }
  }

  // +z
  neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, +1));
  neighborType = getBlockType (neighborPosition);
  if (neighborType == blockType) {
    numWaterNeighbors++;

    neighborPosition = v3di_add (worldPosition, v3di_v (0, 0, +2));
    char neighborType = getBlockType (neighborPosition);
    if (neighborType == blockType) {
      numWaterNeighbors++;
    }
  }

  return numWaterNeighbors;
}


int WorldMap::save(FILE *file) {
  printf("saving WorldMap...\n");

  int errorCode = 0;

  // update the inactive columns
  swapOutToInactive();

  // save the inactive columns to disk
  printf("- saving InactiveColumnManager...\n");
  errorCode = mInactiveColumnManager.saveToDisk(file);
  if (errorCode != 0) {
    fclose(file);
    return errorCode;
  }
  mInactiveColumnManager.clear();

  printf("- saving OverdrawManager...\n");
  mOverdrawManager.save(file);
  mOverdrawManager.clear();

  printf ("WorldMap::saveToDisk(): complete\n");
  return errorCode;
}


int WorldMap::load(FILE *file) {
  printf ("loading WorldMap:\n");

  // clear everything first
  clear(true);

  int errorCode = 0;

  // load the columns
  printf("- loading InactiveColumnManager...\n");
  errorCode = mInactiveColumnManager.loadFromDisk(file);
  printf("- (cols:%d)\n", mInactiveColumnManager.getNumColumns());

  printf("- loading OverdrawManager...\n");
  mOverdrawManager.load(file);

  printf("WorldMap::loadFromDisk(): complete\n");
  return errorCode;
}


void WorldMap::swapOutToInactive() {
  printf("WorldMap::swapOutToInactive()\n");
  saveToInactive();
  printf("A\n");
  for (int i = 0; i < mNumColumns; i++) {
    mColumns[i].clear();
  }
  printf("B\n");
  mChangedList.clear();
  printf("D\n");
}


void WorldMap::saveToInactive() {
  for (int column = 0; column < mNumColumns; column++) {
    if (mColumns[column].mWorldChunks.size () > 0) {
      mInactiveColumnManager.saveToInactiveColumns(mColumns[column]);
    }
  }
}


bool WorldMap::lineOfSight(const v3d_t &a, const v3d_t &b) const {
  v3d_t diff = v3d_sub(b, a);

  int start;
  int end;
  int add;

  // x-axis
  if (diff.x != 0.0) { // && (floor (a.x) != floor (b.x))) {
    int direction = static_cast<int>(sign (diff.x));

    if (direction > 0) {
      start = static_cast<int>(ceil (a.x));
      end = static_cast<int>(floor (b.x));
      add = 1;
    }
    else {
      start = static_cast<int>(floor (a.x));
      end = static_cast<int>(ceil (b.x));
      add = -1;
    }

    for (int x = start; x != (end + add); x += add) {
      double percent = (static_cast<double>(x) - static_cast<double>(a.x)) /
        (static_cast<double>(b.x) - static_cast<double>(a.x));

      v3d_t pos = v3d_interpolate(a, b, percent);

      v3di_t blockPos;
      if (add > 0) {
        blockPos.x = x;
      }
      else {
        blockPos.x = x + add;
      }
      blockPos.y = static_cast<int>(floor(pos.y));
      blockPos.z = static_cast<int>(floor(pos.z));

      if (isSolidBlock(blockPos)) {
        return false;
      }
    }
  }

  // y-axis
  if (diff.y != 0.0) { // && (floor (a.y) != floor (b.y))) {
    int direction = static_cast<int>(sign (diff.y));

    if (direction > 0) {
      start = static_cast<int>(ceil (a.y));
      end = static_cast<int>(floor (b.y));
      add = 1;
    }
    else {
      start = static_cast<int>(floor (a.y));
      end = static_cast<int>(ceil (b.y));
      add = -1;
    }

    for (int y = start; y != (end + add); y += add) {
      double percent = (static_cast<double>(y) - static_cast<double>(a.y)) /
        (static_cast<double>(b.y) - static_cast<double>(a.y));

      v3d_t pos = v3d_interpolate (a, b, percent);

      v3di_t blockPos;
      blockPos.x = static_cast<int>(floor (pos.x));
      if (add > 0) {
        blockPos.y = y;
      }
      else {
        blockPos.y = y + add;
      }
      blockPos.z = static_cast<int>(floor (pos.z));

      if (isSolidBlock (blockPos)) {
        return false;
      }
    }
  }

  // z-axis
  if (diff.z != 0.0) { // && (floor (a.z) != floor (b.z))) {
    int direction = static_cast<int>(sign (diff.z));

    if (direction > 0) {
      start = static_cast<int>(ceil (a.z));
      end = static_cast<int>(floor (b.z));
      add = 1;
    }
    else {
      start = static_cast<int>(floor (a.z));
      end = static_cast<int>(ceil (b.z));
      add = -1;
    }

    for (int z = start; z != (end + add); z += add) {
      double percent = (static_cast<double>(z) - static_cast<double>(a.z)) /
        (static_cast<double>(b.z) - static_cast<double>(a.z));

      v3d_t pos = v3d_interpolate (a, b, percent);

      v3di_t blockPos;
      blockPos.x = static_cast<int>(floor (pos.x));
      blockPos.y = static_cast<int>(floor (pos.y));
      if (add > 0) {
        blockPos.z = z;
      }
      else {
        blockPos.z = z + add;
      }

      if (isSolidBlock (blockPos)) {
        return false;
      }
    }
  }

  return true;
}


bool WorldMap::rayCastSolidBlock(const v3d_t &a, const v3d_t &b, v3di_t &hitPosition, int &face) const {
  v3d_t diff = v3d_sub(b, a);

  double magnitude = v3d_mag(diff);
  double step = 1.0 / (10.0 * magnitude);

  v3di_t blockPosition;
  v3d_t pos;
  v3d_t lastPos = a;

static v3d_t faceAdd[6] = {
    {0.0, 0.0, 0.0},		// BLOCK_SIDE_LEF
    {1.0, 0.0, 0.0},		// BLOCK_SIDE_RIG
    {0.0, 1.0, 0.0},		// BLOCK_SIDE_TOP
    {0.0, 0.0, 0.0},		// BLOCK_SIDE_BOT
    {0.0, 0.0, 1.0},		// BLOCK_SIDE_FRO
    {0.0, 0.0, 0.0}			// BLOCK_SIDE_BAC
  };

  for (double t = step; t <= 1.0; t += step) {
    pos = v3d_add (a, v3d_scale (t, diff));

    // TODO: check if this is the same v3di_t that was checked last frame
    blockPosition = v3di_v (pos);
    block_t *block = getBlock(blockPosition);

    // special case for plants...
    if (block != NULL && gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
      hitPosition = blockPosition;
      face = 0;
      return true;
    }

    if (isSolidBlock(blockPosition)) {
      // the return value gets set
      hitPosition = blockPosition;

      // figure out which face was intersected first

      double faceHitResults[NUM_BLOCK_SIDES];
      double faceHitTimes[NUM_BLOCK_SIDES];

      for (int s = 0; s < NUM_BLOCK_SIDES; s++) {
        Plane3d plane;

        v3d_t point = v3d_add(v3d_v(blockPosition), faceAdd[s]);
        v3d_t normal = {
          gCubeFaceNormalLookup[s][0],
          gCubeFaceNormalLookup[s][1],
          gCubeFaceNormalLookup[s][2] };

        plane.setFromPointAndNormal(point, normal);

        faceHitResults[s] = plane.doesLineSegmentIntersect(lastPos, pos, faceHitTimes[s]);
      }

      double soonestHit = 1000.0;		// super high value

      // this section sets the 'face' variable to the nearest face
      for (int s = 0; s < NUM_BLOCK_SIDES; s++) {
        if (faceHitResults[s]) {
          if (faceHitTimes[s] > 0.0 && faceHitTimes[s] < soonestHit) {
            soonestHit = faceHitTimes[s];
            face = s;
          }
        }
      }

      return true;
    }

    lastPos = pos;
  }

  return false;
}



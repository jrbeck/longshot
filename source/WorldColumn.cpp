#include "WorldColumn.h"

WorldColumn::WorldColumn() {
  clear();
}

WorldColumn::~WorldColumn() {
  size_t numWorldChunks = mWorldChunks.size();
  for (size_t i = 0; i < numWorldChunks; i++) {
    if (mWorldChunks[i] != 0) {
      delete mWorldChunks[i];
    }
  }
}

void WorldColumn::clear() {
  size_t numWorldChunks = mWorldChunks.size();
  for (size_t i = 0; i < numWorldChunks; i++) {
    if( mWorldChunks[i] != 0 ) {
      delete mWorldChunks[i];
    }
  }
  mWorldChunks.clear();

  mWorldIndex.x = 1000000;
  mWorldIndex.y = 1000000;
  mWorldIndex.z = 1000000;

  mWorldPosition.x = 1000000;
  mWorldPosition.y = 1000000;
  mWorldPosition.z = 1000000;

  mLowestBlock = 1000000;
  mHighestBlock = -1000000;

  mNumUnderWater = 0;

  mNeedShadowVolume = false;
  mNeedLightingApplied = false;
  mNeedVisibility = false;
  mNeedDisplayList = false;

//  mNeedsToBeSaved = false;
}

void WorldColumn::clearEmptyChunks() {
  bool hasClearedChunk = false;
  size_t numWorldChunks = mWorldChunks.size();
  size_t lastChunkIndex = numWorldChunks - 1;
  for (size_t i = 0; i < numWorldChunks; i++) {
    if (mWorldChunks[i]->mNumBlocks == 0) {
      if (i != lastChunkIndex) {
        swap(mWorldChunks[i], mWorldChunks[lastChunkIndex]);
        i--;
      }

      // WARNING: unchecked delete...would crash most likely
      // on the previous access, however
      delete mWorldChunks[lastChunkIndex];
      mWorldChunks.pop_back();

      numWorldChunks--;
      lastChunkIndex--;
      hasClearedChunk = true;
    }
  }

  if (hasClearedChunk) {
    updateHighestAndLowest();
  }
}

bool WorldColumn::isInColumn(const v3di_t& worldPosition) const {
  if ((worldPosition.x < mWorldPosition.x) ||
    (worldPosition.x >= (mWorldPosition.x + WORLD_CHUNK_SIDE)) ||
    (worldPosition.z < mWorldPosition.z) ||
    (worldPosition.z >= (mWorldPosition.z + WORLD_CHUNK_SIDE)))
  {
    return false;
  }

  return true;
}

int WorldColumn::pickChunkFromWorldHeight(int height) const {
  for (int i = 0; i < static_cast<int>(mWorldChunks.size()); i++) {
    if (height >= mWorldChunks[i]->mWorldPosition.y &&
      height < (mWorldChunks[i]->mWorldPosition.y + WORLD_CHUNK_SIDE))
    {
        return i;
    }
  }

  return -1;
}

int WorldColumn::getBlockType(const v3di_t& worldPosition) const {
  int chunkIndex = pickChunkFromWorldHeight(worldPosition.y);

  if (chunkIndex >= 0) {
    return mWorldChunks[chunkIndex]->getBlockType(worldPosition);
  }

  return BLOCK_TYPE_INVALID;
}

void WorldColumn::setBlockType(const v3di_t& worldPosition, char type) {
  int chunkIndex = pickChunkFromWorldHeight(worldPosition.y);

  if (chunkIndex < 0) {
    chunkIndex = createChunkContaining(worldPosition);
  }

  mWorldChunks[chunkIndex]->setBlockType(worldPosition, type);

  mNeedShadowVolume = true;
}

block_t* WorldColumn::getBlockAtWorldPosition(const v3di_t& position) const {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex >= 0) {
    return mWorldChunks[chunkIndex]->getBlockAtWorldPosition(position);
  }

  return NULL;
}

void WorldColumn::setBlockAtWorldPosition(const v3di_t& position, const block_t& block) {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex < 0) {
    chunkIndex = createChunkContaining(position);
  }

  mWorldChunks[chunkIndex]->setBlockAtWorldPosition(position, block);

  mNeedShadowVolume = true;
}

BYTE WorldColumn::getUniqueLighting(const v3di_t& position) const {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex >= 0) {
    return mWorldChunks[chunkIndex]->getUniqueLighting(position);
  }

  return LIGHT_LEVEL_INVALID;
}

void WorldColumn::setUniqueLighting(const v3di_t& position, BYTE level ) {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex >= 0) {
    mWorldChunks[chunkIndex]->setUniqueLighting(position, level);
    mNeedDisplayList = false;
  }
}

void WorldColumn::setBlockVisibility(const v3di_t& position, BYTE visibility ) {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex >= 0) {
    mWorldChunks[chunkIndex]->setBlockVisibility(position, visibility);
  }
}

bool WorldColumn::isSolidBlockAtWorldPosition(const v3di_t& position ) const {
  int chunkIndex = pickChunkFromWorldHeight(position.y);

  if (chunkIndex >= 0) {
    return mWorldChunks[chunkIndex]->isSolidBlockAtWorldPosition(position);
  }

  return false;
}

void WorldColumn::clearBlockAtWorldPosition(const v3di_t& position ) {
  block_t block;

  block.type = BLOCK_TYPE_AIR;
  block.uniqueLighting = LIGHT_LEVEL_NOT_SET;
  block.faceVisibility = 0;

  setBlockAtWorldPosition(position, block);
}

int WorldColumn::createChunkContaining(const v3di_t& worldPosition ) {
  int chunkIndex = pickChunkFromWorldHeight(worldPosition.y);

  if (chunkIndex >= 0 ) {
    // WARNING: this behavior is undefined
    printf( "WorldColumn::createChunkContaining: warning: chunk already exists (%d)\n", chunkIndex );
    return chunkIndex;
  }

//  printf("WorldColumn::generateEmptyChunkContaining: generating new empty chunk\n");
  v3di_t worldIndex;

  worldIndex.x = mWorldIndex.x;
  worldIndex.y = static_cast<int>( floor (
    static_cast<double>( worldPosition.y ) / static_cast<double>( WORLD_CHUNK_SIDE ) ) );
  worldIndex.z = mWorldIndex.z;

//  printf("num chunks in column: %d\n", mWorldChunks.size());
//  v3di_print ("empty world index", worldIndex);

  WorldChunk* chunk = new WorldChunk;
  chunk->clear();
  chunk->setWorldPosition(v3di_scale(WORLD_CHUNK_SIDE, worldIndex));
  chunk->mWorldIndex = worldIndex;
  mWorldChunks.push_back(chunk);

  updateHighestAndLowest();

  return (static_cast<int>(mWorldChunks.size()) - 1);
}

int WorldColumn::getHighestBlockHeight() const {
  return mHighestBlock;
}

int WorldColumn::getLowestBlockHeight() const {
  return mLowestBlock;
}

// this is called whenever a chunk is added or removed
void WorldColumn::updateHighestAndLowest() {
  if (mWorldChunks.size() == 0) {
    printf("WorldColumn::updateHighestAndLowest() : no chunks\n");
    return;
  }

  // figure out the highest and lowest
  // WARNING: these numbers may not be sufficient for your needs!
  int highest = -1000000;
  int lowest = 1000000;

  for (size_t i = 0; i < mWorldChunks.size(); i++) {
    if (mWorldChunks[i]->mWorldPosition.y > highest) {
      highest = mWorldChunks[i]->mWorldPosition.y;
    }

    if (mWorldChunks[i]->mWorldPosition.y < lowest) {
      lowest = mWorldChunks[i]->mWorldPosition.y;
    }
  }

  mHighestBlock = highest + (WORLD_CHUNK_SIDE - 1);
  mLowestBlock = lowest;
}

#include "WorldChunk.h"



WorldChunk::WorldChunk() :
  mNumBlocks( 0 ),
  mNumWaterBlocks( 0 ),
  mNumHiddenBlocks( 0 ),
  mWasModified( true )
{
  setWorldPosition( v3di_v( 0, 0, 0 ) );

  mBlocks = new block_t[WORLD_CHUNK_SIDE_CUBED];
  if (mBlocks == NULL) {
    printf ("WorldChunk::WorldChunk(): error: could not allocate memory!\n");
  }
}


// destructor
WorldChunk::~WorldChunk() {
  if (mBlocks != NULL) {
    delete [] mBlocks;
  }
}


void WorldChunk::setWorldPosition( const v3di_t& worldPosition ) {
  mWorldPosition = worldPosition;

  // set the new bounding sphere
  double halfSideLength = static_cast<double>(WORLD_CHUNK_SIDE) * 0.5;
  v3d_t worldPos = v3d_add (v3d_v (halfSideLength, halfSideLength, halfSideLength),
    v3d_v (mWorldPosition));

  mBoundingSphere.setPosition (worldPos);
  mBoundingSphere.setRadius (1.7322 * halfSideLength);
}



void WorldChunk::clear() {
//  if (mBlocks != NULL) {
//    memset (mBlocks, 0, sizeof (*mBlocks) * mSideLength * mSideLengthSquared);
    for (int i = 0; i < WORLD_CHUNK_SIDE_CUBED; i++) {
      mBlocks[i].type = BLOCK_TYPE_AIR;
      mBlocks[i].uniqueLighting = LIGHT_LEVEL_NOT_SET;
    }
//  }

  mNumBlocks = 0;
  mNumWaterBlocks = 0;
  mNumHiddenBlocks = 0;

  mWasModified = true;
}


const block_t* WorldChunk::getBlocks() const {
  return mBlocks;
}


bool WorldChunk::isInRegion( const v3di_t& position ) const {
  if (position.x < mWorldPosition.x ||
    position.x >= (mWorldPosition.x + WORLD_CHUNK_SIDE) ||
    position.y < mWorldPosition.y ||
    position.y >= (mWorldPosition.y + WORLD_CHUNK_SIDE) ||
    position.z < mWorldPosition.z ||
    position.z >= (mWorldPosition.z + WORLD_CHUNK_SIDE))
  {
    return false;
  }

  return true;
}


void WorldChunk::setBlockType( const v3di_t& worldPosition, char type ) {
  setBlockAtRelativePosition( v3di_sub( worldPosition, mWorldPosition ), type );
}


void WorldChunk::setBlockAtWorldPosition( const v3di_t& worldPosition, const block_t& block ) {
  setBlockAtRelativePosition( v3di_sub( worldPosition, mWorldPosition ), block );
}


BYTE WorldChunk::getUniqueLighting( const v3di_t& worldPosition ) const {
  v3di_t relativePosition = v3di_sub( worldPosition, mWorldPosition );

  // check to see if the pos is in range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return LIGHT_LEVEL_INVALID;
  }

  size_t blockIndex = relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED);

  if (mBlocks[blockIndex].type > BLOCK_TYPE_AIR &&
    gBlockData.get(mBlocks[blockIndex].type)->solidityType != BLOCK_SOLIDITY_TYPE_LIQUID) {
    return LIGHT_LEVEL_SOLID;
  }

  return mBlocks[blockIndex].uniqueLighting;
}


void WorldChunk::setUniqueLighting( const v3di_t& worldPosition, BYTE level ) {
  v3di_t relativePosition = v3di_sub (worldPosition, mWorldPosition);

  // check to see if the pos is in range, return if out of range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return;
  }

  size_t blockIndex = relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED);

  // set the new block
  mBlocks[blockIndex].uniqueLighting = level;
}


void WorldChunk::setBlockVisibility( const v3di_t& worldPosition, BYTE visibility ) {
  v3di_t relativePosition = v3di_sub (worldPosition, mWorldPosition);

  // check to see if the pos is in range, return if out of range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return;
  }

  size_t blockIndex = relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED);

  // set the new block
  mBlocks[blockIndex].faceVisibility = visibility;
}


void WorldChunk::setBlockAtRelativePosition( const v3di_t& relativePosition, char type ) {
  // check to see if the pos is in range, return -1 if out of range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return;
  }

  size_t blockIndex = relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED);

  // we have to keep the block count current
  updateBlockCounts (mBlocks[blockIndex].type, type);

  // set the new block
  mBlocks[blockIndex].type = type;
  mBlocks[blockIndex].uniqueLighting = LIGHT_LEVEL_NOT_SET;

  mWasModified = true;
}


void WorldChunk::setBlockAtRelativePosition( const v3di_t& relativePosition, const block_t& block ) {
  // check to see if the pos is in range, return -1 if out of range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return;
  }

  size_t blockIndex = relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED);

  // we have to keep the block count current
  updateBlockCounts (mBlocks[blockIndex].type, block.type);

  // set the new block
  mBlocks[blockIndex] = block;

  mWasModified = true;
}


// this function is called whenever a block changes type
// it ensures that solid/water/... counts are maintained
void WorldChunk::updateBlockCounts( char oldType, char newType ) {
  int oldSolidity = gBlockData.get(oldType)->solidityType;
  int newSolidity = gBlockData.get(newType)->solidityType;


  bool wasSolid;
  if (oldSolidity != BLOCK_SOLIDITY_TYPE_AIR) {
    wasSolid = true;
  }
  else {
    wasSolid = false;
  }

  if (!wasSolid && newSolidity != BLOCK_SOLIDITY_TYPE_AIR) {
    mNumBlocks++;
  }
  else if (wasSolid && newSolidity == BLOCK_SOLIDITY_TYPE_AIR) {
    mNumBlocks--;
  }

  bool wasWater;
  if (oldSolidity == BLOCK_SOLIDITY_TYPE_LIQUID) {
    wasWater = true;
  }
  else {
    wasWater = false;
  }

  if (!wasWater && newSolidity == BLOCK_SOLIDITY_TYPE_LIQUID) {
    mNumWaterBlocks++;
  }
  else if (wasWater && newSolidity != BLOCK_SOLIDITY_TYPE_LIQUID) {
    mNumWaterBlocks--;
  }
}


// get the block at a position if it exists
block_t *WorldChunk::blockAt( const v3di_t& relativePosition ) const {
  // check to see if the pos is in range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return NULL;
  }

  return &mBlocks[relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED)];
}


int WorldChunk::getBlockType( const v3di_t& worldPosition ) const {
  v3di_t relativePosition = v3di_sub (worldPosition, mWorldPosition);

  // check to see if the pos is in range
  if (relativePosition.x < 0 || relativePosition.x >= WORLD_CHUNK_SIDE ||
    relativePosition.y < 0 || relativePosition.y >= WORLD_CHUNK_SIDE ||
    relativePosition.z < 0 || relativePosition.z >= WORLD_CHUNK_SIDE)
  {
    return BLOCK_TYPE_INVALID;
  }

  return mBlocks[relativePosition.x + (relativePosition.y * WORLD_CHUNK_SIDE) +
    (relativePosition.z * WORLD_CHUNK_SIDE_SQUARED)].type;
}


block_t* WorldChunk::getBlockAtWorldPosition( const v3di_t &position ) const {
  return blockAt( v3di_sub ( position, mWorldPosition ) );
}


void WorldChunk::setBlockByIndex( size_t index, const block_t& block ) {
  mBlocks[index] = block;
}


void WorldChunk::setBlockTypeByIndex( size_t index, BYTE type ) {
  mBlocks[index].type = type;
  mBlocks[index].uniqueLighting = LIGHT_LEVEL_NOT_SET;
}


block_t WorldChunk::getBlockByIndex( size_t index ) const {
  return mBlocks[index];
}


BYTE WorldChunk::getBlockTypeByIndex (size_t index) const {
  return mBlocks[index].type;
}


bool WorldChunk::isSolidBlockAtWorldPosition( const v3di_t& worldPosition ) {
  int blockType = getBlockType(worldPosition);
  int solidityType = gBlockData.get(blockType)->solidityType;

  if (solidityType == BLOCK_SOLIDITY_TYPE_LIQUID ||
    solidityType == BLOCK_SOLIDITY_TYPE_AIR ||
    solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
    return false;
  }

  return true;
}



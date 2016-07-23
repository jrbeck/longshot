// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldChunk
// *
// * leaf of the World->WorldMap->WorldColumn->WorldChunk hierarchy
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cmath>

#include "../math/GlCamera.h"

#include "../math/v3d.h"

#include "../math/Periodics.h"
#include "../math/BoundingBox.h"
#include "../math/BoundingSphere.h"
#include "../assets/AssetManager.h"


// WorldChunk * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class WorldChunk {
public:

  WorldChunk();
  ~WorldChunk();

  void setWorldPosition( const v3di_t& worldPosition );

  void clear();

  const block_t* getBlocks() const;

  int getBlockType( const v3di_t& worldPosition ) const;
  void setBlockType( const v3di_t& worldPosition, char type );
  void setBlockAtWorldPosition( const v3di_t& worldPosition, const block_t&block );

  BYTE getUniqueLighting( const v3di_t& worldPosition ) const;
  void setUniqueLighting( const v3di_t& worldPosition, BYTE level );

  void setBlockVisibility( const v3di_t& worldPosition, BYTE visibility );

  void setBlockAtRelativePosition( const v3di_t& relativePosition, char type );
  void setBlockAtRelativePosition( const v3di_t& relativePosition, const block_t& block );

  void updateBlockCounts( char oldType, char newType );

  bool isInRegion( const v3di_t& position ) const;

  block_t* getBlockAtWorldPosition( const v3di_t& position ) const;

  void setBlockByIndex( size_t index, const block_t& block );
  void setBlockTypeByIndex( size_t index, BYTE type );
  block_t getBlockByIndex( size_t index ) const;
  BYTE getBlockTypeByIndex( size_t index ) const;

  bool isSolidBlockAtWorldPosition( const v3di_t& position );


// public members
  v3di_t mWorldIndex;
  v3di_t mWorldPosition;

  bool mWasModified;

  int mNumBlocks;
  int mNumWaterBlocks;

  BoundingSphere mBoundingSphere;

private:
  // copy constructor guard
  WorldChunk( const WorldChunk& worldChunk ) { }
  // assignment operator guard
  WorldChunk& operator=( const WorldChunk& worldChunk ) { return *this; }

  block_t* blockAt( const v3di_t& relativePosition ) const;

// members * * * * * * * * * * * * * * * * * * * * * * * * * *

  block_t* mBlocks;

  int mNumHiddenBlocks;
};

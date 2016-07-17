// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldColumn
// *
// * part of the World->WorldMap->WorldColumn->WorldChunk hierarchy
// *
// * the world column is a stack of WorldChunks that represents the entire volume
// * of the world for a particular area.
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef WorldColumn_h_
#define WorldColumn_h_

//#include <Windows.h>
//#include <direct.h>

#include "v3d.h"

#include "WorldChunk.h"


typedef struct {
  v3di_t worldIndex;
  v3di_t worldPosition;

  int lowestBlock;
  int highestBlock;

  int numChunks;
} world_column_info_t;


typedef struct {
  v3di_t worldIndex;
  v3di_t worldPosition;

  int numBlocks;
  int numWaterBlocks;
} world_chunk_info_t;



class WorldColumn {
private:
  // copy constructor guard
  WorldColumn(const WorldColumn& worldColumn) { }
  // assignment operator guard
  WorldColumn& operator=(const WorldColumn& worldColumn) { return *this; }

public:
  WorldColumn();
  ~WorldColumn();

  void clear();

  void clearEmptyChunks();

  bool isInColumn(const v3di_t& worldPosition) const;
  int pickChunkFromWorldHeight(int height) const;

  int getBlockType(const v3di_t& worldPosition) const;
  void setBlockType(const v3di_t& worldPosition, char type);

  block_t* getBlockAtWorldPosition(const v3di_t& position) const;
  void setBlockAtWorldPosition(const v3di_t& position, const block_t& block);

  BYTE getUniqueLighting(const v3di_t& position) const;
  void setUniqueLighting(const v3di_t& position, BYTE level);

  void setBlockVisibility(const v3di_t& position, BYTE visibility);

  bool isSolidBlockAtWorldPosition(const v3di_t& position) const;

  void clearBlockAtWorldPosition(const v3di_t& position);

  int createChunkContaining(const v3di_t& worldPosition);

  void updateHighestAndLowest();
  int getHighestBlockHeight() const;
  int getLowestBlockHeight() const;

// public members * * * * * * * * * * * * * * *
  v3di_t mWorldIndex;
  v3di_t mWorldPosition;

  bool mNeedShadowVolume;
  bool mNeedLightingApplied;
  bool mNeedVisibility;
  bool mNeedDisplayList;

//  bool mNeedsToBeSaved;

  int mLowestBlock;
  int mHighestBlock;

  int mNumUnderWater;

  vector<WorldChunk*> mWorldChunks;
};



#endif // WorldColumn_h_

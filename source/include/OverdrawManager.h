// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * OverdrawManager.h
// *
// * desc: this is what handles blocks that are draw 'off-camera' so to speak.
// * this allows for:
// * * trees to be draw that span into non-generated columns
// * * features to be drawn on inactive planets (dynamic quests anyone?)
// * * canyons, rivers, cave systems without worry of pre-generating anything
// * * etc...
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "v3d.h"

#include "AssetManager.h"
#include "BlockTypeData.h"
#include "WorldUtil.h"


struct OverdrawBlock {
  int x;
  int y;
  int z;
  BYTE blockType;
};


struct OverdrawColumn {
  int regionIndexX;
  int regionIndexZ;
  vector<OverdrawBlock*> overdrawBlocks;
};


class OverdrawManager {
private:
  // copy constructor guard
  OverdrawManager(const OverdrawManager& overdrawManager) { }
  // assignment operator guard
  OverdrawManager& operator=(const OverdrawManager& overdrawManager) { return *this; }

public:
  OverdrawManager();
  ~OverdrawManager();

  void setBlock(const v3di_t& position, BYTE blockType);
  vector<OverdrawBlock*> *getBlocks(int regionIndexX, int regionIndexZ);
  void clearBlocks(OverdrawColumn* overdrawColumn);

  void removeColumn(int regionIndexX, int regionIndexZ);
  void removeColumn(OverdrawColumn* overdrawColum);

  void clear();

  void save(FILE* file);
  void load(FILE* file);

private:

  OverdrawColumn* getOverdrawColumn(int regionIndexX, int regionIndexZ);
  OverdrawColumn* createOverdrawColumn(int regionIndexX, int regionIndexZ);

  void setOverdrawBlock(OverdrawColumn *overdrawColumn, OverdrawBlock *overdrawBlock);

  vector<OverdrawColumn*> mOverdrawColumns;
  
};


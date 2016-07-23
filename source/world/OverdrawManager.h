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

#include "../math/v3d.h"

#include "../assets/AssetManager.h"
#include "../world/BlockTypeData.h"
#include "../world/WorldUtil.h"


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
  void removeColumn(int regionIndexX, int regionIndexZ);

  void clear();

  void save(FILE* file);
  void load(FILE* file);

private:
//  void removeColumn(OverdrawColumn* overdrawColum);
  void clearBlocks(int index);

  int getOverdrawColumn(int regionIndexX, int regionIndexZ);
  int addOverdrawColumn(int regionIndexX, int regionIndexZ);
  void deleteOverdrawColumn(int overdrawColumnIndex);

  void setOverdrawBlock(int overdrawColumnIndex, OverdrawBlock *overdrawBlock);

  vector<OverdrawColumn*> mOverdrawColumns;
  
};


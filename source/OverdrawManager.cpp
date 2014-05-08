#include "OverdrawManager.h"

OverdrawManager::OverdrawManager() {
}

OverdrawManager::~OverdrawManager() {
  clear();
}

void OverdrawManager::setBlock(const v3di_t& position, BYTE blockType) {
  v3di_t regionIndex = WorldUtil::getRegionIndex(position);
  int overdrawColumnIndex = getOverdrawColumn(regionIndex.x, regionIndex.z);
  if (overdrawColumnIndex == -1) {
    overdrawColumnIndex = addOverdrawColumn(regionIndex.x, regionIndex.z);
  }

  OverdrawBlock *overdrawBlock = new OverdrawBlock();
  overdrawBlock->x = position.x;
  overdrawBlock->y = position.y;
  overdrawBlock->z = position.z;
  overdrawBlock->blockType = blockType;

  setOverdrawBlock(overdrawColumnIndex, overdrawBlock);
}

vector<OverdrawBlock*> *OverdrawManager::getBlocks(int regionIndexX, int regionIndexZ) {
  int overdrawColumnIndex = getOverdrawColumn(regionIndexX, regionIndexZ);
  if(overdrawColumnIndex == -1) {
    return NULL;
  }
  return &mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks;
}

void OverdrawManager::clearBlocks(int overlandColumnIndex) {
  int size = mOverdrawColumns[overlandColumnIndex]->overdrawBlocks.size();
  for (int blockIndex = 0; blockIndex < size; blockIndex++) {
    if (mOverdrawColumns[overlandColumnIndex]->overdrawBlocks[blockIndex] == NULL) {
      printf("NULL BLOCK: %d\n", blockIndex);
    }
    else {
      delete  mOverdrawColumns[overlandColumnIndex]->overdrawBlocks[blockIndex];
      mOverdrawColumns[overlandColumnIndex]->overdrawBlocks[blockIndex] = NULL;
    }
  }
  mOverdrawColumns[overlandColumnIndex]->overdrawBlocks.clear();
}

void OverdrawManager::removeColumn(int regionIndexX, int regionIndexZ) {
  int overdrawColumnIndex = getOverdrawColumn(regionIndexX, regionIndexZ);
  deleteOverdrawColumn(overdrawColumnIndex);
}

void OverdrawManager::clear() {
  size_t size = mOverdrawColumns.size();
  for (size_t index = 0; index < size; index++) {
    // this looks dangerous ... however, each removeColumn() call is deleting the
    // first element, so this will work as long as that is valid
    deleteOverdrawColumn(0);
  }
  mOverdrawColumns.clear();
}

void OverdrawManager::save(FILE* file) {
}

void OverdrawManager::load(FILE* file) {
}

// this implementation seems a bit naive ...
int OverdrawManager::getOverdrawColumn(int regionIndexX, int regionIndexZ) {
  int size = mOverdrawColumns.size();
  for (int index = 0; index < size; index++) {
    OverdrawColumn *overdrawColumn = mOverdrawColumns[index];
    if (overdrawColumn->regionIndexX == regionIndexX && overdrawColumn->regionIndexZ == regionIndexZ) {
      return index;
    }
  }

  return -1;
}

int OverdrawManager::addOverdrawColumn(int regionIndexX, int regionIndexZ) {
  OverdrawColumn *overdrawColumn = new OverdrawColumn();
  overdrawColumn->regionIndexX = regionIndexX;
  overdrawColumn->regionIndexZ = regionIndexZ;
  mOverdrawColumns.push_back(overdrawColumn);
  printf("addedOverdrawColumn: %d\n", mOverdrawColumns.size());
  return mOverdrawColumns.size() - 1;
}

void OverdrawManager::deleteOverdrawColumn(int overdrawColumnIndex) {
  size_t size = mOverdrawColumns.size();
  if (size == 0 || overdrawColumnIndex >= size) {
    // cough ... *bullshit*
    return;
  }

  clearBlocks(overdrawColumnIndex);
  delete mOverdrawColumns[overdrawColumnIndex];

  if (size == overdrawColumnIndex + 1) {
    mOverdrawColumns.pop_back();
  printf("deleted OverdrawColumn: %d\n", mOverdrawColumns.size());
    return;
  }
  swap(mOverdrawColumns[overdrawColumnIndex], mOverdrawColumns[size - 1]);
  mOverdrawColumns.pop_back();
  printf("deleted OverdrawColumn: %d\n", mOverdrawColumns.size());
}

void OverdrawManager::setOverdrawBlock(int overdrawColumnIndex, OverdrawBlock *overdrawBlock) {
  int size = mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks.size();

  for (int index = 0; index < size; index++) {
    OverdrawBlock *existingOverdrawBlock = mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks[index];
    if (existingOverdrawBlock->x == overdrawBlock->x &&
      existingOverdrawBlock->y == overdrawBlock->y &&
      existingOverdrawBlock->z == overdrawBlock->z)
    {
      delete mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks[index];
      mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks[index] = overdrawBlock;
      return;
    }
  }

  // couldn't find a match to overwrite, so add it
  mOverdrawColumns[overdrawColumnIndex]->overdrawBlocks.push_back(overdrawBlock);
}

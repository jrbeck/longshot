#include "OverdrawManager.h"


OverdrawManager::OverdrawManager() {
}

OverdrawManager::~OverdrawManager() {
  clear();
}

void OverdrawManager::setBlock(const v3di_t& position, BYTE blockType) {
  printf("OverdrawManager::setBlock\n");
  v3di_t regionIndex = WorldUtil::getRegionIndex(position);
  OverdrawColumn *overdrawColumn = getOverdrawColumn(regionIndex.x, regionIndex.z);
  if (overdrawColumn == NULL) {
    overdrawColumn = createOverdrawColumn(regionIndex.x, regionIndex.z);
    mOverdrawColumns.push_back(overdrawColumn);
  }

  OverdrawBlock *overdrawBlock = new OverdrawBlock;
  overdrawBlock->x = position.x;
  overdrawBlock->y = position.y;
  overdrawBlock->z = position.z;
  overdrawBlock->blockType = blockType;

  setOverdrawBlock(overdrawColumn, overdrawBlock);
}

vector<OverdrawBlock*> *OverdrawManager::getBlocks(int regionIndexX, int regionIndexZ) {
  printf("GB\n");
  OverdrawColumn *overdrawColumn = getOverdrawColumn(regionIndexX, regionIndexZ);
  if(overdrawColumn == NULL) {
    return NULL;
  }
  return &overdrawColumn->overdrawBlocks;
}

void OverdrawManager::clearBlocks(OverdrawColumn* overdrawColumn) {
  int size = overdrawColumn->overdrawBlocks.size();
  printf("OverdrawManager::clearBlocks(): %d\n", size);
  if (size == 0) {
    return;
  }
  for (int index = 0; index < size; index++) {
    if (overdrawColumn->overdrawBlocks[index] == NULL) {
      printf("NULL BLOCK: %d\n", index);
    }
    else {
      delete overdrawColumn->overdrawBlocks[index];
      overdrawColumn->overdrawBlocks[index] = NULL;
    }
  }
  overdrawColumn->overdrawBlocks.clear();
}

void OverdrawManager::removeColumn(int regionIndexX, int regionIndexZ) {
  printf("RC\n");
  OverdrawColumn *overdrawColumn = getOverdrawColumn(regionIndexX, regionIndexZ);
  removeColumn(overdrawColumn);
}

void OverdrawManager::removeColumn(OverdrawColumn* overdrawColumn) {
  if (overdrawColumn == NULL) {
    return;
  }
  printf("Q\n");
  clearBlocks(overdrawColumn);
  printf("W\n");
  delete overdrawColumn;
  printf("E\n");
  overdrawColumn = NULL;
  printf("R\n");
  if(mOverdrawColumns.size() > 1) {
    printf("T\n");
    swap(overdrawColumn, mOverdrawColumns[mOverdrawColumns.size() - 1]);
  }
  printf("Y\n");
  mOverdrawColumns.pop_back();
  printf("P\n");
}

void OverdrawManager::clear() {
  printf("C\n");
  int size = mOverdrawColumns.size();
  for (int index = 0; index < size; index++) {
    // this looks dangerous ... however, each removeColumn() call is deleting the
    // first element, so this will work as long as that is valid
    removeColumn(mOverdrawColumns[0]);
  }
  mOverdrawColumns.clear();
}

void OverdrawManager::save(FILE* file) {
}

void OverdrawManager::load(FILE* file) {
}

// this implementation seems a bit naive ...
OverdrawColumn* OverdrawManager::getOverdrawColumn(int regionIndexX, int regionIndexZ) {
  printf("OverdrawManager::getOverdrawColumn(): %d, %d\n", regionIndexX, regionIndexZ);
  int size = mOverdrawColumns.size();
  for (int index = 0; index < size; index++) {
    OverdrawColumn *overdrawColumn = mOverdrawColumns[index];
    if (overdrawColumn->regionIndexX == regionIndexX && overdrawColumn->regionIndexZ == regionIndexZ) {
      return overdrawColumn;
    }
  }

  return NULL;
}

OverdrawColumn* OverdrawManager::createOverdrawColumn(int regionIndexX, int regionIndexZ) {
  printf("OverdrawManager::createOverdrawColumn: %d\n", mOverdrawColumns.size());
  OverdrawColumn *overdrawColumn = new OverdrawColumn;
  overdrawColumn->regionIndexX = regionIndexX;
  overdrawColumn->regionIndexZ = regionIndexZ;
  return overdrawColumn;
}

void OverdrawManager::setOverdrawBlock(OverdrawColumn* overdrawColumn, OverdrawBlock *overdrawBlock) {

  printf("OverdrawManager::setOverdrawBlock(): %d\n", overdrawColumn->overdrawBlocks.size());

  int size = overdrawColumn->overdrawBlocks.size();

  for (int index = 0; index < size; index++) {
    OverdrawBlock *existingOverdrawBlock = overdrawColumn->overdrawBlocks[index];
    if (existingOverdrawBlock->x == overdrawBlock->x &&
      existingOverdrawBlock->y == overdrawBlock->y &&
      existingOverdrawBlock->z == overdrawBlock->z)
    {
      delete overdrawColumn->overdrawBlocks[index];
      overdrawColumn->overdrawBlocks[index] = overdrawBlock;
      return;
    }
  }

  // couldn't find a match to overwrite, so add it
  overdrawColumn->overdrawBlocks.push_back(overdrawBlock);
}

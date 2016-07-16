#include "DungeonModel.h"

DungeonModel::DungeonModel(int width, int height) {
  buffer = new DungeonTile[width * height];
  mWidth = width;
  mHeight = height;
}

DungeonModel::~DungeonModel() {
  if( buffer != 0) {
    delete [] buffer;
    buffer = 0;
  }
}

int DungeonModel::getWidth() const {
  return mWidth;
}

int DungeonModel::getHeight() const {
  return mHeight;
}

bool DungeonModel::isInDungeon(int i, int j) const {
  if (i < 0 || j < 0 || i >= mWidth || j >= mHeight) {
    return false;
  }
  return true;
}

DungeonTile* DungeonModel::getTile(int i, int j) const {
  if (isInDungeon(i, j)) {
    return &buffer[i + (j * mWidth)];
  }
  return 0;
}

void DungeonModel::setTile(int i, int j, const DungeonTile& tile) {
  if (isInDungeon(i, j)) {
    buffer[i + (j * mWidth)] = tile;
  }
}

void DungeonModel::setTile(int i, int j, const SelectiveDungeonTile& tile) {
  if (isInDungeon(i, j)) {
    // warning, in the interest of speed, this is code repeated from getTile()
    tile.updateDungeonTile(buffer[i + (j * mWidth)]);
  }
}

DungeonTile* DungeonModel::getBuffer() const {
  return buffer;
}

void DungeonModel::setBuffer(const DungeonTile& tile) {
  int bufferLength = mWidth * mHeight;
  for (int i = 0; i < bufferLength; i++) {
    buffer[i] = tile;
  }
}

void DungeonModel::setBuffer(const SelectiveDungeonTile& tile) {
  int bufferLength = mWidth * mHeight;
  for (int i = 0; i < bufferLength; i++) {
    tile.updateDungeonTile(buffer[i]);
  }
}

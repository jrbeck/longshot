// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldMapView
// *
// * displays the WorldMap class
// * part of the Model View Controller pattern
// *
// * the chunks to be drawn must be loaded into the WorldMap. to be displayed
// * properly, they must be lit, and have the lighting applied (WorldLighting), and
// * have their visibility set (WorldMap::updateBlockVisibility())
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../game/GameModel.h"
#include "../world/WorldMap.h"
#include "../world/WorldLighting.h"


enum {
  DISPLAY_LIST_SOLID,
  DISPLAY_LIST_LIQUID,
  DISPLAY_LIST_CHUNK_BOX,

  NUM_LIST_TYPES
};


class ColumnViewInfo {
public:
  ColumnViewInfo() {
    mDisplayListHandle = 0;
    numChunks = 0;
  }

  ~ColumnViewInfo() {
    if (mDisplayListHandle != 0) {
      glDeleteLists(mDisplayListHandle, numChunks * NUM_LIST_TYPES);
    }
  }

  int columnIndex;
  v2di_t indexCoords;
  //  v3di_t worldCoords;
  int numChunks;

  GLuint mDisplayListHandle;
};


class WorldMapView {
public:
  WorldMapView(AssetManager* assetManager, GameModel* gameModel);
  ~WorldMapView();

  void toggleShowWorldChunkBoxes();

  void update(const LightManager& lightManager);

  void drawSolidBlocks(const GlCamera& camera) const;
  void drawLiquidBlocks(const GlCamera& camera) const;
  void drawChunkBoxes(const GlCamera& camera) const;

private:
  void setGameModel(GameModel* gameModel);
  void setSunColor(GameModel* gameModel);

  void generateDisplayLists(int columnIndex);
  void deleteDisplayLists(int columnIndex);
  void drawSolidForDisplayList(const WorldChunk& chunk) const;
  void drawLiquidForDisplayList(const WorldChunk& chunk) const;
  void drawChunkBoxForDisplayList(v3di_t worldPosition) const;

  AssetManager* mAssetManager;
  WorldMap* mWorldMap;
  ColumnViewInfo* mColumnInfo;

  // these are derived from mWorldMap
  int mWorldMapSideX;
  int mWorldMapSideZ;
  int mNumColumns;

  bool mDrawChunkBoxes;

  IntColor mSunColor;
};

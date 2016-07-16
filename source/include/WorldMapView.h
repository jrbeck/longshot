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

#include "WorldMap.h"
#include "WorldLighting.h"


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
  WorldMapView();
  ~WorldMapView();

  // this will reset everything and prepare it for a new map
  void setWorldMap(WorldMap *worldMap, IntColor &sunColor);
  void toggleShowWorldChunkBoxes(void);

  // this will destroy the inactive display lists and
  // generate the new display lists
  void update(AssetManager &assetManager, const LightManager &lightManager);

  void generateDisplayLists(int columnIndex, const AssetManager &assetManager);
  void deleteDisplayLists(int columnIndex);
  void drawSolidForDisplayList(const WorldChunk &chunk, const AssetManager &assetManager) const;
  void drawLiquidForDisplayList(const WorldChunk &chunk, const AssetManager &assetManager) const;
  void drawChunkBoxForDisplayList(v3di_t worldPosition) const;

  void drawSolidBlocks(const GlCamera &camera, const AssetManager &assetManager) const;
  void drawLiquidBlocks(const GlCamera &camera, const AssetManager &assetManager) const;
  void drawChunkBoxes(const GlCamera &camera, const AssetManager &assetManager) const;

private:
  WorldMap *mWorldMap;
  ColumnViewInfo *mColumnInfo;

  // these are derived from mWorldMap
  int mWorldMapSideX;
  int mWorldMapSideZ;
  int mNumColumns;

  bool mDrawChunkBoxes;

  IntColor mSunColor;
};


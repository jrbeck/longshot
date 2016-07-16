#include "WorldMapView.h"

WorldMapView::WorldMapView() :
mWorldMap(NULL),
mColumnInfo(NULL)
{
  mSunColor.r = LIGHT_LEVEL_MAX;
  mSunColor.g = LIGHT_LEVEL_MAX;
  mSunColor.b = LIGHT_LEVEL_MAX;
  mDrawChunkBoxes = false;
}

WorldMapView::~WorldMapView() {
  if (mColumnInfo != NULL) {
    delete[] mColumnInfo;
  }
}

void WorldMapView::setWorldMap(WorldMap* worldMap, IntColor& sunColor) {
  // we'll delete this no matter what since it calls the
  // destructors, otherwise, we'll leak memory in opengl in
  // the display lists
  if (mColumnInfo != NULL) {
    delete[] mColumnInfo;
    mColumnInfo = NULL;
  }

  mWorldMap = worldMap;
  if (worldMap == NULL) {
    printf("WorldMapView::setWorldMap(): error: NULL worldMap\n");
    return;
  }

  mWorldMapSideX = mWorldMap->mXWidth;
  mWorldMapSideZ = mWorldMap->mZWidth;
  mNumColumns = mWorldMapSideX * mWorldMapSideZ;

  printf("WorldMapView::setWorldMap(): numColumns: %d\n", mNumColumns);

  mColumnInfo = new ColumnViewInfo[mNumColumns];
  if (mColumnInfo == NULL) {
    printf("WorldMapView::setWorldMap(): error: out of memory\n");
    mWorldMap = NULL;
    return;
  }

  // now for the ambient light color
  mSunColor = sunColor;
}

void WorldMapView::toggleShowWorldChunkBoxes() {
  mDrawChunkBoxes = !mDrawChunkBoxes;
}

void WorldMapView::update(AssetManager& assetManager, const LightManager& lightManager) {
  // FIXME: do we really need to do this every update?
  // actually this is dangerous unless we reset
  // mNumColumns since the size may have changed
  //  worldMapSideX = mWorldMap->mXWidth;
  //  worldMapSideZ = mWorldMap->mZWidth;
  //  mNumColumns = worldMapSideX * worldMapSideZ;

  // delete any display lists that pertain to columns that are no longer
  // visible. this fixes the problem of a new column being loaded, but
  // the previous (visible) column (using columnIndex) being drawn
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mWorldMap->mColumns[columnIndex].mWorldIndex.x != mColumnInfo[columnIndex].indexCoords.x ||
      mWorldMap->mColumns[columnIndex].mWorldIndex.z != mColumnInfo[columnIndex].indexCoords.y)
    {
      deleteDisplayLists(columnIndex);
      mColumnInfo[columnIndex].indexCoords.x = mWorldMap->mColumns[columnIndex].mWorldIndex.x;
      mColumnInfo[columnIndex].indexCoords.y = mWorldMap->mColumns[columnIndex].mWorldIndex.z;
    }
  }

  static int lastColumn = 0;

  // update the lighting in a gradual fashion
  // skip if it needs updating anyhow
  if (mWorldMap->mUpdateLightingContinuously) {
    int num = 0;
    while (mWorldMap->mColumns[lastColumn].mNeedDisplayList ||
      mWorldMap->mColumns[lastColumn].mNeedVisibility ||
      mWorldMap->mColumns[lastColumn].mNeedLightingApplied ||
      mWorldMap->mColumns[lastColumn].mNeedShadowVolume)
    {
      lastColumn = (lastColumn + 1) % mNumColumns;

      // check to see if we're gonna be in an infinite loop
      if (++num == mNumColumns) break;
    }

    mWorldMap->mColumns[lastColumn].clearEmptyChunks();
    if (num != mNumColumns) {
      WorldLighting::createShadowVolume(lastColumn, *mWorldMap);
      WorldLighting::applyLighting(lastColumn, *mWorldMap, lightManager, mSunColor);
      generateDisplayLists(lastColumn, assetManager);
    }
    lastColumn = (lastColumn + 1) % mNumColumns;
  }

  // add sunlight
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mWorldMap->mColumns[columnIndex].mNeedShadowVolume) {
      mWorldMap->mColumns[columnIndex].clearEmptyChunks();
      WorldLighting::createShadowVolume(columnIndex, *mWorldMap);
    }
  }
  // update the visibility
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mWorldMap->mColumns[columnIndex].mNeedVisibility) {
      if (mWorldMap->columnHasFourNeighbors(columnIndex)) {
        // only update if we have proper neighbors
        mWorldMap->updateBlockVisibility(columnIndex);
      }
    }
  }
  // apply lighting
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mWorldMap->mColumns[columnIndex].mNeedLightingApplied) {
      WorldLighting::applyLighting(columnIndex, *mWorldMap, lightManager, mSunColor);
    }
  }
  // generate display lists
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mWorldMap->mColumns[columnIndex].mNeedDisplayList) {
      generateDisplayLists(columnIndex, assetManager);
    }
  }
}

void WorldMapView::generateDisplayLists(int columnIndex, const AssetManager &assetManager) {
  int numChunks = mWorldMap->mColumns[columnIndex].mWorldChunks.size();
  if (numChunks == 0) {
    //    printf("WorldMapView::generateDisplayLists(): error: no chunks in this column\n");

    // delete any display lists that are associated with this column
    deleteDisplayLists(columnIndex);
    mColumnInfo[columnIndex].numChunks = 0;
    return;
  }

  //  if (mColumnInfo[columnIndex].numChunks != numChunks) {
  deleteDisplayLists(columnIndex);
  mColumnInfo[columnIndex].numChunks = numChunks;
  mColumnInfo[columnIndex].mDisplayListHandle = glGenLists(numChunks * NUM_LIST_TYPES);
  //  }

  GLuint handle;
  for (int chunkIndex = 0; chunkIndex < numChunks; chunkIndex++) {
    // figure out the handle for this display list
    handle = mColumnInfo[columnIndex].mDisplayListHandle + (chunkIndex * NUM_LIST_TYPES);

    // create the lists
    glNewList(handle + DISPLAY_LIST_SOLID, GL_COMPILE);
    drawSolidForDisplayList(*mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex], assetManager);
    glEndList();

    glNewList(handle + DISPLAY_LIST_LIQUID, GL_COMPILE);
    drawLiquidForDisplayList(*mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex], assetManager);
    glEndList();

    glNewList(handle + DISPLAY_LIST_CHUNK_BOX, GL_COMPILE);
    drawChunkBoxForDisplayList(mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex]->mWorldPosition);
    glEndList();
  }

  // since we've just finished doing that...
  mWorldMap->mColumns[columnIndex].mNeedShadowVolume = false;
  mWorldMap->mColumns[columnIndex].mNeedVisibility = false;
  mWorldMap->mColumns[columnIndex].mNeedLightingApplied = false;
  mWorldMap->mColumns[columnIndex].mNeedDisplayList = false;
}

void WorldMapView::deleteDisplayLists(int columnIndex) {
  if (mColumnInfo[columnIndex].mDisplayListHandle != 0) {
    glDeleteLists(mColumnInfo[columnIndex].mDisplayListHandle, mColumnInfo[columnIndex].numChunks * NUM_LIST_TYPES);
    mColumnInfo[columnIndex].mDisplayListHandle = 0;
  }
}

// draw the blocks for the display list
void WorldMapView::drawSolidForDisplayList(const WorldChunk &chunk, const AssetManager &assetManager) const {
  v3di_t relativePosition;
  int blockIndex = 0;
  const block_t *blocks = chunk.getBlocks();
  block_t block;

  glBegin(GL_QUADS);

  v3di_t worldPosition;
  worldPosition.z = chunk.mWorldPosition.z;
  for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++) {
    worldPosition.y = chunk.mWorldPosition.y;
    for (relativePosition.y = 0; relativePosition.y < WORLD_CHUNK_SIDE; relativePosition.y++) {
      worldPosition.x = chunk.mWorldPosition.x;
      for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++) {
        block = blocks[blockIndex];
        if (block.faceVisibility != 0 &&
          gBlockData.get(block.type)->solidityType != BLOCK_SOLIDITY_TYPE_LIQUID)
        {
          if (gBlockData.get(block.type)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
            assetManager.drawPlantBlock(worldPosition, block);
          }
          else {
            assetManager.drawBlock(1.0f, worldPosition, block);
          }
        }

        blockIndex++;
        worldPosition.x++;
      }
      worldPosition.y++;
    }
    worldPosition.z++;
  }

  glEnd();
}

void WorldMapView::drawLiquidForDisplayList(const WorldChunk &chunk, const AssetManager &assetManager) const {
  if (chunk.mNumWaterBlocks == 0) {
    return;
  }

  v3di_t relativePosition;
  int blockIndex = 0;
  const block_t *blocks = chunk.getBlocks();
  block_t block;
  v3di_t worldPosition;

  glBegin(GL_QUADS);

  worldPosition.z = chunk.mWorldPosition.z;
  for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++) {
    worldPosition.y = chunk.mWorldPosition.y;
    for (relativePosition.y = 0; relativePosition.y < WORLD_CHUNK_SIDE; relativePosition.y++) {
      worldPosition.x = chunk.mWorldPosition.x;
      for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++) {
        block = blocks[blockIndex];
        if (block.faceVisibility != 0 &&
          gBlockData.get(block.type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID)
        {
          // this little hack makes a 10cm 'lip' of ground around the water surface
          if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
            assetManager.drawBlock(0.9f, worldPosition, block);
          }
          else {
            assetManager.drawBlock(1.0f, worldPosition, block);
          }
        }

        blockIndex++;
        worldPosition.x++;
      }
      worldPosition.y++;
    }
    worldPosition.z++;
  }

  glEnd();
}

void WorldMapView::drawChunkBoxForDisplayList(v3di_t worldPosition) const {
  // draw the chunk box
  v3di_t farWorldPosition;
  farWorldPosition.x = worldPosition.x + WORLD_CHUNK_SIDE;
  farWorldPosition.y = worldPosition.y + WORLD_CHUNK_SIDE;
  farWorldPosition.z = worldPosition.z + WORLD_CHUNK_SIDE;

  //  glEnable (GL_TEXTURE_2D);
  //  glBindTexture (GL_TEXTURE_2D, 0);

  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

  glBegin(GL_LINES);
  glVertex3i(worldPosition.x, worldPosition.y, worldPosition.z);
  glVertex3i(farWorldPosition.x, worldPosition.y, worldPosition.z);

  glVertex3i(worldPosition.x, worldPosition.y, worldPosition.z);
  glVertex3i(worldPosition.x, farWorldPosition.y, worldPosition.z);

  glVertex3i(worldPosition.x, worldPosition.y, worldPosition.z);
  glVertex3i(worldPosition.x, worldPosition.y, farWorldPosition.z);

  glVertex3i(farWorldPosition.x, farWorldPosition.y, farWorldPosition.z);
  glVertex3i(worldPosition.x, farWorldPosition.y, farWorldPosition.z);

  glVertex3i(farWorldPosition.x, farWorldPosition.y, farWorldPosition.z);
  glVertex3i(farWorldPosition.x, worldPosition.y, farWorldPosition.z);

  glVertex3i(farWorldPosition.x, farWorldPosition.y, farWorldPosition.z);
  glVertex3i(farWorldPosition.x, farWorldPosition.y, worldPosition.z);

  glVertex3i(farWorldPosition.x, worldPosition.y, worldPosition.z);
  glVertex3i(farWorldPosition.x, worldPosition.y, farWorldPosition.z);

  glVertex3i(farWorldPosition.x, worldPosition.y, worldPosition.z);
  glVertex3i(farWorldPosition.x, farWorldPosition.y, worldPosition.z);

  glVertex3i(worldPosition.x, farWorldPosition.y, worldPosition.z);
  glVertex3i(farWorldPosition.x, farWorldPosition.y, worldPosition.z);

  glVertex3i(worldPosition.x, farWorldPosition.y, worldPosition.z);
  glVertex3i(worldPosition.x, farWorldPosition.y, farWorldPosition.z);

  glVertex3i(worldPosition.x, worldPosition.y, farWorldPosition.z);
  glVertex3i(worldPosition.x, farWorldPosition.y, farWorldPosition.z);

  glVertex3i(worldPosition.x, worldPosition.y, farWorldPosition.z);
  glVertex3i(farWorldPosition.x, worldPosition.y, farWorldPosition.z);
  glEnd();
}

// this actually renders the display lists
void WorldMapView::drawSolidBlocks(const GlCamera &camera, const AssetManager &assetManager) const {
  //  glDisable (GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glAlphaFunc(GL_GREATER, 0.9f);
  glEnable(GL_ALPHA_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, assetManager.getTerrainTextureHandle());

  GLuint handle;
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mColumnInfo[columnIndex].mDisplayListHandle == 0) {
      continue;
    }
    for (int chunkIndex = 0; chunkIndex < mColumnInfo[columnIndex].numChunks; chunkIndex++) {
      // check if the chunk is visible
      //      if (camera.bounding_sphere_test (
      //        mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex]->mBoundingSphere) != FRUSTUM_OUTSIDE)
        {
          handle = mColumnInfo[columnIndex].mDisplayListHandle + (chunkIndex * NUM_LIST_TYPES) + DISPLAY_LIST_SOLID;
          glCallList(handle);
        }
    }
  }

  if (mDrawChunkBoxes) {
    drawChunkBoxes(camera, assetManager);
  }

  glDisable(GL_ALPHA_TEST);

  //  glEnable (GL_TEXTURE_2D);
}

void WorldMapView::drawLiquidBlocks(const GlCamera &camera, const AssetManager &assetManager) const {
  //  glDisable (GL_TEXTURE_2D);
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);

  glBindTexture(GL_TEXTURE_2D, assetManager.getTerrainTextureHandle());

  GLuint handle;
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mColumnInfo[columnIndex].mDisplayListHandle == 0) {
      continue;
    }
    for (int chunkIndex = 0; chunkIndex < mColumnInfo[columnIndex].numChunks; chunkIndex++) {
      // check if the chunk is visible
      //      if (camera.bounding_sphere_test (
      //        mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex]->mBoundingSphere) != FRUSTUM_OUTSIDE)
        {
          handle = mColumnInfo[columnIndex].mDisplayListHandle + (chunkIndex * NUM_LIST_TYPES) + DISPLAY_LIST_LIQUID;
          glCallList(handle);
        }

    }
  }

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
  //  glEnable (GL_TEXTURE_2D);
}

void WorldMapView::drawChunkBoxes(const GlCamera &camera, const AssetManager &assetManager) const {
  glDisable(GL_TEXTURE_2D);

  GLuint handle;
  for (int columnIndex = 0; columnIndex < mNumColumns; columnIndex++) {
    if (mColumnInfo[columnIndex].mDisplayListHandle == 0) {
      continue;
    }
    for (int chunkIndex = 0; chunkIndex < mColumnInfo[columnIndex].numChunks; chunkIndex++) {
      // check if the chunk is visible
      //      if (camera.bounding_sphere_test (
      //        mWorldMap->mColumns[columnIndex].mWorldChunks[chunkIndex]->mBoundingSphere) != FRUSTUM_OUTSIDE)
        {
          handle = mColumnInfo[columnIndex].mDisplayListHandle + (chunkIndex * NUM_LIST_TYPES) + DISPLAY_LIST_CHUNK_BOX;
          glCallList(handle);
        }
    }
  }

  glEnable(GL_TEXTURE_2D);
}

#include "InactiveColumnManager.h"


InactiveColumnManager::InactiveColumnManager() {
}


InactiveColumnManager::~InactiveColumnManager() {
  printf("InactiveColumnManager::~InactiveColumnManager()\n");
  clear();
}


int InactiveColumnManager::saveToInactiveColumns(WorldColumn &worldColumn) {
  worldColumn.clearEmptyChunks();

  if (worldColumn.mWorldChunks.size() == 0) {
    printf("InactiveColumnManager::saveToInactiveColumns(): tried to save empty column...\n");
    return 0;
  }

  ColumnDatum *columnDatum = getColumnDatumForSaving(worldColumn.mWorldIndex);
  if (columnDatum == NULL) {
    printf("getColumnDatumForSaving() returned NULL\n");
    return -1;
  }

  // write chunks
  for (size_t i = 0; i < worldColumn.mWorldChunks.size(); i++) {
    saveChunkDatum(i, *columnDatum, *worldColumn.mWorldChunks[i]);
  }

  return 0;
}




void InactiveColumnManager::saveChunkDatum(size_t chunkIndex, ColumnDatum &column, const WorldChunk &chunk) const {
  ChunkDatum *chunkDatum = new ChunkDatum;

  chunkDatum->worldIndex = chunk.mWorldIndex;
  chunkDatum->worldPosition = chunk.mWorldPosition;
  chunkDatum->numBlocks = chunk.mNumBlocks;
  chunkDatum->numWaterBlocks = chunk.mNumWaterBlocks;

  // WARNING: this data must be freed later!
  chunkDatum->blockData = new BYTE[WORLD_CHUNK_SIDE_CUBED];
  if (chunkDatum->blockData == NULL) {
    // FIXME: should probably return something meaningful at this point
    return;
  }

  // load the chunk data into blockData
  for (int i = 0; i < WORLD_CHUNK_SIDE_CUBED; i++) {
    chunkDatum->blockData[i] = chunk.getBlockTypeByIndex(i);
  }

  column.chunkData.push_back(chunkDatum);
}


int InactiveColumnManager::loadFromInactiveColumns(int x, int z, WorldColumn &worldColumn) {
  // try to find the column in the column manager
  ColumnDatum *column = readColumn(x, z);
  if (column == NULL) {
    //		printf("not loading: %d, %d\n", x, z);
    return 1;
  }

  worldColumn.mWorldIndex = column->worldIndex;
  //	v3di_print("col", column->worldIndex);
  worldColumn.mWorldPosition = column->worldPosition;

  // load chunks
  for (size_t i = 0; i < column->chunkData.size(); i++) {
    loadChunkDatum(*column->chunkData[i], worldColumn);
  }

  worldColumn.updateHighestAndLowest();

  worldColumn.mNeedShadowVolume = true;
  worldColumn.mNeedLightingApplied = false;
  worldColumn.mNeedVisibility = false;
  worldColumn.mNeedDisplayList = false;

  return 0;
}



void InactiveColumnManager::loadChunkDatum(const ChunkDatum &chunk, WorldColumn &worldColumn) {
  // create the chunk in memory
  size_t chunkIndex = worldColumn.createChunkContaining(chunk.worldPosition);

  // if we're just passing a block of chars as type data, and deriving the rest of the
  // block_t data from that, then we need to keep a count of these
  worldColumn.mWorldChunks[chunkIndex]->mNumBlocks = chunk.numBlocks;
  worldColumn.mWorldChunks[chunkIndex]->mNumWaterBlocks = chunk.numWaterBlocks;

  // otherwise it will update as we call setBlockByIndex()
  //	mWorldChunks[chunkIndex]->mNumBlocks = 0;
  //	mWorldChunks[chunkIndex]->mNumWaterBlocks = 0;

  // set the data
  for (int i = 0; i < WORLD_CHUNK_SIDE_CUBED; i++) {
    worldColumn.mWorldChunks[chunkIndex]->setBlockTypeByIndex(i, chunk.blockData[i]);
  }
}



void InactiveColumnManager::clearColumn(size_t columnIndex) {
  if (columnIndex >= mColumnData.size()) {
    // index out of range
    printf("InactiveColumnManager::clearColumn(): index out of range\n");
    return;
  }

  for (size_t chunk = 0; chunk < mColumnData[columnIndex]->chunkData.size(); chunk++) {
    if (mColumnData[columnIndex]->chunkData[chunk] != NULL) {
      // free the block data
      if (mColumnData[columnIndex]->chunkData[chunk]->blockData != NULL) {
        delete[] mColumnData[columnIndex]->chunkData[chunk]->blockData;
        mColumnData[columnIndex]->chunkData[chunk]->blockData = NULL;
      }
      else {
        printf("InactiveColumnManager::clearColumn(): asked to delete NULL blockData");
      }

      // delete the whole thing
      delete mColumnData[columnIndex]->chunkData[chunk];
      mColumnData[columnIndex]->chunkData[chunk] = NULL;
    }
    else {
      printf("InactiveColumnManager::clearColumn(): NULL ChunkDatum");
    }
  }

  mColumnData[columnIndex]->chunkData.clear();
}



void InactiveColumnManager::clear(void) {
  printf("InactiveColumnManager::clear(): clearing %d columns\n", mColumnData.size());
  // deep clear each column
  for (size_t columnIndex = 0; columnIndex < mColumnData.size(); columnIndex++) {
    if (mColumnData[columnIndex] != NULL) {

      //			printf ("#");
      clearColumn(columnIndex);

      delete mColumnData[columnIndex];
      mColumnData[columnIndex] = NULL;
    }
    else {
      printf("InactiveColumnManager::clear(): NULL ColumnDatum*");
    }
  }

  //	printf ("a");

  mColumnData.clear();
}



// this is very naive - some sorting would make this much faster
int InactiveColumnManager::getColumnIndex(int x, int z) {
  for (size_t i = 0; i < mColumnData.size(); i++) {
    if (mColumnData[i]->worldIndex.x == x &&
      mColumnData[i]->worldIndex.z == z)
    {
      return i;
    }
  }

  return -1;
}



ColumnDatum *InactiveColumnManager::getColumnDatumForSaving(v3di_t worldIndex) {
  int columnIndex = getColumnIndex(worldIndex.x, worldIndex.z);

  // if it already exists, clear it, then return a pointer to the proper datum
  if (columnIndex >= 0) {
    clearColumn(columnIndex);
    return mColumnData[columnIndex];
  }

  //	printf ("NEW ONE %d: (%d, %d)\n", mColumnData.size (), worldIndex.x, worldIndex.z);

  // otherwise, we get to make a new one
  ColumnDatum *column = new ColumnDatum;

  column->worldIndex = worldIndex;

  column->worldPosition.x = worldIndex.x * WORLD_CHUNK_SIDE;
  column->worldPosition.y = 0;
  column->worldPosition.z = worldIndex.z * WORLD_CHUNK_SIDE;

  mColumnData.push_back(column);

  return column;
}


/*
void InactiveColumnManager::saveColumn (const ColumnDatum &column) {
int columnIndex = getColumnIndex (column.worldIndex.x, column.worldIndex.z);

if (columnIndex  >= 0) {
clearColumn (columnIndex);

mColumnData[columnIndex] = column;
return;
}

mColumnData.push_back (column);
}
*/


ColumnDatum *InactiveColumnManager::readColumn(int x, int z) {
  int columnIndex = getColumnIndex(x, z);

  if (columnIndex >= 0) {
    return mColumnData[columnIndex];
  }

  return NULL;
}



int InactiveColumnManager::saveToDisk(FILE *file) {
  // we'll need to know this!
  size_t numColumns = mColumnData.size();
  fwrite(&numColumns, sizeof size_t, 1, file);

  size_t totalChunks = 0;

  for (size_t column = 0; column < numColumns; column++) {
    // write column header data
    fwrite(&mColumnData[column]->worldIndex, sizeof v3di_t, 1, file);
    fwrite(&mColumnData[column]->worldPosition, sizeof v3di_t, 1, file);

    // need a current count of the chunks
    size_t numChunks = mColumnData[column]->chunkData.size();
    fwrite(&numChunks, sizeof size_t, 1, file);

    totalChunks += numChunks;

    for (size_t chunk = 0; chunk < numChunks; chunk++) {
      // write chunk header data
      fwrite(&mColumnData[column]->chunkData[chunk]->worldIndex, sizeof v3di_t, 1, file);
      fwrite(&mColumnData[column]->chunkData[chunk]->worldPosition, sizeof v3di_t, 1, file);
      fwrite(&mColumnData[column]->chunkData[chunk]->numBlocks, sizeof (int), 1, file);
      fwrite(&mColumnData[column]->chunkData[chunk]->numWaterBlocks, sizeof (int), 1, file);

      // now for the good stuff
      fwrite(mColumnData[column]->chunkData[chunk]->blockData,
        sizeof BYTE, WORLD_CHUNK_SIDE_CUBED, file);
    }
  }

  printf("InactiveColumnManager::saveToDisk(): saved %d columns, %d chunks\n", numColumns, totalChunks);

  return 0;
}



int InactiveColumnManager::loadFromDisk(FILE *file) {
  printf("InactiveColumnManager::loadFromDisk ()\n");

  //start with a blank slate
  clear();

  size_t numColumns;
  fread(&numColumns, sizeof size_t, 1, file);

  size_t totalChunks = 0;

  printf("loading: %d\n", numColumns);

  for (size_t column = 0; column < numColumns; column++) {
    ColumnDatum *columnDatum = new ColumnDatum;

    // read column header data
    fread(&columnDatum->worldIndex, sizeof v3di_t, 1, file);
    fread(&columnDatum->worldPosition, sizeof v3di_t, 1, file);

    size_t numChunks;
    fread(&numChunks, sizeof size_t, 1, file);

    totalChunks += numChunks;

    for (size_t chunk = 0; chunk < numChunks; chunk++) {
      ChunkDatum *chunkDatum = new ChunkDatum;

      // read chunk header data
      fread(&chunkDatum->worldIndex, sizeof v3di_t, 1, file);
      fread(&chunkDatum->worldPosition, sizeof v3di_t, 1, file);
      fread(&chunkDatum->numBlocks, sizeof (int), 1, file);
      fread(&chunkDatum->numWaterBlocks, sizeof (int), 1, file);

      // now for the good stuff
      chunkDatum->blockData = new BYTE[WORLD_CHUNK_SIDE_CUBED];
      if (chunkDatum->blockData != NULL) {
        fread(chunkDatum->blockData, sizeof BYTE, WORLD_CHUNK_SIDE_CUBED, file);

        columnDatum->chunkData.push_back(chunkDatum);
      }
      else {
        printf("InactiveColumnManager::loadFromDisk(): out of mem\n");
      }
    }

    mColumnData.push_back(columnDatum);
  }

  printf("InactiveColumnManager::loadFromDisk(): loaded %d columns, %d chunks\n", numColumns, totalChunks);


  return 0;
}




size_t InactiveColumnManager::getNumColumns(void) const {
  return mColumnData.size();
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * InactiveColumnManager
// * sub-class of WorldMap
// *
// * this is responsible for keeping track of the columns that have been loaded.
// * it keeps them in memory even when they are part of the WorldMap, so to aid
// * performance. it also saves to disk and loads from disk.
// *
// * WARNING: memory for the individual ChunkDatum.(char*)blockData is allocated
// * with new [] in WorldMap. It is, however, freed here in clearColumn().
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cstdio>
#include <vector>

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include "../math/v3d.h"
#include "../world/WorldColumn.h"

using namespace std;


class ChunkDatum {
public:
  ChunkDatum() : blockData(NULL) { }
  ~ChunkDatum() {
    if (blockData != NULL) {
      printf ("#");
      delete [] blockData;
    }
  }

  v3di_t worldIndex;
  v3di_t worldPosition;

  int numBlocks;
  int numWaterBlocks;

//  bool needsToBeSaved;

  BYTE *blockData;
};


class ColumnDatum {
public:
  ~ColumnDatum () {
//    printf ("COLUMN DATUM DESTROYER!\n");
    size_t numChunks = chunkData.size();
    for (size_t i = 0; i < numChunks; ++i) {
      delete chunkData[i];
    }
    chunkData.clear();
  }

  v3di_t worldIndex;
  v3di_t worldPosition;

  vector <ChunkDatum *>chunkData;
};



class InactiveColumnManager {
private:

public:
  InactiveColumnManager ();
  InactiveColumnManager (int WorldChunkSide);
  ~InactiveColumnManager ();

  int saveToInactiveColumns (WorldColumn &worldColumn);
  void saveChunkDatum (size_t chunkIndex, ColumnDatum &column, const WorldChunk &chunk) const;
  int loadFromInactiveColumns (int x, int z, WorldColumn &worldColumn);
  void loadChunkDatum (const ChunkDatum &chunk, WorldColumn &worldColumn);

  void clearColumn (size_t columnIndex);
  void clear (void);

  int getColumnIndex (int x, int z);

  ColumnDatum *getColumnDatumForSaving (v3di_t worldIndex);
  void saveColumn (const ColumnDatum &column);
  ColumnDatum *readColumn (int x, int z);

  int saveToDisk (FILE *file);
  int loadFromDisk (FILE *file);

  size_t getNumColumns (void) const;

private:
  vector <ColumnDatum*> mColumnData;
};

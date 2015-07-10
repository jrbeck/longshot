// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * DungeonModel
// *
// * desc: basic model for Dungeons
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "DungeonTile.h"


class DungeonModel {
public:
  DungeonModel( int width, int height );
  ~DungeonModel();

  int getWidth() const;
  int getHeight() const;

  bool isInDungeon( int i, int j ) const;

  DungeonTile* getTile( int i, int j ) const;
  void setTile( int i, int j, const DungeonTile& tile );
  // use a SelectiveDungeonTile to only update certain fields
  void setTile( int i, int j, const SelectiveDungeonTile& tile );

  DungeonTile* getBuffer() const;
  void setBuffer( const DungeonTile& tile );
  void setBuffer( const SelectiveDungeonTile& tile );

private:
  int mWidth;
  int mHeight;
  DungeonTile* buffer;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * DungeonTile, SelectiveDungeonTile
// *
// * desc: contains the basic DungeonTile struct and the IncompleteDungeonTile class
// * the SelectiveDungeonTile is a wrapper for the DungeonTile fields that allows for
// * only certain fields to be set. This is to aid util functions, allowing them to
// * eg paint only certain fields in a tile
// *
// * WARNING: these are kept in the same files to remind of their inextricable nature.
// * They must be kept in sync, or behavior is undefined.
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#define DEFAULT_TILE_FIELD_VALUE = -1;

enum {
	DUNGEON_TILE_WALL,
	DUNGEON_TILE_FLOOR,
	DUNGEON_TILE_WATER
};

enum {
	DUNGEON_TILE_SPECIAL_NONE,
	DUNGEON_TILE_SPECIAL_MONSTER_GENERATOR
};


struct DungeonTile {
	DungeonTile() :
		special( DUNGEON_TILE_SPECIAL_NONE ) {}

	int type;
	int id;
	int floor;
	int ceiling;
	int special;
};


class SelectiveDungeonTile {
public:
	SelectiveDungeonTile();
	~SelectiveDungeonTile();

	int* getType();
	void setType( int type);
	void unsetType();

	int* getId();
	void setId( int id );
	void unsetId();

	int* getFloor();
	void setFloor( int floor );
	void unsetFloor();

	int* getCeiling();
	void setCeiling( int ceiling );
	void unsetCeiling();

	int* getSpecial();
	void setSpecial( int special );
	void unsetSpecial();

	void setFromDungeonTile( const DungeonTile& tile );
	DungeonTile getDungeonTile() const;
	void updateDungeonTile( DungeonTile& otherTile ) const;

private:
	DungeonTile dungeonTile;

	bool isTypeSet;
	bool isIdSet;
	bool isFloorSet;
	bool isCeilingSet;
	bool isSpecialSet;
};

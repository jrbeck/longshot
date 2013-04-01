#include "DungeonTile.h"

SelectiveDungeonTile::SelectiveDungeonTile() :
isTypeSet( false ),
isIdSet( false ),
isFloorSet( false ),
isCeilingSet( false ),
isSpecialSet( false )
{
}

SelectiveDungeonTile::~SelectiveDungeonTile() {
}

// type * * * * * * * * * * * *
int* SelectiveDungeonTile::getType() {
	if ( isTypeSet ) {
		return &dungeonTile.type;
	}
	return 0;
}

void SelectiveDungeonTile::setType( int type) {
	dungeonTile.type = type;
	isTypeSet = true;
}

void SelectiveDungeonTile::unsetType() {
	isTypeSet = false;
}

// id * * * * * * * * * * * *
int* SelectiveDungeonTile::getId() {
	if ( isIdSet ) {
		return &dungeonTile.id;
	}
	return 0;
}

void SelectiveDungeonTile::setId( int id ) {
	dungeonTile.id = id;
	isIdSet = true;
}

void SelectiveDungeonTile::unsetId() {
	isIdSet = false;
}

// floor * * * * * * * * * * * *
int* SelectiveDungeonTile::getFloor() {
	if ( isFloorSet ) {
		return &dungeonTile.floor;
	}
	return 0;
}

void SelectiveDungeonTile::setFloor( int floor ) {
	dungeonTile.floor = floor;
	isFloorSet = true;
}

void SelectiveDungeonTile::unsetFloor() {
	isFloorSet = false;
}

// ceiling * * * * * * * * * * * *
int* SelectiveDungeonTile::getCeiling() {
	if ( isCeilingSet ) {
		return &dungeonTile.ceiling;
	}
	return 0;
}

void SelectiveDungeonTile::setCeiling( int ceiling ) {
	dungeonTile.ceiling = ceiling;
	isCeilingSet = true;
}

void SelectiveDungeonTile::unsetCeiling() {
	isCeilingSet = false;
}

// special * * * * * * * * * * * *
int* SelectiveDungeonTile::getSpecial() {
	if ( isSpecialSet ) {
		return &dungeonTile.special;
	}
	return 0;
}

void SelectiveDungeonTile::setSpecial( int special ) {
	dungeonTile.special = special;
	isSpecialSet = true;
}

void SelectiveDungeonTile::unsetSpecial() {
	isSpecialSet = false;
}


void SelectiveDungeonTile::setFromDungeonTile( const DungeonTile& tile ) {
	setType( tile.type );
	setId( tile.id );
	setFloor( tile.floor );
	setCeiling( tile.ceiling );
	setSpecial( tile.special );
}

DungeonTile SelectiveDungeonTile::getDungeonTile() const {
	DungeonTile tile;
	if( isTypeSet ) {
		tile.type = dungeonTile.type;
	}

	if( isIdSet ) {
		tile.id = dungeonTile.id;
	}

	if( isFloorSet ) {
		tile.floor = dungeonTile.floor;
	}

	if( isCeilingSet ) {
		tile.ceiling = dungeonTile.ceiling;
	}

	if( isSpecialSet ) {
		tile.special = dungeonTile.special;
	}
	else {
		tile.special = DUNGEON_TILE_SPECIAL_NONE;
	}

	return tile;
}

void SelectiveDungeonTile::updateDungeonTile( DungeonTile& otherTile ) const {
	if( isTypeSet ) {
		otherTile.type = dungeonTile.type;
	}

	if( isIdSet ) {
		otherTile.id = dungeonTile.id;
	}

	if( isFloorSet ) {
		otherTile.floor = dungeonTile.floor;
	}

	if( isCeilingSet ) {
		otherTile.ceiling = dungeonTile.ceiling;
	}

	if( isSpecialSet ) {
		otherTile.special = dungeonTile.special;
	}
}
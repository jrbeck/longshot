#include "DungeonTile.h"

SelectiveDungeonTile::SelectiveDungeonTile() :
isTypeSet( 0 ),
isIdSet( 0 ),
isFloorSet( 0 ),
isCeilingSet( 0 )
{
}

SelectiveDungeonTile::~SelectiveDungeonTile() {
}

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

void SelectiveDungeonTile::setFromDungeonTile( const DungeonTile& tile ) {
	setType( tile.type );
	setId( tile.id );
	setFloor( tile.floor );
	setCeiling( tile.ceiling );
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
}
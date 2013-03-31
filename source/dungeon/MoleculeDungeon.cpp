#include "MoleculeDungeon.h"


void MoleculeDungeon::createDungeon( DungeonUtil& dungeon, v2di_t& startPosition ) {
	if( dungeon.getDungeonModel() == 0 ) {
		return;
	}

	int dungeonWidth = dungeon.getDungeonModel()->getWidth();
	int dungeonHeight = dungeon.getDungeonModel()->getHeight();

	SelectiveDungeonTile floorTile;
	floorTile.setType( DUNGEON_TILE_FLOOR );
	SelectiveDungeonTile wallTile;
	wallTile.setType( DUNGEON_TILE_WALL );

	// fill it with wall
	dungeon.setAllTiles( wallTile );

	//// draw central room
	dungeon.drawFilledCircle(
		dungeonWidth / 2,
		dungeonHeight / 2,
		8,
		floorTile );


	for( int i = 0; i < 5; i ++ ) {
		int x = r_numi( 0, dungeonWidth );
		int y = r_numi( 0, dungeonHeight );

		dungeon.drawFilledCircle(
			x,
			y,
			r_num( 5.0, 8.0 ),
			floorTile );

		dungeon.drawCrookedLine(
			x,
			y,
			dungeonWidth / 2,
			dungeonHeight / 2,
			4, // subdiv
			10.0, // displacement
			1.0, // brushSize
			floorTile );
	}

	dungeon.drawCrookedLine(
		0, //r_numi( 0, ROGUE_MAP_SIDE ),
		r_numi( 0, dungeonHeight ),
		dungeonWidth, //r_numi( 0, ROGUE_MAP_SIDE ),
		r_numi( 0, dungeonHeight ),
		4, // subdiv
		3.0, // displacement
		0.8, // brushSize
		floorTile );

	// draw a wall around the whole thing
	dungeon.drawRect(
		0,
		0,
		dungeonWidth - 1,
		dungeonHeight - 1,
		wallTile );
}

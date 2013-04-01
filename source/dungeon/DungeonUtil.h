// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * DungeonUtil
// *
// * desc: general utility and drawing routines for the Dungeon classes
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "v2d.h"
#include "DungeonModel.h"
#include "MathUtil.h"

#define DISPLACEMENT_SCALE ( 0.7 ) // this is how the maxDisplacement changes as it recurses in drawCrookedLine


class DungeonUtil {
public:
	DungeonUtil();
	~DungeonUtil();

	void createDungeonModel( int width, int height );
	void destroyDungeonModel();
	DungeonModel* getDungeonModel() const;

	bool isOnMap ( int i, int j ) const;

	// wrapper method for DungeonModel methods
	DungeonTile* getTile( int i, int j ) const;
	void setTile( int i, int j, const SelectiveDungeonTile& tile );
	void setAllTiles( const SelectiveDungeonTile& tile );

	void drawHorizontalLine( int x1, int x2, int y, const SelectiveDungeonTile& tile );
	void drawVerticalLine( int x, int y1, int y2, const SelectiveDungeonTile& tile );
//	void drawLine( int x1, int y1, int x2, int y2, float brushSize, int steps, const SelectiveDungeonTile& tile );
	void drawLine( double x1, double y1, double x2, double y2, float brushSize, int steps, const SelectiveDungeonTile& tile );
//	void drawCrookedLine( int x1, int y1, int x2, int y2, int subdivisons, float maxDisplacement, float brushSize, const SelectiveDungeonTile& tile );
	void drawCrookedLine( double x1, double y1, double x2, double y2, int subdivisons, float maxDisplacement, float brushSize, const SelectiveDungeonTile& tile );

	void drawRect( int x1, int y1, int x2, int y2, const SelectiveDungeonTile& tile );
	void drawFilledRect( int x1, int y1, int x2, int y2, const SelectiveDungeonTile& tile );
	void drawFilledRect( int x1, int y1, int x2, int y2, const SelectiveDungeonTile& fillTile, const SelectiveDungeonTile& borderTile );

	void drawCircle( int x, int y, float radius, const SelectiveDungeonTile& tile );
	void drawFilledCircle( int x, int y, float radius, const SelectiveDungeonTile& tile );
	void drawFilledCircle( int x, int y, float radius, const SelectiveDungeonTile& fillTile, const SelectiveDungeonTile& tile );
	
	void drawArc( int x, int y, float radius, float startAngle, float arcAngle, float brushSize, int steps, const SelectiveDungeonTile& tile );
	void drawSpiral( int x, int y, float brushSize, float startAngle, float rotation, float startRadius, float endRadius, int steps, const SelectiveDungeonTile& tile );

private:

	// math stuff
	int round_int( float r ) const;
	float dist( int x1, int y1, int x2, int y2 ) const;
	v2di_t arcLerp( int x, int y, float radius, float startAngle, float rotation, float percent ) const;
	void orderAscending( int &a, int &b) const;

	DungeonModel* dungeon;
};

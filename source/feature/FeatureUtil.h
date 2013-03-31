// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * FeatureUtil
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "World.h"


enum {
	FG_CORNER_SW,
	FG_CORNER_NW,
	FG_CORNER_NE,
	FG_CORNER_SE,

	FG_NUM_CORNERS
};



struct height_info_t {
	int high;
	int low;
	int avg;
};


class FeatureUtil {
public:
	static void loadWorldRegion(v2di_t cornerIndex, int sideLength, World &world, bool doOutcroppings);
	static height_info_t getHeightInfo (int worldX, int worldZ, int sideX, int sideZ, World &world);

	static void buildSpiralStaircase (v3di_t southwestInsideCorner, int stairsPerLevel, int topHeight,
		int startCorner, WorldMap &worldMap);

};


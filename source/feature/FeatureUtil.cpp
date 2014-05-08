#include "FeatureUtil.h"


void FeatureUtil::loadWorldRegion (
	v2di_t cornerIndex,
	int sideLength,
	World &world,
	bool doOutcroppings)
{
	world.getWorldMap()->resize (sideLength, sideLength);

	for (int k = 0; k < sideLength; k++) {
		for (int i = 0; i < sideLength; i++) {
			world.loadColumn(cornerIndex.x + i, cornerIndex.y + k, doOutcroppings);
		}
	}
}


// this asks a Periodics (perhaps indirectly) for info about the terrain height
// the height_info_t posseses a low, high, avg
height_info_t FeatureUtil::getHeightInfo (int worldX, int worldZ, int sideX, int sideZ, World &world) {
	height_info_t heightInfo;
	heightInfo.low = 100000;
	heightInfo.high = -100000;

	int sum = 0;
	int count = 0;

	for (int k = 0; k < sideZ; k++) {
		for (int i = 0; i < sideX; i++) {
			int height = world.getTerrainHeight (worldX + i, worldZ + k);
			sum += height;
			count++;

			if (height < heightInfo.low) {
//				heightInfo.low = (height > (int)SEA_FLOOR_HEIGHT ? height : (int)SEA_FLOOR_HEIGHT);
				heightInfo.low = height;
			}
			else if (height > heightInfo.high) {
//				heightInfo.high = (height > (int)SEA_FLOOR_HEIGHT ? height : (int)SEA_FLOOR_HEIGHT);
				heightInfo.high = height;
			}
		}
	}

	if (count > 0) {
		heightInfo.avg = sum / count;
	}

	return heightInfo;
}



void FeatureUtil::buildSpiralStaircase(
	v3di_t southwestInsideCorner,
	int stairsPerLevel,
	int topHeight,
	int startCorner,
	WorldMap &worldMap)
{
	int totalHeight = (topHeight + 1) - southwestInsideCorner.y;
	int currentHeight = southwestInsideCorner.y;
	int numLevels =  totalHeight / stairsPerLevel;
	int lastLevelHeight = totalHeight % stairsPerLevel;

	if (lastLevelHeight != 0) numLevels++;

	v3di_t insideCorner;
	v3di_t outsideCorner;

	int stairXAdd;
	int stairZAdd;

	int spiralXAdd;
	int spiralZAdd;

	if (startCorner == FG_CORNER_SW) {
		spiralXAdd = 0;
		spiralZAdd = 1;

		insideCorner = southwestInsideCorner;
	}
	else if (startCorner == FG_CORNER_NW) {
		spiralXAdd = 1;
		spiralZAdd = 0;

		insideCorner.x = southwestInsideCorner.x;
		insideCorner.y = southwestInsideCorner.y;
		insideCorner.z = southwestInsideCorner.z + stairsPerLevel;
	}
	else if (startCorner == FG_CORNER_NE) {
		spiralXAdd = 0;
		spiralZAdd = -1;

		insideCorner.x = southwestInsideCorner.x + stairsPerLevel;
		insideCorner.y = southwestInsideCorner.y;
		insideCorner.z = southwestInsideCorner.z + stairsPerLevel;
	}
	else { // (startCorner == FG_CORNER_SE) {
		spiralXAdd = -1;
		spiralZAdd = 0;

		insideCorner.x = southwestInsideCorner.x + stairsPerLevel;
		insideCorner.y = southwestInsideCorner.y;
		insideCorner.z = southwestInsideCorner.z;
	}



	// let's trace the outer spiral and make them point inwards
	for (int level = 0; level < numLevels; level++) {
		if (lastLevelHeight != 0 && level == (numLevels - 1)) { // last level
			stairsPerLevel = lastLevelHeight;
		}
		for (int stair = 0; stair < stairsPerLevel; stair++) {
			if (stair == 0) { // landing
				// first is 2x2
				if (spiralXAdd == 0) {
					if (spiralZAdd == -1) {
						stairXAdd = 1;
						stairZAdd = 1;
					}
					else {
						stairXAdd = -1;
						stairZAdd = -1;
					}
				}
				else {
					if (spiralXAdd == -1) {
						stairXAdd = 1;
						stairZAdd = -1;
					}
					else {
						stairXAdd = -1;
						stairZAdd = 1;
					}
				}
			}
			else { // stair
				// all after are 2x1
				if (spiralXAdd == 0) {
					if (spiralZAdd == -1) {
						stairXAdd = 1;
						stairZAdd = 0;
					}
					else {
						stairXAdd = -1;
						stairZAdd = 0;
					}
				}
				else {
					if (spiralXAdd == -1) {
						stairXAdd = 0;
						stairZAdd = -1;
					}
					else {
						stairXAdd = 0;
						stairZAdd = 1;
					}
				}
			}

			outsideCorner.x = insideCorner.x + stairXAdd;
			outsideCorner.y = insideCorner.y;
			outsideCorner.z = insideCorner.z + stairZAdd;

			// draw
			worldMap.fillVolume (insideCorner, outsideCorner, BLOCK_TYPE_LIGHT_STONE); //BLOCK_TYPE_WHITE_MARBLE);

			insideCorner.x += spiralXAdd;
			insideCorner.y++;
			insideCorner.z += spiralZAdd;

		}

		// turn (clockwise)
		if (spiralXAdd == 0) {
			if (spiralZAdd == -1) {
				spiralXAdd = -1;
			}
			else {
				spiralXAdd = 1;
			}
			spiralZAdd = 0;
		}
		else {
			if (spiralXAdd == -1) {
				spiralZAdd = 1;
			}
			else {
				spiralZAdd = -1;
			}
			spiralXAdd = 0;
		}
	}
}

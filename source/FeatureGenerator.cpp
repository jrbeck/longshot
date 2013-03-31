#include "FeatureGenerator.h"

void FeatureGenerator::createSetPieces(int xIndex, int zIndex, World& world) {
	// this is the RogueMap used to place set pieces on the world
	RogueMap worldRogueMap;
	worldRogueMap.resize (ROGUE_W, ROGUE_H);
	worldRogueMap.clear ();

	// this stores any items that are generated
	InactiveList inactiveList;

	v2di_t mapIndex = v2di_v(xIndex, zIndex);

	LoadScreen loadScreen;
	loadScreen.setCompletedColor(0.6f, 0.0f, 0.8f);
	loadScreen.setIncompletedColor(0.3f, 0.0f, 0.4f);
	loadScreen.start();

	loadScreen.draw( 0, 6 );

	// okay, let's get that WorldMap
	WorldMap &worldMap = *world.getWorldMap();

	for (int i = 0; i < 6; i++) {
		v2di_t corner;

		corner = worldRogueMap.random_room (ID_START + i, 8, 8);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("dungeon at (%d, %d)\n", corner.x, corner.y);
			createDungeon (corner, 3, world);
//			createPlain (corner, 8, 8, worldMap);

			worldMap.swapOutToInactive ();
		}
	}

	loadScreen.draw(1, 6);

	for (int i = 0; i < 1; i++) {
		v2di_t corner;

		int side = 8;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("great pyramid at (%d, %d)\n", corner.x, corner.y);

			loadWorldRegion(corner, side, world, false);
			createPyramid (side, corner, world);

			worldMap.swapOutToInactive ();
		}
	}

	loadScreen.draw(2, 6);

	for (int i = 0; i < 1; i++) {
		v2di_t corner;

		int side = 16;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("village at (%d, %d)\n", corner.x, corner.y);
			createVillage (side, corner, world);

//			createPlain (corner, side, side, worldMap);

			worldMap.swapOutToInactive ();
		}
	}

	loadScreen.draw(3, 6);

	// hmmm
	for (int i = 0; i < 6; i++) {
		v2di_t corner;

		int side = 6;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("spiral garden at (%d, %d)\n", corner.x, corner.y);
//			createPyramid (side, corner, worldMap);
//			createPlain (corner, side, side, worldMap);

			loadWorldRegion(corner, side, world, false);

//			height_info_t hI = getHeightInfo (corner.x * WORLD_CHUNK_SIDE, corner.y * WORLD_CHUNK_SIDE,
//				side * WORLD_CHUNK_SIDE, side * WORLD_CHUNK_SIDE, worldMap);

			growSpiralGarden (corner, side, side, world);


			worldMap.swapOutToInactive ();
		}
	}


	// CAVERNS
	for (int i = 0; i < 4; i++) {
		v2di_t corner;

		int side = 6;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("cavern system at (%d, %d)\n", corner.x, corner.y);

			loadWorldRegion(corner, side, world, false);

			drillCavern (corner, side, side, world);

			worldMap.swapOutToInactive ();
		}
	}

	loadScreen.draw(4, 6);

	// castle?
	for (int i = 0; i < 1; i++) {
		v2di_t corner;

		int side = 8;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);
			printf ("castle at (%d, %d)\n", corner.x, corner.y);
			createPlain(corner, side, side, world);

			height_info_t hI = getHeightInfo (corner.x * WORLD_CHUNK_SIDE, corner.y * WORLD_CHUNK_SIDE,
				side * WORLD_CHUNK_SIDE, side * WORLD_CHUNK_SIDE, world);

			createCastle8x8 (corner, world);

			worldMap.swapOutToInactive ();
		}
	}

	loadScreen.draw(5, 6);

	for (int i = 0; i < 50; i++) {
		int side = 1;

		v2di_t corner;

		corner = worldRogueMap.random_room (ID_START + i, side, side);

		if (corner.x != 0) {
			corner = v2di_add (corner, mapIndex);

			height_info_t heightInfo = getHeightInfo (corner.x * WORLD_CHUNK_SIDE, corner.y * WORLD_CHUNK_SIDE,
				side * WORLD_CHUNK_SIDE, side * WORLD_CHUNK_SIDE, world);

			if (heightInfo.low > 2) {
				printf ("house at (%d, %d)\n", corner.x, corner.y);

				createPlain(corner, side, side, world);

				createHouse(corner, world);
//				createPlain (corner, 1, 1, worldMap);
//				create1by1Tower (corner, worldMap);

				worldMap.swapOutToInactive ();
			}
		}
	}

	loadScreen.finish();

	inactiveList.saveToDisk ("save/inactive.list");
	inactiveList.clear ();
}



void FeatureGenerator::loadWorldRegion (
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
height_info_t FeatureGenerator::getHeightInfo (int worldX, int worldZ, int sideX, int sideZ, World &world) {
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



void FeatureGenerator::createPyramid (int side, v2di_t cornerIndex, World &world) {
	// WARNING: relative to 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int width = side * WORLD_CHUNK_SIDE;
	int halfWidth = width >> 1;

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, width, width, world);

	v3di_t baseNearPoint = {
		worldX,
		heightInfo.low,
		worldZ };

	drawPyramid (baseNearPoint, side * WORLD_CHUNK_SIDE, BLOCK_TYPE_OBSIDIAN, world);
}



void FeatureGenerator::create1by1Tower (v2di_t cornerIndex, int baseHeight, WorldMap &worldMap) {
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

//	height_info_t heightInfo = getHeightInfo (worldX, worldZ, WORLD_CHUNK_SIDE, WORLD_CHUNK_SIDE, worldMap);

//	createPlain (cornerIndex, 1, 1, worldMap);

	// now build the tower
	v3di_t a, b;

	//heightInfo.avg++;
	int roofHeight = 25;

	// fill it
	a.x = worldX + 3;
	a.y = baseHeight;
	a.z = worldZ + 3;

	b.x = worldX + 12;
	b.y = baseHeight + roofHeight;
	b.z = worldZ + 12;

	worldMap.fillVolume (a, b, BLOCK_TYPE_OLD_BRICK);

	// hollow it out
	a.x = worldX + 4;
	a.y = baseHeight;
	a.z = worldZ + 4;

	b.x = worldX + 11;
	b.y = baseHeight + roofHeight;
	b.z = worldZ + 11;

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);

	// make the door
	a.x = worldX + 3;
	a.z = worldZ + 7;
	a.y = baseHeight;

	b.x = worldX + 3;
	b.z = worldZ + 8;
	b.y = a.y;

	for (int i = 0; i < 3; i++) {
		worldMap.clearBlock (a);
		worldMap.clearBlock (b);
		a.y++;
		b.y++;
	}

	// draw a couple stairs
//	v3di_t southwestInsideCorner = v3di_v (worldX + 6, heightInfo.avg, worldZ + 6);
//	buildSpiralStaircase (southwestInsideCorner, 3, heightInfo.avg + 26,
//		r_numi (0, FG_NUM_CORNERS), worldMap);

	v3di_t southwestInsideCorner = v3di_v (worldX + 5, baseHeight, worldZ + 5);
	buildSpiralStaircase (southwestInsideCorner, 5, baseHeight + 26,
		r_numi (0, FG_NUM_CORNERS), worldMap);

//	v3di_t southwestInsideCorner = v3di_v (worldX + 3, heightInfo.avg, worldZ + 3);
//	buildSpiralStaircase (southwestInsideCorner, 9, heightInfo.avg + 27, FG_CORNER_SW, worldMap);


//	worldMap.swapOutToInactive ();

	

}




void FeatureGenerator::buildSpiralStaircase (v3di_t southwestInsideCorner,
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


void FeatureGenerator::drillSpiral (
	v3d_t top,
	double radius,
	double height,
	double totalRotation,
	int numSteps,
	WorldMap &worldMap)
{
	v3d_t pos;

	double bottom = top.y - height;
	double percent;
	double angle;

	for (int step = 0; step < numSteps; step++) {
		percent = static_cast<double> (step) / static_cast<double>(numSteps - 1);
		angle = percent * totalRotation;

		pos.x = top.x + radius * cos (angle);
		pos.y = lerp (bottom, top.y, percent);
		pos.z = top.z + radius * sin (angle);

		worldMap.clearSphere (pos, 3.0);
	}
}





void FeatureGenerator::drillCavern (
	v2di_t cornerIndex,
	int sideX,
	int sideZ,
	World &world)
{
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int sideLengthX = sideX * WORLD_CHUNK_SIDE;
	int sideLengthZ = sideZ * WORLD_CHUNK_SIDE;

//	height_info_t heightInfo = getHeightInfo (worldX, worldZ, sideLengthX, sideLengthZ, worldMap);

	double halfSideX = (double)sideLengthX / 2;
	double halfSideZ = (double)sideLengthZ / 2;

	v3d_t centerPos = {
		(double)worldX + halfSideX,
		0.0,
		(double)worldZ + halfSideZ
	};

	v3d_t pos;

	for (int j = 0; j < 6; j++) {
		pos.x = centerPos.x + r_num(-halfSideX * 0.9, halfSideX * 0.9);
		pos.z = centerPos.z + r_num(-halfSideZ * 0.9, halfSideZ * 0.9);
		pos.y = (double)(world.getTerrainHeight ((int)pos.x, (int)pos.z));

		double angle = r_num(0.0, MY_2PI);

		v3d_t vel = {
			cos(angle),
			0.5,
			sin(angle)
		};

		double radius = r_num(1.0, 4.0);

		WorldMap &worldMap = *world.getWorldMap();
		for (int i = 0; i < 75; i++) {
//			worldMap.clearSphere (pos, 4.0);
			worldMap.fillSphere (pos, radius, BLOCK_TYPE_DARK_PURPLE, LIGHT_LEVEL_SOLID);

			pos = v3d_add(pos, vel);

			radius += r_num(-0.1, 0.1);
			if (radius < 1.0) {
				radius = 1.0;
			}
			if (radius > 4.0) {
				radius = 4.0;
			}

			vel.x += r_num(-0.1, 0.1);
			vel.z += r_num(-0.1, 0.1);

			double delta = ((double)worldX + halfSideX) - pos.x;
			if (abs(delta) > (double)sideLengthX * 0.4) {
				vel.x = -vel.x;
			}

			delta = ((double)worldZ + halfSideZ) - pos.z;
			if (abs(delta) > (double)sideLengthZ * 0.4) {
				vel.z = -vel.z;
			}
		}
	}
}








void FeatureGenerator::growSpiralGarden (
	v2di_t cornerIndex,
	int sideX,
	int sideZ,
	World &world)
{


	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int sideLengthX = sideX * WORLD_CHUNK_SIDE;
	int sideLengthZ = sideZ * WORLD_CHUNK_SIDE;

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, sideLengthX, sideLengthZ, world);

	int blockType;
	int numSpirals;
	int blockRNum = r_numi (0, 4);
	switch (blockRNum) {
		case 0:
			blockType = BLOCK_TYPE_DARK_PURPLE;
			numSpirals = r_numi (20, 35);
			break;

		case 1:
			blockType = BLOCK_TYPE_FUSCHIA;
			numSpirals = r_numi (3, 15);
			break;

		case 2:
			blockType = BLOCK_TYPE_NAVY_BLUE;
			numSpirals = r_numi (50, 100);
			break;
				
		case 3:
		default:
			blockType = BLOCK_TYPE_BRICK_RED;
			numSpirals = r_numi (7, 15);
			break;
	}

	WorldMap &worldMap = *world.getWorldMap();
	for (int i = 0; i < numSpirals; i++) {
		v3d_t top;

		double height;

		top.x = worldX + r_num (10.0, static_cast<double>(sideLengthX) - 10.0);
		top.z = worldZ + r_num (10.0, static_cast<double>(sideLengthZ) - 10.0);
		double bottom = world.getTerrainHeight (static_cast<int>(top.x), static_cast<int>(top.z));

		v3d_t pos;

		double percent;
		double angle;
		int numSteps = 100;
		double totalRotation;
		double radius;

		double sphereRadiusStart = 2.1;
		double sphereRadiusEnd = 1.1;
		double sphereRadius;
		

		switch (blockType) {
			case BLOCK_TYPE_DARK_PURPLE:
				radius = r_num (2.0, 4.0);
				totalRotation = r_num (2.0, 4.0) * 2.0;
				height = r_num (15.0, 30.0);
				break;

			case BLOCK_TYPE_SLUDGE:
				radius = r_num (4.0, 7.0);
				totalRotation = r_num (2.0, 4.0) * 6.28;
				height = r_num (35.0, 75.0);
				sphereRadiusStart = 2.5;
				sphereRadiusEnd = 4.0;
				break;

			case BLOCK_TYPE_NAVY_BLUE:
				radius = r_num (2.0, 3.0);
				totalRotation = r_num (2.0, 4.0) * 2.0;
				height = r_num (10.0, 20.0);
				sphereRadiusStart = 2.5;
				sphereRadiusEnd = 1.0;
				break;
				
			case BLOCK_TYPE_BRICK_RED:
			default:
				radius = r_num (3.0, 6.0);
				totalRotation = r_num (2.0, 4.0) * 6.28;
				height = r_num (10.0, 25.0);
				break;
		}

		top.y = bottom + height;

		for (int step = 0; step < numSteps; step++) {
			percent = static_cast<double> (step) / static_cast<double>(numSteps - 1);
			angle = percent * totalRotation;

			pos.x = top.x + radius * cos (angle);
			pos.y = lerp (bottom, top.y, percent);
			pos.z = top.z + radius * sin (angle);

			sphereRadius = lerp (sphereRadiusStart, sphereRadiusEnd, percent);

			worldMap.fillSphere (pos, sphereRadius, blockType, LIGHT_LEVEL_SOLID);
		}

	}
}







void FeatureGenerator::createCastle8x8 (v2di_t cornerIndex, World &world) {
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int sideLength = 8 * WORLD_CHUNK_SIDE;

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, sideLength, sideLength, world);

	WorldMap &worldMap = *world.getWorldMap();


	v3di_t a, b;
//	heightInfo.avg++;
	int roofHeight = 20;

	// fill it
	a.x = worldX + WORLD_CHUNK_SIDE + 4;
	a.y = heightInfo.avg + 1;
	a.z = worldZ + WORLD_CHUNK_SIDE + 4;

	b.x = worldX + (7 * WORLD_CHUNK_SIDE) - 5;
	b.y = heightInfo.avg + roofHeight;
	b.z = worldZ + (7 * WORLD_CHUNK_SIDE) - 5;

	worldMap.fillVolume (a, b, BLOCK_TYPE_OLD_BRICK);

	// empty it
	a.x = worldX + WORLD_CHUNK_SIDE + 12;
	a.y = heightInfo.avg + 1;
	a.z = worldZ + WORLD_CHUNK_SIDE + 12;

	b.x = worldX + (7 * WORLD_CHUNK_SIDE) - 13;
	b.y = heightInfo.avg + roofHeight;
	b.z = worldZ + (7 * WORLD_CHUNK_SIDE) - 13;

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);


	// clear the entrance
	a.x = worldX + WORLD_CHUNK_SIDE + 4;
	a.y = heightInfo.avg + 1;
	a.z = worldZ + (WORLD_CHUNK_SIDE * 3) + 12;

	b.x = worldX + WORLD_CHUNK_SIDE + 12;
	b.y = heightInfo.avg + roofHeight - 10;
	b.z = worldZ + (5 * WORLD_CHUNK_SIDE) - 13;

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);


	// fill the keep
	a.x = worldX + (WORLD_CHUNK_SIDE * 3) + 4;
	a.y = heightInfo.avg + 1;
	a.z = worldZ + (WORLD_CHUNK_SIDE * 3) + 4;

	b.x = worldX + (5 * WORLD_CHUNK_SIDE) - 5;
	b.y = heightInfo.avg + roofHeight + 15;
	b.z = worldZ + (5 * WORLD_CHUNK_SIDE) - 5;

	worldMap.fillVolume (a, b, BLOCK_TYPE_OLD_BRICK);


//	heightInfo.avg++;


	v2di_t towerCorner;

	towerCorner = v2di_v (cornerIndex.x + 1, cornerIndex.y + 1);
	create1by1Tower (towerCorner, heightInfo.avg + 1, worldMap);

	towerCorner = v2di_v (cornerIndex.x + 6, cornerIndex.y + 1);
	create1by1Tower (towerCorner, heightInfo.avg + 1, worldMap);

	towerCorner = v2di_v (cornerIndex.x + 1, cornerIndex.y + 6);
	create1by1Tower (towerCorner, heightInfo.avg + 1, worldMap);

	towerCorner = v2di_v (cornerIndex.x + 6, cornerIndex.y + 6);
	create1by1Tower (towerCorner, heightInfo.avg + 1, worldMap);

}



void FeatureGenerator::createHouse (v2di_t cornerIndex, World &world) {
	// WARNING: this always corners at 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, WORLD_CHUNK_SIDE, WORLD_CHUNK_SIDE, world);

	// now build the house
	v3di_t a, b;

	int floorHeight = heightInfo.avg + 1;
	int roofHeight = 5;
	int sideLength = 7;
	int offset = 4;

	// make a roof
	a.x = worldX + (offset - 1);
	a.y = floorHeight + roofHeight;
	a.z = worldZ + (offset - 1);

	drawPyramid (a, sideLength + 2, BLOCK_TYPE_LIGHT_BLUE, world);

	WorldMap &worldMap = *world.getWorldMap();

	// fill it
	a.x = worldX + offset;
	a.y = floorHeight;
	a.z = worldZ + 4;

	b.x = worldX + offset + sideLength;
	b.y = floorHeight + roofHeight;
	b.z = worldZ + offset + sideLength;

	worldMap.fillVolume (a, b, BLOCK_TYPE_LIGHT_STONE);

	// hollow it out
	a.x = worldX + offset + 1;
	a.y = floorHeight;
	a.z = worldZ + offset + 1;

	b.x = worldX + offset + (sideLength - 1);
	b.y = floorHeight + roofHeight - 1;
	b.z = worldZ + offset + (sideLength - 1);

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);

	// make the door
	a.x = worldX + offset;
	a.y = floorHeight;
	a.z = worldZ + offset + 3;

	b.x = worldX + offset;
	b.y = floorHeight + 2;
	b.z = worldZ + offset + 4;

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);
}



void FeatureGenerator::createBuilding2x2 (v2di_t cornerIndex, World &world) {
	// WARNING: this always corners at 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int xSide = 2;
	int zSide = 2;

	height_info_t heightInfo = getHeightInfo (
		worldX, worldZ,
		WORLD_CHUNK_SIDE * xSide, WORLD_CHUNK_SIDE * zSide,
		world);
	WorldMap &worldMap = *world.getWorldMap();

	// now build the house
	v3di_t a, b;

	int floorHeight = heightInfo.low - 1;
	int roofHeight = 80;

	// hollow it out
	a.x = worldX + 5;
	a.y = floorHeight;
	a.z = worldZ + 5;

	b.x = worldX + 10 + (WORLD_CHUNK_SIDE * xSide);
	b.y = floorHeight + roofHeight - 1;
	b.z = worldZ + 10 + (WORLD_CHUNK_SIDE * zSide);

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);

	// fill it
	a.x = worldX + 4;
	a.y = floorHeight;
	a.z = worldZ + 4;

	b.x = worldX + 11 + (WORLD_CHUNK_SIDE * xSide);
	b.y = floorHeight + roofHeight;
	b.z = worldZ + 11 + (WORLD_CHUNK_SIDE * zSide);

	worldMap.fillVolume (a, b, BLOCK_TYPE_STONE);

	// hollow it out
	a.x = worldX + 5;
	a.y = floorHeight;
	a.z = worldZ + 5;

	b.x = worldX + 10 + (WORLD_CHUNK_SIDE * xSide);
	b.y = floorHeight + roofHeight - 1;
	b.z = worldZ + 10 + (WORLD_CHUNK_SIDE * zSide);

	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);


	// make the door
	a.x = worldX + 4;
	a.z = worldZ + 7;
	a.y = floorHeight;

	b.x = worldX + 4;
	b.z = worldZ + 8;
	b.y = a.y;

	for (int i = 0; i < 3; i++) {
		worldMap.clearBlock (a);
		worldMap.clearBlock (b);
		a.y++;
		b.y++;
	}

//	worldMap.swapOutToInactive ();
}





void FeatureGenerator::createPlain (v2di_t cornerIndex, int sideX, int sideZ, World &world) {
	// WARNING: this always corners at 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	height_info_t heightInfo = getHeightInfo (
		worldX, worldZ,
		WORLD_CHUNK_SIDE * sideX, WORLD_CHUNK_SIDE * sideZ,
		world);

	WorldMap &worldMap = *world.getWorldMap();

	worldMap.resize (sideX, sideZ);

	int bufferX = (WORLD_CHUNK_SIDE * sideX) + 2;
	int bufferZ = (WORLD_CHUNK_SIDE * sideZ) + 2;

	int *heightMap = new int[bufferX * bufferZ];
	if (heightMap == NULL) {
		printf ("FeatureGenerator::createPlain(): error: out of mem 1\n");
		return;
	}

	int floorHeight = heightInfo.avg;

	// make it a plain
	for (int k = 0; k < bufferZ; k++) {
		for (int i = 0; i < bufferX; i++) {
			heightMap[i + (k * bufferX)] = floorHeight;
		}
	}

	int indices[4];

	// get actual terrain height for the two edge rows (x-axis)
	indices[0] = 0;
	indices[1] = bufferX;
	indices[2] = bufferX * (bufferZ - 2);
	indices[3] = bufferX * (bufferZ - 1);

	for (int i = 0; i < (WORLD_CHUNK_SIDE * sideX) + 2; i++) {
		int x = worldX + (i - 1);
		heightMap[i + indices[0]] = world.getTerrainHeight (x, worldZ - 1);
		heightMap[i + indices[1]] = world.getTerrainHeight (x, worldZ);
		heightMap[i + indices[2]] = world.getTerrainHeight (x, worldZ + (bufferZ - 2));
		heightMap[i + indices[3]] = world.getTerrainHeight (x, worldZ + (bufferZ - 1));
	}

	// now along the z-axis
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = bufferX - 2;
	indices[3] = bufferX - 1;

	for (int k = 0; k < (WORLD_CHUNK_SIDE * sideZ) + 2; k++) {
		int z = worldZ + (k - 1);
		heightMap[indices[0] + (k * bufferX)] = world.getTerrainHeight (worldX - 1, z);
		heightMap[indices[1] + (k * bufferX)] = world.getTerrainHeight (worldX, z);
		heightMap[indices[2] + (k * bufferX)] = world.getTerrainHeight (worldX + (bufferX - 2), z);
		heightMap[indices[3] + (k * bufferX)] = world.getTerrainHeight (worldX + (bufferX - 1), z);
	}

	// do a little smoothing
	indices[0] = bufferX * 2; // keep track of the writing
	indices[1] = bufferX * (bufferZ - 3);
	indices[2] = bufferX; // these are for the reading
	indices[3] = bufferX * (bufferZ - 2);

	for (int i = 2; i < (WORLD_CHUNK_SIDE * sideX); i++) {
		int x = worldX + (i - 1);
		heightMap[i + indices[0]] = (heightMap[i + indices[2]] + floorHeight) / 2;
		heightMap[i + indices[1]] = (heightMap[i + indices[3]] + floorHeight) / 2;
	}

	// and again along the z-axis
	indices[0] = 2; // writing
	indices[1] = bufferX - 3;
	indices[2] = 1; // reading
	indices[3] = bufferX - 2;

	for (int k = 2; k < (WORLD_CHUNK_SIDE * sideZ); k++) {
		int z = worldZ + (k - 1);
		heightMap[indices[0] + (k * bufferX)] = (heightMap[indices[2] + (k * bufferX)] + floorHeight) / 2;
		heightMap[indices[1] + (k * bufferX)] = (heightMap[indices[3] + (k * bufferX)] + floorHeight) / 2;
	}


	// do a little MORE smoothing
	indices[0] = bufferX * 3; // keep track of the writing
	indices[1] = bufferX * (bufferZ - 4);
	indices[2] = bufferX * 2; // these are for the reading
	indices[3] = bufferX * (bufferZ - 3);

	for (int i = 3; i < ((WORLD_CHUNK_SIDE * sideX) - 1); i++) {
		int x = worldX + (i - 1);
		heightMap[i + indices[0]] = (heightMap[i + indices[2]] + floorHeight) / 2;
		heightMap[i + indices[1]] = (heightMap[i + indices[3]] + floorHeight) / 2;
	}

	// and AGAIN along the z-axis
	indices[0] = 3; // writing
	indices[1] = bufferX - 4;
	indices[2] = 2; // reading
	indices[3] = bufferX - 3;

	for (int k = 3; k < ((WORLD_CHUNK_SIDE * sideZ) - 1); k++) {
		int z = worldZ + (k - 1);
		heightMap[indices[0] + (k * bufferX)] = (heightMap[indices[2] + (k * bufferX)] + floorHeight) / 2;
		heightMap[indices[1] + (k * bufferX)] = (heightMap[indices[3] + (k * bufferX)] + floorHeight) / 2;
	}



	int *columnBuffer = new int[(WORLD_CHUNK_SIDE + 2) * (WORLD_CHUNK_SIDE + 2)];
	if (columnBuffer == NULL) {
		printf ("FeatureGenerator::createPlain(): error: out of mem 2\n");
		delete [] heightMap;
		return;
	}

	// now build the columns
	for (int k = 0; k < sideZ; k++) {
		for (int i = 0; i < sideX; i++) {

			int xCorner = i * (WORLD_CHUNK_SIDE);
			int zCorner = k * (WORLD_CHUNK_SIDE);

			for (int w = 0; w < (WORLD_CHUNK_SIDE + 2); w++) {
				for (int u = 0; u < (WORLD_CHUNK_SIDE + 2); u++) {
					columnBuffer[u + (w * (WORLD_CHUNK_SIDE + 2))] = heightMap[(xCorner + u) + ((zCorner + w) * bufferX)];
				}
			}

			world.loadColumn (cornerIndex.x + i, cornerIndex.y + k, columnBuffer);
		}
	}

	delete [] columnBuffer;
	delete [] heightMap;

//	worldMap.swapOutToInactive ();
}



void FeatureGenerator::createVillage (int side, v2di_t cornerIndex, World &world) {
	// WARNING: relative to 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int width = side * WORLD_CHUNK_SIDE;

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, width, width, world);

	loadWorldRegion(cornerIndex, side, world, false);

	RogueMap rogueMap;
	rogueMap.resize (side, side);

	for (int i = 0; i < 50; i++) {

		int buildingSide = 2;

		v2di_t corner;

		corner = rogueMap.random_room (ID_START + i, buildingSide, buildingSide);

		if (corner.x != 0) {
			corner = v2di_add (corner, cornerIndex);

			printf ("feature at (%d, %d)\n", corner.x, corner.y);

//			createPlain (corner, buildingSide, buildingSide, worldMap);

			createPyramid (buildingSide, corner, world);
//			createBuilding2x2 (corner, worldMap);
		}
	}

	// roads?

}


void FeatureGenerator::drawPyramid (
	v3di_t baseNearPoint,
	int sideLength,
	char blockType,
	World &world)
{
	int halfSide = (sideLength >> 1) + 1;

	v3di_t baseFarPoint = {
		baseNearPoint.x + sideLength,
		baseNearPoint.y,
		baseNearPoint.z + sideLength};

	WorldMap &worldMap = *world.getWorldMap();

	for (int step = 0; step < halfSide; step++) {
		worldMap.fillVolume (baseNearPoint, baseFarPoint, blockType);

		baseNearPoint.x++;
		baseNearPoint.y++;
		baseNearPoint.z++;

		baseFarPoint.x--;
		baseFarPoint.y++;
		baseFarPoint.z--;
	}
}


void FeatureGenerator::createForViewer (v3d_t startPos, int type, World &world) {
	WorldMap &worldMap = *world.getWorldMap();
	v3di_t regionIndex = worldMap.getRegionIndex(startPos);
	v2di_t corner = { regionIndex.x, regionIndex.z };
	printf ("dungeon at (%d, %d)\n", corner.x, corner.y);
//	createDungeon (corner, 6, worldMap);
//	createPlain (corner, 6, 6, worldMap);



	worldMap.resize (6, 6);

	v3di_t nc = {
		(int)startPos.x - 64,
		(int)startPos.y - 5,
		(int)startPos.z - 64
	};

	v3di_t fc = {
		(int)startPos.x + 64,
		(int)startPos.y - 3,
		(int)startPos.z + 64
	};
	

//	loadWorldRegion(corner, 6, worldMap, true);


//	sd

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
//			worldMap.loadColumn(corner.x + i, corner.y + j);
			int xIndex = corner.x + i;
			int zIndex = corner.y + j;

			int columnIndex = worldMap.getColumnIndexByRegionCoords (xIndex, zIndex);

			if (xIndex == worldMap.mColumns[columnIndex].mWorldIndex.x &&
				zIndex == worldMap.mColumns[columnIndex].mWorldIndex.z) {
				printf ("WorldMap::loadColumn() - tried to load a column that is already loaded\n");
				return;
			}

			worldMap.clearColumn (columnIndex);


			int worldX = xIndex * WORLD_CHUNK_SIDE;
			int worldZ = zIndex * WORLD_CHUNK_SIDE;

			worldMap.mColumns[columnIndex].mWorldIndex.x = xIndex;
			worldMap.mColumns[columnIndex].mWorldIndex.z = zIndex;

			worldMap.mColumns[columnIndex].mWorldPosition.x = worldX;
			worldMap.mColumns[columnIndex].mWorldPosition.z = worldZ;

			worldMap.mColumns[columnIndex].mNeedDisplayList = false;
			worldMap.mColumns[columnIndex].mNeedShadowVolume = true;

			worldMap.mColumns[columnIndex].updateHighestAndLowest();

		}
	}

	worldMap.fillVolume (nc, fc, BLOCK_TYPE_DIRT);

}


void FeatureGenerator::clearDungeonBlock (int worldX, int worldY, int worldZ, WorldMap &worldMap) {
	v3di_t nearCorner = {
		worldX,
		worldY,
		worldZ
	};

	v3di_t farCorner = {
		worldX + 1,
		worldY + 2,
		worldZ + 1
	};

	worldMap.fillVolume (nearCorner, farCorner, BLOCK_TYPE_AIR);
}

void FeatureGenerator::draw2x2Block( v3di_t worldPos, char blockType, WorldMap& worldMap ) {
	worldMap.setBlockType( worldPos, blockType );
	worldPos.x++;
	worldMap.setBlockType( worldPos, blockType );
	worldPos.z++;
	worldMap.setBlockType( worldPos, blockType );
	worldPos.x--;
	worldMap.setBlockType( worldPos, blockType );
}


// cornerIndex = chunk index "bottom left"
// depth = dungeon levels
void FeatureGenerator::createDungeon (v2di_t cornerIndex, int depth, World &world) {
	WorldMap &worldMap = *world.getWorldMap();

	// dungeon side length in chunks
	int dungeonSide = 8;

	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int xSide = (dungeonSide * WORLD_CHUNK_SIDE);
	int zSide = (dungeonSide * WORLD_CHUNK_SIDE);

	height_info_t heightInfo = getHeightInfo (worldX, worldZ, xSide, xSide, world);

	loadWorldRegion(cornerIndex, dungeonSide, world, true);

	// this starts the dungeon creation process
	v2di_t stairDownRoomCorner = createDungeonRec (
		cornerIndex, v2di_v (0, 0), heightInfo.low, depth, worldMap);
	//v2di_t stairDownRoomCorner = createRogueRec (
	//	cornerIndex, v2di_v (0, 0), heightInfo.low, depth, worldMap);

	if (stairDownRoomCorner.x == 0) {
		// something went wrong
		printf ("FeatureGenerator::createDungeon() failed!\n");
		return;
	}

	v3di_t worldEntryPoint = {
		worldX + (stairDownRoomCorner.x * 2),
		heightInfo.low - 6,
		worldZ + (stairDownRoomCorner.y * 2),
	};

	// ok, lets build the tower/staircase outta here!
	height_info_t heightInfo2 = getHeightInfo (worldEntryPoint.x - 3, worldEntryPoint.z - 3, 8, 8, world);
	v3di_t a, b;

	// fill the entrance volume
	a = v3di_v (worldEntryPoint.x - 3, worldEntryPoint.y + 3, worldEntryPoint.z - 3);
//	b = v3di_v (worldEntryPoint.x + 4, worldEntryPoint.y + 40, worldEntryPoint.z + 4);
	b = v3di_v (worldEntryPoint.x + 4, heightInfo2.high + 25, worldEntryPoint.z + 4);
	worldMap.fillVolume (a, b, BLOCK_TYPE_OLD_BRICK);

	// clear it out now
	a = v3di_v (worldEntryPoint.x - 2, worldEntryPoint.y + 3, worldEntryPoint.z - 2);
//	b = v3di_v (worldEntryPoint.x + 3, worldEntryPoint.y + 40, worldEntryPoint.z + 3);
	b = v3di_v (worldEntryPoint.x + 3, heightInfo2.high + 25, worldEntryPoint.z + 3);
	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);

	// hmm...need some doors
	a = v3di_v (worldEntryPoint.x - 2, heightInfo2.high + 1, worldEntryPoint.z - 3);
	b = v3di_v (worldEntryPoint.x + 3, heightInfo2.high + 23, worldEntryPoint.z + 4);
	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);
	a = v3di_v (worldEntryPoint.x - 3, heightInfo2.high + 1, worldEntryPoint.z - 2);
	b = v3di_v (worldEntryPoint.x + 4, heightInfo2.high + 23, worldEntryPoint.z + 3);
	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);

	// and for the staircase
	v3di_t southwestInsideCorner = v3di_v (worldEntryPoint.x - 1, worldEntryPoint.y,  worldEntryPoint.z - 1);
	buildSpiralStaircase (southwestInsideCorner, 3, heightInfo2.high + 25,
		FG_CORNER_SW, worldMap);

}


// recursive rogue burrower
// create a level of the dungeon
// returns the stairsUp location
v2di_t FeatureGenerator::createRogueRec (
	v2di_t cornerIndex,
	v2di_t stairsUp,
	int height,
	int levelsRemaining,
	WorldMap &worldMap)
{
	int dungeonSide = 8;

	// since each tile of the RogueMap is 2m x 2m, halve each axis
	RogueMap rogueMap(dungeonSide * (WORLD_CHUNK_SIDE / 2), dungeonSide * (WORLD_CHUNK_SIDE / 2));

	// check to see if we are drawing the first level
	if (stairsUp.x == 0) {
		// decide where the staircase up from this level will be
		stairsUp = rogueMap.random_room (100, 6, 6);
	}
	else {
		// draw the room reserved for the stairway back up
		rogueMap.draw_room (stairsUp, v2di_add (stairsUp, v2di_v (6, 6)), 100);
	}

	// if there's another level beneath this one, make sure we have a room for the
	// downward stairs
	v2di_t stairDownRoomCorner;
	if (levelsRemaining > 0) {
		stairDownRoomCorner = rogueMap.random_room (500, 6, 6);
	}

	stairsUp.x += 2;
	stairsUp.y += 2;

	// number of extra rooms on this level (in addition to up and down rooms)
	int numAdditionalRooms = 3;
	rogueMap.randomize(false, numAdditionalRooms);
//	rogueMap.grow_paths ();

	rogueMap.drawIdRectInRoom(5, 7);

	v2di_t entryPoint = stairsUp;
	v3di_t worldEntryPoint;

	// WARNING: this always corners at 0, 0
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	int xSide = dungeonSide * WORLD_CHUNK_SIDE;
	int zSide = dungeonSide * WORLD_CHUNK_SIDE;


	for (int j = 0; j < dungeonSide; j++) {
		for (int i = 0; i < dungeonSide; i++) {
			for (int v = 0; v < (WORLD_CHUNK_SIDE / 2); v++) {
				for (int u = 0; u < (WORLD_CHUNK_SIDE / 2); u++) {
					int rx = u + (i * (WORLD_CHUNK_SIDE / 2));
					int rz = v + (j * (WORLD_CHUNK_SIDE / 2));

					int wx = worldX + (rx * 2);
					int wz = worldZ + (rz * 2);

					int type = rogueMap.getTile (rx, rz).type;
					int tileId = rogueMap.getTile (rx, rz).id;

					if (type != MAP_TILE_WALL) {
						// clear the block
						clearDungeonBlock (wx, height - 6, wz, worldMap);

						// just ignore the rest of this...move along!
						v3di_t pos = v3di_v (static_cast<int>(wx), height - 7, static_cast<int>(wz));

						char blockType;

						// lay down some flooring
						blockType = BLOCK_TYPE_BRICK_RED + tileId;
						if (tileId == 100) {
							blockType = BLOCK_TYPE_WHITE;
						}
						else if (tileId == -1) {
							blockType = BLOCK_TYPE_GREEN_STAR_TILE;
						}
						else if (tileId == 5) {
							blockType = BLOCK_TYPE_LIGHT_STONE;
						}
						else if (tileId == 7) {
							blockType = BLOCK_TYPE_LAVA;
						}

//						blockType = BLOCK_TYPE_GREEN_STAR_TILE;

						worldMap.setBlockType (pos, blockType);
						pos.x++;
						worldMap.setBlockType (pos, blockType);
						pos.z++;
						worldMap.setBlockType (pos, blockType);
						pos.x--;
						worldMap.setBlockType (pos, blockType);



						// now the ceiling
						blockType = BLOCK_TYPE_BRICK_RED + tileId; // BLOCK_TYPE_STONE;
						if (tileId == 100) {
							blockType = BLOCK_TYPE_ALIEN_SKIN;
						}
						else if (tileId == -1) {
							blockType = BLOCK_TYPE_GREEN_STAR_TILE;
						}

						pos = v3di_v (static_cast<int>(wx), height - 3, static_cast<int>(wz));
						worldMap.setBlockType (pos, blockType);
						pos.x++;
						worldMap.setBlockType (pos, blockType);
						pos.z++;
						worldMap.setBlockType (pos, blockType);
						pos.x--;
						worldMap.setBlockType (pos, blockType);
					}

					// this is a weird way of doing this...
					if (rx == entryPoint.x && rz == entryPoint.y) {
						// store this for later
						worldEntryPoint.x = wx;
						worldEntryPoint.y = height - 6;
						worldEntryPoint.z = wz;
					}
				}
			}
		}
	}


	v3di_t a, b;

	// clear it out now
	a = v3di_v (worldEntryPoint.x - 2, worldEntryPoint.y + 3, worldEntryPoint.z - 2);
//	b = v3di_v (worldEntryPoint.x + 3, worldEntryPoint.y + 40, worldEntryPoint.z + 3);
	b = v3di_v (worldEntryPoint.x + 3, height, worldEntryPoint.z + 3);
	worldMap.fillVolume (a, b, BLOCK_TYPE_AIR);


	// and for the staircase
	v3di_t southwestInsideCorner = v3di_v(worldEntryPoint.x - 1, worldEntryPoint.y,  worldEntryPoint.z - 1);
	buildSpiralStaircase(southwestInsideCorner, 3, height, FG_CORNER_SW, worldMap);

	// place some mosnters while we here
	// THIS MAY ACTUALLY BE USED AT SOME POINT!!
	//v2di_t monsterPos;
	//int numSpawners = r_numi (8, 15);
	//for (int i = 0; i < numSpawners; i++) {

	//	monsterPos = rogueMap.placeRandomMonster ();

	//	if (monsterPos.x >= 0) {
	//		inactive_item_t monsterItem;

	//		monsterItem.type = INACTIVETYPE_ITEM;
	//		monsterItem.type2 = OBJTYPE_MONSTER_SPAWNER;

	//		monsterItem.pos.x = static_cast<double>(worldX + (monsterPos.x * 2)) + 1.0;
	//		monsterItem.pos.y = static_cast<double>(heightInfo.low - 6) + 1.5;
	//		monsterItem.pos.z = static_cast<double>(worldZ + (monsterPos.y * 2)) + 1.0;

	//		mInactiveList.addItem (monsterItem);
	//	}
	//}


	// a little tail recursion anyone?
	if (levelsRemaining > 0 && stairDownRoomCorner.x != 0) {
		createDungeonRec (
			cornerIndex,
			// stair down on this level becomes stair up on next level
			stairDownRoomCorner,
			height - 7,
			levelsRemaining - 1,
			worldMap);
	}




	return stairsUp;
}








// recursive dungeon burrower
// create a level of the dungeon
// returns the stairsUp location
// 
v2di_t FeatureGenerator::createDungeonRec(
	v2di_t cornerIndex,
	v2di_t stairsUp,
	int height,
	int levelsRemaining,
	WorldMap &worldMap)
{
	int dungeonSideChunks = 8; // the length of the dungeon side in chunks
	int dungeonSide = dungeonSideChunks * (WORLD_CHUNK_SIDE / 2); // the DungeonModel side length

	DungeonUtil *dungeon = new DungeonUtil();
	dungeon->createDungeonModel( dungeonSide, dungeonSide );

	// check to see if we are drawing the first level
	if (stairsUp.x == 0) {
		// TEMP: put the world -> dungeon staircase dead center
		stairsUp = v2di_v( dungeonSide / 2, dungeonSide / 2 );
	}

	// if there's another level beneath this one, make sure we have a room for the
	// downward stairs
	v2di_t stairDownRoomCorner;
	if (levelsRemaining > 0) {
		//stairDownRoomCorner = rogueMap.random_room (500, 6, 6);

		// TEMP: this puts it in the same place as the last one ...
		stairDownRoomCorner = v2di_v( dungeonSide / 2, dungeonSide / 2 );
	}

	stairsUp.x += 2;
	stairsUp.y += 2;

	v2di_t entryPoint = stairsUp;
	
	MoleculeDungeon::createDungeon( *dungeon, stairsUp );

	// this is kinda ugly, with the whole worldEntryPoint being 2/3 set in the hollowDungeon2x thing...
	hollowOutDungeon2x( dungeon, worldMap, cornerIndex, dungeonSide, height ); 

	v3di_t worldEntryPoint = v3di_v(
		cornerIndex.x * WORLD_CHUNK_SIDE + ( entryPoint.x * 2 ),
		height - 6,
		cornerIndex.y * WORLD_CHUNK_SIDE + ( entryPoint.y * 2 ) );

	// clear out the shaft for the staircase
	v3di_t a = v3di_v( worldEntryPoint.x - 2, worldEntryPoint.y + 3, worldEntryPoint.z - 2 );
	v3di_t b = v3di_v( worldEntryPoint.x + 3, height, worldEntryPoint.z + 3 );
	worldMap.fillVolume( a, b, BLOCK_TYPE_AIR);

	// and for the staircase
	v3di_t southwestInsideCorner = v3di_v( worldEntryPoint.x - 1, worldEntryPoint.y,  worldEntryPoint.z - 1 );
	buildSpiralStaircase( southwestInsideCorner, 3, height, FG_CORNER_SW, worldMap );

	// place some mosnters while we here
	// THIS MAY ACTUALLY BE USED AT SOME POINT!!
	//v2di_t monsterPos;
	//int numSpawners = r_numi (8, 15);
	//for (int i = 0; i < numSpawners; i++) {

	//	monsterPos = rogueMap.placeRandomMonster ();

	//	if (monsterPos.x >= 0) {
	//		inactive_item_t monsterItem;

	//		monsterItem.type = INACTIVETYPE_ITEM;
	//		monsterItem.type2 = OBJTYPE_MONSTER_SPAWNER;

	//		monsterItem.pos.x = static_cast<double>(worldX + (monsterPos.x * 2)) + 1.0;
	//		monsterItem.pos.y = static_cast<double>(heightInfo.low - 6) + 1.5;
	//		monsterItem.pos.z = static_cast<double>(worldZ + (monsterPos.y * 2)) + 1.0;

	//		mInactiveList.addItem (monsterItem);
	//	}
	//}

	delete dungeon;

	// a little tail recursion anyone?
	if (levelsRemaining > 0 && stairDownRoomCorner.x != 0) {
		createDungeonRec (
			cornerIndex,
			// stair down on this level becomes stair up on next level
			stairDownRoomCorner,
			height - 7,
			levelsRemaining - 1,
			worldMap);
	}


	return stairsUp;
}


// hollows out a DungeonModel, doubling the tiles (ie 2x2 blocks for each dungeon tile)
void FeatureGenerator::hollowOutDungeon2x( DungeonUtil* dungeon, WorldMap& worldMap, const v2di_t& cornerIndex, int dungeonSide, int height ) {
	int worldX = cornerIndex.x * WORLD_CHUNK_SIDE;
	int worldZ = cornerIndex.y * WORLD_CHUNK_SIDE;

	v3di_t worldPos = v3di_v( 0, 0, 0 );

	// iterate through the dungeon
	for( int dz = 0; dz < dungeonSide; dz++ ) {
		worldPos.z = worldZ + (dz * 2);
		for( int dx = 0; dx < dungeonSide; dx++ ) {
			worldPos.x = worldX + (dx * 2);

			int type = dungeon->getTile( dx, dz )->type;

			if (type != DUNGEON_TILE_WALL) {
				// clear the block
				clearDungeonBlock( worldPos.x, height - 6, worldPos.z, worldMap );

				// lay down some flooring
				worldPos.y = height - 7;
				draw2x2Block( worldPos, BLOCK_TYPE_GREEN_STAR_TILE, worldMap );

				// now the ceiling
				worldPos.y = height - 3;
				draw2x2Block( worldPos, BLOCK_TYPE_GREEN_STAR_TILE, worldMap );
			}
		}
	}
}


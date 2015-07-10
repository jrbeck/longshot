// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * FeatureGenerator
// *
// * this is responsible for the procedurally generated world features
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "LoadScreen.h"
#include "World.h"
#include "WorldUtil.h"
#include "RogueMap.h"

#include "FeatureUtil.h"
#include "DungeonFeature.h"



// this is for the worldRogueMap
// i.e. numColumns x numColumns
#define ROGUE_W    64
#define ROGUE_H    64


class FeatureGenerator {
public:

  static void createSetPieces(int xIndex, int zIndex, World& world, LoadScreen* loadScreen);


  static void createPyramid(int side, v2di_t cornerIndex, World &world);
  static void create1by1Tower(v2di_t cornerIndex, int baseHeight, WorldMap &worldMap);

  static void drillSpiral(v3d_t top, double radius, double height,
    double totalRotation, int numSteps, WorldMap &worldMap);

  static void drillCavern(v2di_t cornerIndex, int sideX, int sideZ, World &world);

  static void growSpiralGarden(v2di_t cornerIndex, int sideX, int sideZ, World &world);

  static void createCastle8x8(v2di_t cornerIndex, World &world);

  static void createPlain(v2di_t cornerIndex, World &world);

  static void createVillage(int side, v2di_t cornerIndex, World &world);

  static void drawPyramid(v3di_t baseNearPoint, int sideLength, char blockType, World &world);

  static void createHouse(v2di_t cornerIndex, World &world);
  static void createBuilding2x2(v2di_t cornerIndex, World &world);

  static void createPlain(v2di_t cornerIndex, int sideX, int sideZ, World &world);

  static void createForViewer(v3d_t startPos, int type, World &world);

  // will this ever be used again?
  static v2di_t createRogueRec(v2di_t cornerIndex, v2di_t stairsUp, int height, int levelsRemaining, WorldMap &worldMap);
};

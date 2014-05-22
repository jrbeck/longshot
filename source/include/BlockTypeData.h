// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BlockTypeData
// *
// * this contains the properties for the blocks.
// * it is instatiated as a globally accessable lookup class.
// * i intend for this stuff to be in an XML file at some point.
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <Windows.h>
#include "GL/glut.h"

#include "v2d.h"


enum {
  BLOCK_TYPE_AIR,
  BLOCK_TYPE_DIRT,
  BLOCK_TYPE_DIRT_GRASS,
  BLOCK_TYPE_GRASS,
  BLOCK_TYPE_STONE_GRASS,
  BLOCK_TYPE_STONE,
  BLOCK_TYPE_WATER,
  BLOCK_TYPE_SAND,
  BLOCK_TYPE_SNOW,
  BLOCK_TYPE_DIRT_SNOW,
  BLOCK_TYPE_STONE_SNOW,
  BLOCK_TYPE_ALIEN_SKIN,
  BLOCK_TYPE_CACTUS,
  BLOCK_TYPE_LEAVES,
  BLOCK_TYPE_TRUNK,
  BLOCK_TYPE_MARS,
  BLOCK_TYPE_DARK_GREEN,
  BLOCK_TYPE_GRAY,
  BLOCK_TYPE_NAVY_BLUE,
  BLOCK_TYPE_BRICK_RED,
  BLOCK_TYPE_AQUA,
  BLOCK_TYPE_GLASS,
  BLOCK_TYPE_SLUDGE,
  BLOCK_TYPE_WHITE,
  BLOCK_TYPE_MUSTARD,
  BLOCK_TYPE_OBSIDIAN,
  BLOCK_TYPE_DARK_PURPLE,
  BLOCK_TYPE_TAN,
  BLOCK_TYPE_WHITE_MARBLE,
  BLOCK_TYPE_GREEN_STAR_TILE,
  BLOCK_TYPE_OLD_BRICK,
  BLOCK_TYPE_LIGHT_PURPLE,
  BLOCK_TYPE_LIGHT_BROWN,
  BLOCK_TYPE_COPPER,
  BLOCK_TYPE_MED_BLUE,
  BLOCK_TYPE_FUSCHIA,
  BLOCK_TYPE_LIGHT_BLUE,
  BLOCK_TYPE_YELLOW_TRUNK,
  BLOCK_TYPE_MED_STONE,
  BLOCK_TYPE_LIGHT_STONE,
  BLOCK_TYPE_LAVA,
  BLOCK_TYPE_BLOOD,
  BLOCK_TYPE_ALIEN_SKIN2,
  BLOCK_TYPE_ALIEN_SKIN3,
  BLOCK_TYPE_CONSOLE,
  BLOCK_TYPE_METAL_TILE_GREY,
  BLOCK_TYPE_METAL_TILE_GREEN,

  BLOCK_TYPE_PLANT_1,
  BLOCK_TYPE_PLANT_2,

  BLOCK_TYPE_FLOWER,

  NUM_BLOCK_TYPES,

  BLOCK_TYPE_INVALID,
  BLOCK_TYPE_PLACEHOLDER
};



extern int gWaterBlockType;


enum {
  BLOCK_SOLIDITY_TYPE_ROCK,    // holds together, pick best
  BLOCK_SOLIDITY_TYPE_DIRT,    // holds together, shovel best
  BLOCK_SOLIDITY_TYPE_LIQUID,  // doesn't hold together, bucket best
  BLOCK_SOLIDITY_TYPE_SAND,    // doesn't hold together, shovel/bucket best
  BLOCK_SOLIDITY_TYPE_VINES,   // holds together, machete best
  BLOCK_SOLIDITY_TYPE_WOOD,    // holds together, axe best
  BLOCK_SOLIDITY_TYPE_AIR,     // nothing there
  BLOCK_SOLIDITY_TYPE_PLANT,   // groundcover, etc...
  BLOCK_SOLIDITY_TYPE_GLASS,

  NUM_BLOCK_SOLIDITY_TYPES
};




// WARNING: LIGHT_ATTENUATION_MAX is supposed to be the same thing as
// LIGHT_LEVEL_MAX...that value may have changed!
enum {
  LIGHT_ATTENUATION_NONE,
  LIGHT_ATTENUATION_MAX = 63,
  LIGHT_ATTENUATION_OPAQUE
};




typedef struct {
  GLfloat alpha;
  BYTE lightAttenuation;
  BYTE lightEmission;
  GLfloat visionTint[4];

  double height;
  double healthEffect; // e.g. damaging/healing
  double health;
  BYTE flammability;

  double friction;
  double elasticity;
  double viscosity;

  double climbingDifficulty;

  BYTE solidityType; // solid, internal climber, non-solid, sand/gravel
  double blocksVisibility;

  // derived
  GLuint textureHandle;
  v2d_t textureCoords;
} BlockTypeData;




class BlockTypeDataContainer {
private:
  BlockTypeData mData[NUM_BLOCK_TYPES];

  void loadBlockTypeData();

public:
  BlockTypeDataContainer();

  BlockTypeData* get(int type);

};


// this is quite important!
// oh, and creepy.
extern BlockTypeDataContainer gBlockData;

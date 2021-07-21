// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Constants
// *
// * desc: game constants and enums
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned UINT;
#endif

#include "../vendor/GL/glut.h"

#include "../math/v3d.h"

#define WORLD_CHUNK_SIDE          (16)
#define WORLD_CHUNK_SIDE_SQUARED  (256)
#define WORLD_CHUNK_SIDE_CUBED    (4096)

enum {
  GAMESTATE_PLAY,
  GAMESTATE_MENU,
  GAMESTATE_MERCHANT
};

enum {
  GAMEMENU_BACKTOGAME,
  GAMEMENU_EXITGAME,

  GAMEMENU_SHIP,
  GAMEMENU_GALAXY_MAP,
  GAMEMENU_PLANET_MAP,

  GAMEMENU_DUNGEON_MAP,

  GAMEMENU_MERCHANT
};

enum {
  LIGHT_LEVEL_MIN = 0,
  LIGHT_LEVEL_MAX = 64,
  LIGHT_LEVEL_SOLID,
  LIGHT_LEVEL_INVALID,
  LIGHT_LEVEL_NOT_SET
};

#define ONE_OVER_LIGHT_LEVEL_MAX (1.0f/((float)LIGHT_LEVEL_MAX))

enum {
  BIT_MASK_1 = 0x01,
  BIT_MASK_2 = 0x02,
  BIT_MASK_3 = 0x04,
  BIT_MASK_4 = 0x08,
  BIT_MASK_5 = 0x10,
  BIT_MASK_6 = 0x20,
  BIT_MASK_7 = 0x40,
  BIT_MASK_8 = 0x80
};

enum {
  BLOCK_SIDE_LEF,
  BLOCK_SIDE_RIG,
  BLOCK_SIDE_TOP,
  BLOCK_SIDE_BOT,
  BLOCK_SIDE_FRO,
  BLOCK_SIDE_BAC,

  NUM_BLOCK_SIDES
};

// the various cube corners
enum {
  BOX_CORNER_LBB,    // left bottom back
  BOX_CORNER_LBF,    // left bottom front
  BOX_CORNER_LTB,    // left top back
  BOX_CORNER_LTF,    // left top front
  BOX_CORNER_RBB,    // right bottom back
  BOX_CORNER_RBF,    // right bottom front
  BOX_CORNER_RTB,    // right top back
  BOX_CORNER_RTF,    // right top front

  NUM_BOX_CORNERS
};

enum {
  OBJTYPE_PLAYER,
  OBJTYPE_AI_ENTITY,
  OBJTYPE_CREATE,
  OBJTYPE_DESTROY,
  OBJTYPE_SLIME,
  OBJTYPE_PLASMA,
  OBJTYPE_PLASMA_SPARK,
  OBJTYPE_PLASMA_TRAIL,
  OBJTYPE_PLASMA_BOMB,
  OBJTYPE_FIRE,
  OBJTYPE_NAPALM,
  OBJTYPE_SMOKE,
  OBJTYPE_GRENADE,
  OBJTYPE_ROCKET,
  OBJTYPE_SPARK,
  OBJTYPE_STEAM,
  OBJTYPE_BLOOD_SPRAY,
  OBJTYPE_LIVE_BULLET,
  OBJTYPE_DEAD_BULLET,
  OBJTYPE_EXPLOSION,
  OBJTYPE_TIGER_BAIT,
  OBJTYPE_ITEM,
  OBJTYPE_MELEE_ATTACK,
  OBJTYPE_MONSTER_SPAWNER,

  NUM_OBJTYPES,

  OBJTYPE_UNDEFINED
};

enum {
  AITYPE_DUMMY,
  AITYPE_BALLOON,
  AITYPE_SHOOTER,
  AITYPE_HOPPER,
  AITYPE_HUMAN,
  NUM_AITYPES,

  AITYPE_PLAYER,

};

enum {
  AMMO_BULLET,
  AMMO_SLIME,
  AMMO_PLASMA,
  AMMO_NAPALM,

  NUM_AMMO_TYPES
};

enum {
  ITEMTYPE_GUN_ONE_HANDED,
  ITEMTYPE_GUN_TWO_HANDED,
  ITEMTYPE_ROCKET_PACK,
  ITEMTYPE_AMMO_BULLET,
  ITEMTYPE_AMMO_SLIME,
  ITEMTYPE_AMMO_PLASMA,
  ITEMTYPE_AMMO_NAPALM,
  ITEMTYPE_HEALTHPACK,
  ITEMTYPE_MELEE_ONE_HANDED,
  ITEMTYPE_WORLD_BLOCK,

  NUM_ITEMTYPES,

  ITEMTYPE_UNDEFINED
};

enum {
  GUNTYPE_PISTOL,
  GUNTYPE_RIFLE,
  GUNTYPE_SHOTGUN,
  GUNTYPE_MACHINE_GUN,
  GUNTYPE_GRENADE_LAUNCHER,
  GUNTYPE_ROCKET_LAUNCHER,

  NUM_GUNTYPES
};

enum {
  MELEETYPE_MACE,
  MELEETYPE_AXE,

  NUM_MELEETYPES
};

enum {
  FACE_DIRECTION_XNEG,
  FACE_DIRECTION_XPOS,
  FACE_DIRECTION_ZNEG,
  FACE_DIRECTION_ZPOS
};

struct block_t {
  BYTE type;
  BYTE faceVisibility;
  BYTE faceLighting[6][3];
  BYTE uniqueLighting;
};

struct melee_weapon_state_t {
  int weaponHandle;

  v3d_t headPosition;
  v3d_t handPosition;

  double facing;
  double incline;

  int swingMode;
  double swingStart;
  double swingTime;

  bool hasUsed;
};

extern int gScreenW;
extern int gScreenH;

// this allows for a global default font to be used
// WARNING: it needs to be set externally. in our case,
// it is set in the Longshot class because that is the
// highest class in the program hierarchy that needs it
extern GLuint gDefaultFontTextureHandle;

// unit cube (0, 0, 0) to (1, 1, 1)
extern GLfloat cube_corner[NUM_BOX_CORNERS][3];
// the unit cube, centered at origin
extern GLfloat cube_corner_centered[NUM_BOX_CORNERS][3];
extern GLint gCubeFaceNormalLookup[NUM_BLOCK_SIDES][3];
extern v3di_t gBlockNeighborAddLookup[NUM_BLOCK_SIDES];
extern BYTE gBlockSideBitmaskLookup[NUM_BLOCK_SIDES];

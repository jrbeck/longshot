// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * RogueMap
// *
// * generates convoluted 'rogue' style 2d dungeon layouts
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>
#include <algorithm>

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include "../vendor/GL/glut.h"

#include "../math/MathUtil.h"
#include "../math/v2d.h"
#include "../math/v3d.h"

#define MAP_TILE_INVALID    -1
#define MAP_TILE_WALL        0
#define MAP_TILE_FLOOR       1
#define MAP_TILE_PATH_SEED   2
#define MAP_TILE_DOORWAY     3
#define MAP_TILE_PATH        4

#define ID_NONE    -1
#define ID_START    0

struct rgb_t {
  unsigned char r, g, b;
};

struct map_tile_t {
  int type;
  rgb_t color;
  int id;
  bool monsterPresent;
};

struct room_t {
  int handle;
  int id;
  v2di_t nearCorner;
  v2di_t farCorner;
  v2di_t doorLocation;
};

class RogueMap {
public:
  RogueMap();
  RogueMap(int width, int height);
  ~RogueMap();

  void resize(int width, int height);

  int getWidth() const;
  int getHeight() const;
  int getLastRoomHandle() const;

  void randomize(bool clearMap, int numRooms);
  void clear();

  int getRoomIndexByHandle(int handle);
  int getRoomIndexById(int id, int occurrence);
  v2di_t random_room(int id, int width, int height);

  int placeRandomDoor(int roomId);

  void random_path();
  void grow_paths();
  int draw_path(v2di_t a, v2di_t b);

  v2di_t placeRandomMonster();
  bool isValidMonsterLocation(v2di_t pos);

  bool isOnMap(int i, int j);

  int path_valid_pos(int x, int y);
  int onEdge(v2di_t a);
  void draw_room(v2di_t c, v2di_t d, int id);

  void drawIdRectInRoom(int roomId, int rectId);

  map_tile_t getTile(int i, int j);
  void setTile(int i, int j, map_tile_t tile);

  v2di_t getRandomRoomIndex();
  v2di_t getEntryPoint();

  void draw();

private:
  int mWidth;
  int mHeight;

  map_tile_t* mMap;

  int mLastRoomHandle;
  std::vector <room_t> mRooms;
};

#include "RogueMap.h"

RogueMap::RogueMap() {
  mMap = NULL;
  mWidth = 0;
  mHeight = 0;
  mLastRoomHandle = 0;
}

RogueMap::RogueMap(int width, int height) {
  mMap = NULL;
  resize (width, height);
  mLastRoomHandle = 0;
}

RogueMap::~RogueMap() {
  if (mMap != NULL) {
    delete [] mMap;
  }
}

void RogueMap::resize(int width, int height) {
  if (mMap != NULL) {
    delete [] mMap;
  }

  mWidth = width;
  mHeight = height;

  mMap = new map_tile_t[width * height];

  clear ();
}

int RogueMap::getWidth() const {
  return mWidth;
}

int RogueMap::getHeight() const {
  return mHeight;
}

int RogueMap::getLastRoomHandle() const {
  return mLastRoomHandle;
}

void RogueMap::randomize(bool clearMap, int numRooms) {
  if (clearMap) {
    clear();
  }

  for (int i = 0; i < numRooms; i++) {
//    random_room (i + ID_START, r_numi (4, 30), r_numi (4, 30));
    random_room(5, r_numi (4, 30), r_numi (4, 30));
  }

  if (numRooms >= 2) {
    grow_paths();
  }

  map_tile_t tile;
  tile.type = MAP_TILE_PATH;
  tile.id = ID_NONE;

  // change certain things to MAP_TILE_PATH type
  for (int j = 0; j < mHeight; j++) {
    for (int i = 0; i < mWidth; i++) {
      if (getTile (i, j).color.r == 64  ||  // MAP_TILE_PATH
        getTile (i, j).color.r == 192 ||  // MAP_TILE_SEED
        getTile (i, j).color.g == 192)    // MAP_TILE_SEED
      {
        setTile (i, j, tile);
      }
    }
  }
}

void RogueMap::clear() {
  map_tile_t blank;

  blank.type = MAP_TILE_WALL;
  blank.color.r = 0;
  blank.color.g = 0;
  blank.color.b = 0;
  blank.id = ID_NONE;
  blank.monsterPresent = false;

  for (int j = 0; j < mHeight; j++) {
    for (int i = 0; i < mWidth; i++) {
      setTile (i, j, blank);
    }
  }

  mRooms.clear ();
  mLastRoomHandle = 0;
}

int RogueMap::getRoomIndexByHandle(int handle) {
  if (handle <= 0) {
    return -1;
  }

  for (size_t i = 0; i < mRooms.size (); i++) {
    if (mRooms[i].handle == handle) {
      return i;
    }
  }

  return -1;
}

// WARNING: occurrence index is zero-based
int RogueMap::getRoomIndexById(int id, int occurrence) {
  for (size_t i = 0; i < mRooms.size (); i++) {
    if (mRooms[i].id == id) {
      // what? don't trust the compiler? ;)
      if (occurrence-- <= 0) {
        return i;
      }
    }
  }

  return -1;
}

v2di_t RogueMap::random_room(int id, int width, int height) {
  v2di_t a, b;

  int done;

  int numTries = 0;
  do {
    done = 1;

    // pick a random top left corner where the room will still fit on the map
    a.x = r_numi (0, mHeight - (width + 2));
    a.y = r_numi (0, mWidth - (height + 2));

    // check if the room overlaps anything it shouldn't
    for (int j = 0; j < height + 3; j++) {
      for (int i = 0; i < width + 3; i++) {
        if (getTile (a.x + i, a.y + j).type != MAP_TILE_WALL) {
          done = 0;
        }
      }
    }

    if (numTries++ > 1000) {
      // failed!
      return v2di_v (0, 0);
    }
  } while (!done);


  // make sure we have a wall surrounding the room
  a.x++;
  a.y++;

  // get the bottom right corner of the room
  b.x = a.x + width - 1;
  b.y = a.y + height - 1;

  // draw the room on the map
  draw_room(a, b, id);

  placeRandomDoor(id);

  return a;
}

int RogueMap::placeRandomDoor(int roomId) {
  // not gonna happen if there are no rooms
  if (mRooms.size () == 0) {
    return -1;
  }

  // find the room in question
  int roomIndex = -1;
  for (size_t i = 0; i < mRooms.size () && roomIndex < 0; i++) {
    if (mRooms[i].id == roomId) {
      roomIndex = i;
    }
  }

  // leave if we couldn't find the room
  if (roomIndex < 0) {
    return -1;
  }

  // now place a path seed
  v2di_t c; // where we'll put the guaranteed door
  int tries = 100;

  do {
    if (tries-- <= 0) {
      // failed on the door thing...
      return -1;
    }

    c.x = r_numi (mRooms[roomIndex].nearCorner.x - 1, mRooms[roomIndex].farCorner.x + 2);
    c.y = r_numi (mRooms[roomIndex].nearCorner.y - 1, mRooms[roomIndex].farCorner.y + 2);

  } while (onEdge (c) != roomId);

  map_tile_t tile;
  tile.type = MAP_TILE_PATH_SEED;
  tile.id = roomId;

  setTile (c.x, c.y, tile);

  return 0;
}

void RogueMap::random_path() {
/*  v2di_t a, b;

  // keep trying till we draw a path
  do {
    a.x = r_numi (0, mWidth);
    a.y = r_numi (0, mHeight);

    b.x = r_numi (0, mWidth);
    b.y = r_numi (0, mHeight);
  } while (!isOnEdge (a) || // a is not on edge
       !isOnEdge (b) || // b is not on edge
//       ((a.x == b.x) && (a.y == b.y)) ||
       !draw_path (a, b)); // can't make a path
*/
}

void RogueMap::grow_paths() {
  v2di_t a, b;

  map_tile_t tile;

  for (a.y = 0; a.y < mHeight; a.y++) {
    for (a.x = 0; a.x < mWidth; a.x++) {
      if (getTile (a.x, a.y).type == MAP_TILE_PATH_SEED) {
        int try_again = 1000;
        int success = 0;

        do {
          int attempts = 0;
          do {
            b.x = r_numi (0, mWidth);
            b.y = r_numi (0, mHeight);
          } while ((onEdge (b) == ID_NONE ||
            onEdge (b) == getTile (a.x, a.y).id) &&
            ++attempts < 1000);

          if (attempts >= 1000) {
            // fail
            try_again--;
            success = 1;
            break;
          }

          if (draw_path (a, b) > 1) {
            tile.type = MAP_TILE_DOORWAY;
            tile.id = ID_NONE;

            setTile (a.x, a.y, tile);
            setTile (b.x, b.y, tile);

            success = 1;
            try_again = 0;
            continue;
          }
          try_again--;
        } while (try_again);
        // hide the failed attempts
        if (!success) {
          tile.type = MAP_TILE_WALL;
          tile.id = ID_NONE;

          setTile (a.x, a.y, tile);
        }
      }
    }
  }
}

int RogueMap::draw_path(v2di_t a, v2di_t b) {
  int *open = new int[mWidth * mHeight];

  for (int j = 0; j < mHeight; j++) {
    for (int i = 0; i < mWidth; i++) {
      if (getTile (i, j).type == MAP_TILE_WALL || getTile (i, j).type == MAP_TILE_PATH) {
        open[i + (j * mWidth)] = -1;
      }
      else {
        open[i + (j * mWidth)] = -2;
      }
    }
  }

  // seed the lookup map
  open[a.x + (a.y * mWidth)] = 0;

  int count;
  int move = 0;
  int goal_reached = 0;

  // build the lookup map
  do {
    count = 0;

    for (int j = 0; j < mHeight; j++) {
      for (int i = 0; i < mWidth; i++) {

        // we start on the last iteration's placements
        if (open[i + (j * mWidth)] == move) {

          // if the destination is one move away, we're done
          if ((i == b.x && (j + 1) == b.y) ||
            (i == b.x && (j - 1) == b.y) ||
            ((i + 1) == b.x && j == b.y) ||
            ((i - 1) == b.x && j == b.y)) {
            i = mWidth;
            j = mHeight;
            goal_reached = 1;
            continue;
          }

          // place the move counter if we find a valid spot
          if (path_valid_pos (i, j + 1)) {
            if (open[i + ((j + 1) * mWidth)] == -1) {
              open[i + ((j + 1) * mWidth)] = move + 1;
              count++;
            }
          }
          if (path_valid_pos (i, j - 1)) {
            if (open[i + ((j - 1) * mWidth)] == -1) {
              open[i + ((j - 1) * mWidth)] = move + 1;
              count++;
            }
          }
          if (path_valid_pos (i + 1, j)) {
            if (open[i + 1 + (j * mWidth)] == -1) {
              open[i + 1 + (j * mWidth)] = move + 1;
              count++;
            }
          }
          if (path_valid_pos (i - 1, j)) {
            if (open[i - 1 + (j * mWidth)] == -1) {
              open[i - 1 + (j * mWidth)] = move + 1;
              count++;
            }
          }
        }
      }
    }

    move++;

  } while (count != 0 && !goal_reached);

  // fail
  if (!goal_reached) {
    delete [] open;
    return 0;
  }

  count = 0;

  v2di_t pos = b;
  v2di_t c = a;

  int minimum;

/*  for (int j = 0; j < mHeight; j++) {
    for (int i = 0; i < mWidth; i++) {
      if (open[j][i] >= 0) {
        map[j][i].color.r = open[j][i] * 3;
        map[j][i].type = MAP_TILE_FLOOR;
      }
    }
  }*/

  // compute the path
  do {
    minimum = 10000;

    open[pos.x + (pos.y * mWidth)] = 101010;

    if (isOnMap (pos.x, pos.y + 1) && open[pos.x + ((pos.y + 1) * mWidth)] > -1) {
      if (open[pos.x + ((pos.y + 1) * mWidth)] < minimum) {
        c.x = pos.x;
        c.y = pos.y + 1;
        minimum = open[c.x + (c.y * mWidth)] + 1;

        // favor pre-existing paths
        if (getTile (c.x, c.y).color.r == 64) minimum -= 2;
      }
    }
    if (isOnMap (pos.x, pos.y - 1) && open[pos.x + ((pos.y - 1) * mWidth)] > -1) {
      if (open[pos.x + ((pos.y - 1) * mWidth)] < minimum) {
        c.x = pos.x;
        c.y = pos.y - 1;
        minimum = open[c.x + (c.y * mWidth)] + 1;

        // favor pre-existing paths
        if (getTile (c.x, c.y).color.r == 64) minimum -= 2;
      }
    }
    if (isOnMap (pos.x + 1, pos.y) && open[pos.x + 1 + (pos.y * mWidth)] > -1) {
      if (open[pos.x + 1 + (pos.y * mWidth)] < minimum) {
        c.x = pos.x + 1;
        c.y = pos.y;
        minimum = open[c.x + (c.y * mWidth)] + 1;

        // favor pre-existing paths
        if (getTile (c.x, c.y).color.r == 64) minimum -= 2;
      }
    }
    if (isOnMap (pos.x - 1, pos.y) && open[pos.x - 1 + (pos.y * mWidth)] > -1) {
      if (open[pos.x - 1 + (pos.y * mWidth)] < minimum) {
        c.x = pos.x - 1;
        c.y = pos.y;
        minimum = open[c.x + (c.y * mWidth)] + 1;

        // favor pre-existing paths
        if (getTile (c.x, c.y).color.r == 64) minimum -= 2;
      }
    }
    if (minimum != 10000) count++;

    pos = c;

    if (pos.x == a.x && pos.y == a.y) minimum = 10000;

  } while (minimum != 10000);


/*  if (count) {
    map[a.y][a.x].color.r = 255;
    map[a.y][a.x].color.g = 0;
    map[a.y][a.x].color.b = 0;

    map[b.y][b.x].color.r = 0;
    map[b.y][b.x].color.g = 255;
    map[b.y][b.x].color.b = 0;
  }*/


  if (count) {
    map_tile_t tile;
    tile.type = MAP_TILE_PATH;
    tile.color.r = 64;
    tile.color.g = 64;
    tile.color.b = 64;
    tile.id = ID_NONE;

    for (int j = 0; j < mHeight; j++) {
      for (int i = 0; i < mWidth; i++) {
        if (open[i + (j * mWidth)] == 101010) {
          setTile (i, j, tile);
        }
      }
    }

    tile.type = MAP_TILE_DOORWAY;

    tile.color.r = 0;
    tile.color.g = 192;
    tile.color.b = 0;
    setTile (a.x, a.y, tile);

    tile.color.r = 192;
    tile.color.g = 0;
    tile.color.b = 0;
    setTile (b.x, b.y, tile);
  }


  delete [] open;

  return count;
} // END of draw_path()

int RogueMap::path_valid_pos(int x, int y) {
  for (int j = -1; j < 2; j++) {
    for (int i = -1; i < 2; i++) {
      if (!isOnMap (x + i, y + j)) return 0;
      if (//map[y + j][x + i].type == MAP_TILE_PATH ||
        getTile (x + i, y + j).type == MAP_TILE_FLOOR) return 0;
    }
  }

  return 1;
}

v2di_t RogueMap::placeRandomMonster() {
  bool monsterPlaced = false;
  int numTries = 0;
  v2di_t position;

  while (!monsterPlaced && numTries++ < 10000) {
    position.x = r_numi (1, mHeight - 1);
    position.y = r_numi (1, mWidth - 1);

    if (isValidMonsterLocation (position)) {
      // place the monster
      map_tile_t tile = getTile (position.x, position.y);
      tile.monsterPresent = true;

      setTile (position.x, position.y, tile);
      monsterPlaced = true;
    }
  }

  if (!monsterPlaced) {
    position = v2di_v (-1, -1);
  }

  return position;
}

bool RogueMap::isValidMonsterLocation(v2di_t pos) {
  map_tile_t tile = getTile (pos.x, pos.y);

  if (tile.monsterPresent) return false;
  if (tile.type != MAP_TILE_FLOOR && tile.type != MAP_TILE_PATH) return false;

  return true;
}

// if a is an edge, returns the id of the room it's attached to
int RogueMap::onEdge(v2di_t a) {
  // must be on map
  if (!isOnMap (a.x, a.y)) return ID_NONE;

  // must be a wall
  if (getTile (a.x, a.y).type != MAP_TILE_WALL) return ID_NONE;

  // make sure it is not a map edge
  if (a.x < 2 || a.x >= (mWidth - 2) ||
    a.y < 2 || a.y >= (mHeight - 2)) return ID_NONE;

  // we need to check the surrounding 3 length rows and columns
  int t, l, b, r;
  t = l = b = r = 0;

  if (getTile (a.x - 1, a.y - 1).type == MAP_TILE_WALL &&
    getTile (a.x, a.y - 1).type == MAP_TILE_WALL &&
    getTile (a.x + 1, a.y - 1).type == MAP_TILE_WALL) t = 1;

  if (getTile (a.x - 1, a.y + 1).type == MAP_TILE_WALL &&
    getTile (a.x, a.y + 1).type == MAP_TILE_WALL &&
    getTile (a.x + 1, a.y + 1).type == MAP_TILE_WALL) b = 1;

  if (getTile (a.x - 1, a.y - 1).type == MAP_TILE_WALL &&
    getTile (a.x - 1, a.y).type == MAP_TILE_WALL &&
    getTile (a.x - 1, a.y + 1).type == MAP_TILE_WALL) l = 1;

  if (getTile (a.x + 1, a.y - 1).type == MAP_TILE_WALL &&
    getTile (a.x + 1, a.y).type == MAP_TILE_WALL &&
    getTile (a.x + 1, a.y + 1).type == MAP_TILE_WALL) r = 1;

  if ((t + l + b + r) != 1) return ID_NONE;

  // looks like we have an edge, return the proper id
  return (t * getTile (a.x, a.y + 1).id) +
       (b * getTile (a.x, a.y - 1).id) +
       (l * getTile (a.x + 1, a.y).id) +
       (r * getTile (a.x - 1, a.y).id);
}

bool RogueMap::isOnMap(int i, int j) {
  if (i < 0 || j < 0 || i >= mWidth || j >= mHeight) {
    return false;
  }

  return true;
}

void RogueMap::draw_room(v2di_t c, v2di_t d, int id) {
  int l = MACRO_MIN(c.x, d.x);
  int r = MACRO_MAX(c.x, d.x);
  int t = MACRO_MIN(c.y, d.y);
  int b = MACRO_MAX(c.y, d.y);

  map_tile_t tile;
  tile.type = MAP_TILE_FLOOR;
  tile.color.r = 72;
  tile.color.g = 72;
  tile.color.b = 72;
  tile.id = id;

  for (int j = t; j <= b; j++) {
    for (int i = l; i <= r; i++) {
      setTile (i, j, tile);
    }
  }

  room_t room;
  room.handle = ++mLastRoomHandle;
  room.id = id;

  room.nearCorner.x = l;
  room.nearCorner.y = t;

  room.farCorner.x = r;
  room.farCorner.y = b;

  mRooms.push_back (room);
}

void RogueMap::drawIdRectInRoom(int roomId, int rectId) {
  int index;

  for (size_t i = 0; i < mRooms.size (); i++) {
    index = getRoomIndexById (roomId, i);

    if (index == -1) break;

    for (int j = mRooms[index].nearCorner.y + 1; j < mRooms[index].farCorner.y; j++) {
      for (int i = mRooms[index].nearCorner.x + 1; i < mRooms[index].farCorner.x; i++) {
        map_tile_t tile = getTile (i, j);
        tile.id = rectId;
        setTile (i, j, tile);
      }
    }
  }
}

map_tile_t RogueMap::getTile(int i, int j) {
  if (mMap == NULL || !isOnMap (i, j)) {
    map_tile_t dummy;

    dummy.type = MAP_TILE_INVALID;

    return dummy;
  }

  return mMap[i + (j * mWidth)];
}

void RogueMap::setTile(int i, int j, map_tile_t tile) {
  if (mMap == NULL ||  !isOnMap (i, j)) {
    return;
  }

  mMap[i + (j * mWidth)] = tile;
}

v2di_t RogueMap::getRandomRoomIndex() {
  v2di_t index = v2di_v (-1, -1);

  if (mMap == NULL) {
    return index;
  }

  for (int i = 0; i < 10000; i++) {
    int x = r_numi (8, mWidth - 8);
    int z = r_numi (8, mHeight - 8);

    if (getTile (x, z).type == MAP_TILE_FLOOR) {
      index.x = x;
      index.y = z;

      return index;
    }
  }

  return index;
}

v2di_t RogueMap::getEntryPoint() {
  v2di_t index = v2di_v (-1, -1);

  if (mMap == NULL) {
    return index;
  }

  for (int i = 0; i < 50000; i++) {
    int x = r_numi (8, mWidth - 8);
    int z = r_numi (8, mHeight - 8);

    if (getTile (x, z).type == MAP_TILE_FLOOR &&
      getTile (x - 1, z - 1).type == MAP_TILE_FLOOR &&
      getTile (x,     z - 1).type == MAP_TILE_FLOOR &&
      getTile (x + 1, z - 1).type == MAP_TILE_FLOOR &&
      getTile (x - 1, z).type == MAP_TILE_FLOOR &&
      getTile (x + 1, z).type == MAP_TILE_FLOOR &&
      getTile (x - 1, z + 1).type == MAP_TILE_FLOOR &&
      getTile (x,     z + 1).type == MAP_TILE_FLOOR &&
      getTile (x + 1, z + 1).type == MAP_TILE_FLOOR) {
      index.x = x;
      index.y = z;

//      printf ("that took %d tries!\n", i);

      return index;
    }
  }

  return index;
}

void RogueMap::draw() {
  GLfloat color1[4] = {1.0, 0.0, 0.0, 1.0};
  GLfloat color2[4] = {0.0, 1.0, 0.0, 1.0};
  GLfloat color3[4] = {0.0, 0.0, 1.0, 1.0};
  GLfloat color4[4] = {1.0, 0.0, 1.0, 1.0};
  GLfloat color5[4] = {0.0, 1.0, 1.0, 1.0};
  GLfloat color6[4] = {1.0, 1.0, 1.0, 1.0};
  GLfloat color7[4] = {0.5, 0.5, 0.0, 1.0};

  map_tile_t tile;

  for (int j = 0; j < mHeight; j++) {
    for (int i = 0; i < mWidth; i++) {

      tile = getTile (i, j);

      glBegin (GL_QUADS);
        // top
        glNormal3d (0.0, 1.0, 0.0);

        if (tile.id == 500) {
          glColor4fv (color3);
        }
        else if (tile.id == 501) {
          glColor4fv (color4);
        }
        else if (tile.id == 502) {
          glColor4fv (color5);
        }
        else if (tile.id == 503) {
          glColor4fv (color6);
        }
        else if (tile.id == 504) {
          glColor4fv (color7);
        }
        else if (tile.type == MAP_TILE_WALL) {
          glColor4fv (color1);
        }
        else {
          glColor4fv (color2);
        }

        GLfloat nearCorner[3];
        nearCorner[0] = (GLfloat)i;
        nearCorner[1] = 0.0f;
        nearCorner[2] = (GLfloat)j;

        GLfloat farCorner[3];
        farCorner[0] = (GLfloat)i + 1.0f;
        farCorner[1] = 0.0f;
        farCorner[2] = (GLfloat)j + 1.0f;


//        glTexCoord2f (0.0f, 0.0f);
        glVertex3f (nearCorner[0], 0.0f, nearCorner[2]);  // LTB
//        glTexCoord2f (0.0f, 0.5f);
        glVertex3f (nearCorner[0], 0.0f, farCorner[2]);  // LTF
//        glTexCoord2f (0.5f, 0.5f);
        glVertex3f (farCorner[0], 0.0f, farCorner[2]);  // RTF
//        glTexCoord2f (0.5f, 0.0f);
        glVertex3f (farCorner[0], 0.0f, nearCorner[2]);  // RTB
      glEnd ();


    }
  }
}

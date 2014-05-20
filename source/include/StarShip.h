// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * StarShip
// *
// * handles the player aboard a starship
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "Location.h"
#include "WorldMap.h"
#include "OrbitSky.h"


class StarShip : public Location {
public:
  StarShip();
  virtual ~StarShip();

  // these are all from Location
  int initialize(FILE *file, Galaxy *galaxy, int planetHandle);
  void save(FILE *file);
  void load(FILE *file);
  int update(v3d_t playerPosition);
  void draw(gl_camera_c &cam);
  v3d_t getStartPosition(void);

  void initWorldColumns(bool clearColumns);
  void generateShip();

  static const int WORLD_MAP_SIDE = 16;

  OrbitSky *mOrbitSky;
};


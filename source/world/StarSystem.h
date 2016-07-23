// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * StarSystem
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <algorithm>
#include <vector>
#include <sdl2/SDL_opengl.h>

#include "../math/MathUtil.h"
#include "../world/Planet.h"

#define MIN_PLANETS    (1)
#define MAX_PLANETS    (10)

#define MIN_STAR_RADIUS    (3.0)
#define MAX_STAR_RADIUS    (6.0)

#define MIN_PLANET_RADIUS    (1.0)
#define MAX_PLANET_RADIUS    (3.0)

#define MIN_ORBIT_RADIUS    (10.0)
#define MAX_ORBIT_RADIUS    (50.0)

class StarSystem {
public:
  StarSystem();
  ~StarSystem();

  void clear();
  void randomize();

  void save(FILE* file);
  void load(FILE* file);

  Planet* getPlanetByHandle(int handle);

// MEMBERS * * * * * * * * * * * *
  int mHandle;
  v2d_t mPosition;

  GLfloat mStarColor[4];
  GLfloat mStarRadius;

  vector<Planet*> mPlanets;
};

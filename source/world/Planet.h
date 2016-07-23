// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Planet
// *
// * describes a planet
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cmath>
#include <sdl2/SDL_opengl.h>

#include "../math/v2d.h"


using namespace std;


class Planet {
public:
  Planet();
  ~Planet();

  void save(FILE *file);
  void load(FILE *file);

// MEMBERS * * * * * * * * * * *
  // WARNING: this is being saved/loaded with an fwrite()
  // i.e., data must be simple, no vectors down here
  int mHandle;
  bool mHasBeenVisited;
  int mSeed;

  GLfloat mRadius;
  GLfloat mAngle;
  GLfloat mOrbitRadius;
  v2d_t mPosition;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * OrbitSky
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include <sdl2/SDL_opengl.h>

#include "Galaxy.h"
#include "AssetManager.h"
#include "GlCamera.h"


typedef struct {
  v3d_t pos;
  GLfloat color[4];
  GLfloat size;
} celestial_body;



class OrbitSky {
public:
  OrbitSky();
  ~OrbitSky();

  void setOrbit(Galaxy &galaxy, size_t planetHandle);

  void draw(GlCamera &cam, v3d_t playerPosition);

  vector<celestial_body> mBodies;
};

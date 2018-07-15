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

#include <SDL2/SDL_opengl.h>

#include "../world/Galaxy.h"
#include "../assets/AssetManager.h"
#include "../math/GlCamera.h"


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

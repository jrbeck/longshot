// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * CloudSim
// *
// * this generates and draws the pretty clouds
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/MathUtil.h"
#include "../math/GlCamera.h"
#include "../assets/AssetManager.h"
#include "../world/Terrain.h"

#include "../math/v3d.h"




class CloudSim {
public:
  CloudSim (void);
  ~CloudSim (void);

  void clear(void);
  void newClouds (v3d_t position);

  void redrawDisplayLists (v3d_t position);

  void update (GLfloat starAlpha);

  void drawForDisplayList (v3d_t position);
  void draw (GlCamera &cam);

  void drawCloudBlock (v3di_t position, GLfloat alpha);

private:
  static const int SIDE_WIDTH = 64;
  static const int BLOCK_SIZE = 32;

  Terrain mTerrain;
  PseudoRandom mPrng;

  GLuint mCloudDisplayListHandle;

  GLfloat mLightIntensities[4];

  v3d_t mPosition;
};

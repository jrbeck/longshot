// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * SkySim
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SkySim_h_
#define SkySim_h_


#include "v3d.h"
#include "CloudSim.h"


class SkySim {
public:
  SkySim();
  ~SkySim();

  void initialize(v3d_t playerStartPosition);

  void setSkyColorAndWorldLighting(void);

  void update();
  void draw(gl_camera_c &cam, v3d_t playerPosition);

  int getSkyLightLevel(void);


// MEMBERS * * * * * * * * * *
  v3d_t mSunPosition;
  GLfloat mSunColor[3];
  GLfloat mSkyColor[3];

  double mInitialTimeOfDay;
  double mTimeFactor;

  v3d_t mCurrentSunPosition;

  GLfloat mStarAlpha;
  static const int NUM_STARS = 150;
  v3d_t mStars[NUM_STARS];

  CloudSim *mCloudSim;

};


#endif // SkySim_h_

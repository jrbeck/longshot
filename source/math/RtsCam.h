// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * rts_cam_c
// *
// * this handles the RTS style camera stuff
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cmath>

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include "../math/MathUtil.h"
#include "../math/v3d.h"
#include "../math/GlCamera.h"

class rts_cam_c {
  v3d_t mPosition;  // where the camera is
  v3d_t mTarget;    // where the camera is pointed
  v3d_t mUpVector;  // the 'up' vector for the camera

  GlCamera mGlCamera;  // this is the general camera

  double mFacing;    // the angle on the x-z plane off the positive x-axis

  double mElevation;    // the angle on the vertical plane off the x-z plane intersection...
  double mElevationMin;  // upper constraint for elevation angle
  double mElevationMax;  // lower constraint for elevation angle

  double mDistanceFromTarget;  // how far out from the target the camera is
  double mNearLimit;  // how close to the target we can get
  double mFarLimit;  // max distance from the target we can be

public:
  rts_cam_c();
  ~rts_cam_c();

  void initializeCamera(v2di_t screenDimensions, double FOV, double nearPlane, double farPlane);

  // set with target, facing, elevation, and distance
  int setup_cam(v3d_t t, double f, double e, double d);

  // update the camera position according to target, facing, elevation and distance
  int updatePosition();

  int constrain();

  int pan(v2d_t a);

  int zoom(double a);

  int translate(v2d_t a);

  void changeElevation(double amount);

  void setPosition(v3d_t position);
  v3d_t getPosition() const;

  void setTarget(v3d_t target);
  void setTargetAndUpdate(v3d_t target);
  v3d_t getTarget() const;

  // set the near and far values for the camera
  int set_near_and_far(double n, double f);

  // use the gluLookAt () to set view at render time
  GlCamera* gl_cam_setup();
};

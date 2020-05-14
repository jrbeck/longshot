// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GlCamera
// *
// * responsible for keeping track of screen stuff and fovs and frustums...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#pragma once

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include <GL/glu.h>
#include "../vendor/GL/glut.h"

#include <math.h>

#include "../math/v3d.h"
#include "../math/MathUtil.h"
#include "../math/Plane3d.h"
#include "../math/BoundingBox.h"
#include "../math/BoundingSphere.h"

#define PLANE_TOP  0
#define PLANE_BOT  1
#define PLANE_LEF  2
#define PLANE_RIG  3
#define PLANE_NEA  4
#define PLANE_FAR  5

#define FRUSTUM_INSIDE      0
#define FRUSTUM_INTERSECT    1
#define FRUSTUM_OUTSIDE      2

#define DEFAULT_NEAR    (0.1)
#define DEFAULT_FAR      (512.0)

#define DEFAULT_NEAR_MIN  (0.1)
#define DEFAULT_NEAR_MAX  (1.0)
#define DEFAULT_FAR_MIN    (10.0)
#define DEFAULT_FAR_MAX    (500.0)

#define NEAR_NOTCH    (0.05)
#define FAR_NOTCH    (20.0)

class GlCamera {
public:
  GlCamera();
  ~GlCamera();

  void resize_screen(int w, int h);

  void set_fov_near_far(double fov, double n, double f);
  void set_near_far_min_max(double nmin, double nmax, double fmin, double fmax);

  void adjust_near(double amount);
  void adjust_far(double amount);

  void set_far(double new_far);

  v2d_t getNearAndFar() const;
  void setNearAndFar(v2d_t nearAndFar);

  v3d_t getPosition() const;

  void perspective();
  void look_at(v3d_t position, v3d_t target, v3d_t upVector);
  void updateGlVariables();

  void set_planes();

  int point_visible(v3d_t p);
  int point_visible(v3di_t p);

  int bounding_box_test(BoundingBox& box);
  int bounding_sphere_test(const BoundingSphere& sphere) const;

private:
  int mScreenWidth;        // the dimensions of the viewport
  int mScreenHeight;

  double fovy;        // y-axis viewing angle
  double tan_fovy;      // tan(fovy) for speed
  double aspect;        // aspect ratio (mScreenWidth / mScreenHeight)
  double near_d, far_d;    // near and far draw distances
  double near_w, near_h;    // height and width of the near view plane
  double far_w, far_h;    // height and width of the far view plane

  double near_min;      // constraints for the near and far viewing planes
  double near_max;
  double far_min;
  double far_max;

  v3d_t mPosition;        // where the camera is located
  v3d_t mTarget;          // what the camera is looking at
  v3d_t mUpVector;        // the 'up' vector
//  v3d_t mRightVector;        // the 'right' vector

  Plane3d mPlanes[6];      // the planes for each frustum face
};

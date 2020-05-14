#include "../math/GlCamera.h"

GlCamera::GlCamera() {
  near_d = DEFAULT_NEAR;
  far_d = DEFAULT_FAR;

  near_min = DEFAULT_NEAR_MIN;
  near_max = DEFAULT_NEAR_MAX;
  far_min = DEFAULT_FAR_MIN;
  far_max = DEFAULT_FAR_MAX;

  fovy = DEG2RAD (45.0);
  aspect = 1.333;
}

GlCamera::~GlCamera() {
}

void GlCamera::resize_screen(int w, int h) {
  mScreenWidth = w;
  mScreenHeight = h;

  aspect = (double)mScreenWidth / (double)mScreenHeight;
}

void GlCamera::set_fov_near_far(double fov, double n, double f) {
  fovy = fov;

  near_d = clamp(n, near_min, near_max);
  far_d = clamp(f, far_min, far_max);

  tan_fovy = tan(DEG2RAD(fovy * 0.5));

  near_h = near_d * tan_fovy;
  near_w = near_h * aspect;

  far_h = far_d * tan_fovy;
  far_w = far_h * aspect;

  // now fix the fog
  glFogf(GL_FOG_START, static_cast<GLfloat>(far_d * 0.20));    // Fog Start Depth
  glFogf(GL_FOG_END, static_cast<GLfloat>(far_d));        // Fog End Depth

//  printf ("draw distance: %4.2f\n", far_d);
}

void GlCamera::set_near_far_min_max(double nmin, double nmax, double fmin, double fmax) {
  near_min = nmin;
  near_max = nmax;
  far_min = fmin;
  far_max = fmax;
}

void GlCamera::adjust_near(double amount) {
  set_fov_near_far(fovy, near_d + amount, far_d);
}

void GlCamera::adjust_far(double amount) {
  set_fov_near_far(fovy, near_d, far_d + amount);
}

void GlCamera::set_far(double new_far) {
  set_fov_near_far(fovy, near_d, new_far);
}

v2d_t GlCamera::getNearAndFar() const {
  return v2d_v(near_d, far_d);
}

void GlCamera::setNearAndFar(v2d_t nearAndFar) {
  set_fov_near_far(fovy, nearAndFar.x, nearAndFar.y);
  updateGlVariables();
}

v3d_t GlCamera::getPosition() const {
  return mPosition;
}

void GlCamera::perspective() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, aspect, near_d, far_d + 3.0);
}

void GlCamera::look_at(v3d_t position, v3d_t target, v3d_t upVector) {
  mPosition = position;
  mTarget = target;
  mUpVector = v3d_normalize(upVector);
//  mRightVector = v3d_normalize(v3d_cross (v3d_sub(mTarget, mPosition), mUpVector));

  // generate the frustum planes
  set_planes();

  // now for the opengl stuff
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(position.x, position.y, position.z,
        target.x, target.y, target.z,
        upVector.x, upVector.y, upVector.z);
}

// FIXME: this is very specific to longshot::world::draw_sun()
void GlCamera::updateGlVariables() {
  perspective();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(mPosition.x, mPosition.y, mPosition.z,
        mTarget.x, mTarget.y, mTarget.z,
        mUpVector.x, mUpVector.y, mUpVector.z);
}

// make the view frustum's planes
void GlCamera::set_planes() {
  // this code based on:
  // http://www.lighthouse3d.com/opengl/viewfrustum/index.php?gimp

  v3d_t nc, fc, X, Y, Z;

  // compute the Z axis of camera
  // this axis points in the opposite direction from
  // the looking direction
  Z = v3d_normalize(v3d_sub(mPosition, mTarget));

  // X axis of camera with given "up" vector and Z axis
  X = v3d_normalize(v3d_cross(mUpVector, Z));

  // the real "up" vector is the cross product of Z and X
  Y = v3d_cross(Z, X);

  // compute the centers of the near and far planes
  nc = v3d_sub(mPosition, v3d_scale(Z, near_d));
  fc = v3d_sub(mPosition, v3d_scale(Z, far_d));

  // now compute the planes
  mPlanes[PLANE_NEA].setFromPointAndNormal(nc, v3d_neg(Z));
  mPlanes[PLANE_FAR].setFromPointAndNormal(fc, Z);

  v3d_t aux, normal;

  aux = v3d_normalize(v3d_sub(v3d_add(nc, v3d_scale(Y, near_h)), mPosition));
  normal = v3d_cross (aux, X);
  mPlanes[PLANE_TOP].setFromPointAndNormal(v3d_add(nc, v3d_scale(Y, near_h)), normal);

  aux = v3d_normalize(v3d_sub(v3d_sub(nc, v3d_scale(Y, near_h)), mPosition));
  normal = v3d_cross (X, aux);
  mPlanes[PLANE_BOT].setFromPointAndNormal(v3d_sub(nc, v3d_scale(Y, near_h)), normal);

  aux = v3d_normalize(v3d_sub(v3d_sub(nc, v3d_scale(X, near_w)), mPosition));
  normal = v3d_cross (aux, Y);
  mPlanes[PLANE_LEF].setFromPointAndNormal(v3d_sub(nc, v3d_scale(X, near_w)), normal);

  aux = v3d_normalize(v3d_sub(v3d_add(nc, v3d_scale(X, near_w)), mPosition));
  normal = v3d_cross (Y, aux);
  mPlanes[PLANE_RIG].setFromPointAndNormal(v3d_add(nc, v3d_scale(X, near_w)), normal);
}

int GlCamera::point_visible(v3d_t p) {
  for (int i = 0; i < 6; ++i) {
    if (mPlanes[i].isPointOutside(p)) return 0;
  }
  return 1;
}

int GlCamera::point_visible(v3di_t p) {
  v3d_t pi;

  pi.x = (double)p.x;
  pi.y = (double)p.y;
  pi.z = (double)p.z;

  for (int i = 0; i < 6; ++i) {
    if (mPlanes[i].isPointOutside(pi)) return 0;
  }

  return 1;
}

// tests whether a bounding_box_c is inside a frustum
int GlCamera::bounding_box_test(BoundingBox& box) {
  // v3d_t corner[8];
  // bool all_in = true;
  //
  // box.get_corners (corner);
  //
  // for (int p = 0; p < 6; p++) {
  //   int in_count = 8;
  //   int pt_in = 1;
  //
  //   for (int c = 0; c < 8; c++) {
  //     if (plane[p].point_outside (corner[c])) {
  //       all_in = false;
  //       in_count--; } }
  //
  //   // all points are on the outside of plane[p]: all out baby
  //   if (in_count == 0) return FRUSTUM_OUTSIDE;
  // }
  //
  // if (all_in) {
  //   return FRUSTUM_INSIDE;
  // }

  // FIXME: please fix this

  printf("ERROR: GlCamera::bounding_box_test is broken. please fix.\n");

  return FRUSTUM_INTERSECT;
}

// tests whether a bounding sphere is inside a frustum
int GlCamera::bounding_sphere_test(const BoundingSphere& sphere) const {
  bool all_in = true;
  double d;
  v3d_t center = sphere.getPosition();
  double radius = sphere.getRadius();

  for (int i = 0; i < 6; ++i) {
    d = mPlanes[i].distToPoint(center);
    if (d < -radius) return FRUSTUM_OUTSIDE;
    if (d < radius) all_in = false;
  }

  if (all_in) {
    return FRUSTUM_INSIDE;
  }

  return FRUSTUM_INTERSECT;
}

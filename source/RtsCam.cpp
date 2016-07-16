#include "RtsCam.h"



rts_cam_c::rts_cam_c () {
  // set the target the be the origin
  mTarget = v3d_v (0.0, 0.0, 0.0);

  // setup the camera
  initializeCamera (v2di_v (800, 600), 45.0, 0.1, 1024.0);

  mDistanceFromTarget = 128.0;
  mNearLimit = 1.0;
  mFarLimit = 512.0;

  mFacing = 0.0;

  mElevation = DEG2RAD (45.0);
  mElevationMin = DEG2RAD (-85.0);
  mElevationMax = DEG2RAD (85.0);

  // figure out the camera position from the memebers already set
  updatePosition ();

  // set up to be along y+
  mUpVector = v3d_v (0, 1, 0);
}



rts_cam_c::~rts_cam_c () {
}



void rts_cam_c::initializeCamera (v2di_t screenDimensions, double FOV, double nearPlane, double farPlane) {
  mGlCamera.resize_screen (screenDimensions.x, screenDimensions.y);
  mGlCamera.set_fov_near_far (FOV, nearPlane, farPlane);
}



// set with target, facing, elevation, and distance
int rts_cam_c::setup_cam (v3d_t t, double f, double e, double d) {
  return 0;
}



// constrain the camera
int rts_cam_c::constrain (void) {
  if (mElevation < mElevationMin) mElevation = mElevationMin;
  else if (mElevation > mElevationMax) mElevation = mElevationMax;

  while (mFacing > MY_2PI) mFacing -= MY_2PI;
  while (mFacing < 0) mFacing += MY_2PI;

  if (mDistanceFromTarget < mNearLimit) mDistanceFromTarget = mNearLimit;
  else if (mDistanceFromTarget > mFarLimit) mDistanceFromTarget = mFarLimit;

  return 0;
}



// update the camera position according to target, facing, elevation and distance
int rts_cam_c::updatePosition (void) {
  constrain ();

  mPosition.x = mDistanceFromTarget * cos (mElevation);
  mPosition.y = mDistanceFromTarget * sin (mElevation);
  mPosition.z = 0.0;

  // now rotate about the y-axis
  v3d_t temp = mPosition;

  mPosition.x = (temp.x * cos (mFacing)) - (temp.z * sin (mFacing));
  mPosition.z = (temp.x * sin (mFacing)) + (temp.z * cos (mFacing));

  mPosition = v3d_add (mTarget, mPosition);

  return 0;
}



// pan camera around view globe
int rts_cam_c::pan (v2d_t a) {
  mFacing += a.x;
  mElevation += a.y;

  updatePosition ();

  return 0;
}



// zoom the camera
int rts_cam_c::zoom (double a) {
  mDistanceFromTarget += a;

  updatePosition ();

  return 0;
}



// translate the camera and target position
int rts_cam_c::translate (v2d_t a) {
  v2d_t t1 = v2d_sub (v2d_v (mTarget.x, mTarget.z), v2d_v (mPosition.x, mPosition.z));

  v2d_t t_fb; // translation forward or back
  v2d_t t_lr; // translate left and right

  t_fb = v2d_scale (v2d_normalize (t1), a.y);
  t_lr = v2d_scale (v2d_normalize (v2d_normal (t1)), a.x);

  mTarget.x += t_fb.x + t_lr.x;
  mTarget.z += t_fb.y + t_lr.y;

  updatePosition ();

  return 0;
}



void rts_cam_c::changeElevation (double amount) {
  mTarget.y += amount;

  updatePosition ();
}



// change the camera position
void rts_cam_c::setPosition (v3d_t position) {
  mPosition = position;
}



v3d_t rts_cam_c::getPosition (void) const {
  return mPosition;
}



void rts_cam_c::setTarget (v3d_t target) {
  mTarget = target;
}



void rts_cam_c::setTargetAndUpdate (v3d_t target) {
  mTarget = target;
  updatePosition ();
}



v3d_t rts_cam_c::getTarget (void) const {
  return mTarget;
}



// set the near and far values for the camera
int rts_cam_c::set_near_and_far (double n, double f) {
  return 0;
}



// use the gluLookAt () to set view at render time
GlCamera *rts_cam_c::gl_cam_setup (void) {
  mGlCamera.perspective ();

  mGlCamera.look_at (mPosition, mTarget, mUpVector);

//  v3d_print ("pos", mPosition);
//  v3d_print ("targ", mTarget);
//  v3d_print ("up", mUpVector);

  return &mGlCamera;
}




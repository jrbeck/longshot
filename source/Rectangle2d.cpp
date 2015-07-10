#include "Rectangle2d.h"


Rectangle2d::Rectangle2d () {
  setCorners (v2d_v (0.0, 0.0), v2d_v (1.0, 1.0));
}



Rectangle2d::Rectangle2d (v2d_t nearCorner, v2d_t farCorner) {
  setCorners (nearCorner, farCorner);
}



void Rectangle2d::setCorners (v2d_t nearCorner, v2d_t farCorner) {
  mNearCorner = nearCorner;
  mFarCorner = farCorner;
}



v2d_t Rectangle2d::getNearCorner (void) const {
  return mNearCorner;
}



v2d_t Rectangle2d::getFarCorner (void) const {
  return mFarCorner;
}


double Rectangle2d::getWidth(void) const {
  return mFarCorner.x - mNearCorner.x;
}


double Rectangle2d::getHeight(void) const {
  return mFarCorner.y - mNearCorner.y;
}



bool Rectangle2d::isIntersecting (const Rectangle2d &other) const {
  // test the near corner
  if (mFarCorner.x < other.mNearCorner.x) return false;
  if (mFarCorner.y < other.mNearCorner.y) return false;

  // test the far corner
  if (mNearCorner.x > other.mFarCorner.x) return false;
  if (mNearCorner.y > other.mFarCorner.y) return false;

  return true;
}

v2d_t Rectangle2d::transformPoint(v2d_t point, const Rectangle2d &other) const {
  v2d_t trans;

  double delta = point.x - mNearCorner.x;
  double percent = delta / (mFarCorner.x - mNearCorner.x);
  trans.x = other.getNearCorner().x + (percent * (other.getFarCorner().x - other.getNearCorner().x));

  delta = point.y - mNearCorner.y;
  percent = delta / (mFarCorner.y - mNearCorner.y);
  trans.y = other.getNearCorner().y + (percent * (other.getFarCorner().y - other.getNearCorner().y));

  return trans;
}



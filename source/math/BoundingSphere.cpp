#include "../math/BoundingSphere.h"

BoundingSphere::BoundingSphere(void) {
  mPosition = v3d_v (0.0, 0.0, 0.0);
  mRadius = 1.0;
}

BoundingSphere::BoundingSphere(v3d_t position, double radius) {
  mPosition = position;
  mRadius = radius;
}

BoundingSphere::BoundingSphere(double x, double y, double z, double radius) {
  mPosition.x = x;
  mPosition.y = y;
  mPosition.z = z;
  mRadius = radius;
}

/*
inline bool BoundingSphere::isPointInside (v3d_t point) const {
  double distance = v3d_mag (v3d_sub (point, mPosition));

  if (distance < mRadius) return true;

  return false;
}
*/

void BoundingSphere::setPosition(v3d_t position) {
  mPosition = position;
}

v3d_t BoundingSphere::getPosition(void) const {
  return mPosition;
}

void BoundingSphere::setRadius(double radius) {
  mRadius = radius;
}

double BoundingSphere::getRadius(void) const {
  return mRadius;
}

#include "../math/Plane3d.h"

Plane3d::Plane3d() {
  // FIXME: this may be too expensive
  // need some good constructors...
//  mNormal = v3d_v(0.0, 1.0, 0.0);
//  point = v3d_v(0.0, 0.0, 0.0);
}

Plane3d::~Plane3d() {
}

void Plane3d::setFrom3Points(v3d_t p1, v3d_t p2, v3d_t p3) {
  v3d_t u = v3d_sub(p2, p1);
  v3d_t v = v3d_sub(p3, p1);

  mNormal = v3d_normalize(v3d_cross(v, u));
  mPoint = p1;
}

// WARNING: assumes the normal is normalized
void Plane3d::setFromPointAndNormal(v3d_t p, v3d_t n) {
  mNormal = n;
  mPoint = p;
}

void Plane3d::setFromPointAnd2Vectors(v3d_t p, v3d_t u, v3d_t v) {
  mNormal = v3d_normalize(v3d_cross(u, v));
  mPoint = p;
}

bool Plane3d::isPointOn(v3d_t p) const {
  if (std::abs(v3d_dot(v3d_sub(p, mPoint), mNormal)) <= PLANE_EPSILON) {
    return true;
  }
  return false;
}

bool Plane3d::isPointInside(v3d_t p) const {
  if (v3d_dot(v3d_sub(p, mPoint), mNormal) > PLANE_EPSILON) {
    return true;
  }
  return false;
}

bool Plane3d::isPointOutside(v3d_t p) const {
  if (v3d_dot(v3d_sub(p, mPoint), mNormal) < -PLANE_EPSILON) {
    return true;
  }
  return false;
}

bool Plane3d::doesLineSegmentIntersect(v3d_t a, v3d_t b, double &time) {
  v3d_t lineVec = v3d_sub(b, a);
  double lineSegmentLength = v3d_mag(lineVec);

  // check for a line that doesn't go anywhere
  if (lineSegmentLength < PLANE_EPSILON) {
    return false;
  }

  double numerator = v3d_dot(v3d_sub(mPoint, a), mNormal);
  double denominator = v3d_dot(v3d_normalize(lineVec), mNormal);

  // check for a parallel line intersecting the plane
  if (std::abs(numerator) < PLANE_EPSILON) {
    if (std::abs(denominator) < PLANE_EPSILON) {
      time = 0.0;
      return true;
    }
  }

  // check for a parallel line which never intersects
  if (std::abs(denominator) < PLANE_EPSILON) {
    return false;
  }

  double distance = numerator / denominator;
  time = distance / lineSegmentLength;

  return true;
}

double Plane3d::distToPoint(v3d_t p) const {
  return v3d_dot(v3d_sub(p, mPoint), mNormal);
}

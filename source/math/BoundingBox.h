// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BoundingBox
// *
// *
// *
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <algorithm>

#include "../math/v3d.h"
#include "../math/Rectangle2d.h"

using namespace std;

class BoundingBox {
public:
  BoundingBox();
  BoundingBox(v3d_t dimensions);
  BoundingBox(v3d_t dimensions, v3d_t position);

  void setDimensions(v3d_t dimensions);
  void setPosition(v3d_t position);

  void setCenterPosition(v3d_t position);
  v3d_t getCenterPosition() const;
  v3d_t getNearCorner() const;
  v3d_t getFarCorner() const;
  v3d_t getDimensions() const;

  double computeVolume();
  double getVolume() const;

  void scale(double scalar);
  void translate(v3d_t vector);

  bool isPointInside(v3d_t point) const;
  bool isIntersecting(const BoundingBox &other) const;

  bool clipDisplacement(bool test_x, bool test_y, bool test_z, v3d_t &displacement, const BoundingBox &other) const;
  bool sweepTest(const BoundingBox &other, v3d_t displacement, v3d_t &time0) const;

  // members * * * * * * * * * * * * * * * *
  v3d_t mDimensions;

  double mVolume;

  // for pre-compute optimized situations
  // not needed for different pos every call - use (name of function) for that
  v3d_t mNearCorner;

  // the position of the bounding box's corner opposite from pos
  v3d_t mFarCorner;

private:
  // is this an ok place for this?
  enum {
    TEST_LESS_THAN,
    TEST_INSIDE,
    TEST_GREATER_THAN
  };

};

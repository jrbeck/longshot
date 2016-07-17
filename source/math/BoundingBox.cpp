#include "BoundingBox.h"

BoundingBox::BoundingBox() {
  setDimensions(v3d_v(1.0, 1.0, 1.0));
  setPosition(v3d_zero());
}

BoundingBox::BoundingBox(v3d_t dimensions) {
  setDimensions(dimensions);
  setPosition(v3d_zero());
}

BoundingBox::BoundingBox(v3d_t dimensions, v3d_t position) {
  setDimensions(dimensions);
  setPosition(position);
}

void BoundingBox::setDimensions(v3d_t dimensions) {
  v3d_t delta = v3d_scale(-0.5, v3d_sub(dimensions, mDimensions));
  mNearCorner = v3d_add(mNearCorner, delta);
  mDimensions = dimensions;
  mFarCorner = v3d_add(mNearCorner, mDimensions);

  computeVolume();
}

void BoundingBox::setPosition(v3d_t position) {
  mNearCorner = position;
  mFarCorner = v3d_add(mNearCorner, mDimensions);
}

void BoundingBox::setCenterPosition(v3d_t position) {
  v3d_t halfDimensions = v3d_scale(0.5, mDimensions);
  mNearCorner = v3d_sub(position, halfDimensions);
  mFarCorner = v3d_add(mNearCorner, mDimensions);
}

v3d_t BoundingBox::getCenterPosition() const {
  return v3d_add(mNearCorner, v3d_scale(0.5, mDimensions));
}

v3d_t BoundingBox::getNearCorner() const {
  return mNearCorner;
}

v3d_t BoundingBox::getFarCorner() const {
  return mFarCorner;
}

v3d_t BoundingBox::getDimensions() const {
  return mDimensions;
}

double BoundingBox::computeVolume() {
  mVolume = mDimensions.x * mDimensions.y * mDimensions.z;
  return mVolume;
}

double BoundingBox::getVolume() const {
  return mVolume;
}

void BoundingBox::scale(double scalar) {
  setDimensions(v3d_scale(scalar, mDimensions));
}

void BoundingBox::translate(v3d_t vector) {
  v3d_t new_position = v3d_add(mNearCorner, vector);
  setPosition(new_position);
}

bool BoundingBox::isPointInside(v3d_t point) const {
  if (point.x <= mNearCorner.x) return false;
  if (point.x >= mFarCorner.x) return false;
  if (point.y <= mNearCorner.y) return false;
  if (point.y >= mFarCorner.y) return false;
  if (point.z <= mNearCorner.z) return false;
  if (point.z >= mFarCorner.z) return false;
  return true;
}

bool BoundingBox::isIntersecting(const BoundingBox &other) const {
  if (mNearCorner.x > other.mFarCorner.x) return false;
  if (mFarCorner.x < other.mNearCorner.x) return false;
  if (mNearCorner.y > other.mFarCorner.y) return false;
  if (mFarCorner.y < other.mNearCorner.y) return false;
  if (mNearCorner.z > other.mFarCorner.z) return false;
  if (mFarCorner.z < other.mNearCorner.z) return false;
  return true;
}

bool BoundingBox::sweepTest(const BoundingBox& other, v3d_t displacement, v3d_t& time0) const {
  double t0, t1;

  // if they are intersecting before the translation, return true
  //  if (isIntersecting (other)) {
  //    printf ("intersect\n");
  //    time0 = v3d_zero ();
  //    return true;
  //  }

  displacement = v3d_neg(displacement);
  time0 = v3d_v(0.0, 0.0, 0.0);
  v3d_t time1 = v3d_v(1.0, 1.0, 1.0);

  // x-axis * * * *
  if (mFarCorner.x < other.mNearCorner.x) {
    if (displacement.x < 0.0) {
      time0.x = (mFarCorner.x - other.mNearCorner.x) / displacement.x;
    }
    else {
      return false;
    }
  }
  else if (other.mFarCorner.x < mNearCorner.x) {
    if (displacement.x > 0.0) {
      time0.x = (mNearCorner.x - other.mFarCorner.x) / displacement.x;
    }
    else {
      return false;
    }
  }

  if (other.mFarCorner.x > mNearCorner.x && displacement.x < 0.0) {
    time1.x = (mNearCorner.x - other.mFarCorner.x) / displacement.x;
  }
  else if (mFarCorner.x > other.mNearCorner.x && displacement.x > 0.0) {
    time1.x = (mFarCorner.x - other.mNearCorner.x) / displacement.x;
  }

  // y-axis * * * *
  if (mFarCorner.y < other.mNearCorner.y) {
    if (displacement.y < 0.0) {
      time0.y = (mFarCorner.y - other.mNearCorner.y) / displacement.y;
    }
    else {
      return false;
    }
  }
  else if (other.mFarCorner.y < mNearCorner.y) {
    if (displacement.y > 0.0) {
      time0.y = (mNearCorner.y - other.mFarCorner.y) / displacement.y;
    }
    else {
      return false;
    }
  }

  if (other.mFarCorner.y > mNearCorner.y && displacement.y < 0.0) {
    time1.y = (mNearCorner.y - other.mFarCorner.y) / displacement.y;
  }
  else if (mFarCorner.y > other.mNearCorner.y &&
    displacement.y > 0.0) {
    time1.y = (mFarCorner.y - other.mNearCorner.y) / displacement.y;
  }

  // z-axis * * * *
  if (mFarCorner.z < other.mNearCorner.z) {
    if (displacement.z < 0.0) {
      time0.z = (mFarCorner.z - other.mNearCorner.z) / displacement.z;
    }
    else {
      return false;
    }
  }

  else if (other.mFarCorner.z < mNearCorner.z) {
    if (displacement.z > 0.0) {
      time0.z = (mNearCorner.z - other.mFarCorner.z) / displacement.z;
    }
    else {
      return false;
    }
  }

  if (other.mFarCorner.z > mNearCorner.z && displacement.z < 0.0) {
    time1.z = (mNearCorner.z - other.mFarCorner.z) / displacement.z;
  }
  else if (mFarCorner.z > other.mNearCorner.z && displacement.z > 0.0) {
    time1.z = (mFarCorner.z - other.mNearCorner.z) / displacement.z;
  }

  // determine which axis hit first
  /*  if (time0.x > time0.y) {
      if (time0.x > time0.z) {
      axis = 0;
      t0 = time0.x;
      }
      else {
      axis = 2;
      t0 = time0.z;
      }
      }
      else if (time0.y > time0.z) {
      axis = 1;
      t0 = time0.y;
      }
      else {
      axis = 2;
      t0 = time0.z;
      }*/

  t0 = max(time0.x, max(time0.y, time0.z));
  t1 = min(time1.x, min(time1.y, time1.z));

  if (time0.x > t1) time0.x = 0.0;
  if (time0.y > t1) time0.y = 0.0;
  if (time0.z > t1) time0.z = 0.0;

  if (t0 > 1.0 ||
    t0 <= 0.0) {
    return false;
  }

  return t0 <= t1;
}

bool BoundingBox::clipDisplacement(
  bool test_x, bool test_y, bool test_z,
  v3d_t &displacement,
  const BoundingBox &other) const
{
  // obtain nearest edges for other
  v3d_t o_near = other.getNearCorner();
  v3d_t o_far = other.getFarCorner();

  v3d_t leading;    // leading faces for this BoundingBox
  v3d_t nearest;    // nearest faces of the other bb

  // x-axis
  if (displacement.x < 0.0) {
    leading.x = mNearCorner.x;
    nearest.x = o_far.x;
  }
  else if (displacement.x > 0.0) {
    leading.x = mFarCorner.x;
    nearest.x = o_near.x;
  }

  // y-axis
  if (displacement.y < 0.0) {
    leading.y = mNearCorner.y;
    nearest.y = o_far.y;
  }
  else if (displacement.y > 0.0) {
    leading.y = mFarCorner.y;
    nearest.y = o_near.y;
  }

  // z-axis
  if (displacement.z < 0.0) {
    leading.z = mNearCorner.z;
    nearest.z = o_far.z;
  }
  else if (displacement.z > 0.0) {
    leading.z = mFarCorner.z;
    nearest.z = o_near.z;
  }

  // translate leading faces by displacement
  v3d_t translated = v3d_add(leading, displacement);

  v3d_t percent;  // the percent of displacement where a collision occurs

  // for each axis check if collision occurs
  // x-axis
  if (displacement.x == 0.0) {
    percent.x = 1.0;
  }
  else if (((nearest.x < leading.x) && (nearest.x < translated.x)) ||
    ((nearest.x > leading.x) && (nearest.x > translated.x))) {
    // no possibility for collision
    percent.x = 1.0;
  }
  else {
    percent.x = (nearest.x - leading.x) / displacement.x;

    // these are to check rectangle intersection
    v3d_t disp_scaled = v3d_scale(displacement, percent.x);
    v3d_t nc_moved = v3d_add(mNearCorner, disp_scaled);
    v3d_t fc_moved = v3d_add(mFarCorner, disp_scaled);

    Rectangle2d a(v2d_v(nc_moved.y, nc_moved.z), v2d_v(fc_moved.y, fc_moved.z));
    Rectangle2d b(v2d_v(o_near.y, o_near.z), v2d_v(o_far.y, o_far.z));

    // if not intersecting, don't run tests on this one
    if (!a.isIntersecting(b)) {
      percent.x = 1.0;
    }
  }


  // y-axis
  if (displacement.y == 0.0) {
    percent.y = 1.0;
  }
  else if (((nearest.y < leading.y) && (nearest.y < translated.y)) ||
    ((nearest.y > leading.y) && (nearest.y > translated.y)))
  {
    // no possibility for collision
    percent.y = 1.0;
  }
  else {
    percent.y = (nearest.y - leading.y) / displacement.y;

    // these are to check rectangle intersection
    v3d_t disp_scaled = v3d_scale(displacement, percent.y);
    v3d_t nc_moved = v3d_add(mNearCorner, disp_scaled);
    v3d_t fc_moved = v3d_add(mFarCorner, disp_scaled);

    Rectangle2d a(v2d_v(nc_moved.x, nc_moved.z), v2d_v(fc_moved.x, fc_moved.z));
    Rectangle2d b(v2d_v(o_near.x, o_near.z), v2d_v(o_far.x, o_far.z));

    // if not intersecting, don't run tests on this one
    if (!a.isIntersecting(b)) {
      percent.y = 1.0;
    }
  }


  // z-axis
  if (displacement.z == 0.0) {
    percent.z = 1.0;
  }
  else if (((nearest.z < leading.z) && (nearest.z < translated.z)) ||
    ((nearest.z > leading.z) && (nearest.z > translated.z)))
  {
    // no possibility for collision
    percent.z = 1.0;
  }
  else {
    percent.z = (nearest.z - leading.z) / displacement.z;

    // these are to check rectangle intersection
    v3d_t disp_scaled = v3d_scale(displacement, percent.z);
    v3d_t nc_moved = v3d_add(mNearCorner, disp_scaled);
    v3d_t fc_moved = v3d_add(mFarCorner, disp_scaled);

    Rectangle2d a(v2d_v(nc_moved.x, nc_moved.y), v2d_v(fc_moved.x, fc_moved.y));
    Rectangle2d b(v2d_v(o_near.x, o_near.y), v2d_v(o_far.x, o_far.y));

    // if not intersecting, don't run tests on this one
    if (!a.isIntersecting(b)) {
      percent.z = 1.0;
    }
  }

  if (percent.x == 1.0 && percent.y == 1.0 && percent.z == 1.0) {
    return false;
  }

  // one of these must work
  if (test_x &&
    percent.x < percent.y &&
    percent.x < percent.z)
  {
    displacement.x *= percent.x;
    return true;
  }
  else if (test_y && percent.y < percent.z) {
    displacement.y *= percent.y;
    return true;
  }
  else if (test_z) {
    displacement.z *= percent.z;
    return true;
  }

  return false;
}

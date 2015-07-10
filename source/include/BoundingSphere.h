// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BoundingSphere
// *
// *
// *
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef BoundingSphere_h_
#define BoundingSphere_h_

#include "v3d.h"

class BoundingSphere {
public:
  BoundingSphere (void);
  BoundingSphere (v3d_t position, double radius);
  BoundingSphere (double x, double y, double z, double radius);

  // let's inline this sucker
  bool isPointInside (v3d_t point) const {
    double distance = v3d_mag (v3d_sub (point, mPosition));
    if (distance < mRadius) return true;
    return false;
  }

//  bool isPointInside (v3d_t point) const;

  void setPosition (v3d_t position);
  v3d_t getPosition (void) const;

  void setRadius (double radius);
  double getRadius (void) const;

private:
  v3d_t mPosition;
  double mRadius;
};


#endif // BoundingSphere_h_

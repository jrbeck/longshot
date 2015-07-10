// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Plane3d
// *
// * very basic 3d plane abstraction
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Plane3d_h_
#define Plane3d_h_

#include <cmath>

#include "v3d.h"

#define PLANE_EPSILON    (0.00001)

class Plane3d {
public:
  Plane3d ();
  ~Plane3d ();

  void setFrom3Points (v3d_t p1, v3d_t p2, v3d_t p3);
  void setFromPointAndNormal (v3d_t p, v3d_t n);
  void setFromPointAnd2Vectors (v3d_t p, v3d_t u, v3d_t v);

  bool isPointOn (v3d_t p) const;
  bool isPointInside (v3d_t p) const;
  bool isPointOutside (v3d_t p) const;

  bool doesLineSegmentIntersect (v3d_t a, v3d_t b, double &time);

  double distToPoint (v3d_t p) const;

private:
  v3d_t mNormal;
  v3d_t mPoint;
};


#endif // Plane3d_h_

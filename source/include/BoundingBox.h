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

#ifndef BoundingBox_h_
#define BoundingBox_h_

#include <algorithm>

#include "v3d.h"
#include "Rectangle2d.h"

using namespace std;


class BoundingBox {
public:
	// default constructor: unit cube, pos at origin
	BoundingBox ();

	// constructor: dimensions specified, pos at origin
	BoundingBox (v3d_t dimensions);

	// constructor: dimensions specified, pos specifed
	BoundingBox (v3d_t dimensions, v3d_t position);

	// set_dimensions: resizes the bb
	void setDimensions (v3d_t dimensions);

	// set_pos: sets the position of the bb
	void setPosition (v3d_t position);

	void setCenterPosition (v3d_t position);
	v3d_t getCenterPosition (void) const;
	v3d_t getNearCorner (void) const;
	v3d_t getFarCorner (void) const;
	v3d_t getDimensions (void) const;

	void scale (double scalar);
	void translate (v3d_t vector);

	// intersects: test for collision against another bb using stored position
	// assumes other bb has pos set appropriately
	bool isIntersecting (const BoundingBox &other) const;

	// clip displacement of one bounding box against that of another
	bool clipDisplacement (bool test_x, bool test_y, bool test_z,
							v3d_t &displacement, const BoundingBox &other) const;

	bool sweepTest (const BoundingBox &other, v3d_t displacement, v3d_t &time0) const;

	// point_inside: tests to see if a point is inside the bb
	bool isPointInside (v3d_t point) const;

	// members * * * * * * * * * * * * * * * *
	// dimensions of the bounding box
	v3d_t mDimensions;

	// for pre-compute optimized situations
	// not needed for different pos every call - use (name of function) for that
	v3d_t mNearCorner;

	// the position of the bounding box's corner opposite from pos
	v3d_t mFarCorner;

private:
	static enum {
		TEST_LESS_THAN,
		TEST_INSIDE,
		TEST_GREATER_THAN
	};

};



#endif // BoundingBox_h_

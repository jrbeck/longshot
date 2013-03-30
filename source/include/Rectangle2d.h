// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Rectangle2d
// *
// *
// *
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#ifndef Rectangle2d_h_
#define Rectangle2d_h_


#include "v2d.h"

enum {
	TEST_LESS_THAN,
	TEST_INSIDE,
	TEST_GREATER_THAN
};


class Rectangle2d {
public:
	Rectangle2d ();
	Rectangle2d (v2d_t nearCorner, v2d_t farCorner);

	void setCorners(v2d_t nearCorner, v2d_t farCorner);

	v2d_t getNearCorner(void) const;
	v2d_t getFarCorner(void) const;

	double getWidth(void) const;
	double getHeight(void) const;

	bool isIntersecting(const Rectangle2d &other) const;

	v2d_t transformPoint(v2d_t point, const Rectangle2d &other) const;


// MEMBERS
	v2d_t mNearCorner;
	v2d_t mFarCorner;
};




#endif // Rectangle2d_h_

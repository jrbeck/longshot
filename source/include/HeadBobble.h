// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * HeadBobble
// *
// * this simluates the head movement in reaction to in-game events.
// * for instance, running, walking, swimming, etc...produce different orientation
// * changes for the head (eye reference frame), which must be animated and
// * interpolated smoothly between
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef HeadBobble_h_
#define HeadBobble_h_


#include "v2d.h"
#include "v3d.h"


enum {
	HEADBOB_ACTION_STAND,
	HEADBOB_ACTION_JUMP,
	HEADBOB_ACTION_WALK_FORWARD,
	HEADBOB_ACTION_STRAFE_LEFT,
	HEADBOB_ACTION_STRAFE_RIGHT,
	HEADBOB_ACTION_TREAD_WATER,
	HEADBOB_ACTION_SWIM_FORWARD,
	HEADBOB_ACTION_SWIM_BACKWARD,
	HEADBOB_ACTION_FALL_IMPACT,
	HEADBOB_ACTION_TAKE_DAMAGE,

	NUM_HEADBOB_ACTIONS
};



class HeadBobble {
public:

	HeadBobble ();

	void reset (void);

//	void addAction (int action, v3d_t magnitude);

	void update (int action, double time);

	v3d_t getOffset (void) const;
	v2d_t getFacing (void) const;
	v3d_t getUpVector (void) const;

private:
	double mLastTime;
	double mAnimationTime;

	v3d_t mOffset;			// current offset for the noggin
	v3d_t mLastOffset;

	v2d_t mFacing;
	v2d_t mFacingTarget;

	v2d_t mUp;
	v2d_t mUpTarget;
};



#endif // HeadBobble_h_

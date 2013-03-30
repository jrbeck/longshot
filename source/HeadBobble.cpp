#include "HeadBobble.h"



HeadBobble::HeadBobble () {
	reset ();
}



void HeadBobble::reset (void) {
	mLastTime = 0.0;
	mAnimationTime = 0.0;
	mOffset = v3d_v (0.0, 0.0, 0.0);
}



void HeadBobble::update (int action, double time) {
	double deltaTime = time - mLastTime;

	mLastTime = time;

	if (mAnimationTime > MY_2PI) {
		mAnimationTime -= MY_2PI;
	}
	if (mAnimationTime < 0.0) {
		mAnimationTime += MY_2PI;
	}

//	printf ("anim time: %10f ... ", mAnimationTime);


	switch (action) {
		default:
		case HEADBOB_ACTION_STAND:

//			printf ("standing\n");

			if (mAnimationTime < 0.1 ||
				MY_2PI - mAnimationTime < 0.1)
			{
				mAnimationTime = 0.0;
			}
			else if (mAnimationTime < MY_PI)
			{
				mAnimationTime -= 0.09;
			}
			else {
				mAnimationTime += 0.09;
			}

			break;

		case HEADBOB_ACTION_WALK_FORWARD:
//			printf ("walking\n");

			mAnimationTime += deltaTime * 8.0;

			break;
	}


	// FIXME: this produces a 'slash' motion as opposed to an 'upside-down u'
	// seems ok though?
	mOffset.x = 0.075 * cos (mAnimationTime);
	mOffset.y = -0.035 * cos (mAnimationTime);
	mOffset.z = 0.0;

	mLastOffset = mOffset;

}



v3d_t HeadBobble::getOffset (void) const {
	return mOffset;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Animation
// *
// * desc: this holds a bunch of Texture regions for animation purposes
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Animation_h_
#define Animation_h_

#include <vector>

#include "TextureRegion.h"


enum {
	ANIMATION_LOOPING,
	ANIMATION_NONLOOPING
};


class Animation {
public:
	Animation(float frameDuration);
	~Animation();

	// this class takes control of these pointers
	void addFrame(TextureRegion *textureRegion);
	TextureRegion *getFrame(float stateTime, int mode);

private:
	std::vector<TextureRegion *> mFrames;
	float mFrameDuration;
};


#endif // Animation_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Animation
// *
// * desc: this holds a bunch of Texture regions for animation purposes
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>

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
	void addFrame(TextureRegion* textureRegion);
	TextureRegion *getFrame(float stateTime, int mode);

private:
	std::vector<TextureRegion*> mFrames;
	float mFrameDuration;
};

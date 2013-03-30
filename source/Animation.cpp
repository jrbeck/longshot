#include "Animation.h"

Animation::Animation(float frameDuration) {
	mFrameDuration = frameDuration;
}


Animation::~Animation() {
	size_t numFrames = mFrames.size();
	for (size_t i = 0; i < numFrames; i++) {
		if (mFrames[i] != NULL) {
			delete mFrames[i];
		}
	}

	mFrames.clear();
}


void Animation::addFrame(TextureRegion *textureRegion) {
	mFrames.push_back(textureRegion);
}


TextureRegion *Animation::getFrame(float stateTime, int mode) {
	int frameNumber = (int)(stateTime / mFrameDuration);
		
	if (mode == ANIMATION_NONLOOPING) {
		frameNumber = std::min((int)mFrames.size() - 1, frameNumber);
	}
	else {
		frameNumber = frameNumber % mFrames.size();
	}
		
	return mFrames[frameNumber];
}



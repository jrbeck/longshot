#include "WorldLight.h"

WorldLight::WorldLight() :
	mBuffer(NULL)
{
}


WorldLight::~WorldLight() {
	if (mBuffer != NULL) {
		delete [] mBuffer;
	}
}


void WorldLight::set(v3d_t position, double radius, IntColor color) {
	mWorldPosition = position;
	mRadius = radius;
	mColor.r = color.r;
	mColor.g = color.g;
	mColor.b = color.b;

	v3di_t intPosition = v3di_v(mWorldPosition);

	mBufferNear.x = intPosition.x - (int)ceil(mRadius);
	mBufferNear.y = intPosition.y - (int)ceil(mRadius);
	mBufferNear.z = intPosition.z - (int)ceil(mRadius);

	mBufferFar.x = intPosition.x + (int)ceil(mRadius);
	mBufferFar.y = intPosition.y + (int)ceil(mRadius);
	mBufferFar.z = intPosition.z + (int)ceil(mRadius);

	mBufferDimensions.x = mBufferFar.x - mBufferNear.x + 1;
	mBufferDimensions.y = mBufferFar.y - mBufferNear.y + 1;
	mBufferDimensions.z = mBufferFar.z - mBufferNear.z + 1;

	initBuffer();
}


void WorldLight::initBuffer(void) {
	if (mBuffer != NULL) {
		delete [] mBuffer;
		mBuffer = NULL;
	}

	mBuffer = new IntColor[mBufferDimensions.x * mBufferDimensions.y * mBufferDimensions.z];
	if (mBuffer == NULL) {
		printf("WorldLight::initBuffer(): error: out of memory\n");
		return;
	}

	v3di_t bufPos;
	v3d_t wp;	// the v3d version of the current block world position

	int bufIndex = 0;
	for (bufPos.z = 0; bufPos.z < mBufferDimensions.z; bufPos.z++) {
		// the +0.5 puts it in the center of the block area
		wp.z = (double)(mBufferNear.z + bufPos.z) + 0.5;
		for (bufPos.y = 0; bufPos.y < mBufferDimensions.y; bufPos.y++) {
			wp.y = (double)(mBufferNear.y + bufPos.y) + 0.5;
			for (bufPos.x = 0; bufPos.x < mBufferDimensions.x; bufPos.x++) {
				wp.x = (double)(mBufferNear.x + bufPos.x) + 0.5;

				v3d_t diff = v3d_sub(wp, mWorldPosition);
				double sum = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
				double dist = sqrt(sum);

				if (dist <= mRadius) {
					double scalar = (1.0 - (dist / mRadius));
					mBuffer[bufIndex].r = (int)floor((double)mColor.r * scalar);
					mBuffer[bufIndex].g = (int)floor((double)mColor.g * scalar);
					mBuffer[bufIndex].b = (int)floor((double)mColor.b * scalar);
				}
				else {
					mBuffer[bufIndex].r = 0;
					mBuffer[bufIndex].g = 0;
					mBuffer[bufIndex].b = 0;
				}

				// on to the next position
				bufIndex++;
			}
		}
	}
}


IntColor WorldLight::getLevel(v3di_t worldPosition) const {
	static IntColor noColor = { 0, 0, 0 };
	if (isInVolume(worldPosition) == false ||
		mBuffer == NULL)
	{
		return noColor;
	}

	// compute the relative position and 
	return mBuffer[
		(worldPosition.x - mBufferNear.x) +
		((worldPosition.y - mBufferNear.y) * mBufferDimensions.x) +
		((worldPosition.z - mBufferNear.z) * mBufferDimensions.x * mBufferDimensions.y)];
}


bool WorldLight::isInVolume(v3di_t worldPosition) const {
	if (worldPosition.x < mBufferNear.x ||
		worldPosition.x > mBufferFar.x ||
		worldPosition.y < mBufferNear.y ||
		worldPosition.y > mBufferFar.y ||
		worldPosition.z < mBufferNear.z ||
		worldPosition.z > mBufferFar.z)
	{
		return false;
	}

	return true;
}


void WorldLight::turnOff(void) {
	if (mBuffer != NULL) {
		delete [] mBuffer;
		mBuffer = NULL;
	}
}



void WorldLight::turnOn(void) {
	initBuffer();
}


void WorldLight::save(FILE *file) {
	fwrite(&mWorldPosition, sizeof v3d_t, 1, file);
	fwrite(&mRadius, sizeof (double), 1, file);
	fwrite(&mColor.r, sizeof (int), 1, file);
	fwrite(&mColor.g, sizeof (int), 1, file);
	fwrite(&mColor.b, sizeof (int), 1, file);
}


void WorldLight::load(FILE *file) {
	fread(&mWorldPosition, sizeof v3d_t, 1, file);
	fread(&mRadius, sizeof (double), 1, file);
	fread(&mColor.r, sizeof (int), 1, file);
	fread(&mColor.g, sizeof (int), 1, file);
	fread(&mColor.b, sizeof (int), 1, file);

	set(mWorldPosition, mRadius, mColor);
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * WorldLight
// *
// * this describes in-world lights
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef WorldLight_h_
#define WorldLight_h_

#include "v3d.h"

class IntColor {
public:

	void constrain(int minValue, int maxValue) {
		if (r > maxValue) {
			r = maxValue;
		}
		else if (r < minValue) {
			r = minValue;
		}

		if (g > maxValue) {
			g = maxValue;
		}
		else if (g < minValue) {
			g = minValue;
		}

		if (b > maxValue) {
			b = maxValue;
		}
		else if (b < minValue) {
			b = minValue;
		}
	}

	int r;
	int g;
	int b;
};


class WorldLight {
public:
	WorldLight();
	~WorldLight();

	void set(v3d_t position, double radius, IntColor color);
	void initBuffer(void);

	IntColor getLevel(v3di_t worldPosition) const;
	bool isInVolume(v3di_t worldPosition) const;

	// delete the buffer or create a new one
	void turnOff(void);
	void turnOn(void);

	void save(FILE *file);
	void load(FILE *file);

//private:
	size_t mHandle;

	// this stuff must be saved!
	v3d_t mWorldPosition;
	double mRadius;
	IntColor mColor;

	// this stuff gets generated, i.e. NOT SAVED
	v3di_t mBufferNear;
	v3di_t mBufferFar;
	v3di_t mBufferDimensions;
	IntColor *mBuffer;
};


#endif // WorldLight_h_

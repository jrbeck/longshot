// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * LoadScreen
// *
// * desc: utility to do a basic load/progress screen
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef LoadScreen_h_
#define LoadScreen_h_

#include "v2d.h"
#include "SdlInterface.h"

class LoadScreen {
public:
	LoadScreen();

	void start();
	void finish();

	void draw(int numerator, int denominator);

	void setProgressBarRectangle(v2d_t topLeft, v2d_t bottomRight);

	void setBackgroundColor(float r, float g, float b);
	void setCompletedColor(float r, float g, float b);
	void setIncompletedColor(float r, float g, float b);
	void swapColors();

private:
	v2d_t progressBarTopLeft;
	v2d_t progressBarBottomRight;

	rgb_float_t backgroundColor;
	rgb_float_t completedColor;
	rgb_float_t incompletedColor;
};


#endif // LoadScreen_h_

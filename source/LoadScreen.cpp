#include "LoadScreen.h"


LoadScreen::LoadScreen() {
	progressBarTopLeft = v2d_v(0.3, 0.45);
	progressBarBottomRight = v2d_v(0.7, 0.55);

	backgroundColor.r = 0.0f;
	backgroundColor.g = 0.0f;
	backgroundColor.b = 0.0f;

	completedColor.r = 0.8f;
	completedColor.g = 0.8f;
	completedColor.b = 0.8f;

	incompletedColor.r = 0.3f;
	incompletedColor.g = 0.3f;
	incompletedColor.b = 0.3f;
}

void LoadScreen::start() {
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

//	glOrtho (0, SCREEN_W, SCREEN_H, 0, -1, 1);
	glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0);
	glDisable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();
}

void LoadScreen::finish() {
	glPopMatrix();
}

void LoadScreen::draw(int numerator, int denominator) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawProgressBar(progressBarTopLeft, progressBarBottomRight, numerator, denominator, completedColor, incompletedColor);
		SDL_GL_SwapBuffers();
}

void LoadScreen::setProgressBarRectangle(v2d_t topLeft, v2d_t bottomRight) {
	progressBarTopLeft.x = topLeft.x;
	progressBarTopLeft.y = topLeft.y;

	progressBarBottomRight.x = bottomRight.x;
	progressBarBottomRight.y = bottomRight.y;
}

void LoadScreen::setBackgroundColor(float r, float g, float b) {
	backgroundColor.r = r;
	backgroundColor.g = g;
	backgroundColor.b = b;
}

void LoadScreen::setCompletedColor(float r, float g, float b) {
	completedColor.r = r;
	completedColor.g = g;
	completedColor.b = b;
}

void LoadScreen::setIncompletedColor(float r, float g, float b) {
	incompletedColor.r = r;
	incompletedColor.g = g;
	incompletedColor.b = b;
}

void LoadScreen::swapColors() {
	rgb_float_t temp;
		
	temp.r = completedColor.r;
	temp.g = completedColor.g;
	temp.b = completedColor.b;

	completedColor.r = incompletedColor.r;
	completedColor.g = incompletedColor.g;
	completedColor.b = incompletedColor.b;

	incompletedColor.r = temp.r;
	incompletedColor.g = temp.g;
	incompletedColor.b = temp.b;
}

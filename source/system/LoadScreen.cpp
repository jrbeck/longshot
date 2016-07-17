#include "LoadScreen.h"


float loadScreenColorLookup[NUM_LOAD_SCREEN_COLORS][3] = {
  { 0.0f, 0.0f, 0.0f }, // LOAD_SCREEN_BLACK
  { 1.0f, 1.0f, 1.0f }, // LOAD_SCREEN_WHITE
  { 0.0f, 0.8f, 0.0f },
  { 0.0f, 0.4f, 0.0f },
  { 0.6f, 0.0f, 0.8f },
  { 0.3f, 0.0f, 0.4f },
};




LoadScreen::LoadScreen(GameWindow* gameWindow) {
  mGameWindow = gameWindow;

  progressBarTopLeft = v2d_v(0.3, 0.45);
  progressBarBottomRight = v2d_v(0.7, 0.55);

  mBackgroundColor.r = 0.0f;
  mBackgroundColor.g = 0.0f;
  mBackgroundColor.b = 0.0f;

  mCompletedColor.r = 0.8f;
  mCompletedColor.g = 0.8f;
  mCompletedColor.b = 0.8f;

  mIncompletedColor.r = 0.3f;
  mIncompletedColor.g = 0.3f;
  mIncompletedColor.b = 0.3f;
}

void LoadScreen::show() {
  glPushMatrix();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_TEXTURE_2D);

  glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mGameWindow->swapBuffers();
}

void LoadScreen::hide() {
  glPopMatrix();
}

void LoadScreen:: drawProgressBar(v2d_t lower, v2d_t higher, int numerator, int denominator) {
  double percent = static_cast<double>(numerator) / static_cast<double>(denominator);
  drawProgressBar(lower, higher, percent);
}

void LoadScreen::drawProgressBar(v2d_t lower, v2d_t higher, double percent) {
  double midx; // middle top and bottom
  midx = lower.x + ((higher.x - lower.x) * percent);

  glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_QUADS);
    glColor3f(mCompletedColor.r, mCompletedColor.g, mCompletedColor.b);
    glVertex2d(lower.x, lower.y);
    glVertex2d(lower.x, higher.y);
    glVertex2d(midx, higher.y);
    glVertex2d(midx, lower.y);

    glColor3f(mIncompletedColor.r, mIncompletedColor.g, mIncompletedColor.b);
    glVertex2d(midx, lower.y);
    glVertex2d(midx, higher.y);
    glVertex2d(higher.x, higher.y);
    glVertex2d(higher.x, lower.y);
  glEnd();

  mGameWindow->swapBuffers();
}


void LoadScreen::draw(int numerator, int denominator) {
  drawProgressBar(progressBarTopLeft, progressBarBottomRight, numerator, denominator);
}

void LoadScreen::setProgressBarRectangle(v2d_t topLeft, v2d_t bottomRight) {
  progressBarTopLeft.x = topLeft.x;
  progressBarTopLeft.y = topLeft.y;

  progressBarBottomRight.x = bottomRight.x;
  progressBarBottomRight.y = bottomRight.y;
}

void LoadScreen::setBackgroundColor(size_t color) {
  color = color % NUM_LOAD_SCREEN_COLORS;
  setBackgroundColor(loadScreenColorLookup[color][0], loadScreenColorLookup[color][1], loadScreenColorLookup[color][2]);
}

void LoadScreen::setBackgroundColor(float r, float g, float b) {
  mBackgroundColor.r = r;
  mBackgroundColor.g = g;
  mBackgroundColor.b = b;
  glClearColor(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0);
}

void LoadScreen::setCompletedColor(size_t color) {
  color = color % NUM_LOAD_SCREEN_COLORS;
  mCompletedColor.r = loadScreenColorLookup[color][0];
  mCompletedColor.g = loadScreenColorLookup[color][1];
  mCompletedColor.b = loadScreenColorLookup[color][2];
}

void LoadScreen::setCompletedColor(float r, float g, float b) {
  mCompletedColor.r = r;
  mCompletedColor.g = g;
  mCompletedColor.b = b;
}

void LoadScreen::setIncompletedColor(size_t color) {
  color = color % NUM_LOAD_SCREEN_COLORS;
  mIncompletedColor.r = loadScreenColorLookup[color][0];
  mIncompletedColor.g = loadScreenColorLookup[color][1];
  mIncompletedColor.b = loadScreenColorLookup[color][2];
}

void LoadScreen::setIncompletedColor(float r, float g, float b) {
  mIncompletedColor.r = r;
  mIncompletedColor.g = g;
  mIncompletedColor.b = b;
}

void LoadScreen::swapColors() {
  rgb_float_t temp;
    
  temp.r = mCompletedColor.r;
  temp.g = mCompletedColor.g;
  temp.b = mCompletedColor.b;

  mCompletedColor.r = mIncompletedColor.r;
  mCompletedColor.g = mIncompletedColor.g;
  mCompletedColor.b = mIncompletedColor.b;

  mIncompletedColor.r = temp.r;
  mIncompletedColor.g = temp.g;
  mIncompletedColor.b = temp.b;
}

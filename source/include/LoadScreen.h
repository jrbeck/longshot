// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * LoadScreen
// *
// * desc: utility to do a basic load/progress screen
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "GameWindow.h"
#include "v2d.h"


enum {
  LOAD_SCREEN_BLACK,
  LOAD_SCREEN_WHITE,
  LOAD_SCREEN_LIGHT_GREEN,
  LOAD_SCREEN_DARK_GREEN,
  LOAD_SCREEN_LIGHT_PURPLE,
  LOAD_SCREEN_DARK_PURPLE,
  NUM_LOAD_SCREEN_COLORS
};

extern float loadScreenColorLookup[NUM_LOAD_SCREEN_COLORS][3];

class LoadScreen {
public:
  LoadScreen(GameWindow* gameWindow);

  void show();
  void hide();

  void drawProgressBar(v2d_t lower, v2d_t higher, int numerator, int denominator);
  void drawProgressBar(v2d_t lower, v2d_t higher, double percent);

  void draw(int numerator, int denominator);

  void setProgressBarRectangle(v2d_t topLeft, v2d_t bottomRight);

  void setBackgroundColor(size_t color);
  void setBackgroundColor(float r, float g, float b);

  void setCompletedColor(size_t color);
  void setCompletedColor(float r, float g, float b);

  void setIncompletedColor(size_t color);
  void setIncompletedColor(float r, float g, float b);

  void swapColors();

private:
  GameWindow* mGameWindow;

  v2d_t progressBarTopLeft;
  v2d_t progressBarBottomRight;

  rgb_float_t mBackgroundColor;
  rgb_float_t mCompletedColor;
  rgb_float_t mIncompletedColor;
};

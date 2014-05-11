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

  void start();
  void finish();

  void drawProgressBar(
    v2d_t lower,
    v2d_t higher,
    int numerator,
    int denominator,
    rgb_float_t c1,
    rgb_float_t c2);
  void drawProgressBar(v2d_t lower, v2d_t higher, double percent, rgb_float_t c1, rgb_float_t c2);

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

  rgb_float_t backgroundColor;
  rgb_float_t completedColor;
  rgb_float_t incompletedColor;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GameWindow
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include <SDL/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "Constants.h"

#define MAX_WINDOW_TITLE_LENGTH    (128)

struct sdl_mode_info_t {
  int initialized;
  int screen_w;
  int screen_h;
  bool fullscreen;
};


struct rgb_float_t {
  GLfloat r;
  GLfloat g;
  GLfloat b;
};


class GameWindow {
public:
  GameWindow(const char* windowTitle);
  ~GameWindow();

  void setIcon(const char *path);

  int initSdl();
  void quitSdl();

  int setVideoMode(sdl_mode_info_t mode);
  void toggleFullscreen();
  void swapBuffers();

private:
  char mWindowTitle[MAX_WINDOW_TITLE_LENGTH];

  sdl_mode_info_t mCurrentMode;
  sdl_mode_info_t mDesktopMode;
  sdl_mode_info_t mWindowedMode;
  SDL_Window* mSdlWindow;
  SDL_GLContext mSdlGlcontext;
};

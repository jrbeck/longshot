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

#include "Constants.h"
#include "SdlInterface.h"


class GameWindow {
public:
  GameWindow();
  ~GameWindow();

  int setVideoMode(sdl_mode_info_t mode);
  void toggleFullscreen();
  void swapBuffers();

private:
  sdl_mode_info_t mCurrentMode;
  sdl_mode_info_t mDesktopMode;
  sdl_mode_info_t mWindowedMode;
  SDL_Window* mSdlWindow;
  SDL_GLContext mSdlGlcontext;
};

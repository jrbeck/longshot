#include "GameWindow.h"

GameWindow::GameWindow(const char* windowTitle) :
  mSdlWindow(NULL),
  mSdlGlcontext(NULL)
{
  printf("GameWindow constructor....\n");

  strncpy(mWindowTitle, windowTitle, MAX_WINDOW_TITLE_LENGTH);
  mWindowTitle[MAX_WINDOW_TITLE_LENGTH - 1] = '\0';

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Unable to initialize SDL: %s\n", SDL_GetError());
    assert(false);
  }
  // int imgFlags = IMG_INIT_PNG;
  // if (!(IMG_Init(imgFlags) & imgFlags)) {
  //   printf("SDL_image: IMG_Init error: %s\n", IMG_GetError());
  // }
  SDL_DisplayMode current;
  // TODO: this defaults to display 0 ... there could be more than just the one
  int displayIndex = 0;
  int result = SDL_GetCurrentDisplayMode(displayIndex, &current);
  if (result != 0) {
    printf("Could not get display mode for video display #%d: %s", displayIndex, SDL_GetError());
    assert(false);
  }
  else {
    printf("Display #%d: current display mode is %dx%dpx @ %dhz. \n", displayIndex, current.w, current.h, current.refresh_rate);
    mDesktopMode.screen_w = current.w;
    mDesktopMode.screen_h = current.h;
    mDesktopMode.fullscreen = true;
  }

  // mWindowedMode.screen_w = 320;
  // mWindowedMode.screen_h = 240;

  mWindowedMode.screen_w = (int)((double)mDesktopMode.screen_w * 0.9);
  mWindowedMode.screen_h = (int)((double)mDesktopMode.screen_h * 0.9);

  mWindowedMode.fullscreen = false;

  if (setVideoMode(mWindowedMode)) {
    //constructor_successful = false;
    // leap of faith?
    // meh...
    assert(false);
  }
}


GameWindow::~GameWindow() {
  if (mSdlWindow != NULL) {
    SDL_DestroyWindow(mSdlWindow);
  }
  // IMG_Quit();
  SDL_Quit();
}

void GameWindow::setIcon(const char* path) {
  SDL_SetWindowIcon(mSdlWindow, IMG_Load(path));
}

int GameWindow::setVideoMode(sdl_mode_info_t mode) {
  if (mSdlWindow != NULL) {
    SDL_DestroyWindow(mSdlWindow);
    mSdlWindow = NULL;
  }

  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  if (mode.fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  }

  mSdlWindow = SDL_CreateWindow(
    mWindowTitle,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    mode.screen_w, mode.screen_h,
    flags);

  if (mSdlWindow == NULL) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    return 1;
  }

  mSdlGlcontext = SDL_GL_CreateContext(mSdlWindow);

  if (mSdlGlcontext == NULL) {
    printf("Could not get GlContext.\n");
    return 1;
  }

  mCurrentMode = mode;
  // ugh ... is this really the way to do this?
  SCREEN_W = mCurrentMode.screen_w;
  SCREEN_H = mCurrentMode.screen_h;

  glViewport(0, 0, mCurrentMode.screen_w, mCurrentMode.screen_h);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  printf("display resolution: %d, %d\n", mCurrentMode.screen_w, mCurrentMode.screen_h);

  return 0;
}

void GameWindow::toggleFullscreen() {
  if (mCurrentMode.fullscreen) {
    setVideoMode(mWindowedMode);
  }
  else {
    setVideoMode(mDesktopMode);
  }
}

void GameWindow::swapBuffers() {
  if (mSdlWindow == NULL) {
    return;
  }
  SDL_GL_SwapWindow(mSdlWindow);
}

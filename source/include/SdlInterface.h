// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Sdl Interface
// *
// * SDL utility library
// * used to create window, set GL state, etc...
// *
// * needs major work - should be eliminated in favor of a more abstract interface to
// * openGL/directX/Win32
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

//#include "GL/glaux.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include <SDL/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "MathUtil.h"
#include "v3d.h"
#include "v2d.h"



// TYPEDEFS * * * * * * * * * * * * * * * * * * * * * * * *
struct rgb_float_t {
	GLfloat r;
	GLfloat g;
	GLfloat b;
};


struct sdl_mode_info_t {
	int initialized;
	int screen_w;
	int screen_h;
	bool fullscreen;

};


int initSdl(void);
int deinitSdl(void);
int setVideoMode(sdl_mode_info_t mode, SDL_Window *window);
void drawProgressBar(v2d_t lower, v2d_t higher, int numerator, int denominator, rgb_float_t c1, rgb_float_t c2);
void drawProgressBar(v2d_t lower, v2d_t higher, double percent, rgb_float_t c1, rgb_float_t c2);
int loadTexture(const char* filename, GLuint* texture_handle);

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

int loadTexture(const char* filename, GLuint* texture_handle);

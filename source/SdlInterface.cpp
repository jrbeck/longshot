#include "SdlInterface.h"

sdl_mode_info_t desktopMode;
sdl_mode_info_t currentMode;

int initSdl(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Unable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  return 0;
}

int deinitSdl(void) {
  SDL_Quit();
  return 0;
}

int loadTexture(const char* filename, GLuint* texture_handle) {
  SDL_Surface* surface = IMG_Load(filename);

  if (surface == NULL) {
  //        printf ("SDL could not load image.bmp: %s\n", SDL_GetError());
  //        SDL_Quit ();
    return -1;
  }

  // Check that the image's width is a power of 2
  if ((surface->w & (surface->w - 1)) != 0) {
    printf("warning: %s's width is not a power of 2\n", filename);
  }

  // Also check if the height is a power of 2
  if ((surface->h & (surface->h - 1)) != 0) {
    printf("warning: %s's height is not a power of 2\n", filename);
  }

  // get the number of channels in the SDL surface
  GLint numColors = surface->format->BytesPerPixel;
  GLenum textureFormat;
  if (numColors == 4) { // contains an alpha channel
    if (surface->format->Rmask == 0x000000ff) {
      textureFormat = GL_RGBA;
    }
    else {
      textureFormat = GL_BGRA;
    }
  }
  else if (numColors == 3) { // no alpha channel
    if (surface->format->Rmask == 0x000000ff) {
      textureFormat = GL_RGB;
    }
    else {
      textureFormat = GL_BGR;
    }
  }
  else {
    printf("warning: the image is not truecolor..  this will probably break\n");
    // this error should not go unhandled
  }


//		surfaceAlpha = SDL_DisplayFormat(surface);
//		SDL_SetAlpha(surfaceAlpha, 0, SDL_ALPHA_TRANSPARENT);

/*		tempSurfaceFixed = SDL_CreateRGBSurface(
    SDL_SWSURFACE,
    tempSurface->w, tempSurface->h,
    32, R_Mask, G_Mask, B_Mask, A_Mask);
*/

  // Have OpenGL generate a texture object handle for us
  glGenTextures(1, texture_handle);

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, *texture_handle);

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//		surfaceAlpha = SDL_DisplayFormatAlpha(surface);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    numColors,
    surface->w, surface->h,
    0,
    textureFormat,
    GL_UNSIGNED_BYTE,
    surface->pixels);

  SDL_FreeSurface(surface);

  return 0;
}


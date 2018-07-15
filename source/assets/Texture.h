// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Texture
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

class Texture {
public:
  Texture(std::string fileName);
  ~Texture();

  int loadTexture();

  void bind();

  std::string mFileName;
  GLuint mWidth;
  GLuint mHeight;
  GLuint mGlHandle;
};

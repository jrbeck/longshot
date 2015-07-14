#include "Texture.h"

Texture::Texture(std::string fileName) {
  mFileName = fileName;

  if (loadTexture() != 0) {
    mGlHandle = 0;
  }
}


Texture::~Texture() {
  printf("Texture::~Texture()\n");
  if (mGlHandle != 0) {
    glDeleteTextures(1, &mGlHandle);
  }
}


int Texture::loadTexture() {
  SDL_Surface* surface = IMG_Load(mFileName.c_str());

  if (!surface) {
    printf ("Texture::loadTexture(): failed to load %s: %s\n", mFileName.c_str(), SDL_GetError());
    return -1;
  }

  // make sure the image's width is a power of 2
  if ((surface->w & (surface->w - 1)) != 0) {
    printf("Texture::loadTexture(): warning: %s's width is not a power of 2\n", mFileName.c_str());
  }
  // make sure the height is a power of 2
  if ((surface->h & (surface->h - 1)) != 0) {
    printf("Texture::loadTexture(): warning: %s's height is not a power of 2\n", mFileName.c_str());
  }

  mWidth = surface->w;
  mHeight = surface->h;

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
    printf("Texture::loadTexture(): %s is not in a recognized format, not loading\n", mFileName.c_str());
    mGlHandle = 0;
    SDL_FreeSurface(surface);
    return 1;
  }

  glGenTextures(1, &mGlHandle);
  glBindTexture(GL_TEXTURE_2D, mGlHandle);

  // set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Edit the texture object's image data using the information SDL_Surface gives us
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

  // return success
  return 0;
}


void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, mGlHandle);
}


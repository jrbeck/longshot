// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BitmapModel
// *
// * desc: create a 3d model from a 2d bitmap
// *
// * TODO: allow the use of a height field for depth (Z), could use alpha or a discrete
// * bitmap (region). Would change the visibility requirements
// *
// * The original algorithm draw each voxel front and back individually, but that has been
// * replaced with a simple single-quad front and back with the whole bitmap region drawn on it.
// * The original code to draw the indiviudal voxel front and back still exists in case some day
// * I actually want to implement the depth thing outlined above.
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <SDL2/SDL_opengl.h>

#include "../game/Constants.h"
#include "../assets/TextureRegion.h"

struct Pixel32 {
  BYTE a;
  BYTE b;
  BYTE g;
  BYTE r;
};

#define VOXEL_SCALE_X    (0.01f)
#define VOXEL_SCALE_Y    (-0.01f)
#define VOXEL_SCALE_Z    (0.02f)

class BitmapModel {
public:
  BitmapModel(TextureRegion* region);
  ~BitmapModel();

  void bindTexture();

  void draw();

  int mWidth;
  int mHeight;

private:
  BYTE getFaceVisibility(GLuint s, GLuint t, Pixel32* pixelData, TextureRegion* region) const;
  void drawVoxel(
    BYTE faceVisibility,
    GLuint s, GLuint t,
    GLfloat u, GLfloat v,
    GLfloat pixelW, GLfloat pixelH) const;

  void drawFront(
    GLfloat s1, GLfloat t1,
    GLfloat s2, GLfloat t2,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBack(
    GLfloat s1, GLfloat t1,
    GLfloat s2, GLfloat t2,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceFront(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceBack(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceLeft(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceRight(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceTop(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

  void drawBlockFaceBottom(
    GLfloat s, GLfloat t,
    GLfloat u1, GLfloat v1,
    GLfloat u2, GLfloat v2) const;

// MEMBERS * * * * * * * * * * * * *
  Texture* mTexture;
  GLuint mDisplayListHandle;
};

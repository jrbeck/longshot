// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * TextureRegion
// *
// * desc: holds region info for a sprite/tile/etc...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef TextureRegion_h_
#define TextureRegion_h_

#include "../assets/Texture.h"

class TextureRegion {
public:
  TextureRegion();
  TextureRegion(GLuint x, GLuint y, GLuint width, GLuint height, Texture *texture);
  ~TextureRegion();

  void setupRegion(GLuint x, GLuint y, GLuint width, GLuint height, Texture *texture);

  // return as: [u1, v1, u2, v2]
  void getFlippedCoords(bool flipX, bool flipY, GLfloat coords[4]);

// MEMBERS * * * * * * * * * * * *
  Texture *mTexture;

  // these are in the original image coords
  GLuint mX, mY;
  GLuint mW, mH;

  // these are in normalized coords
  GLfloat u1, v1;
  GLfloat u2, v2;
};


#endif // TextureRegion_h_

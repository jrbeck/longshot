#include "SpriteBatcher.h"


SpriteBatcher::SpriteBatcher(size_t maxSprites) {
  mSprites = new Sprite[maxSprites];
  mMaxSprites = maxSprites;
  mNumSprites = 0;
}


SpriteBatcher::~SpriteBatcher() {
  printf("SpriteBatcher::~SpriteBatcher()\n");
  delete [] mSprites;
}


void SpriteBatcher::beginBatch(Texture *texture) {
  texture->bind();
  mNumSprites = 0;
}


void SpriteBatcher::endBatch() {
  if (mNumSprites == 0) {
    return;
  }

  GLfloat u1, v1;
  GLfloat u2, v2;

  glBegin (GL_QUADS);

  for (size_t i = 0; i < mNumSprites; i++) {
    Sprite &sprite = mSprites[i];

    u1 = sprite.mTextureRegion->u1;
    v1 = sprite.mTextureRegion->v1;
    u2 = sprite.mTextureRegion->u2;
    v2 = sprite.mTextureRegion->v2;

    glTexCoord2f(u1, v2); glVertex2fv(sprite.mVert1);
    glTexCoord2f(u2, v2); glVertex2fv(sprite.mVert2);
    glTexCoord2f(u2, v1); glVertex2fv(sprite.mVert3);
    glTexCoord2f(u1, v1); glVertex2fv(sprite.mVert4);
  }

  glEnd ();
}


void SpriteBatcher::drawSprite(
  GLfloat x,
  GLfloat y,
  GLfloat width,
  GLfloat height,
  TextureRegion *textureRegion)
{
  if (mNumSprites == mMaxSprites) {
    // can't add any more Sprites
    return;
  }

  GLfloat halfWidth = width * 0.5f;
  GLfloat halfHeight = height * 0.5f;

  mSprites[mNumSprites].mVert1[0] = x - halfWidth;
  mSprites[mNumSprites].mVert1[1] = y + halfHeight;

  mSprites[mNumSprites].mVert2[0] = x + halfWidth;
  mSprites[mNumSprites].mVert2[1] = y + halfHeight;

  mSprites[mNumSprites].mVert3[0] = x + halfWidth;
  mSprites[mNumSprites].mVert3[1] = y - halfHeight;

  mSprites[mNumSprites].mVert4[0] = x - halfWidth;
  mSprites[mNumSprites].mVert4[1] = y - halfHeight;

  mSprites[mNumSprites].mTextureRegion = textureRegion;

  mNumSprites++;
}


void SpriteBatcher::drawSprite(
  GLfloat x,
  GLfloat y,
  GLfloat width,
  GLfloat height,
  GLfloat angle,
  TextureRegion *textureRegion)
{
  if (mNumSprites == mMaxSprites) {
    // can't add any more Sprites
    return;
  }

  GLfloat halfWidth = width * 0.5f;
  GLfloat halfHeight = height * 0.5f;

  angle *= 0.017453292f; // DEG_2_RAD
  float aCos = cos(angle);
  float aSin = sin(angle);

  mSprites[mNumSprites].mVert1[0] = x + ((-halfWidth * aCos) - (-halfHeight * aSin));
  mSprites[mNumSprites].mVert1[1] = y + ((-halfWidth * aSin) + (-halfHeight * aCos));

  mSprites[mNumSprites].mVert2[0] = x + ((halfWidth * aCos) - (-halfHeight * aSin));
  mSprites[mNumSprites].mVert2[1] = y + ((halfWidth * aSin) + (-halfHeight * aCos));

  mSprites[mNumSprites].mVert3[0] = x + ((halfWidth * aCos) - (halfHeight * aSin));
  mSprites[mNumSprites].mVert3[1] = y + ((halfWidth * aSin) + (halfHeight * aCos));

  mSprites[mNumSprites].mVert4[0] = x + ((-halfWidth * aCos) - (halfHeight * aSin));
  mSprites[mNumSprites].mVert4[1] = y + ((-halfWidth * aSin) + (halfHeight * aCos));

  mSprites[mNumSprites].mTextureRegion = textureRegion;

  mNumSprites++;
}





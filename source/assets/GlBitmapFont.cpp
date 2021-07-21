#include "../assets/GlBitmapFont.h"


GlBitmapFont::GlBitmapFont() :
  mCharWidth(0),
  mCharHeight(0),
  mBitmapSideLength(0),
  mScreenW(100),
  mScreenH(100),
  mCharsPerLine(0),
  mTextureHandle(0)
{
  // FIXME: aww come on c++...
  mTextureMultiplier[0] = 0.0f;
  mTextureMultiplier[1] = 0.0f;
}



GlBitmapFont::~GlBitmapFont() {
}



int GlBitmapFont::setupFont(int glHandle, int bitmapSideLength, int charWidth, int charHeight) {
  mTextureHandle = glHandle;

  mBitmapSideLength = bitmapSideLength;

  mCharWidth = charWidth;
  mCharHeight = charHeight;

  mCharsPerLine = mBitmapSideLength / mCharWidth;

  mTextureMultiplier[0] = (GLfloat)mCharWidth / (GLfloat)mBitmapSideLength;
  mTextureMultiplier[1] = (GLfloat)mCharHeight / (GLfloat)mBitmapSideLength;

  return 0;
}



GLfloat GlBitmapFont::getStringWidth(const string &s) const {
  GLfloat width = 0.0f;
  BYTE c;

  for (size_t i = 0; i < s.length(); ++i) {

    c = s[i];

/*    if ((c >= 'a' && c <= 'z') ||
      (c >= '0' && c <= '9') ||
      c == '-' ||
      c == '?')
    {
      width += 1.0;
    }
*/
    if (c == ' ' ||
      c == 'i' ||
      c == 'I' ||
      c == '.' ||
      c == ':' ||
      c == '!' ||
      c == '(' ||
      c == ')' ||
      c == '\'' ||
      c == ',')
    {
      width += 0.5;
    }
    else {
      width += 1.0;
    }
  }

  return width;
}



void GlBitmapFont::drawString(
  const GLfloat topLeft[2],
  const GLfloat charDimensions[2],
  const string &s,
  const GLfloat color[4]) const
{
  if (s.size() <= 0) {
    return;
  }

  GLfloat tl[2];
  GLfloat br[2];

  tl[0] = topLeft[0];
  br[1] = topLeft[1];

  int character;

//  enterGlMode();

  for (size_t i = 0; i < s.size(); ++i) {
    character = s[i];

    br[0] = tl[0] + charDimensions[0];
    tl[1] = br[1] + charDimensions[1];

    if (character == ' ') {
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == 'i') {
      drawLetter(tl, br, character - 'a', color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == 'I') {
      drawLetter(tl, br, character - 'A', color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '.') {
      drawLetter(tl, br, FONT_INDEX_PERIOD, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '-') {
      drawLetter(tl, br, FONT_INDEX_DASH, color);
    }
    else if (character == ':') {
      drawLetter(tl, br, FONT_INDEX_COLON, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '!') {
      drawLetter(tl, br, FONT_INDEX_BANG, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '?') {
      drawLetter(tl, br, FONT_INDEX_QUESTION, color);
    }
    else if (character == '(') {
      drawLetter(tl, br, FONT_INDEX_PAREN_OPEN, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == ')') {
      drawLetter(tl, br, FONT_INDEX_PAREN_CLOSE, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '\'') {
      drawLetter(tl, br, FONT_INDEX_APOSTROPHE, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == ',') {
      drawLetter(tl, br, FONT_INDEX_COMMA, color);
      tl[0] += charDimensions[0] * -0.5f;
    }
    else if (character == '/') {
      drawLetter(tl, br, FONT_INDEX_FORWARD_SLASH, color);
    }
    else if (character == '%') {
      drawLetter(tl, br, FONT_INDEX_PERCENT, color);
    }
    else if (character >= 'a' &&
      character <= 'z' )
    {
      drawLetter(tl, br, character - 'a', color);
    }
    else if (character >= 'A' &&
      character <= 'Z' )
    {
      drawLetter(tl, br, character - 'A', color);
    }
    else if (character >= '0' &&
      character <= '9')
    {
      drawLetter(tl, br, (character - '0') + FONT_INDEX_ZERO, color);
    }
    else {
      drawLetter(tl, br, FONT_INDEX_INVALID, color);
    }

    tl[0] += charDimensions[0];
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//  exitGlMode();
}



void GlBitmapFont::drawLetter(
  const GLfloat topLeft[2],
  const GLfloat bottomRight[2],
  const int charIndex,
  const GLfloat *color) const
{
  if (mTextureHandle == 0) {
    // no font loaded...
    return;
  }

  GLfloat texCoords[2] = {
    (GLfloat)(charIndex % mCharsPerLine) * mTextureMultiplier[0], // i
    (GLfloat)(charIndex / mCharsPerLine) * mTextureMultiplier[1]  // j
  };

  // draw the letter
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, mTextureHandle);
  glColor4fv(color);

  glBegin(GL_QUADS);
    // bottom left
    glTexCoord2f(texCoords[0], texCoords[1]);
    glVertex2f(topLeft[0], bottomRight[1]);
    // top left
    glTexCoord2f(texCoords[0], texCoords[1] + mTextureMultiplier[1]);
    glVertex2f(topLeft[0],  topLeft[1]);
    // top right
    glTexCoord2f(texCoords[0] + mTextureMultiplier[0], texCoords[1] + mTextureMultiplier[1]);
    glVertex2f(bottomRight[0],  topLeft[1]);
    // bottom right
    glTexCoord2f(texCoords[0] + mTextureMultiplier[0], texCoords[1]);
    glVertex2f(bottomRight[0], bottomRight[1]);
  glEnd();
}



void GlBitmapFont::enterGlMode() const {
  // set up the opengl viewport
  glPushMatrix();

  glViewport(0, 0, mScreenW, mScreenH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}



void GlBitmapFont::exitGlMode() const {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glPopMatrix();
}

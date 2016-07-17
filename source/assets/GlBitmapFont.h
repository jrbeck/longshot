// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GlBitmapFont
// *
// * loads a square, power-of-2-sided bitmap which contains letter information in
// * the (alphabet, numeral) order starting from 'a' in the top-left corner and proceeding
// * horizontally to wrap when the edge is met
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <string>

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include <SDL2/SDL_opengl.h>

#include "../vendor/GL/glut.h"

using namespace std;



enum {
  FONT_INDEX_A,
  FONT_INDEX_ZERO = 26,
  FONT_INDEX_PERIOD = 36,
  FONT_INDEX_DASH,
  FONT_INDEX_COLON,
  FONT_INDEX_BANG,
  FONT_INDEX_QUESTION,
  FONT_INDEX_PAREN_OPEN,
  FONT_INDEX_PAREN_CLOSE,
  FONT_INDEX_APOSTROPHE,
  FONT_INDEX_COMMA,
  FONT_INDEX_FORWARD_SLASH,
  FONT_INDEX_PERCENT,
  FONT_INDEX_INVALID = 63
};



class GlBitmapFont {
public:
  GlBitmapFont ();
  ~GlBitmapFont ();

  int setupFont(int glHandle, int bitmapSideLength, int charWidth, int charHeight);

  GLfloat getStringWidth (const string &s) const;
  void drawString (const GLfloat topLeft[2], const GLfloat charDimensions[2], const string &s, const GLfloat color[4]) const;

private:

  void drawLetter (const GLfloat topLeft[2], const GLfloat bottomRight[2], const int charIndex, const GLfloat *color) const;

  void enterGlMode (void) const;
  void exitGlMode (void) const;

  // members the user must set
  int mCharWidth;            // the width of an individual char cell in the image
  int mCharHeight;          // the height of an individual char cell in the image
  int mBitmapSideLength;        // the side (both) length for the bitmap

  int mScreenW;            // the width of the screen
  int mScreenH;            // the height of the screen

  // derived members
  int mCharsPerLine;          // the number of characters that fit in one line

  GLuint mTextureHandle;        // the openGL handle for the font texture

  GLfloat mTextureMultiplier[2];    // these get the coords fo the individual chars
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GameMenu
// *
// * this handles buttons and fonts and ....
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <string>
#include <vector>

#include <Windows.h>

#include "GL/glut.h"

#include "AssetManager.h"
#include "GlBitmapFont.h"

#define MENU_APPLICATION_QUIT (-2)


enum {
  MENUITEM_BUTTON,
  MENUITEM_TEXT,
  NUM_MENUITEMS
};



enum {
  TEXT_JUSTIFICATION_LEFT,
  TEXT_JUSTIFICATION_CENTER,
  TEXT_JUSTIFICATION_RIGHT
};



struct menu_item_t {
  int type;

  GLfloat position[2];
  GLfloat dimensions[2];

  GLfloat fontSize[2];

  string text;
  int textJustification;

  int value;

  GLfloat color[4];
  GLfloat backgroundColor[4];
};



class GameMenu {
public:
  // this is for the default font...if this is not set properly,
  // clients are required to use setFont()
  static void setDefaultTextureHandle(GLuint handle);
  static GLuint defaultTextureHandle;

  GameMenu();
  ~GameMenu();

  void clear(void);

  void setFont1(int glHandle);

  size_t addButton(v2d_t position, v2d_t dimensions, v2d_t fontSize, const char* button_text,
    int textJustification, int value, const GLfloat* color, const GLfloat* backgroundColor);

  size_t addText(v2d_t position, v2d_t dimensions, v2d_t fontSize, const char* text,
    int textJustification, const GLfloat* color, const GLfloat *backgroundColor);

  int GameMenuhoice(bool waitForInput);

  int mouse_up(double x, double y);

  void draw(void);

private:

  vector <menu_item_t> mMenuItems;

  GlBitmapFont mGlBitmapFont;
};

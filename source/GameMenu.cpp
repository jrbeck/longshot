#include "GameMenu.h"

GLuint GameMenu::defaultTextureHandle = 0;


void GameMenu::setDefaultTextureHandle(GLuint handle) {
  defaultTextureHandle = handle;
}


GameMenu::GameMenu(void) {
  mGlBitmapFont.setupFont(defaultTextureHandle, 128, 16, 16);
}


GameMenu::~GameMenu(void) {
  clear();
}


void GameMenu::clear(void) {
  mMenuItems.clear ();
}


void GameMenu::setFont1(int glHandle) {
  mGlBitmapFont.setupFont(glHandle, 128, 16, 16);
}


size_t GameMenu::addButton(
  v2d_t position,
  v2d_t dimensions,
  v2d_t fontSize,
  const char *buttonText,
  int textJustification,
  int value,
  const GLfloat *color,
  const GLfloat *backgroundColor)
{
  menu_item_t item;

  item.type = MENUITEM_BUTTON;
  item.position[0] = (GLfloat)position.x;
  item.position[1] = (GLfloat)position.y;
  item.dimensions[0] = (GLfloat)dimensions.x;
  item.dimensions[1] = (GLfloat)dimensions.y;
  item.fontSize[0] = (GLfloat)fontSize.x;
  item.fontSize[1] = (GLfloat)fontSize.y;
  item.textJustification = textJustification;

  item.value = value;

  if (color != NULL) {
    for (int i = 0; i < 4; i++) {
      item.color[i] = color[i];
    }
  }
  else {
    // default color
    for (int i = 0; i < 4; i++) {
      item.color[i] = 1.0f;
    }
  }


  if (backgroundColor != NULL) {
    for (int i = 0; i < 4; i++) {
      item.backgroundColor[i] = backgroundColor[i];
    }
  }
  else {
    // default background color
    for (int i = 0; i < 4; i++) {
      item.backgroundColor[i] = 0.0f;
    }
  }

  // this assignment operator copies the c string
  item.text = buttonText;

  mMenuItems.push_back (item);

  return mMenuItems.size() - 1;
}


size_t GameMenu::addText(
  v2d_t position,
  v2d_t dimensions,
  v2d_t fontSize,
  const char *text,
  int textJustification,
  const GLfloat *color,
  const GLfloat *backgroundColor)
{
  menu_item_t item;

  item.type = MENUITEM_TEXT;
  item.position[0] = (GLfloat)position.x;
  item.position[1] = (GLfloat)position.y;
  item.dimensions[0] = (GLfloat)dimensions.x;
  item.dimensions[1] = (GLfloat)dimensions.y;
  item.fontSize[0] = (GLfloat)fontSize.x;
  item.fontSize[1] = (GLfloat)fontSize.y;
  item.textJustification = textJustification;

  if (color != NULL) {
    for (int i = 0; i < 4; i++) {
      item.color[i] = color[i];
    }
  }
  else {
    // default color
    for (int i = 0; i < 4; i++) {
      item.color[i] = 1.0f;
    }
  }

  if (backgroundColor != NULL) {
    for (int i = 0; i < 4; i++) {
      item.backgroundColor[i] = backgroundColor[i];
    }
  }
  else {
    // default background color
    for (int i = 0; i < 4; i++) {
      item.backgroundColor[i] = 0.0f;
    }
  }

  item.text = text;

  mMenuItems.push_back(item);

  return mMenuItems.size() - 1;
}


int GameMenu::gameMenuChoice(bool waitForInput) {
  // release the mouse
  SDL_SetRelativeMouseMode(SDL_FALSE);
  SDL_ShowCursor(1);

  SDL_Event sdlevent;

//  setupOpenGl ();

  int click = -1;
  // only wait if we're told
  if (waitForInput) {
    SDL_WaitEvent(&sdlevent);
  }
  else {
    if (!SDL_PollEvent(&sdlevent)) {
      return -1;
    }
  }

  switch (sdlevent.type) {
    case SDL_QUIT:
      click = MENU_APPLICATION_QUIT;
      break;

    // handle the mousebuttondown event
    case SDL_MOUSEBUTTONDOWN:
//        handle_mouse_button_down (sdlevent.button.button, v2d_v (sdlevent.button.x, SCREEN_H - sdlevent.button.y));
      break;

    case SDL_MOUSEBUTTONUP:
      if (sdlevent.button.button == SDL_BUTTON_LEFT) {
//        click = mouseUp(sdlevent.button.x / (double)SCREEN_W, ((double)SCREEN_H - sdlevent.button.y) / (double)SCREEN_H);
        click = mouseUp((double)sdlevent.button.x / (double)SCREEN_W, (double)sdlevent.button.y / (double)SCREEN_H);
      }

      break;

    default:
      break;
  }


  if (click <= 0) {
    return click;
  }

  return mMenuItems[click].value;
}


int GameMenu::mouseUp(double x, double y) {
  // see if the dummy actually managed to hit something
  for (size_t i = 0; i < mMenuItems.size(); i++) {
    if (mMenuItems[i].type == MENUITEM_BUTTON) {
      if (x >= mMenuItems[i].position[0] &&
        x <= (mMenuItems[i].position[0] + mMenuItems[i].dimensions[0]) &&
        y >= mMenuItems[i].position[1] &&
        y <= (mMenuItems[i].position[1] + mMenuItems[i].dimensions[1]))
      {
        return (int)i;
      }
    }
  }

  // better luck next time
  return -1;
}


void GameMenu::draw(void) {
  // set up the opengl viewport
  glPushMatrix ();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0, 1.0, 1.0, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  // draw the button/text background
  glBegin (GL_QUADS);
    for (size_t i = 0; i < mMenuItems.size(); i++) {
      GLfloat br[2] = {
        mMenuItems[i].position[0] + mMenuItems[i].dimensions[0],
        mMenuItems[i].position[1] + mMenuItems[i].dimensions[1] };


      glColor4fv(mMenuItems[i].backgroundColor);
      glVertex2f(mMenuItems[i].position[0], br[1]);
      glVertex2f(br[0], br[1]);
      glVertex2f(br[0], mMenuItems[i].position[1]);
      glVertex2f(mMenuItems[i].position[0], mMenuItems[i].position[1]);
    }
  glEnd ();

  glEnable(GL_TEXTURE_2D);

  GLfloat pos[2];
  GLfloat dim[2];

  for (size_t i = 0; i < mMenuItems.size(); i++) {

    switch (mMenuItems[i].textJustification) {
      case TEXT_JUSTIFICATION_LEFT:
        dim[0] = mMenuItems[i].fontSize[0] * mGlBitmapFont.getStringWidth (mMenuItems[i].text);
        dim[1] = mMenuItems[i].fontSize[1];

        pos[0] = mMenuItems[i].position[0] + (0.5f * mMenuItems[i].fontSize[0]);
        pos[1] = mMenuItems[i].position[1] + ((mMenuItems[i].dimensions[1] - dim[1]) * 0.5f);
        break;

      default:
      case TEXT_JUSTIFICATION_CENTER:
        dim[0] = mMenuItems[i].fontSize[0] * mGlBitmapFont.getStringWidth (mMenuItems[i].text);
        dim[1] = mMenuItems[i].fontSize[1];

        pos[0] = mMenuItems[i].position[0] + ((mMenuItems[i].dimensions[0] - dim[0]) * 0.5f);
        pos[1] = mMenuItems[i].position[1] + ((mMenuItems[i].dimensions[1] - dim[1]) * 0.5f);
        break;

      case TEXT_JUSTIFICATION_RIGHT:
        GLfloat rightPos = mMenuItems[i].position[0] + mMenuItems[i].dimensions[0];

        dim[0] = mMenuItems[i].fontSize[0] * mGlBitmapFont.getStringWidth (mMenuItems[i].text);
        dim[1] = mMenuItems[i].fontSize[1];

        pos[0] = rightPos - (dim[0] + (0.5f * mMenuItems[i].fontSize[0]));
        pos[1] = mMenuItems[i].position[1] + ((mMenuItems[i].dimensions[1] - dim[1]) * 0.5f);
        break;
    }

    mGlBitmapFont.drawString(
      pos,
      mMenuItems[i].fontSize,
      mMenuItems[i].text.c_str(),
      mMenuItems[i].color);
  }

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  glPopMatrix();

//  mGameWindow->swapBuffers();
}




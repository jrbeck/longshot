#include "RogueMapViewer.h"

RogueMapViewer::RogueMapViewer(GameWindow* gameWindow) :
  mMenu(0),
  mRogueMap(0),
  dungeon(0)
{
  mGameWindow = gameWindow;
  mLeftMouseButtonClicked = false;
}

RogueMapViewer::~RogueMapViewer() {
  if (mMenu != 0) {
    delete mMenu;
  }
  if (mRogueMap != 0) {
    delete mRogueMap;
  }
  if (dungeon != 0) {
    delete dungeon;
  }
}

void RogueMapViewer::setUpOpenGl() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  mViewRect.mNearCorner.x = 0.0;
  mViewRect.mNearCorner.y = 0.0;
  mViewRect.mFarCorner.x = ROGUE_MAP_SIDE;
  mViewRect.mFarCorner.y = ROGUE_MAP_SIDE;

  glOrtho(
    mViewRect.mNearCorner.x, mViewRect.mFarCorner.x,
    mViewRect.mNearCorner.y, mViewRect.mFarCorner.y,
    -20.0, 20.0);


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glDisable(GL_TEXTURE_2D);

//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  SDL_GL_SwapBuffers();
}

void RogueMapViewer::startViewer() {
  mMenu = new GameMenu();
  if (mMenu == 0) {
    printf("RogueMapViewer::chooseLocation(): error: out of memory 1\n");
    return;
  }
//  mMenu->setFont(gDefaultFontTextureHandle);

  mRogueMap = new RogueMap(ROGUE_MAP_SIDE, ROGUE_MAP_SIDE);
  if (mRogueMap == 0) {
    printf("RogueMapViewer::chooseLocation(): error: out of memory 2\n");
    delete mMenu;
    return;
  }

  printf("PRE CREATE\n");
  dungeon = new DungeonUtil();
  printf("POST CREATE\n");
  dungeon->createDungeonModel( ROGUE_MAP_SIDE, ROGUE_MAP_SIDE);
  randomizeDungeon();
  printf("POST RANDOMIZE\n");

  setUpOpenGl();

  char positionString[256];
  GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.7f };
  v2d_t fontSize = { 0.01f, 0.02f };

  bool quit = false;
  while (!quit) {
    if (handleInput() == 1) {
      quit = true;
    }

    // TEMPORARY * * * * * * * * *
    sprintf(positionString, "this string...");

    mMenu->clear();
    mMenu->addText(
      v2d_v(0.01, 0.01),
      v2d_v(0.4, 0.04),
      fontSize,
      positionString,
      TEXT_JUSTIFICATION_LEFT,
      color,
      bgColor);


    // k let's draw!
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap();
    mMenu->draw();
    mGameWindow->swapBuffers();
    setUpOpenGl();
  }
}

void RogueMapViewer::drawMap() const {
  v3d_t corners[4];
  v3d_t color;
  map_tile_t tile;

  DungeonTile *dungeonTile;

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBegin(GL_QUADS);

  for (int j = 0; j < ROGUE_MAP_SIDE; j++) {
    for (int i = 0; i < ROGUE_MAP_SIDE; ++i) {
      corners[0].x = static_cast<int>(i);
      corners[0].z = 0.0; //mTerrain->get_value(i, j);
      corners[0].y = static_cast<int>(j);

      corners[1].x = static_cast<int>(i + 1);
      corners[1].z = 0.0; //mTerrain->get_value(i + 1, j);
      corners[1].y = static_cast<int>(j);

      corners[2].x = static_cast<int>(i + 1);
      corners[2].z = 0.0; //mTerrain->get_value(i + 1, j + 1);
      corners[2].y = static_cast<int>(j + 1);

      corners[3].x = static_cast<int>(i);
      corners[3].z = 0.0; //mTerrain->get_value(i, j + 1);
      corners[3].y = static_cast<int>(j + 1);

      tile = mRogueMap->getTile(i, j);

      dungeonTile = dungeon->getDungeonModel()->getTile(i, j);

      switch(dungeonTile->type) {
      default:
      case DUNGEON_TILE_FLOOR:
        color.x = 0.4;
        color.y = 0.2;
        color.z = 0.0;
        break;
      case DUNGEON_TILE_WALL:
        color.x = 0.6;
        color.y = 0.6;
        color.z = 0.6;
        break;
      case DUNGEON_TILE_WATER:
        color.x = 0.0;
        color.y = 0.0;
        color.z = 0.7;
        break;
      }

      //switch(tile.type) {
      //default:
      //case MAP_TILE_INVALID:
      //  color.x = 0.0;
      //  color.y = 0.0;
      //  color.z = 0.0;
      //  break;
      //case MAP_TILE_WALL:
      //  color.x = 1.0;
      //  color.y = 0.0;
      //  color.z = 0.0;
      //  break;
      //case MAP_TILE_FLOOR:
      //  color.x = 0.0;
      //  color.y = 1.0;
      //  color.z = 0.0;
      //  break;
      //case MAP_TILE_PATH_SEED:
      //  color.x = 0.0;
      //  color.y = 1.0;
      //  color.z = 1.0;
      //  break;
      //case MAP_TILE_DOORWAY:
      //  color.x = 0.0;
      //  color.y = 0.0;
      //  color.z = 1.0;
      //  break;
      //case MAP_TILE_PATH:
      //  color.x = 1.0;
      //  color.y = 1.0;
      //  color.z = 0.0;
      //  break;
      //}

      glColor3d(color.x, color.y, color.z);
      glVertex3d(corners[0].x, corners[0].y, corners[0].z);
      glVertex3d(corners[1].x, corners[1].y, corners[1].z);
      glVertex3d(corners[2].x, corners[2].y, corners[2].z);
      glVertex3d(corners[3].x, corners[3].y, corners[3].z);
    }
  }


  glEnd();
  glEnable(GL_TEXTURE_2D);
}

// handle an SDL_Event
int RogueMapViewer::handleInput() {
  int quit = 0;
  mMouseMoved = 0;

  // goes through all the queued events and deals with each one
  while (SDL_PollEvent(&sdlevent) && !quit) {
    switch (sdlevent.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_KEYDOWN:
        quit = handleKeystroke();
        break;

      case SDL_KEYUP:
        handleKeyup();
        break;

      case SDL_MOUSEMOTION:
        mMouseDelta.x = -sdlevent.motion.xrel;
        mMouseDelta.y = sdlevent.motion.yrel;

        mMousePos.x = sdlevent.motion.x;
        mMousePos.y = gScreenH - sdlevent.motion.y;

        mMouseMoved = 1;

        break;

      // handle the mousebuttondown event
      case SDL_MOUSEBUTTONDOWN:
        handleMouseButtonDown(sdlevent.button.button, v2d_v(sdlevent.button.x, gScreenH - sdlevent.button.y));
        break;

      case SDL_MOUSEBUTTONUP:
        handleMouseButtonUp(sdlevent.button.button, v2d_v(sdlevent.button.x, gScreenH - sdlevent.button.y));
        break;

      default:
        break;
    }
  }

  // RTS MODE
  // handle mouse drag
  // FIXME: not here please!
  Uint8 ms;
  ms = SDL_GetMouseState(0, 0);

  if (mMouseMoved && (ms & SDL_BUTTON(SDL_BUTTON_LEFT))) {
    v2d_t md = v2d_scale(mMouseDelta, 0.1);

    md.x = -md.x;

//    mRtsCam.translate (md);
  }
  if (mMouseMoved && (ms & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
    v2d_t md = v2d_scale(mMouseDelta, 0.002);

    md.x = -md.x;

//    mRtsCam.pan (md);
  }

  return quit;
}

void RogueMapViewer::randomizeDungeon() {
  MoleculeDungeon::createDungeon(*dungeon, v2di_v(ROGUE_MAP_SIDE / 2, ROGUE_MAP_SIDE / 2));
}

int RogueMapViewer::handleKeystroke() {
  switch (sdlevent.key.keysym.sym) {
    case SDLK_ESCAPE:  // quit
      return 1;

    case SDLK_n:
      mRogueMap->randomize(true, 5);
      randomizeDungeon();
      break;

    case SDLK_z:
      break;

  }

  // don't quit!
  return 0;
}

int RogueMapViewer::handleKeyup() {
/*  if (*mode == MODE_PLAYER) {
    switch (sdlevent.key.keysym.sym) {
      case SDLK_w:
        break;


      case SDLK_SPACE:
        break;
    }
  }
*/

  return 0;
}

void RogueMapViewer::handleMouseButtonDown(int button, v2d_t pos) {
  switch (button) {
    case SDL_BUTTON_RIGHT:
      break;
  }
}

void RogueMapViewer::handleMouseButtonUp(int button, v2d_t pos) {
  switch (button) {
  case SDL_BUTTON_LEFT:
    mLeftMouseButtonClicked = true;
    break;
  case SDL_BUTTON_RIGHT:
    break;
  }
}

#include "PlanetMap.h"

PlanetMap::PlanetMap(GameWindow* gameWindow) :
  mMenu(0),
  mTerrain(0),
  mColors(0),
  mPeriodics(0)
{
  mGameWindow = gameWindow;
  mLeftMouseButtonClicked = false;
}

PlanetMap::~PlanetMap() {
  if (mMenu != 0) {
    delete mMenu;
  }
  if (mTerrain != 0) {
    delete mTerrain;
  }
  if (mColors != 0) {
    delete [] mColors;
  }
  if (mPeriodics != 0) {
    delete mPeriodics;
  }
}

void PlanetMap::setUpOpenGl() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  mViewRect.mNearCorner.x = 0.0;
  mViewRect.mNearCorner.y = 0.0;
  mViewRect.mFarCorner.x = PLANET_MAP_SIDE;
  mViewRect.mFarCorner.y = PLANET_MAP_SIDE;

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

bool PlanetMap::chooseLocation(Planet &planet, v3d_t &planetPos) {
  mMenu = new GameMenu();
  if (mMenu == 0) {
    printf("PlanetMap::chooseLocation(): error: out of memory 1\n");
    return false;
  }
//  mMenu->setFont(gDefaultFontTextureHandle);

  mTerrain = new Terrain(PLANET_MAP_SIDE);
  if (mTerrain == 0) {
    printf("PlanetMap::chooseLocation(): error: out of memory 2\n");
    delete mMenu;
    return false;
  }
  mColors = new v3d_t[PLANET_MAP_SIDE * PLANET_MAP_SIDE];
  if (mColors == 0) {
    printf("PlanetMap::chooseLocation(): error: out of memory 3\n");
    delete mMenu;
    delete mTerrain;
    return false;
  }
  mPeriodics = new Periodics();
  if (mPeriodics == 0) {
    printf("PlanetMap::chooseLocation(): error: out of memory 4\n");
    delete mMenu;
    delete mTerrain;
    delete [] mColors;
    return false;
  }

  buildFromPeriodics(planet.mSeed);

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

    if (mLeftMouseButtonClicked) {
      double x = mViewRect.getWidth() * mMousePos.x / SCREEN_W;
      double y = mViewRect.getHeight() * mMousePos.y / SCREEN_H;

      planetPos.x = (x - (PLANET_MAP_SIDE / 2)) * MAP_MULTIPLIER;
      planetPos.z = (y - (PLANET_MAP_SIDE / 2)) * MAP_MULTIPLIER;
      planetPos.y = mPeriodics->getTerrainHeight(planetPos.x, planetPos.z) + 30.0;


//      v2d_print("mouse", mMousePos);
      printf("planetMap: %.0f, %.0f\n", planetPos.x, planetPos.z);

      mLeftMouseButtonClicked = false;

      return true;
    }


    // TEMPORARY * * * * * * * * *
    double x = mViewRect.getWidth() * mMousePos.x / SCREEN_W;
    double y = mViewRect.getHeight() * mMousePos.y / SCREEN_H;

    v3d_t pPos;
    pPos.x = (x - (PLANET_MAP_SIDE / 2)) * MAP_MULTIPLIER;
    pPos.z = (y - (PLANET_MAP_SIDE / 2)) * MAP_MULTIPLIER;
    pPos.y = mPeriodics->getTerrainHeight(pPos.x, pPos.z);

    BiomeInfo bi = mPeriodics->getBiomeInfo(pPos.x, pPos.z);
    sprintf(positionString, "x: %.0f, z: %.0f, height: %.0f, biome: %d, %.2f", pPos.x, pPos.z, pPos.y, bi.type, bi.value);

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
    drawTerrain();
    mMenu->draw();
    mGameWindow->swapBuffers();
    setUpOpenGl();
  }

  printf("PlanetMap::chooseLocation(): no location chosen\n");

  return false;
}

void PlanetMap::drawMap() {
}

void PlanetMap::buildFromPeriodics(int seed) {
  mPeriodics->randomize(seed);
  printf("PlanetMap::buildFromPeriodics():seed: %d\n", seed);

  v3di_t worldPosition;

  int halfMapSide = PLANET_MAP_SIDE / 2;

  for (int j = 0; j < PLANET_MAP_SIDE; j++) {
    for (int i = 0; i < PLANET_MAP_SIDE; ++i) {
      worldPosition.x = ((i - halfMapSide) * MAP_MULTIPLIER);
      worldPosition.z = ((j - halfMapSide) * MAP_MULTIPLIER);

      int height = worldPosition.y = mPeriodics->getTerrainHeight(worldPosition.x, worldPosition.z);
      mTerrain->set_value(i, j, height);

      int block = mPeriodics->generateBlockAtWorldPosition(worldPosition);
      int colorIndex = i + (j * PLANET_MAP_SIDE);

      if (height <= WATER_LEVEL) {
        mColors[colorIndex].x = 0.0;
        mColors[colorIndex].y = 0.0;
        mColors[colorIndex].z = 0.8;
      }
      else {
        BiomeInfo bi = mPeriodics->getBiomeInfo(worldPosition.x, worldPosition.z);
        mColors[colorIndex] = v3d_scale(1.0, gBiomeTypes[bi.type].planetMapColor);
      }
    }
  }

  mTerrain->normalize(-10.0, 10.0);

  // let's do a little shading to make it look nicer
  for (int j = 0; j < PLANET_MAP_SIDE; j++) {
    for (int i = 0; i < PLANET_MAP_SIDE; ++i) {
      double height = mTerrain->get_value(i, j);

      GLfloat colorMult = 0.8 * ((GLfloat)height + 10.0f) / 20.0;
      colorMult += 0.3f;

      int colorIndex = i + (j * PLANET_MAP_SIDE);
      mColors[colorIndex].x *= colorMult;
      mColors[colorIndex].y *= colorMult;
      mColors[colorIndex].z *= colorMult;
    }
  }

  printf("PlanetMap::buildFromPeriodics():done\n");
}

// handle an SDL_Event
int PlanetMap::handleInput() {
  int quit = 0;
  mMouseMoved = 0;

  // goes through all the queued events and deals with each one
  while (SDL_PollEvent (&sdlevent) && !quit) {
    switch (sdlevent.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_KEYDOWN:
        quit = handleKeystroke ();
        break;

      case SDL_KEYUP:
        handleKeyup ();
        break;

      case SDL_MOUSEMOTION:
        mMouseDelta.x = -sdlevent.motion.xrel;
        mMouseDelta.y = sdlevent.motion.yrel;

        mMousePos.x = sdlevent.motion.x;
        mMousePos.y = SCREEN_H - sdlevent.motion.y;

        mMouseMoved = 1;

        break;

      // handle the mousebuttondown event
      case SDL_MOUSEBUTTONDOWN:
        handleMouseButtonDown (sdlevent.button.button, v2d_v (sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      case SDL_MOUSEBUTTONUP:
        handleMouseButtonUp (sdlevent.button.button, v2d_v (sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      default:
        break;
    }
  }

  // RTS MODE
  // handle mouse drag
  // FIXME: not here please!
  Uint8 ms;
  ms = SDL_GetMouseState (0, 0);

  if (mMouseMoved && (ms & SDL_BUTTON (SDL_BUTTON_LEFT))) {
    v2d_t md = v2d_scale (mMouseDelta, 0.1);
    md.x = -md.x;

//    mRtsCam.translate (md);
  }
  if (mMouseMoved && (ms & SDL_BUTTON (SDL_BUTTON_RIGHT))) {
    v2d_t md = v2d_scale (mMouseDelta, 0.002);

    md.x = -md.x;

//    mRtsCam.pan (md);
  }

  return quit;
}

int PlanetMap::handleKeystroke() {
  switch (sdlevent.key.keysym.sym) {
    case SDLK_ESCAPE:  // quit
      return 1;

    case SDLK_n:
//      buildFromPeriodics();
      break;

    case SDLK_z:
      break;

  }

  // don't quit!
  return 0;
}

int PlanetMap::handleKeyup() {
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

void PlanetMap::handleMouseButtonDown(int button, v2d_t pos) {
  switch (button) {
    case SDL_BUTTON_RIGHT:
      break;
  }
}

void PlanetMap::handleMouseButtonUp(int button, v2d_t pos) {
  switch (button) {
  case SDL_BUTTON_LEFT:
    mLeftMouseButtonClicked = true;
    break;
  case SDL_BUTTON_RIGHT:
    break;
  }
}

void PlanetMap::drawTerrain() const {
  v3d_t corners[4];

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBegin(GL_QUADS);

  for (int j = 0; j < PLANET_MAP_SIDE; j++) {
    for (int i = 0; i < PLANET_MAP_SIDE; ++i) {
      corners[0].x = static_cast<int>(i);
      corners[0].z = mTerrain->get_value(i, j);
      corners[0].y = static_cast<int>(j);

      corners[1].x = static_cast<int>(i + 1);
      corners[1].z = mTerrain->get_value(i + 1, j);
      corners[1].y = static_cast<int>(j);

      corners[2].x = static_cast<int>(i + 1);
      corners[2].z = mTerrain->get_value(i + 1, j + 1);
      corners[2].y = static_cast<int>(j + 1);

      corners[3].x = static_cast<int>(i);
      corners[3].z = mTerrain->get_value(i, j + 1);
      corners[3].y = static_cast<int>(j + 1);

      int colorIndex = i + (j * PLANET_MAP_SIDE);

      glColor3d(mColors[colorIndex].x, mColors[colorIndex].y, mColors[colorIndex].z);
      glVertex3d(corners[0].x, corners[0].y, corners[0].z);
      glVertex3d(corners[1].x, corners[1].y, corners[1].z);
      glVertex3d(corners[2].x, corners[2].y, corners[2].z);
      glVertex3d(corners[3].x, corners[3].y, corners[3].z);
    }
  }

  glEnd();
  glEnable(GL_TEXTURE_2D);
}

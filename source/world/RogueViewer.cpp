#include "../world/RogueViewer.h"

RogueViewer::RogueViewer(GameWindow* gameWindow, AssetManager* assetManager, GameModel* gameModel) :
  mGameWindow(gameWindow),
  mAssetManager(assetManager),
  mGameModel(gameModel),
  mWorldMapView(NULL),
  mVizMode(0)
{
  mWorldMapView = new WorldMapView(assetManager, gameModel);

  for (int i = 0; i < NUM_VIZ_MODES; ++i) {
    mCameraStates[i].position = v3d_v(50.0, 50.0, 50.0);
    mCameraStates[i].target = v3d_v(0.0, 0.0, 0.0);
  }
}

RogueViewer::~RogueViewer() {
  if (mWorldMapView != NULL) {
    delete mWorldMapView;
  }
}

void RogueViewer::setupOpenGl() {
  GLfloat color[4] = { 0.1f, 0.4f, 0.9f, 0.0f };

  // set up the viewport
  glViewport(0, 0, gScreenW, gScreenH);

  // enable various things
  glEnable(GL_TEXTURE_2D);                // Enable 2D Texture Mapping
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // Select The Type Of Blending
//  glEnable (GL_BLEND);
  glShadeModel(GL_SMOOTH);                // Enables Smooth Color Shading
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glEnable(GL_CULL_FACE);

  glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

//  glEnable (GL_COLOR_MATERIAL);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


  // FOG
  glFogi(GL_FOG_MODE, GL_LINEAR);          // Fog Mode
  glFogfv(GL_FOG_COLOR, color);            // Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.15f);            // How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
  glFogf(GL_FOG_START, 200.0f);            // Fog Start Depth
  glFogf(GL_FOG_END, 300.0f);            // Fog End Depth
  glEnable(GL_FOG);                  // Enables GL_FOG

  // go ahead and clear the buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mGameWindow->swapBuffers();

  printf("\n%6d: opengl_setup done ----------------\n", SDL_GetTicks());
}

int RogueViewer::start() {
  printf("%6d: entered rogue_viz ----------------\n", SDL_GetTicks());

  // really? here?
//  GameMenu menu;
//  menu.clear ();
//  float color[] = {0.55f, 0.075f, 0.075f, 1.0f};
//  menu.add_text (v2d_v (0.3, 0.95), v2d_v (0.5, 0.8), "longshot", color);

  mRogueMap.resize(64, 64);

  mAssetManager->loadAssets();
//  mWorld.randomizePeriodics(0);
//  mWorld.initialize(0, NULL);

//  mWorld.loadFeaturesAroundPlayer (v3d_v (0.0, 0.0, 0.0)); //mRtsCam.getTarget ());
//  mWorld.loadAllColumns (mRtsCam.getTarget (), mAssetManager);
//  mWorld.generateClouds (mRtsCam.getTarget ());

  // set up a map for each mode
  for (int i = 0; i < NUM_VIZ_MODES; ++i) {
    swapVizMode(i);
    generateNewMap();
  }
  swapVizMode(VIZ_MODE_ROGUE);

  unsigned int ticks = SDL_GetTicks();
  int frame = 0, quit = 0, blocks_drawn = 0;

  // change the mouse mode
//  SDL_WM_GrabInput(SDL_GRAB_OFF);
//  SDL_ShowCursor(1);

  GlCamera* cam;

  mRtsCam.initializeCamera(v2di_v(gScreenW, gScreenH), 45.0, 0.1, 1024.0);

  setupOpenGl();

  while (!quit) {
    unsigned int delta_t = SDL_GetTicks() - ticks;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam = mRtsCam.gl_cam_setup();

    switch (mVizMode) {
      case VIZ_MODE_MODEL:
        // draw the model
        mWorldMapView->drawSolidBlocks(*cam);
        mWorldMapView->drawLiquidBlocks(*cam);
        break;

      case VIZ_MODE_ROGUE:
        mRogueMap.draw();
        break;

//      case VIZ_MODE_WORLD:
//      mWorld.update(mRtsCam.getTarget(), mAssetManager);

      // draw the world
//      mWorld.drawLand(cam2, mAssetManager);
//      mWorld.drawWater(cam2, mAssetManager);

        break;
    }

    mGameWindow->swapBuffers();

    // do some frames per second calculating
    frame++;
    if (delta_t >= 5000) {
      mRtsCam.gl_cam_setup();

      printf("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
      printf("average blocks per frame: %10.2f\n", (double)blocks_drawn / (double)frame);
      frame = 0;
      blocks_drawn = 0;
      ticks = SDL_GetTicks();
    }

    // deal with the input
    quit = handleInput();
  }

  // clean up a bit
  mAssetManager->freeAssets();

  printf("%6d: exiting rogue_viz ----------------\n", SDL_GetTicks());
  return 0;
}

v3d_t RogueViewer::findStartPosition(WorldMap& worldMap) {
  bool foundAcceptableStartingLocation = false;
  int numAttempts = 0;

  double limit = 100.0;

  v2d_t searchPos;
  searchPos.x = r_num(-10000.0, 10000.0);
  searchPos.y = r_num(-10000.0, 10000.0);

  v3d_t pos;

  while (!foundAcceptableStartingLocation) {
    pos.x = r_num(-limit, limit) + searchPos.x;
    pos.z = r_num(-limit, limit) + searchPos.y;

    // FIXME: this was changed (to: pos.y = 0.0) when Periodics were being removed from
    // WorldMap... i.e. 0.0 is just a random number!!
//    pos.y = worldMap.getTerrainHeight(static_cast<int>(pos.x), static_cast<int>(pos.z)) + 2.0;
    pos.y = 0.0;
    if (pos.y > 1.0 && pos.y < 20.0) break;
    if (numAttempts++ > 10000) break;

    limit += 4.0;
  }

  pos.y += 30.0;

  v3d_print("start position", pos);
  printf("numAttempts: %d\n", numAttempts);

  return pos;
}

// handle an SDL_Event
int RogueViewer::handleInput() {
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
  ms = SDL_GetMouseState(NULL, NULL);

  if (mMouseMoved && (ms & SDL_BUTTON(SDL_BUTTON_LEFT))) {
    v2d_t md = v2d_scale(mMouseDelta, 0.1);
    md.x = -md.x;
    mRtsCam.translate(md);
  }
  if (mMouseMoved && (ms & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
    v2d_t md = v2d_scale(mMouseDelta, 0.002);
    md.x = -md.x;
    mRtsCam.pan(md);
  }

  return quit;
}

int RogueViewer::handleKeystroke() {
  switch (sdlevent.key.keysym.sym) {
    case SDLK_ESCAPE:  // quit
      return 1;

    case SDLK_a:
      mRtsCam.changeElevation(1.0);
      break;

    case SDLK_z:
      mRtsCam.changeElevation(-1.0);
      break;

    case SDLK_v:
      swapVizMode((mVizMode + 1) % NUM_VIZ_MODES);
      break;

    case SDLK_n:
      generateNewMap();
      break;

    case SDLK_p:    // pause
//      paused = (paused + 1) % 2;
      break;

    case SDLK_r:    // generate a random number
      rand();
      break;
  }

  // don't quit!
  return 0;
}

int RogueViewer::handleKeyup() {
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

void RogueViewer::handleMouseButtonDown(int button, v2d_t pos) {
  switch (button) {
    case SDL_BUTTON_RIGHT:
      break;
  }
}

void RogueViewer::handleMouseButtonUp(int button, v2d_t pos) {
/*  if (*mode == MODE_PLAYER) {
    switch (button) {
      case SDL_BUTTON_RIGHT:
        player->add_input_event(INPUT_TYPE_ROCKET_UP, 1.0);
        break;
    }
  }*/
}

void RogueViewer::handleMouseWeelEvent(SDL_MouseWheelEvent wheelEvent) {

  if (wheelEvent.y < 0) {
      mRtsCam.zoom(20.0);
  }
  else if (wheelEvent.y > 0) {
      mRtsCam.zoom(-20.0);
  }
}

void RogueViewer::swapVizMode(int mode) {
  if (mode == mVizMode) {
    // no change needed!
    return;
  }

  mCameraStates[mVizMode].position = mRtsCam.getPosition();
  mCameraStates[mVizMode].target = mRtsCam.getTarget();

  mVizMode = mode;

  mRtsCam.setPosition(mCameraStates[mVizMode].position);
  mRtsCam.setTarget(mCameraStates[mVizMode].target);
}

void RogueViewer::generateNewMap() {

  if (mVizMode == VIZ_MODE_MODEL) {
    mWorldMap.clear(true);

    v3d_t startLocation = findStartPosition(mWorldMap);

    // FIXME: this was commented out when Periodics was removed from WorldMap
    // it will generate NOTHING without this
//    FeatureGenerator::createForViewer(startLocation, 0, mWorldMap);

    // prepare for viewing
    for (int i = 0; i < mWorldMap.mNumColumns; ++i) {
      // FIXME: this was commented out when createShadowVolume
      // got an option for a cloud cover...
//      WorldLighting::createShadowVolume(i, mWorldMap);
    }
    for (int i = 0; i < mWorldMap.mNumColumns; ++i) {
      mWorldMap.updateBlockVisibility(i);
      // FIXME: this was commented out when LightmManager was required
      // it will generate NOTHING without this
//      WorldLighting::applyLighting(i, mWorldMap);
    }
    // FIXME: this was commented out when LightmManager was required
    // it will generate NOTHING without this
//    mWorldMapView->update(mAssetManager, false);


    // we'll just re-use this variable
    startLocation.x += (mWorldMap.mXWidth * WORLD_CHUNK_SIDE * 0.5);
    startLocation.z += (mWorldMap.mZWidth * WORLD_CHUNK_SIDE * 0.5);

    mRtsCam.setTargetAndUpdate(startLocation);

  }
  else if (mVizMode == VIZ_MODE_ROGUE) {
    mRogueMap.clear();
    mRogueMap.random_room(500, 6, 6);
    mRogueMap.random_room(501, 6, 6);
//    mRogueMap.random_room(502, 6, 6);
//    mRogueMap.random_room(503, 6, 6);
//    mRogueMap.random_room(504, 6, 6);

    mRogueMap.randomize(false, 6);
//    mRogueMap.grow_paths();

    v3d_t rogueTarget = {
      (double)mRogueMap.getWidth() * 0.5,
      0.0,
      (double)mRogueMap.getHeight() * 0.5};

    mRtsCam.setTargetAndUpdate(rogueTarget);
  }
}

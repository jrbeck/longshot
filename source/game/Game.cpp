#include "../game/Game.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)

game_c::game_c(GameWindow* gameWindow) :
  mAssetManager(NULL),
  mGameModel(NULL),
  mPlayerView(NULL),
  mPlayerController(NULL),
  mAiView(NULL),
  mWorldMapView(NULL),
  mPhysicsView(NULL),
  mMerchantView(NULL),
  mMenu(NULL)
{
  printf("game_c::game_c ----------------\n");
  mLastUpdateTime = 0.0;

  mGameWindow = gameWindow;

  printf("loading assets\n");
  mAssetManager = new AssetManager();
  mAssetManager->loadAssets();
  printf("\n%6d: assets loaded\n", SDL_GetTicks());

  mGameInput = new GameInput;
}

game_c::~game_c() {
  printf("game_c::~game_c ----------------\n");
  if (mGameInput != NULL) {
    delete mGameInput;
  }
  if (mPlayerView != NULL) {
    delete mPlayerView;
  }
  if (mPlayerController != NULL) {
    delete mPlayerController;
  }
  if (mAiView != NULL) {
    delete mAiView;
  }
  if (mWorldMapView != NULL) {
    delete mWorldMapView;
  }
  if (mPhysicsView != NULL) {
    delete mPhysicsView;
  }
  if (mMenu != NULL) {
    delete mMenu;
  }
  if (mMerchantView != NULL) {
    delete mMerchantView;
  }
  if (mGameModel != NULL) {
    delete mGameModel;
  }
  if (mAssetManager != NULL) {
    delete mAssetManager;
  }
}

void game_c::loadPlanetMenu() {
  if (mMenu != NULL) {
    delete mMenu;
    mMenu = NULL;
  }
  mMenu = new GameMenu();
//  mMenu->setFont(gDefaultFontTextureHandle);

  GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
  v2d_t fontSize = { 0.015f, 0.03f };

  mMenu->addButton(v2d_v(0.80, 0.15), v2d_v(0.2, 0.1), fontSize, "back to game", TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
  mMenu->addButton(v2d_v(0.80, 0.3), v2d_v(0.2, 0.1), fontSize, "quit to menu", TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);

  // TEMP: these are just for testing
  mMenu->addButton(v2d_v(0.0, 0.15), v2d_v(0.2, 0.1), fontSize, "ship", TEXT_JUSTIFICATION_CENTER, GAMEMENU_SHIP, color, bgColor);
}

void game_c::loadShipMenu() {
  if (mMenu != NULL) {
    delete mMenu;
    mMenu = NULL;
  }
  mMenu = new GameMenu();
//  mMenu->setFont(gDefaultFontTextureHandle);

  GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
  v2d_t fontSize = { 0.015f, 0.03f };

  mMenu->addButton(v2d_v(0.80, 0.15), v2d_v(0.2, 0.1), fontSize, "back to game", TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
  mMenu->addButton(v2d_v(0.80, 0.3), v2d_v(0.2, 0.1), fontSize, "quit to menu", TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);

  if (mGameModel->mCurrentPlanet != NULL) {
    sprintf(mPlanetNameString, "orbiting planet: %d", mGameModel->mCurrentPlanet->mHandle);
  }
  else {
    sprintf(mPlanetNameString, "orbiting planet: NULL");
  }
  mMenu->addText(v2d_v(0.35, 0.05), v2d_v(0.3, 0.1), fontSize, mPlanetNameString,  TEXT_JUSTIFICATION_CENTER, color, bgColor);


  // TEMP: these are just for testing
  mMenu->addButton(v2d_v(0.0, 0.15), v2d_v(0.2, 0.1), fontSize, "planet map", TEXT_JUSTIFICATION_CENTER, GAMEMENU_PLANET_MAP, color, bgColor);
  mMenu->addButton(v2d_v(0.0, 0.30), v2d_v(0.2, 0.1), fontSize, "galaxy map", TEXT_JUSTIFICATION_CENTER, GAMEMENU_GALAXY_MAP, color, bgColor);
  mMenu->addButton(v2d_v(0.0, 0.45), v2d_v(0.2, 0.1), fontSize, "merchant", TEXT_JUSTIFICATION_CENTER, GAMEMENU_MERCHANT, color, bgColor);
  mMenu->addButton(v2d_v(0.0, 0.60), v2d_v(0.2, 0.1), fontSize, "dungeon map", TEXT_JUSTIFICATION_CENTER, GAMEMENU_DUNGEON_MAP, color, bgColor);
}

GameWindow* game_c::getGameWindow() {
  return mGameWindow;
}

void game_c::setupOpenGl() {
  glViewport(0, 0, gScreenW, gScreenH);

  glEnable(GL_TEXTURE_2D);
//  glEnable (GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

//  glColorMaterial (GL_FRONT_AND_BACK, GL_EMISSION);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//  glEnable (GL_COLOR_MATERIAL);

  // FOG
  glFogi(GL_FOG_MODE, GL_LINEAR);
//  glFogfv (GL_FOG_COLOR, color[skyColor]);      // Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.05f);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogf(GL_FOG_START, 100.0f);
  glFogf(GL_FOG_END, 200.0f);
  glEnable(GL_FOG);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mGameWindow->swapBuffers();

  printf("\n%6d: opengl_setup done ----------------\n", SDL_GetTicks());
}

// it is here where the magic begins
int game_c::enterGameMode(bool createNewWorld) {
  printf ("\n%6d: entered GAME mode ----------------\n", SDL_GetTicks());

  mGameModel = new GameModel;

  // create a new game or load an old one
  if (createNewWorld) {
    printf("game_c::enterGameMode(): new game\n");
    FileSystem::deleteAllFilesInFolder(SAVE_FOLDER);

    mGameModel->mCurrentPlanet = mGameModel->mGalaxy->mStarSystems[0]->mPlanets[0];
    mGameModel->initializeStarShip(true);
  }
  else {
    printf("game_c::enterGameMode(): loading game\n");
    mGameModel->load(mGameWindow);
  }

  // load the appropriate menu for the location
  if (mGameModel->mLocation->getType() == LOCATION_SHIP) {
    loadShipMenu();
  }
  else {
    loadPlanetMenu();
  }

  initializeWorldViews();

  // FIXME: maybe this should rely on the mPlayerView so the it doesn't have to
  // happen after initializeWorldViews()
  mPlayerController = new PlayerController(mGameModel, mPlayerView);

  // grab the cursor
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(0);

  setupOpenGl();

  mAssetManager->mSoundSystem.initialize();
  // FIXME: wtf is this??
  mAssetManager->mSoundSystem.loadSound("cow");
  // mAssetManager->mSoundSystem.playSoundByHandle(SOUND_AMBIENT, 64);

  gameLoop();

  mAssetManager->mSoundSystem.stopAllSounds();

  // FIXME: this is temporary ... since we aren't
  // saving the AI, we need to eliminate their items
  mGameModel->destroyItemsOwnedByPhysicsAndAi();

  mGameModel->save();
  delete mGameModel;
  mGameModel = NULL;

  printf("%6d: exiting GAME mode ----------------\n\n", SDL_GetTicks());

  return 0;
}

void game_c::initializeWorldViews() {
  printf("PLANET: %d\n", mGameModel->mCurrentPlanet->mHandle);

  if (mWorldMapView != NULL) {
    delete mWorldMapView;
  }
  mWorldMapView = new WorldMapView(mAssetManager, mGameModel);

  if (mPlayerView != NULL) {
    delete mPlayerView;
  }
  mPlayerView = new PlayerView(mGameModel, mAssetManager);

  if (mPhysicsView != NULL) {
    delete mPhysicsView;
  }
  mPhysicsView = new PhysicsView();

  if (mAiView != NULL) {
    delete mAiView;
  }
  mAiView = new AiView();
}

void game_c::gameLoop() {
  int frame = 0;
  int blocks_drawn = 0;
  unsigned int ticks = SDL_GetTicks();
  mLastUpdateTime = (double)ticks / 1000.0;
  mGameState = GAMESTATE_PLAY;

  int quit = 0;
  bool escapePressed;

  while (quit == 0) {
    handleInput();
    escapePressed = mGameInput->isEscapePressed();
    update();
    draw();

    if (mGameState == GAMESTATE_PLAY) {
      mGameInput->update();
    }
    else if (mGameState == GAMESTATE_MENU) {
      int menuChoice = mMenu->gameMenuChoice(false);
      if (escapePressed) {
        printf("escape\n");
        menuChoice = GAMEMENU_BACKTOGAME;
      }
      quit = handleMenuChoice(menuChoice);
    }
    else if (mGameState == GAMESTATE_MERCHANT) {
      if (mMerchantView->update(*mGameModel->mPlayer, *mGameModel->mItemManager) != 0) {
        delete mMerchantView;
        mMerchantView = NULL;
        mGameState = GAMESTATE_MENU;
      }
    }

    // player hit 'esc' in play mode, switch to menu
    if (escapePressed && mGameState == GAMESTATE_PLAY) {
      mGameState = GAMESTATE_MENU;
      // release the mouse
      SDL_SetRelativeMouseMode(SDL_FALSE);
      SDL_ShowCursor(1);
    }

    // update the world
    // FIXME: this should be done in update()
    // being done here ties it to the framerate
    mGameModel->mLocation->update(mGameModel->mPhysics->getCenter(mGameModel->mPhysics->getPlayerHandle()));

    // HACK * * * * * * *
    mGameModel->mPlayer->placeLight(mGameInput);

    mWorldMapView->update(*mGameModel->mLocation->getLightManager());

    if (mGameInput->isToggleWorldChunkBoxes()) {
      mWorldMapView->toggleShowWorldChunkBoxes();
    }

    // do some frames per second calculating
    unsigned int delta_t = SDL_GetTicks() - ticks;
    frame++;

    if (delta_t >= 5000 && OUTPUT_FRAME_STATS) {
      printf("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
      printf("avg frame: %10.3fms\n", (double)delta_t / (double)frame);
      frame = 0;
      blocks_drawn = 0;
      ticks = SDL_GetTicks();

      printf("num phys: %d\n", mNumPhysicsObjects);
      printf("num ai: %d\n", mNumAiObjects);
      printf("num items: %d\n\n", mNumItems);

    }

    // rescue single processor machines
    // SDL_Delay(1);
  }
}

int game_c::handleMenuChoice(int menuChoice) {
  GalaxyMap* galaxyMap;
  PlanetMap* planetMap;
  RogueMapViewer* rogueMapViewer;
  v3d_t planetPos = v3d_zero();

  switch (menuChoice) {
  case GAMEMENU_BACKTOGAME:
    break;

  case GAMEMENU_EXITGAME:
    return 2;

  case GAMEMENU_SHIP:
    // ignore if we're already on the ship
    if (mGameModel->mLocation->getType() != LOCATION_SHIP) {
      mGameModel->saveLocation();
      mGameModel->initializeStarShip(false);
      initializeWorldViews();
      // load the appropriate menu
      loadShipMenu();
    }
    break;

  case GAMEMENU_GALAXY_MAP:
    galaxyMap = new GalaxyMap(mGameWindow);
    galaxyMap->enterViewMode(mGameModel->mGalaxy, mGameModel->mCurrentPlanet);

    if (galaxyMap->mResult.action == ACTION_WARP) {
      mGameModel->mCurrentPlanet = galaxyMap->mResult.planet;
      printf("-----------\nwarp to world: %d\n", mGameModel->mCurrentPlanet->mHandle);

      // need to reload this to reflect the change in orbit
      loadShipMenu();

      // change our orbit location
      static_cast<StarShip*>(mGameModel->mLocation)->mOrbitSky->setOrbit(*mGameModel->mGalaxy, mGameModel->mCurrentPlanet->mHandle);
    }
    delete galaxyMap;

    break;

  case GAMEMENU_PLANET_MAP:
    // make a planet map for the player to choose a location
    planetMap = new PlanetMap(mGameWindow);

    if (planetMap->chooseLocation(*mGameModel->mCurrentPlanet, planetPos)) {
      mGameModel->saveLocation();

      // now deal with the new planet
      mGameModel->initializePlanet(false, &planetPos, true, mGameWindow);
      initializeWorldViews();
      // load the appropriate menu
      loadPlanetMenu();

      // HACK - need better timekeeping
      mLastUpdateTime = (double)SDL_GetTicks() / 1000.0;
    }
    delete planetMap;

    break;
  case GAMEMENU_DUNGEON_MAP:
    rogueMapViewer = new RogueMapViewer(mGameWindow);
    rogueMapViewer->startViewer();
    delete rogueMapViewer;
    break;


  case GAMEMENU_MERCHANT:
    mMerchantView = new MerchantView();
    mMerchantView->engageMerchant(*mGameModel->mPlayer, *mGameModel->mItemManager);
    mGameState = GAMESTATE_MERCHANT;
    return 0;

  default:
    return 0;
  }

  mGameState = GAMESTATE_PLAY;
  // capture the mouse
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(0);

  // FIXME: do something to make the GI->elevation and facing delta = 0
  // i don't think this works
  SDL_Event sdlEvent;
  SDL_PollEvent(&sdlEvent);

  return 0;
}

void game_c::handleInput() {
  mPlayerController->update(mGameInput);
}

void game_c::update() {
  double ticks = (double)SDL_GetTicks() / 1000.0;

  while (mLastUpdateTime < ticks) {
    v3d_print("sdf", mGameModel->mPlayer->get_pos());


    mNumPhysicsObjects = mGameModel->mPhysics->update(mLastUpdateTime, mAssetManager);

    mGameModel->mPlayer->update(&mPlayerController->mMovementInput, mAssetManager);
    mPlayerView->update();

    mGameModel->mAiManager->setPlayerFacingAndIncline(mGameModel->mPlayer->getFacingAndIncline());

    mNumAiObjects = mGameModel->mAiManager->update();

    mNumItems = mGameModel->mItemManager->update();

    mLastUpdateTime += PHYSICS_TIME_GRANULARITY;
  }

  mPhysicsView->update(mGameModel->mPhysics->getEntityVector(), mLastUpdateTime);
}

int game_c::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GlCamera camera = mPlayerView->glCamSetup();

  mPhysicsView->setViewPosition(camera.getPosition());

  mGameModel->mLocation->drawEnvironment(camera);

  mWorldMapView->drawSolidBlocks(camera);
  mPhysicsView->drawSolidEntities(mGameModel->mPhysics->getEntityVector(), *mGameModel->mLocation->getWorldMap(), mAssetManager);
  mPlayerView->drawPlayerTargetBlock();
  mPlayerView->drawEquipped();
  mAiView->draw(mGameModel);

  // draw the transparent physics objs
  bool playerHeadInWater = mGameModel->mPlayer->isHeadInWater();
  mPhysicsView->drawTransparentEntities(mGameModel->mPhysics->getEntityVector(), mAssetManager, !playerHeadInWater);
  mWorldMapView->drawLiquidBlocks(camera);
  mPhysicsView->drawTransparentEntities(mGameModel->mPhysics->getEntityVector(), mAssetManager, playerHeadInWater);

  mPlayerView->drawHud();

  if (mGameState == GAMESTATE_MENU) {
    mMenu->draw();
  }
  if (mGameState == GAMESTATE_MERCHANT) {
    mMerchantView->draw();
  }

  mGameWindow->swapBuffers();

  // don't wanna just leave that sitting there
//  glFlush ();

  return 0; //blocks_drawn;
}

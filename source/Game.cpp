#include "Game.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)



game_c::game_c(GameWindow* gameWindow) :
  mGalaxy(NULL),
  mAiView(NULL),
  mPhysicsView(NULL),
  mMenu(NULL),
  mMerchantView(NULL)
{
  printf("game_c constructor ----------------\n");
  mLastUpdateTime = 0.0;

  mGameWindow = gameWindow;

  printf("loading assets\n");
  mAssetManager.loadAssets();
  printf("\n%6d: assets loaded\n", SDL_GetTicks());

  mGameModel = new GameModel;

  mPlayer = new player_c(mGameModel);
  mGameModel->player = mPlayer;

  mGalaxy = new Galaxy();
  mGameModel->galaxy = mGalaxy;

  mPhysics = new Physics(mGameModel);
  mGameModel->physics = mPhysics;

  mAiManager = new AiManager(mGameModel);
  mGameModel->aiManager = mAiManager;

  mItemManager = new ItemManager(mGameModel);
  mGameModel->itemManager = mItemManager;

  mGameInput = new GameInput;
  mGameModel->gameInput = mGameInput;
}



game_c::~game_c() {

  if (mGameInput != NULL) {
    delete mGameInput;
  }
  if (mAiManager != NULL) {
    delete mAiManager;
  }
  if (mItemManager != NULL) {
    delete mItemManager;
  }
  if (mPhysics != NULL) {
    delete mPhysics;
  }
  if (mPlayer != NULL) {
    delete mPlayer;
  }
  if (mGalaxy != NULL) {
    delete mGalaxy;
  }
  if (mAiView != NULL) {
    delete mAiView;
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

  printf("you must set my assets free!\n");
  mAssetManager.freeAssets();
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

  mMenu->addButton(v2d_v (0.80, 0.15), v2d_v (0.2, 0.1), fontSize, "back to game", TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
  mMenu->addButton(v2d_v (0.80, 0.3), v2d_v (0.2, 0.1), fontSize, "quit to menu", TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);

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

  if (mGameModel->currentPlanet != NULL) {
    sprintf(mPlanetNameString, "orbiting planet: %d", mGameModel->currentPlanet->mHandle);
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

void game_c::setup_opengl() {
  glViewport(0, 0, SCREEN_W, SCREEN_H);

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
int game_c::enter_game_mode(bool createNewWorld) {
  printf ("\n%6d: entered GAME mode ----------------\n", SDL_GetTicks());

  // create a new game or load an old one
  if (createNewWorld) {
    printf("game_c::enter_game_mode(): new game\n");
    FileSystem::deleteAllFilesInFolder(SAVE_FOLDER);

    mGameModel->currentPlanet = mGalaxy->mStarSystems[0]->mPlanets[0];
    mGameModel->initializeSpaceShip(true);
  }
  else {
    printf("game_c::enter_game_mode(): loading game\n");
    mGameModel->load(mGameWindow);
  }

  // load the appropriate menu for the location
  if (mGameModel->location->getType() == LOCATION_SHIP) {
    loadShipMenu();
  }
  else {
    loadPlanetMenu();
  }

  initializeWorldViews();

  // grab the cursor
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(0);

  setup_opengl();

  mAssetManager.mSoundSystem.initialize();
  mAssetManager.mSoundSystem.loadSound("cow");
//  mAssetManager.mSoundSystem.playSoundByHandle (SOUND_AMBIENT, 64);

  gameLoop();

  // FIXME: this is temporary ... since we aren't
  // saving the AI, we need to eliminate their items
  mGameModel->destroyItemsOwnedByPhysicsAndAi();

  // save the current game state
  mGameModel->save();

  // DONE WITH THIS ROUND FOLKS * * * * * * *
  mAiManager->clear();
  mItemManager->clear();
  mPhysics->clear();

  mAssetManager.mSoundSystem.stopAllSounds();

  printf("%6d: exiting GAME mode ----------------\n\n", SDL_GetTicks());
  printf("world seed: %d\n", mGameModel->worldSeed);

  return 0;
}

void game_c::initializeWorldViews() {
  // we need to give it an ambient light color
  printf("PLANET: %d\n", mGameModel->currentPlanet->mHandle);

  IntColor sunColor;
  if (mGameModel->location->getType() == LOCATION_SHIP && false) {
    sunColor.r = LIGHT_LEVEL_MAX;
    sunColor.g = LIGHT_LEVEL_MAX;
    sunColor.b = LIGHT_LEVEL_MAX;
  }
  else {
    GLfloat* starColor = mGameModel->galaxy->getStarSystemByHandle(mGameModel->currentPlanet->mHandle)->mStarColor;
    sunColor.r = (int)((starColor[0] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.g = (int)((starColor[1] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.b = (int)((starColor[2] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);
  }

  mWorldMapView.setWorldMap(mGameModel->location->getWorldMap(), sunColor);

  // physics
  if (mPhysicsView != NULL) {
    delete mPhysicsView;
  }
  mPhysicsView = new PhysicsView();

  // ai
  if (mAiView != NULL) {
    delete mAiView;
  }
  mAiView = new AiView();
}

void game_c::gameLoop() {
  printf("game_c::gameLoop() - begin\n");

  unsigned int ticks = SDL_GetTicks();
  int frame = 0;
  int blocks_drawn = 0;

  mLastUpdateTime = (double)ticks / 1000.0;

  mGameState = GAMESTATE_PLAY;

  int quit = 0;
  bool escapePressed = false;

  // the main loop
  while (quit == 0) {
    // advance the state of the game model
    escapePressed = update();

    // draw after updates are all completed
    draw();

    // take care of the input/menu
    if (mGameState == GAMESTATE_PLAY) {
      mGameInput->update();

      if (escapePressed) {
        quit = 1;
      }
    }
    else if (mGameState == GAMESTATE_MENU) {
      int menuChoice = mMenu->GameMenuhoice(false);
      if (escapePressed) {
        printf("escape\n");
        menuChoice = GAMEMENU_BACKTOGAME;
      }
      quit = handleMenuChoice(menuChoice);
    }
    else if (mGameState == GAMESTATE_MERCHANT) {
      if (mMerchantView->update(*mPlayer, *mItemManager) != 0) {
        delete mMerchantView;
        mMerchantView = NULL;
        mGameState = GAMESTATE_MENU;
      }
    }

    // player hit 'esc' in play mode, switch to menu
    if (quit == 1) {
      mGameState = GAMESTATE_MENU;
      // release the mouse
      SDL_SetRelativeMouseMode(SDL_FALSE);
      SDL_ShowCursor(1);
      quit = 0;
    }

    // update the world
    // FIXME: this should be done in update ()
    // being done here ties it to the framerate
    mGameModel->location->update(mPhysics->getCenter(mGameModel->physics->getPlayerHandle()));

    // HACK * * * * * * *
    mPlayer->placeLight();

    mWorldMapView.update(mAssetManager, *mGameModel->location->getLightManager());

    if (mGameInput->isToggleWorldChunkBoxes()) {
      mWorldMapView.toggleShowWorldChunkBoxes();
    }


    // do some frames per second calculating
    unsigned int delta_t = SDL_GetTicks() - ticks;
    frame++;

    if (delta_t >= 5000 && OUTPUT_FRAME_STATS) {
      printf("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
      printf("avg frame: %10.3fms\n", (double)delta_t / (double)frame);
      frame = 0;
      blocks_drawn = 0;
      ticks = SDL_GetTicks ();

      printf("num phys: %d\n", mNumPhysicsObjects);
      printf("num ai: %d\n", mNumAiObjects);
      printf("num items: %d\n\n", mNumItems);

    }

    // rescue single processor machines
//    SDL_Delay(1);
  }

  printf("game_c::gameLoop() - done\n");
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
    if (mGameModel->location->getType() != LOCATION_SHIP) {
      mGameModel->saveLocation();
      mGameModel->initializeSpaceShip(false);
      initializeWorldViews();
      // load the appropriate menu
      loadShipMenu();
    }
    break;

  case GAMEMENU_GALAXY_MAP:
    galaxyMap = new GalaxyMap(mGameWindow);
    galaxyMap->enterViewMode(mGalaxy, mGameModel->currentPlanet);

    if (galaxyMap->mResult.action == ACTION_WARP) {
      mGameModel->currentPlanet = galaxyMap->mResult.planet;
      printf("-----------\nwarp to world: %d\n", mGameModel->currentPlanet->mHandle);

      // need to reload this to reflect the change in orbit
      loadShipMenu();

      // change our orbit location
      static_cast<StarShip*>(mGameModel->location)->mOrbitSky->setOrbit(*mGalaxy, mGameModel->currentPlanet->mHandle);
    }
    delete galaxyMap;

    break;

  case GAMEMENU_PLANET_MAP:
    // make a planet map for the player to choose a location
    planetMap = new PlanetMap(mGameWindow);

    if (planetMap->chooseLocation(*mGameModel->currentPlanet, planetPos)) {
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
    mMerchantView->engageMerchant(*mPlayer, *mItemManager);
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


// update the game model
bool game_c::update() {
  double cur_time = (double)SDL_GetTicks() / 1000.0;
  bool escapePressed = false;

  while (mLastUpdateTime < cur_time) {
    // update the physics objects
    mNumPhysicsObjects = mPhysics->update(mLastUpdateTime, mAssetManager);

    // apply the player physics/update with input
    escapePressed = mPlayer->update(mAssetManager) || escapePressed;
    // FIXME: this is done like this so that the player
    // can catch an escape press (i.e. to get out of the
    // character sheet). this should be done differently
    if (escapePressed) {
      return escapePressed;
    }

    mAiManager->setPlayerFacingAndIncline(mPlayer->getFacingAndIncline());

    mNumAiObjects = mAiManager->update();
    mNumItems = mItemManager->update();

    mLastUpdateTime += PHYSICS_TIME_GRANULARITY;
  }

  // this updates colors/transparencies...
  mPhysicsView->update(mPhysics->getEntityVector(), mLastUpdateTime);

  return escapePressed;
}


// draw the game world
int game_c::draw() {
  // clear the buffer before drawing to it
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // get the camera from the player's perspective
  gl_camera_c cam = mPlayer->gl_cam_setup();

  // we need this for the billboard sprites
  mPhysicsView->setViewPosition(cam.getPosition());

  // draw the stars and stuff
  mGameModel->location->draw(cam);

  mWorldMapView.drawSolidBlocks(cam, mAssetManager);
  mPhysicsView->drawSolidEntities(mPhysics->getEntityVector(), *mGameModel->location->getWorldMap(), mAssetManager);
  PlayerView::drawPlayerTargetBlock(mPlayer);
  PlayerView::drawEquipped(mGameModel, mAssetManager);
  mAiView->draw(mGameModel);

  // draw the transparent physics objs
  bool playerHeadInWater = mPlayer->isHeadInWater();
  mPhysicsView->drawTransparentEntities(mPhysics->getEntityVector(), mAssetManager, !playerHeadInWater);
  mWorldMapView.drawLiquidBlocks(cam, mAssetManager);
  mPhysicsView->drawTransparentEntities(mPhysics->getEntityVector(), mAssetManager, playerHeadInWater);

  // draw the hud
  mPlayer->drawHud();

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

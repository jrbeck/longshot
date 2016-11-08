#include "../game/Game.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)

game_c::game_c(GameWindow* gameWindow) :
  mLastUpdateTime(0.0),
  mAssetManager(NULL),
  mGameModel(NULL),
  mGameView(NULL),
  mMerchantView(NULL),
  mPlayerController(NULL),
  mGameWindow(gameWindow)
{
  printf("game_c::game_c ----------------\n");
}

game_c::~game_c() {
  printf("game_c::~game_c ----------------\n");
  if (mGameInput != NULL) {
    delete mGameInput;
  }
  if (mGameView != NULL) {
    delete mGameView;
  }
  if (mMerchantView != NULL) {
    delete mMerchantView;
  }
  if (mPlayerController != NULL) {
    delete mPlayerController;
  }
  if (mGameModel != NULL) {
    delete mGameModel;
  }
  if (mAssetManager != NULL) {
    delete mAssetManager;
  }
}

int game_c::run(bool createNewWorld) {
  printf ("\n%6d: entered GAME mode ----------------\n", SDL_GetTicks());

  setup(createNewWorld);
  gameLoop();
  teardown();

  printf("%6d: exiting GAME mode ----------------\n\n", SDL_GetTicks());
  return 0;
}

void game_c::setup(bool createNewWorld) {
  mGameInput = new GameInput;
  mGameModel = new GameModel;

  mAssetManager = new AssetManager;
  mAssetManager->loadAssets();
  mAssetManager->mSoundSystem.initialize();
  mAssetManager->mSoundSystem.loadSounds();
  // mAssetManager->mSoundSystem.playSoundByHandle(SOUND_AMBIENT, 64);

  mGameView = new GameView(mGameModel, mAssetManager);

  if (createNewWorld) {
    printf("game_c::enterGameMode(): new game\n");
    FileSystem::deleteAllFilesInFolder(SAVE_FOLDER);

    mGameModel->mCurrentPlanet = mGameModel->mGalaxy->mStarSystems[0]->mPlanets[0];
    mGameModel->initializeStarShip(true);
    mGameView->initializeForLocation();

    XXXpostLoactionInitializeSetup();
  }
  else {
    printf("game_c::enterGameMode(): loading game\n");
    mGameModel->load(mGameWindow);
  }
}

void game_c::teardown() {
  mAssetManager->mSoundSystem.stopAllSounds();

  delete mGameView;
  mGameView = NULL;

  // FIXME: this is temporary ... since we aren't
  // saving the AI, we need to eliminate their items
  mGameModel->destroyItemsOwnedByPhysicsAndAi();

  mGameModel->save();
  delete mGameModel;
  mGameModel = NULL;
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
    updateControllers();
    mGameModel->mPlayer->applyMovementInput(mPlayerController->getMovementInput(), mAssetManager);

    // FIXME: this masks the doing of interface stuff in the PlayerController
    escapePressed = mGameInput->isEscapePressed();
    mGameModel->mAiManager->setPlayerFacingAndIncline(mGameModel->mPlayer->getFacingAndIncline());

    update();

    printf("after game view\n");
    mGameView->update(mLastUpdateTime);
    printf("after game view\n");
    mGameView->draw(mGameState, mMerchantView);
    printf("after game view\n");

    // should this be in GameView??
    mGameWindow->swapBuffers();

    if (mGameState == GAMESTATE_PLAY) {
      mGameInput->update();
    }
    else if (mGameState == GAMESTATE_MENU) {
      int menuChoice = mGameView->getMenuChoice();
      if (escapePressed) {
        printf("escape\n");
        menuChoice = GAMEMENU_BACKTOGAME;
      }
      quit = handleMenuChoice(menuChoice);
    }
    else if (mGameState == GAMESTATE_MERCHANT) {
      if (mMerchantView->update() != 0) {
        delete mMerchantView;
        mMerchantView = NULL;
        mGameState = GAMESTATE_MENU;
      }
    }

    // player hit 'esc' in play mode, switch to menu
    if (escapePressed && mGameState == GAMESTATE_PLAY) {
      mGameState = GAMESTATE_MENU;
      SDL_SetRelativeMouseMode(SDL_FALSE);
      SDL_ShowCursor(1);
    }

    // update the world
    // FIXME: this should be done in update()
    // being done here ties it to the framerate
    mGameModel->mLocation->update(mGameModel->mPhysics->getCenter(mGameModel->mPhysics->getPlayerHandle()));

    // HACK * * * * * * *
    mGameModel->mPlayer->placeLight(mGameInput);

    if (mGameInput->isToggleWorldChunkBoxes()) {
      mGameView->toggleShowWorldChunkBoxes();
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
    if (mGameModel->mLocation->getType() != LOCATION_SHIP) {
      mGameModel->saveLocation();
      mGameModel->initializeStarShip(false);
      mGameView->initializeForLocation();
      XXXpostLoactionInitializeSetup();
    }
    break;

  case GAMEMENU_GALAXY_MAP:
    galaxyMap = new GalaxyMap(mGameWindow);
    galaxyMap->enterViewMode(mGameModel->mGalaxy, mGameModel->mCurrentPlanet);

    if (galaxyMap->mResult.action == ACTION_WARP) {
      mGameModel->mCurrentPlanet = galaxyMap->mResult.planet;
      printf("-----------\nwarp to world: %d\n", mGameModel->mCurrentPlanet->mHandle);

      // need to reload this to reflect the change in orbit
      mGameView->loadShipMenu();

      static_cast<StarShip*>(mGameModel->mLocation)->mOrbitSky->setOrbit(*mGameModel->mGalaxy, mGameModel->mCurrentPlanet->mHandle);
    }
    delete galaxyMap;
    break;

  case GAMEMENU_PLANET_MAP:
    planetMap = new PlanetMap(mGameWindow);

    if (planetMap->chooseLocation(*mGameModel->mCurrentPlanet, planetPos)) {
      mGameModel->saveLocation();
      mGameModel->initializePlanet(false, &planetPos, true, mGameWindow);
      mGameView->initializeForLocation();
      XXXpostLoactionInitializeSetup();
    }
    delete planetMap;
    break;

  case GAMEMENU_DUNGEON_MAP:
    rogueMapViewer = new RogueMapViewer(mGameWindow);
    rogueMapViewer->startViewer();
    delete rogueMapViewer;
    break;

  case GAMEMENU_MERCHANT:
    mMerchantView = new MerchantView(mGameModel->mPlayer, mGameModel->mItemManager);
    mMerchantView->engageMerchant();
    mGameState = GAMESTATE_MERCHANT;
    return 0;

  default:
    return 0;
  }

  mGameState = GAMESTATE_PLAY;
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(0);

  // FIXME: do something to make the GI->elevation and facing delta = 0
  // i don't think this works
  SDL_Event sdlEvent;
  SDL_PollEvent(&sdlEvent);

  return 0;
}

void game_c::updateControllers() {
  mPlayerController->update(mGameInput);
}

void game_c::update() {
  double ticks = (double)SDL_GetTicks() / 1000.0;

  while (mLastUpdateTime < ticks) {
    mNumPhysicsObjects = mGameModel->mPhysics->update(mLastUpdateTime, mAssetManager);
    mNumAiObjects = mGameModel->mAiManager->update();
    mNumItems = mGameModel->mItemManager->update();

    mLastUpdateTime += PHYSICS_TIME_GRANULARITY;
  }
}


void game_c::XXXpostLoactionInitializeSetup() {
  // TODO: move this into GameView?
  mGameWindow->swapBuffers();

  // FIXME: this is stupid
  if (mPlayerController != NULL) {
    delete mPlayerController;
  }
  printf("making PC * * * * * * * * * * * * * * * * * * * * * *\n");

  mPlayerController = new PlayerController(mGameModel, mGameView->mPlayerView);

  // HACK - need better timekeeping
  mLastUpdateTime = (double)SDL_GetTicks() / 1000.0;

}

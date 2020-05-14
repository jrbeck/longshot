#include "GameView.h"


GameView::GameView(GameModel *gameModel, AssetManager* assetManager, GameWindow* gameWindow) :
  mGameModel(gameModel),
  mAssetManager(assetManager),
  mGameWindow(gameWindow),
  mPlayerView(NULL),
  mAiView(NULL),
  mWorldMapView(NULL),
  mPhysicsView(NULL),
  mMenu(NULL),
  mUpVector(v3d_v(0, 1, 0))
{
  mCamera.resize_screen(gScreenW, gScreenH);
  mCamera.set_fov_near_far(45.0, 0.15, 500.0);
  setDrawDistance(500.0);
}

GameView::~GameView() {
  if (mPlayerView != NULL) {
    delete mPlayerView;
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
}

void GameView::initializeForLocation() {
  if (mGameModel->mLocation->getType() == LOCATION_SHIP) {
    loadShipMenu();
  }
  else {
    loadPlanetMenu();
  }

  initializeWorldViews();

  // grab the cursor
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(0);

  setupOpenGl();

  mGameWindow->swapBuffers();

}

void GameView::update(double lastUpdateTime, GameInput* gameInput) {
  if (gameInput->isDecreasingDrawDistance()) {
    adjustDrawDistance(-20);
  }
  if (gameInput->isIncreasingDrawDistance()) {
    adjustDrawDistance(20);
  }

  mWorldMapView->update(*mGameModel->mLocation->getLightManager());
  mPhysicsView->update(mGameModel->mPhysics->getEntityVector(), lastUpdateTime);
  mPlayerView->update();
}

void GameView::draw(int gameState, CharacterSheetView* characterSheetView, MerchantView* merchantView) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCamSetup();

  mPhysicsView->setViewPosition(mCamera.getPosition());

  mGameModel->mLocation->drawEnvironment(mCamera);

  mWorldMapView->drawSolidBlocks(mCamera);
  mPhysicsView->drawSolidEntities(mGameModel->mPhysics->getEntityVector(), *mGameModel->mLocation->getWorldMap(), mAssetManager);
  mPlayerView->drawPlayerTargetBlock();
  mPlayerView->drawEquipped();
  mAiView->draw(mGameModel);

  // draw the transparent physics objs
  bool playerHeadInWater = mGameModel->mPlayer->isHeadInWater();
  mPhysicsView->drawTransparentEntities(mGameModel->mPhysics->getEntityVector(), mAssetManager, !playerHeadInWater);
  mWorldMapView->drawLiquidBlocks(mCamera);
  mPhysicsView->drawTransparentEntities(mGameModel->mPhysics->getEntityVector(), mAssetManager, playerHeadInWater);

  mPlayerView->drawHud();
  if (mGameModel->mPlayer->isDisplayingCharacterSheet()) {
    characterSheetView->draw();
  }

  if (gameState == GAMESTATE_MENU) {
    mMenu->draw();
  }
  if (gameState == GAMESTATE_MERCHANT) {
    // TODO: this is pretty gross, but until i have a better feel for
    // how i want this all to work, it goes here
    merchantView->draw();
  }
}

int GameView::getMenuChoice() {
  return mMenu->gameMenuChoice(false);
}

void GameView::toggleShowWorldChunkBoxes() {
  mWorldMapView->toggleShowWorldChunkBoxes();
}

void GameView::loadPlanetMenu() {
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

void GameView::loadShipMenu() {
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

void GameView::setupOpenGl() {
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

  printf("\n%6d: opengl_setup done ----------------\n", SDL_GetTicks());
}

void GameView::initializeWorldViews() {
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

void GameView::setDrawDistance(double distance) {
  mCamera.set_far(distance);
}

void GameView::adjustDrawDistance(double amount) {
  mCamera.adjust_far(amount);
}

GlCamera GameView::glCamSetup() {
  mCamera.perspective();
  mCamera.look_at(mGameModel->mPlayer->getHeadPosition(), mGameModel->mPlayer->getLookTarget(), mUpVector);
  return mCamera;
}

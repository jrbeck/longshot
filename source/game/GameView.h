#pragma once

#include "../game/Constants.h"
#include "../game/GameWindow.h"
#include "../player/PlayerView.h"
#include "../world/WorldMapView.h"
#include "../ai/AiView.h"
#include "../physics/PhysicsView.h"
#include "../items/MerchantView.h"
#include "../assets/AssetManager.h"

class GameView {
public:
  GameView(GameModel *gameModel, AssetManager* assetManager, GameWindow* gameWindow);
  ~GameView();

  void initializeForLocation();
  void update(double lastUpdateTime, GameInput* gameInput);
  void draw(int gameState, CharacterSheetView* characterSheetView, MerchantView* merchantView);

  int getMenuChoice();

  // TODO: this should be a part of a state stored somewhere else
  void toggleShowWorldChunkBoxes();

  void loadPlanetMenu();
  void loadShipMenu();

  void setDrawDistance(double distance);
  void adjustDrawDistance(double amount);

  GlCamera glCamSetup();

private:
  void setupOpenGl();

  void initializeWorldViews();

  GlCamera mCamera;

  GameModel* mGameModel;
  AssetManager* mAssetManager;
  GameWindow* mGameWindow;

  GameMenu* mMenu;

  PlayerView* mPlayerView;
  AiView* mAiView;
  WorldMapView* mWorldMapView;
  PhysicsView* mPhysicsView;

  // HACKS
  char mPlanetNameString[100];
  v3d_t mUpVector;
};

#pragma once

#include "../game/Constants.h"
#include "../game/GameWindow.h"
#include "../player/PlayerView.h"
#include "../player/CharacterSheetView.h"
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
  void update(double lastUpdateTime);
  void draw(int gameState, MerchantView* merchantView);

  int getMenuChoice();

  // TODO: this should be a part of a state stored somewhere else
  void toggleShowWorldChunkBoxes();

  void loadPlanetMenu();
  void loadShipMenu();

  // HACK - hide this ... PlayerController wants it for some reason
  PlayerView* mPlayerView;
private:
  void setupOpenGl();

  void initializeWorldViews();

  GameModel* mGameModel;
  AssetManager* mAssetManager;
  GameWindow* mGameWindow;

  GameMenu* mMenu;

  CharacterSheetView* mCharacterSheetView;
  AiView* mAiView;
  WorldMapView* mWorldMapView;
  PhysicsView* mPhysicsView;

  // HACKS
  char mPlanetNameString[100];
};

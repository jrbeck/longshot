// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * game_c
// *
// * this contains the main game loop
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

// the main include directory
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <assert.h>

//#include <winbase.h>
#include <cstring>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "../vendor/GL/glut.h"

// the root/code/include directory
#include "../math/v2d.h"
#include "../game/FileSystem.h"
#include "../game/GameModel.h"
#include "../game/GameWindow.h"
#include "../world/GalaxyMap.h"
#include "../world/WorldUtil.h"
#include "../player/PlayerView.h"
#include "../world/WorldMapView.h"
#include "../ai/AiView.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsView.h"
#include "../game/GameInput.h"
#include "../assets/AssetManager.h"
#include "../items/MerchantView.h"

// let's try to get rid of includes below here...
#include "../dungeon/RogueMapViewer.h"

#define SAVE_FOLDER "save"

#define OUTPUT_FRAME_STATS (false)

enum {
  GAMESTATE_PLAY,
  GAMESTATE_MENU,
  GAMESTATE_MERCHANT
};

enum {
  GAMEMENU_BACKTOGAME,
  GAMEMENU_EXITGAME,

  GAMEMENU_SHIP,
  GAMEMENU_GALAXY_MAP,
  GAMEMENU_PLANET_MAP,

  GAMEMENU_DUNGEON_MAP,

  GAMEMENU_MERCHANT
};

class game_c {
  GameModel* mGameModel;
  GameInput* mGameInput;

  AssetManager* mAssetManager;

  PlayerView* mPlayerView;
  AiView* mAiView;
  WorldMapView* mWorldMapView;
  PhysicsView* mPhysicsView;
  MerchantView* mMerchantView;

  double mLastUpdateTime;

  GameMenu* mMenu;
  int mGameState;

  int mNumPhysicsObjects;
  int mNumAiObjects;
  int mNumItems;

  GameWindow* mGameWindow;

  // HACKS
  char mPlanetNameString[100];

public:
  game_c(GameWindow* gameWindow);
  ~game_c();

  void loadPlanetMenu();
  void loadShipMenu();

  GameWindow* getGameWindow();
  void setupOpenGl();

  int enterGameMode(bool createNewWorld);

  void initializeWorldViews();

  void gameLoop();
  int handleMenuChoice(int menuChoice);
  bool update();
  int draw();
};

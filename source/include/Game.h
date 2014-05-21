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

using namespace std;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/glut.h"

// the root/code/include directory
#include "v2d.h"

#include "GameModel.h"

#include "GameWindow.h"
#include "GalaxyMap.h"
#include "WorldUtil.h"
#include "WorldMapView.h"
#include "AiView.h"
#include "physics.h"
#include "PhysicsView.h"
#include "GameInput.h"
#include "AssetManager.h"
#include "MerchantView.h"

// let's try to get rid of stuff below here...
#include "RogueMapViewer.h"

#define OUTPUT_FRAME_STATS		false


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
  GameModel *mGameModel;

  player_c* mPlayer;
  Galaxy* mGalaxy;
//  Location* mLocation;
  AiManager* mAiManager;
  Physics* mPhysics;
  GameInput* mGameInput;
  ItemManager* mItemManager;

  AssetManager mAssetManager;

  AiView* mAiView;
  WorldMapView mWorldMapView;
  PhysicsView* mPhysicsView;
  MerchantView* mMerchantView;

//  size_t mPlayerAiHandle;

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
  void setup_opengl();

  int enter_game_mode(bool createNewWorld);

  void initializeWorldViews();

  void gameLoop();
  int handleMenuChoice(int menuChoice);
  bool update();
  int draw();

  // this really shouldn't be here...
  // it draws the 'touched' block in front of the player
  // put it in PlayerView
  void drawPlayerTargetBlock();

  // currently Windows specific
  void deleteAllFilesInFolder(LPWSTR folderPath);
};

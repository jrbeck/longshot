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

#include "GameWindow.h"
#include "GalaxyMap.h"
#include "World.h"
#include "WorldUtil.h"
#include "StarShip.h"
#include "WorldMapView.h"
#include "player.h"
#include "AiManager.h"
#include "AiView.h"
#include "physics.h"
#include "PhysicsView.h"
#include "GameInput.h"
#include "AssetManager.h"
#include "ItemManager.h"
#include "feature/FeatureGenerator.h"
#include "MerchantView.h"

// let's try to get rid of stuff below here...
#include "RogueMapViewer.h"




#define OUTPUT_FRAME_STATS		false


typedef struct {
  bool loadSucceeded;
  v3d_t physicsPos;
  int locationType;
  int planetHandle;
} GameSaveData;


enum {
  LOAD_SUCCESSFUL,
  LOAD_UNSUCCESSFUL

};


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
  player_c mPlayer;	// save
  Galaxy *mGalaxy;	// save
  Location *mLocation;	// save
  WorldMap *mWorldMap;
  WorldMapView mWorldMapView;
  AiManager mAiManager;	// save
  AiView *mAiView;
  Physics mPhysics;	// save
  PhysicsView *mPhysicsView;
  GameInput mGameInput;
  AssetManager mAssetManager;
  ItemManager mItemManager;	// save

  MerchantView *mMerchantView;

  size_t mPlayerPhysicsHandle;
  size_t mPlayerAiHandle;

  double mLastUpdateTime;

  GameMenu *mMenu;
  int mGameState;

  int mNumPhysicsObjects;
  int mNumAiObjects;
  int mNumItems;

  int mWorldSeed;
  Planet *mCurrentPlanet;

  bool mCycleLighting;

  GameWindow *mGameWindow;

  // HACKS
  char mPlanetNameString[100];

public:
  game_c(GameWindow *gameWindow);
  ~game_c();

  void loadAssets();
  void freeAssets();

  void loadPlanetMenu();
  void loadShipMenu();

  GameWindow* getGameWindow();
  void setup_opengl();

  int enter_game_mode(bool createNewWorld);

  void initializePlanet(bool resetPlayer, Planet* planet, v3d_t* startPos, bool createSetPieces);
  void initSpaceShip(bool resetPlayer);
  void resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer);

  void gameLoop();
  int handleMenuChoice(int menuChoice);
  bool update();
  int draw(double& time);

  // this really shouldn't be here...
  // it draws the 'touched' block in front of the player
  // put it in PlayerView
  void drawPlayerTargetBlock();

  // currently Windows specific
  void deleteAllFilesInFolder(LPWSTR folderPath);

  void save();
  int load();
  int saveGameData();
  GameSaveData loadGameData();
  void saveLocation();
};

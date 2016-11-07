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

#include "../math/v2d.h"
#include "../game/Constants.h"
#include "../game/FileSystem.h"
#include "../game/GameInput.h"
#include "../game/GameModel.h"
#include "../game/GameView.h"
#include "../game/GameWindow.h"
#include "../items/MerchantView.h"
#include "../world/GalaxyMap.h"
#include "../world/WorldUtil.h"
#include "../player/PlayerController.h"
#include "../physics/Physics.h"
#include "../assets/AssetManager.h"

// let's try to get rid of includes below here...
#include "../dungeon/RogueMapViewer.h"

#define SAVE_FOLDER "save"

#define OUTPUT_FRAME_STATS (false)

class game_c {
public:
  game_c(GameWindow* gameWindow);
  ~game_c();

  int run(bool createNewWorld);

private:
  void setup(bool createNewWorld);
  void teardown();

  void gameLoop();
  int handleMenuChoice(int menuChoice);
  void updateControllers();
  void update();
  int draw();

  void XXXpostLoactionInitializeSetup();

  GameModel* mGameModel;
  GameInput* mGameInput;
  GameView* mGameView;
  MerchantView* mMerchantView;

  AssetManager* mAssetManager;

  PlayerController* mPlayerController;

  double mLastUpdateTime;

  int mGameState;

  int mNumPhysicsObjects;
  int mNumAiObjects;
  int mNumItems;

  GameWindow* mGameWindow;
};

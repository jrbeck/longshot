// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Longshot
// *
// * this is the root of the (...problem) program
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

// longshot constants * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define SCREEN_ASPECT_RATIO    ((double)gScreenW / (double)gScreenH)

#include "../assets/AssetManager.h"
#include "../game/FileSystem.h"
#include "../game/Game.h"
#include "../game/GameMenu.h"
#include "../math/PseudoRandom.h"
#include "../world/RogueViewer.h"
#include "../world/GalaxyMap.h"
#include "../world/PlanetMap.h"
#include "../world/World.h"

// Longshot * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class Longshot {
public:
  Longshot();
  ~Longshot();

  void reloadMenu();
  int loop();

private:
  enum {
    PROGRAM_MODE_MENU,
    PROGRAM_MODE_NEW_GAME,
    PROGRAM_MODE_LOAD_GAME,
    PROGRAM_MODE_TOGGLE_FULLSCREEN,
    PROGRAM_MODE_ROGUE,
    NUM_PROGRAM_MODES,
    PROGRAM_QUIT
  };

  enum {
    MENU_ENTER_GAME_MODE,
    MENU_QUIT
  };

  GameWindow* mGameWindow;
  GameMenu* mMainMenu;
  game_c* mGame;
  RogueViewer* mRogueViewer;

  int mProgramMode;
  bool mConstructorSuccessful;
};

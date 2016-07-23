// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PlanetMap
// *
// * this allows the user to view a planet map
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../game/GameWindow.h"
#include "../game/GameMenu.h"
#include "../math/Periodics.h"
#include "../math/Rectangle2d.h"
#include "../world/Planet.h"

#define PLANET_MAP_SIDE      512
#define MAP_MULTIPLIER      128


class PlanetMap {
public:
  PlanetMap(GameWindow* gameWindow);
  ~PlanetMap(void);

  void setUpOpenGl(void);

  // this method enters the interaction mode and returns the result
  bool chooseLocation(Planet &planet, v3d_t &planetPos);

  void drawMap(void);

  void buildFromPeriodics(int seed);

  // TODO: get this i/o junk outta here
  int handleInput (void);
  int handleKeystroke (void);
  int handleKeyup (void);
  void handleMouseButtonDown (int button, v2d_t pos);
  void handleMouseButtonUp (int button, v2d_t pos);

  void drawTerrain(void) const;

// MEMBERS
  GameWindow* mGameWindow;

  SDL_Event sdlevent;
  v2d_t mMousePos;
  v2d_t mMouseDelta;
  int mMouseMoved;
  bool mLeftMouseButtonClicked;

  GameMenu *mMenu;

  Terrain *mTerrain;
  v3d_t *mColors;
  Periodics *mPeriodics;

  Rectangle2d mViewRect;
};

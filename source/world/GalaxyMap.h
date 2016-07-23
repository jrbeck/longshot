// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GalaxyMap
// *
// *
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../game/GameWindow.h"
#include "../world/Galaxy.h"
#include "../world/PlanetMap.h"

#define ZOOM_GALAXY      0
#define ZOOM_SYSTEM      1

#define ACTION_NONE      0
#define ACTION_WARP      1

struct GalaxyMapResult {
  int action;
  StarSystem* starSystem;
  Planet* planet;
};

class GalaxyMap {
public:
  GalaxyMap(GameWindow* gameWindow);
  ~GalaxyMap();

  GalaxyMapResult enterViewMode(Galaxy* galaxy, Planet* selectedPlanet);

  void drawGalaxy(Planet* selectedPlanet);
  void drawStarSystem(StarSystem& starSystem, Planet* selectedPlanet);

  void updateInfoMessage(StarSystem* currentSystem, StarSystem* hoverSystem);
  void updateGalaxyMenu(StarSystem* currentSystem, StarSystem* hoverSystem);
  void updateStarSystemMenu();

  void setUpOpenGl();
  void drawHex(float radius, v2d_t center, const GLfloat color[4]);
  void drawRing(float radius, v2d_t center, const GLfloat color[4]);

  GameWindow* mGameWindow;

  Galaxy* mGalaxy;

  // TODO: get this i/o junk outta here
  int handleInput();
  int handleKeystroke();
  int handleKeyup();
  void handleMouseButtonDown(int button, v2d_t pos);
  void handleMouseButtonUp(int button, v2d_t pos);

  SDL_Event sdlevent;
  v2d_t mMousePos;
  v2d_t mMouseDelta;
  int mMouseMoved;
  bool mLeftMouseButtonClicked;

  GameMenu* mMenu;

  int mZoomLevel;
  int mSelectedSystem;
  char mMessage[256];

  GalaxyMapResult mResult;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * RogueMapViewer
// *
// * this allows the user to view a RogueMap
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "GameWindow.h"
#include "GameMenu.h"
#include "Rectangle2d.h"
#include "RogueMap.h"

#include "dungeon/DungeonUtil.h"
#include "dungeon/MoleculeDungeon.h"


#define ROGUE_MAP_SIDE      128


class RogueMapViewer {
public:
  RogueMapViewer(GameWindow* gameWindow);
  ~RogueMapViewer();

  void setUpOpenGl();

  // this method enters the interaction mode
  void startViewer();

  void drawMap() const;

  // TODO: get this i/o junk outta here
  int handleInput ();
  int handleKeystroke ();
  int handleKeyup ();
  void handleMouseButtonDown (int button, v2d_t pos);
  void handleMouseButtonUp (int button, v2d_t pos);

  void randomizeDungeon();

// MEMBERS
  GameWindow* mGameWindow;
  GameMenu* mMenu;

  SDL_Event sdlevent;
  v2d_t mMousePos;
  v2d_t mMouseDelta;
  int mMouseMoved;
  bool mLeftMouseButtonClicked;

  RogueMap* mRogueMap;
  Rectangle2d mViewRect;
  DungeonUtil* dungeon;
};

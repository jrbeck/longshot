// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * RogueViewer
// *
// * this allows the user to view landscapes/features/rogue dungeons...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../game/GameWindow.h"
#include "../assets/AssetManager.h"
#include "../dungeon/FeatureGenerator.h"
#include "../world/World.h"
#include "../world/WorldMapView.h"
#include "../math/v2d.h"
#include "../math/RtsCam.h"


struct CameraState {
  v3d_t position;
  v3d_t target;
};

enum {
  VIZ_MODE_MODEL,
  VIZ_MODE_ROGUE,

  NUM_VIZ_MODES,

  VIZ_MODE_PLANET
};


class RogueViewer {
private:
  GameWindow* mGameWindow;
  rts_cam_c mRtsCam;

  SDL_Event sdlevent;
  v2d_t mMousePos;
  v2d_t mMouseDelta;
  int mMouseMoved;

  int mVizMode;

  CameraState mCameraStates[NUM_VIZ_MODES];

  World mWorld;
  WorldMap mWorldMap;
  WorldMapView* mWorldMapView;
  AssetManager* mAssetManager;
  GameModel* mGameModel;
  RogueMap mRogueMap;

public:
  RogueViewer(GameWindow* gameWindow, AssetManager* assetManager, GameModel* gameModel);
  ~RogueViewer();

  void setupOpenGl();

  int start();

  v3d_t findStartPosition(WorldMap& worldMap);

  // TODO: get this i/o junk outta here
  int handleInput();
  int handleKeystroke();
  int handleKeyup();
  void handleMouseButtonDown(int button, v2d_t pos);
  void handleMouseButtonUp(int button, v2d_t pos);
  void handleMouseWeelEvent(SDL_MouseWheelEvent wheelEvent);

  void swapVizMode(int mode);
  void generateNewMap();
};

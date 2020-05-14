// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GameModel
// *
// * desc: a container for the game models
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../game/MessageBus.h"
#include "../items/ItemManager.h"
#include "../player/Player.h"
#include "../world/Galaxy.h"
#include "../world/World.h"
#include "../world/StarShip.h"
#include "../ai/AiManager.h"
#include "../dungeon/FeatureGenerator.h"

enum {
  LOAD_SUCCESSFUL,
  LOAD_UNSUCCESSFUL
};

class Player;
class Galaxy;
class Physics;
class Location;
class AiManager;
class ItemManager;
class GameInput;
class MessageBus;

struct GameSaveData {
  bool loadSucceeded;
  v3d_t physicsPos;
  int locationType;
  int planetHandle;
};

class GameModel {
public:
  GameModel();
  ~GameModel();

  void save();
  int load(GameWindow* gameWindow);

  int saveGameData();
  GameSaveData loadGameData();
  void saveLocation();

  void initializePlanet(bool resetPlayer, v3d_t* startPos, bool createFeatures, GameWindow* gameWindow);
  void initializeStarShip(bool resetPlayer);
  void resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer);

  // TODO: eliminate the need for this
  void destroyItemsOwnedByPhysicsAndAi();

  int mWorldSeed;
  Planet* mCurrentPlanet;

  Player* mPlayer;
  Galaxy* mGalaxy;
  Physics* mPhysics;
  Location* mLocation;
  AiManager* mAiManager;
  ItemManager* mItemManager;
  MessageBus* mMessageBus;

  bool isDisplayingCharacterSheet;
};

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

#include "ItemManager.h"
#include "Player.h"
#include "Galaxy.h"
#include "World.h"
#include "StarShip.h"
#include "AiManager.h"


enum {
  LOAD_SUCCESSFUL,
  LOAD_UNSUCCESSFUL

};



class player_c;
class Galaxy;
class Physics;
class Location;
class AiManager;
class ItemManager;
class GameInput;





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
  int load();

  int saveGameData();
  GameSaveData loadGameData();
  void saveLocation();

  void initializePlanet(bool resetPlayer, Planet* planet, v3d_t* startPos, bool createSetPieces);
  void initSpaceShip(bool resetPlayer);
  void resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer);

  int worldSeed;
  Planet* currentPlanet;

  player_c* player;
  Galaxy* galaxy;
  Physics* physics;
  Location* location;
  AiManager* aiManager;
  ItemManager* itemManager;
  GameInput* gameInput;
};

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

class player_c;
class Galaxy;
class Physics;
class Location;
class AiManager;
class ItemManager;
class GameInput;

class GameModel {
public:
  GameModel();
  ~GameModel();

  player_c* player;
  Galaxy* galaxy;
  Physics* physics;
  Location* location;
  AiManager* aiManager;
  ItemManager* itemManager;
  GameInput* gameInput;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiManager
// *
// * this manages the AiEntity(s). it spawns, manages the list, etc...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#pragma once

#include <vector>

#include <Windows.h>

#include "GL/glut.h"

#include "v3d.h"

#include "GameModel.h"

#include "World.h"
#include "Physics.h"
#include "ItemManager.h"
#include "AiEntity.h"


class AiManager {
public:
  AiManager(GameModel* gameModel);
  ~AiManager();

  void clear();

  size_t setPlayerPhysicsHandle(size_t playerPhysicsHandle, Physics& physics, double time);
  void setPlayerFacingAndIncline(v2d_t facingAndElevation);

  AiEntity* addEntity(int type, const v3d_t& position, Physics& physics, double time);
  void removeEntity(size_t handle);
  AiEntity* getEntityByHandle(size_t handle);
  vector<AiEntity*>* getEntities();

  size_t spawnEntity(int type, v3d_t position, double time, Physics& physics, ItemManager& itemManager);

  int update(double time, WorldMap& worldMap, Physics& physics, ItemManager& itemManager);

  void releaseItems(int index, Physics& physics);
  void trimEntitiesList();

  void readPhysicsMessages(double time, Physics& physics, ItemManager& itemManager);

  vector<size_t> getAllItemHandles();

  void setMaxCritters(int num);

private:
  // copy constructor guard
  AiManager(const AiManager& aiManager) { }
  // assignment operator guard
  AiManager& operator=(const AiManager& aiManager) { return *this; }

  GameModel* mGameModel;

  size_t mNextHandle;

  size_t mPlayerAiHandle;
  size_t mPlayerPhysicsHandle;

  vector<AiEntity*> mAiEntities;

  int mMaxCrittersHACK;
};


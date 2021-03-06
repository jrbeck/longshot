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

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include "../vendor/GL/glut.h"

#include "../math/v3d.h"
#include "../game/GameModel.h"
#include "../game/MessageBus.h"
#include "../world/World.h"
#include "../physics/Physics.h"
#include "../items/ItemManager.h"
#include "../ai/AiEntity.h"

class AiEntity;

class AiManager {
public:
  AiManager(GameModel* gameModel);
  ~AiManager();

  void clear();

  size_t setPlayerPhysicsHandle();
  void setPlayerFacingAndIncline(v2d_t facingAndElevation);

  AiEntity* addEntity(int type, const v3d_t& position);
  void removeEntity(size_t handle);
  AiEntity* getEntityByHandle(size_t handle);
  vector<AiEntity*>* getEntities();

  size_t spawnEntity(int type, v3d_t position);

  int update();

  void releaseItems(int aiIndex);
  void trimEntitiesList();

  void readPhysicsMessages();

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

  vector<AiEntity*> mAiEntities;

  int mMaxCrittersHACK;
};

#include "AiManager.h"

AiManager::AiManager(GameModel* gameModel) {
  mGameModel = gameModel;
  mMaxCrittersHACK = 0;
}



AiManager::~AiManager() {
  clear();
}



void AiManager::clear() {
  mNextHandle = 1;

  mPlayerAiHandle = 0;

  size_t numEntities = mAiEntities.size();
  for (size_t i = 0; i < numEntities; i++) {
    if (mAiEntities[i] != NULL) {
      delete mAiEntities[i];
    }
    else {
      printf("AiManager::clear(): error: NULL AiEntity\n");
    }
    
  }
  mAiEntities.clear();
}




size_t AiManager::setPlayerPhysicsHandle() {
  // add the player as an AI entity
  AiEntity *e = addEntity(AITYPE_PLAYER, v3d_zero());
  if (e == NULL) {
    return 0;
  }

  // HACK: since the caller has already created a phys_entity, and
  // AiManager::addEntity() also creates one, we'll just delete the
  // second one
  mGameModel->physics->removeEntity(e->mPhysicsHandle);
  e->mPhysicsHandle = mGameModel->physics->getPlayerHandle();

  mPlayerAiHandle = e->mHandle;
  return mPlayerAiHandle;
}



void AiManager::setPlayerFacingAndIncline(v2d_t facingAndElevation) {
  AiEntity* e = getEntityByHandle(mPlayerAiHandle);
  if (e == NULL) {
    printf("AiManager::setPlayerFacingAndIncline(): no player entity\n");
    return;
  }

  e->mFacingAngle = facingAndElevation.x;
  e->mLookIncline = facingAndElevation.y;
}


AiEntity* AiManager::addEntity(int type, const v3d_t& position) {
  AiEntity* e = new AiEntity();
  if (e == NULL) {
    printf("AiManager::addEntity(): error: out of mem\n");
    return NULL;
  }

  size_t physicsHandle = mGameModel->physics->createAiEntity(type, position);
  if (physicsHandle == 0) {
    printf("AiManager::addEntity(): error: failed to create phys entity\n");
    return NULL;
  }

  e->mActive = true;
  e->mHandle = mNextHandle++;
  e->mPhysicsHandle = physicsHandle;
  e->mTargetPhysicsHandle = 0;
  e->mType = type;
  e->mWorldPosition = position;

  mAiEntities.push_back(e);

  return e;
}


void AiManager::removeEntity(size_t handle) {
  size_t mNumEntities = mAiEntities.size();
  for (size_t i = 0; i < mNumEntities; i++) {
    if (mAiEntities[i]->mHandle == handle) {
      if (mAiEntities[i]->mType == AITYPE_PLAYER) {
        printf("AiManager::removeEntity(): error: asked to remove player AI\n");
        return;
      }

      delete mAiEntities[i];
      swap(mAiEntities[i], mAiEntities[mNumEntities - 1]);
      mAiEntities.pop_back();
      return;
    }
  }
}


AiEntity* AiManager::getEntityByHandle(size_t handle) {
  size_t mNumEntities = mAiEntities.size();
  for (size_t i = 0; i < mNumEntities; i++) {
    if (mAiEntities[i]->mHandle == handle) {
      return mAiEntities[i];
    }
  }
  return NULL;
}


vector<AiEntity*>* AiManager::getEntities(void) {
  return &mAiEntities;
}


size_t AiManager::spawnEntity(
  int type,
  v3d_t position)
{
  AiEntity* newAiEntity = addEntity(type, position);
  if (newAiEntity == NULL) {
    printf("AiManager::spawnEntity(): error: could not allocate new entity\n\n");
    return 0;
  }
  size_t physicsHandle = newAiEntity->mPhysicsHandle;
  if (physicsHandle == 0) {
    return 0;
  }

  double level;

  switch (type) {
    case AITYPE_BALLOON:
      newAiEntity->mMaxHealth = 15.0;
      newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
      newAiEntity->mWillAttackSameSpecies = true;

      mGameModel->physics->setHealth(physicsHandle, newAiEntity->mMaxHealth);
      mGameModel->physics->setMass(physicsHandle, 10.0);
      mGameModel->physics->setDimensions(physicsHandle, v3d_v(1.8, 1.8, 1.8));
      break;

    case AITYPE_SHOOTER:
      newAiEntity->mMaxHealth = 15.0;
      newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
      newAiEntity->mWillAttackSameSpecies = false;

      mGameModel->physics->setHealth(physicsHandle, newAiEntity->mMaxHealth);
      mGameModel->physics->setMass(physicsHandle, 100.0);
      mGameModel->physics->setDimensions(physicsHandle, v3d_v(0.75, 1.8, 0.75));
      break;

    case AITYPE_HOPPER:
      level = r_num(1.0, 3.0);
      newAiEntity->mMaxHealth = (float)level * 5.0f;
      newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
      newAiEntity->mWillAttackSameSpecies = true;

      mGameModel->physics->setHealth(physicsHandle, newAiEntity->mMaxHealth);
      mGameModel->physics->setMass(physicsHandle, 100.0);
      mGameModel->physics->setDimensions(physicsHandle, v3d_scale(0.5 + (level * 0.3), v3d_v(0.85, 0.85, 0.85)));
      break;

    case AITYPE_HUMAN:
      level = r_num(1.0, 3.0);
      newAiEntity->mMaxHealth = (float)level * 5.0f;
      newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
      newAiEntity->mWillAttackSameSpecies = false;

      mGameModel->physics->setHealth(physicsHandle, newAiEntity->mMaxHealth);
      mGameModel->physics->setMass(physicsHandle, 100.0);
      mGameModel->physics->setDimensions(physicsHandle, v3d_v(0.75, 1.8, 0.75));
      break;

    default:
      newAiEntity->mMaxHealth = 600000.0;
      newAiEntity->mCurrentHealth = 600000.0;

      mGameModel->physics->setHealth(physicsHandle, newAiEntity->mMaxHealth);
      mGameModel->physics->setMass(physicsHandle, 10.0);
      mGameModel->physics->setDimensions(physicsHandle, v3d_v(1.25, 1.25, 1.25));
      break;
  }

  // tell the physics who we really are
  mGameModel->physics->setOwner(physicsHandle, newAiEntity->mHandle);

  // give it some items
  for (int i = 0; i < AiEntity::AI_INVENTORY_SIZE; i++) {
    double itemRandom = r_num (0.0, 10.0);

    if (type == AITYPE_HOPPER) {
      if (itemRandom > 7.0) {
        newAiEntity->mInventory[i] = mGameModel->itemManager->generateRandomItem();
      }
      else {
        newAiEntity->mInventory[i] = 0;
      }
    }
    else if (type == AITYPE_SHOOTER ||
        type == AITYPE_BALLOON ||
        type == AITYPE_HUMAN)
    {
      if (i == 0) {
        // generate gun in first slot
        if (itemRandom > 9.6) {
          // this one could be dangerous
          newAiEntity->mInventory[i] = mGameModel->itemManager->generateRandomGun(r_num(5.0, 10.0));
        }
        else {
          // 'weak' gun
          newAiEntity->mInventory[i] = mGameModel->itemManager->generateRandomGun(r_num(1.0, 2.5));
        }
      }
      else {
        // maybe generate some other random item
        if (itemRandom > 9.0) {
          newAiEntity->mInventory[i] = mGameModel->itemManager->generateRandomItem();
        }
        else {
          newAiEntity->mInventory[i] = 0;
        }
      }
    }
  }

  return newAiEntity->mHandle;
}



int AiManager::update(
  WorldMap& worldMap)
{
  if (mGameModel->physics->isPaused()) {
    return (int)mAiEntities.size();
  }

  // clean up the entity list a bit
  trimEntitiesList();

  // we'll just count these up
  size_t liveCritters = 0;
  for (size_t i = 0; i < mAiEntities.size (); i++) {
    if (mAiEntities[i]->mCurrentHealth > 0.0) {
      liveCritters++;
    }
  }

  // check out what the physics engine has to say
  readPhysicsMessages();

  // FIXME: critter spawner
  if (liveCritters < mMaxCrittersHACK) {
    v3d_t pos = mGameModel->physics->getCenter(mGameModel->physics->getPlayerHandle());
    v2d_t offset = v2d_v(r_num (-10.0, 10.0), r_num(-10.0, 10.0));
    offset = v2d_scale(40.0, v2d_normalize(offset));

    pos.x += offset.x;
    pos.z += offset.y;

    int columnIndex = worldMap.getColumnIndex(pos);
    double height = worldMap.mColumns[columnIndex].getHighestBlockHeight();

    if (height < WATER_LEVEL + 1) {
      height = WATER_LEVEL + 3;
    }

    pos.y = height + 30.0;

    int type = r_numi(1, NUM_AITYPES);

    spawnEntity(type, pos);
  }

//	double pre_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
//	double post_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
//	printf ("AI time: %.4f\n", post_time - pre_time);
  
  // now to update the entities
  for (size_t i = 0; i < mAiEntities.size (); i++) {
    // ignore players and dead things
    if (mAiEntities[i]->mType == AITYPE_PLAYER) {
      mAiEntities[i]->updatePlayer(*mGameModel->physics);
    }

    // kill off dead physics objects
    if (mGameModel->physics->getIndexFromHandle(mAiEntities[i]->mPhysicsHandle) == -1) {
      mAiEntities[i]->mActive = false;
    }
    else if (mAiEntities[i]->mActive) {
      mAiEntities[i]->update(mGameModel->physics->getLastUpdateTime(), worldMap, *mGameModel->physics, mAiEntities, *mGameModel->itemManager);
    }

    // handle dead entities here by giving up their weapons to the physics engine
    if (mAiEntities[i]->mActive == false) {
      releaseItems(i);
    }
  }

  return (int)mAiEntities.size();
}


void AiManager::releaseItems(int aiIndex) {
  int items = 0;
  printf("eliminating inventory\n");
      
  // tell the ItemManager to release the inventory items
  for (int inventoryIndex = 0; inventoryIndex < AiEntity::AI_INVENTORY_SIZE; inventoryIndex++) {
    if (mAiEntities[aiIndex]->mInventory[inventoryIndex] > 0) {
      phys_message_t message;
      message.recipient = MAILBOX_ITEMMANAGER;
      message.type = PHYS_MESSAGE_ITEM_DESTROYED;
      message.iValue = (int)mAiEntities[aiIndex]->mInventory[inventoryIndex];
      mGameModel->physics->sendMessage(message);

      mAiEntities[aiIndex]->mInventory[inventoryIndex] = 0;
      items++;
    }
  }

  if (items > 0) {
    printf("AiManager::releaseItems(): eliminating inventory %d\n", items);
  }
}


void AiManager::trimEntitiesList() {
  size_t numEntities = mAiEntities.size();
  for (size_t i = 0; i < numEntities; i++) {
    if (mAiEntities[i]->mActive == false) {
      if (mAiEntities[i]->mType == AITYPE_PLAYER) {
        printf("AiManager::trimEntitiesList(): error: player inactive\n");
        printf(" (phys handle: %d)\n", mAiEntities[i]->mPhysicsHandle);
        continue;
      }
      removeEntity(mAiEntities[i]->mHandle);
      numEntities = mAiEntities.size();
      i--;
    }
  }
}


void AiManager::readPhysicsMessages() {
  phys_message_t message;
  int type;

  while (mGameModel->physics->getNextMessage(MAILBOX_AIMANAGER, &message)) {
    switch (message.type) {
    case PHYS_MESSAGE_SPAWN_CREATURE:
      type = r_numi(1, NUM_AITYPES);
      spawnEntity(type, message.vec3);
      break;

    default:
      break;
    }
  }
}



vector<size_t> AiManager::getAllItemHandles(void) {
  vector<size_t> itemList;

  for (size_t i = 0; i < mAiEntities.size(); i++) {
    for (size_t j = 0; j < AiEntity::AI_INVENTORY_SIZE; j++) {
      if (mAiEntities[i]->mInventory[j] > 0) {
        itemList.push_back(mAiEntities[i]->mInventory[j]);
      }
    }
  }

  return itemList;
}


void AiManager::setMaxCritters(int num) {
  mMaxCrittersHACK = num;
}


#include "AiEntity.h"


AiEntity::AiEntity() {
  mNextShotTime = 0.0;
  mNextUpdateTime = 0.0;
  mPhysicsHandle = 0;
  mTargetPhysicsHandle = 0;
  mMaxHealth = 1.0;
  mCurrentHealth = 0.0;
  mCurrentState = AISTATE_STOP;
  
  mFacingAngle = 0.0;
  mLookIncline = 0.0;
  v3d_zero(&mHeadOrientation);

  for (int i = 0; i < AI_INVENTORY_SIZE; i++) {
    mInventory[i] = 0;
  }
}


void AiEntity::update(
  double time,
  WorldMap& worldMap,
  Physics& physics,
  vector<AiEntity*>& aiEntities,
  ItemManager& itemManager)
{
  // this has to be non-NULL since AiManager::update() checks before calling this.
  // all the same...
  mPhysicsEntity = physics.getEntityByHandle(mPhysicsHandle);
  if (mPhysicsEntity == NULL) {
    // this is bad!
    printf("AiEntity::update(): error: NULL physics entity\n");
    mActive = false;
    return;
  }
  
  mWorldPosition = physics.getCenter(mPhysicsHandle);

  // won't know this until we read the mail
  mWasAttacked = false;

  readMail(physics);

  // handle death
  // FIXME: the other half of this exists in AiManager...
  // ever get the urge to merge?
  if (mCurrentHealth <= 0.0) {
    // turn inventory items into phys entities
    for (int i = 0; i < AI_INVENTORY_SIZE; i++) {
      if (mInventory[i] > 0) {
        item_t item = itemManager.getItem(mInventory[i]);

        if (item.type == ITEMTYPE_GUN_ONE_HANDED && r_numi(0, 10) < 3) {
          itemManager.destroyItem(mInventory[i]);
        }
        else {
          v3d_t itemForce = v3d_random(20.0);
          itemForce.y = 10.0;
          size_t itemPhysicsHandle = physics.createEntity(OBJTYPE_ITEM, mWorldPosition, itemForce, true);
          physics.setItemHandleAndType(itemPhysicsHandle, mInventory[i], itemManager.getItemType(mInventory[i]));
        }

        mInventory[i] = 0;
      }
    }

    return;
  }

  // is it time to think yet?
  if (mNextUpdateTime < time) {
    updateTarget(time, worldMap, physics, aiEntities);

//		updateState(time, worldMap, physics, aiEntities, itemManager);
  
    mNextUpdateTime = time + 0.3;
  }

  if (mTargetPhysicsHandle > 0) {
    mTargetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);
  }

  bool couldHaveFiredGun = false;
  bool didFireGun = false;

  if (mTargetPhysicsEntity == NULL) {
    mTargetPhysicsHandle = 0;
  }
  else {
    // now shoot'm if you got'm!
    v3d_t targetPosition = mTargetPhysicsEntity->boundingBox.getCenterPosition();

    bool hasLineOfSight = worldMap.lineOfSight(mWorldPosition, targetPosition);
    if (mTargetPhysicsEntity->type != OBJTYPE_TIGER_BAIT && hasLineOfSight) {
      if (mInventory[0] > 0 && itemManager.getItem (mInventory[0]).type == ITEMTYPE_GUN_ONE_HANDED) {
        couldHaveFiredGun = true;
      }

      didFireGun = useWeapon(time, physics, itemManager);
    }
  }

  // now for movement
  switch (mType) {
    case AITYPE_BALLOON:
      updateBalloon(time, worldMap, physics, aiEntities, itemManager);
      break;

    case AITYPE_SHOOTER:
      if (!couldHaveFiredGun || (couldHaveFiredGun && !didFireGun && mNextShotTime <= time)) {
        updateHopper(time, worldMap, physics, aiEntities, itemManager);
      }
      break;

    case AITYPE_HOPPER:
      updateHopper(time, worldMap, physics, aiEntities, itemManager);
      break;

    case AITYPE_HUMAN:
      if (!couldHaveFiredGun || (couldHaveFiredGun && !didFireGun && mNextShotTime <= time)) {
        updateHopper(time, worldMap, physics, aiEntities, itemManager);
      }
      break;

    case AITYPE_DUMMY:
      updateDummy(time, worldMap, physics, aiEntities);
      break;

    default:
      break;
  }
}





void AiEntity::updateState(
  double time,
  WorldMap &worldMap,
  Physics &physics,
  vector<AiEntity *> &aiEntities,
  ItemManager &itemManager)
{

  // test conditions
  AiSpecies *spec = gSpeciesData.get(mType);
  size_t totalStates = spec->stateMachine.mStates.size();
  bool isDone = false;
  for (size_t i = 0; i < totalStates && !isDone; i++) {
    if (spec->stateMachine.mStates[i].state == mCurrentState) {
      if (testCondition(spec->stateMachine.mStates[i].condition,
        time,
        worldMap,
        physics,
        aiEntities,
        itemManager))
      {
        mCurrentState = spec->stateMachine.mStates[i].nextState;
        isDone = true;
      }
    }
  }



  switch(mCurrentState) {
  case AISTATE_ALL:
    printf("AiEntity::updateState(): AISTATE_ALL: should not be here...\n");
    mCurrentState = AISTATE_STOP;
    break;
  case AISTATE_STOP:
    break;
  case AISTATE_WANDER:
    break;
  case AISTATE_MOVE_TO_WAYPOINT:
    break;
  case AISTATE_PURSUE:
    break;
  case AISTATE_ATTACK_MELEE:
    break;
  case AISTATE_ATTACK_RANGE:
    break;
  case AISTATE_RANGE:
    break;
  case AISTATE_FLEE:
    break;
  case AISTATE_FOLLOW:
    break;
  default:
    printf("AiEntity::updateState(): unknown state\n");
    mCurrentState = AISTATE_STOP;
    break;
  }
}




bool AiEntity::testCondition(
  int condition,
  double time,
  WorldMap &worldMap,
  Physics &physics,
  vector<AiEntity *> &aiEntities,
  ItemManager &itemManager)
{

  switch(condition) {
  case CONDITION_FALSE:
    return false;
  case CONDITION_BEGIN_WANDER:
    return false;
  case CONDITION_END_WANDER:
    return false;
  case CONDITION_ENEMY_SPOTTED:
    return false;
  case CONDITION_ATTACKED:
    return mWasAttacked;
  case CONDITION_BADLY_HURT:
    return false;
  case CONDITION_TARGET_IN_RANGE:
    return false;
  case CONDITION_TARGET_TOO_CLOSE:
    return false;
  case CONDITION_TARGET_TOO_FAR:
    return false;
  case CONDITION_TARGET_DEAD:
    return false;
  case CONDITION_TARGET_LOST:
    return false;
  default:
    printf("AiEntity::testCondition(): unknown condition\n");
    return false;
  }
}


void AiEntity::readMail(Physics& physics) {
  // lets read some mail
  // FIXME: this needs to be different for each type
  phys_message_t message;

  while (physics.getNextMessage (static_cast<int>(mPhysicsHandle), &message)) {
//		printf ("message: to: %d, from: %d\n", message.recipient, message.sender);

    switch (message.type) {
    case PHYS_MESSAGE_DAMAGE:
      if (message.sender != mPhysicsHandle && message.sender != MAILBOX_PHYSICS) {
        if (physics.getIndexFromHandle(message.sender) >= 0) {
          if (physics.getEntityByHandle(message.sender)->aiType == AITYPE_PLAYER && mType == PLAYER_SPECIES) {
            // ignore player if same species
          }
          else if (physics.getEntityByHandle(message.sender)->aiType == mType) {
            if (gSpeciesData.get(mType)->willAttackSameSpecies) {
              mWasAttacked = true;
              mAttackerHandle = message.sender;
              mTargetPhysicsHandle = message.sender;
            }
          }
          else {
            mWasAttacked = true;
            mAttackerHandle = message.sender;
            mTargetPhysicsHandle = message.sender;
          }
        }
          
      }
      else {
        printf ("AiEntity::readMail(): hurt myself!\n");
      }

      mCurrentHealth -= static_cast<float>(message.dValue);
      break;

    default:
      break;
    }
  } // done reading mail
}



void AiEntity::updatePlayer(Physics& physics) {
  mPhysicsEntity = physics.getEntityByHandle(mPhysicsHandle);
  mWorldPosition = physics.getCenter(mPhysicsHandle);
}


void AiEntity::updateBalloon(
  double time,
  WorldMap& worldMap,
  Physics& physics,
  vector<AiEntity*>& aiEntities,
  ItemManager& itemManager)
{
  v3d_t movementForce = v3d_zero();

  if (mPhysicsEntity->worldViscosity > 0.0) {
    physics.add_force(mPhysicsHandle, v3d_v(0.0, 120.0, 0.0));
  }

  if (physics.getIndexFromHandle(mTargetPhysicsHandle) >= 0) {
    // we can't fly if we're on the ground!
    if (mPhysicsEntity->on_ground) {
//      movementForce = v3d_v(0.0, 8000.0, 0.0);
      physics.add_force(mPhysicsHandle, v3d_v(0.0, 8000.0, 0.0));
    }

    PhysicsEntity* targetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);
    if (targetPhysicsEntity == NULL) {
      printf("AiEntity::updateBalloon() - target is NULL\n");
      return;
    }

    v3d_t targetPosition = targetPhysicsEntity->boundingBox.getCenterPosition();

    // let's keep this thing afloat
    double desiredHeight;
    if (targetPhysicsEntity->aiType == AITYPE_HOPPER ||
      targetPhysicsEntity->aiType == AITYPE_SHOOTER ||
      targetPhysicsEntity->aiType == AITYPE_PLAYER)
    {
      double sin2 = (2.0 * sin(time));
      desiredHeight = targetPosition.y + 14.0 + sin2;
      double height2 = worldMap.mPeriodics->getTerrainHeight(mWorldPosition.x, mWorldPosition.z) + 10.0 + sin2;
      if (height2 > desiredHeight) {
        desiredHeight = height2;
      }
    }
    else {
      // this is so that when floaters fight they don't just keep flying higher
      desiredHeight = worldMap.mPeriodics->getTerrainHeight(mWorldPosition.x, mWorldPosition.z) + 10.0 + (2.0 * sin(time));
    }

    double delta = mWorldPosition.y - desiredHeight;

    if (delta > 0.0) {
  //		delta += 0.2 * mPhysicsEntity.vel.y;
      delta = 0.0;
    }
    else if (delta < 0.0) {
      delta += 0.2 * mPhysicsEntity->vel.y;
    }

    double forceMagnitude = 5.0 * (delta * delta); //; - physicsEntity.vel.y;

    if (forceMagnitude > 200.0) {
      forceMagnitude = 200.0;
    }

    if (delta > 0.0) {
      movementForce = v3d_v(0.0, -0.5 * forceMagnitude, 0.0);
    }
    else {
      movementForce = v3d_v(0.0, forceMagnitude, 0.0);
    }

    // try to get to the target

    v3d_t vecToTarget = v3d_sub(targetPosition, mWorldPosition);
    vecToTarget.y = 0.0;

    v3d_t velNoY = mPhysicsEntity->vel;
    velNoY.y = 0.0;

    double angle1 = atan2(vecToTarget.x, vecToTarget.z);
    double angle2 = atan2(velNoY.x, velNoY.z);
    double angle = angle1 - angle2;

    // this is for critter with 'steerage'
    if (abs(angle) > 0.1) {
      angle = absConstrain(angle, 0.01);

      v3d_t rot = v3d_rotateY(mPhysicsEntity->vel, angle);
      mPhysicsEntity->vel.x = rot.x;
      mPhysicsEntity->vel.z = rot.z;
    }


    if (v3d_mag(vecToTarget) < mMinDistanceToPlayer) {
      vecToTarget = v3d_scale(15.0, v3d_normalize(vecToTarget));

      movementForce = v3d_add(movementForce, vecToTarget);
    }	
  
//		physics.add_force (mPhysicsHandle, movementForce);
//		mPhysicsEntity->force = v3d_add(mPhysicsEntity->force, movementForce);

    mPhysicsEntity->force.x += movementForce.x;
    mPhysicsEntity->force.y += movementForce.y;
    mPhysicsEntity->force.z += movementForce.z;
  }
}


void AiEntity::updateHopper(
  double time,
  WorldMap &worldMap,
  Physics &physics,
  vector<AiEntity *> &aiEntities,
  ItemManager &itemManager)
{
  v3d_t movementForce = v3d_zero();

  if (mPhysicsEntity->health < 0.0) {
    return;
  }

  if (r_numi(0, 1085) == 3) {
    physics.addSoundEvent(SOUND_TIGER_ROAR, mPhysicsEntity->pos);
  }

  // keep it afloat in water even without a target
  if (mTargetPhysicsHandle == 0 && mPhysicsEntity->worldViscosity > 0.0) {
    physics.add_force(mPhysicsHandle, v3d_v(0.0, 4200.0, 0.0));
  }
  else if (physics.getIndexFromHandle(mTargetPhysicsHandle) >= 0) {
    PhysicsEntity *targetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);
    v3d_t targetPosition = targetPhysicsEntity->boundingBox.getCenterPosition();
    v3d_t vecToTarget = v3d_sub(targetPosition, mWorldPosition);

    if (mPhysicsEntity->worldViscosity > 0.0) {
      if (v3d_mag(vecToTarget) < mMinDistanceToPlayer) {
        vecToTarget = v3d_scale(1600.0 * (sin(time * 8.0) + 1.5), v3d_normalize(vecToTarget));

        movementForce = v3d_add(movementForce, vecToTarget);

        // give it a little more buoyancy
        if (targetPosition.y - mWorldPosition.y > 0.0) {
          physics.add_force(mPhysicsHandle, v3d_v(0.0, 2000.0, 0.0));
        }
      }
    }
    else if (mPhysicsEntity->on_ground) {
      vecToTarget.y = 0.0;

      if (v3d_mag(vecToTarget) < mMinDistanceToPlayer) {
        // big leap?
        if (r_numi(0, 60) == 3) {
          vecToTarget = v3d_scale(15000.0, v3d_normalize(vecToTarget));
          vecToTarget.y = 60000.0;
          movementForce = v3d_add(movementForce, vecToTarget);
        }
        else {
          vecToTarget = v3d_scale(2000.0, v3d_normalize(vecToTarget));
          //vecToTarget.y = r_num(8000.0, 10000.0);
          movementForce = v3d_add(movementForce, vecToTarget);
        }
      }
    }
    else {
      vecToTarget.y = 0.0;

      if (v3d_mag(vecToTarget) < mMinDistanceToPlayer) {
        vecToTarget = v3d_scale(5.0, v3d_normalize(vecToTarget));
  //			vecToPlayer.y = 20.0;
        movementForce = v3d_add(movementForce, vecToTarget);
      }
    }
      
    physics.add_force(mPhysicsHandle, movementForce);
  }
}



void AiEntity::updateDummy(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities) {
  // check for line of sight
/*	v3d_t playerPos = v3d_add (physics.getCenter (1), v3d_v (0.0, 1.6 - 0.9, 0.0));

  bool lineOfSight = 
    worldMap.lineOfSight (physics.getCenter (mPhysicsHandle), playerPos) ||
    worldMap.lineOfSight (physics.getFarCorner (mPhysicsHandle), playerPos) ||
    worldMap.lineOfSight (physics.getNearCorner (mPhysicsHandle), playerPos);


  if (!lineOfSight) {
    printf ("no line of sight!\n");
  }
  else {
    printf ("peek a boo!\n");
  }
*/
}


void AiEntity::updateTarget(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities) {
  mTargetPhysicsEntity = NULL;

  if (mTargetPhysicsHandle != 0) {
    mTargetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);

    // get rid of current target if dead
    if (mTargetPhysicsEntity == NULL) {
      mTargetPhysicsHandle = 0;
    }
    else if (mTargetPhysicsEntity->health <= 0.0 && mTargetPhysicsEntity->type == OBJTYPE_PLAYER) {
      mTargetPhysicsHandle = 0;
      mTargetPhysicsEntity = NULL;
    }
  }

  if (mTargetPhysicsHandle == 0) {
    aquireTarget(time, worldMap, physics, aiEntities);
  }
}

void AiEntity::aquireTarget(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities) {
  double minDistance = 100000.0;
  int minHandle = -1;

  // try to find target
  for (size_t j = 0; j < aiEntities.size (); j++) {
    // ignore self and dead things
    if (!aiEntities[j]->mActive || aiEntities[j]->mType == mType || aiEntities[j]->mHandle == mHandle) {
      continue;
    }

    if (DONT_ATTACK_PLAYER && aiEntities[j]->mType == AITYPE_PLAYER) continue;
    // ignore player if same species...
    // FIXME: hard-wired
    if (aiEntities[j]->mType == AITYPE_PLAYER && mType == PLAYER_SPECIES) continue;

    size_t jPhysicsHandle = aiEntities[j]->mPhysicsHandle;
    // don't target if dead
    mTargetPhysicsEntity = physics.getEntityByHandle(jPhysicsHandle);
    if (mTargetPhysicsEntity == NULL) continue;
    if (mTargetPhysicsEntity->health <= 0.0) continue;

    double jDistance = v3d_dist(mWorldPosition, physics.getCenter(jPhysicsHandle));

    if (jDistance <= 35.0 && jDistance < minDistance) {
      // check for line of sight
      if (worldMap.lineOfSight(mWorldPosition, physics.getCenter(jPhysicsHandle))) {
        minHandle = static_cast<int>(jPhysicsHandle);
        minDistance = jDistance;
      }
    }
  }

  if (minHandle > 0) {
    mTargetPhysicsHandle = minHandle;
    mTargetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);
    return;
  }
  else {
    mTargetPhysicsHandle = 0;
    mTargetPhysicsEntity = NULL;
  }

  // perhaps a stroll...
  if (mTargetPhysicsHandle == 0 && r_numi(0, 5) == 2) {
    PhysicsEntity* playerPhysicsEntity = physics.getEntityByHandle(physics.getPlayerHandle());
    v3d_t baitPosition;
    if(playerPhysicsEntity != NULL) {
      // hehe let's sneak up on the player!
      baitPosition = v3d_add(playerPhysicsEntity->pos, v3d_v(r_num(-40.0, 40.0), 10.0, r_num(-40.0, 40.0)));
    }
    else {
      baitPosition = v3d_add(mWorldPosition, v3d_v(r_num(-25.0, 25.0), 10.0, r_num(-25.0, 25.0)));
    }

    size_t baitHandle = physics.createEntity(OBJTYPE_TIGER_BAIT, baitPosition, true);

    physics.setHealth(baitHandle, 1.0);

    mTargetPhysicsHandle = baitHandle;
    mTargetPhysicsEntity = physics.getEntityByHandle(mTargetPhysicsHandle);
    return;
  }
}

bool AiEntity::useWeapon(double time, Physics& physics, ItemManager& itemManager) {
  if (mInventory[0] == 0) return false;
  if (mNextShotTime > time) return false;

  item_t item = itemManager.getItem(mInventory[0]);
  if (item.type != ITEMTYPE_GUN_ONE_HANDED) {
    return false;
  }

  v3d_t targetPosition = mTargetPhysicsEntity->boundingBox.getCenterPosition();
  v3d_t vecToTarget = v3d_sub(targetPosition, mWorldPosition);
  double distanceToTarget = v3d_mag(vecToTarget);
  if (!isTargetInRange(item.gunType, distanceToTarget)) {
    return false;
  }

  shot_info_t shotInfo;
  shotInfo.angle = v3d_normalize(vecToTarget);
  shotInfo.ownerPhysicsHandle = mPhysicsHandle;
  shotInfo.position = mWorldPosition;
  shotInfo.time = time;

  mNextShotTime = itemManager.useGun(mInventory[0], shotInfo);

  return true;
}

bool AiEntity::isTargetInRange(int gunType, double distanceToTarget) {
  // HACK: range should be included in item_t?
  switch (gunType) {
  case GUNTYPE_RIFLE:
    if (distanceToTarget > 100.0) {
      return false;
    }
    break;

  case GUNTYPE_MACHINE_GUN:
    if (distanceToTarget > 40.0) {
      return false;
    }
    break;

  case GUNTYPE_ROCKET_LAUNCHER:
    if (distanceToTarget > 60.0) {
      return false;
    }
    break;

  default:
    if (distanceToTarget > 30.0) {
      return false;
    }
    break;
  }

  return true;
}




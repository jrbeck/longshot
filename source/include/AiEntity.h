// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiEntity
// *
// * this is where all the individual AI stuff happens.
// * the plan is to use a bunch of state table to control the behaviors of the
// * various critter types
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#pragma once

#include <vector>

#include "GL/glut.h"

#include "AiSpecies.h"
#include "WorldMap.h"
#include "Physics.h"
#include "ItemManager.h"
#include "MathUtil.h"


#define DONT_ATTACK_PLAYER    (true)
#define PLAYER_SPECIES        (AITYPE_HUMAN)

extern class Physics;
extern struct PhysicsEntity;

class AiEntity {
public:
  AiEntity(GameModel* gameModel);

  void update();
  // this is called only if the entity is a AITYPE_PLAYUH
  void updatePlayer();

  void updateState();
  bool testCondition(int condition);

  void readMail();

  // this is mostly slated to be paramaterized and moved out of here.
  // good riddance!
  void updateBalloon();
  void updateHopper();
  void updateDummy();

  void updateTarget();
  void aquireTarget();

  bool useWeapon();
  bool isTargetInRange(int gunType, double distanceToTarget);

  // members * * * * * * * * * * * * * * * * *
  int mActive;

  int mType;

  GameModel* mGameModel;

  v3d_t mWorldPosition;

  size_t mHandle;
  size_t mPhysicsHandle;
  size_t mTargetPhysicsHandle;

  PhysicsEntity* mPhysicsEntity;
  PhysicsEntity* mTargetPhysicsEntity;

  float mMaxHealth;
  float mCurrentHealth;

  int mCurrentState;

#define mMinDistanceToPlayer  370.0

  double mNextShotTime;
  double mNextUpdateTime;

  static const int AI_INVENTORY_SIZE = 5;
  size_t mInventory[AI_INVENTORY_SIZE];

  int mCurrentWeapon;

  // behavior stuff
  bool mWasAttacked;
  int mAttackerHandle;
  bool mWillAttackSameSpecies;


  double mFacingAngle;
  double mLookIncline;
  v3d_t mHeadOrientation;
};

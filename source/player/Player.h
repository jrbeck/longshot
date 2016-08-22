// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Player
// *
// * this contains all (way too much) player related stuff:
// * inventory
// * camera info
// * movement
// *
// * etc...
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cmath>

#include "../math/MathUtil.h"
#include "../math/v2d.h"
#include "../math/v3d.h"
#include "../math/GlCamera.h"

#include "../game/GameModel.h"

#include "../game/GameInput.h"
#include "../physics/Physics.h"
#include "../game/GameMenu.h"
#include "../items/ItemManager.h"
#include "../items/Inventory.h"
#include "../player/HeadBobble.h"

#include "../world/LightManager.h"

// orientation defaults
#define DEFAULT_POS_X      (30.0)
#define DEFAULT_POS_Y      (30.0)
#define DEFAULT_POS_Z      (30.0)

#define DEFAULT_FACING      (45.0)
#define DEFAULT_INCLINE      (0.0)
#define DEFAULT_INCLINE_MIN    (-88.0)
#define DEFAULT_INCLINE_MAX    (88.0)

enum {
  EQUIP_PRIMARY,
  EQUIP_SECONDARY
};

#define LEFT_HANDED  (-1.0)
#define RIGHT_HANDED  (1.0)

#define DEFAULT_BACKPACK_SIZE  (15)

class ItemManager;
struct item_t;
class Inventory;

struct MovementInput {
  v2d_t walkInput;
  double facingDelta;
  double inclinationDelta;
  bool isJumping;
  bool isSwimming;
};

class Player {
public:
  Player(GameModel* gameModel);
  ~Player();

  int reset(size_t physicsHandle, size_t aiHandle);
  int soft_reset(v3d_t& startPosition);
  void HACK_resurrect();
  void setStartPosition(v3d_t& startPosition);

  void godMode();
  bool isDead();

  void updateOrientation(double facingDelta, double inclinationDelta);
  void updateCameraTarget();
  void constrainViewAngles();

  v2d_t getFacingAndIncline();
  bool isHeadInWater();

  v3d_t getLookTarget() const;

  v3d_t get_pos() const;
  int set_pos(v3d_t a);
  v3d_t getHeadPosition() const;
  int getHeadPostionBlockType() const;
  double getCurrentHealth() const;

  bool pickUpItem(item_t item, AssetManager* assetManager);

  void useEquipped(int whichEquip);
  double fireGun(item_t item, double handedness);
  double useMeleeWeapon(item_t item);

  melee_weapon_state_t* getMeleeWeaponState(int hand);

  void useBackpackItem();

  v2d_t computeWalkVector(v2d_t walkInput);

  void updateTargetBlock();
  v3di_t* getTargetBlock(int& targetBlockFace);

  void applyMovementInput(MovementInput* movementInput, AssetManager* assetManager);

  void readMessages(AssetManager* assetManager);

  // HACK
  void placeLight(GameInput* gameInput);
  Inventory* getInventory();

private:
  GameModel* mGameModel;

  v3d_t mPos;
  v3d_t mHeadOffset;
  v3d_t mFinalHeadOffset;
  HeadBobble mHeadBobble;

  v3d_t mLookTarget;
  v3d_t mLookVector;

  bool mIsBlockTargeted;
  v3di_t mTargetBlock;
  int mTargetBlockFace;

  bool mStartLocationFound;
  v3d_t mStartPosition;

  double mMaxHealth;
  double mCurrentHealth;

  bool deathScreamUttered;

  double mLastFootStep;

  double mLastUpdateTime;

  double mNextShotTimePrimary;
  double mNextShotTimeSecondary;

  Inventory mInventory;

  melee_weapon_state_t mMeleeStatePrimary;
  melee_weapon_state_t mMeleeStateSecondary;

  v2d_t mWalkInput;

  bool mInWater;
  bool mHeadInWater;
  int mHeadPostionBlockType;

  double mFacing;     // the angle on the x-z plane off the positive x-axis
  double mIncline;    // the angle on the vertical plane off the x-z plane intersection...
  double mInclineMin; // upper constraint for incline angle
  double mInclineMax; // lower constraint for incline angle

  bool mPlacedBlock;
};

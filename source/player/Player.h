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

class Player {
public:
  Player(GameModel* gameModel);
  ~Player();

  int reset(size_t physicsHandle, size_t aiHandle);
  int soft_reset(v3d_t& startPosition);
  void setStartPosition(v3d_t& startPosition);

  void godMode();

  // update the camera target
  void update_target();
  int constrain_view_angles();

  v2d_t getFacingAndIncline();
  bool isHeadInWater();

  int set_target(v3d_t a);

  void set_draw_distance(double distance);
  void adjust_draw_distance(double amount);

  // use the gluLookAt () to set view at render time
  GlCamera gl_cam_setup();

  v3d_t get_pos();
  int set_pos(v3d_t a);

  bool pickUpItem(item_t item, AssetManager& assetManager);

  void useEquipped(int whichEquip);
  double fireGun(item_t item, double handedness);
  double useMeleeWeapon(item_t item);

  melee_weapon_state_t* getMeleeWeaponState(int hand);

  void useBackpackItem();

  v2d_t obtainWalkVector(v2d_t walkInput);

  void updateTargetBlock();
  v3di_t* getTargetBlock(int& targetBlockFace);

  void updateHud();
  void drawHud();
  void drawWaterOverlay();

  void updateCharacterSheet();

  bool update(AssetManager& assetManager, GameInput* gi);

  void readPhysicsMessages(AssetManager& assetManager);

  // HACK
  void placeLight(GameInput* gameInput);
  Inventory* getInventory();

  GlCamera cam;

private:
  GameModel* mGameModel;

  GameMenu mHud;

  GameMenu mCharacterSheet;
  bool mShowCharacterSheet;

  v3d_t mPos;      // where the 'feet' are

  v3d_t mHeadOffset;      // where's the noggin relative to the phys position
  v3d_t mFinalHeadOffset;    // where is it after all is considered
  HeadBobble mHeadBobble;    // something to consider

  v3d_t mTarget;      // where the camera is pointed
  v3d_t up;        // the 'up' vector for the camera
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

  GLfloat mVisionTint[4];

  double mFacing;      // the angle on the x-z plane off the positive x-axis
  double mIncline;    // the angle on the vertical plane off the x-z plane intersection...
  double mInclineMin;    // upper constraint for incline angle
  double mInclineMax;    // lower constraint for incline angle

  bool mPlacedBlock;
};

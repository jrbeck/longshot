#include "../player/Player.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)

Player::Player(GameModel* gameModel) :
  mGameModel(gameModel)
{
  mInventory = new Inventory(DEFAULT_BACKPACK_SIZE);
}

Player::~Player () {
  if (mInventory != NULL) {
    delete mInventory;
  }
}

int Player::reset(size_t physicsHandle, size_t aiHandle) {
  mStartLocationFound = false;
  mStartPosition = v3d_zero();

  mHeadOffset = v3d_v(0.25, 1.6, 0.25);
  mFinalHeadOffset = mHeadOffset;

  mFacing = DEG2RAD(DEFAULT_FACING);

  mIncline = DEG2RAD(DEFAULT_INCLINE);
  mInclineMin = DEG2RAD(DEFAULT_INCLINE_MIN);
  mInclineMax = DEG2RAD(DEFAULT_INCLINE_MAX);

  updateCameraTarget();

  mLastUpdateTime = 0.0;
  mNextShotTimePrimary = 0.0;
  mNextShotTimeSecondary = 0.0;

  mInventory->clear();

  // give the man a pointy stick
  if (!mInventory->getBackpack()->isFull()) {
    mInventory->getBackpack()->addItem(mGameModel->mItemManager->generateRandomGun(1.0));
  }

  // melee weapon
  if (!mInventory->getBackpack()->isFull()) {
    mInventory->getBackpack()->addItem(mGameModel->mItemManager->generateRandomMeleeWeapon(3.0));
  }

  // block
  if (!mInventory->getBackpack()->isFull()) {
    item_t blockItem;
    memset(&blockItem, 0, sizeof (item_t));
    blockItem.active = true;
    blockItem.type = ITEMTYPE_WORLD_BLOCK;
    strcpy(blockItem.name, "blocker");

    mInventory->getBackpack()->addItem(mGameModel->mItemManager->createItem(blockItem));
  }

  mPlacedBlock = false;

  return 0;
}

// reset everything but the physics handle
int Player::soft_reset(v3d_t& startPosition) {
  mHeadOffset = v3d_v(0.25, 1.6, 0.25);
  mFinalHeadOffset = mHeadOffset;
  mHeadBobble.reset();

  mIsBlockTargeted = false;

  // find a good place to call home
  if (!mStartLocationFound) {
    setStartPosition(startPosition);
  }
  mGameModel->mPhysics->set_pos(mGameModel->mPhysics->getPlayerHandle(), mStartPosition);

  mMaxHealth = 100.0;
  mCurrentHealth = 100.0;
  mGameModel->mPhysics->setHealth(mGameModel->mPhysics->getPlayerHandle(), mCurrentHealth);

  deathScreamUttered = false;

  mInWater = false;
  mHeadInWater = false;

  mLastFootStep = -1.0;

  memset(&mMeleeStatePrimary, 0, sizeof (melee_weapon_state_t));
  memset(&mMeleeStateSecondary, 0, sizeof (melee_weapon_state_t));

  return 0;
}

void Player::HACK_resurrect() {
  mMaxHealth = 100.0;
  mCurrentHealth = 100.0;
  mGameModel->mPhysics->setHealth(mGameModel->mPhysics->getPlayerHandle(), 100.0);
  mFinalHeadOffset = mHeadOffset;
  deathScreamUttered = false;
}

void Player::setStartPosition(v3d_t& startPosition) {
    mStartPosition = startPosition;
    mStartLocationFound = true;
}

void Player::godMode() {
  printf ("you naughty little stinker you...\n");

  // some ammo?
  for (int i = 0; i < NUM_AMMO_TYPES; ++i) {
    mInventory->mAmmoCounter[i] = 10000;
  }

  // how bout a couple a nice shooters
  item_t gun;

  if (!mInventory->getBackpack()->isFull()) {
    mGameModel->mItemManager->generateRandomRocketLauncher(gun, 1.5);
    gun.type = ITEMTYPE_GUN_ONE_HANDED;
    gun.bulletType = OBJTYPE_LIVE_BULLET;
    gun.ammoType = AMMO_BULLET;
    gun.explosionForce = 5000.0; // * 0.2;
    mGameModel->mItemManager->nameGun(gun);

    mInventory->getBackpack()->addItem(mGameModel->mItemManager->createItem(gun));
  }

  // okay one more...
  if (!mInventory->getBackpack()->isFull()) {
    mGameModel->mItemManager->generateRandomRocketLauncher(gun, 3.0);
//    gun.bulletType = OBJTYPE_NAPALM;
//    gun.ammoType = AMMO_NAPALM;
    gun.bulletType = OBJTYPE_SLIME;
    gun.ammoType = AMMO_SLIME;
    gun.explosionForce = 5000.0 * 0.2;
    mGameModel->mItemManager->nameGun(gun);

    mInventory->getBackpack()->addItem(mGameModel->mItemManager->createItem(gun));
  }

  // and the rocket pack
  if (!mInventory->getBackpack()->isFull()) {
    gun.type = ITEMTYPE_ROCKET_PACK;
    gun.value1 = 2000.0;
    sprintf(gun.name, "rocket pack");
    mInventory->getBackpack()->addItem(mGameModel->mItemManager->createItem(gun));
  }
}

bool Player::isDead() {
  return mCurrentHealth <= 0.0;
}

void Player::constrainViewAngles() {
  if (mIncline < mInclineMin) mIncline = mInclineMin;
  else if (mIncline > mInclineMax) mIncline = mInclineMax;

  // this is ugly, but really shouldn't ever be a problem
  while (mFacing > MY_2PI) mFacing -= MY_2PI;
  while (mFacing < 0) mFacing += MY_2PI;
}

v2d_t Player::getFacingAndIncline() {
  return v2d_v(mFacing, mIncline);
}

bool Player::isHeadInWater() {
  return mHeadInWater;
}

v3d_t Player::getLookTarget() const {
  return mLookTarget;
}

void Player::updateOrientation(double facingDelta, double inclinationDelta) {
  mFacing += facingDelta;
  mIncline += inclinationDelta;
  constrainViewAngles();
  updateCameraTarget();
  updateTargetBlock();
}

void Player::updateCameraTarget() {
  mLookVector = v3d_getLookVector(mFacing, mIncline);
  mLookTarget = v3d_add(v3d_add(mFinalHeadOffset, mPos), mLookVector);
}

v3d_t Player::get_pos() const {
  return mPos;
}

int Player::set_pos(v3d_t a) {
  mPos = a;
  return 0;
}

v3d_t Player::getHeadPosition() const {
  return v3d_add(mFinalHeadOffset, mPos);
}

int Player::getHeadPostionBlockType() const {
  return mHeadPostionBlockType;
}

double Player::getCurrentHealth() const {
  return mCurrentHealth;
}

bool Player::pickUpItem(item_t item, AssetManager* assetManager) {
  // special items first:
  if (item.type == ITEMTYPE_AMMO_BULLET ||
    item.type == ITEMTYPE_AMMO_SLIME ||
    item.type == ITEMTYPE_AMMO_PLASMA ||
    item.type == ITEMTYPE_AMMO_NAPALM)
  {
    mInventory->mAmmoCounter[item.ammoType] += item.quantity;
    assetManager->mSoundSystem.playSoundByHandle(SOUND_HEALTHPACK, 192);
    return true;
  }

  // now for inventory items
  if (mInventory->getBackpack()->isFull()) {
    printf("inventory full!\n");
    return false;
  }
  else {
    mInventory->getBackpack()->addItem(item.handle);
  }

  switch (item.type) {
    case ITEMTYPE_HEALTHPACK:
      assetManager->mSoundSystem.playSoundByHandle(SOUND_HEALTHPACK, 192);
      break;

    case ITEMTYPE_GUN_ONE_HANDED:
      assetManager->mSoundSystem.playSoundByHandle(SOUND_PISTOL_RELOAD, 192);
      break;

    default:
      printf ("Player::pickUpItem(): warning! unknown item type");
      break;
  }

  return true;
}

void Player::useEquipped(int whichEquip) {
  item_t item;
  double time = mGameModel->mPhysics->getLastUpdateTime();

  switch (whichEquip) {
    case EQUIP_PRIMARY:
      // can't use a non-item
      if (mInventory->mPrimaryItem <= 0) return;
      item = mGameModel->mItemManager->getItem(mInventory->mPrimaryItem);
      break;

    case EQUIP_SECONDARY:
      if (mInventory->mSecondaryItem <= 0) return;
      item = mGameModel->mItemManager->getItem(mInventory->mSecondaryItem);
      break;

    default:
      printf("Player::useEquipped(): trying to use invalid equip location\n");
      return;
  }

  if (item.type == ITEMTYPE_GUN_ONE_HANDED || item.type == ITEMTYPE_GUN_TWO_HANDED) {
    if (whichEquip == EQUIP_PRIMARY) {
      if (mNextShotTimePrimary > time) return;
      mNextShotTimePrimary = fireGun(item, LEFT_HANDED);
    }
    else {
      if (mNextShotTimeSecondary > time) return;
      mNextShotTimeSecondary = fireGun(item, RIGHT_HANDED);
    }
  }
  else if (item.type == ITEMTYPE_MELEE_ONE_HANDED) {
    if (whichEquip == EQUIP_PRIMARY) {
      if (mNextShotTimePrimary > time) return;
      mNextShotTimePrimary = useMeleeWeapon(item);
      mMeleeStatePrimary.swingStart = time;
      mMeleeStatePrimary.hasUsed = true;
    }
    else {
      if (mNextShotTimeSecondary > time) return;
      mNextShotTimeSecondary = useMeleeWeapon(item);
      mMeleeStateSecondary.swingStart = time;
      mMeleeStateSecondary.hasUsed = true;
    }
  }
  else if (item.type == ITEMTYPE_WORLD_BLOCK) {
    mPlacedBlock = true;
  }
  else {
    if (mGameModel->mItemManager->useItem(computeWalkVector(mWalkInput), item.handle, mGameModel->mPhysics->getPlayerHandle())) {
      if (whichEquip == EQUIP_PRIMARY) {
        mInventory->mPrimaryItem = 0;
      }
      else {
        mInventory->mSecondaryItem = 0;
      }
    }
  }
}

double Player::fireGun(item_t item, double handedness) {
  v3d_t world_head_pos = getHeadPosition();

  double shoulderOffset = handedness * 0.5;
  double gunMuzzleOffset = 0.65;
  v3d_t gunMuzzle = v3d_scale(shoulderOffset, v3d_normalize(v3d_cross(mLookVector, v3d_v(0.0, 1.0, 0.0))));
  gunMuzzle = v3d_add(v3d_scale(gunMuzzleOffset, mLookVector), gunMuzzle);

  v3d_t temp = v3d_sub(mLookTarget, world_head_pos);
  v3d_t targAngle = v3d_normalize(temp);

  v3d_t pos = v3d_v(
    world_head_pos.x + temp.x + gunMuzzle.x,
    world_head_pos.y + temp.y - 0.35 + gunMuzzle.y,
    world_head_pos.z + temp.z + gunMuzzle.z
  );

  shot_info_t shotInfo;
  shotInfo.angle = targAngle;
  shotInfo.ownerPhysicsHandle = mGameModel->mPhysics->getPlayerHandle();
  shotInfo.position = pos;
  shotInfo.time = mGameModel->mPhysics->getLastUpdateTime();

  return mGameModel->mItemManager->useGun(item.handle, shotInfo, mInventory->mAmmoCounter);
}

double Player::useMeleeWeapon(item_t item) {
  v3d_t world_head_pos = getHeadPosition();

  v3d_t targAngle = v3d_normalize(v3d_sub(mLookTarget, world_head_pos));
  v3d_t displacement = v3d_scale(targAngle, 1.5);
  v3d_t pos = v3d_add(world_head_pos, displacement);

  shot_info_t shotInfo;
  shotInfo.angle = targAngle;
  shotInfo.ownerPhysicsHandle = mGameModel->mPhysics->getPlayerHandle();
  shotInfo.position = pos;
  shotInfo.time = mGameModel->mPhysics->getLastUpdateTime();

  return mGameModel->mItemManager->useMeleeWeapon(item.handle, shotInfo);
}

melee_weapon_state_t* Player::getMeleeWeaponState(int hand) {
  melee_weapon_state_t* weaponState;

  if (hand == EQUIP_PRIMARY) {
    weaponState = &mMeleeStatePrimary;
    weaponState->weaponHandle = mInventory->mPrimaryItem;
    weaponState->swingTime = mLastUpdateTime - mMeleeStatePrimary.swingStart;
  }
  else {
    weaponState = &mMeleeStateSecondary;
    weaponState->weaponHandle = mInventory->mSecondaryItem;
    weaponState->swingTime = mLastUpdateTime - mMeleeStateSecondary.swingStart;
  }

  weaponState->swingMode = 2; // OMG

  weaponState->facing = mFacing;
  weaponState->incline = mIncline;
  weaponState->headPosition = v3d_add(mPos, mFinalHeadOffset);
  weaponState->handPosition = v3d_v(0.7, -0.7, 0.2);

  return weaponState;
}

void Player::useBackpackItem() {
  // can't use a non-item
  size_t selectedBackpackItemHandle = mInventory->getSelectedBackpackItemHandle();
  if (selectedBackpackItemHandle <= 0) return;

  item_t item = mGameModel->mItemManager->getItem(selectedBackpackItemHandle);

  switch (item.type) {
  case ITEMTYPE_UNDEFINED:
  case ITEMTYPE_ROCKET_PACK:
  case ITEMTYPE_GUN_ONE_HANDED:
  case ITEMTYPE_GUN_TWO_HANDED:
    // nothing to do...
    break;
  default:
    printf("player using item\n");
    if (mGameModel->mItemManager->useItem(computeWalkVector(mWalkInput), selectedBackpackItemHandle, mGameModel->mPhysics->getPlayerHandle())) {
      mInventory->getBackpack()->removeItem(selectedBackpackItemHandle);
    }
    break;
  }
}

v2d_t Player::computeWalkVector(v2d_t walkInput) {
  v2d_t facingVector = v2d_v(cos(mFacing), sin(mFacing));
  v2d_t foreBackTranslation = v2d_scale(facingVector, walkInput.y);
  v2d_t strafeTranslation = v2d_scale(v2d_normal(facingVector), walkInput.x);
  return v2d_normalize(v2d_add(foreBackTranslation, strafeTranslation));
}

void Player::updateTargetBlock() {
  v3d_t eye = v3d_add(mPos, mFinalHeadOffset);
  double rayLength = 2.75;
  v3d_t finalTarget = v3d_add(eye, v3d_scale(rayLength, v3d_normalize(v3d_sub(mLookTarget, eye))));

  if (mGameModel->mLocation->getWorldMap()->rayCastSolidBlock(eye, finalTarget, mTargetBlock, mTargetBlockFace)) {
    mIsBlockTargeted = true;
  }
  else {
    mIsBlockTargeted = false;
  }
}

v3di_t* Player::getTargetBlock(int& targetBlockFace) {
  if (mIsBlockTargeted) {
    targetBlockFace = mTargetBlockFace;
    return &mTargetBlock;
  }
  return NULL;
}

void Player::applyMovementInput(MovementInput* movementInput, AssetManager* assetManager) {
  mWalkInput = movementInput->walkInput;

  mLastUpdateTime = mGameModel->mPhysics->getLastUpdateTime();
  int headBobbleAction = HEADBOB_ACTION_STAND;

  PhysicsEntity* physicsEntity = mGameModel->mPhysics->getEntityByHandle(mGameModel->mPhysics->getPlayerHandle());
  // update position from physics
  // FIXME: this is a little flaky ... why not just use the source?
  mPos = physicsEntity->pos;
  readMessages(assetManager);

  // FIXME: this isn't quite right, perhaps the near clip plane needs to be compensated
  // for (vertically) to head pos
  block_t* block = mGameModel->mLocation->getWorldMap()->getBlock(getHeadPosition());
  // FIXME: doing this here feels a little wrong
  if (block == NULL) {
    mHeadPostionBlockType = BLOCK_TYPE_AIR;
  }
  else {
    mHeadPostionBlockType = block->type;
  }

  mHeadInWater = gBlockData.get(mHeadPostionBlockType)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID;

  // deal with the health
  mCurrentHealth = physicsEntity->health;
  if (mCurrentHealth > mMaxHealth) {
    mCurrentHealth = mMaxHealth;
    mGameModel->mPhysics->setHealth(mGameModel->mPhysics->getPlayerHandle(), mCurrentHealth);
  }

  if (isDead()) {
    if (!deathScreamUttered) {
      // TODO: extract into handleDeath()
      assetManager->mSoundSystem.playSoundByHandle(SOUND_HUMAN_DEATH, 112);
      deathScreamUttered = true;

      // put the player close to the ground
      // FIXME: this needs to be formalized
      mFinalHeadOffset = v3d_v(0.25, 0.2, 0.25);
    }

    return;
  }

  // FIXME: should really call something like: WorldMap::damageBlock()...
  if (mMeleeStatePrimary.hasUsed) {
    if (mIsBlockTargeted) {
      mGameModel->mLocation->getWorldMap()->clearBlock(mTargetBlock);
    }
    mMeleeStatePrimary.hasUsed = false;
  }
  if (mMeleeStateSecondary.hasUsed) {
    if (mIsBlockTargeted) {
      mGameModel->mLocation->getWorldMap()->clearBlock(mTargetBlock);
    }
    mMeleeStateSecondary.hasUsed = false;
  }

  if (physicsEntity->worldViscosity < 0.01) {
    mInWater = false;

    // jump from ground
    if (movementInput->isJumping && mGameModel->mPhysics->isHandleOnGround(mGameModel->mPhysics->getPlayerHandle())) {
      v3d_t force = v3d_v(0.0, 45000.0, 0.0);
      mGameModel->mPhysics->add_force(mGameModel->mPhysics->getPlayerHandle(), force);

      if (r_numi(0, 16) == 3) {
        assetManager->mSoundSystem.playSoundByHandle(SOUND_HUMAN_JUMP, 72);
      }
    }

    // so the player wants to walk eh?
    if (v2d_mag(mWalkInput) > EPSILON) {
      v2d_t walk_force_2d;

      // player on ground
      if (mGameModel->mPhysics->isHandleOnGround(mGameModel->mPhysics->getPlayerHandle())) {
        walk_force_2d = v2d_scale(computeWalkVector(mWalkInput), 2500.0);

        if (mLastUpdateTime > (mLastFootStep + 0.6) && v3d_mag(physicsEntity->vel) > 0.2) {
          assetManager->mSoundSystem.playSoundByHandle(SOUND_FOOTSTEP, r_numi (32, 56));
          mLastFootStep = mLastUpdateTime + r_num(0.0, 0.2);
        }

        headBobbleAction = HEADBOB_ACTION_WALK_FORWARD;
      }
      else {  // player not on ground
        walk_force_2d = v2d_scale(computeWalkVector(mWalkInput), 300.0);
      }

      v3d_t force = v3d_v(walk_force_2d.x, 0.0, walk_force_2d.y);
      mGameModel->mPhysics->add_force(mGameModel->mPhysics->getPlayerHandle(), force);
    }

  /*
    // FLYING * * * * * * * * *
    if (gameInput->isUsingSecondary() && !phys.isHandleOnGround(mPhysicsHandle)) {
      // glide
      v3d_t liftVector = v3d_v(0.0, 0.0, 0.0);

      v3d_t velocity = physEntity.vel;

      if (velocity.y > 0) {
        velocity.y = 0;
      }

      double speed = v3d_mag(velocity);

      if (speed > 0.0) {

        v2d_t walkVector = walk_vec(walk);

        double liftMagnitude = 80.0 * speed;

        if (liftMagnitude > 5000.0) {
          liftMagnitude = 5000.0;
        }

  //      liftVector.y += 1.0;
  //      liftVector.x += 1.5 * walkVector.x;
  //      liftVector.z += 1.5 * walkVector.y;


        // get the player facing vector
        v3d_t tempVec = mLookVector;
        tempVec.y -= 1.0;

        liftVector = v3d_cross(mLookVector, tempVec);
        liftVector = v3d_cross(mLookVector, liftVector);

        v3d_t normalizedForce = v3d_normalize(liftVector);

  //      if (walkVector.y < 0.0) {
  //        normalizedForce.x = -normalizedForce.x;
  //        normalizedForce.z = -normalizedForce.z;
  //      }

        if (v2d_mag (walkVector) > EPSILON) {
          normalizedForce.x += 0.5 * walkVector.x;
          normalizedForce.z += 0.5 * walkVector.y;

          v3d_t thrustVector = v3d_v(0.0, 500.0, 0.0);

          phys.add_force(mPhysicsHandle, thrustVector);
        }

        liftVector = v3d_normalize(normalizedForce);

        liftVector = v3d_scale(liftMagnitude, liftVector);

  //      printf("lift: %6.6f\n", liftMagnitude);
  //      v3d_print("liftVector", liftVector);
  //      v3d_print("normalizedForce", normalizedForce);
  //      v3d_print("physEntity.vel", physEntity.vel);

        phys.add_force(mPhysicsHandle, liftVector);
      }
    }
    // end FLYING
  */

  }
  else { // in water
    if (mInWater == false) {
      assetManager->mSoundSystem.playSoundByHandle(SOUND_SPLASH_MEDIUM, 72);
    }
    mInWater = true;

    v3d_t swimForce = v3d_zero();

    if (physicsEntity->on_ground && movementInput->isJumping) {
      mGameModel->mPhysics->add_force(mGameModel->mPhysics->getPlayerHandle(), v3d_v(0.0, 4000.0, 0.0));
    }

    if (v2d_mag(mWalkInput) > EPSILON) {
      v2d_t walkVector = computeWalkVector(mWalkInput);
      v3d_t force = v3d_zero();

      if (mWalkInput.y > 0.0) { // w is pressed
        double mag = 2500.0 * (sin(mLastUpdateTime * 10.0) + 1.5);
        force = v3d_scale(mag, v3d_normalize(mLookVector));

        if (mHeadInWater) {
          force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
        }
      }
      else if (mWalkInput.y < 0.0) { // s is pressed
        double mag = -2500.0 * (sin(mLastUpdateTime * 6.0) + 1.5);
        force = v3d_scale(mag, v3d_normalize(mLookVector));

        if (mHeadInWater) {
          force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
        }
      }
      if (mWalkInput.x != 0.0) {
        double mag = 2500.0 * (sin (mLastUpdateTime * 6.0) + 1.5);
        force.x += mag * walkVector.x;
        force.z += mag * walkVector.y;

        if (mHeadInWater) {
          force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
        }
      }

      swimForce = v3d_add(swimForce, force);
    }
    if (movementInput->isSwimming) {
      // 'up' force
      double mag = 1500.0 * (sin(mLastUpdateTime * 10.0) + 1.5);
      mGameModel->mPhysics->add_force(mGameModel->mPhysics->getPlayerHandle(), v3d_v(0.0, mag, 0.0));
    }

    swimForce = v3d_scale(1900.0, v3d_normalize(swimForce));
    mGameModel->mPhysics->add_force(mGameModel->mPhysics->getPlayerHandle(), swimForce);
  }

  mHeadBobble.update(headBobbleAction, mLastUpdateTime);

  v3d_t headBobbleOffset = mHeadBobble.getOffset();
  v3d_t rotatedHeadBobble = {
    headBobbleOffset.x * cos(mFacing) - headBobbleOffset.z * sin(mFacing),
    headBobbleOffset.y,
    headBobbleOffset.x * sin(mFacing) + headBobbleOffset.z * cos(mFacing)
  };
  mFinalHeadOffset = v3d_add(mHeadOffset, rotatedHeadBobble);

  updateOrientation(movementInput->facingDelta, movementInput->inclinationDelta);
}

void Player::readMessages(AssetManager* assetManager) {
  Message message;

  size_t itemHandle;
  bool gotItem;

  while (mGameModel->mMessageBus->getNextMessage((int)mGameModel->mPhysics->getPlayerHandle(), &message)) {
    switch (message.type) {
    case MESSAGE_DAMAGE:
      if (message.dValue >= 1.0 &&  mCurrentHealth > 0.0) {
        assetManager->mSoundSystem.playSoundByHandle(SOUND_HUMAN_PAIN, 96);
      }
      break;

    case MESSAGE_ITEMGRAB:
      itemHandle = message.iValue;

      gotItem = pickUpItem(mGameModel->mItemManager->getItem(itemHandle), assetManager);

      if (gotItem) {
        message.sender = mGameModel->mPhysics->getPlayerHandle();
        message.recipient = MAILBOX_PHYSICS;
        message.type = MESSAGE_ITEMGRABBED;
        mGameModel->mMessageBus->sendMessage(message);
      }

      break;
    default:
      break;
    }
  }
}

void Player::placeLight(GameInput* gameInput) {
  if (mPlacedBlock) {
    mPlacedBlock = false;

    // FIXME: only works if in primary position
    if (gameInput->isClickMouse1() && mIsBlockTargeted) {
      v3di_t neighborPos = v3di_add(mTargetBlock, gBlockNeighborAddLookup[mTargetBlockFace]);
      v3d_t pos = {
        (double)neighborPos.x + 0.5,
        (double)neighborPos.y + 0.5,
        (double)neighborPos.z + 0.5
      };

      IntColor color = {
        LIGHT_LEVEL_MAX,
        LIGHT_LEVEL_MAX,
        LIGHT_LEVEL_MAX
      };
      mGameModel->mLocation->getLightManager()->addLight(pos, 10.0, color, *mGameModel->mLocation->getWorldMap());
    }
  }
}

Inventory* Player::getInventory() {
  return mInventory;
}

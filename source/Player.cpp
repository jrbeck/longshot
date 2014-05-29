#include "Player.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)


player_c::player_c(GameModel* gameModel) {
  mGameModel = gameModel;
  mInventory.resizeBackpack(DEFAULT_BACKPACK_SIZE);
}


player_c::~player_c () {
}


int player_c::reset(size_t physicsHandle, size_t aiHandle) {
  mStartLocationFound = false;
  mStartPosition = v3d_zero();

  // set up the head displacement
  mHeadOffset = v3d_v(0.25, 1.6, 0.25);
  mFinalHeadOffset = mHeadOffset;

  mFacing = DEG2RAD(DEFAULT_FACING);

  mIncline = DEG2RAD(DEFAULT_INCLINE);
  mInclineMin = DEG2RAD(DEFAULT_INCLINE_MIN);
  mInclineMax = DEG2RAD(DEFAULT_INCLINE_MAX);

  // setup the camera
  cam.resize_screen(SCREEN_W, SCREEN_H);
  cam.set_fov_near_far(45.0, 0.15, 500.0);

  // figure out where we're looking
  update_target();

  // set up to be straight up
  up = v3d_v(0, 1, 0);

  mLastUpdateTime = 0.0;
  mNextShotTimePrimary = 0.0;
  mNextShotTimeSecondary = 0.0;

  mInventory.clear();

  // give the man a pointy stick
  int inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->generateRandomGun(1.0);
  }

  // melee weapon
  inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->generateRandomMeleeWeapon(3.0);
  }

  // block
  inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    item_t blockItem;
    memset(&blockItem, 0, sizeof item_t);

    blockItem.active = true;
    blockItem.type = ITEMTYPE_WORLD_BLOCK;
    strcpy(blockItem.name, "blocker");

    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->createItem(blockItem);
  }

  mVisionTint[0] = 0.0;
  mVisionTint[1] = 0.0;
  mVisionTint[2] = 0.0;
  mVisionTint[3] = 0.0;

  mShowCharacterSheet = false;

  mPlacedBlock = false;

  return 0;
}



// reset everything but the physics handle
int player_c::soft_reset(v3d_t& startPosition) {
  mHeadOffset = v3d_v(0.25, 1.6, 0.25);
  mFinalHeadOffset = mHeadOffset;
  mHeadBobble.reset();

  mIsBlockTargeted = false;

  // find a good place to call home
  if (!mStartLocationFound) {
    setStartPosition(startPosition);
  }
  mGameModel->physics->set_pos(mGameModel->physics->getPlayerHandle(), mStartPosition);

  mMaxHealth = 100.0;
  mCurrentHealth = 100.0;
  mGameModel->physics->setHealth(mGameModel->physics->getPlayerHandle(), mCurrentHealth);

  deathScreamUttered = false;

  mInWater = false;
  mHeadInWater = false;

  mLastFootStep = -1.0;

  mShowCharacterSheet = false;

  memset(&mMeleeStatePrimary, 0, sizeof melee_weapon_state_t);
  memset(&mMeleeStateSecondary, 0, sizeof melee_weapon_state_t);

  return 0;
}


void player_c::setStartPosition(v3d_t& startPosition) {
    mStartPosition = startPosition;
    mStartLocationFound = true;
}


void player_c::godMode() {
  printf ("you naughty little stinker you...\n");

  // some ammo?
  for (int i = 0; i < NUM_AMMO_TYPES; i++) {
    mInventory.mAmmoCounter[i] = 10000;
  }

  // how bout a couple a nice shooters
  item_t gun;

  int inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    mGameModel->itemManager->generateRandomRocketLauncher(gun, 1.5);
    gun.type = ITEMTYPE_GUN_ONE_HANDED;
    gun.bulletType = OBJTYPE_LIVE_BULLET;
    gun.ammoType = AMMO_BULLET;
    gun.explosionForce = 5000.0; // * 0.2;
    mGameModel->itemManager->nameGun(gun);

    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->createItem(gun);
  }

  // okay one more...
  inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    mGameModel->itemManager->generateRandomRocketLauncher(gun, 3.0);
//		gun.bulletType = OBJTYPE_NAPALM;
//		gun.ammoType = AMMO_NAPALM;

    gun.bulletType = OBJTYPE_SLIME;
    gun.ammoType = AMMO_SLIME;

    gun.explosionForce = 5000.0 * 0.2;
    mGameModel->itemManager->nameGun(gun);

    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->createItem(gun);
  }

  // and the rocket pack
  inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle >= 0) {
    gun.type = ITEMTYPE_ROCKET_PACK;
    gun.value1 = 2000.0;
    sprintf(gun.name, "rocket pack");
    mInventory.mBackpack[inventoryHandle] = mGameModel->itemManager->createItem(gun);
  }
}

// constrain the viewing angles
int player_c::constrain_view_angles(void) {
  // constrain the view angle elevation
  if (mIncline < mInclineMin) mIncline = mInclineMin;
  else if (mIncline > mInclineMax) mIncline = mInclineMax;

  // this is ugly, but really shouldn't ever be a problem
  while (mFacing > MY_2PI) mFacing -= MY_2PI;
  while (mFacing < 0) mFacing += MY_2PI;

  return 0;
}

v2d_t player_c::getFacingAndIncline(void) {
  return v2d_v(mFacing, mIncline);
}

bool player_c::isHeadInWater(void) {
  return mHeadInWater;
}

// update the camera target
void player_c::update_target(void) {
  mLookVector = v3d_getLookVector(mFacing, mIncline);

  // now shift to the position
  mTarget = v3d_add(v3d_add(mFinalHeadOffset, mPos), mLookVector);
}

void player_c::set_draw_distance(double distance) {
  cam.set_far(distance);
}

// change the far clipping plane distance
void player_c::adjust_draw_distance(double amount) {
  cam.adjust_far(amount);
}

// use the gluLookAt () to set view at render time
// also set the frustum
gl_camera_c player_c::gl_cam_setup(void) {
  cam.perspective();
  cam.look_at(v3d_add(mPos, mFinalHeadOffset), mTarget, up);
  return cam;
}

// return the pos vector
v3d_t player_c::get_pos(void) {
  return mPos;
}

// change the camera position
int player_c::set_pos(v3d_t a) {
  mPos = a;
  return 0;
}

bool player_c::pickUpItem(item_t item, AssetManager& assetManager) {
  // special items first:
  if (item.type == ITEMTYPE_AMMO_BULLET ||
    item.type == ITEMTYPE_AMMO_SLIME ||
    item.type == ITEMTYPE_AMMO_PLASMA ||
    item.type == ITEMTYPE_AMMO_NAPALM)
  {
    mInventory.mAmmoCounter[item.ammoType] += item.quantity;
    assetManager.mSoundSystem.playSoundByHandle(SOUND_HEALTHPACK, 192);
    return true;
  }

  // now for inventory items
  int inventoryHandle = mInventory.getNextFreeBackpackSlot();
  if (inventoryHandle < 0) {
    printf("inventory full!\n");
    return false;
  }
  else {
    mInventory.mBackpack[inventoryHandle] = item.handle;
  }

  switch (item.type) {
    case ITEMTYPE_HEALTHPACK:
      assetManager.mSoundSystem.playSoundByHandle(SOUND_HEALTHPACK, 192);
      break;
    
    case ITEMTYPE_GUN_ONE_HANDED:
      assetManager.mSoundSystem.playSoundByHandle(SOUND_PISTOL_RELOAD, 192);
      break;

    default:
      printf ("player_c::pickUpItem(): warning! unknown item type");
      break;
  }

  return true;
}

void player_c::useEquipped(int whichEquip) {
  item_t item;
  double time = mGameModel->physics->getLastUpdateTime();

  switch (whichEquip) {
    case EQUIP_PRIMARY:
      // can't use a non-item	
      if (mInventory.mPrimaryItem <= 0) return;
      item = mGameModel->itemManager->getItem(mInventory.mPrimaryItem);
      break;

    case EQUIP_SECONDARY:
      if (mInventory.mSecondaryItem <= 0) return;
      item = mGameModel->itemManager->getItem(mInventory.mSecondaryItem);
      break;

    default:
      printf("player_c::useEquipped(): trying to use invalid equip location\n");
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
    if (mGameModel->itemManager->useItem(obtainWalkVector(mWalkInput), item.handle, mGameModel->physics->getPlayerHandle())) {
      if (whichEquip == EQUIP_PRIMARY) {
        mInventory.mPrimaryItem = 0;
      }
      else {
        mInventory.mSecondaryItem = 0;
      }
    }
  }
}

double player_c::fireGun(item_t item, double handedness) {
  v3d_t world_head_pos = v3d_add(mFinalHeadOffset, mPos);

  double shoulderOffset = handedness * 0.5;
  double gunMuzzleOffset = 0.65;
  v3d_t gunMuzzle = v3d_scale(shoulderOffset, v3d_normalize(v3d_cross(mLookVector, v3d_v(0.0, 1.0, 0.0))));
  gunMuzzle = v3d_add(v3d_scale(gunMuzzleOffset, mLookVector), gunMuzzle);

  v3d_t temp = v3d_sub(mTarget, world_head_pos);
  v3d_t targAngle = v3d_normalize(temp);

  v3d_t pos = v3d_v(world_head_pos.x + temp.x + gunMuzzle.x,
            world_head_pos.y + temp.y - 0.35 + gunMuzzle.y,
            world_head_pos.z + temp.z + gunMuzzle.z);

  shot_info_t shotInfo;
  shotInfo.angle = targAngle;
  shotInfo.ownerPhysicsHandle = mGameModel->physics->getPlayerHandle();
  shotInfo.position = pos;
  shotInfo.time = mGameModel->physics->getLastUpdateTime();

  return mGameModel->itemManager->useGun(item.handle, shotInfo, mInventory.mAmmoCounter);
}

double player_c::useMeleeWeapon(item_t item) {
  v3d_t world_head_pos = v3d_add(mFinalHeadOffset, mPos);
  
  v3d_t targAngle = v3d_normalize(v3d_sub(mTarget, world_head_pos));
  v3d_t displacement = v3d_scale(targAngle, 1.5);
  v3d_t pos = v3d_add(world_head_pos, displacement);

  shot_info_t shotInfo;
  shotInfo.angle = targAngle;
  shotInfo.ownerPhysicsHandle = mGameModel->physics->getPlayerHandle();
  shotInfo.position = pos;
  shotInfo.time = mGameModel->physics->getLastUpdateTime();

  return mGameModel->itemManager->useMeleeWeapon(item.handle, shotInfo);
}

melee_weapon_state_t* player_c::getMeleeWeaponState(int hand) {
  melee_weapon_state_t* weaponState;

  if (hand == EQUIP_PRIMARY) {
    weaponState = &mMeleeStatePrimary;
    weaponState->weaponHandle = mInventory.mPrimaryItem;
    weaponState->swingTime = mLastUpdateTime - mMeleeStatePrimary.swingStart;
  }
  else {
    weaponState = &mMeleeStateSecondary;
    weaponState->weaponHandle = mInventory.mSecondaryItem;
    weaponState->swingTime = mLastUpdateTime - mMeleeStateSecondary.swingStart;
  }

  weaponState->swingMode = 2; // OMG

  weaponState->facing = mFacing;
  weaponState->incline = mIncline;
  weaponState->headPosition = v3d_add(mPos, mFinalHeadOffset);
  weaponState->handPosition = v3d_v(0.7, -0.7, 0.2);

  return weaponState;
}

void player_c::useBackpackItem() {
  // can't use a non-item
  if (mInventory.mBackpack[mInventory.mSelectedBackpackItem] <= 0) return;

  item_t item = mGameModel->itemManager->getItem(mInventory.mBackpack[mInventory.mSelectedBackpackItem]);
  
  switch (item.type) {
  case ITEMTYPE_UNDEFINED:
  case ITEMTYPE_ROCKET_PACK:
  case ITEMTYPE_GUN_ONE_HANDED:
  case ITEMTYPE_GUN_TWO_HANDED:
    // nothing to do...
    break;
  default:
    printf("player using item\n");
    if (mGameModel->itemManager->useItem(obtainWalkVector(mWalkInput), mInventory.mBackpack[mInventory.mSelectedBackpackItem], mGameModel->physics->getPlayerHandle())) {
      mInventory.mBackpack[mInventory.mSelectedBackpackItem] = 0;
    }
    break;
  }
}

// gives the normalized walk vector
v2d_t player_c::obtainWalkVector(v2d_t walkInput) {
  // get a vector pointing the way the camera is facing
  v2d_t t1 = v2d_v(cos(mFacing), sin(mFacing));

  v2d_t t_fb; // translation, forward or back
  v2d_t t_lr; // translation, left and right

  // this is the forward-backward translation
  t_fb = v2d_scale(t1, walkInput.y);

  // this is the side-side translation
  t_lr = v2d_scale(v2d_normal(t1), walkInput.x);

  // get the new position
  t1 = v2d_normalize(v2d_add(t_fb, t_lr));

  return t1;
}

void player_c::updateTargetBlock() {
  v3d_t eye = v3d_add(mPos, mFinalHeadOffset);

  double rayLength = 2.75;

  v3d_t finalTarget = v3d_add(eye, v3d_scale(rayLength, v3d_normalize(v3d_sub(mTarget, eye))));

  if (mGameModel->location->getWorldMap()->rayCastSolidBlock(eye, finalTarget, mTargetBlock, mTargetBlockFace)) {
    mIsBlockTargeted = true;
  }
  else {
    mIsBlockTargeted = false;
  }
}

v3di_t* player_c::getTargetBlock(int& targetBlockFace) {
  if (mIsBlockTargeted) {
    targetBlockFace = mTargetBlockFace;
    return &mTargetBlock;
  }
  return NULL;
}

void player_c::updateHud() {
  mHud.clear();

  GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  v2d_t fontSize = { 0.01f, 0.02f };

  static char text[64];

  // let's give some indication of health status
  static char healthString[50];
  sprintf(healthString, "health %.0f\0", mCurrentHealth);
  mHud.addText(v2d_v(0.4, 0.0), v2d_v(0.2, 0.05), fontSize, healthString, TEXT_JUSTIFICATION_CENTER, color, NULL);

  // display the primary equip
  if (mInventory.mPrimaryItem != 0) {
    item_t currentEquip = mGameModel->itemManager->getItem(mInventory.mPrimaryItem);
    sprintf(text, "%s\0", currentEquip.name);
    mHud.addText(v2d_v(0.0, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);

    // display the ammo
    switch (currentEquip.type) {
      case ITEMTYPE_GUN_ONE_HANDED:
        sprintf(text, "ammo: %d\0", mInventory.mAmmoCounter[currentEquip.ammoType]);
        mHud.addText(v2d_v(0.0, 0.05), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);
        break;
    }
  }
  else {
    sprintf(text, "nothing equipped\0");
    mHud.addText(v2d_v(0.0, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);
  }


  // display the secondary equip
  if (mInventory.mSecondaryItem != 0) {
    item_t currentEquip = mGameModel->itemManager->getItem(mInventory.mSecondaryItem);
    sprintf (text, "%s\0", currentEquip.name);
    mHud.addText (v2d_v (0.8, 0.0), v2d_v (0.2, 0.05), fontSize,
      text, TEXT_JUSTIFICATION_RIGHT, color, NULL);

    // display the ammo
    switch (currentEquip.type) {
      case ITEMTYPE_GUN_ONE_HANDED:
        sprintf (text, "ammo: %d\0", mInventory.mAmmoCounter[currentEquip.ammoType]);
        mHud.addText (v2d_v (0.8, 0.05), v2d_v (0.2, 0.05), fontSize,
          text, TEXT_JUSTIFICATION_RIGHT, color, NULL);
        break;

      default:
        break;
    }
  }
  else {
    sprintf(text, "nothing equipped\0");
    mHud.addText(v2d_v(0.8, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_RIGHT, color, NULL);
  }

  // well this seems a bit random to be finding right here...
  fontSize = v2d_v(0.01f, 0.01f);

  // crosshair? ha!
  static char crossHair[2] = "x";

  color[3] = 0.75f;
  mHud.addText(v2d_v (0.495, 0.5), v2d_v (0.01, 0.01), fontSize, crossHair, TEXT_JUSTIFICATION_CENTER, color, NULL);


  // player position
//	sprintf(text, "%.4f, %.4f, %.4f", mPos.x, mPos.y, mPos.z);
//	mHud.addText (v2d_v(0.4, 0.1), v2d_v(0.2, 0.05), fontSize,
//		text, TEXT_JUSTIFICATION_CENTER, color, NULL);
}

void player_c::drawHud() {
  if (mHeadInWater && mVisionTint[3] > 0.0) {
    drawWaterOverlay();
  }

  if (mShowCharacterSheet) {
    mCharacterSheet.draw();
  }
  else {
    mHud.draw();
  }
}

void player_c::drawWaterOverlay() {
  // need to set up the opengl viewport
  glPushMatrix();

  glViewport(0, 0, SCREEN_W, SCREEN_H);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, 1, 1, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  //glColor4f(0.1f, 0.1f, 0.5f, 0.5f);
  glColor4fv(mVisionTint);

  glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
  glEnd();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

  glPopMatrix();
}

void player_c::updateCharacterSheet() {
  mCharacterSheet.clear();

  static GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat color2[4] = { 0.8f, 0.6f, 0.2f, 1.0f };
//	static GLfloat colorBlack[4] = {0.0f, 0.0f, 0.0f, 0.6f};
  static GLfloat selectedColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  
  static v2d_t fontSize = { 0.015f, 0.03f };

  // equipped
  mCharacterSheet.addText(v2d_v(0.1, 0.2), v2d_v(0.3, 0.05), fontSize, "primary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mInventory.mPrimaryItem != 0) {
    item_t item = mGameModel->itemManager->getItem(mInventory.mPrimaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet.addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet.addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  mCharacterSheet.addText(v2d_v(0.1, 0.3), v2d_v(0.3, 0.05), fontSize, "secondary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mInventory.mSecondaryItem != 0) {
    item_t item = mGameModel->itemManager->getItem(mInventory.mSecondaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet.addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet.addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  // ammo
  v2d_t tl, dimensions;
  double buttonHeight = (0.9 - 0.7) / static_cast<double>(NUM_AMMO_TYPES);
  char ammoString[50];

  for (size_t i = 0; i < NUM_AMMO_TYPES; i++) {
    tl = v2d_v(0.1, lerp(0.7, 0.9 - buttonHeight, i, NUM_AMMO_TYPES));
//		br = v2d_v (0.4, lerp (0.7 + (buttonHeight * 0.9), 0.9, i, NUM_AMMO_TYPES));;
    dimensions.x = 0.3;
    dimensions.y = buttonHeight * 0.9;

    // FIXME: switch in a for loop? show some class man!
    switch (i) {
      case AMMO_BULLET:
        sprintf(ammoString, "bullets: %d\0", mInventory.mAmmoCounter[i]);
        break;
      case AMMO_SLIME:
        sprintf(ammoString, "slime: %d\0", mInventory.mAmmoCounter[i]);
        break;
      case AMMO_PLASMA:
        sprintf(ammoString, "plasma: %d\0", mInventory.mAmmoCounter[i]);
        break;
      case AMMO_NAPALM:
        sprintf(ammoString, "napalm: %d\0", mInventory.mAmmoCounter[i]);
        break;
      default:
        sprintf(ammoString, "undefined: %d\0", mInventory.mAmmoCounter[i]);
        break;
    }

    mCharacterSheet.addText(tl, dimensions, fontSize, ammoString, TEXT_JUSTIFICATION_LEFT, color, NULL);
  }


  // inventory
  mCharacterSheet.addText(v2d_v(0.5, 0.2), v2d_v(0.4, 0.1), fontSize, "inventory:", TEXT_JUSTIFICATION_CENTER, color2, NULL);

  buttonHeight = (0.9 - 0.3) / static_cast<double>(mInventory.mBackpack.size());
  GLfloat *itemColor;

  for (size_t i = 0; i < mInventory.mBackpack.size(); i++) {
    tl = v2d_v(0.5, lerp(0.3, 0.9 - buttonHeight, i, mInventory.mBackpack.size()));
//		br = v2d_v (0.9, lerp (0.3 + (buttonHeight * 0.9), 0.9, i, mInventory.mBackpack.size ()));;
    dimensions.x = 0.4;
    dimensions.y = buttonHeight * 0.9;

    if (i == mInventory.mSelectedBackpackItem) {
      itemColor = selectedColor;
    }
    else {
      itemColor = color;
    }

    if (mInventory.mBackpack[i] != 0) {
      item_t item = mGameModel->itemManager->getItem(mInventory.mBackpack[i]);
      if (item.type != ITEMTYPE_UNDEFINED) {
        mCharacterSheet.addText(tl, dimensions, fontSize, item.name, TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
      }
    }
    else {
      mCharacterSheet.addText(tl, dimensions, fontSize, "---", TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
    }
  }

  // let's give some indication of health status
  static char tempString[50];
  sprintf(tempString, "health %.0f\0", mCurrentHealth);
  mCharacterSheet.addText(v2d_v(0.1, 0.1), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);

  // money money money
  sprintf(tempString, "credits %d\0", mInventory.mCredits);
  mCharacterSheet.addText(v2d_v(0.1, 0.6), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);
}

bool player_c::update(AssetManager& assetManager) {
  mLastUpdateTime = mGameModel->physics->getLastUpdateTime();
  int headBobbleAction = HEADBOB_ACTION_STAND;

  GameInput& gi = *mGameModel->gameInput;

  PhysicsEntity *physicsEntity = mGameModel->physics->getEntityByHandle(mGameModel->physics->getPlayerHandle());
  // get position from physics
  mPos = physicsEntity->pos;

  // FIXME: this isn't quite right, perhaps the near clip plane needs to be compensated
  // for (vertically) to head pos
  block_t *block = mGameModel->location->getWorldMap()->getBlock(v3d_add(mFinalHeadOffset, mPos));
  int blockType;
  if (block == NULL) {
    // default to BLOCK_TYPE_AIR
    blockType = BLOCK_TYPE_AIR;
  }
  else {
//		blockType = worldMap.getBlock(v3d_add (mFinalHeadOffset, mPos))->type;
    blockType = block->type;
  }

  if (gBlockData.get(blockType)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
    if (!mHeadInWater) {
      // entering water
      if (gBlockData.get(blockType)->visionTint[3] > 0.0f) {
        mVisionTint[0] = gBlockData.get(blockType)->visionTint[0];
        mVisionTint[1] = gBlockData.get(blockType)->visionTint[1];
        mVisionTint[2] = gBlockData.get(blockType)->visionTint[2];
        mVisionTint[3] = gBlockData.get(blockType)->visionTint[3];
      }
      adjust_draw_distance(-450.0);
    }
    mHeadInWater = true;
  }
  else {
    if (mHeadInWater) {
      // leaving water
      adjust_draw_distance(450.0);
      mVisionTint[3] = 0.0f;
    }
    mHeadInWater = false;
  }

  readPhysicsMessages(assetManager);

  // deal with the health
  mCurrentHealth = physicsEntity->health;
  if (mCurrentHealth > mMaxHealth) {
    mCurrentHealth = mMaxHealth;
    mGameModel->physics->setHealth(mGameModel->physics->getPlayerHandle(), mCurrentHealth);
  }

  // does the player want to mess with the draw distance?
  if (gi.isDecreasingDrawDistance()) {
    adjust_draw_distance(-20);
  }
  if (gi.isIncreasingDrawDistance()) {
    adjust_draw_distance(20);
  }


  // deal with an escape press
  bool escapePressed = false;
  if (gi.isEscapePressed()) {
    if (mShowCharacterSheet) {
      mShowCharacterSheet = false;
    }
    else {
      escapePressed = true;
    }
  }

  // accommodate the changes in orientation
  mFacing += gi.getFacingDelta();
  mIncline += gi.getInclinationDelta();
  constrain_view_angles();

  // update the camera accordingly
  update_target();

  // check if there's a block that the player is looking at and close to
  updateTargetBlock();


  // * * * * * * begin dead section * * * * * * * * * * *
  // if dead, player can't do any more
  if (mCurrentHealth <= 0.0) {
//		printf ("dead\n");

    if (!deathScreamUttered) {
      assetManager.mSoundSystem.playSoundByHandle(SOUND_HUMAN_DEATH, 112);
      deathScreamUttered = true;

      // put the player close to the ground
      // FIXME: this needs to be formalized
      mFinalHeadOffset = v3d_v(0.25, 0.2, 0.25);
    }

    mHud.clear();

    // let's give some indication of health status
    char healthString[50];
    float color[] = {1.0f, 1.0f, 1.0f, 0.8f};
    v2d_t fontSize = { 0.01f, 0.02f };

    sprintf(healthString, "health: %.0f\0", mCurrentHealth);
    mHud.addText(v2d_v(0.00, 0.05), v2d_v(0.2, 0.04), fontSize, healthString, TEXT_JUSTIFICATION_LEFT, color, NULL);
    mHud.addText(v2d_v(0.00, 0.00), v2d_v(0.2, 0.04), fontSize, "press (f1) or (esc)", TEXT_JUSTIFICATION_LEFT, color, NULL);

    if (gi.isSoftReset()) {
      mMaxHealth = 100.0;
      mCurrentHealth = 100.0;

      mGameModel->physics->setHealth(mGameModel->physics->getPlayerHandle(), 100.0);

      mFinalHeadOffset = mHeadOffset;

      deathScreamUttered = false;
    }

    return escapePressed;
  }
  // * * * * * * end dead section * * * * * * * * * * *



  if (mShowCharacterSheet) {
    if (gi.isClickMouse1()) {
      if (mInventory.mBackpack[mInventory.mSelectedBackpackItem] == 0) {
        mInventory.swapBackPackItemIntoPrimary();
      }
      else {
        mInventory.swapBackPackItemIntoPrimary();
      }
    }
    if (gi.isClickMouse2()) {
      if (mInventory.mBackpack[mInventory.mSelectedBackpackItem] == 0) {
        mInventory.swapBackPackItemIntoSecondary();
      }
      else {
        mInventory.swapBackPackItemIntoSecondary();
      }
    }

    if (gi.isUseBackpackItem()) {
      item_t item = mGameModel->itemManager->getItem(mInventory.mBackpack[mInventory.mSelectedBackpackItem]);

      if (item.type == ITEMTYPE_HEALTHPACK) {
        useBackpackItem();
      }
    }

    if (gi.isNextGun()) {
      mInventory.nextBackPackItem();
    }

    if (gi.isPreviousGun()) {
      mInventory.previousBackPackItem();
    }

    // did the player want to drop that item?
    // FIXME: destroys item!
    // this needs to create a physics item...
    if (gi.isDroppedItem() && mInventory.mBackpack[mInventory.mSelectedBackpackItem] != 0) {
      mGameModel->itemManager->destroyItem(mInventory.mBackpack[mInventory.mSelectedBackpackItem]);
      mInventory.mBackpack[mInventory.mSelectedBackpackItem] = 0;
    }
  } // end mShowCharacterSheet == true
  else { // if (!mShowCharacterSheet) {
    if (gi.isUsingPrimary()) {
      useEquipped(EQUIP_PRIMARY);
    }
    if (gi.isUsingSecondary()) {
      useEquipped(EQUIP_SECONDARY);
    }
  }

/*
  // HACK TO PLACE BLOCKS * * * * * *
  if (mPlacedBlock) {
    mPlacedBlock = false;

    // FIXME: only works if in primary position
    if (gi.isClickMouse1 () && mIsBlockTargeted) {
      v3di_t neighborPos = v3di_add(mTargetBlock, gBlockNeighborAddLookup[mTargetBlockFace]);

      BoundingBox bb (v3d_v (1.0, 1.0, 1.0), v3d_v (neighborPos));

      // FIXME: hack that makes it easier for the player to place some
      // blocks. Could result in a player being inside a block.
      bb.scale (0.998);
      bb.translate (v3d_v (0.001, 0.001, 0.001));

      if (!bb.isIntersecting (phys.getEntityByHandle (mPhysicsHandle)->boundingBox)) {
//				worldMap.fillSphere (v3d_v (neighborPos), 3.0, BLOCK_TYPE_GREEN_STAR_TILE, 0);
        worldMap.setBlockType (neighborPos, BLOCK_TYPE_GREEN_STAR_TILE);
      }
    }
  }
*/

  // FIXME: should really call something like: WorldMap::damageBlock()...
  // snap to it!
  if (mMeleeStatePrimary.hasUsed) {
    if (mIsBlockTargeted) {
      mGameModel->location->getWorldMap()->clearBlock(mTargetBlock);
    }
    mMeleeStatePrimary.hasUsed = false;
  }
  if (mMeleeStateSecondary.hasUsed) {
    if (mIsBlockTargeted) {
      mGameModel->location->getWorldMap()->clearBlock(mTargetBlock);
    }
    mMeleeStateSecondary.hasUsed = false;
  }


  // does the player wanna pick stuff up?
  if (gi.isPickUpItem()) {
    phys_message_t message;
    message.sender = mGameModel->physics->getPlayerHandle();
    message.recipient = MAILBOX_PHYSICS;
    message.type = PHYS_MESSAGE_PLAYERPICKUPITEMS;

    // this sends a message to everyone who needs to know that the player
    // wants to pick something up
    mGameModel->physics->sendMessage(message);
  }

  // deal with a soft reset
  if (gi.isSoftReset()) {
    soft_reset(mStartPosition);

    // this stuff has changed, so take note!
    physicsEntity = mGameModel->physics->getEntityByHandle(mGameModel->physics->getPlayerHandle());
  }

  updateHud();

  // toggle the character sheet on/off according to the user input
  if (gi.isToggleCharacterSheet()) {
    mShowCharacterSheet = !mShowCharacterSheet;
  }
  
  updateCharacterSheet();

  if (gi.isToggleGodMode()) {
    godMode();
  }

  if (gi.isTogglePhysics()) mGameModel->physics->togglePause();
  if (gi.isAdvanceOneFrame()) mGameModel->physics->advanceOneFrame();


  mWalkInput = v2d_v(0.0, 0.0);
  if (gi.isWalkingForward())		mWalkInput.y += 1.0;
  if (gi.isWalkingBackward())	mWalkInput.y -= 1.0;
  if (gi.isWalkingLeft())		mWalkInput.x += 1.0;
  if (gi.isWalkingRight())		mWalkInput.x -= 1.0;

  bool isJumping = gi.isJumping();

  if (physicsEntity->worldViscosity < 0.01) {
    mInWater = false;

    // jump from ground
    if (isJumping && mGameModel->physics->isHandleOnGround(mGameModel->physics->getPlayerHandle())) {
      v3d_t force = v3d_v(0.0, 45000.0, 0.0);

      mGameModel->physics->add_force(mGameModel->physics->getPlayerHandle(), force);

      if (r_numi(0, 16) == 3) {
        assetManager.mSoundSystem.playSoundByHandle(SOUND_HUMAN_JUMP, 72);
      }
    }

    // so the player wants to walk eh?
    if (v2d_mag(mWalkInput) > EPSILON) {
      v2d_t walk_force_2d;

      // player on ground
      if (mGameModel->physics->isHandleOnGround(mGameModel->physics->getPlayerHandle())) {
        walk_force_2d = v2d_scale(obtainWalkVector(mWalkInput), 2500.0);

        if (mLastUpdateTime > (mLastFootStep + 0.6) && v3d_mag(physicsEntity->vel) > 0.2) {
          assetManager.mSoundSystem.playSoundByHandle(SOUND_FOOTSTEP, r_numi (32, 56));
          mLastFootStep = mLastUpdateTime + r_num(0.0, 0.2);
        }

        headBobbleAction = HEADBOB_ACTION_WALK_FORWARD;
      }
      else {	// player not on ground
        walk_force_2d = v2d_scale(obtainWalkVector(mWalkInput), 300.0);
      }

      v3d_t force = v3d_v(walk_force_2d.x, 0.0, walk_force_2d.y);
      mGameModel->physics->add_force(mGameModel->physics->getPlayerHandle(), force);
    }

  /*	if (isJumping && !phys.isHandleOnGround (mPhysicsHandle)) {
      v3d_t force = v3d_v (0.0, 33000.0, 0.0);

      phys.add_force (mPhysicsHandle, force);
    }*/

  /*
    // FLYING * * * * * * * * *
    if (gi.isUsingSecondary () && !phys.isHandleOnGround (mPhysicsHandle)) {
      // glide
      v3d_t liftVector = v3d_v (0.0, 0.0, 0.0);

      v3d_t velocity = physEntity.vel;

      if (velocity.y > 0) {
        velocity.y = 0;
      }

      double speed = v3d_mag (velocity);

      if (speed > 0.0) {

        v2d_t walkVector = walk_vec (walk);

        double liftMagnitude = 80.0 * speed;

        if (liftMagnitude > 5000.0) {
          liftMagnitude = 5000.0;
        }

  //			liftVector.y += 1.0;
  //			liftVector.x += 1.5 * walkVector.x;
  //			liftVector.z += 1.5 * walkVector.y;


        // get the player facing vector
        v3d_t tempVec = mLookVector;
        tempVec.y -= 1.0;

        liftVector = v3d_cross (mLookVector, tempVec);
        liftVector = v3d_cross (mLookVector, liftVector);

        v3d_t normalizedForce = v3d_normalize (liftVector);

  //			if (walkVector.y < 0.0) {
  //				normalizedForce.x = -normalizedForce.x;
  //				normalizedForce.z = -normalizedForce.z;
  //			}

        if (v2d_mag (walkVector) > EPSILON) {
          normalizedForce.x += 0.5 * walkVector.x;
          normalizedForce.z += 0.5 * walkVector.y;

          v3d_t thrustVector = v3d_v (0.0, 500.0, 0.0);

          phys.add_force (mPhysicsHandle, thrustVector);
        }

        liftVector = v3d_normalize (normalizedForce);

        liftVector = v3d_scale (liftMagnitude, liftVector);

  //			printf ("lift: %6.6f\n", liftMagnitude);
  //			v3d_print ("liftVector", liftVector);
  //			v3d_print ("normalizedForce", normalizedForce);
  //			v3d_print ("physEntity.vel", physEntity.vel);

        phys.add_force (mPhysicsHandle, liftVector);
      }
    }
    // end FLYING
  */

  }
  else { // in water
    if (mInWater == false) {
      assetManager.mSoundSystem.playSoundByHandle(SOUND_SPLASH_MEDIUM, 72);
    }
    mInWater = true;

    v3d_t swimForce = v3d_zero();

    if (physicsEntity->on_ground && isJumping) {
      mGameModel->physics->add_force(mGameModel->physics->getPlayerHandle(), v3d_v(0.0, 4000.0, 0.0));
    }

    if (v2d_mag(mWalkInput) > EPSILON) {
      v2d_t walkVector = obtainWalkVector(mWalkInput);
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
    if (gi.isSwimming()) {
      // 'up' force
      double mag = 1500.0 * (sin(mLastUpdateTime * 10.0) + 1.5);
      mGameModel->physics->add_force(mGameModel->physics->getPlayerHandle(), v3d_v(0.0, mag, 0.0));
    }

    swimForce = v3d_scale(1900.0, v3d_normalize(swimForce));
    mGameModel->physics->add_force(mGameModel->physics->getPlayerHandle(), swimForce);
  }


  // now we know what the player is doing, so let's update this
  mHeadBobble.update(headBobbleAction, mLastUpdateTime);

  v3d_t headBobbleOffset = mHeadBobble.getOffset();
  v3d_t rotatedHeadBobble = {
    headBobbleOffset.x * cos(mFacing) - headBobbleOffset.z * sin(mFacing),
    headBobbleOffset.y,
    headBobbleOffset.x * sin(mFacing) + headBobbleOffset.z * cos(mFacing) };
  mFinalHeadOffset = v3d_add(mHeadOffset, rotatedHeadBobble);

  return escapePressed;
}


void player_c::readPhysicsMessages(AssetManager& assetManager) {
  phys_message_t message;

  size_t itemHandle;
  bool gotItem;

  while (mGameModel->physics->getNextMessage((int)mGameModel->physics->getPlayerHandle(), &message)) {
//		printf ("player message: to: %d, from: %d\n", message.recipient, message.sender);

    switch (message.type) {
      case PHYS_MESSAGE_DAMAGE:
        if (message.dValue >= 1.0 &&  mCurrentHealth > 0.0) {
          assetManager.mSoundSystem.playSoundByHandle(SOUND_HUMAN_PAIN, 96);
        }
        break;

      case PHYS_MESSAGE_ITEMGRAB:
        itemHandle = message.iValue;

        gotItem = pickUpItem(mGameModel->itemManager->getItem(itemHandle), assetManager);

        if (gotItem) {
          message.sender = mGameModel->physics->getPlayerHandle();
          message.recipient = MAILBOX_PHYSICS;
          message.type = PHYS_MESSAGE_ITEMGRABBED;
//					message.iValue2 = message.iValue2;

          mGameModel->physics->sendMessage(message);
        }

        break;

      default:
        break;
    }
  }
}




void player_c::placeLight() {
  if (mPlacedBlock) {
    mPlacedBlock = false;

    // FIXME: only works if in primary position
    if (mGameModel->gameInput->isClickMouse1() && mIsBlockTargeted) {
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
      mGameModel->location->getLightManager()->addLight(pos, 10.0, color, *mGameModel->location->getWorldMap());
    }
  }
}


Inventory *player_c::getInventory(void) {
  return &mInventory;
}


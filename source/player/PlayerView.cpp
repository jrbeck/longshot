#include "../player/PlayerView.h"

PlayerView::PlayerView(GameModel* gameModel, AssetManager* assetManager) :
  mGameModel(NULL),
  mAssetManager(NULL),
  mHud(NULL),
  mCharacterSheet(NULL),
  mShowCharacterSheet(false)
{
  mGameModel = gameModel;
  mAssetManager = assetManager;
  mHud = new GameMenu;
  mCharacterSheet = new GameMenu;

  mCamera.resize_screen(gScreenW, gScreenH);
  mCamera.set_fov_near_far(45.0, 0.15, 500.0);
  setDrawDistance(500.0);
  mVisionTint[0] = 0.0;
  mVisionTint[1] = 0.0;
  mVisionTint[2] = 0.0;
  mVisionTint[3] = 0.0;
}

PlayerView::~PlayerView() {
  if (mHud != NULL) {
    delete mHud;
  }
  if (mCharacterSheet != NULL) {
    delete mCharacterSheet;
  }
}

void PlayerView::drawEquipped() {
  drawEquipped(mGameModel->mPlayer->getMeleeWeaponState(EQUIP_PRIMARY), LEFT_HANDED);
  drawEquipped(mGameModel->mPlayer->getMeleeWeaponState(EQUIP_SECONDARY), RIGHT_HANDED);
}

void PlayerView::drawEquipped(const melee_weapon_state_t* weaponState, double handedness) {
  item_t item = mGameModel->mItemManager->getItem(weaponState->weaponHandle);

  if (item.type == ITEMTYPE_MELEE_ONE_HANDED &&
    weaponState->swingTime >= 0.0 &&
    weaponState->swingTime <= 0.4)
  {
    GLuint modelDisplayListHandle = mAssetManager->mModelDisplayListHandles[mGameModel->mItemManager->getItem(weaponState->weaponHandle).gunType];
    drawMeleeWeapon(weaponState, modelDisplayListHandle);
  }
  else if (item.type == ITEMTYPE_GUN_ONE_HANDED) {
    drawEquippedGun(weaponState, handedness, mAssetManager->mGunBitmapModel);
  }
}

void PlayerView::drawEquippedGun(const melee_weapon_state_t* weaponState, double handedness, BitmapModel* model) {
  v3d_t handPosition = v3d_v(0.0, -0.1, 0.0);
  v3d_t offset = v3d_v(0.2, 0.0, handedness * 0.2);

  glEnable(GL_TEXTURE_2D);
  model->bindTexture();
  glColor4d(1.0, 1.0, 1.0, 1.0);

  glEnable(GL_BLEND);
  glAlphaFunc(GL_GREATER, 0.9f);
  glEnable(GL_ALPHA_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();

  glTranslated(weaponState->headPosition.x, weaponState->headPosition.y, weaponState->headPosition.z);
  glTranslated(handPosition.x, handPosition.y, handPosition.z);
  glRotated(RAD2DEG(-weaponState->facing), 0.0, 1.0, 0.0);
  glRotated(RAD2DEG(weaponState->incline), 0.0, 0.0, 1.0);
  glTranslated(offset.x, offset.y, offset.z);
//  glScaled(1.0, 1.0, 1.0);
  model->draw();

  glPopMatrix();

  glDisable(GL_ALPHA_TEST);
}

void PlayerView::drawMeleeWeapon(const melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle) {
  double handFacing = weaponState->facing + (MY_PI / 4.0);

  v3d_t headPosition = weaponState->headPosition;
  v3d_t handPosition = weaponState->handPosition; //v3d_v (0.7, -0.7, 0.2);

  double swing;

  if (weaponState->swingMode == 0) {
    swing = 7.0 * (MY_PI / 8.0);
    // or (0.225 * sin (9.0 * MY_PI / 8.0));
  }
  else if (weaponState->swingMode == 1) {
    swing = (0.10 * sin(weaponState->swingTime * 5.0) - (MY_PI / 6.0));

  }
  else {
    swing = (0.45 * sin(((weaponState->swingTime) * 8.0) + (MY_PI / 2.0)));
    if (swing < 0.0)  {
      swing *= 5.0;
    }
  }

  glDisable(GL_TEXTURE_2D);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glPushMatrix();
  glTranslated(headPosition.x, headPosition.y, headPosition.z);
  glRotated(RAD2DEG(-handFacing), 0.0, 1.0, 0.0);
  glRotated(RAD2DEG(weaponState->incline), 1.0, 0.0, 1.0);
  //    glScaled (-1.0, 1.0, 1.0);
  glTranslated(handPosition.x, handPosition.y, handPosition.z);
  glRotated(RAD2DEG(swing), 1.0, 0.0, 0.0);

  glBegin(GL_QUADS);
    glCallList(modelDisplayListHandle);
  glEnd();

  glPopMatrix();
}

void PlayerView::drawPlayerTargetBlock() {
  int targetFace;
  v3di_t* playerTarg = mGameModel->mPlayer->getTargetBlock(targetFace);
  if (playerTarg == NULL) {
    return;
  }

  v3d_t nearCorner = v3d_v(*playerTarg);
  v3d_t farCorner = v3d_add(nearCorner, v3d_v(1.01, 1.01, 1.01));
  nearCorner = v3d_add(nearCorner, v3d_v(-0.01, -0.01, -0.01));

  /*
  v3d_t nearCorner2 = v3d_v (*playerTarg);
  GLfloat faceColor[4] = { 0.5, 0.0, 0.0, 0.5 };

  // draw the targeted face

  glPushMatrix ();
  glTranslated (nearCorner2.x, nearCorner2.y, nearCorner2.z);

  glBegin (GL_QUADS);
  mAssetManager.drawBlockFace (targetFace, faceColor);
  glEnd ();
  glPopMatrix ();
  */

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glColor4f(0.3f, 0.3f, 0.3f, 1.0f);

  glBegin(GL_LINES);
  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);
  glEnd();
}

void PlayerView::updateVisionTint() {
  BlockTypeData* headPositionBlockTypeData = gBlockData.get(mGameModel->mPlayer->getHeadPostionBlockType());
  if (headPositionBlockTypeData->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
    if (headPositionBlockTypeData->visionTint[3] > 0.0f) {
      mVisionTint[0] = headPositionBlockTypeData->visionTint[0];
      mVisionTint[1] = headPositionBlockTypeData->visionTint[1];
      mVisionTint[2] = headPositionBlockTypeData->visionTint[2];
      mVisionTint[3] = headPositionBlockTypeData->visionTint[3];
    }
    // adjustDrawDistance(-450.0);
  }
  else {
    // adjustDrawDistance(450.0);
    mVisionTint[3] = 0.0f;
  }
}

void PlayerView::updateHud() {
  mHud->clear();

  // // toggle the character sheet on/off according to the user input
  // if (gameInput->isToggleCharacterSheet()) {
  //   mShowCharacterSheet = !mShowCharacterSheet;
  // }
  //
  updateCharacterSheet();

  updateVisionTint();

  if (mGameModel->mPlayer->getCurrentHealth() <= 0.0f) {
    showDeadPlayerHud();
  }
  else { // NOT DEAD
    showLivePlayerHud();
  }

  // TODO: move this crosshair biz outta here
  v2d_t fontSize = { 0.01f, 0.01f };
  static char crossHair[2] = "x";
  float color[] = {1.0f, 1.0f, 1.0f, 0.75f};
  mHud->addText(v2d_v(0.495, 0.5), v2d_v(0.01, 0.01), fontSize, crossHair, TEXT_JUSTIFICATION_CENTER, color, NULL);


  // player position
//  sprintf(text, "%.4f, %.4f, %.4f", mPos.x, mPos.y, mPos.z);
//  mHud->addText (v2d_v(0.4, 0.1), v2d_v(0.2, 0.05), fontSize,
//    text, TEXT_JUSTIFICATION_CENTER, color, NULL);
}

void PlayerView::showDeadPlayerHud() {
  char healthString[50];
  float color[] = {1.0f, 1.0f, 1.0f, 0.8f};
  v2d_t fontSize = { 0.01f, 0.02f };

  sprintf(healthString, "health: %.0f\0", mGameModel->mPlayer->getCurrentHealth());
  mHud->addText(v2d_v(0.00, 0.05), v2d_v(0.2, 0.04), fontSize, healthString, TEXT_JUSTIFICATION_LEFT, color, NULL);
  mHud->addText(v2d_v(0.00, 0.00), v2d_v(0.2, 0.04), fontSize, "press (f1) or (esc)", TEXT_JUSTIFICATION_LEFT, color, NULL);
  return;
}

void PlayerView::showLivePlayerHud() {
  GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  v2d_t fontSize = { 0.01f, 0.02f };

  static char text[64];

  // let's give some indication of health status
  static char healthString[50];
  sprintf(healthString, "health %.0f\0", mGameModel->mPlayer->getCurrentHealth());
  mHud->addText(v2d_v(0.4, 0.0), v2d_v(0.2, 0.05), fontSize, healthString, TEXT_JUSTIFICATION_CENTER, color, NULL);

  // display the primary equip
  if (mGameModel->mPlayer->getInventory()->mPrimaryItem != 0) {
    item_t currentEquip = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mPrimaryItem);
    sprintf(text, "%s\0", currentEquip.name);
    mHud->addText(v2d_v(0.0, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);

    // display the ammo
    switch (currentEquip.type) {
      case ITEMTYPE_GUN_ONE_HANDED:
        sprintf(text, "ammo: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[currentEquip.ammoType]);
        mHud->addText(v2d_v(0.0, 0.05), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);
        break;
    }
  }
  else {
    sprintf(text, "nothing equipped\0");
    mHud->addText(v2d_v(0.0, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_LEFT, color, NULL);
  }

  // display the secondary equip
  if (mGameModel->mPlayer->getInventory()->mSecondaryItem != 0) {
    item_t currentEquip = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mSecondaryItem);
    sprintf (text, "%s\0", currentEquip.name);
    mHud->addText (v2d_v(0.8, 0.0), v2d_v(0.2, 0.05), fontSize,
      text, TEXT_JUSTIFICATION_RIGHT, color, NULL);

    // display the ammo
    switch (currentEquip.type) {
      case ITEMTYPE_GUN_ONE_HANDED:
        sprintf (text, "ammo: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[currentEquip.ammoType]);
        mHud->addText (v2d_v(0.8, 0.05), v2d_v(0.2, 0.05), fontSize,
          text, TEXT_JUSTIFICATION_RIGHT, color, NULL);
        break;

      default:
        break;
    }
  }
  else {
    sprintf(text, "nothing equipped\0");
    mHud->addText(v2d_v(0.8, 0.0), v2d_v(0.2, 0.05), fontSize, text, TEXT_JUSTIFICATION_RIGHT, color, NULL);
  }
}

void PlayerView::drawHud() {
  updateHud();

  if (mVisionTint[3] > 0.0) {
    drawWaterOverlay();
  }

  if (mShowCharacterSheet) {
    mCharacterSheet->draw();
  }
  else {
    mHud->draw();
  }
}

void PlayerView::drawWaterOverlay() {
  // need to set up the opengl viewport
  glPushMatrix();

  glViewport(0, 0, gScreenW, gScreenH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, 1, 1, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

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

void PlayerView::updateCharacterSheet() {
  mCharacterSheet->clear();

  static GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat color2[4] = { 0.8f, 0.6f, 0.2f, 1.0f };
//  static GLfloat colorBlack[4] = {0.0f, 0.0f, 0.0f, 0.6f};
  static GLfloat selectedColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

  static v2d_t fontSize = { 0.015f, 0.03f };

  // equipped
  mCharacterSheet->addText(v2d_v(0.1, 0.2), v2d_v(0.3, 0.05), fontSize, "primary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mGameModel->mPlayer->getInventory()->mPrimaryItem != 0) {
    item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mPrimaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet->addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet->addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  mCharacterSheet->addText(v2d_v(0.1, 0.3), v2d_v(0.3, 0.05), fontSize, "secondary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mGameModel->mPlayer->getInventory()->mSecondaryItem != 0) {
    item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mSecondaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet->addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet->addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  // ammo
  v2d_t tl, dimensions;
  double buttonHeight = (0.9 - 0.7) / static_cast<double>(NUM_AMMO_TYPES);
  char ammoString[50];

  for (size_t i = 0; i < NUM_AMMO_TYPES; ++i) {
    tl = v2d_v(0.1, lerp(0.7, 0.9 - buttonHeight, i, NUM_AMMO_TYPES));
//    br = v2d_v(0.4, lerp (0.7 + (buttonHeight * 0.9), 0.9, i, NUM_AMMO_TYPES));;
    dimensions.x = 0.3;
    dimensions.y = buttonHeight * 0.9;

    // FIXME: switch in a for loop? show some class man!
    switch (i) {
      case AMMO_BULLET:
        sprintf(ammoString, "bullets: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_SLIME:
        sprintf(ammoString, "slime: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_PLASMA:
        sprintf(ammoString, "plasma: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_NAPALM:
        sprintf(ammoString, "napalm: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      default:
        sprintf(ammoString, "undefined: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
    }

    mCharacterSheet->addText(tl, dimensions, fontSize, ammoString, TEXT_JUSTIFICATION_LEFT, color, NULL);
  }


  // inventory
  mCharacterSheet->addText(v2d_v(0.5, 0.2), v2d_v(0.4, 0.1), fontSize, "inventory:", TEXT_JUSTIFICATION_CENTER, color2, NULL);

  buttonHeight = (0.9 - 0.3) / static_cast<double>(mGameModel->mPlayer->getInventory()->mBackpack.size());
  GLfloat *itemColor;

  for (size_t i = 0; i < mGameModel->mPlayer->getInventory()->mBackpack.size(); ++i) {
    tl = v2d_v(0.5, lerp(0.3, 0.9 - buttonHeight, i, mGameModel->mPlayer->getInventory()->mBackpack.size()));
//    br = v2d_v(0.9, lerp (0.3 + (buttonHeight * 0.9), 0.9, i, mGameModel->mPlayer->getInventory()->mBackpack.size()));;
    dimensions.x = 0.4;
    dimensions.y = buttonHeight * 0.9;

    if (i == mGameModel->mPlayer->getInventory()->mSelectedBackpackItem) {
      itemColor = selectedColor;
    }
    else {
      itemColor = color;
    }

    if (mGameModel->mPlayer->getInventory()->mBackpack[i] != 0) {
      item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mBackpack[i]);
      if (item.type != ITEMTYPE_UNDEFINED) {
        mCharacterSheet->addText(tl, dimensions, fontSize, item.name, TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
      }
    }
    else {
      mCharacterSheet->addText(tl, dimensions, fontSize, "---", TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
    }
  }

  // let's give some indication of health status
  static char tempString[50];
  sprintf(tempString, "health %.0f\0", mGameModel->mPlayer->getCurrentHealth());
  mCharacterSheet->addText(v2d_v(0.1, 0.1), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);

  // money money money
  sprintf(tempString, "credits %d\0", mGameModel->mPlayer->getInventory()->mCredits);
  mCharacterSheet->addText(v2d_v(0.1, 0.6), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);
}

void PlayerView::setDrawDistance(double distance) {
  mCamera.set_far(distance);
}

void PlayerView::adjustDrawDistance(double amount) {
  mCamera.adjust_far(amount);
}

GlCamera PlayerView::glCamSetup() {
  v3d_t up = v3d_v(0, 1, 0);

  mCamera.perspective();
  mCamera.look_at(mGameModel->mPlayer->getHeadPosition(), mGameModel->mPlayer->getLookTarget(), up);
  return mCamera;
}

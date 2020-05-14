#include "../player/PlayerView.h"

PlayerView::PlayerView(GameModel* gameModel, AssetManager* assetManager) :
  mGameModel(gameModel),
  mAssetManager(assetManager),
  mHudView(new HudView(gameModel))
{
  mVisionTint[0] = 0.0;
  mVisionTint[1] = 0.0;
  mVisionTint[2] = 0.0;
  mVisionTint[3] = 0.0;
}

PlayerView::~PlayerView() {
  delete mHudView;
}

void PlayerView::update() {
  mGameModel->mPlayer->updateCameraTarget();
  updateVisionTint();
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
  v3di_t* playerTargetBlock = mGameModel->mPlayer->getTargetBlock(targetFace);
  if (playerTargetBlock == NULL) {
    return;
  }

  v3d_t nearCorner = v3d_v(*playerTargetBlock);
  v3d_t farCorner = v3d_add(nearCorner, v3d_v(1.01, 1.01, 1.01));
  nearCorner = v3d_add(nearCorner, v3d_v(-0.01, -0.01, -0.01));

  /*
  v3d_t nearCorner2 = v3d_v(*playerTargetBlock);
  GLfloat faceColor[4] = { 0.5, 0.0, 0.0, 0.5 };

  // draw the targeted face

  glPushMatrix();
  glTranslated(nearCorner2.x, nearCorner2.y, nearCorner2.z);

  glBegin(GL_QUADS);
  mAssetManager.drawBlockFace(targetFace, faceColor);
  glEnd();
  glPopMatrix();
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

void PlayerView::drawHud() {
  mHudView->draw();
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

void PlayerView::drawWaterOverlay() {
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

#include "PlayerView.h"






void PlayerView::drawEquipped(GameModel *gameModel, AssetManager& assetManager) {
  melee_weapon_state_t* leftHand;
  melee_weapon_state_t* rightHand;
  gameModel->player->getMeleeWeaponStates(leftHand, rightHand);

  item_t primaryItem = gameModel->itemManager->getItem(rightHand->weaponHandle);
  item_t secondaryItem = gameModel->itemManager->getItem(leftHand->weaponHandle);

  if (primaryItem.type == ITEMTYPE_MELEE_ONE_HANDED &&
    rightHand->swingTime >= 0.0 &&
    rightHand->swingTime <= 0.4)
  {
    GLuint modelDisplayListHandle = assetManager.mModelDisplayListHandles[gameModel->itemManager->getItem(rightHand->weaponHandle).gunType];
    drawMeleeWeapon(rightHand, modelDisplayListHandle);
  }
  else if (primaryItem.type == ITEMTYPE_GUN_ONE_HANDED) {
    drawEquippedGun(rightHand, LEFT_HANDED, *assetManager.mGunBitmapModel);
  }

  if (secondaryItem.type == ITEMTYPE_MELEE_ONE_HANDED &&
    leftHand->swingTime >= 0.0 &&
    leftHand->swingTime <= 0.4)
  {
    GLuint modelDisplayListHandle = assetManager.mModelDisplayListHandles[gameModel->itemManager->getItem(leftHand->weaponHandle).gunType];
    drawMeleeWeapon(leftHand, modelDisplayListHandle);
  }
  else if (secondaryItem.type == ITEMTYPE_GUN_ONE_HANDED) {
    drawEquippedGun(leftHand, RIGHT_HANDED, *assetManager.mGunBitmapModel);
  }
}

void PlayerView::drawEquippedGun(melee_weapon_state_t* weaponState, double handedness, BitmapModel& model) {
  v3d_t handPosition = v3d_v(0.0, -0.1, 0.0);
  v3d_t offset = v3d_v(0.2, 0.0, 0.2);

  glEnable(GL_TEXTURE_2D);
  model.bindTexture();
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
  glTranslated(offset.x, offset.y, handedness * offset.z);
  glScaled(1.0, 1.0, 1.0);
  model.draw();

  glPopMatrix();

  glDisable(GL_ALPHA_TEST);
}




void PlayerView::drawMeleeWeapon(melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle) {
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
  //		glScaled (-1.0, 1.0, 1.0);
  glTranslated(handPosition.x, handPosition.y, handPosition.z);
  glRotated(RAD2DEG(swing), 1.0, 0.0, 0.0);

  glBegin(GL_QUADS);
    glCallList(modelDisplayListHandle);
  glEnd();

  glPopMatrix();
}





void PlayerView::drawPlayerTargetBlock(player_c* player) {
  int targetFace;
  v3di_t* playerTarg = player->getTargetBlock(targetFace);
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






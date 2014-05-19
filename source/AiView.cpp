#include "AiView.h"



AiView::AiView() {
  for (int i = 0; i < NUM_AITYPES; i++) {
    mHeadTextureHandles[i] = 0;
  }
  for (int i = 0; i < NUM_AISKINS; i++) {
    mTextureHandles[i] = 0;
  }

  loadAssets();
}


AiView::~AiView() {
  freeAssets();
}


void AiView::setAiEntities(vector<AiEntity *> *aiEntities) {
  mAiEntities = aiEntities;
}


void AiView::loadAssets(void) {
  mHeadTextureHandles[AITYPE_HOPPER] = AssetManager::loadImg("art/32_head_tiger.png");
  //	mHeadTextureHandles[AITYPE_HOPPER] = AssetManager::loadImg("art/32_head_peopleeater.bmp");
  mHeadTextureHandles[AITYPE_SHOOTER] = AssetManager::loadImg("art/32_head_shooter.bmp");
  mHeadTextureHandles[AITYPE_HUMAN] = AssetManager::loadImg("art/32_head_human.bmp");

  mTextureHandles[AISKIN_DUMMY] = AssetManager::loadImg("art/32_ai_dummy.bmp");
  mTextureHandles[AISKIN_BALLOON] = AssetManager::loadImg("art/32_ai_balloon.bmp");
  mTextureHandles[AISKIN_BALLOON_DEAD] = AssetManager::loadImg("art/32_ai_balloon_dead.bmp");
  mTextureHandles[AISKIN_SHOOTER] = AssetManager::loadImg("art/32_ai_shooter.bmp");
  mTextureHandles[AISKIN_SHOOTER_DEAD] = AssetManager::loadImg("art/32_ai_shooter_dead.bmp");
  mTextureHandles[AISKIN_HOPPER] = AssetManager::loadImg("art/32_ai_tiger.bmp");
  //	mTextureHandles[AISKIN_HOPPER] = AssetManager::loadImg("art/32_ai_peopleeater.bmp");
  mTextureHandles[AISKIN_HOPPER_DEAD] = AssetManager::loadImg("art/32_ai_tiger_dead.bmp");
  mTextureHandles[AISKIN_HUMAN] = AssetManager::loadImg("art/32_ai_human.bmp");
  mTextureHandles[AISKIN_HUMAN_DEAD] = AssetManager::loadImg("art/32_ai_human_dead.bmp");
}



void AiView::freeAssets(void) {
  for (int i = 0; i < NUM_AITYPES; i++) {
    if (mHeadTextureHandles[i] > 0) {
      glDeleteTextures(1, &mHeadTextureHandles[i]);
      mHeadTextureHandles[i] = 0;
    }
  }
  for (int i = 0; i < NUM_AISKINS; i++) {
    if (mTextureHandles[i] > 0) {
      glDeleteTextures(1, &mTextureHandles[i]);
      mTextureHandles[i] = 0;
    }
  }
}



void AiView::draw(
  WorldMap &worldMap,
  ItemManager &itemManager,
  const LightManager &lightManager)
{
  glEnable(GL_TEXTURE_2D);
  size_t numEntities = mAiEntities->size();
  for (size_t i = 0; i < numEntities; i++) {
    drawEntity(
      worldMap,
      itemManager,
      *(*mAiEntities)[i],
      lightManager);
  }
}



void AiView::drawEntity(
  WorldMap &worldMap,
  ItemManager &itemManager,
  AiEntity &aiEntity,
  const LightManager &lightManager)
{
  if (aiEntity.mType == AITYPE_PLAYER) {
    drawWeapon(aiEntity, worldMap, itemManager);
    // WARNING: this will not draw players!
    return;
  }

  // WARNING: this is a hack to fix the problem of explosions being rendered
  // with AI skins...
  if (aiEntity.mPhysicsEntity->type != OBJTYPE_AI_ENTITY) {
    return;
  }

  if (aiEntity.mType == AITYPE_SHOOTER) {
    drawWeapon(aiEntity, worldMap, itemManager);
  }

  // draw the critter's body
  drawBody(aiEntity, worldMap, lightManager);

  // move the head around
  updateHeadOrientation(aiEntity);

  // draw the head
  drawHead(aiEntity, worldMap, lightManager);
}


void AiView::updateHeadOrientation(AiEntity &aiEntity) const {
  v3d_t targetCenter;
  if (aiEntity.mTargetPhysicsEntity == NULL) {
    v3d_zero(&targetCenter);
  }
  else {
    targetCenter = aiEntity.mTargetPhysicsEntity->boundingBox.getCenterPosition();
  }

  v3d_t headToTarget;

  // stop the head from moving if dead
  if (aiEntity.mCurrentHealth <= 0.0) {
    // that should be the last mFacingAngle
    aiEntity.mHeadOrientation.x = cos(aiEntity.mFacingAngle);
    aiEntity.mHeadOrientation.y = 0.0;
    aiEntity.mHeadOrientation.z = sin(aiEntity.mFacingAngle);

    // make them face down. aww sad keanu
    updateLookIncline(aiEntity, -MY_PI / 20.0);
  }
  else {
    if (aiEntity.mTargetPhysicsEntity != NULL &&
      v3d_mag(aiEntity.mPhysicsEntity->vel) < 0.2 &&
      aiEntity.mPhysicsEntity->on_ground)
    {
      // face towards the target
      headToTarget = v3d_normalize(v3d_sub(targetCenter, aiEntity.mWorldPosition));
    }
    else {
      // face it in the direction of velocity
      headToTarget = v3d_normalize(aiEntity.mPhysicsEntity->vel);
    }

    // move head towards target around y-axis
    updateFacingAngle(aiEntity, atan2(headToTarget.z, headToTarget.x));

    // figure out the vertical angle, constrain if too inclined
    double adjacentLength = sqrt(1.0 - (headToTarget.y * headToTarget.y));
    updateLookIncline(aiEntity, atan2(headToTarget.y, adjacentLength));

    aiEntity.mHeadOrientation = v3d_getLookVector(aiEntity.mFacingAngle, aiEntity.mLookIncline);
  }
}



void AiView::updateFacingAngle(AiEntity &aiEntity, double desiredFacingAngle) const {
  // slow down the angle change a bit
  if (desiredFacingAngle - aiEntity.mFacingAngle > MY_PI) {
    desiredFacingAngle += MY_PI * 2.0;
  }
  if (desiredFacingAngle - aiEntity.mFacingAngle < -MY_PI) {
    desiredFacingAngle += MY_PI * 2.0;
  }

  if (desiredFacingAngle > aiEntity.mFacingAngle) {
    aiEntity.mFacingAngle += 0.03;
    if (desiredFacingAngle < aiEntity.mFacingAngle) {
      aiEntity.mFacingAngle = desiredFacingAngle;
    }
  }
  if (desiredFacingAngle < aiEntity.mFacingAngle) {
    aiEntity.mFacingAngle -= 0.03;
    if (desiredFacingAngle > aiEntity.mFacingAngle) {
      aiEntity.mFacingAngle = desiredFacingAngle;
    }
  }

  if (aiEntity.mFacingAngle > MY_2PI) {
    aiEntity.mFacingAngle = fmod(aiEntity.mFacingAngle, MY_2PI);
  }
  while (aiEntity.mFacingAngle < 0.0) {
    aiEntity.mFacingAngle += MY_2PI;
  }
}




void AiView::updateLookIncline(AiEntity &aiEntity, double desiredLookIncline) const {
  if (desiredLookIncline >(MY_PI / 20.0)) {
    desiredLookIncline = (MY_PI / 20.0);
  }
  else if (desiredLookIncline < (-MY_PI / 20.0)) {
    desiredLookIncline = (-MY_PI / 20.0);
  }

  // change the head inclination
  if (desiredLookIncline > aiEntity.mLookIncline) {
    aiEntity.mLookIncline += 0.01;
    if (desiredLookIncline < aiEntity.mLookIncline) {
      aiEntity.mLookIncline = desiredLookIncline;
    }
  }
  if (desiredLookIncline < aiEntity.mLookIncline) {
    aiEntity.mLookIncline -= 0.01;
    if (desiredLookIncline > aiEntity.mLookIncline) {
      aiEntity.mLookIncline = desiredLookIncline;
    }
  }
}



void AiView::drawBody(
  AiEntity &aiEntity,
  WorldMap &worldMap,
  const LightManager &lightManager)
{
  PhysicsEntity &pe = *aiEntity.mPhysicsEntity;

  // FIXME: this isn't all necessary...
  // it's leftover from the first lighting style
  // changed (2012-05-03)
  v3d_t nearCorner = pe.boundingBox.mNearCorner;
  v3d_t farCorner = pe.boundingBox.mFarCorner;
  v3d_t dimensions = pe.boundingBox.mDimensions;

  glPushMatrix();
  // orient it how we want it
  glTranslated(
    nearCorner.x + (dimensions.x * 0.5),
    nearCorner.y + (dimensions.y * 0.5),
    nearCorner.z + (dimensions.z * 0.5));
  glScaled(dimensions.x, dimensions.y, dimensions.z);
  glRotated(RAD2DEG(-aiEntity.mFacingAngle), 0.0, 1.0, 0.0);

  // figure out the texture
  switch (aiEntity.mType) {
  case AITYPE_DUMMY:
    glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_DUMMY]);
    break;

  case AITYPE_BALLOON:
    if (pe.health > 1.0) {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_BALLOON]);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_BALLOON_DEAD]);
    }
    break;

  case AITYPE_SHOOTER:
    if (pe.health > 1.0) {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_SHOOTER]);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_SHOOTER_DEAD]);
    }
    break;

  case AITYPE_HUMAN:
    if (pe.health > 1.0) {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_HUMAN]);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_HUMAN_DEAD]);
    }
    break;

  case AITYPE_HOPPER:
    if (pe.health > 1.0) {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_HOPPER]);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mTextureHandles[AISKIN_HOPPER_DEAD]);
    }
    break;

  case AITYPE_PLAYER:
    break;


  default:
    glBindTexture(GL_TEXTURE_2D, 0);
    break;
  }

  // figure out the color
  IntColor color = getLightValue(pe.boundingBox.getCenterPosition(), worldMap, lightManager);

  // draw the box
  glBegin(GL_QUADS);
  drawBodyBlock(color);
  glEnd();

  glPopMatrix();

}




void AiView::drawHead(
  AiEntity &aiEntity,
  WorldMap &worldMap,
  const LightManager &lightManager)
{
  v3d_t headPosition;
  v3d_t headDimensions; // lol
  IntColor color;

  glPushMatrix();
  switch (aiEntity.mType) {
  case AITYPE_HUMAN:
  case AITYPE_SHOOTER:
    headPosition = v3d_add(aiEntity.mWorldPosition, v3d_scale(aiEntity.mHeadOrientation, 0.12));
    headPosition.y += 0.7;

    headDimensions = v3d_v(0.85, 0.8, 0.85);

    glTranslated(headPosition.x, headPosition.y, headPosition.z);
    glScaled(headDimensions.x, headDimensions.y, headDimensions.z);
    glRotated(RAD2DEG(-aiEntity.mFacingAngle), 0.0, 1.0, 0.0);
    glRotated(RAD2DEG(aiEntity.mLookIncline), 0.0, 0.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, mHeadTextureHandles[aiEntity.mType]);

    // get the color
    color = getLightValue(headPosition, worldMap, lightManager);
    // draw it
    glBegin(GL_QUADS);
    drawBlockWithFace(color);
    glEnd();
    break;

  case AITYPE_HOPPER:
    headPosition = v3d_add(aiEntity.mWorldPosition, v3d_scale(aiEntity.mHeadOrientation, 0.5));
    if (aiEntity.mCurrentHealth > (aiEntity.mMaxHealth * 0.25)) {
      // raise its head if it is healthy
      headPosition.y += 0.1;
    }

    headDimensions = v3d_v(0.6, 0.6, 0.6);

    glTranslated(headPosition.x, headPosition.y, headPosition.z);
    glScaled(headDimensions.x, headDimensions.y, headDimensions.z);
    glRotated(RAD2DEG(-aiEntity.mFacingAngle), 0.0, 1.0, 0.0);
    glRotated(RAD2DEG(aiEntity.mLookIncline), 0.0, 0.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, mHeadTextureHandles[aiEntity.mType]);

    // get the color
    color = getLightValue(headPosition, worldMap, lightManager);
    // draw it
    glBegin(GL_QUADS);
    drawBlockWithFace(color);
    glEnd();
    break;

  default:
    // NO HEAD FOR YOU!
    break;
  }

  glPopMatrix();
}




void AiView::drawWeapon(
  AiEntity &aiEntity,
  WorldMap &worldMap,
  ItemManager &itemManager)
{

  glDisable(GL_TEXTURE_2D);
  glPushMatrix();

  double handFacing = aiEntity.mFacingAngle + (MY_PI / 4.0);
  double handIncline = (aiEntity.mLookIncline) - (MY_PI / 6.0) + 0.0;
  double armLength = 1.0;

  v3d_t lookVector = v3d_getLookVector(aiEntity.mFacingAngle, aiEntity.mLookIncline);

  v3d_t headPosition = v3d_add(aiEntity.mWorldPosition, v3d_v(0.0, 0.7, 0.0));

  glColor4d(1.0, 1.0, 1.0, 1.0);

  v3d_t handTranslation = v3d_v(0.7, -0.7, 0.2);

  double swing;

  if (aiEntity.mCurrentHealth > 0.0) {
    // WARNING * * * * * * * * *
    // i don't want to have to rely on Physics to draw AI, so
    // this line was changed
    //swing = (0.10 * sin (physics.getLastUpdateTime () * 5.0) - (MY_PI / 6.0));
    // to this:
    swing = (0.10 * sin(((double)SDL_GetTicks() / 1000.0) * 5.0) - (MY_PI / 6.0));

    //		swing = (0.45 * sin (physics.getLastUpdateTime () * 10.0));

    //		if (swing < 0.0)  {
    //			swing *= 5.0;
    //		}
    //	}
  }
  else {
    swing = 7.0 * (MY_PI / 8.0);
  }


  glTranslated(headPosition.x, headPosition.y, headPosition.z);
  glRotated(RAD2DEG(-handFacing), 0.0, 1.0, 0.0);
  glRotated(RAD2DEG(aiEntity.mLookIncline), 1.0, 0.0, 1.0);
  glTranslated(handTranslation.x, handTranslation.y, handTranslation.z);
  glRotated(RAD2DEG(swing), 1.0, 0.0, 0.0);

  // FIXME: this is where the melee item used to be drawn
  //	itemManager.drawItem(0);

  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
}



void AiView::drawBodyBlock(IntColor color) const {
  GLfloat floatColor[4] = {
    (GLfloat)color.r * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)color.g * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)color.b * ONE_OVER_LIGHT_LEVEL_MAX,
    1.0f
  };

  // set the color once
  glColor4fv(floatColor);

  // left
  glNormal3d(-1.0, 0.0, 0.0);
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB

  // right
  glNormal3d(1.0, 0.0, 0.0);
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF

  // top
  glNormal3d(0.0, 1.0, 0.0);
  glTexCoord2f(0.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB
  glTexCoord2f(0.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB

  // bottom
  glNormal3d(0.0, -1.0, 0.0);
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(0.5f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF
  glTexCoord2f(1.0f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF

  // front
  glNormal3d(0.0, 0.0, 1.0);
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF

  // back
  glNormal3d(0.0, 0.0, -1.0);
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB	
}





void AiView::drawBlockWithFace(IntColor color) const {
  GLfloat floatColor[4] = {
    (GLfloat)color.r * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)color.g * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)color.b * ONE_OVER_LIGHT_LEVEL_MAX,
    1.0f
  };

  // set the color once
  glColor4fv(floatColor);

  // left - BACK
  glNormal3d(-1.0, 0.0, 0.0);
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB

  // right - FACE
  glNormal3d(1.0, 0.0, 0.0);
  glTexCoord2f(0.5f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB
  glTexCoord2f(0.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(0.0f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF

  // top
  glNormal3d(0.0, 1.0, 0.0);
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB
  glTexCoord2f(0.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF
  glTexCoord2f(0.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB

  // bottom
  glNormal3d(0.0, -1.0, 0.0);
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF

  // front - right
  glNormal3d(0.0, 0.0, 1.0);
  glTexCoord2f(1.0f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBF]);	// LBF
  glTexCoord2f(0.5f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBF]);	// RBF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTF]);	// RTF
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTF]);	// LTF

  // back - left
  glNormal3d(0.0, 0.0, -1.0);
  glTexCoord2f(0.5f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_RBB]);	// RBB
  glTexCoord2f(1.0f, 1.0f); glVertex3fv(cube_corner_centered[BOX_CORNER_LBB]);	// LBB
  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_LTB]);	// LTB
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner_centered[BOX_CORNER_RTB]);	// RTB	
}



IntColor AiView::getLightValue(v3d_t position, WorldMap &worldMap, const LightManager &lightManager) const {
  IntColor color = { 0, 0, 0 };

  block_t *block = worldMap.getBlock(position);
  if (block != NULL) {
    color.r += block->uniqueLighting;
    color.g += block->uniqueLighting;
    color.b += block->uniqueLighting;
  }
  else {
    // THIS MAKES IT FULL BRIGHT WHEN NOT IN A REAL
    // CHUNK...
    color.r += worldMap.mWorldLightingCeiling;
    color.b += worldMap.mWorldLightingCeiling;
    color.g += worldMap.mWorldLightingCeiling;
  }

  IntColor light = lightManager.getLightLevel(v3di_v(position));
  color.r += light.r;
  color.g += light.g;
  color.b += light.b;
  color.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);

  return color;
}





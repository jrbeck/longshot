// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiView
// *
// * displays the AiManager/AiEntities
// * part of the Model View Controller pattern
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../ai/AiEntity.h"
#include "../assets/AssetManager.h"
#include "../math/MathUtil.h"
#include "../world/LightManager.h"

enum {
  AISKIN_DUMMY,
  AISKIN_BALLOON,
  AISKIN_BALLOON_DEAD,
  AISKIN_SHOOTER,
  AISKIN_SHOOTER_DEAD,
  AISKIN_HOPPER,
  AISKIN_HOPPER_DEAD,
  AISKIN_HUMAN,
  AISKIN_HUMAN_DEAD,

  NUM_AISKINS
};

class AiView {
public:
  AiView();
  ~AiView();

  void loadAssets();
  void freeAssets();

  void draw(GameModel* gameModel);

  void drawEntity(AiEntity& aiEntity);

  void updateHeadOrientation(AiEntity& aiEntity) const;
  void updateFacingAngle(AiEntity& aiEntity, double desiredLookIncline) const;
  void updateLookIncline(AiEntity& aiEntity, double desiredLookIncline) const;

  void drawBody(AiEntity& aiEntity);
  void drawHead(AiEntity& aiEntity);
  void drawWeapon(AiEntity& aiEntity);


  void drawBodyBlock(IntColor color) const;
  void drawBlockWithFace(IntColor color) const;

  IntColor getLightValue(v3d_t position) const;

private:
  // members * * * * * * * * * * * * * * * * * * * * * * * * * * *
  WorldMap* mWorldMap;
  LightManager *mLightManager;

  // these hold the texture handles for the skins
  // FIXME: how about atlas textures? already basically doing it
  // * * * heads
  GLuint mHeadTextureHandles[NUM_AITYPES];
  // * * * bodies
  GLuint mTextureHandles[NUM_AISKINS];

};

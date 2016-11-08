#pragma once

#include "../math/MathUtil.h"
#include "../assets/AssetManager.h"
#include "../game/GameModel.h"
#include "../player/HudView.h"

class PlayerView {
public:
  PlayerView(GameModel* gameModel, AssetManager* assetManager);
  ~PlayerView();

  void update();

  void drawEquipped();
  void drawEquipped(const melee_weapon_state_t* weaponState, double handedness);
  void drawEquippedGun(const melee_weapon_state_t* weaponState, double handedness, BitmapModel* model);
  void drawMeleeWeapon(const melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle);

  void drawPlayerTargetBlock();

  void drawHud();

  void updateVisionTint();
  void drawWaterOverlay();

  void setDrawDistance(double distance);
  void adjustDrawDistance(double amount);

  GlCamera glCamSetup();

private:
  GameModel* mGameModel;
  AssetManager* mAssetManager;

  HudView* mHudView;
  GlCamera mCamera;

  bool mHeadPostionBlockType;
  GLfloat mVisionTint[4];
  v3d_t mUpVector;
};

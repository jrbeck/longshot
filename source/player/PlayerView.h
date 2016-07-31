// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PlayerView
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/MathUtil.h"
#include "../assets/AssetManager.h"
#include "../game/GameModel.h"

class PlayerView {
public:
  PlayerView(GameModel* gameModel, AssetManager* assetManager);
  ~PlayerView();

  void drawEquipped();
  void drawEquipped(const melee_weapon_state_t* weaponState, double handedness);
  void drawEquippedGun(const melee_weapon_state_t* weaponState, double handedness, BitmapModel* model);
  void drawMeleeWeapon(const melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle);

  void drawPlayerTargetBlock();

  void updateHud();
  void showDeadPlayerHud();
  void showLivePlayerHud();
  void drawHud();

  void updateVisionTint();
  void drawWaterOverlay();

  void updateCharacterSheet();

  void setDrawDistance(double distance);
  void adjustDrawDistance(double amount);

  // use the gluLookAt() to set view at render time
  GlCamera glCamSetup();

private:
  GameModel* mGameModel;
  AssetManager* mAssetManager;
  GameMenu* mHud;
  GameMenu* mCharacterSheet;

  GlCamera mCamera;

  bool mHeadPostionBlockType;
  GLfloat mVisionTint[4];

  bool mShowCharacterSheet;
};

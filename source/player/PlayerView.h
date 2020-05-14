#pragma once

#include "../math/MathUtil.h"
#include "../assets/AssetManager.h"
#include "../game/GameModel.h"
#include "../player/HudView.h"
#include "../player/CharacterSheetView.h"

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

private:
  GameModel* mGameModel;
  AssetManager* mAssetManager;

  HudView* mHudView;

  bool mHeadPostionBlockType;
  GLfloat mVisionTint[4];
};

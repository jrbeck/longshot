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
#include "../player/Player.h"

class PlayerView {

public:
  static void drawEquipped(GameModel* gameModel, AssetManager& assetManager);
  static void drawEquipped(const melee_weapon_state_t* weaponState, double handedness, GameModel* gameModel, AssetManager& assetManager);
  static void drawEquippedGun(const melee_weapon_state_t* weaponState, double handedness, BitmapModel* model);
  static void drawMeleeWeapon(const melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle);

  static void drawPlayerTargetBlock(Player* player);
};

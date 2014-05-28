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

#include "MathUtil.h"

#include "Player.h"


class PlayerView {

public:


  static void drawEquipped(GameModel *gameModel, AssetManager& assetManager);
  static void drawEquippedGun(melee_weapon_state_t* weaponState, double handedness, BitmapModel& model);
  static void drawMeleeWeapon(melee_weapon_state_t* weaponState, GLuint modelDisplayListHandle);

  static void drawPlayerTargetBlock(player_c* player);

};


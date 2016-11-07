#include "HudView.h"

HudView::HudView() :
  mHud(new GameMenu)
{
}

HudView::~HudView() {
  if (mHud != NULL) {
    delete mHud;
  }
}

void draw() {
  // if (mVisionTint[3] > 0.0) {
  //   drawWaterOverlay();
  // }

  update();
  mHud->draw();
}

void HudView::update() {
  mHud->clear();

  if (mGameModel->mPlayer->isDead()) {
    showDeadPlayerHud();
  }
  else {
    showLivePlayerHud();
  }

  // TODO: move this crosshair biz outta here
  v2d_t fontSize = { 0.01f, 0.01f };
  static char crossHair[2] = "x";
  float color[] = { 1.0f, 1.0f, 1.0f, 0.75f };
  mHud->addText(v2d_v(0.495, 0.5), v2d_v(0.01, 0.01), fontSize, crossHair, TEXT_JUSTIFICATION_CENTER, color, NULL);
}

void HudView::showDeadPlayerHud() {
  char healthString[50];
  float color[] = { 1.0f, 1.0f, 1.0f, 0.8f };
  v2d_t fontSize = { 0.01f, 0.02f };

  sprintf(healthString, "health: %.0f\0", mGameModel->mPlayer->getCurrentHealth());
  mHud->addText(v2d_v(0.00, 0.05), v2d_v(0.2, 0.04), fontSize, healthString, TEXT_JUSTIFICATION_LEFT, color, NULL);
  mHud->addText(v2d_v(0.00, 0.00), v2d_v(0.2, 0.04), fontSize, "press (f1) or (esc)", TEXT_JUSTIFICATION_LEFT, color, NULL);
  return;
}

void HudView::showLivePlayerHud() {
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

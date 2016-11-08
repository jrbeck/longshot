#include "CharacterSheetView.h"

CharacterSheetView::CharacterSheetView(GameModel* gameModel) :
  mGameModel(gameModel),
  mCharacterSheet(NULL)
{
  mCharacterSheet = new GameMenu;
}

CharacterSheetView::~CharacterSheetView() {
  if (mCharacterSheet != NULL) {
    delete mCharacterSheet;
  }
}

void CharacterSheetView::draw() {
  update();
  mCharacterSheet->draw();
}

void CharacterSheetView::update() {
  mCharacterSheet->clear();

  static GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat color2[4] = { 0.8f, 0.6f, 0.2f, 1.0f };
//  static GLfloat colorBlack[4] = {0.0f, 0.0f, 0.0f, 0.6f};
  static GLfloat selectedColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

  static v2d_t fontSize = { 0.015f, 0.03f };

  // equipped
  mCharacterSheet->addText(v2d_v(0.1, 0.2), v2d_v(0.3, 0.05), fontSize, "primary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mGameModel->mPlayer->getInventory()->mPrimaryItem != 0) {
    item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mPrimaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet->addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet->addText(v2d_v(0.1, 0.25), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  mCharacterSheet->addText(v2d_v(0.1, 0.3), v2d_v(0.3, 0.05), fontSize, "secondary", TEXT_JUSTIFICATION_CENTER, color2, NULL);
  if (mGameModel->mPlayer->getInventory()->mSecondaryItem != 0) {
    item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->mSecondaryItem);
    if (item.type != ITEMTYPE_UNDEFINED) {
      mCharacterSheet->addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, item.name, TEXT_JUSTIFICATION_CENTER, color, NULL);
    }
  }
  else {
    mCharacterSheet->addText(v2d_v(0.1, 0.35), v2d_v(0.3, 0.05), fontSize, "---", TEXT_JUSTIFICATION_CENTER, color, NULL);
  }

  // ammo
  v2d_t tl, dimensions;
  double buttonHeight = (0.9 - 0.7) / static_cast<double>(NUM_AMMO_TYPES);
  char ammoString[50];

  for (size_t i = 0; i < NUM_AMMO_TYPES; ++i) {
    tl = v2d_v(0.1, lerp(0.7, 0.9 - buttonHeight, i, NUM_AMMO_TYPES));
//    br = v2d_v(0.4, lerp (0.7 + (buttonHeight * 0.9), 0.9, i, NUM_AMMO_TYPES));;
    dimensions.x = 0.3;
    dimensions.y = buttonHeight * 0.9;

    switch (i) {
      case AMMO_BULLET:
        sprintf(ammoString, "bullets: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_SLIME:
        sprintf(ammoString, "slime: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_PLASMA:
        sprintf(ammoString, "plasma: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      case AMMO_NAPALM:
        sprintf(ammoString, "napalm: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
      default:
        sprintf(ammoString, "undefined: %lu\0", mGameModel->mPlayer->getInventory()->mAmmoCounter[i]);
        break;
    }

    mCharacterSheet->addText(tl, dimensions, fontSize, ammoString, TEXT_JUSTIFICATION_LEFT, color, NULL);
  }


  // inventory
  mCharacterSheet->addText(v2d_v(0.5, 0.2), v2d_v(0.4, 0.1), fontSize, "inventory:", TEXT_JUSTIFICATION_CENTER, color2, NULL);

  buttonHeight = (0.9 - 0.3) / static_cast<double>(mGameModel->mPlayer->getInventory()->getBackpack()->size());
  GLfloat* itemColor;

  for (size_t i = 0; i < mGameModel->mPlayer->getInventory()->getBackpack()->size(); ++i) {
    tl = v2d_v(0.5, lerp(0.3, 0.9 - buttonHeight, i, mGameModel->mPlayer->getInventory()->getBackpack()->size()));
//    br = v2d_v(0.9, lerp (0.3 + (buttonHeight * 0.9), 0.9, i, mGameModel->mPlayer->getInventory()->getBackpack()->size()));;
    dimensions.x = 0.4;
    dimensions.y = buttonHeight * 0.9;

    if (i == mGameModel->mPlayer->getInventory()->mSelectedBackpackItemSlot) {
      itemColor = selectedColor;
    }
    else {
      itemColor = color;
    }

    if (mGameModel->mPlayer->getInventory()->getBackpack()->getItemInSlot(i) != 0) {
      item_t item = mGameModel->mItemManager->getItem(mGameModel->mPlayer->getInventory()->getBackpack()->getItemInSlot(i));
      if (item.type != ITEMTYPE_UNDEFINED) {
        mCharacterSheet->addText(tl, dimensions, fontSize, item.name, TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
      }
    }
    else {
      mCharacterSheet->addText(tl, dimensions, fontSize, "---", TEXT_JUSTIFICATION_LEFT, itemColor, NULL);
    }
  }

  // let's give some indication of health status
  static char tempString[50];
  sprintf(tempString, "health %.0f\0", mGameModel->mPlayer->getCurrentHealth());
  mCharacterSheet->addText(v2d_v(0.1, 0.1), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);

  // money money money
  sprintf(tempString, "credits %d\0", mGameModel->mPlayer->getInventory()->mCredits);
  mCharacterSheet->addText(v2d_v(0.1, 0.6), v2d_v(0.2, 0.04), fontSize, tempString, TEXT_JUSTIFICATION_LEFT, color, NULL);
}

void CharacterSheetView::handleInput(GameInput* gameInput) {
  Inventory* inventory = mGameModel->mPlayer->getInventory();
  int selectedItemHandle = inventory->getBackpack()->getItemInSlot(inventory->mSelectedBackpackItemSlot);

  if (gameInput->isClickMouse1()) {
    inventory->swapBackPackItemIntoPrimary();
  }
  if (gameInput->isClickMouse2()) {
    inventory->swapBackPackItemIntoSecondary();
  }

  if (gameInput->isUseBackpackItem()) {
    item_t item = mGameModel->mItemManager->getItem(selectedItemHandle);
    if (item.type == ITEMTYPE_HEALTHPACK) {
      mGameModel->mPlayer->useBackpackItem();
    }
  }

  if (gameInput->isNextGun()) {
    inventory->nextBackPackItem();
  }

  if (gameInput->isPreviousGun()) {
    inventory->previousBackPackItem();
  }

  // did the player want to drop that item?
  // FIXME: destroys item!
  // this needs to create a physics item...
  if (gameInput->isDroppedItem() && selectedItemHandle != 0) {
    mGameModel->mItemManager->destroyItem(selectedItemHandle);
    inventory->getBackpack()->removeItem(selectedItemHandle);
  }
}

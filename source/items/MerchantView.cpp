#include "../items/MerchantView.h"

MerchantView::MerchantView(Player* player, ItemManager* itemManager) :
  mPlayer(player),
  mItemManager(itemManager),
  mMenu(NULL),
  mMode(MERCHANT_MODE_SELL),
  mSelectedItemIndex(-1),
  mMerchant(NULL)
{
  // FIXME: aww...c'mon...
  colorWhite[0] = 1.0f;
  colorWhite[1] = 1.0f;
  colorWhite[2] = 1.0f;
  colorWhite[3] = 1.0f;

  colorBackground[0] = 0.0f;
  colorBackground[1] = 0.0f;
  colorBackground[2] = 0.6f;
  colorBackground[3] = 1.0f;

  colorSelection[0] = 0.6f;
  colorSelection[1] = 0.6f;
  colorSelection[2] = 0.0f;
  colorSelection[3] = 1.0f;

  fontSize.x = 0.015f;
  fontSize.y = 0.03f;
}

MerchantView::~MerchantView() {
  if (mMenu != NULL) {
    delete mMenu;
  }
  if (mMerchant != NULL) {
    delete mMerchant;
  }
}

void MerchantView::engageMerchant() {
  if (mMerchant != NULL) {
    delete mMerchant;
    mMerchant = NULL;
  }
  mMerchant = new Merchant();

  printf("MerchantView::engageMerchant(): setup merchant\n");
  setupMerchant(mMerchant);
  printf("MerchantView::engageMerchant(): setup menu\n");
  setupMenu();
}

void MerchantView::setupMerchant(Merchant* merchant) {
  merchant->getInventory()->resize(MERCHANT_INVENTORY_SIZE);
  merchant->getInventory()->clear();

  for (size_t i = 0; i < MERCHANT_INVENTORY_SIZE; ++i) {
    size_t handle = mItemManager->generateRandomGun(r_num(1.0, 7.0));
    merchant->getInventory()->addItem(handle);
  }
}

int MerchantView::update() {
  int choice = mMenu->gameMenuChoice(false);

  switch (choice) {
  case MERCHANT_BUTTON_END_TRANSACTION:
    for (size_t i = 0; i < mMerchant->getInventory()->size(); ++i) {
      int itemHandle = mMerchant->getInventory()->getItemInSlot(i);
      mItemManager->destroyItem(itemHandle);
    }
    return 1;

  case MERCHANT_BUTTON_SELL_MODE:
    switchMode(MERCHANT_MODE_SELL);
    break;

  case MERCHANT_BUTTON_BUY_MODE:
    switchMode(MERCHANT_MODE_BUY);
    break;

  case MERCHANT_BUTTON_SELL:
    sellSelectedItem();
    break;

  case MERCHANT_BUTTON_BUY:
    buySelectedItem();
    break;
  }

  // check to see if the player clicked on an item
  if (choice >= MERCHANT_BUTTON_ITEM_BEGIN) {
    mSelectedItemIndex = choice - MERCHANT_BUTTON_ITEM_BEGIN;
    setupMenu();
  }

  return 0;
}

void MerchantView::draw() {
  mMenu->draw();
}

void MerchantView::setupMenu() {
  if (mMenu != 0) {
    delete mMenu;
    mMenu = 0;
  }
  mMenu = new GameMenu();

  mMenu->addText(v2d_v(0.6, 0.1), v2d_v(0.3, 0.05), fontSize,
    "inventory", TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

  mMenu->addButton(v2d_v(0.55, 0.85), v2d_v(0.35, 0.05), fontSize,
    "end transaction", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_END_TRANSACTION, colorWhite, colorBackground);

  // clear the inventory list
  mItemHandles.clear();

  if (mMode == MERCHANT_MODE_SELL) {
    setupSellMenu();
  }
  else {
    setupBuyMenu();
  }
}

void MerchantView::setupSellMenu() {
  mMenu->addButton(v2d_v(0.15, 0.2), v2d_v(0.2, 0.05), fontSize,
    "sell mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL_MODE, colorWhite, colorSelection);

  mMenu->addButton(v2d_v(0.15, 0.1), v2d_v(0.2, 0.05), fontSize,
    "buy mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY_MODE, colorWhite, colorBackground);

  mMenu->addButton(v2d_v(0.1, 0.85), v2d_v(0.3, 0.05), fontSize,
    "sell item", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL, colorWhite, colorBackground);

  addSelectedItemInfo();
  loadItemHandles(mPlayer->getInventory()->getBackpack());
  setupInventoryList();
}

void MerchantView::setupBuyMenu() {
  mMenu->addButton(v2d_v(0.15, 0.2), v2d_v(0.2, 0.05), fontSize,
    "sell mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL_MODE, colorWhite, colorBackground);

  mMenu->addButton(v2d_v(0.15, 0.1), v2d_v(0.2, 0.05), fontSize,
    "buy mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY_MODE, colorWhite, colorSelection);

  mMenu->addButton(v2d_v(0.1, 0.85), v2d_v(0.3, 0.05), fontSize,
    "purchase item", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY, colorWhite, colorBackground);

  addSelectedItemInfo();
  loadItemHandles(mMerchant->getInventory());
  setupInventoryList();
}

void MerchantView::addSelectedItemInfo() {
  char tempString[128];

  // show details for the selected item
  if (mSelectedItemIndex >= 0) {
    item_t item = mItemManager->getItem(mItemHandles[mSelectedItemIndex]);

    mMenu->addText(v2d_v(0.05, 0.3), v2d_v(0.4, 0.05), fontSize,
      item.name, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

    if (item.type == ITEMTYPE_GUN_ONE_HANDED) {
      //int gunType;
      //int ammoType;
      //int bulletType;
      //double shotDelay;
      //int bulletsPerShot;
      //double bulletDamage;
      //double bulletSpread;
      //double bulletForce;
      //double explosionForce;
      //int numParticles;
      //int shotSound;

      sprintf(tempString, "shot delay: %.2f", item.shotDelay);
      mMenu->addText(v2d_v(0.05, 0.35), v2d_v(0.4, 0.05), fontSize,
        tempString, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

      sprintf(tempString, "ammo per shot: %d", item.bulletsPerShot);
      mMenu->addText(v2d_v(0.05, 0.4), v2d_v(0.4, 0.05), fontSize,
        tempString, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

      sprintf(tempString, "spread: %.3f", item.bulletSpread);
      mMenu->addText(v2d_v(0.05, 0.45), v2d_v(0.4, 0.05), fontSize,
        tempString, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

      sprintf(tempString, "shot force: %.2f", item.bulletForce);
      mMenu->addText(v2d_v(0.05, 0.5), v2d_v(0.4, 0.05), fontSize,
        tempString, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

//      sprintf(tempString, "shot delay: %.2f", item.shotDelay);
//      mMenu->addText(v2d_v(0.1, 0.55), v2d_v(0.3, 0.05), fontSize,
//        tempString, TEXT_JUSTIFICATION_CENTER, colorWhite, colorBackground);

    }
  }
}

void MerchantView::setupInventoryList() {
  double buttonHeight = (0.875 - 0.175) / static_cast<double>(mItemHandles.size());
  v2d_t tl, dimensions;

  for (size_t i = 0; i < mItemHandles.size(); ++i) {
    tl = v2d_v (0.55, lerp (0.175, 0.875 - buttonHeight, i, mItemHandles.size()));
    dimensions.x = 0.4;
    dimensions.y = buttonHeight * 0.9; // 0.9 is the TOTAL HEIGHT

    item_t item = mItemManager->getItem(mItemHandles[i]);
    if (item.type != ITEMTYPE_UNDEFINED) {
      GLfloat *bgColor;
      if (i == mSelectedItemIndex) {
        bgColor = colorSelection;
      }
      else {
        bgColor = colorBackground;
      }

      mMenu->addButton (tl, dimensions, fontSize,
        item.name, TEXT_JUSTIFICATION_LEFT, MERCHANT_BUTTON_ITEM_BEGIN + i, colorWhite, bgColor);
    }
  }
}

void MerchantView::loadItemHandles(const ItemContainer* itemContainer) {
  size_t itemContainerSize = itemContainer->size();
  mItemHandles.resize(itemContainerSize, 0);
  for (size_t i = 0; i < itemContainerSize; ++i) {
    size_t itemHandle = itemContainer->getItemInSlot(i);
    if (itemHandle != 0) {
      mItemHandles.push_back(itemHandle);
    }
  }
}

void MerchantView::switchMode(int mode) {
  if (mMode == mode) {
    return;
  }
  mMode = mode;
  mSelectedItemIndex = -1;
  setupMenu();
}

void MerchantView::sellSelectedItem() {
  if (mSelectedItemIndex < 0) {
    return;
  }
  bool itemWasRemovedFromBackpack = mPlayer->getInventory()->getBackpack()->removeItem(mItemHandles[mSelectedItemIndex]);
  if (itemWasRemovedFromBackpack) {
    mItemManager->destroyItem(mItemHandles[mSelectedItemIndex]);
  }
  mSelectedItemIndex = -1;
  setupMenu();
}

void MerchantView::buySelectedItem() {
  if (mSelectedItemIndex < 0) {
    return;
  }
  bool itemWasAddedToBackpack = mPlayer->getInventory()->getBackpack()->addItem(mItemHandles[mSelectedItemIndex]);
  if (itemWasAddedToBackpack) {
    mMerchant->getInventory()->removeItem(mItemHandles[mSelectedItemIndex]);
    mSelectedItemIndex = -1;
    setupMenu();
  }
}

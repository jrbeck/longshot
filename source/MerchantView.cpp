#include "MerchantView.h"

MerchantView::MerchantView() :
  mMenu(0),
  mMode(MERCHANT_MODE_SELL),
  mSelectedItem(-1),
  mMerchant(0)
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
  if (mMenu != 0) {
    delete mMenu;
  }
  if (mMerchant != 0) {
    delete mMerchant;
  }
}

void MerchantView::engageMerchant(player_c& player, ItemManager& itemManager) {
  if (mMerchant != 0) {
    delete mMerchant;
    mMerchant = 0;
  }
  mMerchant = new Merchant();

  printf("MerchantView::engageMerchant(): setup merchant\n");
  setupMerchant(*mMerchant, itemManager);
  printf("MerchantView::engageMerchant(): setup menu\n");
  setupMenu(player, itemManager);
}

void MerchantView::setupMerchant(Merchant& merchant, ItemManager& itemManager) {

  merchant.mInventoryList.clear();

  for (size_t i = 0; i < 8; i++) {
    size_t handle = itemManager.generateRandomGun(r_num(1.0, 7.0));
    merchant.mInventoryList.push_back(handle);
  }
}

int MerchantView::update(player_c& player, ItemManager& itemManager) {
  int choice = mMenu->GameMenuhoice(false);

  switch (choice) {
  case MERCHANT_BUTTON_END_TRANSACTION:
    // TODO: for now, just destroy the placeholder merchant's
    // inventory
    for (size_t i = 0; i < mMerchant->mInventoryList.size(); i++) {
      itemManager.destroyItem(mMerchant->mInventoryList[i]);
    }
    return 1;

  case MERCHANT_BUTTON_SELL_MODE:
    if (mMode != MERCHANT_MODE_SELL) {
      mMode = MERCHANT_MODE_SELL;
      mSelectedItem = -1;
      setupMenu(player, itemManager);
    }
    break;

  case MERCHANT_BUTTON_BUY_MODE:
    if (mMode != MERCHANT_MODE_BUY) {
      mMode = MERCHANT_MODE_BUY;
      mSelectedItem = -1;
      setupMenu(player, itemManager);
    }
    break;

  case MERCHANT_BUTTON_SELL:
    if (mSelectedItem >= 0) {
      Inventory *inv = player.getInventory();
      for (int i = 0; i < inv->mBackpack.size(); i++) {
        if (inv->mBackpack[i] == tempList[mSelectedItem]) {
          // remove from backpack
          inv->mBackpack[i] = 0;
          // destroy the item
          // FIXME: this should probably be added to the merchant's
          // inventory
          itemManager.destroyItem(tempList[mSelectedItem]);
          mSelectedItem = -1;
          setupMenu(player, itemManager);
          return 0;
        }
      }
    }
    break;

  case MERCHANT_BUTTON_BUY:
    if (mSelectedItem >= 0) {
      Inventory *inv = player.getInventory();
      for (int i = 0; i < inv->mBackpack.size(); i++) {
        if (inv->mBackpack[i] == 0) {
          // put in backpack
          inv->mBackpack[i] = tempList[mSelectedItem];
          // remove from seller's inventory
          for (size_t j = 0; j < mMerchant->mInventoryList.size(); j++) {
            if (mMerchant->mInventoryList[j] == tempList[mSelectedItem]) {
              mMerchant->mInventoryList[j] = 0;
            }
          }
          mSelectedItem = -1;
          setupMenu(player, itemManager);
          return 0;
        }
      }
    }
    break;
  }

  // check to see if the player clicked on an item
  if (choice >= MERCHANT_BUTTON_ITEM_BEGIN) {
    mSelectedItem = choice - MERCHANT_BUTTON_ITEM_BEGIN;
    setupMenu(player, itemManager);
  }

  return 0;
}

void MerchantView::setupMenu(player_c& player, ItemManager& itemManager) {
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
  tempList.clear();

  if (mMode == MERCHANT_MODE_SELL) {
    setupSellMenu(player, itemManager);
  }
  else {
    setupBuyMenu(*mMerchant, itemManager);
  }
}

void MerchantView::setupSellMenu(player_c& player, ItemManager& itemManager) {
  char tempString[128];

  mMenu->addButton(v2d_v(0.15, 0.2), v2d_v(0.2, 0.05), fontSize,
    "sell mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL_MODE, colorWhite, colorSelection);

  mMenu->addButton(v2d_v(0.15, 0.1), v2d_v(0.2, 0.05), fontSize,
    "buy mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY_MODE, colorWhite, colorBackground);

  mMenu->addButton(v2d_v(0.1, 0.85), v2d_v(0.3, 0.05), fontSize,
    "sell item", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL, colorWhite, colorBackground);

  // show info about selected item
  addSelectedItemInfo(itemManager);

  // setup the MerchantView list from the player's backpack
  Inventory *inventory = player.getInventory();
  for (size_t i = 0; i < inventory->mBackpack.size(); i++) {
    if (inventory->mBackpack[i] != 0) {
      tempList.push_back(inventory->mBackpack[i]);
    }
  }

  // add the buttons for the player's inventory
  setupInventoryList(itemManager);
}

void MerchantView::setupBuyMenu(Merchant& merchant, ItemManager& itemManager) {
  mMenu->addButton(v2d_v(0.15, 0.2), v2d_v(0.2, 0.05), fontSize,
    "sell mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_SELL_MODE, colorWhite, colorBackground);

  mMenu->addButton(v2d_v(0.15, 0.1), v2d_v(0.2, 0.05), fontSize,
    "buy mode", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY_MODE, colorWhite, colorSelection);

  mMenu->addButton(v2d_v(0.1, 0.85), v2d_v(0.3, 0.05), fontSize,
    "purchase item", TEXT_JUSTIFICATION_CENTER, MERCHANT_BUTTON_BUY, colorWhite, colorBackground);

  // show info about selected item
  addSelectedItemInfo(itemManager);

  // setup the MerchantView list from the merchant's inventory
  for (size_t i = 0; i < mMerchant->mInventoryList.size(); i++) {
    if (mMerchant->mInventoryList[i] != 0) {
      tempList.push_back(mMerchant->mInventoryList[i]);
    }
  }

  // add the buttons for the merchant's inventory
  setupInventoryList(itemManager);
}

void MerchantView::addSelectedItemInfo(ItemManager& itemManager) {
  char tempString[128];

  // show details for the selected item
  if (mSelectedItem >= 0) {
    item_t item = itemManager.getItem(tempList[mSelectedItem]);

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

void MerchantView::setupInventoryList(ItemManager& itemManager) {
  double buttonHeight = (0.875 - 0.175) / static_cast<double>(tempList.size ());
  v2d_t tl, dimensions;

  for (size_t i = 0; i < tempList.size(); i++) {
    tl = v2d_v (0.55, lerp (0.175, 0.875 - buttonHeight, i, tempList.size ()));
    dimensions.x = 0.4;
    dimensions.y = buttonHeight * 0.9; // 0.9 is the TOTAL HEIGHT

    item_t item = itemManager.getItem(tempList[i]);
    if (item.type != ITEMTYPE_UNDEFINED) {
      GLfloat *bgColor;
      if (i == mSelectedItem) {
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

void MerchantView::draw(void) {
  mMenu->draw();
}

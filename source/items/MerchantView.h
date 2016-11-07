#pragma once

#include <vector>

#include "../game/GameMenu.h"
#include "../items/Inventory.h"
#include "../items/ItemContainer.h"
#include "../player/Player.h"

enum {
  MERCHANT_MODE_SELL,
  MERCHANT_MODE_BUY
};

enum {
  MERCHANT_BUTTON_END_TRANSACTION = 1,
  MERCHANT_BUTTON_SELL_MODE,
  MERCHANT_BUTTON_BUY_MODE,

  MERCHANT_BUTTON_SELL,
  MERCHANT_BUTTON_BUY,

  MERCHANT_BUTTON_ITEM_BEGIN = 100
};

#define MERCHANT_INVENTORY_SIZE 8

// TODO: move this into it's own header/cpp files
class Merchant {
public:

  ItemContainer* getInventory() { return mInventory; }

  // void save(FILE *file);
  // void load(FILE *file);

private:
  size_t mCredits;
  ItemContainer* mInventory;
};



class MerchantView {
public:
  MerchantView(Player* player, ItemManager* itemManager);
  ~MerchantView();

  void engageMerchant();
  void setupMerchant(Merchant* merchant);

  int update();
  void draw();

  void setupMenu();
  void setupSellMenu();
  void setupBuyMenu();

  void addSelectedItemInfo();
  void setupInventoryList();

private:
  void loadItemHandles(const ItemContainer* itemContainer);
  void switchMode(int mode);
  void sellSelectedItem();
  void buySelectedItem();

  Player* mPlayer;
  ItemManager* mItemManager;
  Merchant* mMerchant;

  GameMenu* mMenu;

  int mMode;

  int mSelectedItemIndex;
  vector<size_t> mItemHandles;

  GLfloat colorWhite[4];
  GLfloat colorBackground[4];
  GLfloat colorSelection[4];
  v2d_t fontSize;
};

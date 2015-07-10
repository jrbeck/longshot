// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * MerchantView
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef MerchantView_h_
#define MerchantView_h_

#include <vector>

#include "GameMenu.h"
#include "Inventory.h"
#include "Player.h"


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



// TODO: move this into it's own header/cpp files
class Merchant {
public:
  void save(FILE *file);
  void load(FILE *file);

  vector<size_t> mInventoryList;

  size_t mCredits;
};




class MerchantView {
public:
  MerchantView();
  ~MerchantView();


  // this begins the interaction with a merchant
  void engageMerchant(player_c &player, ItemManager &itemManager);

  void setupMerchant(Merchant &merchant, ItemManager &itemManager);

  int update(player_c &player, ItemManager &itemManager);

  void setupMenu(player_c &player, ItemManager &itemManager);
  void setupSellMenu(player_c &player, ItemManager &itemManager);
  void setupBuyMenu(Merchant &merchant, ItemManager &itemManager);

  void addSelectedItemInfo(ItemManager &itemManager);
  void setupInventoryList(ItemManager &itemManager);


  void draw(void);

// MEMBERS * * * * * * * * * *

  GameMenu *mMenu;

  int mMode;
  int mSelectedItem;

  vector<size_t> tempList;

  GLfloat colorWhite[4];
  GLfloat colorBackground[4];
  GLfloat colorSelection[4];
  v2d_t fontSize;

  // TODO: this is for testing only
  Merchant *mMerchant;
};


#endif // MerchantView_h_

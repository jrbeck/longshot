// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Inventory
// *
// * this is as inventory container for a player.
// * it contains info about equipped gear as well as backpack inventory.
// * the items are just handles to items contained in an ItemManager
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "../game/Constants.h"

class Inventory {
public:
  Inventory();
  ~Inventory();

  void clear();

  void resizeBackpack(int newSize);

  int getNextFreeBackpackSlot();

  bool swapBackPackItemIntoPrimary();
  bool swapBackPackItemIntoSecondary();

  void nextBackPackItem();
  void previousBackPackItem();

  void save(FILE* file);
  void load(FILE* file);


  // MEMBERS * * * * * * * * * * * *
  size_t mCredits;

  std::vector<size_t> mBackpack;

  size_t mPrimaryItem;
  size_t mSecondaryItem;

  size_t mHeadGear;
  size_t mFootGear;
  size_t mLegGear;
  size_t mTorsoGear;

  size_t mAmmoCounter[NUM_AMMO_TYPES];

  size_t mSelectedBackpackItem;
};

#pragma once

#include <vector>

#include "../items/ItemContainer.h"
#include "../game/Constants.h"

class Inventory {
public:
  Inventory(size_t backpackSize);
  ~Inventory();

  void clear();

  bool swapBackPackItemIntoPrimary();
  bool swapBackPackItemIntoSecondary();

  void nextBackPackItem();
  void previousBackPackItem();

  ItemContainer* getBackpack();
  size_t getSelectedBackpackItemHandle() const;

  void save(FILE* file);
  void load(FILE* file);

  // MEMBERS * * * * * * * * * * * *
  size_t mCredits;

  ItemContainer* mBackpack;
  size_t mSelectedBackpackItemSlot;

  size_t mPrimaryItem;
  size_t mSecondaryItem;

  size_t mHeadGear;
  size_t mFootGear;
  size_t mLegGear;
  size_t mTorsoGear;

  size_t mAmmoCounter[NUM_AMMO_TYPES];
};

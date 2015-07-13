#include "Inventory.h"

Inventory::Inventory() {
  mBackpack.resize(5);
  clear();
}



Inventory::~Inventory() {
}



void Inventory::clear() {
  mCredits = 1000;

  for (size_t i = 0; i < mBackpack.size(); i++) {
    mBackpack[i] = 0;
  }

  mPrimaryItem = 0;
  mSecondaryItem = 0;

  mHeadGear = 0;
  mFootGear = 0;
  mLegGear = 0;
  mTorsoGear = 0;

  for (int i = 0; i < NUM_AMMO_TYPES; i++) {
    mAmmoCounter[i] = 0;
  }

  mSelectedBackpackItem = 0;
}



void Inventory::resizeBackpack(int newSize) {
  int oldSize = mBackpack.size();

  if (oldSize > newSize) {
    // FIXME: deal with a shrinking inventory
    printf("Inventory::resizeBackpack() - error!!! backpack shrinking - no can do!\n");
    return;
  }

  mBackpack.resize(newSize);

  for (int i = oldSize; i < newSize; i++) {
    mBackpack[i] = 0;
  }
}



int Inventory::getNextFreeBackpackSlot() {
  for (size_t i = 0; i < mBackpack.size(); i++) {
    if (mBackpack[i] == 0) {
      return i;
    }
  }

  return -1;
}



bool Inventory::swapBackPackItemIntoPrimary() {
  int temp = mPrimaryItem;

  mPrimaryItem = mBackpack[mSelectedBackpackItem];
  mBackpack[mSelectedBackpackItem] = temp;

  return true;
}



bool Inventory::swapBackPackItemIntoSecondary() {
  int temp = mSecondaryItem;

  mSecondaryItem = mBackpack[mSelectedBackpackItem];
  mBackpack[mSelectedBackpackItem] = temp;

  return true;
}



void Inventory::nextBackPackItem() {
  if (mSelectedBackpackItem == 0) {
    mSelectedBackpackItem = mBackpack.size() - 1;
    return;
  }

  mSelectedBackpackItem = mSelectedBackpackItem - 1;
}



void Inventory::previousBackPackItem() {
  mSelectedBackpackItem = (mSelectedBackpackItem + 1) % mBackpack.size();
}



void Inventory::save(FILE* file) {
  fwrite(&mCredits, sizeof (size_t), 1, file);

  size_t backpackSize = mBackpack.size();
  fwrite(&backpackSize, sizeof (size_t), 1, file);
  for (size_t i = 0; i < backpackSize; i++) {
    fwrite(&mBackpack[i], sizeof (size_t), 1, file);
  }

  fwrite(&mPrimaryItem, sizeof (size_t), 1, file);
  fwrite(&mSecondaryItem, sizeof (size_t), 1, file);
  fwrite(&mHeadGear, sizeof (size_t), 1, file);
  fwrite(&mFootGear, sizeof (size_t), 1, file);
  fwrite(&mLegGear, sizeof (size_t), 1, file);
  fwrite(&mTorsoGear, sizeof (size_t), 1, file);
  fwrite(&mAmmoCounter, sizeof (size_t), NUM_AMMO_TYPES, file);
  fwrite(&mSelectedBackpackItem, sizeof (size_t), 1, file);
}


void Inventory::load(FILE* file) {
  fread(&mCredits, sizeof (size_t), 1, file);

  mBackpack.clear();

  size_t backpackSize;
  fread(&backpackSize, sizeof (size_t), 1, file);
  for (size_t i = 0; i < backpackSize; i++) {
    size_t backpackItemHandle;
    fread(&backpackItemHandle, sizeof (size_t), 1, file);
    mBackpack.push_back(backpackItemHandle);
  }

  fread(&mPrimaryItem, sizeof (size_t), 1, file);
  fread(&mSecondaryItem, sizeof (size_t), 1, file);
  fread(&mHeadGear, sizeof (size_t), 1, file);
  fread(&mFootGear, sizeof (size_t), 1, file);
  fread(&mLegGear, sizeof (size_t), 1, file);
  fread(&mTorsoGear, sizeof (size_t), 1, file);
  fread(&mAmmoCounter, sizeof (size_t), NUM_AMMO_TYPES, file);
  fread(&mSelectedBackpackItem, sizeof (size_t), 1, file);
}






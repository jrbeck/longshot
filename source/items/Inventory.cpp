#include "../items/Inventory.h"

Inventory::Inventory(size_t backpackSize) {
  mBackpack = new ItemContainer(backpackSize);
  clear();
}

Inventory::~Inventory() {
  if (mBackpack != NULL) {
    delete mBackpack;
  }
}

void Inventory::clear() {
  mCredits = 0;

  mBackpack->clear();
  mSelectedBackpackItemSlot = 0;

  mPrimaryItem = 0;
  mSecondaryItem = 0;

  mHeadGear = 0;
  mFootGear = 0;
  mLegGear = 0;
  mTorsoGear = 0;

  for (int i = 0; i < NUM_AMMO_TYPES; ++i) {
    mAmmoCounter[i] = 0;
  }
}

bool Inventory::swapBackPackItemIntoPrimary() {
  int backpackItem = mBackpack->getItemInSlot(mSelectedBackpackItemSlot);
  mBackpack->putItemInSlot(mSelectedBackpackItemSlot, mPrimaryItem);
  mPrimaryItem = backpackItem;
  return true;
}

bool Inventory::swapBackPackItemIntoSecondary() {
  int backpackItem = mBackpack->getItemInSlot(mSelectedBackpackItemSlot);
  mBackpack->putItemInSlot(mSelectedBackpackItemSlot, mSecondaryItem);
  mSecondaryItem = backpackItem;
  return true;
}

void Inventory::nextBackPackItem() {
  if (mSelectedBackpackItemSlot == 0) {
    mSelectedBackpackItemSlot = mBackpack->size() - 1;
    return;
  }
  mSelectedBackpackItemSlot = mSelectedBackpackItemSlot - 1;
}

void Inventory::previousBackPackItem() {
  mSelectedBackpackItemSlot = (mSelectedBackpackItemSlot + 1) % mBackpack->size();
}

ItemContainer* Inventory::getBackpack() {
  return mBackpack;
}

size_t Inventory::getSelectedBackpackItemHandle() const {
  return mBackpack->getItemInSlot(mSelectedBackpackItemSlot);
}

void Inventory::save(FILE* file) {
  fwrite(&mCredits, sizeof (size_t), 1, file);
  fwrite(&mPrimaryItem, sizeof (size_t), 1, file);
  fwrite(&mSecondaryItem, sizeof (size_t), 1, file);
  fwrite(&mHeadGear, sizeof (size_t), 1, file);
  fwrite(&mFootGear, sizeof (size_t), 1, file);
  fwrite(&mLegGear, sizeof (size_t), 1, file);
  fwrite(&mTorsoGear, sizeof (size_t), 1, file);
  fwrite(&mAmmoCounter, sizeof (size_t), NUM_AMMO_TYPES, file);
  fwrite(&mSelectedBackpackItemSlot, sizeof (size_t), 1, file);
  mBackpack->save(file);
}

void Inventory::load(FILE* file) {
  fread(&mCredits, sizeof (size_t), 1, file);
  fread(&mPrimaryItem, sizeof (size_t), 1, file);
  fread(&mSecondaryItem, sizeof (size_t), 1, file);
  fread(&mHeadGear, sizeof (size_t), 1, file);
  fread(&mFootGear, sizeof (size_t), 1, file);
  fread(&mLegGear, sizeof (size_t), 1, file);
  fread(&mTorsoGear, sizeof (size_t), 1, file);
  fread(&mAmmoCounter, sizeof (size_t), NUM_AMMO_TYPES, file);
  fread(&mSelectedBackpackItemSlot, sizeof (size_t), 1, file);
  mBackpack->load(file);
}

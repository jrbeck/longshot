#include "../items/ItemContainer.h"

ItemContainer::ItemContainer(size_t size) {
  resize(size);
}

ItemContainer::~ItemContainer() {
}

void ItemContainer::clear() {
  size_t containerSize = mItemHandles.size();
  for (size_t i = 0; i < containerSize; ++i) {
    mItemHandles[i] = 0;
  }
}

bool ItemContainer::containsItem(size_t itemHandle) const {
  size_t containerSize = mItemHandles.size();
  for (size_t i = 0; i < containerSize; ++i) {
    if (mItemHandles[i] == itemHandle) {
      return true;
    }
  }
  return false;
}

int ItemContainer::addItem(size_t itemHandle) {
  int freeSlotIndex = getFreeSlot();
  if (freeSlotIndex == -1 || containsItem(itemHandle)) {
    return -1;
  }
  mItemHandles[freeSlotIndex] = itemHandle;
  return freeSlotIndex;
}

bool ItemContainer::removeItem(size_t itemHandle) {
  size_t containerSize = mItemHandles.size();
  for (size_t i = 0; i < containerSize; ++i) {
    if (mItemHandles[i] == itemHandle) {
      mItemHandles[i] = 0;
      return true;
    }
  }
  return false;
}

bool ItemContainer::isFull() const {
  size_t containerSize = mItemHandles.size();
  for (size_t i = 0; i < containerSize; ++i) {
    if (mItemHandles[i] == 0) {
      return false;
    }
  }
  return true;
}

size_t ItemContainer::size() const {
  return mItemHandles.size();
}

void ItemContainer::resize(size_t newSize) {
  if (mItemHandles.size() > newSize) {
    // FIXME: deal with a shrinking inventory
    // don't want to abandon items
    printf("ItemContainer::resize() - error!!! container shrinking - no can do!\n");
    return;
  }
  mItemHandles.resize(newSize, 0);
}

int ItemContainer::getItemInSlot(size_t index) const {
  if (index >= size()) {
    return -1;
  }
  return mItemHandles[index];
}

int ItemContainer::putItemInSlot(size_t index, size_t itemHandle) {
  if (index >= size()) {
    return -1;
  }
  size_t oldHandle = mItemHandles[index];
  mItemHandles[index] = itemHandle;
  return oldHandle;
}

int ItemContainer::getFreeSlot() const {
  int containerSize = (int)mItemHandles.size();
  for (int i = 0; i < containerSize; ++i) {
    if (mItemHandles[i] == 0) {
      return i;
    }
  }
  return -1;
}

void ItemContainer::save(FILE* file) {
  size_t itemHandlesSize = size();
  fwrite(&itemHandlesSize, sizeof (size_t), 1, file);
  fwrite(&mItemHandles[0], sizeof (size_t), itemHandlesSize, file);
}

void ItemContainer::load(FILE* file) {
  size_t itemHandlesSize;
  fread(&itemHandlesSize, sizeof (size_t), 1, file);
  resize(itemHandlesSize);
  fread(&mItemHandles[0], sizeof (size_t), itemHandlesSize, file);
}

#pragma once

#include <cstdio>
#include <vector>

class ItemContainer {
public:
  ItemContainer(size_t size);
  ~ItemContainer();

  void clear();

  bool containsItem(size_t itemHandle) const;
  int addItem(size_t itemHandle);
  bool removeItem(size_t itemHandle);

  bool isFull() const;

  size_t size() const;
  void resize(size_t newSize);

  int getItemInSlot(size_t index) const;
  int putItemInSlot(size_t index, size_t itemHandle);

  void save(FILE* file);
  void load(FILE* file);

private:
  int getFreeSlot() const;

  std::vector<size_t> mItemHandles;
};

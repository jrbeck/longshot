#include "InactiveList.h"



InactiveList::InactiveList () {
}



InactiveList::~InactiveList () {
  clear ();
}



void InactiveList::clear (void) {
  mItems.clear ();
}



void InactiveList::addItem (inactive_item_t item) {
  mItems.push_back (item);
}



inactive_item_t InactiveList::popNextItemInColumn (v2di_t index) {
  inactive_item_t item;

  for (size_t i = 0; i < mItems.size (); i++) {
    if (isInColumn (index, mItems[i].pos)) {
      item = mItems[i];

      if (i != mItems.size () - 1) {
        swap (mItems[i], mItems[mItems.size () - 1]);
      }
      mItems.pop_back ();

      printf ("item popped, %lu remain...\n", mItems.size());

      return item;
    }
  }

  item.type = INACTIVETYPE_NULL;
  return item;
}



bool InactiveList::isInColumn (v2di_t index, v3d_t position) {
  int xPos = static_cast<int>(floor (position.x));
  int zPos = static_cast<int>(floor (position.z));

  int worldX = index.x * WORLD_CHUNK_SIDE;
  int worldZ = index.y * WORLD_CHUNK_SIDE;


  if ((xPos < worldX) ||
    (xPos >= (worldX + WORLD_CHUNK_SIDE)) ||
    (zPos < worldZ) ||
    (zPos >= (worldZ + WORLD_CHUNK_SIDE))) {
    return false;
  }
//  if (xPosIndex != index.x) return false;
//  if (zPosIndex != index.y) return false;

  return true;
}



int InactiveList::save( FILE* file ) {
  int size = mItems.size ();

  // write the number of items
  fwrite (&size, sizeof (int), 1, file);

  for (int i = 0; i < size; i++) {
    fwrite (&mItems[i], sizeof (inactive_item_t), 1, file);
//    printf ("T: (%d) ", mItems[i].type);
//    v3d_print ("pos", mItems[i].pos);
  }

  printf ("InactiveList::saveToDisk(): saved %d items\n", size);

  return 0;
}



int InactiveList::load( FILE* file ) {

  // read the number of items
  int size;
  fread (&size, sizeof (int), 1, file);

  inactive_item_t item;
  for (int i = 0; i < size; i++) {
    fread (&item, sizeof (inactive_item_t), 1, file);
    mItems.push_back (item);
  }

  printf ("InactiveList::loadFromDisk(): loaded %d items\n", size);

  return 0;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * InactiveList
// *
// * the inactive list is used to maintain inactive physics/AiEntity items.
// * they are put onto the list when they leave the world scope, and are
// * brought back from this list into the physics engine... when they reenter
// * the world focus
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "../math/v2d.h"
#include "../math/v3d.h"

#include "../world/WorldMap.h"

using namespace std;


enum {
  INACTIVETYPE_NULL = -1,
  INACTIVETYPE_AI,
  INACTIVETYPE_ITEM,
  NUM_INACTIVETYPES
};


struct inactive_item_t {
  int type;
  int type2;

  v3d_t pos;
};



class InactiveList {
public:
  InactiveList();
  ~InactiveList();

  void clear( void );

  void addItem( inactive_item_t item );

  inactive_item_t popNextItemInColumn( v2di_t index );

  bool isInColumn( v2di_t index, v3d_t position );

  int save( FILE* file );
  int load( FILE* file );


private:
  vector <inactive_item_t> mItems;
};

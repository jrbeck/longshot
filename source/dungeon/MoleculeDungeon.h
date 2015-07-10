// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * MoleculeDungeon
// *
// * desc: creates a Dungeon with a 'molecular' layout. This means it's built from
// * circular rooms with connecting passageways
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "DungeonUtil.h"


class MoleculeDungeon {
public:
  static void createDungeon(DungeonUtil& dungeon, const v2di_t& startPosition);

};


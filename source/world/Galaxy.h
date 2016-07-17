// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Galaxy
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "MathUtil.h"
#include "StarSystem.h"

#define GALACTIC_WIDTH  (100)
#define GALACTIC_HEIGHT (100)

#define NUM_STARSYSTEMS (100)


class Galaxy {
public:
  Galaxy();
  ~Galaxy();

  void clear();
  void randomize(size_t numSystems);

  void save(FILE* file);
  void load(FILE* file);

  StarSystem* getStarSystemByHandle(int handle);
  Planet* getPlanetByHandle(int handle);


  // MEMBERS * * * * * * * * * * *
  int mNextHandle;
  vector<StarSystem*> mStarSystems;
};

#include "../world/Galaxy.h"

Galaxy::Galaxy() {
  mNextHandle = 1;
  randomize(NUM_STARSYSTEMS);
}

Galaxy::~Galaxy() {
  clear();
}

void Galaxy::clear() {
  size_t numSystems = mStarSystems.size();
  for (size_t i = 0; i < numSystems; ++i) {
    if (mStarSystems[i] != NULL) {
      delete mStarSystems[i];
    }
  }
  mStarSystems.clear();
}

void Galaxy::randomize(size_t numSystems) {
  for (size_t i = 0; i < numSystems; ++i) {
    StarSystem *starSystem = new StarSystem();
    if (starSystem == NULL) {
      printf("Galaxy::Galaxy(): out of memory\n");
      return;
    }

    starSystem->mHandle = mNextHandle++;
    starSystem->mPosition.x = r_num(0.0, GALACTIC_WIDTH);
    starSystem->mPosition.y = r_num(0.0, GALACTIC_HEIGHT);

    starSystem->randomize();
    mNextHandle += starSystem->mPlanets.size();

    mStarSystems.push_back(starSystem);
  }
}

void Galaxy::save(FILE* file) {
  // save the members
  fwrite(&mNextHandle, sizeof mNextHandle, 1, file);

  // save the StarSystems
  size_t numSystems = mStarSystems.size();
  fwrite(&numSystems, sizeof numSystems, 1, file);
  for (size_t i = 0; i < numSystems; ++i) {
    mStarSystems[i]->save(file);
  }
}

void Galaxy::load(FILE* file) {
  // clear out anything that might be in memory
  clear();

  // load the members
  fread(&mNextHandle, sizeof mNextHandle, 1, file);

  // load from the file
  size_t numSystems;
  fread(&numSystems, sizeof numSystems, 1, file);
  for (size_t i = 0; i < numSystems; ++i) {
    StarSystem* starSystem = new StarSystem();
    if (starSystem == NULL) {
      printf("Galaxy::load(): error: out of memory\n");
      return;
    }
    starSystem->load(file);
    mStarSystems.push_back(starSystem);
  }
}

StarSystem* Galaxy::getStarSystemByHandle(int handle) {
  for (size_t i = 0; i < mStarSystems.size(); ++i) {
    if (mStarSystems[i]->mHandle == handle) {
      return mStarSystems[i];
    }

    if (mStarSystems[i]->getPlanetByHandle(handle) != NULL) {
      return mStarSystems[i];
    }
  }

  return NULL;
}

Planet* Galaxy::getPlanetByHandle(int handle) {
  Planet* planet;

  for (size_t i = 0; i < mStarSystems.size(); ++i) {
    planet = mStarSystems[i]->getPlanetByHandle(handle);
    if (planet != NULL) {
      return planet;
    }
  }

  return NULL;
}

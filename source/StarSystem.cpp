#include "StarSystem.h"

StarSystem::StarSystem() {
}

StarSystem::~StarSystem() {
  clear();
}

void StarSystem::clear() {
  size_t numPlanets = mPlanets.size();
  for(size_t i = 0; i < numPlanets; ++i) {
    if(mPlanets[i] != NULL) {
      delete mPlanets[i];
    }
  }
  mPlanets.clear();
}

void StarSystem::randomize() {
  mStarColor[0] = (GLfloat)r_num(0.0, 1.0);
  mStarColor[1] = (GLfloat)r_num(0.0, 1.0);
  mStarColor[2] = (GLfloat)r_num(0.0, 1.0);
  mStarColor[3] = 1.0f;

  GLfloat maxColor = max(max(mStarColor[0], mStarColor[1]), mStarColor[2]);
  mStarColor[0] += 1.0f - maxColor;
  mStarColor[1] += 1.0f - maxColor;
  mStarColor[2] += 1.0f - maxColor;

  mStarRadius = r_num(MIN_STAR_RADIUS, MAX_STAR_RADIUS);
  size_t numPlanets = r_numi(MIN_PLANETS, MAX_PLANETS);

  for (size_t i = 0; i < numPlanets; ++i) {
    Planet* planet = new Planet();
    if (planet == NULL) {
      printf("StarSystem::StarSystem(): out of memory\n");
      return;
    }

    planet->mHandle = mHandle + i + 1;
    planet->mHasBeenVisited = false;
    planet->mRadius = r_num(MIN_PLANET_RADIUS, MAX_PLANET_RADIUS);
    planet->mOrbitRadius = r_num(MIN_ORBIT_RADIUS, MAX_ORBIT_RADIUS);
    planet->mAngle = r_num(0.0, MY_2PI);
    planet->mPosition.x = planet->mRadius * cos(planet->mAngle);
    planet->mPosition.y = planet->mRadius * sin(planet->mAngle);
    planet->mSeed = rand();
    mPlanets.push_back(planet);
  }
}


void StarSystem::save(FILE* file) {
  // write the members
  fwrite(&mHandle, sizeof (mHandle), 1, file);
  fwrite(&mPosition, sizeof (mPosition), 1, file);
  fwrite(&mStarColor, sizeof (mStarColor)[0], 4, file);
  fwrite(&mStarRadius, sizeof (mStarRadius), 1, file);

  // write the planets
  size_t numPlanets = mPlanets.size();
  fwrite(&numPlanets, sizeof (numPlanets), 1, file);

  for (size_t i = 0; i < numPlanets; ++i) {
    fwrite(mPlanets[i], sizeof (Planet), 1, file);
//    mPlanets[i]->save(file);
  }
}


void StarSystem::load(FILE* file) {
  // clear out anything that might be in memory
  clear();

  // read the members
  fread(&mHandle, sizeof (mHandle), 1, file);
  fread(&mPosition, sizeof (mPosition), 1, file);
  fread(&mStarColor, sizeof (mStarColor[0]), 4, file);
  fread(&mStarRadius, sizeof (mStarRadius), 1, file);

  // load the planets
  size_t numPlanets;
  fread(&numPlanets, sizeof (numPlanets), 1, file);
  for (size_t i = 0; i < numPlanets; ++i) {
    Planet* planet = new Planet();
    if (planet == NULL) {
      printf("StarSystem::load(): error: out of memory\n");
      return;
    }

    fread(planet, sizeof (Planet), 1, file);
    mPlanets.push_back(planet);
//    mPlanets[i]->load(file);
  }
}

Planet* StarSystem::getPlanetByHandle(int handle) {
  for (size_t i = 0; i < mPlanets.size(); ++i) {
    if (mPlanets[i]->mHandle == handle) {
      return mPlanets[i];
    }
  }

  return NULL;
}

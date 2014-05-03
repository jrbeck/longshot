#include "Periodics.h"



Periodics::Periodics (void) {
  mSeed = 0;
  mPrng.setSeed(mSeed);
}



Periodics::~Periodics (void) {
}



int Periodics::saveToDisk (FILE *file) {
  int errorCode = 0;

/*	for (int i = 0; i < NUM_TERRAINS; i++) {
    errorCode = mTerrains[i].saveToDisk (file);
    if (errorCode != 0) {
      return errorCode;
    }
  }

  errorCode = mRandomMap.saveToDisk (file);
*/

  fwrite (&mSeed, sizeof mSeed, 1, file);

  return errorCode;
}


int Periodics::loadFromDisk(FILE *file) {
  int errorCode = 0;

  fread(&mSeed, sizeof mSeed, 1, file);
  printf("Periodics::loadFromDisk(): loaded seed: %d\n", mSeed);
  randomize(mSeed);

  return errorCode;
}


void Periodics::randomize(int seed) {
  mSeed = seed;
  mPrng.setSeed(seed);

  mTerrains[0].resize(1024);
  mTerrains[0].generateTilable(256.0, mPrng);
  mTerrains[0].normalize(-256.0, 256.0);

  mTerrains[1].resize(1024);
  mTerrains[1].generateTilable(128.0, mPrng);
  mTerrains[1].normalize(-128.0, 128.0);

  mTerrains[2].resize(1024);
  mTerrains[2].generateTilable(256.0, mPrng);
  mTerrains[2].normalize(-96.0, 96.0);

  mRandomMap.resize(256);
  mRandomMap.generateTilable(10.0, mPrng);
  mRandomMap.normalize(0.0, 1.0);

  mBiomeMap.randomize(mPrng, BIOME_TYPE_W, BIOME_TYPE_H);
}


int Periodics::getTerrainHeight (int x, int z) const {
  double u, v;

  u = 0.007759 * static_cast<double>(x);
  v = 0.007759 * static_cast<double>(z);

  double val1 = mTerrains[0].getValueBilerp (u, v);

  u = 0.05123 * static_cast<double>(x);
  v = 0.05123 * static_cast<double>(z);

  double val2 = mTerrains[1].getValueBilerp (u, v);

  u = 0.0256 * static_cast<double>(x);
  v = 0.0256 * static_cast<double>(z);

  double rVal = mRandomMap.getValueBilerp (u, v);

  u = 0.3756 * static_cast<double>(x);
  v = 0.3756 * static_cast<double>(z);

  double val3 = rVal * mTerrains[2].getValueBilerp (u, v);

  double height = val1 + val2 + val3;

  if (height < SEA_FLOOR_HEIGHT) {
    height = SEA_FLOOR_HEIGHT;
  }

  return static_cast<int>(floor (height));
}


BiomeInfo Periodics::getBiomeInfo(int x, int z) const {
  return mBiomeMap.getBiomeInfo(x, z);
}


double Periodics::getPrecipitationLevel (double worldX, double worldZ) const {
  return mRandomMap.getValueBilerp (worldX * 0.03257, worldZ * 0.03257);
}



BYTE Periodics::generateBlockAtWorldPosition (v3di_t worldPosition) {
  int terrainHeight = getTerrainHeight (worldPosition.x, worldPosition.z);

  return generateBlockAtWorldPosition (worldPosition, terrainHeight);
}



BYTE Periodics::generateBlockAtWorldPosition (v3di_t worldPosition, int terrainHeight) {
  double x = static_cast<double>(worldPosition.x);
  double y = static_cast<double>(worldPosition.y);
  double z = static_cast<double>(worldPosition.z);

  if (getPrecipitationLevel (x, z) < DESERT_MOISTURE_LEVEL) {
    return generateDesertBlock (worldPosition, terrainHeight);
  }

  double rValue = mRandomMap.getValueBilerp (x * 0.257, z * 0.257);

  BYTE blockType;

  double value1 = noise (x * 0.07, y * 0.07, z * 0.07);
  double value2 = noise (x * 0.013 + (rValue * 0.05),
    y * 0.013 + (rValue * 0.05),
    z * 0.013 + (rValue * 0.05));

  if (value1 < 0.5) {
    // ground type 1 (dirt)
    blockType = BLOCK_TYPE_DIRT;
  }
  else {
    // ground type 2 (stone)
    blockType = BLOCK_TYPE_STONE;
  }


  if (worldPosition.y < terrainHeight) { // below surface
    // underground - no change to block type
  }
  else if (worldPosition.y == terrainHeight) { // surface
    if (worldPosition.y >= WATER_LEVEL + SNOW_LEVEL + static_cast<int>(floor ((rValue * 20.0)))) { // high altitude
      if (blockType == BLOCK_TYPE_STONE) {
        blockType = BLOCK_TYPE_STONE_SNOW;
      }
      else if (blockType == BLOCK_TYPE_DIRT) {
        blockType = BLOCK_TYPE_DIRT_SNOW;
      }
    }
    else if (worldPosition.y >= WATER_LEVEL + static_cast<int>(floor ((rValue * 2.5)))) { // mid/low altitude
      if (blockType == BLOCK_TYPE_DIRT) {
        // main groundcover (grass)
//				blockType = BLOCK_TYPE_GRASS;
        blockType = BLOCK_TYPE_GRASS;
      }
      else if (blockType == BLOCK_TYPE_STONE) {
        blockType = BLOCK_TYPE_STONE_GRASS;
      }
    }
    else { // shoreline altitude
      if (rValue < 0.75) {
        // sand
        blockType = BLOCK_TYPE_SAND;
      }
    }


    // TESTING!!! overwrite whatever just happened...
    BiomeInfo bi = mBiomeMap.getBiomeInfo(x, z);
    if (bi.value < 0.95) BLOCK_TYPE_WATER;
    else blockType = BLOCK_TYPE_SAND + bi.type;
//    blockType = BLOCK_TYPE_SAND + bi.type;

  }
  else { // above surface
    int positiveNoiseHeight = static_cast<int>(floor (
      25.0 * mRandomMap.getValueBilerp (
      x * 0.00321,
      z * 0.00321)));

    double value = mRandomMap.getValueBilerp (x * 0.357, z * 0.357);

    if (worldPosition.y < terrainHeight + positiveNoiseHeight &&
      worldPosition.y > WATER_LEVEL &&
      terrainHeight >= WATER_LEVEL &&
      /* value1 > 0.0 &&*/
      value2 > value * 0.75) {
      if (worldPosition.y >= WATER_LEVEL + SNOW_LEVEL + static_cast<int>(floor ((rValue * 20.0)))) {
        blockType = BLOCK_TYPE_STONE_SNOW;
      }
      else {
        // outcroppings
        blockType = BLOCK_TYPE_MED_STONE;
      }
    }
    else {
      if (worldPosition.y < WATER_LEVEL) {
        // water
        blockType = gWaterBlockType;
      }
      else {
        blockType = BLOCK_TYPE_AIR;
//				blockType = BLOCK_TYPE_ALIEN_SKIN;
      }
    }
  }
  
  return blockType;
}



BYTE Periodics::generateDesertBlock (v3di_t worldPosition, int terrainHeight) {
  double x = static_cast<double>(worldPosition.x);
  double y = static_cast<double>(worldPosition.y);
  double z = static_cast<double>(worldPosition.z);

  double rValue = mRandomMap.getValueBilerp (x * 0.257, z * 0.257);

  BYTE blockType;

  double value1 = noise (x * 0.07, y * 0.07, z * 0.07);
  double value2 = noise (x * 0.013 + (rValue * 0.05),
    y * 0.013 + (rValue * 0.05),
    z * 0.013 + (rValue * 0.05));

  if (value1 < 0.5) {
    // ground type 1 (sand)
//		blockType = BLOCK_TYPE_SAND;
    blockType = BLOCK_TYPE_SAND;
  }
  else {
    // ground type 2 (stone)
//		blockType = BLOCK_TYPE_SAND;
    blockType = BLOCK_TYPE_SAND;
  }


  if (worldPosition.y < terrainHeight) {
    // underground
  }
  else if (worldPosition.y == terrainHeight) {
    if (worldPosition.y >= WATER_LEVEL + SNOW_LEVEL + static_cast<int>(floor ((rValue * 20.0)))) {
      if (blockType == BLOCK_TYPE_STONE) {
        blockType = BLOCK_TYPE_STONE_SNOW;
      }
      else if (blockType == BLOCK_TYPE_DIRT) {
        blockType = BLOCK_TYPE_DIRT_SNOW;
      }
    }
    else if (worldPosition.y >= WATER_LEVEL + static_cast<int>(floor ((rValue * 2.5)))) {
      if (blockType == BLOCK_TYPE_SAND) {
        // main groundcover (grass)
        blockType = BLOCK_TYPE_SAND;
      }
      else if (blockType == BLOCK_TYPE_MARS) {
        blockType = BLOCK_TYPE_MARS;
      }
    }
    else {
      if (rValue < 0.75) {
        // sand
        blockType = BLOCK_TYPE_SAND;
      }
    }

    // TESTING!!! overwrite whatever just happened...
    BiomeInfo bi = mBiomeMap.getBiomeInfo(x, z);
    if (bi.value < 0.95) BLOCK_TYPE_WATER;
    else blockType = BLOCK_TYPE_SAND + bi.type;
//    blockType = BLOCK_TYPE_SAND + bi.type;


  }

  else {
    int positiveNoiseHeight = static_cast<int>(floor (
      25.0 * mRandomMap.getValueBilerp (
      x * 0.00321,
      z * 0.00321)));

    double value = mRandomMap.getValueBilerp (x * 0.357, z * 0.357);

    if (worldPosition.y < terrainHeight + positiveNoiseHeight &&
      worldPosition.y > WATER_LEVEL &&
      terrainHeight >= WATER_LEVEL &&
      /* value1 > 0.0 &&*/
      value2 > value * 0.75) {
      if (worldPosition.y >= WATER_LEVEL + SNOW_LEVEL + static_cast<int>(floor ((rValue * 20.0)))) {
        blockType = BLOCK_TYPE_STONE_SNOW;
      }
      else {
        // outcroppings
        blockType = BLOCK_TYPE_MED_STONE;
      }
    }
    else {
      if (worldPosition.y < WATER_LEVEL) {
        // water
        blockType = gWaterBlockType;
      }
      else {
        blockType =  BLOCK_TYPE_AIR;
//				blockType =  BLOCK_TYPE_ALIEN_SKIN;
      }
    }
  }


  return blockType;
}


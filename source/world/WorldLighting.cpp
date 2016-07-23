#include "../world/WorldLighting.h"

// this is only 256 * 2 * sizeof(int)
// this is fine as long we don't thread this
int layerBuffer[WORLD_CHUNK_SIDE_SQUARED * 2];

void WorldLighting::createShadowVolume(int columnIndex, WorldMap& worldMap) {
  v3di_t worldPosition;
  v3di_t kernelPos;
  char blockType;
  int currentLightLevel;
  int lightAttenuation;
  int lightValue;
  int lightSum;
  int lightCount;

//  int kernel[9];

  int highestBlockHeight = worldMap.mColumns[columnIndex].getHighestBlockHeight();
  int lowestBlockHeight = worldMap.mColumns[columnIndex].getLowestBlockHeight();

  int layer = 0;

  // we'll need 1 plane for the reading, 1 for the writing
  // i.e. WORLD_CHUNK_SIDE x 2 x WORLD_CHUNK_SIDE (X x Y x Z)
  // FIXME: please don't do this every time!!
//  BYTE *buffer = new BYTE[WORLD_CHUNK_SIDE_SQUARED * 2];

  #define bufferIndex(x,y,z)    ((x)+((z)*WORLD_CHUNK_SIDE)+((y)*WORLD_CHUNK_SIDE_SQUARED))

  // these get swapped (0, 1) at each layer
  int readPlane = 0;
  int writePlane = (readPlane + 1) % 2;

  // initialize it with sunlight!
  for (int x = 0; x < WORLD_CHUNK_SIDE; x++) {
    for (int z = 0; z < WORLD_CHUNK_SIDE; z++) {
      layerBuffer[bufferIndex(x, readPlane, z)] = worldMap.mWorldLightingCeiling;
    }
  }

  // we'll use this to do false radiosity
  bool isSolidBelow;
  #define NEIGHBOR_ATTENUATION    2

  for (worldPosition.y = highestBlockHeight; worldPosition.y >= lowestBlockHeight; worldPosition.y--) {
    worldPosition.z = worldMap.mColumns[columnIndex].mWorldPosition.z;
    for (int z = 0; z < WORLD_CHUNK_SIDE; z++, worldPosition.z++) {
      worldPosition.x = worldMap.mColumns[columnIndex].mWorldPosition.x;
      for (int x = 0; x < WORLD_CHUNK_SIDE; x++, worldPosition.x++) {

        blockType = worldMap.mColumns[columnIndex].getBlockType(worldPosition);
        lightAttenuation = gBlockData.get(blockType)->lightAttenuation;

        if (lightAttenuation >= LIGHT_ATTENUATION_MAX) { // opaque
          worldMap.mColumns[columnIndex].setUniqueLighting(worldPosition, worldMap.mWorldLightingFloor);
          layerBuffer[bufferIndex(x, writePlane, z)] = LIGHT_LEVEL_SOLID;
        }
        else {  // transparent block
          kernelPos.x = worldPosition.x;
          kernelPos.y = worldPosition.y - 1;
          kernelPos.z = worldPosition.z;

          isSolidBelow =
            gBlockData.get(worldMap.mColumns[columnIndex].getBlockType(kernelPos))->lightAttenuation >= LIGHT_ATTENUATION_MAX ? true : false;

          int numSolidNeighbors = 0;
          if (isSolidBelow) {
            numSolidNeighbors = getNumSolidNeighbors(worldPosition, worldMap);
          }

          // check directly overhead
          kernelPos.x = worldPosition.x;
          kernelPos.y = worldPosition.y + 1;
          kernelPos.z = worldPosition.z;

//          lightSum = getUniqueLighting(kernelPos);
          lightSum = layerBuffer[bufferIndex(x, readPlane, z)];
          lightCount = 1;

          if (lightSum == worldMap.mWorldLightingCeiling ||
            lightSum == LIGHT_LEVEL_INVALID)
          {
            // if we're getting sunlight from above, just propagate it
            currentLightLevel = worldMap.mWorldLightingCeiling - lightAttenuation - (numSolidNeighbors * NEIGHBOR_ATTENUATION);
            currentLightLevel = max(currentLightLevel, (int)worldMap.mWorldLightingFloor);
//            currentLightLevel = min (currentLightLevel, (int)worldMap.mWorldLightingCeiling);

            worldMap.mColumns[columnIndex].setUniqueLighting(
              worldPosition, currentLightLevel);
            layerBuffer[bufferIndex(x, writePlane, z)] = currentLightLevel;
          }
          else {
            if (lightSum == LIGHT_LEVEL_SOLID) {
              lightSum = worldMap.mWorldLightingFloor;
            }

            // X - 1, Z - 1 * * * * * * * * * * * * * *
            if (x > 0 && z > 0) {
              lightValue = layerBuffer[bufferIndex(x - 1, readPlane, z - 1)];
            }
            else {
              kernelPos.x = worldPosition.x - 1;
              kernelPos.z = worldPosition.z - 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X - 1, Z + 0 * * * * * * * * * * * * * *
            if (x > 0) {
              lightValue = layerBuffer[bufferIndex(x - 1, readPlane, z)];
            }
            else {
              kernelPos.x = worldPosition.x - 1;
              kernelPos.z = worldPosition.z;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X - 1, Z + 1 * * * * * * * * * * * * * *
            if (x > 0 && z < (WORLD_CHUNK_SIDE - 1)) {
              lightValue = layerBuffer[bufferIndex(x - 1, readPlane, z + 1)];
            }
            else {
              kernelPos.x = worldPosition.x - 1;
              kernelPos.z = worldPosition.z + 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X + 0, Z - 1 * * * * * * * * * * * * * *
            if (z > 0) {
              lightValue = layerBuffer[bufferIndex(x, readPlane, z - 1)];
            }
            else {
              kernelPos.x = worldPosition.x;
              kernelPos.z = worldPosition.z - 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X + 0, Z + 1 * * * * * * * * * * * * * *
            if (z < (WORLD_CHUNK_SIDE - 1)) {
              lightValue = layerBuffer[bufferIndex(x, readPlane, z + 1)];
            }
            else {
              kernelPos.x = worldPosition.x;
              kernelPos.z = worldPosition.z + 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X + 1, Z - 1 * * * * * * * * * * * * * *
            if (x < (WORLD_CHUNK_SIDE - 1) && z > 0) {
              lightValue = layerBuffer[bufferIndex(x + 1, readPlane, z - 1)];
            }
            else {
              kernelPos.x = worldPosition.x + 1;
              kernelPos.z = worldPosition.z - 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X + 1, Z + 0 * * * * * * * * * * * * * *
            if (x < (WORLD_CHUNK_SIDE - 1)) {
              lightValue = layerBuffer[bufferIndex(x + 1, readPlane, z)];
            }
            else {
              kernelPos.x = worldPosition.x + 1;
              kernelPos.z = worldPosition.z;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            // X + 1, Z + 1 * * * * * * * * * * * * * *
            if (x < (WORLD_CHUNK_SIDE - 1) && z < (WORLD_CHUNK_SIDE - 1)) {
              lightValue = layerBuffer[bufferIndex(x + 1, readPlane, z + 1)];
            }
            else {
              kernelPos.x = worldPosition.x + 1;
              kernelPos.z = worldPosition.z + 1;
              lightValue = worldMap.getUniqueLighting(kernelPos);
            }

            if (lightValue == LIGHT_LEVEL_INVALID ||
              lightValue == LIGHT_LEVEL_NOT_SET ||
              lightValue == LIGHT_LEVEL_SOLID)
            {
              // ignore
            }
            else {
              lightCount++;
              lightSum += lightValue;
            }

            currentLightLevel = (lightSum / lightCount) - lightAttenuation - (numSolidNeighbors * NEIGHBOR_ATTENUATION);
            currentLightLevel = max(currentLightLevel, (int)worldMap.mWorldLightingFloor);
//            currentLightLevel = min (currentLightLevel, (int)worldMap.mWorldLightingCeiling);
            worldMap.mColumns[columnIndex].setUniqueLighting(worldPosition, currentLightLevel);
            layerBuffer[bufferIndex(x, writePlane, z)] = currentLightLevel;
          }
        }
      } // x
    } // z
    layer++;

    // this looks weird, but ensures that the two are always
    // opposing
    readPlane = (readPlane + 1) % 2;
    writePlane = (readPlane + 1) % 2;

  } // y


//  delete [] buffer;

  worldMap.mColumns[columnIndex].mNeedShadowVolume = false;
  worldMap.mColumns[columnIndex].mNeedVisibility = true;
  worldMap.mColumns[columnIndex].mNeedLightingApplied = false;
  worldMap.mColumns[columnIndex].mNeedDisplayList = false;
}

int WorldLighting::getNumSolidNeighbors( v3di_t worldPosition, const WorldMap& worldMap ) {
  int numSolidNeighbors = 0;

  // left
  worldPosition.x -= 1;
  if (gBlockData.get(worldMap.getBlockType(worldPosition))->lightAttenuation >= LIGHT_ATTENUATION_MAX) {
    numSolidNeighbors++;
  }

  // right
  worldPosition.x += 2;
  if (gBlockData.get(worldMap.getBlockType(worldPosition))->lightAttenuation >= LIGHT_ATTENUATION_MAX) {
    numSolidNeighbors++;
  }

  // fore
  worldPosition.x -= 1;
  worldPosition.z -= 1;
  if (gBlockData.get(worldMap.getBlockType(worldPosition))->lightAttenuation >= LIGHT_ATTENUATION_MAX) {
    numSolidNeighbors++;
  }

  // aft
  worldPosition.z += 2;
  if (gBlockData.get(worldMap.getBlockType(worldPosition))->lightAttenuation >= LIGHT_ATTENUATION_MAX) {
    numSolidNeighbors++;
  }

  return numSolidNeighbors;
}

void WorldLighting::applyLighting(
  int columnIndex,
  WorldMap& worldMap,
  const LightManager& lightManager,
  IntColor& sunColor )
{
  // apply to faces
  v3di_t worldPosition, relativePosition, neighborPosition;
  v3di_t chunkPosition;
  block_t *block, *neighbor;
  BYTE neighborType;
  IntColor level;

  float sunlightgMult;

  for (size_t chunkIndex = 0; chunkIndex < worldMap.mColumns[columnIndex].mWorldChunks.size(); chunkIndex++) {
    chunkPosition = worldMap.mColumns[columnIndex].mWorldChunks[chunkIndex]->mWorldPosition;

    for (relativePosition.z = 0; relativePosition.z < WORLD_CHUNK_SIDE; relativePosition.z++) {
      for (relativePosition.y = 0; relativePosition.y < WORLD_CHUNK_SIDE; relativePosition.y++) {
        for (relativePosition.x = 0; relativePosition.x < WORLD_CHUNK_SIDE; relativePosition.x++) {
          worldPosition = v3di_add (chunkPosition, relativePosition);

          block = worldMap.mColumns[columnIndex].getBlockAtWorldPosition (worldPosition);

          if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
            // plants...
            level = lightManager.getLightLevel(worldPosition);
            sunlightgMult = (float)block->uniqueLighting / (float)LIGHT_LEVEL_MAX;
            level.r += sunlightgMult * sunColor.r;
            level.g += sunlightgMult * sunColor.g;
            level.b += sunlightgMult * sunColor.b;
            level.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);

            // plant hack...we store the colored lighting info
            // in the first face
            block->faceLighting[0][0] = level.r;
            block->faceLighting[0][1] = level.g;
            block->faceLighting[0][2] = level.b;
          }
          else if (gBlockData.get(block->type)->alpha > 0.0 &&
            block->faceVisibility != 0)
          {
            for (int i = 0; i < NUM_BLOCK_SIDES; ++i) {
              if (block->faceVisibility & gBlockSideBitmaskLookup[i]) {
                neighborPosition = v3di_add (worldPosition, gBlockNeighborAddLookup[i]);
                neighbor = worldMap.getBlock (neighborPosition);
                if (neighbor == NULL) {
                  neighborType = BLOCK_TYPE_INVALID;
                }
                else {
                  neighborType = neighbor->type;
                }

                if (neighborType == BLOCK_TYPE_INVALID) {
                  // these are PROBABLY directly under the sun
                  level = lightManager.getLightLevel(worldPosition);
                  sunlightgMult = (float)worldMap.mWorldLightingCeiling / (float)LIGHT_LEVEL_MAX;
                  level.r += sunlightgMult * sunColor.r;
                  level.g += sunlightgMult * sunColor.g;
                  level.b += sunlightgMult * sunColor.b;
                  level.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);

                  block->faceLighting[i][0] = level.r;
                  block->faceLighting[i][1] = level.g;
                  block->faceLighting[i][2] = level.b;
                }
                else {
                  if (neighbor->uniqueLighting > LIGHT_LEVEL_MAX) {
                    // this is to highlight problems
                    block->faceLighting[i][0] = LIGHT_LEVEL_MAX;
                    block->faceLighting[i][1] = LIGHT_LEVEL_MIN;
                    block->faceLighting[i][2] = LIGHT_LEVEL_MIN;
                  }
                  else {
                    level = lightManager.getLightLevel(worldPosition);
                    sunlightgMult = (float)neighbor->uniqueLighting / (float)LIGHT_LEVEL_MAX;
                    level.r += sunlightgMult * sunColor.r;
                    level.g += sunlightgMult * sunColor.g;
                    level.b += sunlightgMult * sunColor.b;
                    level.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);

                    block->faceLighting[i][0] = level.r;
                    block->faceLighting[i][1] = level.g;
                    block->faceLighting[i][2] = level.b;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // gotta update these
  worldMap.mColumns[columnIndex].mNeedShadowVolume = false;
  worldMap.mColumns[columnIndex].mNeedVisibility = false;
  worldMap.mColumns[columnIndex].mNeedLightingApplied = false;
  worldMap.mColumns[columnIndex].mNeedDisplayList = true;
}

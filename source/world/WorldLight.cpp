#include "../world/WorldLight.h"

WorldLight::WorldLight() :
  mBuffer(nullptr)
{
}

WorldLight::~WorldLight() {
  delete [] mBuffer;
}

void WorldLight::set(const v3d_t& position, double radius, const IntColor& color) {
  mWorldPosition = position;
  mRadius = radius;
  mColor.r = color.r;
  mColor.g = color.g;
  mColor.b = color.b;

  v3di_t intPosition = v3di_v(mWorldPosition);

  mBufferNear.x = intPosition.x - (int)ceil(mRadius);
  mBufferNear.y = intPosition.y - (int)ceil(mRadius);
  mBufferNear.z = intPosition.z - (int)ceil(mRadius);

  mBufferFar.x = intPosition.x + (int)ceil(mRadius);
  mBufferFar.y = intPosition.y + (int)ceil(mRadius);
  mBufferFar.z = intPosition.z + (int)ceil(mRadius);

  mBufferDimensions.x = mBufferFar.x - mBufferNear.x + 1;
  mBufferDimensions.y = mBufferFar.y - mBufferNear.y + 1;
  mBufferDimensions.z = mBufferFar.z - mBufferNear.z + 1;

  initBuffer();
}

void WorldLight::initBuffer() {
  if (mBuffer != nullptr) {
    delete [] mBuffer;
    mBuffer = nullptr;
  }

  mBuffer = new IntColor[mBufferDimensions.x * mBufferDimensions.y * mBufferDimensions.z];
  if (mBuffer == nullptr) {
    printf("WorldLight::initBuffer(): error: out of memory\n");
    return;
  }

  v3di_t bufferPos;
  v3d_t worldPos;  // the v3d version of the current block world position

  int bufferIndex = 0;
  for (bufferPos.z = 0; bufferPos.z < mBufferDimensions.z; bufferPos.z++) {
    // the +0.5 puts it in the center of the block area
    worldPos.z = (double)(mBufferNear.z + bufferPos.z) + 0.5;
    for (bufferPos.y = 0; bufferPos.y < mBufferDimensions.y; bufferPos.y++) {
      worldPos.y = (double)(mBufferNear.y + bufferPos.y) + 0.5;
      for (bufferPos.x = 0; bufferPos.x < mBufferDimensions.x; bufferPos.x++) {
        worldPos.x = (double)(mBufferNear.x + bufferPos.x) + 0.5;

        v3d_t diff = v3d_sub(worldPos, mWorldPosition);
        double sum = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
        double dist = sqrt(sum);

        if (dist <= mRadius) {
          double scalar = (1.0 - (dist / mRadius));
          mBuffer[bufferIndex].r = (int)floor((double)mColor.r * scalar);
          mBuffer[bufferIndex].g = (int)floor((double)mColor.g * scalar);
          mBuffer[bufferIndex].b = (int)floor((double)mColor.b * scalar);
        }
        else {
          mBuffer[bufferIndex].r = 0;
          mBuffer[bufferIndex].g = 0;
          mBuffer[bufferIndex].b = 0;
        }

        // on to the next position
        bufferIndex++;
      }
    }
  }
}

const IntColor& WorldLight::getLevel(const v3di_t& worldPosition) const {
  static IntColor noColor = { 0, 0, 0 };
  if (isInVolume(worldPosition) == false ||
    mBuffer == nullptr)
  {
    return noColor;
  }

  // compute the relative position and
  return mBuffer[
    (worldPosition.x - mBufferNear.x) +
    ((worldPosition.y - mBufferNear.y) * mBufferDimensions.x) +
    ((worldPosition.z - mBufferNear.z) * mBufferDimensions.x * mBufferDimensions.y)];
}


bool WorldLight::isInVolume(const v3di_t& worldPosition) const {
  if (worldPosition.x < mBufferNear.x ||
    worldPosition.x > mBufferFar.x ||
    worldPosition.y < mBufferNear.y ||
    worldPosition.y > mBufferFar.y ||
    worldPosition.z < mBufferNear.z ||
    worldPosition.z > mBufferFar.z)
  {
    return false;
  }

  return true;
}

void WorldLight::turnOff() {
  delete [] mBuffer;
  mBuffer = nullptr;
}

void WorldLight::turnOn() {
  initBuffer();
}

void WorldLight::save(FILE* file) const {
  fwrite(&mWorldPosition, sizeof (v3d_t), 1, file);
  fwrite(&mRadius, sizeof (double), 1, file);
  fwrite(&mColor.r, sizeof (int), 1, file);
  fwrite(&mColor.g, sizeof (int), 1, file);
  fwrite(&mColor.b, sizeof (int), 1, file);
}

void WorldLight::load(FILE* file) {
  fread(&mWorldPosition, sizeof (v3d_t), 1, file);
  fread(&mRadius, sizeof (double), 1, file);
  fread(&mColor.r, sizeof (int), 1, file);
  fread(&mColor.g, sizeof (int), 1, file);
  fread(&mColor.b, sizeof (int), 1, file);

  set(mWorldPosition, mRadius, mColor);
}

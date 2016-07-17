#include "PseudoRandom.h"

PseudoRandom::PseudoRandom() {
  setSeed(mDefaultSeed);
}

PseudoRandom::PseudoRandom(UINT seed) {
  setSeed(seed);
}

void PseudoRandom::setSeed(UINT seed) {
  mX = mSeed = seed;
}

UINT PseudoRandom::getSeed() const {
  return mSeed;
}

int PseudoRandom::getNextInt() {
  int value = (int)((mX = ((mA * mX) + mC)) >> 16u);

  // FIXME: is there a faster way to do this?
  // perhaps value = (value % mRandMaxMod)...
  while (value > mRandMax) {
    value -= mRandMaxMod;
  }

  return value;
}

// returns int in [low, high)
int PseudoRandom::getNextInt(int low, int high) {
  double val = (double)getNextInt ();
  return (int)floor(((val / (double)(mRandMax + 1)) * (double)(high - low)) + (double)low);
}

double PseudoRandom::getNextDouble() {
  return (double)getNextInt () / ((double)mRandMax + 1.0);
}

double PseudoRandom::getNextDouble(double low, double high) {
  double nextInt = (double)getNextInt ();
  return (((nextInt / (double)(mRandMaxMod)) * (high - low)) + low);
}

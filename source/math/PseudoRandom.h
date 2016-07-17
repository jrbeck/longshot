// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PseudoRandom
// *
// * this is a linear congruential generator designed to produce the same output
// * as the msvc++ generator, but with some extra built-in utility.
// *
// *  the numbers are generated as:
// *  X[0] = seed value
// *  X[n + 1] = (a * X[n]) + c
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef PseudoRandom_h_
#define PseudoRandom_h_

#include <cmath>

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

class PseudoRandom {
public:
  PseudoRandom();
  PseudoRandom(UINT seed);

  void setSeed(UINT seed);
  UINT getSeed(void) const;

  int getNextInt(void);
  int getNextInt(int low, int high);

  double getNextDouble(void);
  double getNextDouble(double low, double high);

  static const UINT mDefaultSeed = 0;
  static const UINT mA = 214013;
  static const UINT mC = 2531011;
  static const UINT mRandMax = 32767;
  static const UINT mRandMaxMod = mRandMax + 1;

private:
  UINT mSeed;
  UINT mX;
};


#endif // PseudoRandom_h_

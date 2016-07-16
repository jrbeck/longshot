// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Terrain
// *
// * this generates (2^n)X(2^n) tilable noise maps
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Terrain_h_
#define Terrain_h_

#include "GL/glut.h"

#include "MathUtil.h"
#include "PseudoRandom.h"
#include "v2d.h"
#include "v3d.h"


// defines * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define DEFAULT_FIELD_SIDE      (32)

#define DEFAULT_TERRAIN_OFFSET    (16.0)


//  class definition * * * * * * * * * * * * * * * * * * * * * * *
class Terrain {
public:
  Terrain (void);
  Terrain (int side);
  ~Terrain (void);

  int saveToDisk (FILE *file);
  int loadFromDisk (FILE *file);

  int resize(int side);

  int get_side_length (void) const;

  void generateTilable (double delta, PseudoRandom &prng);

  void normalize (double low, double high);

  double get_value (int i, int j) const;
  double getValueBilerp (double i, double j) const;

  int set_value (int i, int j, double value);

  int set_all (double value);
  int set_min (double min);

  int smooth (void);
  void noise (PseudoRandom &prng);
  double blur_square (int i, int j, int side) const;

  void alt_smooth (void);

  double getMax (void) const;
  double getMin (void) const;

  // gives the value of the lowest neighbor of the eight surrounding points
  double lowest_neighbor (int i, int j) const;

  void draw (void) const;

private:
  double getClose(double num, double delta, PseudoRandom &prng) const;


  int mFieldSide;
  double *mField;
};




#endif // Terrain_h_

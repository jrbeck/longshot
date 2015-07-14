#include "Terrain.h"



Terrain::Terrain (void) :
  mFieldSide(0),
  mField(NULL)
{
  if (resize (DEFAULT_FIELD_SIDE) == 0) {
    set_all (0.0);
  }
}



Terrain::Terrain (int side) :
  mField(NULL),
  mFieldSide(0)
{
  if (resize (side) == 0) {
    set_all (0.0);
  }
}



Terrain::~Terrain (void) {
  // free the field
  if (mField != NULL) {
    delete [] mField;
  }
}



int Terrain::saveToDisk (FILE *file) {
  if (mField == NULL ||
    mFieldSide <= 0) {
    return 1;
  }

  fwrite (&mFieldSide, sizeof mFieldSide, 1, file);
  fwrite (&mField, sizeof mField[0], mFieldSide * mFieldSide, file);

  return 0;
}



int Terrain::loadFromDisk (FILE *file) {
  int fieldSide;
  fread (&fieldSide, sizeof fieldSide, 1, file);

  resize (fieldSide);

  if (mField == NULL) {
    // memory alloc error
    printf ("Terrain::loadFromDisk(): out of mem");
    return -1;
  }

  fread (&mField, sizeof mField[0], mFieldSide * mFieldSide, file);

  printf ("Terrain loaded: %d\n", mFieldSide);

  return 0;
}



int Terrain::resize (int side) {
  // if the field is already allocated and of the same size, nothing to do
  if (mFieldSide == side && mField != NULL) {
    return 0;
  }

  // free the old field
  if (mField != NULL) {
    delete [] mField;
    mField = NULL;
  }

  // allocate the field
  mField = new double[side * side];
  if (mField == NULL) {
    mFieldSide = 0;
    return -1;
  }

  mFieldSide = side;

  // success
  return 0;
}


// **************************************************
//  resize the terrain
// **************************************************
int Terrain::get_side_length (void) const {
  return mFieldSide;
}


// **************************************************
//  start the subdivision process
//    set one corner as seed
// **************************************************
void Terrain::generateTilable (double delta, PseudoRandom &prng) {
//  printf ("\nterrain_c::seed (delta: %.2f)\n", delta);
  // bail if we've got nothing to work with
  if (mField == NULL || mFieldSide < 2) return;

  // seed bl corner
  set_value (0, 0, 0.0);
//  set_all (0.0);

  double avg;
  v2di_t bl;
  int add = mFieldSide >> 1;
  int addTimesTwo;

  // get'r done
  while (add > 0) {
    addTimesTwo = add << 1;

    // squares
    for (bl.y = 0; bl.y < mFieldSide; bl.y += addTimesTwo) {
      for (bl.x = 0; bl.x < mFieldSide; bl.x += addTimesTwo) {
        avg = (get_value (bl.x, bl.y) +
            get_value (bl.x + addTimesTwo, bl.y) +
            get_value (bl.x, bl.y + addTimesTwo) +
            get_value (bl.x + addTimesTwo, bl.y + addTimesTwo)) * 0.25;

        set_value (bl.x + add, bl.y + add, getClose(avg, delta, prng));
      }
    }

    // diamonds
    for (bl.y = 0; bl.y < mFieldSide; bl.y += addTimesTwo) {
      for (bl.x = 0; bl.x < mFieldSide; bl.x += addTimesTwo) {
        // diamond: left
        avg = (get_value (bl.x, bl.y + addTimesTwo) +
            get_value (bl.x + add, bl.y + add) +
            get_value (bl.x, bl.y) +
            get_value (bl.x - add, bl.y + add)) * 0.25;

        set_value (bl.x, bl.y + add, getClose (avg, delta, prng));

        // diamond: bottom
        avg = (get_value (bl.x + add, bl.y + add) +
            get_value (bl.x + addTimesTwo, bl.y) +
            get_value (bl.x + add, bl.y - add) +
            get_value (bl.x, bl.y)) * 0.25;

        set_value (bl.x + add, bl.y, getClose (avg, delta, prng));
      }
    }

    add >>= 1;
    delta *= 0.6;
  }

}


void Terrain::normalize(double low, double high) {
  double newRange = high - low;
  double oldMin = getMin();
  double oldMax = getMax();

  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      double oldValue = get_value (i, j);
      set_value(i, j, (newRange * (oldValue - oldMin) / (oldMax - oldMin)) + low);
    }
  }
}




// **************************************************
//  return the value of field at (i, j)
//  return 0.0 on error
// **************************************************
double Terrain::get_value (int i, int j) const {
/*  // off the map, or mFieldSide == 0
  if (x < 0 || x >= mFieldSide ||
    z < 0 || z >= mFieldSide) return 0.0;*/

  if (mField == NULL || mFieldSide == 0) return 0.0;

  // ensure the i coord is on the map
  while (i < 0) i += mFieldSide;
  if (i >= mFieldSide) i = i % mFieldSide;

  // ensure the j coord is on the map
  while (j < 0) j += mFieldSide;
  if (j >= mFieldSide) j = j % mFieldSide;

  return mField[i + (j * mFieldSide)];
}



double Terrain::getValueBilerp (double i, double j) const {
/*  if (mField == NULL || mFieldSide == 0) return 0.0;

  double side = static_cast<double>(mFieldSide);

  // ensure the i coord is on the map
  while (i < 0.0) i += side;
  if (i >= (side)) {
    i = fmod (i, side);
  }

  // ensure the j coord is on the map
  while (j < 0.0) j += side;
  if (j >= (side)) {
    j = fmod (j, side);
  }

  v2di_t corner[4];

  corner[0].x = static_cast<int>(floor (i));
  corner[0].y = static_cast<int>(floor (j));

  corner[1].x = static_cast<int>(floor (i));
  corner[1].y = static_cast<int>(ceil (j)) % mFieldSide;

  corner[2].x = static_cast<int>(fmod (ceil (i), side));
  corner[2].y = static_cast<int>(ceil (j)) % mFieldSide;

  corner[3].x = static_cast<int>(fmod (ceil (i), side));
  corner[3].y = static_cast<int>(floor (j));

  //printf ("%.3f %.3f\n", i, j);
  //v2di_print ("1", corner[0]);
  //v2di_print ("2", corner[1]);
  //v2di_print ("3", corner[2]);
  //v2di_print ("4", corner[3]);

  // check if we are on a point
  if (corner[0].x == corner[2].x &&
    corner[0].y == corner[1].y) {
    return mField[corner[0].x + (corner[0].y * mFieldSide)];
  }

  // check if we are on an edge
  if (corner[0].x == corner[2].x) {
    double a = mField[corner[0].x + (corner[0].y * mFieldSide)];
    double b = mField[corner[0].x + (corner[1].y * mFieldSide)];

    return lerp (a, b, j - floor (j));
  }
  if (corner[0].y == corner[1].y) {
    double a = mField[corner[0].x + (corner[0].y * mFieldSide)];
    double b = mField[corner[2].x + (corner[0].y * mFieldSide)];

    return lerp (a, b, i - floor (i));
  }

  // dang somewhere in the middle
  double a = mField[corner[0].x + (corner[0].y * mFieldSide)];
  double b = mField[corner[1].x + (corner[1].y * mFieldSide)];
  double c = mField[corner[2].x + (corner[2].y * mFieldSide)];
  double d = mField[corner[3].x + (corner[3].y * mFieldSide)];

  double l1 = lerp (a, d, i - floor (i));
  double l2 = lerp (b, c, i - floor (i));

  return lerp (l1, l2, j - floor (j));
*/
////////////////////////////////////////////////////////////////////////////////////
  if (mField == NULL || mFieldSide == 0) return 0.0;

  double side = static_cast<double>(mFieldSide);

  // ensure the the coords are on the map
  while (i < 0.0) i += side;
  if (i >= (side)) {
    i = fmod (i, side);
  }

  while (j < 0.0) j += side;
  if (j >= (side)) {
    j = fmod (j, side);
  }

  v2di_t nearCorner;
  v2di_t farCorner;
  v2d_t unitPosition;

  nearCorner.x = static_cast<int>(floor (i));
  nearCorner.y = static_cast<int>(floor (j));

  unitPosition.x = i - static_cast<double>(nearCorner.x);
  unitPosition.y = j - static_cast<double>(nearCorner.y);

  farCorner.x = (nearCorner.x + 1) % mFieldSide;
  farCorner.y = (nearCorner.y + 1) % mFieldSide;

  double vert00 = mField[nearCorner.x + (nearCorner.y * mFieldSide)];
  double vert01 = mField[nearCorner.x + (farCorner.y * mFieldSide)];
  double vert10 = mField[farCorner.x + (nearCorner.y * mFieldSide)];
  double vert11 = mField[farCorner.x + (farCorner.y * mFieldSide)];

  // this was taken from: http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/index.html
  // section on trilinear interpolation. It has been adapted to bilinear interp
  return (vert00 * (1.0 - unitPosition.x) * (1.0 - unitPosition.y)) +
    (vert01 * (1.0 -unitPosition.x) * (unitPosition.y)) +
    (vert10 * (unitPosition.x) * (1.0 - unitPosition.y)) +
    (vert11 * (unitPosition.x) * (unitPosition.y));
}



// **************************************************
//  set the value at (i, j)
//  return 0 on success, -1 on error
// **************************************************
int Terrain::set_value (int i, int j, double value) {
  if (i < 0 || i >= mFieldSide ||
    j < 0 || j >= mFieldSide) return -1;

  mField[i + (j * mFieldSide)] = value;

  return 0;
}


// **************************************************
//  set the entire field to some value
// **************************************************
int Terrain::set_all (double value) {
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      set_value (i, j, value); } }

  return 0;
}


// **************************************************
//  if any elements in the field are below a certain number, set them to that number
// **************************************************
int Terrain::set_min (double min) {
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      // get the value at i, j
      double val = get_value (i, j);

      // if it's lower than our min, set it's new value
      if (val < min) {
        set_value (i, j, min);
      }
    }
  }

  return 0;
}


// **************************************************
//  use a simple blur filter to smooth the field
// **************************************************
int Terrain::smooth (void) {
  // initialize a scratch buffer
  double *buf = new double[mFieldSide * mFieldSide];

  // bail if that didn't work
  if (buf == NULL) return -1;

  // now fill the scratch buffer up with smoothed values
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      if (get_value (i, j) < 12.0) {
        buf[i + (j * mFieldSide)] = blur_square (i, j, 4); }
      if (get_value (i, j) < 24.0) {
        buf[i + (j * mFieldSide)] = blur_square (i, j, 2); }
      if (get_value (i, j) < 32.0) {
        buf[i + (j * mFieldSide)] = blur_square (i, j, 2); }
      else {
        buf[i + (j * mFieldSide)] = get_value (i, j);
      }
    }
  }

  // set the real field values
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      set_value (i, j, buf[i + (j * mFieldSide)]);
    }
  }

  // free up the scratch buffer
  delete [] buf;

  // success
  return 0;
}


void Terrain::noise (PseudoRandom &prng) {
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      double v = get_value (i, j);
      set_value (i, j, getClose (v, 1.0, prng));
    }
  }
}



void Terrain::alt_smooth (void) {
  // initialize a scratch buffer
  double *buf = (double *)malloc (sizeof (double) * mFieldSide * mFieldSide);

  // bail if that didn't work
  if (buf == NULL) return;

  double minValue = getMin ();
  double maxValue = getMax ();
  double range = maxValue - minValue;

  if (range == 0.0) range = 1.0;

  // now fill the scratch buffer up with smoothed values
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      double orig = get_value (i, j);
      double blur = blur_square (i, j, 4);

      double percent = (0.6 * (maxValue - orig) / range) + 0.4;
      percent = percent * percent * percent;

      buf[i + (j * mFieldSide)] = lerp (orig, blur, percent);
    }
  }

  // set the real field values
  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      set_value (i, j, buf[i + (j * mFieldSide)]); } }

  // free up the scratch buffer
  free (buf);
}



double Terrain::blur_square (int i, int j, int side) const {
  double total = 0.0;
  int count = 0;

  for (int b = j - side; b <= j + side; b++) {
    for (int a = i - side; a <= i + side; a++) {
      total += get_value (a, b);
      count++; } }

  total += (side * side * get_value (i, j));
  count += (side * side);

  return total / (double)count;
}



double Terrain::getMax (void) const {
  double value;
  double maxValue = mField[0];

  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      if ((value = get_value (i, j)) > maxValue) {
        maxValue = value;
      }
    }
  }

  return maxValue;
}



double Terrain::getMin (void) const {
  double value;
  double minValue = mField[0];

  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      if ((value = get_value (i, j)) < minValue) {
        minValue = value;
      }
    }
  }

  return minValue;
}


// gives the value of the lowest neighbor of the 4 surrounding points
double Terrain::lowest_neighbor (int i, int j) const {
  double lowest = 100000000.0;

//  if (get_value (i - 1, j - 1) < lowest) lowest = get_value (i - 1, j - 1);
  if (get_value (i,     j - 1) < lowest) lowest = get_value (i,     j - 1);
//  if (get_value (i + 1, j - 1) < lowest) lowest = get_value (i + 1, j - 1);
  if (get_value (i - 1, j    ) < lowest) lowest = get_value (i - 1, j    );
  if (get_value (i + 1, j    ) < lowest) lowest = get_value (i + 1, j    );
//  if (get_value (i - 1, j + 1) < lowest) lowest = get_value (i - 1, j + 1);
  if (get_value (i,     j + 1) < lowest) lowest = get_value (i,     j + 1);
//  if (get_value (i + 1, j + 1) < lowest) lowest = get_value (i + 1, j + 1);

  return lowest;
}



void Terrain::draw (void) const {
  double minValue = getMin ();
  double maxValue = getMax ();
  double range = maxValue - minValue;
  if (range == 0.0) range = 1.0;

  v3d_t corners[4];
  double colors[4];

  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glBegin (GL_QUADS);

  for (int j = 0; j < mFieldSide; j++) {
    for (int i = 0; i < mFieldSide; i++) {
      corners[0].x = static_cast<int>(i);
      corners[0].y = get_value (i, j);
      corners[0].z = static_cast<int>(j);

      corners[1].x = static_cast<int>(i);
      corners[1].y = get_value (i, j + 1);
      corners[1].z = static_cast<int>(j + 1);

      corners[2].x = static_cast<int>(i + 1);
      corners[2].y = get_value (i + 1, j + 1);
      corners[2].z = static_cast<int>(j + 1);

      corners[3].x = static_cast<int>(i + 1);
      corners[3].y = get_value (i + 1, j);
      corners[3].z = static_cast<int>(j);

      if (j == mFieldSide - 1 || i == mFieldSide - 1) {
        colors[0] = 0.0;
        colors[1] = 0.0;
        colors[2] = 0.0;
        colors[3] = 0.0;
      }
      else {
        colors[0] = sqrt((0.8 * (corners[0].y - minValue) / range) + 0.2);
        colors[1] = sqrt((0.8 * (corners[1].y - minValue) / range) + 0.2);
        colors[2] = sqrt((0.8 * (corners[2].y - minValue) / range) + 0.2);
        colors[3] = sqrt((0.8 * (corners[3].y - minValue) / range) + 0.2);
      }

      glColor3d (1.0 - colors[0], colors[0], colors[0]);
      glVertex3d (corners[0].x, corners[0].y, corners[0].z);

      glColor3d (1.0 - colors[1], colors[1], colors[1]);
      glVertex3d (corners[1].x, corners[1].y, corners[1].z);

      glColor3d (1.0 - colors[2], colors[2], colors[2]);
      glVertex3d (corners[2].x, corners[2].y, corners[2].z);

      glColor3d (1.0 - colors[3], colors[3], colors[3]);
      glVertex3d (corners[3].x, corners[3].y, corners[3].z);
    }
  }

  glEnd ();
  glEnable (GL_TEXTURE_2D);
}


double Terrain::getClose(double num, double delta, PseudoRandom &prng) const {
  return prng.getNextDouble(num - delta, num + delta);
}



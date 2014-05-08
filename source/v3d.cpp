#include "v3d.h"



void v3d_print(const char *str, v3d_t a) {
  printf("%s <%.3f, %.3f, %.3f>\n", str, a.x, a.y, a.z);
}


// return 1 if equal, 0 otherwise
int v3d_isequal(v3d_t a, v3d_t b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z) return 1;
  return 0;
}


// returns a vector of magnitude zero
v3d_t v3d_zero(void) {
  v3d_t a;

  a.x = 0.0;
  a.y = 0.0;
  a.z = 0.0;

  return a;
}


// zeroes out an existing vector
void v3d_zero(v3d_t *a) {
  a->x = 0.0;
  a->y = 0.0;
  a->z = 0.0;
}


// returns a v3d_t
v3d_t v3d_v(double x, double y, double z) {
  v3d_t a;

  a.x = x;
  a.y = y;
  a.z = z;

  return a;
}


v3d_t v3d_v(v3di_t v3di) {
  v3d_t a;

  a.x = static_cast<double>(v3di.x);
  a.y = static_cast<double>(v3di.y);
  a.z = static_cast<double>(v3di.z);

  return a;
}



// returns the magnitude of the vector
double v3d_mag(const v3d_t &a) {
  return sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}


// returns the distance between two points
double v3d_dist(v3d_t a, v3d_t b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;

  return sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}


// returns the unit vector in the same direction as a
v3d_t v3d_normalize(v3d_t a) {
  double len = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));

  if (len <= 0.0) return a;

  len = 1.0 / len;

  a.x *= len;
  a.y *= len;
  a.z *= len;

  return a;
}


// returns scalar * a (s is scalar, a is vector)
v3d_t v3d_scale(v3d_t a, double scalar) {
  a.x *= scalar;
  a.y *= scalar;
  a.z *= scalar;

  return a;
}


v3d_t v3d_scale(double scalar, v3d_t a) {
  a.x *= scalar;
  a.y *= scalar;
  a.z *= scalar;

  return a;
}


// returns a + b
v3d_t v3d_add(v3d_t a, v3d_t b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;

  return a;
}


// returns a - b
v3d_t v3d_sub(v3d_t a, v3d_t b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;

  return a;
}


// returns a dot b ( |a| * |b| * cos (theta) )
double v3d_dot(v3d_t a, v3d_t b) {
  return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}


// returns a cross b
v3d_t v3d_cross(v3d_t a, v3d_t b) {
  v3d_t c;
  
  c.x = (a.y * b.z) - (a.z * b.y);
  c.y = (a.z * b.x) - (a.x * b.z);
  c.z = (a.x * b.y) - (a.y * b.x);
  
  return c;
}


// returns the opposite vector
v3d_t v3d_neg(v3d_t a) {
  a.x = -a.x;
  a.y = -a.y;
  a.z = -a.z;

  return a;
}



// returns the projection of a onto b
v3d_t v3d_project(v3d_t a, v3d_t b) {
  v3d_t p;
  double dot = (a.x * b.x) + (a.y * b.y) + (a.z * b.z); // dot product of a and b
  double bsq = (b.x * b.x) + (b.y * b.y) + (b.z * b.z); // |b|^2

  // i can't deal with this! bail!
  if (bsq == 0.0) return v3d_zero();

  // so: dot = DOT (b) / |b|^2
  dot /= bsq;

  p.x = dot * b.x;
  p.y = dot * b.y;
  p.z = dot * b.z;

  return p;
}



// returns the v3d that is located {percent} of the distance from a to b
v3d_t v3d_interpolate(v3d_t a, v3d_t b, double percent) {
  v3d_t ret;

  ret.x = a.x + ((b.x - a.x) * percent);
  ret.y = a.y + ((b.y - a.y) * percent);
  ret.z = a.z + ((b.z - a.z) * percent);

  return ret;
}

v3d_t v3d_random(double length) {
  v3d_t ret;

  ret.x = r_num(-10.0, 10.0);
  ret.y = r_num(-10.0, 10.0);
  ret.z = r_num(-10.0, 10.0);

  ret = v3d_normalize(ret);

  return v3d_scale(length, ret);
}



v3d_t v3d_getLookVector(double facing, double incline) {
  v3d_t lookVector;

  lookVector.x = cos(incline);
  lookVector.y = sin(incline);
  lookVector.z = 0.0;

  return v3d_rotateY(lookVector, -facing);
}


v3d_t v3d_rotateX(v3d_t a, double angle) {
  v3d_t r;

  double s = sin(angle);
  double c = cos(angle);

  r.x = a.x;
  r.y = (a.y * c) - (a.z * s);
  r.z = (a.y * s) + (a.z * c);

  return r;
}

v3d_t v3d_rotateY(v3d_t a, double angle) {
  v3d_t r;

  double s = sin(angle);
  double c = cos(angle);

  r.x = (a.z * s) + (a.x * c);
  r.y = a.y;
  r.z = (a.z * c) - (a.x * s);

  return r;
}

v3d_t v3d_rotateZ(v3d_t a, double angle) {
  v3d_t r;

  double s = sin(angle);
  double c = cos(angle);

  r.x = (a.x * c) - (a.y * s);
  r.y = (a.x * s) + (a.y * c);
  r.z = a.z;

  return r;
}


// INTEGER (v3di_t) * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


void v3di_print(const char *str, v3di_t a) {
  printf("%s <%d, %d, %d>\n", str, a.x, a.y, a.z);
}



// return 1 if equal, 0 otherwise
int v3di_isequal (v3di_t a, v3di_t b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z) return 1;
  
  return 0;
}


// return a v3di_t for x, y, z
v3di_t v3di_v (int x, int y, int z) {
  v3di_t v;
  
  v.x = x;
  v.y = y;
  v.z = z;
  
  return v;
}


// return a v3di_t for a vector [ x  y  z ]
v3di_t v3di_v (int *vector) {
  v3di_t v;
  
  v.x = vector[0];
  v.y = vector[1];
  v.z = vector[2];
  
  return v;
}


// constructs a v3di_t from a v3d_t
v3di_t v3di_v(v3d_t a) {
  v3di_t v;
  
  v.x = (int)floor(a.x);
  v.y = (int)floor(a.y);
  v.z = (int)floor(a.z);
  
  return v;
}


// returns a + b
v3di_t v3di_add (v3di_t a, v3di_t b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;

  return a;
}


// returns a - b
v3di_t v3di_sub (v3di_t a, v3di_t b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;

  return a;
}


// returns a(b)
v3di_t v3di_scale (int a, v3di_t b) {
  b.x *= a;
  b.y *= a;
  b.z *= a;

  return b;
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * math_util.h
// *
// * math library
// *
// * by john beck
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef MathUtil_h_
#define MathUtil_h_

#include <cstdlib>
#include <cmath>

// macros * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define MY_PI			(3.14159265358979323846)
#define MY_2PI			(6.28318530717958647693)
#define DEG_2_RAD		(0.01745329251994329577)
#define RAD_2_DEG		(57.2957795130823208768)

#define DEG2RAD(x)		((x)*DEG_2_RAD)
#define RAD2DEG(x)		((x)*RAD_2_DEG)


// prototypes * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


// returns a double in [low, high) // FIXME: hopefully [low
inline double r_num(double low, double high) {
  return (((double)rand() / (double)(RAND_MAX + 1) * (high - low)) + low);
}



// returns an int in [low, high)
inline int r_numi(int low, int high) {
  return static_cast<int>(floor((((double)rand() / (double)(RAND_MAX + 1)) * (high - low)) + low));
}



// gets within [num - delta, num + delta)
inline double get_close(double num, double delta) {
  return r_num(num - delta, num + delta);
}



// returns the sign of a number: 1 if zero or positive, -1 if negative, 0 otherwise
inline double sign(double num) {
  if (num < 0.0) return -1.0;
  else if (num > 0.0) return 1.0;

  return 0.0;
}



// returns the number that is percent% of the way from a to b
inline double lerp(double a, double b, double percent) {
  return (percent * (b - a)) + a;
}



// returns the number that is %ratio of the way from a to b
inline double lerp(double a, double b, int numerator, int denominator) {
  double percent = static_cast<double>(numerator) / static_cast<double>(denominator);
  return (percent * (b - a)) + a;
}


inline double clamp(double num, double min, double max) {
  if (num < min) { return min; }
  if (num > max) { return max; }

  return num;
}


inline double avg(double a, double b) {
  return 0.5 * (a + b);
}

inline double absConstrain(double value, double limit) {
  if (value < -limit) { return -limit; }
  if (value > limit) { return limit; }
  return value;
}


//double r_num (double low, double high);
//int r_numi (int low, int high);
//double get_close (double num, double delta);
//double sign (double num);
//double lerp (double h1, double h2, double per);
//double lerp (double a, double b, int numerator, int denominator);
//double constrain_num (double num, double min, double max);
unsigned long hash (const char *str);

#endif // MathUtil_h_

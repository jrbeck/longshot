// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * v2d.h
// *
// * 2d vector math library
// *
// * by john beck
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cstdio>
#include <cmath>

// typedefs * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
union v2d_t {
  struct {
    double x;
    double y;
  };
  double vec[2];
};

union v2di_t {
  struct {
    int x;
    int y;
  };
  int vec[2];
};

// prototypes * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void v2d_print(const char* str, v2d_t a);
void v2di_print(const char* str, v2di_t a);
int v2d_isequal(v2d_t a, v2d_t b);
v2d_t v2d_zero();
void v2d_zero(v2d_t* a);
v2d_t v2d_unit(double theta);
v2d_t v2d_v(double x, double y);
v2di_t v2di_v(int x, int y);
double v2d_dist(v2d_t a, v2d_t b);
double v2d_relative_dist(v2d_t a, v2d_t b);
double v2d_mag(v2d_t a);
v2d_t v2d_normalize(v2d_t a);
v2d_t v2d_scale(v2d_t a, double scalar);
v2d_t v2d_scale(double scalar, v2d_t a);
v2d_t v2d_add(v2d_t a, v2d_t b);
v2di_t v2di_add(v2di_t a, v2di_t b);
v2d_t v2d_sub(v2d_t a, v2d_t b);
double v2d_dot(v2d_t a, v2d_t b);
double v2d_cross(v2d_t a, v2d_t b);
v2d_t v2d_rot(v2d_t a, double theta);
v2d_t v2d_neg(v2d_t a);
v2d_t v2d_normal(v2d_t a);
double v2d_theta(v2d_t a);
v2d_t v2d_project(v2d_t a, v2d_t b);
v2d_t v2d_interpolate(v2d_t a, v2d_t b, double percent);

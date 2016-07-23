// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Simplex
// *
// * This adapted from:
// * Simplex Noise Demystified
// * (Stefan Gustavson 2005)
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <cmath>

double dot(double x, double y, double z, double* g);
double noise(double xin, double yin, double zin);
double simplex_noise(int octaves, double x, double y, double z);

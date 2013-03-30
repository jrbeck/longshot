// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * v3d.h
// *
// * 3d vector math library
// *
// * by john beck
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef v3d_h_
#define v3d_h_

#include <cstdio>
#include <cmath>

#include "MathUtil.h"


// typedefs * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
typedef struct {
	union {
		struct {
			double x;
			double y;
			double z;
		};
		double vec[3];
	};
} v3d_t;



typedef struct {
	union {
		struct {
			int x;
			int y;
			int z;
		};
		int vec[3];
	};
} v3di_t;



// prototypes * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void v3d_print (const char *str, v3d_t a);
int v3d_isequal (v3d_t a, v3d_t b);
v3d_t v3d_zero (void);
void v3d_zero (v3d_t *a);
v3d_t v3d_v (double x, double y, double z);
v3d_t v3d_v (v3di_t v3di);
double v3d_mag (const v3d_t &a);
double v3d_dist (v3d_t a, v3d_t b);
v3d_t v3d_normalize (v3d_t a);
v3d_t v3d_scale (v3d_t a, double scalar);
v3d_t v3d_scale (double scalar, v3d_t a);
v3d_t v3d_add (v3d_t a, v3d_t b);
v3d_t v3d_sub (v3d_t a, v3d_t b);
double v3d_dot (v3d_t a, v3d_t b);
v3d_t v3d_cross (v3d_t a, v3d_t b);
v3d_t v3d_neg (v3d_t a);
v3d_t v3d_project (v3d_t a, v3d_t b);
v3d_t v3d_interpolate (v3d_t a, v3d_t b, double percent);
v3d_t v3d_random (double length);
v3d_t v3d_getLookVector (double facing, double incline);

// INTEGER (v3di_t) * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void v3di_print (const char *str, v3di_t a);
int v3di_isequal (v3di_t a, v3di_t b);
v3di_t v3di_v (int x, int y, int z);
v3di_t v3di_v (int *vector);
v3di_t v3di_v (v3d_t a);
v3di_t v3di_add (v3di_t a, v3di_t b);
v3di_t v3di_sub (v3di_t a, v3di_t b);
v3di_t v3di_scale (int a, v3di_t b);



#endif // v3d_h_
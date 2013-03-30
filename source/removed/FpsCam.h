// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * fps_cam_c
// *
// * this handles various viewport setup for an FPS style camera
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef FpsCam_h_
#define FpsCam_h_

#include <cmath>


#include "GL/glut.h"

#include "math_util.h"
#include "v2d.h"
#include "v3d.h"



class fps_cam_c {
	v3d_t pos;			// where the 'feet' are
	v3d_t target;		// where the camera is pointed
	v3d_t up;			// the 'up' vector for the camera

	double height;

	double facing;		// the angle on the x-z plane off the positive x-axis

	double elevation;		// the angle on the vertical plane off the x-z plane intersection...
	double elevation_min;	// upper constraint for elevation angle
	double elevation_max;	// lower constraint for elevation angle

public:
	fps_cam_c ();
	~fps_cam_c ();

	// set with target, facing, elevation, and distance
	int setup_cam (v3d_t t, double f, double e, double d);

	// sets the height (y) for pos
	int set_foot_height (double h);

	// update the camera target
	int update_target (void);

	// make sure everything is legal
	int constrain (void);

	// pan view around view globe
	int pan (v2d_t a);

	// translate the camera and target position
	int translate (v2d_t a);

	// change height relative to its current value
	int change_height_rel (double delta);

	// change the camera position
	int set_cam (v3d_t a);

	// change the target position
	int set_target (v3d_t a);

	// set the near and far values for the camera
	int set_near_and_far (double n, double f);

	// use the gluLookAt () to set view at render time
	int look_at (frustum_c &f);

	// return the pos vector
	v3d_t get_pos (void);
};


#endif // FpsCam_h_

#include "FpsCam.h"


fps_cam_c::fps_cam_c () {
	// set the 'feet' the be the origin
	pos = v3d_v (0.0, 0.0, 0.0);
	height = 1.9;

	facing = 0.0;

	elevation = DEG2RAD (0.0);
	elevation_min = DEG2RAD (-80.0);
	elevation_max = DEG2RAD (80.0);

	// figure out where we're looking
	update_target ();

	// set up to be straight up
	up = v3d_v (0, 1, 0);
}


fps_cam_c::~fps_cam_c () {
}


// set with target, facing, elevation, and distance
int fps_cam_c::setup_cam (v3d_t t, double f, double e, double d) {
	return 0;
}


int fps_cam_c::set_foot_height (double h) {
	pos.y = h;

	update_target ();

	return 1;
}



// constrain the camera
int fps_cam_c::constrain (void) {
	// constrain the view angle elevation
	if (elevation < elevation_min) elevation = elevation_min;
	else if (elevation > elevation_max) elevation = elevation_max;

	// this is ugly, but really shouldn't be a problem ever
	while (facing > MY_2PI) facing -= MY_2PI;
	while (facing < 0) facing += MY_2PI;

	return 0;
}


// update the camera target
int fps_cam_c::update_target (void) {
	target.x = cos (elevation);
	target.y = sin (elevation);
	target.z = 0.0;

	// now rotate about the y-axis
	v3d_t temp = target;

	target.x = (temp.x * cos (facing)) - (temp.z * sin (facing));
	target.z = (temp.x * sin (facing)) + (temp.z * cos (facing));

	v3d_t head_pos = pos;

	head_pos.y += height;

	// now shift to the position
	target = v3d_add (head_pos, target);

	return 0;
}


// pan camera around view globe
int fps_cam_c::pan (v2d_t a) {
	// update these values
	facing += a.x;
	elevation += a.y;

	// now make sure the new values are in range
	constrain ();

	// since things changed
	update_target ();

	return 0;
}


// translate the camera and target position
int fps_cam_c::translate (v2d_t a) {
	// get a vector pointing the way the camera is facing
	v2d_t t1 = v2d_v (cos (facing), sin (facing));

	v2d_t t_fb; // translation forward or back
	v2d_t t_lr; // translate left and right

	// this is the forward-backward translation
	t_fb = v2d_scale (t1, a.y);

	// this is the side-side translation
	t_lr = v2d_scale (v2d_normal (t1), a.x);

	// update the position
	pos.x += t_fb.x + t_lr.x;
	pos.z += t_fb.y + t_lr.y;

	update_target ();

	return 0;
}

// change height relative to its current value
int fps_cam_c::change_height_rel (double delta) {
	height += delta;

	update_target ();

	return 0;
}



// change the camera position
int fps_cam_c::set_cam (v3d_t a) {
	return 0;
}


// change the target position
int fps_cam_c::set_target (v3d_t a) {
	return 0;
}


// set the near and far values for the camera
int fps_cam_c::set_near_and_far (double n, double f) {
	return 0;
}


// use the gluLookAt () to set view at render time
int fps_cam_c::look_at (frustum_c &f) {
	gluLookAt (pos.x, pos.y + height, pos.z,
				target.x, target.y, target.z,
				up.x, up.y, up.z);

	f.look_at (v3d_v (pos.x, pos.y + height, pos.z), target, up);

	return 0;
}


// return the pos vector
v3d_t fps_cam_c::get_pos (void) {
	return pos;
}



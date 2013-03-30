// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * OrbitSky
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef OrbitSky_h_
#define OrbitSky_h_

#include <vector>

#include "Galaxy.h"
#include "SdlInterface.h"
#include "AssetManager.h"
#include "GlCamera.h"


typedef struct {
	v3d_t pos;
	GLfloat color[4];
	GLfloat size;
} celestial_body;



class OrbitSky {
public:
	OrbitSky();
	~OrbitSky();

	void setOrbit(Galaxy &galaxy, size_t planetHandle);

	void draw(gl_camera_c &cam, v3d_t playerPosition);

	vector<celestial_body> mBodies;
};


#endif // OrbitSky_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Planet
// *
// * describes a planet
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Planet_h_
#define Planet_h_

#include <cmath>

#include "v2d.h"
#include "SdlInterface.h"

using namespace std;


class Planet {
public:
	Planet();
	~Planet();

	void save(FILE *file);
	void load(FILE *file);

// MEMBERS * * * * * * * * * * *
	// WARNING: this is being saved/loaded with an fwrite()
	// i.e., data must be simple, no vectors down here
	int mHandle;
	bool mHasBeenVisited;
	int mSeed;

	GLfloat mRadius;
	GLfloat mAngle;
	GLfloat mOrbitRadius;
	v2d_t mPosition;
};





#endif // Planet_h_

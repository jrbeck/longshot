// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Location
// *
// * desc: interface for game locations e.g.
// * 	planets, space stations, ships...
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Location_h_
#define Location_h_

#include "GlCamera.h"
#include "Galaxy.h"
#include "LightManager.h"



enum {
	LOCATION_DEFAULT,
	LOCATION_WORLD,
	LOCATION_SHIP
};

class Location {
public:
	virtual ~Location() { mType = LOCATION_DEFAULT; }

	virtual int initialize(FILE *file, Galaxy *galaxy, int planetHandle) = 0;
	virtual void save(FILE *file) = 0;
	virtual int update(v3d_t playerPosition) = 0;
	virtual void draw(gl_camera_c &cam) = 0;
	virtual int getType() { return mType; }
	WorldMap *getWorldMap() { return mWorldMap; }
	LightManager *getLightManager() { return mLightManager; }
	virtual v3d_t getStartPosition() = 0;

	int mHandle;

protected:
	int mType;
	WorldMap *mWorldMap;
	LightManager *mLightManager;
};


#endif // Location_h_
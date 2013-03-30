// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PlanetMap
// *
// * this allows the user to view a planet map
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef PlanetMap_h_
#define PlanetMap_h_

#include "Menu.h"
#include "Periodics.h"
#include "Rectangle2d.h"
#include "Planet.h"


#define PLANET_MAP_SIDE			512
#define MAP_MULTIPLIER			128


class PlanetMap {
public:
	PlanetMap(void);
	~PlanetMap(void);

	void setUpOpenGl(void);

	// this method enters the interaction mode and returns the result
	bool chooseLocation(Planet &planet, v3d_t &planetPos);

	void drawMap(void);

	void buildFromPeriodics(int seed);

	// TODO: get this i/o junk outta here
	int handleInput (void);
	int handleKeystroke (void);
	int handleKeyup (void);
	void handleMouseButtonDown (int button, v2d_t pos);
	void handleMouseButtonUp (int button, v2d_t pos);

	void drawTerrain(void) const;

// MEMBERS
	SDL_Event sdlevent;
	v2d_t mMousePos;
	v2d_t mMouseDelta;
	int mMouseMoved;
	bool mLeftMouseButtonClicked;

	menu_c *mMenu;

	Terrain *mTerrain;
	v3d_t *mColors;
	Periodics *mPeriodics;

	Rectangle2d mViewRect;
};




#endif // PlanetMap_h_
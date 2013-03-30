// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GalaxyMap
// *
// *
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef GalaxyMap_h_
#define GalaxyMap_h_

#include "Galaxy.h"
#include "PlanetMap.h"

#define ZOOM_GALAXY			0
#define ZOOM_SYSTEM			1

#define ACTION_NONE			0
#define ACTION_WARP			1


typedef struct {
	int action;
	StarSystem *starSystem;
	Planet *planet;
} GalaxyMapResult;


class GalaxyMap {
public:
	GalaxyMap();
	~GalaxyMap();

	GalaxyMapResult enterViewMode(Galaxy *galaxy, Planet *selectedPlanet);

	void drawGalaxy(Planet *selectedPlanet);
	void drawStarSystem(StarSystem &starSystem, Planet *selectedPlanet);

	void updateGalaxyMenu(StarSystem *currentSystem, StarSystem *hoverSystem);
	void updateStarSystemMenu();

	void setUpOpenGl();
	void drawHex(float radius, v2d_t center, const GLfloat color[4]);
	void drawRing(float radius, v2d_t center, const GLfloat color[4]);


	Galaxy *mGalaxy;

	// TODO: get this i/o junk outta here
	int handleInput (void);
	int handleKeystroke (void);
	int handleKeyup (void);
	void handleMouseButtonDown (int button, v2d_t pos);
	void handleMouseButtonUp (int button, v2d_t pos);

	SDL_Event sdlevent;
	v2d_t mMousePos;
	v2d_t mMouseDelta;
	int mMouseMoved;
	bool mLeftMouseButtonClicked;

	menu_c *mMenu;

	int mZoomLevel;
	int mSelectedSystem;

	GalaxyMapResult mResult;
};


#endif // GalaxyMap_h_
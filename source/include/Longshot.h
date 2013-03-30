// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Longshot
// *
// * this is the root of the (...problem) program
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

// longshot constants * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

//#define SCREEN_W				(1920)
//#define SCREEN_H				(1200)
//#define FULLSCREEN_MODE			(true)


//#define SCREEN_W				(1000)
//#define SCREEN_H				(700)
//#define FULLSCREEN_MODE			(false)


//#define SCREEN_W				(1366)
//#define SCREEN_H				(768)
//#define FULLSCREEN_MODE		(true)

#define SCREEN_ASPECT_RATIO		((double)SCREEN_W / (double)SCREEN_H)

//#define DEFAULT_SIDE_UNIV		(256)		// default side length for octree/terrain


// the longshot/include directory
#include "SdlInterface.h"

#include "game.h"	// game mode
#include "edit.h"	// editor
#include "menu.h"	// menu / interface entry
#include "rogueviewer.h"

#include "PseudoRandom.h"
#include "GalaxyMap.h"
#include "PlanetMap.h"

#include "World.h"


//#include "Noise3d.h"




// Longshot * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class Longshot {
public:
	Longshot (void);
	~Longshot (void);

	void reloadMenu (void);

	int loop (void);

private:
	enum {
		PROGRAM_MODE_EDIT,
		PROGRAM_MODE_MENU,
		PROGRAM_MODE_NEW_GAME,
		PROGRAM_MODE_LOAD_GAME,
		PROGRAM_MODE_TOGGLE_FULLSCREEN,
		PROGRAM_MODE_ROGUE,
		NUM_PROGRAM_MODES,
		PROGRAM_QUIT
	};

	enum {
		MENU_ENTER_GAME_MODE,
		MENU_ENTER_EDIT_MODE,
		MENU_QUIT
	};

	menu_c *mMainMenu;
	game_c *mGame;
	edit_c edit;
	RogueViewer mRogueViewer;

	int program_mode;

	sdl_mode_info_t windowedMode;

	bool constructor_successful;
};


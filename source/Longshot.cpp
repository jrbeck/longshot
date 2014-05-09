#include "Longshot.h"


Longshot::Longshot (void) : 
	mMainMenu(NULL),
	mGame(NULL)
{
//	printf ("%6d: Longshot constructor ----------------\n", SDL_GetTicks ());
	printf ("Longshot constructor ----------------\n");

	// lets hope this stays this way
	constructor_successful = true;

	// set up the graphics
	if (initSdl ()) {
		constructor_successful = false;
		return;
	}

	SCREEN_W = windowedMode.screen_w = //640;
		static_cast<int>(static_cast<double>(desktopMode.screen_w) * 0.9);
	SCREEN_H = windowedMode.screen_h = //480;
		static_cast<int>(static_cast<double>(desktopMode.screen_h) * 0.9);
	windowedMode.fullscreen = false;

	if (setVideoMode(windowedMode)) {
		constructor_successful = false;
		return;
	}
	
	// set the window stuff
	SDL_WM_SetCaption( "Longshot", NULL );
	SDL_WM_SetIcon( IMG_Load( "art/resource/longshot_icon.png" ), NULL );
	
	// show the mouse cursor
	SDL_ShowCursor (1);

	// MENU_C * * * * * * * * * * * * * * * * * * * * * * * *
	reloadMenu ();

	// start off in the menu
	program_mode = PROGRAM_MODE_MENU;

	// create the save directory if it doesn't exist
	// WARNING: windows specific
	CreateDirectory(TEXT("save"), NULL);

	printf ("%6d: exiting Longshot constructor\n", SDL_GetTicks ());
	printf ("-----------------------------------\n\n");
}


Longshot::~Longshot (void) {
	printf ("%6d: Longshot destructor\n", SDL_GetTicks ());
	printf ("---------------------------------------------------------\n");

	if (mMainMenu != NULL) {
		delete mMainMenu;
		mMainMenu = NULL;
	}


	// this should never happen
	if (mGame != NULL) {
		printf("Longshot::~Longshot(): error: mGame not NULL\n");
		delete mGame;
		mGame = NULL;
	}

	if (gDefaultFontTextureHandle != 0) {
		glDeleteTextures(1, &gDefaultFontTextureHandle);
	}

	// quit SDL
	deinitSdl ();

	printf ("\n");
	printf ("---------------------------------------------------------\n");
	printf ("program execution time: %fs\n", (double)SDL_GetTicks () / 1000.0);
}



void Longshot::reloadMenu(void) {
	// WARNING: this is where the default font is being loaded
	// it has to be done when there is a context change (e.g.
	// windowed mode->fullscreen), so it is here...
	if (gDefaultFontTextureHandle != 0) {
		glDeleteTextures(1, &gDefaultFontTextureHandle);
	}
	if (!loadTexture("art/fonts/font3.png", &gDefaultFontTextureHandle) == 0) {
		printf("Longshot::reloadMenu(): failed to load font\n");
	}
	menu_c::setDefaultTextureHandle(gDefaultFontTextureHandle);

	// create a new menu
	if (mMainMenu != NULL) {
		delete mMainMenu;
		mMainMenu = NULL;
	}
	mMainMenu = new menu_c();

	GLfloat color[4] = { 0.55f, 0.075f, 0.075f, 1.0f };
	GLfloat bgColor[4] = { 0.2f, 0.5f, 0.5f, 1.0f };
	v2d_t fontSize = { 0.03f, 0.06f };

	mMainMenu->addText (v2d_v (0.6, 0.0), v2d_v (0.2, 0.2), fontSize,
		"longshot", TEXT_JUSTIFICATION_RIGHT, color, NULL);

//	GLfloat color2[4] = { 0.2f, 0.0f, 0.0f, 1.0f };
	GLfloat color2[4] = { 0.8f, 0.4f, 0.0f, 1.0f };
	GLfloat bgColor2[4] = { 0.55f, 0.075f, 0.075f, 1.0f };


	fontSize = v2d_v (0.03f, 0.06f);

	mMainMenu->addButton (v2d_v (0.5, 0.35), v2d_v (0.3, 0.1), fontSize,
		"new game", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_NEW_GAME, color2, bgColor2);
	mMainMenu->addButton (v2d_v (0.4, 0.5), v2d_v (0.4, 0.1), fontSize,
		"load game", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_LOAD_GAME, color2, bgColor2);
//	mMainMenu->addButton (v2d_v (0.3, 0.5), v2d_v (0.5, 0.1), fontSize,
//		"rogue viz", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_ROGUE, color2, bgColor2);
	mMainMenu->addButton (v2d_v (0.4, 0.65), v2d_v (0.4, 0.1), fontSize,
		"fullscreen", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_TOGGLE_FULLSCREEN, color2, bgColor2);
	mMainMenu->addButton (v2d_v (0.6, 0.8), v2d_v (0.2, 0.1), fontSize,
		"quit", TEXT_JUSTIFICATION_RIGHT, PROGRAM_QUIT, color2, bgColor2);
}



// main program loop * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int Longshot::loop (void) {



	// this is where i test stupid things! * * * * * * * * * * * * * * * * * * * * *

	Galaxy *galaxy = new Galaxy();
	GalaxyMap *galaxyMap = new GalaxyMap();

/*
	FILE *test;
	test = fopen("save/test.dat", "rb");
	if (test != NULL) {
		galaxy->load(test);
		fclose(test);
	}
*/

//	galaxyMap->enterViewMode(galaxy);


//	char fileName[25] = "save/savedata.dat";

/*
	FILE *test;
	test = fopen("save/test.dat", "wb");
	if (test != NULL) {
		galaxy->save(test);
		fclose(test);
	}
*/

	delete galaxy;
	delete galaxyMap;

	// PLANET MAP TESTING!
//	PlanetMap *map = new PlanetMap();
//	map->chooseLocation();
//	delete map;


//	printf ("\n\n");

	// glad i got that outta my system! * * * * * * * * * * * * * * * * * * * * *


	printf ("block_t size: %d\n", sizeof (block_t));
	printf ("PhysicsEntity size: %d\n", sizeof (PhysicsEntity));


	// bail immediately if constructor failed at some point
	if (constructor_successful == false) {
		return -1;
	}

	while (program_mode != PROGRAM_QUIT && program_mode != MENU_APPLICATION_QUIT) {
		glClearColor (0.1f, 0.0f, 0.1f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mMainMenu->draw ();
		SDL_GL_SwapBuffers ();

		// figure out what the user wants to do
		program_mode = mMainMenu->menu_choice (true);

		// and do it
		switch (program_mode) {
			case PROGRAM_MODE_NEW_GAME:
				printf ("menu choice: new game\n");
				mGame = new game_c();
				mGame->enter_game_mode(true);
				break;
			case PROGRAM_MODE_LOAD_GAME:
				printf ("menu choice: load game\n");
				mGame = new game_c();
				mGame->enter_game_mode(false);
				break;

			case PROGRAM_MODE_TOGGLE_FULLSCREEN:
				printf ("menu choice: toggle fullscreen\n");

				if (!currentMode.fullscreen) {
					setVideoMode(desktopMode);
					SCREEN_W = desktopMode.screen_w;
					SCREEN_H = desktopMode.screen_h;
				}
				else {
					setVideoMode(windowedMode);
					SCREEN_W = windowedMode.screen_w;
					SCREEN_H = windowedMode.screen_h;
				}

				glViewport(0, 0, SCREEN_W, SCREEN_H);
				printf ("screen resolution: %d, %d\n", currentMode.screen_w, currentMode.screen_h);

				// since the context has changed, ogl assets must be reloaded
				reloadMenu();
				break;

			case PROGRAM_MODE_ROGUE:
				printf ("menu choice: rogue_viz\n");
				mRogueViewer.start ();
				break;

			case MENU_APPLICATION_QUIT:
			case PROGRAM_QUIT:
				printf ("menu choice: quit\n");
				break;

			default:
				break;
		}

		if (mGame != NULL) {
			delete mGame;
			mGame = NULL;
		}
	}

	return 0;
}


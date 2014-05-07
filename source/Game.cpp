#include "Game.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)



game_c::game_c (void) :
	mGalaxy(NULL),
	mLocation(NULL),
	mAiView(NULL),
	mPhysicsView(NULL),
	mCurrentPlanet(NULL),
	mMenu(NULL),
	mMerchantView(NULL)
{
	printf ("game_c constructor ----------------\n");
	mLastUpdateTime = 0.0;

	loadAssets ();
	printf ("\n%6d: assets loaded\n", SDL_GetTicks ());
}



game_c::~game_c (void) {
	if (mGalaxy != NULL) {
		delete mGalaxy;
	}
	if (mLocation != NULL) {
		delete mLocation;
	}
	if (mAiView != NULL) {
		delete mAiView;
	}
	if (mPhysicsView != NULL) {
		delete mPhysicsView;
	}
	if (mMenu != NULL) {
		delete mMenu;
	}
	if (mMerchantView != NULL) {
		delete mMerchantView;
	}

	freeAssets();
}



void game_c::loadAssets (void) {
	mAssetManager.loadAssets ();
	mItemManager.loadAssets ();

}


void game_c::freeAssets (void) {
	mAssetManager.freeAssets ();
	mItemManager.freeAssets ();
}



void game_c::loadPlanetMenu(void) {
	if (mMenu != NULL) {
		delete mMenu;
		mMenu = NULL;
	}
	mMenu = new menu_c();
//	mMenu->setFont(gDefaultFontTextureHandle);

	GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	v2d_t fontSize = { 0.015f, 0.03f };

	mMenu->addButton (v2d_v (0.80, 0.15), v2d_v (0.2, 0.1), fontSize, "back to game",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
	mMenu->addButton (v2d_v (0.80, 0.3), v2d_v (0.2, 0.1), fontSize, "quit to menu",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);

	
	// TEMP: these are just for testing
	mMenu->addButton (v2d_v (0.0, 0.15), v2d_v (0.2, 0.1), fontSize, "ship",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_SHIP, color, bgColor);
}


void game_c::loadShipMenu(void) {
	if (mMenu != NULL) {
		delete mMenu;
		mMenu = NULL;
	}
	mMenu = new menu_c();
//	mMenu->setFont(gDefaultFontTextureHandle);

	GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	v2d_t fontSize = { 0.015f, 0.03f };

	mMenu->addButton (v2d_v (0.80, 0.15), v2d_v (0.2, 0.1), fontSize, "back to game",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
	mMenu->addButton (v2d_v (0.80, 0.3), v2d_v (0.2, 0.1), fontSize, "quit to menu",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);


	if (mCurrentPlanet != NULL) {
		sprintf(mPlanetNameString, "orbiting planet: %d", mCurrentPlanet->mHandle);
	}
	else {
		sprintf(mPlanetNameString, "orbiting planet: NULL");
	}
	mMenu->addText(v2d_v (0.35, 0.05), v2d_v (0.3, 0.1), fontSize, mPlanetNameString,
		TEXT_JUSTIFICATION_CENTER, color, bgColor);

	

	
	// TEMP: these are just for testing
	mMenu->addButton (v2d_v (0.0, 0.15), v2d_v (0.2, 0.1), fontSize, "planet map",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_PLANET_MAP, color, bgColor);
	mMenu->addButton (v2d_v (0.0, 0.30), v2d_v (0.2, 0.1), fontSize, "galaxy map",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_GALAXY_MAP, color, bgColor);

	mMenu->addButton (v2d_v (0.0, 0.45), v2d_v (0.2, 0.1), fontSize, "merchant",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_MERCHANT, color, bgColor);

	mMenu->addButton (v2d_v (0.0, 0.60), v2d_v (0.2, 0.1), fontSize, "dungeon map",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_DUNGEON_MAP, color, bgColor);


	
}



void game_c::setup_opengl(void) {
	glViewport(0, 0, SCREEN_W, SCREEN_H);

	glEnable (GL_TEXTURE_2D);
//	glEnable (GL_BLEND);
	glShadeModel (GL_SMOOTH);
	glEnable (GL_DEPTH_TEST);
	glClearDepth (1.0f);
	glDepthFunc (GL_LEQUAL);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_CULL_FACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


//	glColorMaterial (GL_FRONT_AND_BACK, GL_EMISSION);
	glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glEnable (GL_COLOR_MATERIAL);

	// FOG
	glFogi (GL_FOG_MODE, GL_LINEAR);					// Fog Mode
//	glFogfv (GL_FOG_COLOR, color[skyColor]);			// Set Fog Color
	glFogf (GL_FOG_DENSITY, 0.05f);						// How Dense Will The Fog Be
	glHint (GL_FOG_HINT, GL_DONT_CARE);					// Fog Hint Value
	glFogf (GL_FOG_START, 100.0f);						// Fog Start Depth
	glFogf (GL_FOG_END, 200.0f);						// Fog End Depth
	glEnable (GL_FOG);									// Enables GL_FOG



	// go ahead and clear the buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers ();

	printf ("\n%6d: opengl_setup done ----------------\n", SDL_GetTicks ());
}


// it is here where the magic begins
int game_c::enter_game_mode(bool createNewWorld) {
	printf ("\n%6d: entered GAME mode ----------------\n", SDL_GetTicks ());

	mGalaxy = new Galaxy();

	if (createNewWorld) {
		printf("game_c::enter_game_mode(): new game\n");
		deleteAllFilesInFolder(TEXT("save"));

		mCurrentPlanet = mGalaxy->mStarSystems[0]->mPlanets[0];

		mLocation = new StarShip();
		initSpaceShip(true);
	}
	else {
		printf("game_c::enter_game_mode(): loading game\n");
		load();
	}


	// grab the cursor
	SDL_WM_GrabInput (SDL_GRAB_ON);
	SDL_ShowCursor (0);

	// set up the opengl situation
	setup_opengl ();

	// initialize the sound
	mAssetManager.mSoundSystem.initialize ();
	mAssetManager.mSoundSystem.loadSound ("cow");
//	mAssetManager.mSoundSystem.playSoundByHandle (SOUND_AMBIENT, 64);

	// the loop
	gameLoop();

	// destroy all the items
	// FIXME: this is temporary
	vector<size_t> itemList = mAiManager.getAllItemHandles();
	mItemManager.destroyItemList(itemList);
	itemList = mPhysics.getAllItemHandles();
	mItemManager.destroyItemList(itemList);

	// save the current game state
	save();

	// DONE WITH THIS ROUND FOLKS * * * * * * *
	mAiManager.clear();
	mItemManager.clear();
	mPhysics.clear();

	mAssetManager.mSoundSystem.stopAllSounds();

	printf ("%6d: exiting GAME mode ----------------\n\n", SDL_GetTicks ());
	printf ("world seed: %d\n", mWorldSeed);

	return 0;
}


void game_c::initializePlanet(bool resetPlayer, Planet *planet, v3d_t *startPos, bool createSetPieces) {
	FILE *file = NULL;
	mCurrentPlanet = planet;
	v3d_t playerStartPosition;

	// this is just a generic planet
	if (planet == NULL) {
		mWorldSeed = SDL_GetTicks ();
//		seed = 4450; // major bug: when MAX_CRITTERS_HACK == 15, AiEntity::updateBalloon(),
//						if allowed to modify mPhysicsEntity->force.y, game slows to ~zero FPS.
//						only observed once...

		printf ("world seed: %d\n", mWorldSeed);
	}
	else {
		// try to open the file for this planet
		char fileName[48];
		sprintf(fileName, "save/planet%d.dat", planet->mHandle);
		printf("game_c::initializePlanet(): trying to load file: %s\n", fileName);
		file = fopen(fileName, "rb");
		mWorldSeed = planet->mSeed;
	}

	if (startPos != NULL) {
		v3d_print("game_c::initializePlanet(): setting startPos, ", *startPos);
		static_cast<World*>(mLocation)->setStartPosition(*startPos);
		playerStartPosition.x = startPos->x;
		playerStartPosition.y = startPos->y;
		playerStartPosition.z = startPos->z;
	}

	mLocation->initialize(file, mGalaxy, planet->mHandle);
	if (file != NULL) {
		fclose(file);
	}


	if (startPos == NULL) {
		playerStartPosition = mLocation->getStartPosition();
	}
	else if (createSetPieces) {
		v3di_t worldIndex = getWorldRegionIndex(*startPos);
		FeatureGenerator::createSetPieces(
			worldIndex.x,
			worldIndex.z,
			*static_cast<World *>(mLocation));
	}

	// this needs to be done after the lighting info has been set
	static_cast<World *>(mLocation)->preloadColumns(NUM_PRELOADED_COLUMNS, playerStartPosition);

	// load the appropriate menu
	loadPlanetMenu();

	mCycleLighting = true;

	resetForNewLocation(playerStartPosition, resetPlayer);
}



void game_c::initSpaceShip(bool resetPlayer) {
	FILE *file = fopen("save/playership.dat", "rb");
	// this decides whose map we're gonna render/interact with

	mLocation->initialize(file, mGalaxy, mCurrentPlanet->mHandle);
	if (file != NULL) {
		fclose(file);
	}

	// make a physical entity to represent the player
	v3d_t playerStartPosition = { 12.5, 1.1, 50.0 };

	// load the appropriate menu
	loadShipMenu();

	mCycleLighting = false;

	resetForNewLocation(playerStartPosition, resetPlayer);
}




void game_c::resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer) {
	// this decides whose map we're gonna render/interact with
	mWorldMap = mLocation->getWorldMap();
	// we need to give it an ambient light color
	IntColor sunColor;
	if (mLocation->getType() == LOCATION_SHIP) {
		sunColor.r = LIGHT_LEVEL_MAX;
		sunColor.g = LIGHT_LEVEL_MAX;
		sunColor.b = LIGHT_LEVEL_MAX;
	}
	else {
		GLfloat *starColor = mGalaxy->getStarSystemByHandle(mCurrentPlanet->mHandle)->mStarColor;
		sunColor.r = (starColor[0] + 0.5f) * LIGHT_LEVEL_MAX;
		sunColor.g = (starColor[1] + 0.5f) * LIGHT_LEVEL_MAX;
		sunColor.b = (starColor[2] + 0.5f) * LIGHT_LEVEL_MAX;
		sunColor.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);
	}
	mWorldMapView.setWorldMap(mWorldMap, sunColor);


	// let's handle the items first...
	// FIXME: this is temporary
	vector<size_t> itemList = mAiManager.getAllItemHandles();
	mItemManager.destroyItemList(itemList);
	itemList = mPhysics.getAllItemHandles();
	mItemManager.destroyItemList(itemList);
	mItemManager.trimItemsList();



	// reset this to be safe
	mPhysics.reset();

	// create a new PhysicsView
	if (mPhysicsView != NULL) {
		delete mPhysicsView;
	}
	mPhysicsView = new PhysicsView();

	// make a physical entity to represent the player
	// FIXME: make sure this succeeds you knucklehead!
	mPlayerPhysicsHandle = mPhysics.createEntity(OBJTYPE_PLAYER, playerStartPosition, 0.0, false);
	printf("player physics handle: %d\n", mPlayerPhysicsHandle);

	// load the pre-generated physics items
	mPhysics.loadInactiveList();

	// AI
	mAiManager.clear();
	if (mAiView != NULL) {
		delete mAiView;
	}
	mAiView = new AiView();
	mAiView->setAiEntities(mAiManager.getEntities());
	mPlayerAiHandle = mAiManager.setPlayerPhysicsHandle(mPlayerPhysicsHandle, mPhysics, 0.0);


	// reset the player
	if (resetPlayer) {
		mPlayer.reset(mPlayerPhysicsHandle, mPlayerAiHandle, mItemManager);
	}
	mPlayer.setPhysicsHandle(mPlayerPhysicsHandle);
	mPlayer.setStartPosition(playerStartPosition);
	mPlayer.soft_reset (playerStartPosition, mPhysics);


	// FIXME: this is really a hack...
	if (mLocation->getType() == LOCATION_SHIP) {
		mPlayer.set_draw_distance(500.0);
		mAiManager.setMaxCritters(0);
	}
	else {
		mPlayer.set_draw_distance(r_num(50.0, 500.0));
		mAiManager.setMaxCritters(10);
	}

	// TODO: handle the ItemManager:
	// some items are persistent (i.e. what the player and his posse
	// are holding onto), the resdt should probably be destroyed

}


void game_c::gameLoop(void) {
	printf("game_c::gameLoop() - begin\n");

	unsigned int ticks = SDL_GetTicks();
	int frame = 0;
	int blocks_drawn = 0;

	mLastUpdateTime = (static_cast<double>(ticks) / 1000.0);

	mGameState = GAMESTATE_PLAY;

	int quit = 0;
	bool escapePressed = false;

	// the main loop
	while (quit == 0) {
		// advance the state of the game model
		escapePressed = update ();

		// draw after updates are all completed
		draw (mLastUpdateTime);

		// take care of the input/menu
		if (mGameState == GAMESTATE_PLAY) {
			mGameInput.update ();

			if (escapePressed) {
				quit = 1;
			}
		}
		else if (mGameState == GAMESTATE_MENU) {
			int menuChoice = mMenu->menu_choice (false);
			if (escapePressed) {
				printf("escape\n");
				menuChoice = GAMEMENU_BACKTOGAME;
			}
			quit = handleMenuChoice(menuChoice);
		}
		else if (mGameState == GAMESTATE_MERCHANT) {
			if (mMerchantView->update(mPlayer, mItemManager) != 0) {
				delete mMerchantView;
				mMerchantView = NULL;
				mGameState = GAMESTATE_MENU;
			}
		}

		// player hit 'esc' in play mode, switch to menu
		if (quit == 1) {
			mGameState = GAMESTATE_MENU;
			SDL_WM_GrabInput (SDL_GRAB_OFF);
			SDL_ShowCursor(1);
			quit = 0;
		}

		// update the world
		// FIXME: this should be done in update ()
		// being done here ties it to the framerate
		mLocation->update(mPhysics.getCenter (mPlayerPhysicsHandle));

		// HACK * * * * * * *
		mPlayer.placeLight(*mLocation->getLightManager(), *mWorldMap, mGameInput);

		mWorldMapView.update(mAssetManager, *mLocation->getLightManager(), mCycleLighting);

		if (mGameInput.isToggleWorldChunkBoxes()) {
			mWorldMapView.toggleShowWorldChunkBoxes();
		}


		// do some frames per second calculating
		unsigned int delta_t = SDL_GetTicks () - ticks;
		frame++;

		if (delta_t >= 5000 && OUTPUT_FRAME_STATS) {
			printf ("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
			printf ("avg frame: %10.3fms\n", (double)delta_t / (double)frame);
			frame = 0;
			blocks_drawn = 0;
			ticks = SDL_GetTicks ();

			printf ("num phys: %d\n", mNumPhysicsObjects);
			printf ("num ai: %d\n", mNumAiObjects);
			printf ("num items: %d\n\n", mNumItems);

		}

		// rescue single processor machines
//		SDL_Delay(1);
	}

	printf("game_c::gameLoop() - done\n");
}



int game_c::handleMenuChoice(int menuChoice) {
	GalaxyMap *galaxyMap;
	PlanetMap *planetMap;
	RogueMapViewer *rogueMapViewer;
	v3d_t planetPos;

	switch (menuChoice) {
	case GAMEMENU_BACKTOGAME:
		break;

	case GAMEMENU_EXITGAME:
		return 2;

	case GAMEMENU_SHIP:
		// ignore if we're already on the ship
		if (mLocation->getType() != LOCATION_SHIP) {
			saveLocation();
			delete mLocation;

			mLocation = new StarShip();
			initSpaceShip(false);
		}
		break;

	case GAMEMENU_GALAXY_MAP:
		galaxyMap = new GalaxyMap();
		galaxyMap->enterViewMode(mGalaxy, mCurrentPlanet);

		if (galaxyMap->mResult.action == ACTION_WARP) {
			mCurrentPlanet = galaxyMap->mResult.planet;
			printf("-----------\nwarp to world: %d\n", mCurrentPlanet->mHandle);

			// need to reload this to reflect the change in orbit
			loadShipMenu();

			// change our orbit location
			static_cast<StarShip *>(mLocation)->mOrbitSky->setOrbit(*mGalaxy, mCurrentPlanet->mHandle);
		}
		delete galaxyMap;

		break;

	case GAMEMENU_PLANET_MAP:
		// make a planet map for the player to choose a location
		planetMap = new PlanetMap();

		if (planetMap->chooseLocation(*mCurrentPlanet, planetPos)) {
			saveLocation();
			delete mLocation;

			// now deal with the new planet
			mLocation = new World();
			initializePlanet(false, mCurrentPlanet, &planetPos, true);

			// HACK - need better timekeeping
			mLastUpdateTime = (static_cast<double>(SDL_GetTicks()) / 1000.0);
		}
		delete planetMap;


		break;
	case GAMEMENU_DUNGEON_MAP:
		rogueMapViewer = new RogueMapViewer();
		rogueMapViewer->startViewer();
		delete rogueMapViewer;
		break;


	case GAMEMENU_MERCHANT:
		mMerchantView = new MerchantView();
		mMerchantView->engageMerchant(mPlayer, mItemManager);
		mGameState = GAMESTATE_MERCHANT;
		return 0;

	default:
		return 0;
	}

	mGameState = GAMESTATE_PLAY;
	SDL_WM_GrabInput (SDL_GRAB_ON);
	SDL_ShowCursor (0);

	// FIXME: do something to make the GI->elevation and facing delta = 0
	// i don't think this works
	SDL_Event sdlEvent;
	SDL_PollEvent (&sdlEvent);

	return 0;
}


// update the game model
bool game_c::update (void) {
	double cur_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
	bool escapePressed = false;

	while (mLastUpdateTime < cur_time) {

		// update the physics objects
		mNumPhysicsObjects = mPhysics.update (mLastUpdateTime, *mWorldMap, mAssetManager);

		// apply the player physics/update with input
		escapePressed = mPlayer.update (mLastUpdateTime, *mWorldMap,
			mPhysics, mGameInput, mAssetManager, mItemManager) || escapePressed;
		// FIXME: this is done like this so that the player
		// can catch an escape press (i.e. to get out of the
		// character sheet). this should be done differently
		if (escapePressed) {
			return escapePressed;
		}

		mAiManager.setPlayerFacingAndIncline (mPlayer.getFacingAndIncline());

		mNumAiObjects = mAiManager.update (mLastUpdateTime, *mWorldMap, mPhysics, mItemManager);

		mNumItems = mItemManager.update (mPhysics);

		mLastUpdateTime += PHYSICS_TIME_GRANULARITY;
	}

	// this just updates colors/transparencies...
	// i.e. it doesn't have to be done with the PHYSICS_TIME_GRANULARITY
	// updates
	mPhysicsView->update(mPhysics.getEntityVector(), mLastUpdateTime);

	return escapePressed;
}


// draw the game world
int game_c::draw (double &time) {
	// clear the buffer before drawing to it
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the camera from the player's perspective
	gl_camera_c cam = mPlayer.gl_cam_setup ();

	// we need this for the billboard sprites
	mPhysicsView->setViewPosition(cam.getPosition());

	// draw the stars and stuff
	mLocation->draw(cam);

	// draw the solid blocks of the WorldMap
	mWorldMapView.drawSolidBlocks(cam, mAssetManager);

	// draw the solid physics objs
	mPhysicsView->drawSolidEntities(mPhysics.getEntityVector(), *mWorldMap, mAssetManager);

	drawPlayerTargetBlock();

	mPlayer.drawEquipped(mItemManager, *mAssetManager.mGunBitmapModel);

	// draw the AI
	mAiView->draw(*mWorldMap, mItemManager, *mLocation->getLightManager());

	// draw the transparent physics objs
	bool playerHeadInWater = mPlayer.isHeadInWater ();
	mPhysicsView->drawTransparentEntities(mPhysics.getEntityVector(), mAssetManager, !playerHeadInWater);

	// draw the liquid/translucent blocks of the WorldMap
	mWorldMapView.drawLiquidBlocks(cam, mAssetManager);

	// draw the transparent physics objs
	mPhysicsView->drawTransparentEntities(mPhysics.getEntityVector(), mAssetManager, playerHeadInWater);

	// draw the hud
	mPlayer.drawHud();

	if (mGameState == GAMESTATE_MENU) {
		mMenu->draw();
	}
	if (mGameState == GAMESTATE_MERCHANT) {
		mMerchantView->draw();
	}

	SDL_GL_SwapBuffers ();

	// don't wanna just leave that sitting there
//	glFlush ();

	return 0; //blocks_drawn;
}



void game_c::drawPlayerTargetBlock (void) {
	int targetFace;

	v3di_t *playerTarg = mPlayer.getTargetBlock(targetFace);

	if (playerTarg == NULL) {
		return;
	}

	v3d_t nearCorner = v3d_v (*playerTarg);
	v3d_t farCorner = v3d_add (nearCorner, v3d_v (1.01, 1.01, 1.01));
	nearCorner = v3d_add (nearCorner, v3d_v (-0.01, -0.01, -0.01));

/*
	v3d_t nearCorner2 = v3d_v (*playerTarg);
	GLfloat faceColor[4] = { 0.5, 0.0, 0.0, 0.5 };

	// draw the targeted face
	
	glPushMatrix ();
		glTranslated (nearCorner2.x, nearCorner2.y, nearCorner2.z);

		glBegin (GL_QUADS);
			mAssetManager.drawBlockFace (targetFace, faceColor);
		glEnd ();
	glPopMatrix ();
*/
		
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, 0);

	glColor4f (0.3f, 0.3f, 0.3f, 1.0f);

	glBegin (GL_LINES);
		glVertex3d (nearCorner.x, nearCorner.y, nearCorner.z);
		glVertex3d (farCorner.x, nearCorner.y, nearCorner.z);

		glVertex3d (nearCorner.x, nearCorner.y, nearCorner.z);
		glVertex3d (nearCorner.x, farCorner.y, nearCorner.z);

		glVertex3d (nearCorner.x, nearCorner.y, nearCorner.z);
		glVertex3d (nearCorner.x, nearCorner.y, farCorner.z);

		glVertex3d (farCorner.x, farCorner.y, farCorner.z);
		glVertex3d (nearCorner.x, farCorner.y, farCorner.z);

		glVertex3d (farCorner.x, farCorner.y, farCorner.z);
		glVertex3d (farCorner.x, nearCorner.y, farCorner.z);

		glVertex3d (farCorner.x, farCorner.y, farCorner.z);
		glVertex3d (farCorner.x, farCorner.y, nearCorner.z);

		glVertex3d (farCorner.x, nearCorner.y, nearCorner.z);
		glVertex3d (farCorner.x, nearCorner.y, farCorner.z);

		glVertex3d (farCorner.x, nearCorner.y, nearCorner.z);
		glVertex3d (farCorner.x, farCorner.y, nearCorner.z);

		glVertex3d (nearCorner.x, farCorner.y, nearCorner.z);
		glVertex3d (farCorner.x, farCorner.y, nearCorner.z);

		glVertex3d (nearCorner.x, farCorner.y, nearCorner.z);
		glVertex3d (nearCorner.x, farCorner.y, farCorner.z);

		glVertex3d (nearCorner.x, nearCorner.y, farCorner.z);
		glVertex3d (nearCorner.x, farCorner.y, farCorner.z);

		glVertex3d (nearCorner.x, nearCorner.y, farCorner.z);
		glVertex3d (farCorner.x, nearCorner.y, farCorner.z);
	glEnd ();
}



// WARNING: Windows specific
void game_c::deleteAllFilesInFolder (LPWSTR folderPath) {
	WIN32_FIND_DATA info;
	HANDLE hp;

	wchar_t findPath[260];

	wsprintf (findPath, TEXT ("%s\\*.*"), folderPath);

	hp = FindFirstFile (findPath, &info);

	wchar_t fullName[260];

	int numFilesDeleted = 0;

	do {
		wsprintf (fullName, TEXT ("%s\\%s"), folderPath, info.cFileName);

		if (!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			numFilesDeleted++;
//			wprintf (TEXT ("deleting: %s\n"), fullName);
			DeleteFile (fullName);
		}
	} while (FindNextFile (hp, &info));

	printf ("deleted: %d files\n", numFilesDeleted);

	FindClose (hp);
}



void game_c::save(void) {
//	player_c player;	// save
	// PLAYER * * * * * * *
	saveGameData();


	//	Galaxy *mGalaxy;	// save
	// GALAXY * * * * * *
	FILE *file;
	file = fopen("save/galaxy.dat", "wb");
	if (file != NULL) {
		mGalaxy->save(file);
		fclose(file);
	}


//	Location *mLocation;	// save
	// LOCATION * * * * * * *
	saveLocation();


//	AiManager mAiManager;	// save
	// AI STUFF * * * * * * *
	// TODO

//	physics_c mPhysics;	// save
	// PHYSICS * * * * * * *
	// TODO

//	ItemManager mItemManager;	// save
	// ITEMS * * * * * * *
	file = fopen("save/items.dat", "wb");
	if (file != NULL) {
		mItemManager.save(file);
		// WARNING: this pertains to the player...
		mPlayer.getInventory()->save(file);
		fclose(file);
	}

}


int game_c::load(void) {

//	player_c player;	// save
	// PLAYER * * * * * * *
	GameSaveData gameSaveData = loadGameData();
	if (!gameSaveData.loadSucceeded) {
		printf(" game_c::load(): UNSUCCESSFUL 1\n");
		return LOAD_UNSUCCESSFUL;
	}

//	Galaxy *mGalaxy;	// save
	// GALAXY * * * * * *
	FILE *file;
	file = fopen("save/galaxy.dat", "rb");
	if (file != NULL) {
		mGalaxy->load(file);
		fclose(file);
	}
	else {
		printf(" game_c::load(): UNSUCCESSFUL 2\n");
		return LOAD_UNSUCCESSFUL;
	}


	mCurrentPlanet = mGalaxy->getPlanetByHandle(gameSaveData.planetHandle);



//	Location *mLocation;	// save
	// LOCATION * * * * * * *
	if (mLocation != NULL) {
		delete mLocation;
		mLocation = NULL;
	}
	switch(gameSaveData.locationType) {
	case LOCATION_SHIP:
		printf("game_c::load(): loading ship\n");
		mLocation = new StarShip();
		initSpaceShip(true);
		break;
	case LOCATION_WORLD:
		printf("game_c::load(): loading planet\n");
		mLocation = new World();
		initializePlanet(true, mGalaxy->getPlanetByHandle( gameSaveData.planetHandle ), &gameSaveData.physicsPos, false);
		break;
	default:
		printf("game_c::load(): error, default location\n");
		return LOAD_UNSUCCESSFUL;
	}



//	AiManager mAiManager;	// save
	// AI STUFF * * * * * * *

//	physics_c mPhysics;	// save
	// PHYSICS * * * * * * *
	mPhysics.set_pos(mPlayerPhysicsHandle, gameSaveData.physicsPos);

//	ItemManager mItemManager;	// save
	// ITEMS * * * * * * *
	file = fopen("save/items.dat", "rb");
	if (file != NULL) {
		mItemManager.load(file);
		// WARNING: this pertains to the player...
		mPlayer.getInventory()->load(file);
		fclose(file);
	}


	return LOAD_SUCCESSFUL;
}


int game_c::saveGameData(void) {
	GameSaveData gameSaveData;
	gameSaveData.loadSucceeded = true;

	gameSaveData.physicsPos = mPhysics.getNearCorner(mPlayerPhysicsHandle);
	v3d_print("game_c::saveGameData(): saving playerPos, ", gameSaveData.physicsPos);

	gameSaveData.locationType = mLocation->getType();
	gameSaveData.planetHandle = mCurrentPlanet->mHandle;

	// now for the file stuff
	FILE *file = fopen ("save/game.dat", "wb");
	if (file == NULL) {
		return -1;
	}
	fwrite(&gameSaveData, sizeof GameSaveData, 1, file);	// physics position
	fclose (file);
	return 0;
}


GameSaveData game_c::loadGameData() {
	GameSaveData gameSaveData;
	FILE *file = fopen( "save/game.dat", "rb" );
	if (file == NULL) {
		gameSaveData.loadSucceeded = false;
		return gameSaveData;
	}
	fread( &gameSaveData, sizeof GameSaveData, 1, file );
	fclose( file );
	return gameSaveData;
}


void game_c::saveLocation() {
	// see if we need to save the current Location
	if (mLocation->getType() == LOCATION_SHIP) {
		FILE *file = fopen( "save/playership.dat", "wb" );
		mLocation->save( file );
		fclose( file );
	}
	else if (mCurrentPlanet != NULL) {
		char fileName[128];
		sprintf( fileName, "save/planet%d.dat", mCurrentPlanet->mHandle );
		FILE *file = fopen( fileName, "wb" );
		mLocation->save( file );
		fclose( file );
	}
}



#include "SpaceGame.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)



SpaceGame::SpaceGame (void) {
	printf ("SpaceGame constructor ----------------\n");
	mLastUpdateTime = 0.0;
}



SpaceGame::~SpaceGame (void) {
}

/*

void SpaceGame::loadAssets (void) {
	mAssetManager.loadAssets ();
	physics.loadAssets (mAssetManager);
	mAiManager.loadAssets (mAssetManager);
	mItemManager.loadAssets (mAssetManager);
	mWorld.loadAssets (mAssetManager);

	mMenu.loadFont ();

	GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	v2d_t fontSize = { 0.015f, 0.03f };

	mMenu.addButton (v2d_v (0.80, 0.15), v2d_v (0.2, 0.1), fontSize, "back to game",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_BACKTOGAME, color, bgColor);
	mMenu.addButton (v2d_v (0.80, 0.3), v2d_v (0.2, 0.1), fontSize, "quit to menu",
		TEXT_JUSTIFICATION_CENTER, GAMEMENU_EXITGAME, color, bgColor);
}



void SpaceGame::freeAssets (void) {
	mAssetManager.freeAssets ();
	physics.freeAssets ();
	mAiManager.freeAssets ();
	mItemManager.freeAssets ();
	mWorld.freeAssets ();

	mMenu.clear ();
}



void SpaceGame::setup_opengl (void) {
	#define NUM_COLORS_HACK		6

	GLfloat color[NUM_COLORS_HACK][4] = {
		{0.0f, 0.0f, 0.0f, 0.0f},	// black
		{0.8f, 0.55f, 0.1f, 0.0f},	// aussie yellow
		{0.3f, 0.3f, 0.3f, 0.0f},	// overcast
		{0.0f, 0.35f, 0.6f, 0.0f},	// comment blue
		{0.0f, 0.2f, 0.0f, 0.0f},	// dark green
		{0.5f, 0.2f, 0.0f, 0.0f}	// orange
	};

	int skyColor = r_numi (0, NUM_COLORS_HACK);


	// set the clear color
//	glClearColor (color[skyColor][0],
//					color[skyColor][1],
//					color[skyColor][2],
//					color[skyColor][3]);

	// set up the viewport
	glViewport (0, 0, SCREEN_W, SCREEN_H);

	// enable various things
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
	glFogfv (GL_FOG_COLOR, color[skyColor]);			// Set Fog Color
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
int SpaceGame::enter_game_mode (bool createNewWorld) {
	printf ("\n%6d: entered GAME mode ----------------\n", SDL_GetTicks ());

	loadAssets ();
	printf ("\n%6d: assets loaded\n", SDL_GetTicks ());

	initializePlanet(createNewWorld);
//	initSpaceShip();


	// grab the cursor
	SDL_WM_GrabInput (SDL_GRAB_ON);
	SDL_ShowCursor (0);

	// set up the opengl situation
	setup_opengl ();

	// initialize the sound
	mAssetManager.mSoundSystem.initialize ();
	mAssetManager.mSoundSystem.loadSound ("cow");
//	mAssetManager.mSoundSystem.playSoundByHandle (SOUND_AMBIENT, 64);

	gameLoop();

	// DONE WITH THIS ROUND FOLKS
	mAssetManager.mSoundSystem.stopAllSounds ();
	freeAssets ();

	mWorldMap->saveToDisk ();
	printf ("post save: (cols:%d)\n", mWorldMap->mInactiveColumnManager.getNumColumns ());

	savePlayerData ();

//	mWorld.clear (true);
	mAiManager.clear ();
	mItemManager.clear ();
	physics.clear ();

//	deleteAllFilesInFolder (TEXT ("save"));

	printf ("%6d: exiting GAME mode ----------------\n\n", SDL_GetTicks ());
	printf ("world seed: %d\n", mWorldSeed);



	return 0;
}


void SpaceGame::initializePlanet(bool createNewWorld) {
	// this decides whose map we're gonna render/interact with
	mWorldMap = mWorld.getWorldMap();

	int worldLoaded = 1;

	if (createNewWorld == false) {
		worldLoaded = mWorldMap->loadFromDisk ();
//		mWorldSeed = mWorld.mPeriodics.mSeed;
	}

	// if the world wasn't loaded, start a new one
	if (worldLoaded != 0) {
		mWorldSeed = SDL_GetTicks ();
//		seed = 4450; // major bug: when MAX_CRITTERS_HACK == 15, AiEntity::updateBalloon(),
//						if allowed to modify mPhysicsEntity->force.y, game slows to zero FPS

		printf ("world seed: %d\n", mWorldSeed);
		mWorld.randomizePeriodics (mWorldSeed);
	}

	mWorld.setupWorld (mAssetManager);

	// reset this to be safe
	physics.reset ();

	// make a physical entity to represent the player
	v3d_t pos = v3d_v (DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
	double time = 0.0;

	// FIXME: make sure this succeeds you knucklehead!
	mPlayerPhysicsHandle = physics.createEntity (OBJTYPE_PLAYER, pos, time, false);
	printf ("player physics handle: %d\n", mPlayerPhysicsHandle);

	// AI
	mPlayerAiIndex = mAiManager.setPlayerPhysicsHandle (mPlayerPhysicsHandle);

	// reset the player
	player.reset (mPlayerPhysicsHandle, mPlayerAiIndex, mItemManager);
	player.soft_reset (*mWorldMap, physics);

	// this moves the player to the last position
	// FIXME: change the player_c::mStartPosition
	if (worldLoaded == 0) {
		loadPlayerData ();
	}
	else {
		// set up the world features
		mWorld.loadFeaturesAroundPlayer (physics.getCenter(mPlayerPhysicsHandle));
	
	
//		mWorld.mFeatureGenerator.createForViewer(physics.getCenter(mPlayerPhysicsHandle), 0, mWorld.mWorldMap);
	
	}




	// load the pre-generated physics items
	physics.loadInactiveList ();

	// load the initial terrain
	mWorld.preloadColumns (75, physics.getCenter (mPlayerPhysicsHandle), mAssetManager);

	mWorld.generateClouds (physics.getCenter (mPlayerPhysicsHandle));
}



void SpaceGame::initSpaceShip() {
	// this decides whose map we're gonna render/interact with
	mWorldMap = mWorld.getWorldMap();

	mWorld.setupWorld (mAssetManager);

	// reset this to be safe
	physics.reset ();

	// make a physical entity to represent the player
	v3d_t pos = v3d_v (DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
	double time = 0.0;

	// FIXME: make sure this succeeds you knucklehead!
	mPlayerPhysicsHandle = physics.createEntity (OBJTYPE_PLAYER, pos, time, false);
	printf ("player physics handle: %d\n", mPlayerPhysicsHandle);

	// AI
	mPlayerAiIndex = mAiManager.setPlayerPhysicsHandle (mPlayerPhysicsHandle);

	// reset the player
	player.reset (mPlayerPhysicsHandle, mPlayerAiIndex, mItemManager);
	player.soft_reset (*mWorldMap, physics);

	// this moves the player to the last position
	// FIXME: change the player_c::mStartPosition
//	if (worldLoaded == 0) {
//		loadPlayerData ();
//	}
//	else {
		// set up the world features
//		mWorld.loadFeaturesAroundPlayer (physics.getCenter(mPlayerPhysicsHandle));
	
	
		mWorld.mFeatureGenerator.createForViewer(physics.getCenter(mPlayerPhysicsHandle), 0, *mWorldMap);
	
//	}




	// load the pre-generated physics items
//	physics.loadInactiveList ();

	// load the initial terrain
	mWorld.preloadColumns (75, physics.getCenter (mPlayerPhysicsHandle), mAssetManager);

//	mWorld.generateClouds (physics.getCenter (mPlayerPhysicsHandle));
}



void SpaceGame::gameLoop(void) {
	unsigned int ticks = SDL_GetTicks ();
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

		// take care of the input/menu
		if (mGameState == GAMESTATE_PLAY) {
			mGameInput.update ();

			if (escapePressed) {
				quit = 1;
			}
		}
		else {
			int menuChoice = mMenu.menu_choice (false);
			if (menuChoice == GAMEMENU_BACKTOGAME) {
				mGameState = GAMESTATE_PLAY;
				SDL_WM_GrabInput (SDL_GRAB_ON);
				SDL_ShowCursor (0);

				// FIXME: do something to make the GI->elevation and facing delta = 0
				// i don't think this works
				SDL_Event sdlEvent;
				SDL_PollEvent (&sdlEvent);
			}
			else if (menuChoice == GAMEMENU_EXITGAME) {
				quit = 2;
			}
		}

		if (quit == 1) {
			mGameState = GAMESTATE_MENU;
			SDL_WM_GrabInput (SDL_GRAB_OFF);
			SDL_ShowCursor (1);
			quit = 0;
		}

		// update the world
		// FIXME: this should be done in update ()
		// being done here ties it to the framerate
		mWorld.update (physics.getCenter (mPlayerPhysicsHandle), mAssetManager);


		// draw the game world
//		int num = draw (mLastUpdateTime);
		int num = 0;

//		printf ("blocks: %d\n", num);

		blocks_drawn += num;

		// do some frames per second calculating
		unsigned int delta_t = SDL_GetTicks () - ticks;
		frame++;

		if (delta_t >= 5000) {
			printf ("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
//			printf ("average blocks per frame: %10.2f\n", (double)blocks_drawn / (double)frame);
			printf ("avg frame: %10.5fms\n", (double)delta_t / (double)frame);
			frame = 0;
			blocks_drawn = 0;
			ticks = SDL_GetTicks ();

			printf ("num phys: %d\n", mNumPhysicsObjects);
			printf ("num ai: %d\n", mNumAiObjects);
			printf ("num items: %d\n\n", mNumItems);

		}
	}
}


// update the game model
bool SpaceGame::update (void) {
	double cur_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
	bool escapePressed = false;

	while (mLastUpdateTime < cur_time) {
		// update the physics objects
		mNumPhysicsObjects = physics.update (mLastUpdateTime, *mWorldMap, mAssetManager);

		// apply the player physics/update with input
		escapePressed = player.update (mLastUpdateTime, *mWorldMap,
			physics, mGameInput, mAssetManager, mItemManager) || escapePressed;

		if (escapePressed) {
			return escapePressed;
		}

		mAiManager.setFacingAndIncline (player.getFacingAndIncline ());

		mNumAiObjects = mAiManager.update (mLastUpdateTime, *mWorldMap, physics, mItemManager);

		mNumItems = mItemManager.update (physics);

		// update the world
//		mWorld.update (physics.get_pos (mPlayerPhysicsHandle), mAssetManager);

		mLastUpdateTime += PHYSICS_TIME_GRANULARITY;
	}
	
	// only draw after updates
	draw (mLastUpdateTime);

	return escapePressed;
}


// draw the game world
int SpaceGame::draw (double &time) {
	// clear the buffer before drawing to it
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the camera from the player's perspective
	gl_camera_c cam = player.gl_cam_setup ();

	// draw the world
	int blocks_drawn = mWorld.drawLand (cam, mAssetManager);

	// draw the solid physics objs
	physics.drawSolidEntities (time, *mWorldMap, mAssetManager);

	drawPlayerTargetBlock ();

	player.drawEquipped (mItemManager);

	// draw the AI
	mAiManager.draw (physics, *mWorldMap, mAssetManager, mItemManager);

	// draw the transparent physics objs
	bool playerHeadInWater = player.isHeadInWater ();
	physics.drawTransparentEntities (time, mAssetManager, !playerHeadInWater);

	mWorld.drawWater (cam, mAssetManager);

	// draw the transparent physics objs
	physics.drawTransparentEntities (time, mAssetManager, playerHeadInWater);

	// draw the hud
	player.drawHud ();

	if (mGameState == GAMESTATE_MENU) {
		mMenu.draw ();
	}

	SDL_GL_SwapBuffers ();

	// don't wanna just leave that sitting there
//	glFlush ();

	return blocks_drawn;
}



void SpaceGame::drawPlayerTargetBlock (void) {
	int targetFace;

	v3di_t *playerTarg = player.getTargetBlock (targetFace);

	if (playerTarg == NULL) {
		return;
	}

	v3d_t nearCorner = v3d_v (*playerTarg);
	v3d_t farCorner = v3d_add (nearCorner, v3d_v (1.001, 1.001, 1.001));
	nearCorner = v3d_add (nearCorner, v3d_v (-0.001, -0.001, -0.001));


	//v3d_t nearCorner2 = v3d_v (*playerTarg);
	//GLfloat faceColor[4] = { 0.5, 0.0, 0.0, 0.5 };

	//// draw the targeted face
	//
	//glPushMatrix ();
	//	glTranslated (nearCorner2.x, nearCorner2.y, nearCorner2.z);

	//	glBegin (GL_QUADS);
	//		mAssetManager.drawBlockFace (targetFace, faceColor);
	//	glEnd ();
	//glPopMatrix ();
		
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
void SpaceGame::deleteAllFilesInFolder (LPWSTR folderPath) {
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



int SpaceGame::savePlayerData (void) {
	FILE *file = fopen ("save/player.dat", "wb");
	if (file == NULL) {
		return -1;
	}

	v3d_t playerPos = physics.getNearCorner (mPlayerPhysicsHandle);

	fwrite (&playerPos, sizeof v3d_t, 1, file);

	fclose (file);
	return 0;
}



int SpaceGame::loadPlayerData (void) {
	FILE *file = fopen ("save/player.dat", "rb");
	if (file == NULL) {
		return -1;
	}

	v3d_t playerPos;
	fread (&playerPos, sizeof v3d_t, 1, file);

	physics.set_pos (mPlayerPhysicsHandle, playerPos);

	fclose (file);
	return 0;
}

*/


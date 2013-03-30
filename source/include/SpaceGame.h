// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * SpaceGame
// *
// * this contains the main game loop
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SpaceGame_h_
#define SpaceGame_h_

// the main include directory
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <assert.h>

//#include <winbase.h>
#include <cstring>

using namespace std;

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/glut.h"


// the root/code/include directory
#include "v2d.h"

#include "World.h"
#include "StarShip.h"
#include "player.h"
#include "AiManager.h"
#include "physics.h"
#include "GameInput.h"
#include "AssetManager.h"
#include "ItemManager.h"


enum {
	GAMESTATE_PLAY,
	GAMESTATE_MENU
};

enum {
	GAMEMENU_BACKTOGAME,
	GAMEMENU_EXITGAME
};



class SpaceGame {
//	World mWorld;
	StarShip *mStarShip;
	WorldMap *mWorldMap;
	player_c player;
	AiManager mAiManager;
	physics_c physics;
	GameInput mGameInput;
	AssetManager mAssetManager;
	ItemManager mItemManager;

	size_t mPlayerPhysicsHandle;
	size_t mPlayerAiIndex;

	double mLastUpdateTime;

	menu_c mMenu;
	double mGameState;

	int mNumPhysicsObjects;
	int mNumAiObjects;
	int mNumItems;

	int mWorldSeed;

public:
	SpaceGame (void);
	~SpaceGame (void);

	void loadAssets (void);
	void freeAssets (void);

	void setup_opengl (void);

	int enter_game_mode (bool createNewWorld);

	void initializePlanet(bool createNewWorld);
	void initSpaceShip(void);

	void gameLoop();

	bool update (void);
	int draw (double &time);

	void drawPlayerTargetBlock (void);

	void deleteAllFilesInFolder (LPWSTR folderPath);

	int savePlayerData (void);
	int loadPlayerData (void);
};



#endif // SpaceGame_h_

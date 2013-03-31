// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * RogueMapViewer
// *
// * this allows the user to view a RogueMap
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef RogueMapViewer_h_
#define RogueMapViewer_h_

#include "Menu.h"
#include "Rectangle2d.h"
#include "RogueMap.h"

#include "dungeon/DungeonUtil.h"
#include "dungeon/MoleculeDungeon.h"


#define ROGUE_MAP_SIDE			64
#define MAP_MULTIPLIER			128


class RogueMapViewer {
public:
	RogueMapViewer(void);
	~RogueMapViewer(void);

	void setUpOpenGl(void);

	// this method enters the interaction mode
	void startViewer();

	void drawMap(void) const;

	// TODO: get this i/o junk outta here
	int handleInput (void);
	int handleKeystroke (void);
	int handleKeyup (void);
	void handleMouseButtonDown (int button, v2d_t pos);
	void handleMouseButtonUp (int button, v2d_t pos);

	void randomizeDungeon();

// MEMBERS
	SDL_Event sdlevent;
	v2d_t mMousePos;
	v2d_t mMouseDelta;
	int mMouseMoved;
	bool mLeftMouseButtonClicked;

	menu_c *mMenu;

	RogueMap *mRogueMap;

	Rectangle2d mViewRect;


	DungeonUtil *dungeon;
};




#endif // RogueMapViewer_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * GameInput
// *
// * this provides the interface between controller input and the player class
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <SDL2/SDL.h>

#include "AssetManager.h"
#include "v2d.h"


// FIXME: use soft key mappings. preferably a data struct for easy saving.

class GameInput {
public:
	GameInput (void);
	~GameInput (void);

	void reset (void);

	double getFacingDelta (void);
	double getInclinationDelta (void);

	bool isEscapePressed (void);
	bool isWindowClosed (void);

	bool isWalkingForward (void);
	bool isWalkingBackward (void);
	bool isWalkingLeft (void);
	bool isWalkingRight (void);

	bool isJumping (void);

	bool isSwimming (void);

	bool isPickUpItem (void);
	bool isDroppedItem (void);

	bool isUsingPrimary (void);
	bool isUsingSecondary (void);

	bool isClickMouse1 (void);
	bool isClickMouse2 (void);

	bool isNextGun (void);
	bool isPreviousGun (void);

	bool isSoftReset (void);
	bool isTogglePhysics (void);
	bool isAdvanceOneFrame (void);

	bool isIncreasingDrawDistance (void);
	bool isDecreasingDrawDistance (void);

	bool isToggleCharacterSheet (void);
	bool isToggleGodMode (void);

	bool isUseBackpackItem (void);

	bool isToggleWorldChunkBoxes (void);

	int update (void);

private:
	int handleKeyDownEvent (int key);
	int handleKeyUpEvent (int key);
	void handleMouseButtonDownEvent (int button, v2d_t pos);
	void handleMouseButtonUpEvent (int button, v2d_t pos);

	// data members * * * * * * * * * * * *
	double mMouseSensitivity;

	double mFacingDelta;
	double mInclinationDelta;

	bool mEscapePressed;
	bool mWindowClosed;

	bool mWalkForward;
	bool mWalkBackward;
	bool mWalkLeft;
	bool mWalkRight;

	bool mJump;

	bool mSwim;

	bool mPickUpItem;
	bool mDropItem;

	bool mUsePrimary;
	bool mUseSecondary;

	bool mClickMouse1;
	bool mClickMouse2;

	int mNextGun;
	int mPreviousGun;

	bool mSoftReset;
	bool mTogglePhysics;
	bool mAdvanceOneFrame;

	bool mIncreaseDrawDistance;
	bool mDecreaseDrawDistance;

	bool mToggleCharacterSheet;
	bool mToggleGodMode;

	bool mUseBackpackItem;

	bool mToggleWorldChunkBoxes;
};

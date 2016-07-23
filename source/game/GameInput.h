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

#include "../math/v2d.h"

// FIXME: use soft key mappings. preferably a data struct for easy saving.

class GameInput {
public:
  GameInput();
  ~GameInput();

  void reset();

  double getFacingDelta();
  double getInclinationDelta();

  bool isEscapePressed();
  bool isWindowClosed();

  bool isWalkingForward();
  bool isWalkingBackward();
  bool isWalkingLeft();
  bool isWalkingRight();

  bool isJumping();

  bool isSwimming();

  bool isPickUpItem();
  bool isDroppedItem();

  bool isUsingPrimary();
  bool isUsingSecondary();

  bool isClickMouse1();
  bool isClickMouse2();

  bool isNextGun();
  bool isPreviousGun();

  bool isSoftReset();
  bool isTogglePhysics();
  bool isAdvanceOneFrame();

  bool isIncreasingDrawDistance();
  bool isDecreasingDrawDistance();

  bool isToggleCharacterSheet();
  bool isToggleGodMode();

  bool isUseBackpackItem();

  bool isToggleWorldChunkBoxes();

  int update();

private:
  int handleKeyDownEvent(int key);
  int handleKeyUpEvent(int key);
  void handleMouseButtonDownEvent(int button);
  void handleMouseButtonUpEvent(int button);
  void handleMouseWeelEvent(SDL_MouseWheelEvent wheelEvent);


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

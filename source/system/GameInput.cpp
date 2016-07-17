#include "GameInput.h"

GameInput::GameInput() {
  reset ();
}



GameInput::~GameInput() {
}



void GameInput::reset() {
  mMouseSensitivity = 0.004;

  mFacingDelta = 0.0;
  mInclinationDelta = 0.0;

  mEscapePressed = false;
  mWindowClosed = false;

  mWalkForward = false;
  mWalkBackward = false;
  mWalkLeft = false;
  mWalkRight = false;

  mJump = false;

  mSwim = false;

  mPickUpItem = false;
  mDropItem = false;

  mUsePrimary = false;
  mUseSecondary = false;

  mClickMouse1 = false;
  mClickMouse2 = false;

  mNextGun = false;
  mPreviousGun = false;

  mSoftReset = false;
  mTogglePhysics = false;
  mAdvanceOneFrame = false;

  mIncreaseDrawDistance = false;
  mDecreaseDrawDistance = false;

  mToggleCharacterSheet = false;
  mToggleGodMode = false;

  mUseBackpackItem = false;

  mToggleWorldChunkBoxes = false;
}



double GameInput::getFacingDelta (void) {
  double ret = mFacingDelta;

  mFacingDelta = 0.0;

  return ret;
}



double GameInput::getInclinationDelta (void) {
  double ret = mInclinationDelta;

  mInclinationDelta = 0.0;

  return ret;
}



bool GameInput::isEscapePressed (void) {
  if (mEscapePressed) {
    mEscapePressed = false;
    return true;
  }
  return false;
}



bool GameInput::isWindowClosed (void) {
  if (mWindowClosed) {
    mWindowClosed = false;
    return true;
  }
  return false;
}



bool GameInput::isWalkingForward (void) {
  return mWalkForward;
}



bool GameInput::isWalkingBackward (void) {
  return mWalkBackward;
}



bool GameInput::isWalkingLeft (void) {
  return mWalkLeft;
}



bool GameInput::isWalkingRight (void) {
  return mWalkRight;
}



bool GameInput::isJumping (void) {
  if (mJump) {
    mJump = false;
    return true;
  }

  return false;
}



bool GameInput::isSwimming (void) {
  return mSwim;
}



bool GameInput::isPickUpItem (void) {
  return mPickUpItem;
}



bool GameInput::isDroppedItem (void) {
  if (mDropItem) {
    mDropItem = false;
    return true;
  }

  return false;
}



bool GameInput::isUsingPrimary (void) {
  return mUsePrimary;
}



bool GameInput::isUsingSecondary (void) {
  return mUseSecondary;
}



bool GameInput::isClickMouse1 (void) {
  if (mClickMouse1) {
    mClickMouse1 = false;
    return true;
  }

  return false;
}



bool GameInput::isClickMouse2 (void) {
  if (mClickMouse2) {
    mClickMouse2 = false;
    return true;
  }

  return false;
}



bool GameInput::isNextGun (void) {
  if (mNextGun > 0) {
    mNextGun--;
    return true;
  }

  return false;
}



bool GameInput::isPreviousGun (void) {
  if (mPreviousGun > 0) {
    mPreviousGun--;
    return true;
  }

  return false;
}



bool GameInput::isSoftReset (void) {
  if (mSoftReset) {
    mSoftReset = false;
    return true;
  }

  return false;
}



bool GameInput::isTogglePhysics (void) {
  if (mTogglePhysics) {
    mTogglePhysics = false;
    return true;
  }

  return false;
}



bool GameInput::isAdvanceOneFrame (void) {
  if (mAdvanceOneFrame) {
    mAdvanceOneFrame = false;
    return true;
  }

  return false;
}



bool GameInput::isIncreasingDrawDistance (void) {
  if (mIncreaseDrawDistance) {
    mIncreaseDrawDistance = false;
    return true;
  }

  return false;
}



bool GameInput::isDecreasingDrawDistance (void) {
  if (mDecreaseDrawDistance) {
    mDecreaseDrawDistance = false;
    return true;
  }

  return false;
}



bool GameInput::isToggleCharacterSheet (void) {
  if (mToggleCharacterSheet) {
    mToggleCharacterSheet = false;
    return true;
  }

  return false;
}



bool GameInput::isToggleGodMode (void) {
  if (mToggleGodMode) {
    mToggleGodMode = false;
    return true;
  }

  return false;
}



bool GameInput::isUseBackpackItem (void) {
  if (mUseBackpackItem) {
    mUseBackpackItem = false;
    return true;
  }

  return false;
}



bool GameInput::isToggleWorldChunkBoxes (void) {
  if (mToggleWorldChunkBoxes) {
    mToggleWorldChunkBoxes = false;
    return true;
  }

  return false;
}



// handle an SDL_Event
int GameInput::update() {
  SDL_Event sdlEvent;

  int quit = 0;

  // go through all the queued events and handle each one
  while (SDL_PollEvent (&sdlEvent) && !quit) {
    switch (sdlEvent.type) {
      case SDL_QUIT:
        mWindowClosed = true;
        break;

      case SDL_KEYDOWN:
        quit = handleKeyDownEvent (sdlEvent.key.keysym.sym);
        break;

      case SDL_KEYUP:
        handleKeyUpEvent (sdlEvent.key.keysym.sym);
        break;

      case SDL_MOUSEMOTION:
        // change the player's viewing direction
        if (sdlEvent.motion.xrel != 0.0) {
          mFacingDelta += (sdlEvent.motion.xrel * mMouseSensitivity);
        }

        // change the player's view elevation
        if (sdlEvent.motion.yrel != 0.0) {
          mInclinationDelta -= (sdlEvent.motion.yrel * mMouseSensitivity);
        }

        break;

      // handle the mousebuttondown event
      case SDL_MOUSEBUTTONDOWN:
        handleMouseButtonDownEvent(sdlEvent.button.button);
        break;

      case SDL_MOUSEBUTTONUP:
        handleMouseButtonUpEvent(sdlEvent.button.button);
        break;

      case SDL_MOUSEWHEEL:
        // this doens't exist ... hasn't been needed yet
        handleMouseWeelEvent(sdlEvent.wheel);

      default:
        break;
    }
  }

  return quit;
}



// deal with a key down event
int GameInput::handleKeyDownEvent (int key) {
  switch (key) {
    case SDLK_ESCAPE:  // quit
      mEscapePressed = true;
      break;

    case SDLK_w:    // move forward
      mWalkForward = true;
      break;

    case SDLK_s:    // move back
      mWalkBackward = true;
      break;

    case SDLK_a:    // move left
      mWalkLeft = true;
      break;

    case SDLK_d:    // move right
      mWalkRight = true;
      break;

    case SDLK_SPACE:  // jump/swim
      mJump = true;
      mSwim = true;
      break;

    case SDLK_e:
      mPickUpItem = true;
      break;

    case SDLK_m:
      mDropItem = true;
      break;

    case SDLK_u:
      mUseBackpackItem = true;
      break;

    case SDLK_F1:    // reset the player to default settings
      mSoftReset = true;
      break;

    case SDLK_p:    // toggle physics on other objects
      mTogglePhysics = true;
      break;

    case SDLK_n:
      mAdvanceOneFrame = true;
      break;

    case SDLK_RIGHTBRACKET:
      mIncreaseDrawDistance = true;
      break;

    case SDLK_LEFTBRACKET:
      mDecreaseDrawDistance = true;
      break;

    case SDLK_c:
      mToggleCharacterSheet = true;
      break;

    case SDLK_g:
      mToggleGodMode = true;
      break;

    case SDLK_F5:
      mToggleWorldChunkBoxes = true;
      break;


    default:
      break;
  }

  // don't quit!
  return 0;
}



// deal with a key up event
int GameInput::handleKeyUpEvent (int key) {
  switch (key) {
    case SDLK_w:
      mWalkForward = false;
      break;

    case SDLK_s:
      mWalkBackward = false;
      break;

    case SDLK_a:
      mWalkLeft = false;
      break;

    case SDLK_d:
      mWalkRight = false;
      break;

    case SDLK_m:
      mDropItem = false;
      break;

    case SDLK_u:
      mUseBackpackItem = false;
      break;

    case SDLK_e:
      mPickUpItem = false;
      break;

    case SDLK_SPACE:
      mSwim = false;
      break;

    default:
      break;
  }

  return 0;
}



// deal with a mouse button down event
void GameInput::handleMouseButtonDownEvent (int button) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      mUsePrimary = true;
      mClickMouse1 = true;
      break;

    case SDL_BUTTON_RIGHT:
      mUseSecondary = true;
      mClickMouse2 = true;
      break;

    default:
      break;
  }
}



// deal with a mouse button up event
void GameInput::handleMouseButtonUpEvent (int button) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      mUsePrimary = false;
      mClickMouse1 = false;
      break;

    case SDL_BUTTON_RIGHT:
      mUseSecondary = false;
      mClickMouse2 = false;
      break;

    default:
      break;
  }
}

void GameInput::handleMouseWeelEvent(SDL_MouseWheelEvent wheelEvent) {
  if (wheelEvent.y < 0) {
      mPreviousGun++;
  }
  else if (wheelEvent.y > 0) {
      mNextGun++;
  }
}



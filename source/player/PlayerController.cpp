#include "PlayerController.h"

PlayerController::PlayerController(GameModel* gameModel, PlayerView* playerView) :
  mGameModel(gameModel),
  mPlayerView(playerView)
{
  mPlayer = mGameModel->mPlayer;
}

PlayerController::~PlayerController() {}

void PlayerController::update(GameInput* gameInput) {
  mGameInput = gameInput;

  // does the player want to mess with the draw distance?
  // FIXME: this sheeit is temporarily borkened
  // if (mGameInput->isDecreasingDrawDistance()) {
  //   adjust_draw_distance(-20);
  // }
  // if (mGameInput->isIncreasingDrawDistance()) {
  //   adjust_draw_distance(20);
  // }

  // if (mGameInput->isToggleCharacterSheet()) {
  //   mPlayerView->mShowCharacterSheet = !mPlayerView->mShowCharacterSheet;
  // }
  //
  // if (mPlayerView->mShowCharacterSheet) {
  //   if (mGameInput->isEscapePressed()) {
  //     mPlayerView->mShowCharacterSheet = false;
  //   }
  // }
  //
  // if (mPlayerView->mShowCharacterSheet) {
  //   handleInventoryInput();
  // }
  // else {
  //   handleItemUsage();
  // }

  printf("in the loop\n");

  if (mPlayer->isDead()) {
    handleDeadPlayerInput();
    return;
  }
  printf("in the loop\n");


  // does the player wanna pick stuff up?
  if (mGameInput->isPickUpItem()) {
    printf("WARNING: pickUpItem disabled\n");
    // Message message;
    // message.sender = mGameModel->mPhysics->getPlayerHandle();
    // message.recipient = MAILBOX_PHYSICS;
    // message.type = MESSAGE_PLAYERPICKUPITEMS;
    // mGameModel->mMessageBus->sendMessage(message);
  }

  // deal with a soft reset
  if (mGameInput->isSoftReset()) {
    printf("WARNING: soft_reset disabled\n");
    // soft_reset(mStartPosition);
    //
    // // this stuff has changed, so take note!
    // physicsEntity = mGameModel->mPhysics->getEntityByHandle(mGameModel->mPhysics->getPlayerHandle());
  }

  if (mGameInput->isToggleGodMode()) {
    mPlayer->godMode();
  }

  // if (mGameInput->isTogglePhysics()) {
  //   mGameModel->mPhysics->togglePause();
  // }
  // if (mGameInput->isAdvanceOneFrame()) {
  //   mGameModel->mPhysics->advanceOneFrame();
  // }

  updateMovementInput();
}

MovementInput* PlayerController::getMovementInput() {
  return &mMovementInput;
}

void PlayerController::handleItemUsage() {
  if (mGameInput->isUsingPrimary()) {
    mPlayer->useEquipped(EQUIP_PRIMARY);
  }
  if (mGameInput->isUsingSecondary()) {
    mPlayer->useEquipped(EQUIP_SECONDARY);
  }
}

void PlayerController::handleDeadPlayerInput() {
  if (mGameInput->isSoftReset()) {
    mPlayer->HACK_resurrect();
  }
}

// void PlayerController::placeBlock() {
//     // HACK TO PLACE BLOCKS * * * * * *
//     if (mPlacedBlock) {
//       mPlacedBlock = false;
//
//       // FIXME: only works if in primary position
//       if (mGameInput->isClickMouse1() && mIsBlockTargeted) {
//         v3di_t neighborPos = v3di_add(mTargetBlock, gBlockNeighborAddLookup[mTargetBlockFace]);
//
//         BoundingBox bb(v3d_v(1.0, 1.0, 1.0), v3d_v(neighborPos));
//
//         // FIXME: hack that makes it easier for the player to place some
//         // blocks. Could result in a player being inside a block.
//         bb.scale(0.998);
//         bb.translate(v3d_v(0.001, 0.001, 0.001));
//
//         if (!bb.isIntersecting(phys.getEntityByHandle(mPhysicsHandle)->boundingBox)) {
//           // worldMap.fillSphere(v3d_v(neighborPos), 3.0, BLOCK_TYPE_GREEN_STAR_TILE, 0);
//           worldMap.setBlockType(neighborPos, BLOCK_TYPE_GREEN_STAR_TILE);
//         }
//       }
//     }
// }

void PlayerController::updateMovementInput() {
  v2d_zero(&mMovementInput.walkInput);
  if (mGameInput->isWalkingForward()) {
    mMovementInput.walkInput.y += 1.0;
  }
  if (mGameInput->isWalkingBackward()) {
    mMovementInput.walkInput.y -= 1.0;
  }
  if (mGameInput->isWalkingLeft()) {
    mMovementInput.walkInput.x += 1.0;
  }
  if (mGameInput->isWalkingRight()) {
    mMovementInput.walkInput.x -= 1.0;
  }

  mMovementInput.facingDelta = mGameInput->getFacingDelta();
  mMovementInput.inclinationDelta = mGameInput->getInclinationDelta();

  mMovementInput.isJumping = mGameInput->isJumping();
  mMovementInput.isSwimming = mGameInput->isSwimming();
}

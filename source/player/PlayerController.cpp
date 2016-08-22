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

  if (mGameInput->isToggleCharacterSheet()) {
    mPlayerView->mShowCharacterSheet = !mPlayerView->mShowCharacterSheet;
  }

  if (mPlayerView->mShowCharacterSheet) {
    if (mGameInput->isEscapePressed()) {
      mPlayerView->mShowCharacterSheet = false;
    }
  }

  if (mPlayerView->mShowCharacterSheet) {
    handleInventoryInput();
  }
  else {
    handleItemUsage();
  }

  if (mPlayer->isDead()) {
    handleDeadPlayerInput();
    return;
  }


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

void PlayerController::handleInventoryInput() {
  Inventory* inventory = mPlayer->getInventory();

  if (mGameInput->isClickMouse1()) {
    if (inventory->mBackpack[inventory->mSelectedBackpackItem] == 0) {
      inventory->swapBackPackItemIntoPrimary();
    }
    else {
      inventory->swapBackPackItemIntoPrimary();
    }
  }
  if (mGameInput->isClickMouse2()) {
    if (inventory->mBackpack[inventory->mSelectedBackpackItem] == 0) {
      inventory->swapBackPackItemIntoSecondary();
    }
    else {
      inventory->swapBackPackItemIntoSecondary();
    }
  }

  if (mGameInput->isUseBackpackItem()) {
    item_t item = mGameModel->mItemManager->getItem(inventory->mBackpack[inventory->mSelectedBackpackItem]);
    if (item.type == ITEMTYPE_HEALTHPACK) {
      mPlayer->useBackpackItem();
    }
  }

  if (mGameInput->isNextGun()) {
    inventory->nextBackPackItem();
  }

  if (mGameInput->isPreviousGun()) {
    inventory->previousBackPackItem();
  }

  // did the player want to drop that item?
  // FIXME: destroys item!
  // this needs to create a physics item...
  if (mGameInput->isDroppedItem() && inventory->mBackpack[inventory->mSelectedBackpackItem] != 0) {
    mGameModel->mItemManager->destroyItem(inventory->mBackpack[inventory->mSelectedBackpackItem]);
    inventory->mBackpack[inventory->mSelectedBackpackItem] = 0;
  }
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
//       if (mGameInput->isClickMouse1 () && mIsBlockTargeted) {
//         v3di_t neighborPos = v3di_add(mTargetBlock, gBlockNeighborAddLookup[mTargetBlockFace]);
//
//         BoundingBox bb (v3d_v (1.0, 1.0, 1.0), v3d_v (neighborPos));
//
//         // FIXME: hack that makes it easier for the player to place some
//         // blocks. Could result in a player being inside a block.
//         bb.scale (0.998);
//         bb.translate (v3d_v (0.001, 0.001, 0.001));
//
//         if (!bb.isIntersecting (phys.getEntityByHandle (mPhysicsHandle)->boundingBox)) {
//           // worldMap.fillSphere (v3d_v (neighborPos), 3.0, BLOCK_TYPE_GREEN_STAR_TILE, 0);
//           worldMap.setBlockType (neighborPos, BLOCK_TYPE_GREEN_STAR_TILE);
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

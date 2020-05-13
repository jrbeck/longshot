#include "PlayerController.h"

PlayerController::PlayerController(GameModel* gameModel) :
  mGameModel(gameModel)
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

  if (mPlayer->isDead()) {
    handleDeadPlayerInput();
    return;
  }

  // does the player wanna pick stuff up?
  if (mGameInput->isPickUpItem()) {
    printf("WARNING: pickUpItem disabled\n");
    // Message message;
    // message.sender = physicsEntity->handle;
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
    // physicsEntity = mGameModel->mPhysics->getEntityByHandle(physicsEntity->handle);
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
  applyMovementInput();
}

// MovementInput* PlayerController::getMovementInput() {
//   return &mMovementInput;
// }

void PlayerController::handleItemUsage() {
  if (mGameInput->isUsingPrimary()) {
    mPlayer->useEquipped(EQUIP_PRIMARY, &mMovementInput.walkInput);
  }
  if (mGameInput->isUsingSecondary()) {
    mPlayer->useEquipped(EQUIP_SECONDARY, &mMovementInput.walkInput);
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

void PlayerController::applyMovementInput() {
  // TODO: move this somewhere that makes sense ...
  // checkForBlockDamage();

  PhysicsEntity* physicsEntity = mPlayer->getPhysicsEntity();

  if (physicsEntity->worldViscosity > 0.1) {
    applyMovementInputWet(physicsEntity);
  }
  else {
    applyMovementInputDry(physicsEntity);
  }

  mPlayer->updateOrientation(mMovementInput.facingDelta, mMovementInput.inclinationDelta);
}

void PlayerController::checkForBlockDamage() {
  // FIXME: should really call something like: WorldMap::damageBlock()...
  // if (mMeleeStatePrimary.hasUsed) {
  //   if (mIsBlockTargeted) {
  //     mGameModel->mLocation->getWorldMap()->clearBlock(mTargetBlock);
  //   }
  //   mMeleeStatePrimary.hasUsed = false;
  // }
  // if (mMeleeStateSecondary.hasUsed) {
  //   if (mIsBlockTargeted) {
  //     mGameModel->mLocation->getWorldMap()->clearBlock(mTargetBlock);
  //   }
  //   mMeleeStateSecondary.hasUsed = false;
  // }
}

void PlayerController::applyMovementInputDry(PhysicsEntity* physicsEntity) {
  if (mMovementInput.isJumping && physicsEntity->on_ground) {
    v3d_t force = v3d_v(0.0, 45000.0, 0.0);
    mGameModel->mPhysics->add_force(physicsEntity->handle, force);
    // if (r_numi(0, 16) == 3) {
    //   assetManager->mSoundSystem.playSoundByHandle(SOUND_HUMAN_JUMP, 72);
    // }
  }

  if (v2d_mag(mMovementInput.walkInput) > EPSILON) {
    v2d_t walk_force_2d;
    if (physicsEntity->on_ground) {
      walk_force_2d = v2d_scale(mPlayer->computeWalkVector(mMovementInput.walkInput), 2500.0);
      // if (mLastUpdateTime > (mLastFootStep + 0.6) && v3d_mag(physicsEntity->vel) > 0.2) {
      //   assetManager->mSoundSystem.playSoundByHandle(SOUND_FOOTSTEP, r_numi (32, 56));
      //   mLastFootStep = mLastUpdateTime + r_num(0.0, 0.2);
      // }

      // FIXME: probably need to do something about this...
      // ... this used to be in Player
      // headBobbleAction = HEADBOB_ACTION_WALK_FORWARD;
    }
    else {
      walk_force_2d = v2d_scale(mPlayer->computeWalkVector(mMovementInput.walkInput), 300.0);
    }

    v3d_t force = v3d_v(walk_force_2d.x, 0.0, walk_force_2d.y);
    mGameModel->mPhysics->add_force(physicsEntity->handle, force);
  }
}

void PlayerController::applyMovementInputWet(PhysicsEntity* physicsEntity) {
  // double lastUpdateTime = mGameModel->mPhysics->getLastUpdateTime();
  // v3d_t swimForce = v3d_zero();

  // if (physicsEntity->on_ground && mMovementInput.isJumping) {
  //   mGameModel->mPhysics->add_force(physicsEntity->handle, v3d_v(0.0, 4000.0, 0.0));
  // }

  // if (v2d_mag(mMovementInput.walkInput) > EPSILON) {
  //   v2d_t walkVector = mPlayer->computeWalkVector(mMovementInput.walkInput);
  //   v3d_t force = v3d_zero();

  //   if (mMovementInput.walkInput.y > 0.0) { // w is pressed
  //     double mag = 2500.0 * (sin(lastUpdateTime * 10.0) + 1.5);
  //     force = v3d_scale(mag, v3d_normalize(mLookVector));

  //     if (mHeadInWater) {
  //       force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
  //     }
  //   }
  //   else if (mMovementInput.walkInput.y < 0.0) { // s is pressed
  //     double mag = -2500.0 * (sin(lastUpdateTime * 6.0) + 1.5);
  //     force = v3d_scale(mag, v3d_normalize(mLookVector));

  //     if (mHeadInWater) {
  //       force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
  //     }
  //   }
  //   if (mMovementInput.walkInput.x != 0.0) {
  //     double mag = 2500.0 * (sin (lastUpdateTime * 6.0) + 1.5);
  //     force.x += mag * walkVector.x;
  //     force.z += mag * walkVector.y;

  //     if (mHeadInWater) {
  //       force = v3d_add(force, v3d_v(0.0, 600.0, 0.0));
  //     }
  //   }

  //   swimForce = v3d_add(swimForce, force);
  // }
  // if (mMovementInput.isSwimming) {
  //   // 'up' force
  //   double mag = 1500.0 * (sin(lastUpdateTime * 10.0) + 1.5);
  //   mGameModel->mPhysics->add_force(physicsEntity->handle, v3d_v(0.0, mag, 0.0));
  // }

  // swimForce = v3d_scale(1900.0, v3d_normalize(swimForce));
  // mGameModel->mPhysics->add_force(physicsEntity->handle, swimForce);
}

#include "AiManager.h"

AiManager::AiManager (void) {
	mMaxCrittersHACK = 0;
}



AiManager::~AiManager (void) {
	clear();
}



void AiManager::clear(void) {
	mNextHandle = 1;

	mPlayerAiHandle = 0;
	mPlayerPhysicsHandle = 0;

	size_t numEntities = mAiEntities.size();
	for (size_t i = 0; i < numEntities; i++) {
		if (mAiEntities[i] != NULL) {
			delete mAiEntities[i];
		}
		else {
			printf("AiManager::clear(): error: NULL AiEntity\n");
		}
		
	}
	mAiEntities.clear ();
}




size_t AiManager::setPlayerPhysicsHandle (size_t playerPhysicsHandle, physics_c &physics, double time) {
	mPlayerPhysicsHandle = playerPhysicsHandle;

	// add the player as an AI entity
	AiEntity *e = addEntity (AITYPE_PLAYER, v3d_zero(), physics, time);
	if (e == NULL) {
		return 0;
	}

	// HACK - since the caller has already created a phys_entity, and
	// AiManager::addEntity() also creates one, we'll just delete the
	// second one
	physics.removeEntity(e->mPhysicsHandle);
	e->mPhysicsHandle = mPlayerPhysicsHandle;

	mPlayerAiHandle = e->mHandle;
	return mPlayerAiHandle;
}



void AiManager::setPlayerFacingAndIncline (v2d_t facingAndElevation) {
	AiEntity *e = getEntityByHandle(mPlayerAiHandle);
	if (e == NULL) {
		printf("AiManager::setPlayerFacingAndIncline(): no player entity\n");
		return;
	}

	e->mFacingAngle = facingAndElevation.x;
	e->mLookIncline = facingAndElevation.y;
}


AiEntity *AiManager::addEntity(int type, const v3d_t &position, physics_c &physics, double time) {
	AiEntity *e = new AiEntity();
	if (e == NULL) {
		printf("AiManager::addEntity(): error: out of mem\n");
		return NULL;
	}

	size_t physicsHandle = physics.createAiEntity (type, position, time);
	if (physicsHandle == 0) {
		printf("AiManager::addEntity(): error: failed to create phys entity\n");
		return NULL;
	}

	e->mActive = true;
	e->mHandle = mNextHandle++;
	e->mPhysicsHandle = physicsHandle;
	e->mTargetPhysicsHandle = 0;
	e->mType = type;
	e->mWorldPosition = position;

	mAiEntities.push_back(e);

	return e;
}


void AiManager::removeEntity(size_t handle) {
	size_t mNumEntities = mAiEntities.size();
	for (size_t i = 0; i < mNumEntities; i++) {
		if (mAiEntities[i]->mHandle == handle) {
			if (mAiEntities[i]->mType == AITYPE_PLAYER) {
				printf("AiManager::removeEntity(): error: asked to remove player AI\n");
				return;
			}

			delete mAiEntities[i];
			swap(mAiEntities[i], mAiEntities[mNumEntities - 1]);
			mAiEntities.pop_back();
			return;
		}
	}
}


AiEntity *AiManager::getEntityByHandle(size_t handle) {
	size_t mNumEntities = mAiEntities.size();
	for (size_t i = 0; i < mNumEntities; i++) {
		if (mAiEntities[i]->mHandle == handle) {
			return mAiEntities[i];
		}
	}
	return NULL;
}


vector<AiEntity *> *AiManager::getEntities(void) {
	return &mAiEntities;
}


size_t AiManager::spawnEntity(
	int type,
	v3d_t position,
	double time,
	physics_c &physics,
	ItemManager &itemManager)
{
	AiEntity *newAiEntity = addEntity(type, position, physics, time);
	if (newAiEntity == NULL) {
		printf ("AiManager::spawnEntity(): error: could not allocate new entity\n\n");
		return 0;
	}
	size_t physicsHandle = newAiEntity->mPhysicsHandle;
	if (physicsHandle == 0) {
		return 0;
	}

	double level;

	switch (type) {
		case AITYPE_BALLOON:
			newAiEntity->mMaxHealth = 15.0;
			newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
			newAiEntity->mWillAttackSameSpecies = true;

			physics.setHealth(physicsHandle, newAiEntity->mMaxHealth);
			physics.setMass(physicsHandle, 10.0);
			physics.setDimensions(physicsHandle, v3d_v (1.8, 1.8, 1.8));
			break;

		case AITYPE_SHOOTER:
			newAiEntity->mMaxHealth = 15.0;
			newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
			newAiEntity->mWillAttackSameSpecies = false;

			physics.setHealth(physicsHandle, newAiEntity->mMaxHealth);
			physics.setMass(physicsHandle, 100.0);
			physics.setDimensions(physicsHandle, v3d_v(0.75, 1.8, 0.75));
			break;

		case AITYPE_HOPPER:
			level = r_num (1.0, 3.0);
			newAiEntity->mMaxHealth = (float)level * 5.0f;
			newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
			newAiEntity->mWillAttackSameSpecies = true;

			physics.setHealth(physicsHandle, newAiEntity->mMaxHealth);
			physics.setMass(physicsHandle, 100.0);
			physics.setDimensions(physicsHandle, v3d_scale(0.5 + (level * 0.3), v3d_v(0.85, 0.85, 0.85)));
			break;

		case AITYPE_HUMAN:
			level = r_num (1.0, 3.0);
			newAiEntity->mMaxHealth = (float)level * 5.0f;
			newAiEntity->mCurrentHealth = newAiEntity->mMaxHealth;
			newAiEntity->mWillAttackSameSpecies = false;

			physics.setHealth(physicsHandle, newAiEntity->mMaxHealth);
			physics.setMass(physicsHandle, 100.0);
			physics.setDimensions(physicsHandle, v3d_v (0.75, 1.8, 0.75));
			break;

		default:
			newAiEntity->mMaxHealth = 600000.0;
			newAiEntity->mCurrentHealth = 600000.0;

			physics.setHealth(physicsHandle, newAiEntity->mMaxHealth);
			physics.setMass(physicsHandle, 10.0);
			physics.setDimensions(physicsHandle, v3d_v(1.25, 1.25, 1.25));
			break;
	}

	// tell the physics who we really are
	physics.setOwner(physicsHandle, newAiEntity->mHandle);

	// give it some items
	for (int i = 0; i < AiEntity::AI_INVENTORY_SIZE; i++) {
		double itemRandom = r_num (0.0, 10.0);

		if (type == AITYPE_HOPPER) {
			if (itemRandom > 7.0) {
				newAiEntity->mInventory[i] = itemManager.generateRandomItem ();
			}
			else {
				newAiEntity->mInventory[i] = 0;
			}
		}
		else if (type == AITYPE_SHOOTER ||
				type == AITYPE_BALLOON ||
				type == AITYPE_HUMAN)
		{
			if (i == 0) {
				// generate gun in first slot
				if (itemRandom > 9.6) {
					// this one could be dangerous
					newAiEntity->mInventory[i] =
						itemManager.generateRandomGun (r_num (5.0, 10.0));
				}
				else {
					// 'weak' gun
					newAiEntity->mInventory[i] =
						itemManager.generateRandomGun (r_num (1.0, 2.5));
				}
			}
			else {
				// maybe generate some other random item
				if (itemRandom > 9.0) {
					newAiEntity->mInventory[i] = itemManager.generateRandomItem ();
				}
				else {
					newAiEntity->mInventory[i] = 0;
				}
			}
		}
	}

	return newAiEntity->mHandle;
}



int AiManager::update (
	double time,
	WorldMap &worldMap,
	physics_c &physics,
	ItemManager &itemManager)
{
	if (physics.isPaused ()) {
		return static_cast<int>(mAiEntities.size ());
	}

	// clean up the entity list a bit
	trimEntitiesList ();

	// we'll just count these up
	size_t liveCritters = 0;
	for (size_t i = 0; i < mAiEntities.size (); i++) {
		if (mAiEntities[i]->mCurrentHealth > 0.0) {
			liveCritters++;
		}
	}

	// check out what the physics engine has to say
	readPhysicsMessages (time, physics, itemManager);

	// FIXME: critter spawner
	if (liveCritters < mMaxCrittersHACK) {
		v3d_t pos = physics.getCenter (mPlayerPhysicsHandle);
		v2d_t offset = v2d_v (r_num (-10.0, 10.0), r_num (-10.0, 10.0));
		offset = v2d_scale (40.0, v2d_normalize (offset));

		pos.x += offset.x;
		pos.z += offset.y;

		int columnIndex = worldMap.getColumnIndex(pos);
		double height = worldMap.mColumns[columnIndex].getHighestBlockHeight();

		if (height < WATER_LEVEL + 1) {
			height = WATER_LEVEL + 3;
		}

		pos.y = height + 30.0;

		int type = r_numi (1, NUM_AITYPES);

		spawnEntity (type, pos, time, physics, itemManager);
	}

//	double pre_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
//	double post_time = (static_cast<double>(SDL_GetTicks ()) / 1000.0);
//	printf ("AI time: %.4f\n", post_time - pre_time);
	
	// now to update the entities
	for (size_t i = 0; i < mAiEntities.size (); i++) {
		// ignore players and dead things
		if (mAiEntities[i]->mType == AITYPE_PLAYER) {
			mAiEntities[i]->updatePlayer (physics);
		}

		// kill off dead physics objects
		if (physics.getIndexFromHandle (mAiEntities[i]->mPhysicsHandle) == -1) {
			mAiEntities[i]->mActive = false;
		}
		else if (mAiEntities[i]->mActive) {
			mAiEntities[i]->update (time, worldMap, physics, mAiEntities, itemManager);
		}

		// handle dead entities here by giving up their weapons to the physics engine
		if (mAiEntities[i]->mActive == false) {
			releaseItems(i, physics);
		}
	}

	return static_cast<int>(mAiEntities.size ());
}


void AiManager::releaseItems(int index, physics_c &physics) {
	int items = 0;
	printf("eliminating inventory\n");
			
	// tell the ItemManager to release the inventory items
	for (int inventoryIndex = 0; inventoryIndex < AiEntity::AI_INVENTORY_SIZE; inventoryIndex++) {
		if (mAiEntities[index]->mInventory[inventoryIndex] > 0) {
			phys_message_t message;
			message.recipient = MAILBOX_ITEMMANAGER;
			message.type = PHYS_MESSAGE_ITEM_DESTROYED;
			message.iValue = static_cast<int>(mAiEntities[index]->mInventory[inventoryIndex]);
			physics.sendMessage(message);

			mAiEntities[index]->mInventory[inventoryIndex] = 0;
			items++;
		}
	}

	if (items > 0) printf("AiManager::releaseItems(): eliminating inventory %d\n", items);
}


void AiManager::trimEntitiesList (void) {
	size_t numEntities = mAiEntities.size();
	for (size_t i = 0; i < numEntities; i++) {
		if (mAiEntities[i]->mActive == false) {
			if (mAiEntities[i]->mType == AITYPE_PLAYER) {
				printf("AiManager::trimEntitiesList(): error: player inactive\n");
				printf(" (phys handle: %d)\n", mAiEntities[i]->mPhysicsHandle);
				continue;
			}
			removeEntity(mAiEntities[i]->mHandle);
			numEntities = mAiEntities.size();
			i--;
		}
	}
}


void AiManager::readPhysicsMessages (double time, physics_c &physics, ItemManager &itemManager) {
	phys_message_t message;
	int type;

	while (physics.getNextMessage (MAILBOX_AIMANAGER, &message)) {
		switch (message.type) {
		case PHYS_MESSAGE_SPAWN_CREATURE:
			type = r_numi (1, NUM_AITYPES);
			spawnEntity (type, message.vec3, time, physics, itemManager);
			break;

		default:
			break;
		}
	}
}



vector<size_t> AiManager::getAllItemHandles(void) {
	vector<size_t> itemList;

	for (size_t i = 0; i < mAiEntities.size(); i++) {
		for (size_t j = 0; j < AiEntity::AI_INVENTORY_SIZE; j++) {
			if (mAiEntities[i]->mInventory[j] > 0) {
				itemList.push_back(mAiEntities[i]->mInventory[j]);
			}
		}
	}

	return itemList;
}


void AiManager::setMaxCritters(int num) {
	mMaxCrittersHACK = num;
}


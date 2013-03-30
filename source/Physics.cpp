#include "Physics.h"


physics_c::physics_c () {

	// * * * * * * * * * * * * * * * * * * *
	// lmao...this was getting a bit tedious
	// FIXME: but seriously, this is disgusting, and needs to
	// be addressed...
#include "phys_entity_types.hack"
	// * * * * * * * * * * * * * * * * * * *
	// * * * * * * * * * * * * * * * * * * *

	reset ();
}



physics_c::~physics_c () {
	clear ();
}


void physics_c::reset (void) {
	clear ();

	mLastUpdateTime = 0.0;

	mPaused = false;
	mAdvanceOneFrame = false;

	mGravity = DEFAULT_GRAVITY;
	mFriction = DEFAULT_FRICTION;

	mNumSoundEvents = 0;
}



void physics_c::clear (void) {
	obj.clear ();

	mLastEntityHandle = 0;
	mEntityAdded = false;

	mPlayerHandle = 0;

	mMessages.clear ();
	mGotPickupMessage = false;
}


vector <phys_entity_t> *physics_c::getEntityVector(void) {
	return &obj;
}



void physics_c::loadInactiveList (void) {
	mInactiveList.loadFromDisk ("save/inactive.list");
}



int physics_c::getIndexFromHandle (size_t handle) {
	for (size_t index = 0; index < obj.size (); index++) {
		if (obj[index].handle == handle) {
			return static_cast<int>(index);
		}
	}

	// failed to find the object
	return -1;
}



void physics_c::togglePause (void) {
	mPaused = !mPaused;
}


bool physics_c::isPaused (void) {
	return mPaused;
}



void physics_c::advanceOneFrame (void) {
	mAdvanceOneFrame = true;
}



void physics_c::addQueuedEntities (void) {
	// return if nothing was added
	if (!mEntityAdded) return;

	for (size_t i = 0; i < obj.size (); i++) {
		if (obj[i].queued) {
			obj[i].queued = false;
		}
	}

	mEntityAdded = false;
}



void physics_c::manageEntitiesList (void) {
	for (int i = 0; i < static_cast<int>(obj.size ()); i++) {
		if (!obj[i].active) {
			// clear out the ol' mailbox
			clearMailBox (static_cast<int>(obj[i].handle));

			int lastIndex = static_cast<int>(obj.size ()) - 1;

			if (i == lastIndex) {
				obj.pop_back ();
			}
			else {
				swap (obj[i], obj[lastIndex]);
				obj.pop_back ();
				i--;
			}
		}
	}
}



size_t physics_c::createEntity (int type, v3d_t position, double time, bool center) {
	phys_entity_t e;

	e.handle = ++mLastEntityHandle;

	e.queued = true;
	e.active = true;

	e.type = type;
	if (type == OBJTYPE_PLAYER) {
		e.aiType = AITYPE_PLAYER;
		mPlayerHandle = e.handle;
	}
	
	e.force = v3d_zero ();
	e.vel = v3d_zero ();
	e.displacement = v3d_zero ();

	e.on_ground = false;
	e.worldViscosity = 0.0;

	e.applyPhysics = true;

	// MASS
	e.mass = r_num (mEntityTypeInfo[type].massLow, mEntityTypeInfo[type].massHigh);
	e.one_over_mass = 1.0 / e.mass;

	// LIFESPAN
	e.lifespan = r_num (mEntityTypeInfo[type].lifespanLow, mEntityTypeInfo[type].lifespanHigh);
	if (e.lifespan >= 0.0) {
		e.expirationTime = time + e.lifespan;
	}
	else {
		e.expirationTime = -1.0;
	}

	// COLOR
	e.baseColor[0] = e.color[0] = mEntityTypeInfo[type].color[0];
	e.baseColor[1] = e.color[1] = mEntityTypeInfo[type].color[1];
	e.baseColor[2] = e.color[2] = mEntityTypeInfo[type].color[2];
	e.baseColor[3] = e.color[3] = mEntityTypeInfo[type].color[3];

	// DIMENSIONS
	v3d_t dimensions;

	dimensions.x = r_num (mEntityTypeInfo[type].dimensionXLow, mEntityTypeInfo[type].dimensionXHigh);
	dimensions.y = r_num (mEntityTypeInfo[type].dimensionYLow, mEntityTypeInfo[type].dimensionYHigh);
	dimensions.z = r_num (mEntityTypeInfo[type].dimensionZLow, mEntityTypeInfo[type].dimensionZHigh);

	e.boundingBox.setDimensions (dimensions);

	// do we want the given position to be the center of the box?
	if (center) {
		e.boundingBox.setCenterPosition (position);
	}
	else {
		e.boundingBox.setPosition (position);
	}

	e.pos = e.boundingBox.getNearCorner ();

	e.explosionForce = 0.0;

	size_t size_before = obj.size ();

	obj.push_back (e);

	// you fail
	if (obj.size () == size_before) return 0;

	// you win!
	mEntityAdded = true;

	return e.handle;
}



size_t physics_c::createEntity (int type, v3d_t position, v3d_t initialForce, double time, bool center) {
	size_t handle = createEntity (type, position, time, center);

	if (handle != 0) {
		int index = getIndexFromHandle (handle);

		obj[index].force = initialForce;
	}

	return handle;
}



size_t physics_c::createAiEntity (int aiType, v3d_t position, double time) {
	size_t handle = createEntity (OBJTYPE_AI_ENTITY, position, time, true);

	if (handle != 0) {
		int index = getIndexFromHandle (handle);

		obj[index].aiType = aiType;
	}

	return handle;
}



void physics_c::removeEntity (size_t handle) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].active = false;
		return;
	}
}



void physics_c::setMass (size_t handle, double mass) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].mass = mass;
		obj[index].one_over_mass = 1.0 / mass;
		return;
	}
}



void physics_c::setHealth (size_t handle, double health) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].health = health;
		return;
	}
}



void physics_c::setDimensions (size_t handle, v3d_t dimensions) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].boundingBox.setDimensions (dimensions);
		return;
	}
}



phys_entity_t *physics_c::getEntityByHandle (size_t handle) {
	int index = getIndexFromHandle (handle);
	if (index >= 0) {
		return &obj[index];
	}
//	printf ("physics_c::getEntityByHandle (): error: obj (%d) doesn't exist\n", handle);

	return NULL;
}



void physics_c::setEntity (phys_entity_t entity) {
	int index = getIndexFromHandle (entity.handle);

	if (index != -1) {
		obj[index] = entity;
	}
	else {
		printf ("physics_c::setEntity (): failed to set entity\n");
	}
}



v3d_t physics_c::getCenter (size_t handle) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		return obj[index].boundingBox.getCenterPosition ();
	}

	return v3d_zero ();
}



v3d_t physics_c::getNearCorner (size_t handle) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		return obj[index].pos;
	}

	return v3d_zero ();
}



v3d_t physics_c::getFarCorner (size_t handle) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		return obj[index].boundingBox.getFarCorner ();
	}

	return v3d_zero ();
}



v3d_t physics_c::getVelocity (size_t handle) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		return obj[index].vel;
	}

	return v3d_zero ();
}



void physics_c::setVelocity (size_t handle, v3d_t velocity) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].vel = velocity;
	}
}



void physics_c::set_pos (size_t handle, v3d_t pos) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].pos = pos;
		obj[index].vel = v3d_zero ();

		obj[index].boundingBox.setPosition (pos);
	}
}



void physics_c::setOwner (size_t handle, size_t owner) {
	int index = getIndexFromHandle(handle);

	if (index != -1) {
		obj[index].owner = owner;
	}
}



// this is for ItemManager items - get the handle and the type
void physics_c::setItemHandleAndType (size_t handle, size_t itemHandle, int itemType) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].itemHandle = itemHandle;
		obj[index].type2 = itemType;
	}
}



// apply the physics
int physics_c::update (double time, WorldMap &worldMap, AssetManager &assetManager) {

	// read the mail
	phys_message_t message;
	int index;

	while (getNextMessage (MAILBOX_PHYSICS, &message)) {
		switch (message.type) {
			case PHYS_MESSAGE_PLAYERPICKUPITEMS:
				printf ("got message\n");
				mGotPickupMessage = true;
				break;

			case PHYS_MESSAGE_ITEMGRABBED:
				removeEntity (message.iValue2);
				break;

			case PHYS_MESSAGE_ADJUSTHEALTH:
				index = getIndexFromHandle (message.iValue);

				if (index >= 0) {
					obj[index].health += message.dValue;
				}

				break;

			default:
				break;
		}
	}


//	printf ("phys objs: %d\n", obj.size ());
	size_t numEntities = obj.size ();
	bool timeAdvanced;

	if (!mPaused || (mPaused && mAdvanceOneFrame)) {
		timeAdvanced = true;
	}
	else {
		timeAdvanced = false;
	}

	// step through the objects
	for (size_t index = 0; index < numEntities; index++) {
		if (obj[index].active == false) continue;

		if (mPaused && !timeAdvanced && (obj[index].expirationTime > 0.0)) {
			obj[index].expirationTime += PHYSICS_TIME_GRANULARITY;
		}
			
		if (obj[index].queued) continue;

		if (!mPaused ||
			(mAdvanceOneFrame || obj[index].handle == mPlayerHandle))
		{
			// update that mofar
			updateEntity (index, time, worldMap);
		}

		if (obj[index].applyPhysics == false) {
			obj[index].vel = v3d_zero ();
		}

	}
	// now clip against the other objects
	for (size_t index = 0; index < numEntities; index++) {
		if (obj[index].active == false ||
			obj[index].queued)
		{
			continue;
		}

		if (!mPaused ||
			(mAdvanceOneFrame || obj[index].handle == mPlayerHandle))
		{
			clipDisplacementAgainstOtherObjects (index, worldMap);
		}
	}
	// now we can move them
	for (size_t index = 0; index < numEntities; index++) {
		if (obj[index].active == false ||
			obj[index].queued)
		{
			continue;
		}

		if (!mPaused ||
			(mAdvanceOneFrame || obj[index].handle == mPlayerHandle))
		{
			// now move that shizzat
			displaceObject (index, worldMap);
		}
	}


	// clean up the object list if possible
	manageEntitiesList ();

	mAdvanceOneFrame = false;

	mLastUpdateTime = time;

	if (timeAdvanced)  {
		// add the queued entities
		addQueuedEntities ();
	}

	// play the sounds
	playSoundEvents (assetManager);

//	printf ("phys: %d\n", obj.size ());
//	printf ("messages: %d\n", mMessages.size ());

	return static_cast<int>(obj.size ());
}



void physics_c::updateEntity (size_t index, double time, WorldMap &worldMap) {
	if ((obj[index].expirationTime < time) &&
		(obj[index].expirationTime >= 0.0))
	{
		expireEntity(index, time, worldMap);
		return;
	}

	// kill off the things that die in liquid
	if (obj[index].worldViscosity > 0.0 &&
		(obj[index].type == OBJTYPE_SMOKE ||
		 obj[index].type == OBJTYPE_NAPALM ||
		 obj[index].type == OBJTYPE_FIRE ||
		 obj[index].type == OBJTYPE_SPARK))
	{
		expireEntity(index, time, worldMap);
		return;
	}


	// handle plant interaction (arnie!)
	if (obj[index].type == OBJTYPE_SLIME ||
		obj[index].type == OBJTYPE_NAPALM ||
		obj[index].type == OBJTYPE_PLASMA ||
		obj[index].type == OBJTYPE_FIRE ||
		obj[index].type == OBJTYPE_LIVE_BULLET ||
		obj[index].type == OBJTYPE_PLASMA_BOMB)
	{
		if (r_numi(0, 10) > 4 &&
			v3d_mag(obj[index].vel) > 7.0 &&
			isIntersectingPlant(index, worldMap))
		{
			v3di_t pos = v3di_v(obj[index].boundingBox.getCenterPosition());
			worldMap.clearBlock(pos);
			expireEntity (index, time, worldMap);
			return;
		}
	}


	// deal with AI
	if (obj[index].type == OBJTYPE_AI_ENTITY) {
		// check if AI is dead
		if (obj[index].health < -5.0) {
			expireEntity (index, time, worldMap);
			return;
		}
	}

	// kill it if it's fallen too far
	// FIXME: hack, really
	if (obj[index].type != OBJTYPE_PLAYER &&
		obj[index].pos.y < SEA_FLOOR_HEIGHT - 1000.0)
	{
//		printf ("someone fell off the planet!\n");
		expireEntity (index, time, worldMap);
		return;
	}

	// get the acceleration from the forces at work
	obj[index].acc = calculateAcceleration (index);

	if (obj[index].applyPhysics) {
		integrate_euler (index, time, worldMap);
	}

	size_t otherHandle;
	int otherIndex;
	int rnum = r_numi (2, 3);

	// FIXME: this is a stupid place for this...
	switch (obj[index].type) {
		case OBJTYPE_NAPALM:
			if (r_numi (0, 100) == 0) {
				otherHandle = createEntity (OBJTYPE_FIRE, obj[index].pos, time, false);
				otherIndex = getIndexFromHandle (otherHandle);
				if (otherIndex >= 0) {
					obj[otherIndex].owner = obj[index].owner;
					obj[otherIndex].impactDamage = obj[index].impactDamage;
				}
			}
			break;

		case OBJTYPE_SMOKE:
			obj[index].boundingBox.scale (1.009);

			break;

		case OBJTYPE_DEAD_BULLET:
		case OBJTYPE_LIVE_BULLET:
			if (r_numi (0, 100) == 0) {
				otherHandle = createEntity (OBJTYPE_STEAM, obj[index].pos, time, false);
				otherIndex = getIndexFromHandle (otherHandle);
				if (otherIndex >= 0) {
					obj[otherIndex].color[3] = 0.2f;
				}
			}
			break;

		case OBJTYPE_ROCKET:
			if (r_numi (0, 4) == 0) {
				otherHandle = createEntity (OBJTYPE_FIRE, obj[index].pos, time, false);

				otherIndex = getIndexFromHandle (otherHandle);

				if (otherIndex >= 0) {
					obj[otherIndex].owner = obj[index].owner;
					obj[otherIndex].impactDamage = obj[index].impactDamage;
				}
			}
			break;

		case OBJTYPE_PLASMA_BOMB:
			for (int i = 0; i < rnum; i++) {
				otherHandle = createEntity (OBJTYPE_PLASMA_SPARK,
					v3d_add (v3d_random (0.20), obj[index].boundingBox.getCenterPosition ()), time, true);

				setVelocity (otherHandle, v3d_random (0.5));
			}
			
			createEntity (OBJTYPE_PLASMA_TRAIL,
				obj[index].boundingBox.getCenterPosition (), time, true);

			break;

		case OBJTYPE_AI_ENTITY:
			obj[index].health += worldMap.getHealthEffects (obj[index].boundingBox);
			break;

		case OBJTYPE_PLAYER:
			obj[index].health += worldMap.getHealthEffects (obj[index].boundingBox);
			break;

		default:
			break;
	}
}



void physics_c::expireEntity (size_t index, double time, WorldMap &worldMap) {
	size_t newHandle, newIndex;
	v3d_t pos;
	bool removeEntity = true;

	switch (obj[index].type) {
		case OBJTYPE_FIRE:
			newHandle = createEntity (OBJTYPE_SMOKE, obj[index].pos, time, false);

			if (newHandle != 0) {
				newIndex = getIndexFromHandle (newHandle);
				obj[newIndex].boundingBox.setDimensions (obj[index].boundingBox.getDimensions ());

				obj[newIndex].vel.x = r_num (-0.2, 0.2);
				obj[newIndex].vel.y = obj[index].vel.y * 0.6;
				obj[newIndex].vel.z = r_num (-0.2, 0.2);
			}

			break;

		case OBJTYPE_EXPLOSION:
			pos = obj[index].boundingBox.getCenterPosition ();

			for (size_t i = 0; i < 10; i++) {
				v3d_t force = v3d_v (r_num (-5., 5.0), r_num (-5.0, 5.0), r_num (-5.0, 5.0));

				createEntity (OBJTYPE_SMOKE, pos, force, time, false);
			}

			break;

		case OBJTYPE_ROCKET:
		case OBJTYPE_GRENADE:
			grenadeExplosion (index, time, worldMap);
			break;

		case OBJTYPE_PLASMA:
			plasmaBombExplode (obj[index].boundingBox.getCenterPosition (), time, 5, worldMap);
			break;

		case OBJTYPE_PLASMA_BOMB:
			plasmaBombExplode (obj[index].boundingBox.getCenterPosition (), time, 30, worldMap);
			break;

		case OBJTYPE_AI_ENTITY:
			if (obj[index].health <= 0.0) {
				spawnMeatExplosion (obj[index].boundingBox.getCenterPosition (), time + PHYSICS_TIME_GRANULARITY, 15, worldMap);
			}

			break;

		case OBJTYPE_ITEM:
			phys_message_t message;

//			printf ("expiring item: %d\n", obj[index].itemHandle);

			message.recipient = MAILBOX_ITEMMANAGER;
			message.type = PHYS_MESSAGE_ITEM_DESTROYED;
			message.iValue = static_cast<int>(obj[index].itemHandle);

			sendMessage (message);

			break;

		case OBJTYPE_MONSTER_SPAWNER:
			pos = obj[index].boundingBox.getCenterPosition ();

			// check if close enough to player
			if (v3d_dist (pos, getEntityByHandle (mPlayerHandle)->pos) < 100.0 &&
				!worldMap.lineOfSight (pos, getEntityByHandle(mPlayerHandle)->boundingBox.getCenterPosition ()))
			{
				phys_message_t message;

				message.sender = MAILBOX_PHYSICS;
				message.recipient = MAILBOX_AIMANAGER;
				message.type = PHYS_MESSAGE_SPAWN_CREATURE;
				pos.y += 1.5;
				message.vec3 = pos;

				v3d_print ("BLURP!", pos);
				sendMessage (message);

				// eh! why not?
//				spawnMeatExplosion (pos, time + PHYSICS_TIME_GRANULARITY, 15, worldMap);

			}

			obj[index].expirationTime += obj[index].lifespan;

			removeEntity = false;

			break;

		default:
			break;

	}

	// mark the object for destruction
	if (removeEntity) {
		obj[index].active = false;
	}
}





v3d_t physics_c::calculateAcceleration (size_t index) {
	if (obj[index].type == OBJTYPE_MELEE_ATTACK) {
		return obj[index].acc;
	}

	// a = f / m
	v3d_t acc = v3d_scale(obj[index].force, obj[index].one_over_mass);

	// clear the force now that we done with it
	obj[index].force = v3d_zero ();

	// FIXME: c'mon more hacks?
	static double frictionCoeff = 0.98;

	v3d_t drag;
	double speed;

	switch (obj[index].type) {
		case OBJTYPE_PLAYER:
		case OBJTYPE_AI_ENTITY:
			acc.y += mGravity;
			frictionCoeff = mFriction;
			
			speed = v3d_mag (obj[index].vel);

//			printf ("speed: %3.5f\n", speed);
//			v3d_print ("pos", obj[index].pos);

			if (speed > 0.0 && obj[index].worldViscosity > 0.0) {
				drag = v3d_scale (speed * obj[index].worldViscosity, v3d_normalize (v3d_neg (obj[index].vel)));
				acc = v3d_add (acc, drag);
			}
		

			break;

/*		case OBJTYPE_LIVE_BULLET:
			acc.y = -1.0;
			frictionCoeff = 0.98;
			break;
*/

		case OBJTYPE_GRENADE:
			acc.y += mGravity;
			frictionCoeff = 0.98;
			break;

		case OBJTYPE_FIRE:
			acc.y = 20.0;

			frictionCoeff = 0.98;
			break;

		case OBJTYPE_ROCKET:
			// FIXME: c'mon bro! fo real?

		case OBJTYPE_PLASMA:
		case OBJTYPE_PLASMA_TRAIL:
		case OBJTYPE_PLASMA_BOMB:
		case OBJTYPE_TIGER_BAIT:
		case OBJTYPE_MELEE_ATTACK:
			break;

		case OBJTYPE_STEAM:
		case OBJTYPE_SMOKE:
			if (obj[index].vel.y > 0.8) {
				acc.y = -4.25;
			}
			else {
				acc.y = 0.5;
			}

//			drag = v3d_scale (150.0, v3d_neg (v3d_normalize (obj[handle].vel)));
//			acc = v3d_add (acc, drag);
			
			frictionCoeff = 0.98;
			break;

		default:
			acc.y += mGravity;
			frictionCoeff = mFriction;
			break;
	}

	if (!obj[index].applyPhysics) {
		acc = v3d_zero ();
	}
	else if (obj[index].on_ground && acc.y <= 0.0) {
		// FIXME: this is ugly
		// FIXME: also, this doesn't account for gravity against any other surfaces
		// i.e. ceilings, walls...
		if (acc.y < 0.0) acc.y = 0.0;

		if (obj[index].vel.y < 0.0) obj[index].vel.y = 0.0;

		// now apply friction
		v3d_t ground_vel = obj[index].vel;
		ground_vel.y = 0.0;

		double groundSpeed = v3d_mag(ground_vel);

		if (groundSpeed >= 0.1) {
			double fric;

			// this kinda resembles: F = mu * N
			// i.e. force of fric = coeff of fric * normal force
//			frictionCoeff = 0.98;
//			fric = frictionCoeff * DEFAULT_GRAVITY;// * obj[index].mass;

			// FIXME: this was altered to keep fast moving objects from
			// being 'reflected' by the ground friction (velocity squared)
			// it is in SUPER HACK state right now
			if (groundSpeed < 4.0) {
//				fric = -mFriction * (1.0 / abs (groundSpeed));
				fric = -0.75 * ((4.0 - groundSpeed) + (4.0 * 4.0 * frictionCoeff));
			}
			else if (groundSpeed < 20.0) {
				fric = -frictionCoeff * (groundSpeed * groundSpeed);
			}
			else {
				fric = -1.5 * (groundSpeed * frictionCoeff);
			}

			v3d_t drag_vec = v3d_scale (v3d_normalize (ground_vel), fric);

			acc = v3d_add (acc, drag_vec);
		}
		else {
			obj[index].vel.x = 0.0;
			obj[index].vel.z = 0.0;
		}

	}

	return acc;
}



void physics_c::integrate_euler (size_t index, double time, WorldMap &worldMap) {
	if (obj[index].type == OBJTYPE_MELEE_ATTACK) {
		return;
	}


	// update velocity
	obj[index].vel = v3d_add (obj[index].vel,
						v3d_scale (obj[index].acc, PHYSICS_TIME_GRANULARITY));

	// get a displacement vector for the time slice
	v3d_t disp = v3d_scale (obj[index].vel, PHYSICS_TIME_GRANULARITY);

	// mark them as inert if they've stopped moving
//	if (obj[index].type != OBJTYPE_PLAYER &&
//		obj[index].type != OBJTYPE_AI_ENTITY &&
//		v3d_mag (disp) < EPSILON) {
//		obj[index].applyPhysics = false;
//	}

	obj[index].displacement = disp;
	v3d_t disp_before_clip = disp;

	// clip displacement
	if (obj[index].type != OBJTYPE_SMOKE &&
		obj[index].type != OBJTYPE_FIRE)
	{
		clip_displacement_against_world (worldMap, index);
	}

	v3d_t clip = v3d_sub (obj[index].displacement, disp_before_clip);

	if (fabs (clip.x) > 0.0) {
		obj[index].vel.x = 0.0;
//		obj[index].vel.x = -0.5 * obj[index].vel.x;
	}
	if (fabs (clip.y) > 0.0) {
		obj[index].vel.y = 0.0;
//		obj[index].vel.y = -0.5 * obj[index].vel.y;
	}
	if (fabs (clip.z) > 0.0) {
		obj[index].vel.z = 0.0;
//		obj[index].vel.z = -0.5 * obj[index].vel.z;
	}

//	if (obj[index].type == OBJTYPE_SLIME) {
//		v3d_print ("clip", clip);
//		v3d_print ("pos", obj[index].pos);
//		v3d_print ("vel", obj[index].vel);
//		v3d_print ("dis", obj[index].displacement);
//	}

	// create a bouncing force
	if (v3d_mag(clip) > 0.0) {
		double coeff_elasticity;

		switch (obj[index].type) {
			case OBJTYPE_PLAYER:
				coeff_elasticity = 3000.0;
				break;

			case OBJTYPE_GRENADE:
				coeff_elasticity = 50.0;
				break;

			case OBJTYPE_FIRE:
			case OBJTYPE_SMOKE:
				if (r_numi (0, 12) == 0) {
					coeff_elasticity = 500.0;
				}
				else {
					coeff_elasticity = 100.0;
				}

				break;

			case OBJTYPE_SLIME:
				coeff_elasticity = r_num (600.0, 1000.0);

				break;

			default:
				coeff_elasticity = 3000.0;
				break;
		}

		obj[index].force = v3d_add (obj[index].force, v3d_scale (coeff_elasticity, clip));
	}
}


void physics_c::displaceObject (size_t index, WorldMap &worldMap) {
	// move the object
	obj[index].pos = v3d_add (obj[index].pos, obj[index].displacement);

	// if the displacement clip clipped anything, take note!
	if (obj[index].displacement.x == 0.0) obj[index].vel.x = 0.0;
	if (obj[index].displacement.y == 0.0) obj[index].vel.y = 0.0;
	if (obj[index].displacement.z == 0.0) obj[index].vel.z = 0.0;

	// this solves certain issues with the applyPhysics flag
	obj[index].displacement = v3d_zero ();

	// update the bounding box position
	obj[index].boundingBox.setPosition (obj[index].pos);

	// update the on-ground status
	// WARNING: should smoke and fire be omitted?
	if (obj[index].type != OBJTYPE_SMOKE &&
		obj[index].type != OBJTYPE_FIRE) {
		obj[index].on_ground = updateOnGroundStatus (index, worldMap);
	}

	if (obj[index].type != OBJTYPE_AI_ENTITY &&
		obj[index].type != OBJTYPE_PLAYER &&
		obj[index].on_ground &&
		v3d_mag (obj[index].vel) < EPSILON) {
		obj[index].applyPhysics = false;
	}

	// update the inWater status
//	if (obj[index].type == OBJTYPE_PLAYER ||
//		obj[index].type == OBJTYPE_AI_ENTITY) {
		obj[index].worldViscosity = worldMap.getViscosity (obj[index].boundingBox);
//	}

//	if (obj[index].handle == mPlayerHandle) printf ("on ground: %d\n", obj[index].on_ground);
}



// tests to see if a phys entity is on the ground
bool physics_c::updateOnGroundStatus (size_t index, WorldMap &worldMap) {
	v3d_t nc = obj[index].boundingBox.getNearCorner ();

	// first exit chance, can't be on ground if nc.y is not an integer
	if (fabs(nc.y - floor (nc.y))/* > EPSILON &&
		// FIXME: this hack cures the 'fall through the ground sometimes when
		// should be on ground on a y = -1 block' - NOT A SOLUTION!
		fabs (nc.y - 0.0) > EPSILON*/)
	{
		return false;
	}

	v3d_t fc = obj[index].boundingBox.getFarCorner ();

	int y = static_cast<int>(floor (nc.y)) - 1;


//	if (index == getIndexFromHandle(mPlayerHandle)) {
//		printf("y: %d, %f\n", y, nc.y - floor (nc.y));
//	}

	for (int z = static_cast<int>(floor(nc.z)); z <= static_cast<int>(floor(fc.z)); z++) {
		for (int x = static_cast<int>(floor(nc.x)); x <= static_cast<int>(floor(fc.x)); x++) {
			if (worldMap.isSolidBlock (v3di_v (x, y, z))) {
				return true;
			}
		}
	}

	return false;
}



double physics_c::getLastUpdateTime (void) {
	return mLastUpdateTime;
}



bool physics_c::sweepObjects (size_t indexA, size_t indexB, double &t0, double &t1, int &axis) {
	// if they are intersecting before the translation, return true
	if (obj[indexA].boundingBox.isIntersecting (obj[indexB].boundingBox)) {
//		printf ("intersect\n");
		t0 = t1 = 0.0;
		return true;
	}

	v3d_t relativeDisplacement = v3d_sub (obj[indexB].displacement, obj[indexA].displacement);

	v3d_t time0 = v3d_v (0.0, 0.0, 0.0);
	v3d_t time1 = v3d_v (1.0, 1.0, 1.0);

	// x-axis * * * *
	if (obj[indexA].boundingBox.mFarCorner.x < obj[indexB].boundingBox.mNearCorner.x) {
		if (relativeDisplacement.x < 0.0) {
			time0.x = (obj[indexA].boundingBox.mFarCorner.x - obj[indexB].boundingBox.mNearCorner.x) /
				relativeDisplacement.x;
		}
		else {
			return false;
		}
	}
	else if (obj[indexB].boundingBox.mFarCorner.x < obj[indexA].boundingBox.mNearCorner.x) {
		if (relativeDisplacement.x > 0.0) {
			time0.x = (obj[indexA].boundingBox.mNearCorner.x - obj[indexB].boundingBox.mFarCorner.x) /
				relativeDisplacement.x;
		}
		else {
			return false;
		}
	}

	if (obj[indexB].boundingBox.mFarCorner.x > obj[indexA].boundingBox.mNearCorner.x &&
		relativeDisplacement.x < 0.0) {
		time1.x = (obj[indexA].boundingBox.mNearCorner.x - obj[indexB].boundingBox.mFarCorner.x) /
			relativeDisplacement.x;
	}
	else if (obj[indexA].boundingBox.mFarCorner.x > obj[indexB].boundingBox.mNearCorner.x &&
		relativeDisplacement.x > 0.0) {
		time1.x = (obj[indexA].boundingBox.mFarCorner.x - obj[indexB].boundingBox.mNearCorner.x) /
			relativeDisplacement.x;
	}

	// y-axis * * * *
	if (obj[indexA].boundingBox.mFarCorner.y < obj[indexB].boundingBox.mNearCorner.y) {
		if (relativeDisplacement.y < 0.0) {
			time0.y = (obj[indexA].boundingBox.mFarCorner.y - obj[indexB].boundingBox.mNearCorner.y) /
				relativeDisplacement.y;
		}
		else {
			return false;
		}
	}
	else if (obj[indexB].boundingBox.mFarCorner.y < obj[indexA].boundingBox.mNearCorner.y) {
		if (relativeDisplacement.y > 0.0) {
			time0.y = (obj[indexA].boundingBox.mNearCorner.y - obj[indexB].boundingBox.mFarCorner.y) /
				relativeDisplacement.y;
		}
		else {
			return false;
		}
	}

	if (obj[indexB].boundingBox.mFarCorner.y > obj[indexA].boundingBox.mNearCorner.y &&
		relativeDisplacement.y < 0.0) {
		time1.y = (obj[indexA].boundingBox.mNearCorner.y - obj[indexB].boundingBox.mFarCorner.y) /
			relativeDisplacement.y;
	}
	else if (obj[indexA].boundingBox.mFarCorner.y > obj[indexB].boundingBox.mNearCorner.y &&
		relativeDisplacement.y > 0.0) {
		time1.y = (obj[indexA].boundingBox.mFarCorner.y - obj[indexB].boundingBox.mNearCorner.y) /
			relativeDisplacement.y;
	}

	// z-axis * * * *
	if (obj[indexA].boundingBox.mFarCorner.z < obj[indexB].boundingBox.mNearCorner.z) {
		if (relativeDisplacement.z < 0.0) {
			time0.z = (obj[indexA].boundingBox.mFarCorner.z - obj[indexB].boundingBox.mNearCorner.z) /
				relativeDisplacement.z;
		}
		else {
			return false;
		}
	}

	else if (obj[indexB].boundingBox.mFarCorner.z < obj[indexA].boundingBox.mNearCorner.z) {
		if (relativeDisplacement.z > 0.0) {
			time0.z = (obj[indexA].boundingBox.mNearCorner.z - obj[indexB].boundingBox.mFarCorner.z) /
				relativeDisplacement.z;
		}
		else {
			return false;
		}
	}

	if (obj[indexB].boundingBox.mFarCorner.z > obj[indexA].boundingBox.mNearCorner.z &&
		relativeDisplacement.z < 0.0) {
		time1.z = (obj[indexA].boundingBox.mNearCorner.z - obj[indexB].boundingBox.mFarCorner.z) /
			relativeDisplacement.z;
	}
	else if (obj[indexA].boundingBox.mFarCorner.z > obj[indexB].boundingBox.mNearCorner.z &&
		relativeDisplacement.z > 0.0) {
		time1.z = (obj[indexA].boundingBox.mFarCorner.z - obj[indexB].boundingBox.mNearCorner.z) /
			relativeDisplacement.z;
	}

	if (time0.x > time0.y) {
		if (time0.x > time0.z) {
			axis = 0;
			t0 = time0.x;
		}
		else {
			axis = 2;
			t0 = time0.z;
		}
	}
	else if (time0.y > time0.z) {
		axis = 1;
		t0 = time0.y;
	}
	else {
		axis = 2;
		t0 = time0.z;
	}

//	t0 = max (time0.x, max (time0.y, time0.z));
	t1 = min (time1.x, min (time1.y, time1.z));


	if (t0 > 1.0 ||
		t0 <= 0.0) {
		return false;
	}

	return t0 <= t1;
}



int physics_c::clipDisplacementAgainstOtherObjects (size_t index, WorldMap &worldMap) {

	int numCollisions = 0;

	bool collision = false;

	double t0, t1;
	int axis;

	if (obj[index].type == OBJTYPE_AI_ENTITY ||
		obj[index].type == OBJTYPE_PLAYER) {
		for (size_t other = 0; other < obj.size (); other++) {
			if (!obj[other].active ||
				obj[other].queued ||
				other == index ||
				obj[other].owner == obj[index].handle) continue;

			if (obj[other].type == OBJTYPE_AI_ENTITY) {
				if (sweepObjects (index, other, t0, t1, axis)) {
					// they're already touching
					if (t0 == 0.0) {
						// push them apart
						v3d_t difference = v3d_sub (obj[other].boundingBox.getCenterPosition (),
							obj[index].boundingBox.getCenterPosition ());
						
						difference = v3d_normalize (difference);
						difference = v3d_scale (120.0, difference);

						add_force (obj[other].handle, difference);
						add_force (obj[index].handle, v3d_neg (difference));
					}
					else {
						obj[index].displacement = v3d_scale (t0 * 0.99, obj[index].displacement);
						obj[other].displacement = v3d_scale (t0 * 0.99, obj[other].displacement);

						if (axis == 0) {
							double temp = obj[index].vel.x;
							obj[index].vel.x = obj[other].vel.x;
							obj[other].vel.x = temp;
						}
						else if (axis == 1) {
							if (obj[index].vel.y < 0.0 && obj[other].on_ground) {
								obj[index].vel.y = -obj[index].vel.y;
							}
							else if (obj[other].vel.y < 0.0 && obj[index].on_ground) {
								obj[other].vel.y = -obj[index].vel.y;
							}
							else {
								double temp = obj[index].vel.y;
								obj[index].vel.y = obj[other].vel.y;
								obj[other].vel.y = temp;
							}
						}
						else if (axis == 2) {
							double temp = obj[index].vel.z;
							obj[index].vel.z = obj[other].vel.z;
							obj[other].vel.z = temp;
						}

						// FIXME: what the heck is this?
//						if (v3d_mag (obj[index].vel) < EPSILON &&
//							v3d_mag (obj[other].vel) < EPSILON) {
//							obj[index].health = 0.0;
//						}

						// deal some damage
						// FIXME: hack: tiger needs to do melee attacks
						// this is ludicrous!
						if ((obj[index].aiType == AITYPE_HOPPER &&
							obj[index].health > 0.0) &&
							(obj[other].aiType != AITYPE_HOPPER ||
							obj[other].type == OBJTYPE_PLAYER)) {

							double damage = r_num (1.0, 8.0);
							obj[other].health -= damage;

							// tell the critter whodunnit
							phys_message_t message;

							message.sender = static_cast<int>(obj[index].handle);
							message.recipient = static_cast<int>(obj[other].handle);
							message.type = PHYS_MESSAGE_DAMAGE;
							message.dValue = damage;

							sendMessage (message);
						}
						else if ((obj[index].aiType != AITYPE_HOPPER ||
							obj[index].type == OBJTYPE_PLAYER) &&
							(obj[other].aiType == AITYPE_HOPPER &&
							obj[other].health > 0.0)) {

							double damage = r_num (1.0, 8.0);
							obj[index].health -= damage;

							// tell the critter whodunnit
							phys_message_t message;

							message.sender = static_cast<int>(obj[other].handle);
							message.recipient = static_cast<int>(obj[index].handle);
							message.type = PHYS_MESSAGE_DAMAGE;
							message.dValue = damage;

							sendMessage (message);
						}
					}
				}
			}
			else if (obj[other].type == OBJTYPE_PLAYER) {
				if (sweepObjects (index, other, t0, t1, axis)) {
					if (t0 == 0.0) {
						// push them apart
						v3d_t difference = v3d_sub (obj[other].boundingBox.getCenterPosition (),
							obj[index].boundingBox.getCenterPosition ());
						
						difference = v3d_normalize (difference);
						difference = v3d_scale (120.0, difference);

						add_force (obj[other].handle, difference);
						add_force (obj[index].handle, v3d_neg (difference));


					}
					else {
						obj[index].displacement = v3d_scale (t0 * 0.99, obj[index].displacement);
						obj[other].displacement = v3d_scale (t0 * 0.99, obj[other].displacement);

						if (axis == 0) {
							double temp = obj[index].vel.x;
							obj[index].vel.x = obj[other].vel.x;
							obj[other].vel.x = temp;
						}
						else if (axis == 1) {
							if (obj[index].vel.y < 0.0 && obj[other].on_ground) {
								obj[index].vel.y = -obj[index].vel.y;
							}
							else if (obj[other].vel.y < 0.0 && obj[index].on_ground) {
								obj[other].vel.y = -obj[index].vel.y;
							}
							else {
								double temp = obj[index].vel.y;
								obj[index].vel.y = obj[other].vel.y;
								obj[other].vel.y = temp;
							}
						}
						else if (axis == 2) {
							double temp = obj[index].vel.z;
							obj[index].vel.z = obj[other].vel.z;
							obj[other].vel.z = temp;
						}

						// FIXME: what the heck is this?
//						if (v3d_mag (obj[index].vel) < EPSILON &&
//							v3d_mag (obj[other].vel) < EPSILON) {
//							obj[index].health = 0.0;
//						}

						// deal some damage
//						obj[other].health -= r_num (1.0, 8.0);
					}
				}
			}
			else if (obj[other].type == OBJTYPE_SLIME ||
				obj[other].type == OBJTYPE_PLASMA ||
				obj[other].type == OBJTYPE_FIRE ||
				(obj[other].type == OBJTYPE_NAPALM && obj[other].applyPhysics) ||
				obj[other].type == OBJTYPE_SPARK ||
				obj[other].type == OBJTYPE_LIVE_BULLET ||
				obj[other].type == OBJTYPE_DEAD_BULLET) {
//				if (obj[other].boundingBox.isIntersecting (obj[index].boundingBox)) {

				if (sweepObjects (index, other, t0, t1, axis)) {
//					printf ("t0: %.2f, t1: %.2f\n", t0, t1);
//						spawnExplosion (obj[index].boundingBox.getCenterPosition (), mLastUpdateTime + PHYSICS_TIME_GRANULARITY, 100);
					
					// directly add knockback
					obj[index].force = v3d_add (obj[index].force, v3d_scale (obj[other].mass, obj[other].vel));

					if (r_numi (0, 5) == 3) {
						v3d_t position = v3d_add (obj[other].pos, v3d_scale (t0, obj[other].displacement));
//						printf ("BLOOD!\n");
						createEntity (OBJTYPE_BLOOD_SPRAY, position, mLastUpdateTime, false);
					}

					double damage = obj[other].impactDamage;
					obj[index].health -= damage;

					// tell the critter whodunnit
					phys_message_t message;

					message.sender = static_cast<int>(obj[other].owner);
					message.recipient = static_cast<int>(obj[index].handle);
					message.type = PHYS_MESSAGE_DAMAGE;
					message.dValue = damage;

					sendMessage (message);

					removeEntity (obj[other].handle);

					if (obj[index].type == AITYPE_HOPPER &&
						r_numi (0, 10) == 3) {
						addSoundEvent (SOUND_TIGER_ROAR, obj[other].pos);
					}

					collision = true;

					numCollisions++;
				}
			}
			else if (obj[other].type == OBJTYPE_GRENADE ||
					 obj[other].type == OBJTYPE_ROCKET) {
				if (sweepObjects (index, other, t0, t1, axis)) {
					expireEntity (other, mLastUpdateTime, worldMap);

					double damage = obj[other].impactDamage;
					obj[index].health -= damage;

					// tell the critter whodunnit
					phys_message_t message;

					message.sender = static_cast<int>(obj[other].owner);
					message.recipient = static_cast<int>(obj[index].handle);
					message.type = PHYS_MESSAGE_DAMAGE;
					message.dValue = damage;

					sendMessage (message);

					collision = true;

					numCollisions++;
				}
			}
			else if (obj[other].type == OBJTYPE_PLASMA_BOMB) {
				if (sweepObjects (index, other, t0, t1, axis)) {
					v3d_t position = v3d_add (obj[other].boundingBox.getCenterPosition (), v3d_scale (t0, obj[other].displacement));
					plasmaBombExplode (position, mLastUpdateTime + PHYSICS_TIME_GRANULARITY, 10, worldMap);

					obj[index].force = v3d_add (obj[index].force, v3d_scale (10.0 * obj[index].mass, obj[other].vel));

					for (int blood = 0; blood < 5; blood++) {
						v3d_t bloodForce = v3d_random (75.0);
						createEntity (OBJTYPE_BLOOD_SPRAY, position, bloodForce, mLastUpdateTime, true);
					}

					double damage = obj[other].impactDamage;
					obj[index].health -= damage;

					// tell the critter whodunnit
					phys_message_t message;

					message.sender = static_cast<int>(obj[other].owner);
					message.recipient = static_cast<int>(obj[index].handle);
					message.type = PHYS_MESSAGE_DAMAGE;
					message.dValue = damage;

					sendMessage (message);

					removeEntity (obj[other].handle);

					collision = true;

					numCollisions++;
				}
			}
			else if (obj[other].type == OBJTYPE_MELEE_ATTACK) {

				if (obj[other].boundingBox.isIntersecting (obj[index].boundingBox)) {
					v3d_t otherPosition = obj[other].boundingBox.getCenterPosition ();
					createEntity (OBJTYPE_BLOOD_SPRAY, otherPosition, mLastUpdateTime, false);

					double damage = obj[other].impactDamage;
					obj[index].health -= damage;

					// tell the critter whodunnit
					phys_message_t message;

					message.sender = static_cast<int>(obj[other].owner);
					message.recipient = static_cast<int>(obj[index].handle);
					message.type = PHYS_MESSAGE_DAMAGE;
					message.dValue = damage;

					sendMessage (message);

					// add the knockback
//					v3d_t knockBack = v3d_scale (50000.0, v3d_normalize (
//						v3d_sub (obj[index].boundingBox.getCenterPosition (), otherPosition)));

//					add_force (obj[index].handle, knockBack);
					add_force (obj[index].handle, obj[other].acc);

					addSoundEvent (SOUND_PUNCH, obj[other].boundingBox.getCenterPosition ());

					removeEntity (obj[other].handle);

					numCollisions++;
				}
			}




			else if (mGotPickupMessage &&
				obj[index].type == OBJTYPE_PLAYER &&
				obj[other].type == OBJTYPE_ITEM) {
				if (sweepObjects (index, other, t0, t1, axis)) {
					// try to pick up the object
					phys_message_t message;

					message.sender = MAILBOX_PHYSICS;
					message.recipient = static_cast<int>(obj[index].handle);
					message.type = PHYS_MESSAGE_ITEMGRAB;
					message.iValue = static_cast<int>(obj[other].itemHandle);
					message.iValue2 = obj[other].handle;

					sendMessage (message);

					collision = true;

					numCollisions++;
				}
			}
		}
	}
	else if (obj[index].type == OBJTYPE_EXPLOSION) {
		for (size_t other = 0; other < obj.size (); other++) {
			if (obj[other].active == false ||
				obj[index].queued ||
				other == index) continue;

			if (obj[other].type != OBJTYPE_EXPLOSION) {
				if (obj[other].boundingBox.isIntersecting (obj[index].boundingBox)) {
					if (obj[other].type == OBJTYPE_SMOKE ||
						obj[other].type == OBJTYPE_STEAM) {
						removeEntity (obj[other].handle);
					}
					else {
						v3d_t dim = obj[other].boundingBox.getDimensions ();
						
						double strength = (dim.x * dim.x) + (dim.y * dim.y) + (dim.y * dim.y);

						add_force (obj[other].handle, getRadialForce (obj[other].boundingBox.getCenterPosition (),
							obj[index].boundingBox.getCenterPosition (), obj[index].explosionForce * strength, 10.0));
					}

					numCollisions++;
				}
			}
		}
	}


	

	// reset this
	mGotPickupMessage = false;

	return 0;

} // END: clipDisplacementAgainstOtherObjects ()



void physics_c::add_force (size_t handle, v3d_t force) {
	int index = getIndexFromHandle (handle);

	if (index != -1) {
		obj[index].force = v3d_add (obj[index].force, force);
		obj[index].applyPhysics = true;
	}
}



void physics_c::clip_displacement_against_world (WorldMap &worldMap, size_t index) {
	if (v3d_mag (obj[index].displacement) == 0.0) {
		return;
	}

	int solidityType;

	v3d_t nc = obj[index].boundingBox.getNearCorner();
	v3d_t fc = obj[index].boundingBox.getFarCorner();

	// bounding box for the world block we'll be clipping against
	BoundingBox bb2(v3d_v (1.0, 1.0, 1.0));

	v3di_t i;
	v3d_t time0;

	int y_start;

	if (obj[index].on_ground) y_start = static_cast<int>(floor (nc.y));
	else y_start = static_cast<int>(floor (nc.y - 1.0));

	if (obj[index].type == OBJTYPE_PLAYER) {
//		printf ("y_start: %d\n", y_start);
	}

	// FIXME: this is a horribly inefficient way of doing this
	// it also fails if the velocity is too high
	// your mom
	for (i.z = static_cast<int>(floor (nc.z - 1.0)); i.z <= static_cast<int>(floor (fc.z + 1.0)); i.z++) {
		for (i.y = y_start; i.y <= static_cast<int>(floor (fc.y + 1.0)); i.y++) {
			for (i.x = static_cast<int>(floor (nc.x - 1.0)); i.x <= static_cast<int>(floor (fc.x + 1.0)); i.x++) {
				block_t *block = worldMap.getBlock (i);
				if (block == NULL) {
					continue;
				}

				solidityType = gBlockData.get(block->type)->solidityType;

				// check if solid
				if (solidityType == BLOCK_SOLIDITY_TYPE_AIR ||
					solidityType == BLOCK_SOLIDITY_TYPE_LIQUID ||
					solidityType == BLOCK_SOLIDITY_TYPE_PLANT) continue;

//				if (block.type > BLOCK_TYPE_AIR &&
//					block.type < NUM_BLOCK_TYPES &&
//					block.type != globalWaterBlockType) {

				// FIXME: sort this mess out!
				{
					bb2.setPosition (v3d_v (i));

					bool test_x;
					bool test_y;
					bool test_z;


					if (obj[index].boundingBox.isIntersecting (bb2)) {
						if (nc.x == bb2.mFarCorner.x && obj[index].displacement.x < 0.0) {
							obj[index].displacement.x = 0.0;
						}
						if (fc.x == bb2.mNearCorner.x && obj[index].displacement.x > 0.0) {
							obj[index].displacement.x = 0.0;
						}
						if (nc.y == bb2.mFarCorner.y && obj[index].displacement.y < 0.0) {
							obj[index].displacement.y = 0.0;
						}
						if (fc.y == bb2.mNearCorner.y && obj[index].displacement.y > 0.0) {
							obj[index].displacement.y = 0.0;
						}
						if (nc.z == bb2.mFarCorner.z && obj[index].displacement.z < 0.0) {
							obj[index].displacement.z = 0.0;
						}
						if (fc.z == bb2.mNearCorner.z && obj[index].displacement.z > 0.0) {
							obj[index].displacement.z = 0.0;
						}
					}


					// x-axis
					if (obj[index].displacement.x < 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
							test_x = true; }
						else {
							test_x = false; } }
					else if (obj[index].displacement.x > 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
							test_x = true; }
						else {
							test_x = false; } }
					else {
						test_x = false; }

					// y-axis
					if (obj[index].displacement.y < 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
							test_y = true; }
						else {
							test_y = false; } }
					else if (obj[index].displacement.y > 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
							test_y = true; }
						else {
							test_y = false; } }
					else {
						test_y = false; }

					// z-axis
					if (obj[index].displacement.z > 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
							test_z = true; }
						else {
							test_z = false; } }
					else if (obj[index].displacement.z < 0.0) {
						if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
							test_z = true; }
						else {
							test_z = false; } }
					else {
						test_z = false; }

//					test_x = test_y = test_z = true;

//					if (obj[index].type == OBJTYPE_SLIME) {
//						printf ("test %d, %d, %d\n", test_x, test_y, test_z);
//					}

//					if (obj[index].boundingBox.clipDisplacement (test_x,
//													test_y, test_z, displacement, bb2)) {


					if ((test_x || test_y || test_z) &&
						obj[index].boundingBox.sweepTest (bb2, obj[index].displacement, time0))
					{
						double t0 = 0.0;

						int axis = -1;

						// figure out when the collision occurred
						if (test_x) {
							t0 = time0.x;
							axis = 0;
						}
						if (test_y && time0.y > t0) {
							t0 = time0.y;
							axis = 1;
						}
						if (test_z && time0.z > t0) {
							t0 = time0.z;
							axis = 2;
						}


//						if (obj[index].type == OBJTYPE_SLIME) {
//							printf ("test %d, %d, %d\n", test_x, test_y, test_z);
//							printf ("time %f, %f, %f\n", time0.x, time0.y, time0.z);
//							printf ("axis: %d\n", axis);
		//				}


						// no sliding
						if (obj[index].type == OBJTYPE_PLASMA_BOMB ||
							obj[index].type == OBJTYPE_NAPALM)
						{
							obj[index].displacement = v3d_scale (t0, obj[index].displacement);
						}
						// sliding
						else {
							if (axis == 0) {
								obj[index].displacement.x = t0 * obj[index].displacement.x;
							}
							else if (axis == 1) {
								obj[index].displacement.y = t0 * obj[index].displacement.y;
							}
							else if (axis == 2) {
								obj[index].displacement.z = t0 * obj[index].displacement.z;
							}
						}


						switch (obj[index].type) {
							case OBJTYPE_SPARK:
								if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
									createEntity (OBJTYPE_STEAM, obj[index].pos, mLastUpdateTime, false);
									obj[index].expirationTime = mLastUpdateTime;
								}
								break;

							case OBJTYPE_NAPALM:
								if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
									createEntity (OBJTYPE_STEAM, obj[index].pos, mLastUpdateTime, false);
									obj[index].expirationTime = mLastUpdateTime;
								}
								else {
									obj[index].vel = v3d_zero ();
									obj[index].applyPhysics = false;
								}
								break;

							case OBJTYPE_LIVE_BULLET:
							case OBJTYPE_DEAD_BULLET:
								if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
									createEntity (OBJTYPE_STEAM, obj[index].pos, mLastUpdateTime, false);
									obj[index].expirationTime = 0.1;
								}
								else if (block->type == BLOCK_TYPE_DIRT ||
									block->type == BLOCK_TYPE_DIRT_GRASS ||
									block->type == BLOCK_TYPE_DIRT_SNOW ||
									block->type == BLOCK_TYPE_GRASS ||
									block->type == BLOCK_TYPE_SNOW ||
									block->type == BLOCK_TYPE_CACTUS ||
									block->type == BLOCK_TYPE_TRUNK ||
									block->type == BLOCK_TYPE_LEAVES ||
									block->type == BLOCK_TYPE_SAND)
								{
									createEntity (OBJTYPE_STEAM, obj[index].pos, mLastUpdateTime, false);
									obj[index].expirationTime = 0.1;
//									obj[index].vel = v3d_zero ();
//									obj[index].applyPhysics = false;
								}
								break;

							case OBJTYPE_ROCKET:
							case OBJTYPE_PLASMA:
							case OBJTYPE_PLASMA_BOMB:
								obj[index].expirationTime = 0.1;
								break;

							case OBJTYPE_SLIME:
							case OBJTYPE_BLOOD_SPRAY:
//								obj[index].vel = v3d_zero ();
//								obj[index].applyPhysics = false;
								break;

							case OBJTYPE_CREATE:
//								if (i.y > 0) {
//									v3di_t bleh = v3di_v (obj[index].pos);
//									block_t b;
//									b.type = 1;
//								}
								break;

							case OBJTYPE_DESTROY:
								worldMap.clearBlock(i);
								obj[index].expirationTime = mLastUpdateTime;
								break;

							default:
								break;
						} // switch
					} // collision
				} // block_type air/water
			} // x
		} // y
	} // z
}


bool physics_c::isIntersectingPlant(size_t index, WorldMap &worldMap) {
	block_t *block = worldMap.getBlock(obj[index].boundingBox.getCenterPosition());
	if (block == NULL) return false;
	if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
		return true;
	}

	return false;
}


// return the on_ground flag for an object
bool physics_c::isIndexOnGround (size_t index) {
	return obj[index].on_ground;
}



// return the on_ground flag for an object
bool physics_c::isHandleOnGround (size_t handle) {
	size_t index = getIndexFromHandle (handle);

	if (index != -1) {
		return obj[index].on_ground;
	}

	return false;
}


void physics_c::grenadeExplosion (size_t index, double time, WorldMap &worldMap) {
	v3d_t centerPosition = obj[index].boundingBox.getCenterPosition ();

	addSoundEvent (SOUND_GRENADE_EXPLOSION, centerPosition);
	
	for (int i = 0; i < obj[index].numParticles; i++) {
		v3d_t randomPosition = v3d_add (centerPosition, v3d_random (0.5));

		size_t shrapnelHandle = createEntity (obj[index].type2, randomPosition, time, true);

		int shrapnelIndex = getIndexFromHandle (shrapnelHandle);

		if (shrapnelIndex > 0) {
			obj[shrapnelIndex].owner = obj[index].owner;
			obj[shrapnelIndex].impactDamage = obj[index].impactDamage;
		}
	}


	if (obj[index].explosionForce > 2500.0) {
		double clearRadius = (obj[index].explosionForce - 2500.0) * 0.001;

		worldMap.clearSphere (centerPosition, clearRadius);
	}

	size_t explosionHandle = createEntity (OBJTYPE_EXPLOSION, centerPosition, time, true);

	// FIXME: hack!
	if (explosionHandle > 0) {
		int explosionIndex = getIndexFromHandle (explosionHandle);

		obj[explosionIndex].owner = obj[index].owner;
		obj[explosionIndex].applyPhysics = false;
		obj[explosionIndex].explosionForce = obj[index].explosionForce;
	}
}




void physics_c::spawnExplosion (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap) {
	v3d_t new_pos;

	new_pos.x = pos.x;
	new_pos.y = pos.y + 0.05;
	new_pos.z = pos.z;

	addSoundEvent (SOUND_GRENADE_EXPLOSION, pos);

	for (size_t i = 0; i < numParticles - 10; i++) {

		new_pos.x = r_num (-10.0, 10.0);
		new_pos.y = r_num (-10.0, 10.0);
		new_pos.z = r_num (-10.0, 10.0);

		//v3d_t force = v3d_v (r_num (-1.0, 1.0), r_num (-1.0, 1.0), r_num (-1.0, 1.0));
		//force = v3d_scale (r_num (1000.0, 1200.0), v3d_normalize (force));

		//createEntity (OBJTYPE_SPARK, new_pos, force, time, true);

		createEntity (OBJTYPE_SPARK,
			v3d_add (pos, v3d_scale (r_num (0.5, 0.6), v3d_normalize (new_pos))), time, true);
	}


	int num_napalms = r_numi (5, 10);

	for (size_t i = 0; i < static_cast<size_t>(num_napalms); i++) {
//		v3d_t force = v3d_v (r_num (-1.0, 1.0), r_num (-1.0, 1.0), r_num (-1.0, 1.0));
//		force = v3d_scale (r_num (800.0, 900.0), v3d_normalize (force));

//		createEntity (OBJTYPE_NAPALM, new_pos, force, time, true);

		new_pos.x = r_num (-10.0, 10.0);
		new_pos.y = r_num (-10.0, 10.0);
		new_pos.z = r_num (-10.0, 10.0);

		//v3d_t force = v3d_v (r_num (-1.0, 1.0), r_num (-1.0, 1.0), r_num (-1.0, 1.0));
		//force = v3d_scale (r_num (1000.0, 1200.0), v3d_normalize (force));

		//createEntity (OBJTYPE_SPARK, new_pos, force, time, true);

		createEntity (OBJTYPE_NAPALM,
			v3d_add (pos, v3d_scale (r_num (0.5, 0.6), v3d_normalize (new_pos))), time, true);

	}

	worldMap.clearSphere (pos, 2.5);

	size_t handle = createEntity (OBJTYPE_EXPLOSION, pos, time, true);
	// FIXME: hack!
	if (handle != 0) {
		int index = getIndexFromHandle (handle);

		obj[index].applyPhysics = false;
		obj[index].explosionForce = 5000.0;
	}
}



void physics_c::spawnMeatExplosion (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap) {
	v3d_t new_pos;

	new_pos.x = pos.x;
	new_pos.y = pos.y + 0.05;
	new_pos.z = pos.z;

	addSoundEvent (SOUND_SPLAT, pos);

	for (size_t i = 0; i < numParticles - 10; i++) {

		new_pos.x = r_num (-10.0, 10.0);
		new_pos.y = r_num (-10.0, 10.0);
		new_pos.z = r_num (-10.0, 10.0);

		//v3d_t force = v3d_v (r_num (-1.0, 1.0), r_num (-1.0, 1.0), r_num (-1.0, 1.0));
		//force = v3d_scale (r_num (1000.0, 1200.0), v3d_normalize (force));

		//createEntity (OBJTYPE_SPARK, new_pos, force, time, true);

		createEntity (OBJTYPE_BLOOD_SPRAY,
			v3d_add (pos, v3d_scale (r_num (0.5, 0.6), v3d_normalize (new_pos))), time, true);
	}

	size_t handle = createEntity(OBJTYPE_EXPLOSION, pos, time, true);
	// FIXME: hack!
	if (handle != 0) {
		int index = getIndexFromHandle (handle);

		obj[index].applyPhysics = false;
		obj[index].explosionForce = 250.0;
	}
}



void physics_c::plasmaBombExplode (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap) {
	v3d_t new_pos;

	new_pos.x = pos.x;
	new_pos.y = pos.y + 0.05;
	new_pos.z = pos.z;


	for (size_t i = 0; i < numParticles; i++) {

		new_pos.x = r_num (-10.0, 10.0);
		new_pos.y = r_num (-10.0, 10.0);
		new_pos.z = r_num (-10.0, 10.0);

		//v3d_t force = v3d_v (r_num (-1.0, 1.0), r_num (-1.0, 1.0), r_num (-1.0, 1.0));
		//force = v3d_scale (r_num (1000.0, 1200.0), v3d_normalize (force));

		//createEntity (OBJTYPE_SPARK, new_pos, force, time, true);

		size_t handle = createEntity (OBJTYPE_PLASMA_SPARK, pos, time, true);
//			v3d_add (pos, v3d_scale (r_num (0.5, 0.6), v3d_normalize (new_pos))), time, true);

		if (handle != 0) {
			int index = getIndexFromHandle (handle);
			if (!worldMap.isBoundingBoxEmpty (obj[index].boundingBox)) {
				obj[index].active = false;
			}
			else {
				obj[index].boundingBox.setDimensions (v3d_scale (r_num (2.0, 5.0),
					obj[index].boundingBox.getDimensions ()));

				// FIXME: is this safe?
				obj[index].expirationTime += r_num (0.2, 0.4);

				v3d_t randomVel = v3d_random (r_num (0.2, 1.0));

				obj[index].vel = randomVel;
			}
		}

	}
/*
//	clearSphere (pos, 0.5, world);

	size_t handle = createEntity (OBJTYPE_EXPLOSION, pos, time, true);
	// FIXME: hack!
	if (handle != 0) {
		int index = getIndexFromHandle (handle);

		obj[index].applyPhysics = false;
		obj[index].explosionForce = 200.0;
		obj[index].boundingBox.setDimensions (v3d_v (1.0, 1.0, 1.0));
	}
	*/
}





v3d_t physics_c::getRadialForce (v3d_t pos, v3d_t center, double force, double radius) {
	v3d_t diff = v3d_sub (pos, center);

	double dist = v3d_mag (diff);

	if (dist > radius || dist == 0.0) return v3d_zero ();

	if (dist < 0.1) {
		dist = 0.1;
	}

	double magnitude = force / (dist * dist);

	return v3d_scale (magnitude, v3d_normalize (diff));
}





int physics_c::addSoundEvent (int type, v3d_t position) {
	double distanceToPlayer = v3d_dist (getCenter (mPlayerHandle), position);

	if (mNumSoundEvents < MAX_SOUND_EVENTS) {
		int volume;

		if (distanceToPlayer >= 8.0) {
			volume = static_cast<int>(floor (96.0 * (1.0 / (distanceToPlayer - 7.0))));
		}
		else {
			volume = 96;
		}

//		printf ("distance: %.2f\n", distanceToPlayer);
//		printf ("volume: %d\n", mSoundVolumes[mNumSoundEvents]);

		if (volume > 0) {
			mSoundEvents[mNumSoundEvents] = type;
			mSoundVolumes[mNumSoundEvents] = volume;
			mNumSoundEvents++;
		}

		return 0;
	}

	return 1;
}



void physics_c::playSoundEvents (AssetManager &assetManager) {
	for (int i = 0; i < mNumSoundEvents; i++) {
		assetManager.mSoundSystem.playSoundByHandle (mSoundEvents[i], mSoundVolumes[i]);
	}
	
	if (mNumSoundEvents > 0) {
//		printf ("sound events: %d\n", mNumSoundEvents);
	}

	mNumSoundEvents = 0;
}



size_t physics_c::getPlayerHandle (void) {
	return mPlayerHandle;
}



void physics_c::sendMessage (phys_message_t message) {
	mMessages.push_back (message);
}



int physics_c::getNextMessage (int recipient, phys_message_t *message) {
	for (size_t i = 0; i < mMessages.size (); i++) {
		if (mMessages[i].recipient == recipient) {
			*message = mMessages[i];

			size_t lastMessage = mMessages.size () - 1;

			// get rid of this message
			if (i == lastMessage) {
				mMessages.pop_back ();
			}
			else {
				swap (mMessages[i], mMessages[lastMessage]);
				mMessages.pop_back ();
				i--;
			}

			return 1;
		}
	}

	return 0;
}



void physics_c::clearMailBox (int recipient) {
	for (size_t i = 0; i < mMessages.size (); i++) {
		if (mMessages[i].recipient == recipient) {
			size_t lastMessage = mMessages.size () - 1;

			// get rid of this message
			if (i == lastMessage) {
				mMessages.pop_back ();
			}
			else {
				swap (mMessages[i], mMessages[lastMessage]);
				mMessages.pop_back ();
				i--;
			}
		}
	}
}


vector<size_t> physics_c::getAllItemHandles(void) {
	vector<size_t> itemList;

	for (size_t i = 0; i < obj.size(); i++) {
		if (obj[i].type == OBJTYPE_ITEM) {
			itemList.push_back(obj[i].itemHandle);
		}
	}

	return itemList;
}


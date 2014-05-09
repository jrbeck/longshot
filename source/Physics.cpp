#include "Physics.h"


Physics::Physics() {

  // * * * * * * * * * * * * * * * * * * *
  // lmao...this was getting a bit tedious
  // FIXME: but seriously, this is disgusting, and needs to
  // be addressed...
#include "phys_entity_types.hack"
  // * * * * * * * * * * * * * * * * * * *
  // * * * * * * * * * * * * * * * * * * *

  reset();
}



Physics::~Physics() {
  clear();
}


void Physics::reset() {
  clear();

  mLastUpdateTime = 0.0;

  mPaused = false;
  mAdvanceOneFrame = false;

  mGravity = DEFAULT_GRAVITY;
  mFriction = DEFAULT_FRICTION;

  mNumSoundEvents = 0;
}


void Physics::clear() {
  obj.clear();

  mLastEntityHandle = 0;
  mEntityAdded = false;

  mPlayerHandle = 0;

  mMessages.clear();
  mGotPickupMessage = false;
}


vector <PhysicsEntity*>* Physics::getEntityVector() {
  return &obj;
}


void Physics::loadInactiveList() {
//	mInactiveList.load( "save/inactive.list" );
}


int Physics::getIndexFromHandle(size_t handle) const {
  size_t numObjs = obj.size();
  for (size_t index = 0; index < numObjs; index++) {
    if (obj[index]->handle == handle) {
      return (int)index;
    }
  }

  // failed to find the object
  return -1;
}


void Physics::togglePause() {
  mPaused = !mPaused;
}


bool Physics::isPaused() const {
  return mPaused;
}


void Physics::advanceOneFrame() {
  mAdvanceOneFrame = true;
}


void Physics::addQueuedEntities() {
  if (!mEntityAdded) {
    return;
  }

  size_t numObjs = obj.size();
  for (size_t i = 0; i < numObjs; i++) {
    obj[i]->queued = false;
  }

  mEntityAdded = false;
}


void Physics::manageEntitiesList() {
  size_t numObjs = obj.size();
  for (size_t i = 0; i < numObjs; i++) {
    if (!obj[i]->active) {
      // clear out the ol' mailbox
      clearMailBox((int)obj[i]->handle);
      delete obj[i];
      size_t lastIndex = numObjs - 1;
      if (i == lastIndex) { obj.pop_back(); }
      else {
        swap(obj[i], obj[lastIndex]);
        obj.pop_back();
        i--;
        numObjs--;
      }
    }
  }
}

size_t Physics::createEntity(int type, const v3d_t& position, double time, bool center) {
  PhysicsEntity *e = NULL;
  e = new PhysicsEntity;
  if (e == NULL) {
    return 0;
  }
  size_t size_before = obj.size();
  obj.push_back(e);
  if (obj.size() == size_before) {
    delete e;
    return 0;
  }

  e->handle = ++mLastEntityHandle;

  e->queued = true;
  e->active = true;

  e->type = type;
  if (type == OBJTYPE_PLAYER) {
    e->aiType = AITYPE_PLAYER;
    mPlayerHandle = e->handle;
  }
  
  e->force = v3d_zero();
  e->vel = v3d_zero();
  e->displacement = v3d_zero();

  e->on_ground = false;
  e->worldViscosity = 0.0;

  e->applyPhysics = true;

  // MASS
  e->mass = r_num(mEntityTypeInfo[type].massLow, mEntityTypeInfo[type].massHigh);
  e->one_over_mass = 1.0 / e->mass;

  // LIFESPAN
  e->lifespan = r_num(mEntityTypeInfo[type].lifespanLow, mEntityTypeInfo[type].lifespanHigh);
  if (e->lifespan >= 0.0) {
    e->expirationTime = time + e->lifespan;
  }
  else {
    e->expirationTime = -1.0;
  }

  // COLOR
  e->baseColor[0] = e->color[0] = mEntityTypeInfo[type].color[0];
  e->baseColor[1] = e->color[1] = mEntityTypeInfo[type].color[1];
  e->baseColor[2] = e->color[2] = mEntityTypeInfo[type].color[2];
  e->baseColor[3] = e->color[3] = mEntityTypeInfo[type].color[3];

  // DIMENSIONS
  v3d_t dimensions;

  dimensions.x = r_num(mEntityTypeInfo[type].dimensionXLow, mEntityTypeInfo[type].dimensionXHigh);
  dimensions.y = r_num(mEntityTypeInfo[type].dimensionYLow, mEntityTypeInfo[type].dimensionYHigh);
  dimensions.z = r_num(mEntityTypeInfo[type].dimensionZLow, mEntityTypeInfo[type].dimensionZHigh);

  e->boundingBox.setDimensions(dimensions);

  // do we want the given position to be the center of the box?
  if (center) {
    e->boundingBox.setCenterPosition(position);
  }
  else {
    e->boundingBox.setPosition(position);
  }

  e->pos = e->boundingBox.getNearCorner();

  e->explosionForce = 0.0;

  mEntityAdded = true;
  return e->handle;
}

size_t Physics::createEntity(int type, const v3d_t& position, const v3d_t& initialForce, double time, bool center) {
  size_t handle = createEntity(type, position, time, center);
  if (handle != 0) {
    int index = getIndexFromHandle(handle);
    obj[index]->force = initialForce;
  }
  return handle;
}

size_t Physics::createAiEntity(int aiType, const v3d_t& position, double time) {
  size_t handle = createEntity(OBJTYPE_AI_ENTITY, position, time, true);
  if (handle != 0) {
    int index = getIndexFromHandle(handle);
    obj[index]->aiType = aiType;
  }
  return handle;
}

void Physics::removeEntity(size_t handle) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    obj[index]->active = false;
  }
}

void Physics::setMass(size_t handle, double mass) {
  int index = getIndexFromHandle (handle);
  if (index != -1) {
    obj[index]->mass = mass;
    obj[index]->one_over_mass = 1.0 / mass;
  }
}

void Physics::setHealth(size_t handle, double health) {
  int index = getIndexFromHandle (handle);
  if (index != -1) {
    obj[index]->health = health;
  }
}

void Physics::setDimensions(size_t handle, const v3d_t& dimensions) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    obj[index]->boundingBox.setDimensions(dimensions);
  }
}

PhysicsEntity* Physics::getEntityByHandle(size_t handle) {
  int index = getIndexFromHandle(handle);
  if (index >= 0) {
    return obj[index];
  }
  return NULL;
}

// WARNING: this whole thing seems a little fishy...
// who calls this and why?
void Physics::setEntity(PhysicsEntity* entity) {
  int index = getIndexFromHandle(entity->handle);
  if (index != -1) {
    delete obj[index];
    obj[index] = entity;
  }
  else {
    printf ("Physics::setEntity (): failed to set entity\n");
  }
}

v3d_t Physics::getCenter(size_t handle) const {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    return obj[index]->boundingBox.getCenterPosition();
  }
  return v3d_zero();
}

v3d_t Physics::getNearCorner(size_t handle) const {
  int index = getIndexFromHandle (handle);
  if (index != -1) {
    return obj[index]->pos;
  }
  return v3d_zero ();
}

v3d_t Physics::getFarCorner(size_t handle) const {
  int index = getIndexFromHandle (handle);
  if (index != -1) {
    return obj[index]->boundingBox.getFarCorner();
  }
  return v3d_zero ();
}

v3d_t Physics::getVelocity(size_t handle) const {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    return obj[index]->vel;
  }
  return v3d_zero ();
}

void Physics::setVelocity(size_t handle, const v3d_t& velocity) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    obj[index]->vel = velocity;
  }
}

void Physics::set_pos(size_t handle, const v3d_t& pos) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    // eww... pos and the bounding box position both exist?
    obj[index]->pos = pos;
    // vel?
    obj[index]->vel = v3d_zero();
    obj[index]->boundingBox.setPosition(pos);
  }
}

void Physics::setOwner(size_t handle, size_t owner) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    obj[index]->owner = owner;
  }
}

// this is for ItemManager items - get the handle and the type
void Physics::setItemHandleAndType(size_t handle, size_t itemHandle, int itemType) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    obj[index]->itemHandle = itemHandle;
    obj[index]->type2 = itemType;
  }
}

// apply the physics
int Physics::update(double time, WorldMap& worldMap, AssetManager& assetManager) {
  readMail();
  bool timeAdvanced;
  if (!mPaused || (mPaused && mAdvanceOneFrame)) {
    timeAdvanced = true;
  }
  else {
    timeAdvanced = false;
  }

  // step through the objects
  size_t numEntities = obj.size();
  for (size_t index = 0; index < numEntities; index++) {
    if (obj[index]->active == false) continue;

    if (mPaused && !timeAdvanced && (obj[index]->expirationTime > 0.0)) {
      obj[index]->expirationTime += PHYSICS_TIME_GRANULARITY;
    }
      
    if (obj[index]->queued) continue;

    if (!mPaused || (mAdvanceOneFrame || obj[index]->handle == mPlayerHandle)) {
      // update that mofar
      updateEntity(index, time, worldMap);
    }

    if (obj[index]->applyPhysics == false) {
      obj[index]->vel = v3d_zero();
    }
  }
  // now clip against the other objects
  for (size_t index = 0; index < numEntities; index++) {
    if (obj[index]->active == false || obj[index]->queued) {
      continue;
    }

    if (!mPaused || (mAdvanceOneFrame || obj[index]->handle == mPlayerHandle)) {
      clipDisplacementAgainstOtherObjects(index, worldMap);
    }
  }
  // now we can move them
  for (size_t index = 0; index < numEntities; index++) {
    if (obj[index]->active == false || obj[index]->queued) {
      continue;
    }

    if (!mPaused || (mAdvanceOneFrame || obj[index]->handle == mPlayerHandle)) {
      // now move that shizzat
      displaceObject(index, worldMap);
    }
  }

  // clean up the object list if possible
  manageEntitiesList();
  mAdvanceOneFrame = false;
  mLastUpdateTime = time;

  if (timeAdvanced) {
    addQueuedEntities();
  }

  // play the sounds
  playSoundEvents(assetManager);

  return static_cast<int>(obj.size());
}

void Physics::updateEntity(size_t index, double time, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];

  if ((physicsEntity->expirationTime < time) && (obj[index]->expirationTime >= 0.0)) {
    expireEntity(index, time, worldMap);
    return;
  }

  // kill off the things that die in liquid
  if (physicsEntity->worldViscosity > 0.0 &&
    (physicsEntity->type == OBJTYPE_SMOKE ||
     physicsEntity->type == OBJTYPE_NAPALM ||
     physicsEntity->type == OBJTYPE_FIRE ||
     physicsEntity->type == OBJTYPE_SPARK))
  {
    expireEntity(index, time, worldMap);
    return;
  }


  // handle plant interaction (arnie!)
  if (physicsEntity->type == OBJTYPE_SLIME ||
    physicsEntity->type == OBJTYPE_NAPALM ||
    physicsEntity->type == OBJTYPE_PLASMA ||
    physicsEntity->type == OBJTYPE_FIRE ||
    physicsEntity->type == OBJTYPE_LIVE_BULLET ||
    physicsEntity->type == OBJTYPE_PLASMA_BOMB)
  {
    if (r_numi(0, 10) > 4 &&
      v3d_mag(physicsEntity->vel) > 7.0 &&
      isIntersectingPlant(index, worldMap))
    {
      v3di_t pos = v3di_v(physicsEntity->boundingBox.getCenterPosition());
      worldMap.clearBlock(pos);
      expireEntity(index, time, worldMap);
      return;
    }
  }


  // deal with AI
  if (physicsEntity->type == OBJTYPE_AI_ENTITY) {
    // check if AI is dead
    if (physicsEntity->health < -5.0) {
      expireEntity (index, time, worldMap);
      return;
    }
  }

  // kill it if it's fallen too far
  // FIXME: hack, really
  if (physicsEntity->type != OBJTYPE_PLAYER && physicsEntity->pos.y < SEA_FLOOR_HEIGHT - 1000.0) {
    expireEntity(index, time, worldMap);
    return;
  }

  // get the acceleration from the forces at work
  physicsEntity->acc = calculateAcceleration (index);

  if (physicsEntity->applyPhysics) {
    integrate_euler (index, time, worldMap);
  }

  size_t otherHandle;
  int otherIndex;
  int rnum = r_numi (2, 3);

  // FIXME: this is a stupid place for this...
  switch (physicsEntity->type) {
    case OBJTYPE_NAPALM:
      if (r_numi(0, 100) == 0) {
        otherHandle = createEntity(OBJTYPE_FIRE, physicsEntity->pos, time, false);
        otherIndex = getIndexFromHandle(otherHandle);
        if (otherIndex >= 0) {
          obj[otherIndex]->owner = physicsEntity->owner;
          obj[otherIndex]->impactDamage = physicsEntity->impactDamage;
        }
      }
      break;

    case OBJTYPE_SMOKE:
      physicsEntity->boundingBox.scale(1.009);
      break;

    case OBJTYPE_DEAD_BULLET:
    case OBJTYPE_LIVE_BULLET:
      if (r_numi(0, 100) == 0) {
        otherHandle = createEntity (OBJTYPE_STEAM, physicsEntity->pos, time, false);
        otherIndex = getIndexFromHandle (otherHandle);
        if (otherIndex >= 0) {
          obj[otherIndex]->color[3] = 0.2f;
        }
      }
      break;

    case OBJTYPE_ROCKET:
      if (r_numi (0, 4) == 0) {
        otherHandle = createEntity (OBJTYPE_FIRE, physicsEntity->pos, time, false);
        otherIndex = getIndexFromHandle (otherHandle);
        if (otherIndex >= 0) {
          obj[otherIndex]->owner = physicsEntity->owner;
          obj[otherIndex]->impactDamage = physicsEntity->impactDamage;
        }
      }
      break;

    case OBJTYPE_PLASMA_BOMB:
      for (int i = 0; i < rnum; i++) {
        otherHandle = createEntity(OBJTYPE_PLASMA_SPARK, v3d_add(v3d_random(0.20), physicsEntity->boundingBox.getCenterPosition()), time, true);
        setVelocity(otherHandle, v3d_random(0.5));
      }
      createEntity(OBJTYPE_PLASMA_TRAIL, physicsEntity->boundingBox.getCenterPosition(), time, true);
      break;

    case OBJTYPE_AI_ENTITY:
      physicsEntity->health += worldMap.getHealthEffects(physicsEntity->boundingBox);
      break;

    case OBJTYPE_PLAYER:
      physicsEntity->health += worldMap.getHealthEffects (physicsEntity->boundingBox);
      break;

    default:
      break;
  }
}

void Physics::expireEntity(size_t index, double time, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];
  size_t newHandle, newIndex;
  v3d_t pos;
  bool removeEntity = true;

  switch (physicsEntity->type) {
    case OBJTYPE_FIRE:
      newHandle = createEntity(OBJTYPE_SMOKE, physicsEntity->pos, time, false);

      if (newHandle != 0) {
        newIndex = getIndexFromHandle(newHandle);
        PhysicsEntity *newPhysicsEntity = obj[newIndex];
        newPhysicsEntity->boundingBox.setDimensions(physicsEntity->boundingBox.getDimensions());

        newPhysicsEntity->vel.x = r_num(-0.2, 0.2);
        newPhysicsEntity->vel.y = physicsEntity->vel.y * 0.6;
        newPhysicsEntity->vel.z = r_num(-0.2, 0.2);
      }

      break;

    case OBJTYPE_EXPLOSION:
      pos = physicsEntity->boundingBox.getCenterPosition();
      v3d_t force;
      for (size_t i = 0; i < 10; i++) {
        force = v3d_v(r_num(-5., 5.0), r_num(-5.0, 5.0), r_num(-5.0, 5.0));
        createEntity(OBJTYPE_SMOKE, pos, force, time, false);
      }
      break;

    case OBJTYPE_ROCKET:
    case OBJTYPE_GRENADE:
      grenadeExplosion(index, time, worldMap);
      break;

    case OBJTYPE_PLASMA:
      plasmaBombExplode (physicsEntity->boundingBox.getCenterPosition(), time, 5, worldMap);
      break;

    case OBJTYPE_PLASMA_BOMB:
      plasmaBombExplode (physicsEntity->boundingBox.getCenterPosition(), time, 30, worldMap);
      break;

    case OBJTYPE_AI_ENTITY:
      if (physicsEntity->health <= 0.0) {
        spawnMeatExplosion(physicsEntity->boundingBox.getCenterPosition(), time + PHYSICS_TIME_GRANULARITY, 15, worldMap);
      }
      break;

    case OBJTYPE_ITEM:
      phys_message_t message;
      message.recipient = MAILBOX_ITEMMANAGER;
      message.type = PHYS_MESSAGE_ITEM_DESTROYED;
      message.iValue = static_cast<int>(physicsEntity->itemHandle);
      sendMessage(message);
      break;

    case OBJTYPE_MONSTER_SPAWNER: // what a hack!
      pos = physicsEntity->boundingBox.getCenterPosition();

      // check if close enough to player
      if (v3d_dist(pos, getEntityByHandle(mPlayerHandle)->pos) < 100.0 &&
        !worldMap.lineOfSight(pos, getEntityByHandle(mPlayerHandle)->boundingBox.getCenterPosition()))
      {
        phys_message_t message;
        message.sender = MAILBOX_PHYSICS;
        message.recipient = MAILBOX_AIMANAGER;
        message.type = PHYS_MESSAGE_SPAWN_CREATURE;
        pos.y += 1.5;
        message.vec3 = pos;
        sendMessage(message);
      }

      physicsEntity->expirationTime += physicsEntity->lifespan;
      removeEntity = false;
      break;

    default:
      break;
  }

  // mark the object for destruction
  if (removeEntity) {
    physicsEntity->active = false;
  }
}

v3d_t Physics::calculateAcceleration(size_t index) {
  PhysicsEntity *physicsEntity = obj[index];
  if (physicsEntity->type == OBJTYPE_MELEE_ATTACK) {
    return physicsEntity->acc;
  }

  // a = f / m
  v3d_t acc = v3d_scale(physicsEntity->force, physicsEntity->one_over_mass);

  // clear the force now that we done with it
  physicsEntity->force = v3d_zero();

  // FIXME: c'mon more hacks?
  static double frictionCoeff = 0.98;

  v3d_t drag;
  double speed;

  switch (physicsEntity->type) {
    case OBJTYPE_PLAYER:
    case OBJTYPE_AI_ENTITY:
      acc.y += mGravity;
      frictionCoeff = mFriction;
      
      speed = v3d_mag(physicsEntity->vel);
      if (speed > 0.0 && physicsEntity->worldViscosity > 0.0) {
        drag = v3d_scale (speed * physicsEntity->worldViscosity, v3d_normalize(v3d_neg(physicsEntity->vel)));
        acc = v3d_add(acc, drag);
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
      // that is to say: no physics for rockets?

    case OBJTYPE_PLASMA:
    case OBJTYPE_PLASMA_TRAIL:
    case OBJTYPE_PLASMA_BOMB:
    case OBJTYPE_TIGER_BAIT:
    case OBJTYPE_MELEE_ATTACK:
      break;

    case OBJTYPE_STEAM:
    case OBJTYPE_SMOKE:
      if (physicsEntity->vel.y > 0.8) {
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

  if (!physicsEntity->applyPhysics) {
    acc = v3d_zero();
  }
  else if (physicsEntity->on_ground && acc.y <= 0.0) {
    // FIXME: this is ugly
    // FIXME: also, this doesn't account for gravity against any other surfaces
    // i.e. ceilings, walls...
    if (acc.y < 0.0) acc.y = 0.0;

    if (physicsEntity->vel.y < 0.0) physicsEntity->vel.y = 0.0;

    // now apply friction
    v3d_t ground_vel = physicsEntity->vel;
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
      physicsEntity->vel.x = 0.0;
      physicsEntity->vel.z = 0.0;
    }
  }

  return acc;
}

void Physics::integrate_euler(size_t index, double time, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];
  if (physicsEntity->type == OBJTYPE_MELEE_ATTACK) {
    return;
  }

  // update velocity
  physicsEntity->vel = v3d_add(physicsEntity->vel, v3d_scale(physicsEntity->acc, PHYSICS_TIME_GRANULARITY));

  // get a displacement vector for the time slice
  v3d_t disp = v3d_scale(physicsEntity->vel, PHYSICS_TIME_GRANULARITY);

  // mark them as inert if they've stopped moving
//	if (obj[index].type != OBJTYPE_PLAYER &&
//		obj[index].type != OBJTYPE_AI_ENTITY &&
//		v3d_mag (disp) < EPSILON) {
//		obj[index].applyPhysics = false;
//	}

  physicsEntity->displacement = disp;
  v3d_t disp_before_clip = disp;

  // clip displacement
  if (physicsEntity->type != OBJTYPE_SMOKE && physicsEntity->type != OBJTYPE_FIRE) {
    clip_displacement_against_world(worldMap, index);
  }

  v3d_t clip = v3d_sub(physicsEntity->displacement, disp_before_clip);

  if (fabs(clip.x) > 0.0) {
    physicsEntity->vel.x = 0.0;
//		obj[index].vel.x = -0.5 * obj[index].vel.x;
  }
  if (fabs(clip.y) > 0.0) {
    physicsEntity->vel.y = 0.0;
//		obj[index].vel.y = -0.5 * obj[index].vel.y;
  }
  if (fabs(clip.z) > 0.0) {
    physicsEntity->vel.z = 0.0;
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

    switch (physicsEntity->type) {
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

    physicsEntity->force = v3d_add(physicsEntity->force, v3d_scale(coeff_elasticity, clip));
  }
}


void Physics::displaceObject(size_t index, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];
  // move the object
  physicsEntity->pos = v3d_add(physicsEntity->pos, physicsEntity->displacement);

  // if the displacement clip clipped anything, take note!
  if (physicsEntity->displacement.x == 0.0) physicsEntity->vel.x = 0.0;
  if (physicsEntity->displacement.y == 0.0) physicsEntity->vel.y = 0.0;
  if (physicsEntity->displacement.z == 0.0) physicsEntity->vel.z = 0.0;

  // this solves certain issues with the applyPhysics flag
  physicsEntity->displacement = v3d_zero();

  // update the bounding box position
  physicsEntity->boundingBox.setPosition(physicsEntity->pos);

  // update the on-ground status
  // WARNING: should smoke and fire be omitted?
  if (physicsEntity->type != OBJTYPE_SMOKE && physicsEntity->type != OBJTYPE_FIRE) {
    physicsEntity->on_ground = updateOnGroundStatus(index, worldMap);
  }

  if (physicsEntity->type != OBJTYPE_AI_ENTITY &&
    physicsEntity->type != OBJTYPE_PLAYER &&
    physicsEntity->on_ground &&
    v3d_mag(physicsEntity->vel) < EPSILON)
  {
    physicsEntity->applyPhysics = false;
  }

  // update the inWater status
//	if (obj[index].type == OBJTYPE_PLAYER ||
//		obj[index].type == OBJTYPE_AI_ENTITY) {
    physicsEntity->worldViscosity = worldMap.getViscosity (physicsEntity->boundingBox);
//	}

//	if (obj[index].handle == mPlayerHandle) printf ("on ground: %d\n", obj[index].on_ground);
}



// tests to see if a phys entity is on the ground
bool Physics::updateOnGroundStatus(size_t index, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];
  v3d_t nc = physicsEntity->boundingBox.getNearCorner();

  // first exit chance, can't be on ground if nc.y is not an integer
  if (fabs(nc.y - floor(nc.y)) > EPSILON /*) &&
    // FIXME: this hack cures the 'fall through the ground sometimes when
    // should be on ground on a y = -1 block' - NOT A SOLUTION!
    fabs (nc.y - 0.0) > EPSILON*/)
  {
    return false;
  }

  v3d_t fc = physicsEntity->boundingBox.getFarCorner();

  int y = static_cast<int>(floor(nc.y)) - 1;

//	if (index == getIndexFromHandle(mPlayerHandle)) {
//		printf("y: %d, %f\n", y, nc.y - floor (nc.y));
//	}

  for (int z = static_cast<int>(floor(nc.z)); z <= static_cast<int>(floor(fc.z)); z++) {
    for (int x = static_cast<int>(floor(nc.x)); x <= static_cast<int>(floor(fc.x)); x++) {
      if (worldMap.isSolidBlock(v3di_v(x, y, z))) {
        return true;
      }
    }
  }

  return false;
}

double Physics::getLastUpdateTime() const {
  return mLastUpdateTime;
}

bool Physics::sweepObjects(size_t indexA, size_t indexB, double& t0, double& t1, int& axis) const {
  PhysicsEntity *entityA = obj[indexA];
  PhysicsEntity *entityB = obj[indexB];

  // if they are intersecting before the translation, return true
  if (entityA->boundingBox.isIntersecting (entityB->boundingBox)) {
//		printf ("intersect\n");
    t0 = t1 = 0.0;
    return true;
  }

  v3d_t relativeDisplacement = v3d_sub (entityB->displacement, entityA->displacement);

  v3d_t time0 = v3d_v (0.0, 0.0, 0.0);
  v3d_t time1 = v3d_v (1.0, 1.0, 1.0);

  // x-axis * * * *
  if (entityA->boundingBox.mFarCorner.x < entityB->boundingBox.mNearCorner.x) {
    if (relativeDisplacement.x < 0.0) {
      time0.x = (entityA->boundingBox.mFarCorner.x - entityB->boundingBox.mNearCorner.x) /
        relativeDisplacement.x;
    }
    else {
      return false;
    }
  }
  else if (entityB->boundingBox.mFarCorner.x < entityA->boundingBox.mNearCorner.x) {
    if (relativeDisplacement.x > 0.0) {
      time0.x = (entityA->boundingBox.mNearCorner.x - entityB->boundingBox.mFarCorner.x) /
        relativeDisplacement.x;
    }
    else {
      return false;
    }
  }

  if (entityB->boundingBox.mFarCorner.x > entityA->boundingBox.mNearCorner.x &&
    relativeDisplacement.x < 0.0) {
    time1.x = (entityA->boundingBox.mNearCorner.x - entityB->boundingBox.mFarCorner.x) /
      relativeDisplacement.x;
  }
  else if (entityA->boundingBox.mFarCorner.x > entityB->boundingBox.mNearCorner.x &&
    relativeDisplacement.x > 0.0) {
    time1.x = (entityA->boundingBox.mFarCorner.x - entityB->boundingBox.mNearCorner.x) /
      relativeDisplacement.x;
  }

  // y-axis * * * *
  if (entityA->boundingBox.mFarCorner.y < entityB->boundingBox.mNearCorner.y) {
    if (relativeDisplacement.y < 0.0) {
      time0.y = (entityA->boundingBox.mFarCorner.y - entityB->boundingBox.mNearCorner.y) /
        relativeDisplacement.y;
    }
    else {
      return false;
    }
  }
  else if (entityB->boundingBox.mFarCorner.y < entityA->boundingBox.mNearCorner.y) {
    if (relativeDisplacement.y > 0.0) {
      time0.y = (entityA->boundingBox.mNearCorner.y - entityB->boundingBox.mFarCorner.y) /
        relativeDisplacement.y;
    }
    else {
      return false;
    }
  }

  if (entityB->boundingBox.mFarCorner.y > entityA->boundingBox.mNearCorner.y &&
    relativeDisplacement.y < 0.0) {
    time1.y = (entityA->boundingBox.mNearCorner.y - entityB->boundingBox.mFarCorner.y) /
      relativeDisplacement.y;
  }
  else if (entityA->boundingBox.mFarCorner.y > entityB->boundingBox.mNearCorner.y &&
    relativeDisplacement.y > 0.0) {
    time1.y = (entityA->boundingBox.mFarCorner.y - entityB->boundingBox.mNearCorner.y) /
      relativeDisplacement.y;
  }

  // z-axis * * * *
  if (entityA->boundingBox.mFarCorner.z < entityB->boundingBox.mNearCorner.z) {
    if (relativeDisplacement.z < 0.0) {
      time0.z = (entityA->boundingBox.mFarCorner.z - entityB->boundingBox.mNearCorner.z) /
        relativeDisplacement.z;
    }
    else {
      return false;
    }
  }

  else if (entityB->boundingBox.mFarCorner.z < entityA->boundingBox.mNearCorner.z) {
    if (relativeDisplacement.z > 0.0) {
      time0.z = (entityA->boundingBox.mNearCorner.z - entityB->boundingBox.mFarCorner.z) /
        relativeDisplacement.z;
    }
    else {
      return false;
    }
  }

  if (entityB->boundingBox.mFarCorner.z > entityA->boundingBox.mNearCorner.z &&
    relativeDisplacement.z < 0.0) {
    time1.z = (entityA->boundingBox.mNearCorner.z - entityB->boundingBox.mFarCorner.z) /
      relativeDisplacement.z;
  }
  else if (entityA->boundingBox.mFarCorner.z > entityB->boundingBox.mNearCorner.z &&
    relativeDisplacement.z > 0.0) {
    time1.z = (entityA->boundingBox.mFarCorner.z - entityB->boundingBox.mNearCorner.z) /
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

int Physics::clipDisplacementAgainstOtherObjects(size_t index, WorldMap& worldMap) {
  PhysicsEntity *physicsEntity = obj[index];

  int numCollisions = 0;
  bool collision = false;
  double t0, t1;
  int axis;

  if (physicsEntity->type == OBJTYPE_AI_ENTITY || physicsEntity->type == OBJTYPE_PLAYER) {
    for (size_t other = 0; other < obj.size (); other++) {
      PhysicsEntity *otherPhysicsEntity = obj[other];
      
      if (!otherPhysicsEntity->active ||
        otherPhysicsEntity->queued ||
        other == index ||
        otherPhysicsEntity->owner == physicsEntity->handle) continue;

      if (otherPhysicsEntity->type == OBJTYPE_AI_ENTITY) {
        if (sweepObjects(index, other, t0, t1, axis)) {
          // they're already touching
          if (t0 == 0.0) {
            // push them apart
            // FIXME: this doesn't fully work as intended
            v3d_t difference = v3d_sub(otherPhysicsEntity->boundingBox.getCenterPosition(), physicsEntity->boundingBox.getCenterPosition());
            difference = v3d_normalize(difference);
            difference = v3d_scale(120.0, difference);

            add_force(otherPhysicsEntity->handle, difference);
            add_force(physicsEntity->handle, v3d_neg(difference));
          }
          else {
            physicsEntity->displacement = v3d_scale(t0 * 0.99, physicsEntity->displacement);
            otherPhysicsEntity->displacement = v3d_scale(t0 * 0.99, otherPhysicsEntity->displacement);

            if (axis == 0) {
              double temp = physicsEntity->vel.x;
              physicsEntity->vel.x = otherPhysicsEntity->vel.x;
              otherPhysicsEntity->vel.x = temp;
            }
            else if (axis == 1) {
              if (physicsEntity->vel.y < 0.0 && otherPhysicsEntity->on_ground) {
                physicsEntity->vel.y = -physicsEntity->vel.y;
              }
              else if (otherPhysicsEntity->vel.y < 0.0 && physicsEntity->on_ground) {
                otherPhysicsEntity->vel.y = -physicsEntity->vel.y;
              }
              else {
                double temp = physicsEntity->vel.y;
                physicsEntity->vel.y = otherPhysicsEntity->vel.y;
                otherPhysicsEntity->vel.y = temp;
              }
            }
            else if (axis == 2) {
              double temp = physicsEntity->vel.z;
              physicsEntity->vel.z = otherPhysicsEntity->vel.z;
              otherPhysicsEntity->vel.z = temp;
            }
            // deal some damage
            // FIXME: hack: tiger needs to do melee attacks
            // this is ludicrous!
            if ((physicsEntity->aiType == AITYPE_HOPPER &&
              physicsEntity->health > 0.0) &&
              (otherPhysicsEntity->aiType != AITYPE_HOPPER ||
              otherPhysicsEntity->type == OBJTYPE_PLAYER))
            {
              double damage = r_num(1.0, 8.0);
              otherPhysicsEntity->health -= damage;

              // tell the critter whodunnit
              phys_message_t message;
              message.sender = static_cast<int>(physicsEntity->handle);
              message.recipient = static_cast<int>(otherPhysicsEntity->handle);
              message.type = PHYS_MESSAGE_DAMAGE;
              message.dValue = damage;
              sendMessage (message);
            }
            else if ((physicsEntity->aiType != AITYPE_HOPPER ||
              physicsEntity->type == OBJTYPE_PLAYER) &&
              (otherPhysicsEntity->aiType == AITYPE_HOPPER &&
              otherPhysicsEntity->health > 0.0))
            {
              double damage = r_num (1.0, 8.0);
              physicsEntity->health -= damage;

              // tell the critter whodunnit
              phys_message_t message;
              message.sender = static_cast<int>(otherPhysicsEntity->handle);
              message.recipient = static_cast<int>(physicsEntity->handle);
              message.type = PHYS_MESSAGE_DAMAGE;
              message.dValue = damage;
              sendMessage (message);
            }
          }
        }
      }
      else if (otherPhysicsEntity->type == OBJTYPE_PLAYER) {
        if (sweepObjects(index, other, t0, t1, axis)) {
          if (t0 == 0.0) {
            // push them apart
            v3d_t difference = v3d_sub(otherPhysicsEntity->boundingBox.getCenterPosition(), physicsEntity->boundingBox.getCenterPosition());
            difference = v3d_normalize(difference);
            difference = v3d_scale(120.0, difference);

            add_force (otherPhysicsEntity->handle, difference);
            add_force (physicsEntity->handle, v3d_neg(difference));
          }
          else {
            physicsEntity->displacement = v3d_scale(t0 * 0.99, physicsEntity->displacement);
            otherPhysicsEntity->displacement = v3d_scale(t0 * 0.99, otherPhysicsEntity->displacement);

            if (axis == 0) {
              double temp = physicsEntity->vel.x;
              physicsEntity->vel.x = otherPhysicsEntity->vel.x;
              otherPhysicsEntity->vel.x = temp;
            }
            else if (axis == 1) {
              if (physicsEntity->vel.y < 0.0 && otherPhysicsEntity->on_ground) {
                physicsEntity->vel.y = -physicsEntity->vel.y;
              }
              else if (otherPhysicsEntity->vel.y < 0.0 && physicsEntity->on_ground) {
                otherPhysicsEntity->vel.y = -physicsEntity->vel.y;
              }
              else {
                double temp = physicsEntity->vel.y;
                physicsEntity->vel.y = otherPhysicsEntity->vel.y;
                otherPhysicsEntity->vel.y = temp;
              }
            }
            else if (axis == 2) {
              double temp = physicsEntity->vel.z;
              physicsEntity->vel.z = otherPhysicsEntity->vel.z;
              otherPhysicsEntity->vel.z = temp;
            }

            // deal some damage
//						otherPhysicsEntity->health -= r_num (1.0, 8.0);
          }
        }
      }
      else if (otherPhysicsEntity->type == OBJTYPE_SLIME ||
        otherPhysicsEntity->type == OBJTYPE_PLASMA ||
        otherPhysicsEntity->type == OBJTYPE_FIRE ||
        (otherPhysicsEntity->type == OBJTYPE_NAPALM && otherPhysicsEntity->applyPhysics) ||
        otherPhysicsEntity->type == OBJTYPE_SPARK ||
        otherPhysicsEntity->type == OBJTYPE_LIVE_BULLET ||
        otherPhysicsEntity->type == OBJTYPE_DEAD_BULLET) {
//				if (otherPhysicsEntity->boundingBox.isIntersecting (physicsEntity->boundingBox)) {

        if (sweepObjects(index, other, t0, t1, axis)) {
          // directly add knockback
          physicsEntity->force = v3d_add(physicsEntity->force, v3d_scale(otherPhysicsEntity->mass, otherPhysicsEntity->vel));

          if (r_numi(0, 5) == 3) {
            v3d_t position = v3d_add(otherPhysicsEntity->pos, v3d_scale(t0, otherPhysicsEntity->displacement));
//						printf ("BLOOD!\n");
            createEntity(OBJTYPE_BLOOD_SPRAY, position, mLastUpdateTime, false);
          }

          double damage = otherPhysicsEntity->impactDamage;
          physicsEntity->health -= damage;

          // tell the critter whodunnit
          phys_message_t message;

          message.sender = static_cast<int>(otherPhysicsEntity->owner);
          message.recipient = static_cast<int>(physicsEntity->handle);
          message.type = PHYS_MESSAGE_DAMAGE;
          message.dValue = damage;

          sendMessage(message);

          removeEntity(otherPhysicsEntity->handle);

          if (physicsEntity->type == AITYPE_HOPPER && r_numi(0, 10) == 3) {
            // here??
            // TODO: move to somewhere a little more AI-related
            addSoundEvent(SOUND_TIGER_ROAR, otherPhysicsEntity->pos);
          }

          collision = true;

          numCollisions++;
        }
      }
      else if (otherPhysicsEntity->type == OBJTYPE_GRENADE ||
           otherPhysicsEntity->type == OBJTYPE_ROCKET) {
        if (sweepObjects (index, other, t0, t1, axis)) {
          expireEntity (other, mLastUpdateTime, worldMap);

          double damage = otherPhysicsEntity->impactDamage;
          physicsEntity->health -= damage;

          // tell the critter whodunnit
          phys_message_t message;
          message.sender = static_cast<int>(otherPhysicsEntity->owner);
          message.recipient = static_cast<int>(physicsEntity->handle);
          message.type = PHYS_MESSAGE_DAMAGE;
          message.dValue = damage;
          sendMessage(message);

          collision = true;

          numCollisions++;
        }
      }
      else if (otherPhysicsEntity->type == OBJTYPE_PLASMA_BOMB) {
        if (sweepObjects(index, other, t0, t1, axis)) {
          v3d_t position = v3d_add(otherPhysicsEntity->boundingBox.getCenterPosition(), v3d_scale(t0, otherPhysicsEntity->displacement));
          plasmaBombExplode (position, mLastUpdateTime + PHYSICS_TIME_GRANULARITY, 10, worldMap);

          physicsEntity->force = v3d_add(physicsEntity->force, v3d_scale(10.0 * physicsEntity->mass, otherPhysicsEntity->vel));

          for (int blood = 0; blood < 5; blood++) {
            v3d_t bloodForce = v3d_random (75.0);
            createEntity (OBJTYPE_BLOOD_SPRAY, position, bloodForce, mLastUpdateTime, true);
          }

          double damage = otherPhysicsEntity->impactDamage;
          physicsEntity->health -= damage;

          // tell the critter whodunnit
          phys_message_t message;
          message.sender = static_cast<int>(otherPhysicsEntity->owner);
          message.recipient = static_cast<int>(physicsEntity->handle);
          message.type = PHYS_MESSAGE_DAMAGE;
          message.dValue = damage;
          sendMessage(message);

          removeEntity(otherPhysicsEntity->handle);

          collision = true;

          numCollisions++;
        }
      }
      else if (otherPhysicsEntity->type == OBJTYPE_MELEE_ATTACK) {
        if (otherPhysicsEntity->boundingBox.isIntersecting(physicsEntity->boundingBox)) {
          v3d_t otherPosition = otherPhysicsEntity->boundingBox.getCenterPosition();
          createEntity(OBJTYPE_BLOOD_SPRAY, otherPosition, mLastUpdateTime, false);

          double damage = otherPhysicsEntity->impactDamage;
          physicsEntity->health -= damage;

          // tell the critter whodunnit
          phys_message_t message;
          message.sender = static_cast<int>(otherPhysicsEntity->owner);
          message.recipient = static_cast<int>(physicsEntity->handle);
          message.type = PHYS_MESSAGE_DAMAGE;
          message.dValue = damage;
          sendMessage (message);

          // knockback
          add_force(physicsEntity->handle, otherPhysicsEntity->acc);
          addSoundEvent(SOUND_PUNCH, otherPhysicsEntity->boundingBox.getCenterPosition());
          removeEntity(otherPhysicsEntity->handle);
          numCollisions++;
        }
      }
      else if (mGotPickupMessage && physicsEntity->type == OBJTYPE_PLAYER && otherPhysicsEntity->type == OBJTYPE_ITEM) {
        if (sweepObjects(index, other, t0, t1, axis)) {
          // try to pick up the object
          phys_message_t message;
          message.sender = MAILBOX_PHYSICS;
          message.recipient = static_cast<int>(physicsEntity->handle);
          message.type = PHYS_MESSAGE_ITEMGRAB;
          message.iValue = static_cast<int>(otherPhysicsEntity->itemHandle);
          message.iValue2 = otherPhysicsEntity->handle;
          sendMessage(message);

          collision = true;
          numCollisions++;
        }
      }
    }
  }
  else if (physicsEntity->type == OBJTYPE_EXPLOSION) {
    for (size_t other = 0; other < obj.size (); other++) {
      PhysicsEntity *otherPhysicsEntity = obj[other];
      if (otherPhysicsEntity->active == false ||
        physicsEntity->queued ||
        other == index) continue;

      if (otherPhysicsEntity->type != OBJTYPE_EXPLOSION) {
        if (otherPhysicsEntity->boundingBox.isIntersecting(physicsEntity->boundingBox)) {
          if (otherPhysicsEntity->type == OBJTYPE_SMOKE || otherPhysicsEntity->type == OBJTYPE_STEAM) {
            removeEntity(otherPhysicsEntity->handle);
          }
          else {
            v3d_t dim = otherPhysicsEntity->boundingBox.getDimensions();
            
            double strength = (dim.x * dim.x) + (dim.y * dim.y) + (dim.y * dim.y);
            add_force(otherPhysicsEntity->handle, getRadialForce(otherPhysicsEntity->boundingBox.getCenterPosition(),
              physicsEntity->boundingBox.getCenterPosition(), physicsEntity->explosionForce * strength, 10.0));
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


void Physics::add_force(size_t handle, const v3d_t& force) {
  int index = getIndexFromHandle(handle);
  if (index != -1) {
    PhysicsEntity *physicsEntity = obj[index];
    physicsEntity->force = v3d_add (physicsEntity->force, force);
    physicsEntity->applyPhysics = true;
  }
}


void Physics::clip_displacement_against_world( WorldMap& worldMap, size_t index ) {
  PhysicsEntity *physicsEntity = obj[index];
  if (v3d_mag( physicsEntity->displacement ) == 0.0) {
    return;
  }

  int solidityType;

  v3d_t nc = physicsEntity->boundingBox.getNearCorner();
  v3d_t fc = physicsEntity->boundingBox.getFarCorner();

  // bounding box for the world block we'll be clipping against
  BoundingBox bb2( v3d_v( 1.0, 1.0, 1.0 ) );

  v3di_t i;
  v3d_t time0;

  int y_start;

  if (physicsEntity->on_ground) y_start = static_cast<int>(floor( nc.y ));
  else y_start = static_cast<int>(floor( nc.y - 1.0 ));

  if (physicsEntity->type == OBJTYPE_PLAYER) {
//		printf ("y_start: %d\n", y_start);
  }

  // FIXME: this is a horribly inefficient way of doing this
  // it also fails if the velocity is too high
  // your mom
  for (i.z = static_cast<int>(floor( nc.z - 1.0 )); i.z <= static_cast<int>(floor( fc.z + 1.0 )); i.z++) {
    for (i.y = y_start; i.y <= static_cast<int>(floor( fc.y + 1.0 )); i.y++) {
      for (i.x = static_cast<int>(floor( nc.x - 1.0 )); i.x <= static_cast<int>(floor( fc.x + 1.0 )); i.x++) {
        block_t *block = worldMap.getBlock( i );
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


          if (physicsEntity->boundingBox.isIntersecting (bb2)) {
            if (nc.x == bb2.mFarCorner.x && physicsEntity->displacement.x < 0.0) {
              physicsEntity->displacement.x = 0.0;
            }
            if (fc.x == bb2.mNearCorner.x && physicsEntity->displacement.x > 0.0) {
              physicsEntity->displacement.x = 0.0;
            }
            if (nc.y == bb2.mFarCorner.y && physicsEntity->displacement.y < 0.0) {
              physicsEntity->displacement.y = 0.0;
            }
            if (fc.y == bb2.mNearCorner.y && physicsEntity->displacement.y > 0.0) {
              physicsEntity->displacement.y = 0.0;
            }
            if (nc.z == bb2.mFarCorner.z && physicsEntity->displacement.z < 0.0) {
              physicsEntity->displacement.z = 0.0;
            }
            if (fc.z == bb2.mNearCorner.z && physicsEntity->displacement.z > 0.0) {
              physicsEntity->displacement.z = 0.0;
            }
          }


          // x-axis
          if (physicsEntity->displacement.x < 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
              test_x = true; }
            else {
              test_x = false; } }
          else if (physicsEntity->displacement.x > 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
              test_x = true; }
            else {
              test_x = false; } }
          else {
            test_x = false; }

          // y-axis
          if (physicsEntity->displacement.y < 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
              test_y = true; }
            else {
              test_y = false; } }
          else if (physicsEntity->displacement.y > 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
              test_y = true; }
            else {
              test_y = false; } }
          else {
            test_y = false; }

          // z-axis
          if (physicsEntity->displacement.z > 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
              test_z = true; }
            else {
              test_z = false; } }
          else if (physicsEntity->displacement.z < 0.0) {
            if (block->faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
              test_z = true; }
            else {
              test_z = false; } }
          else {
            test_z = false; }

//					test_x = test_y = test_z = true;

//					if (physicsEntity->type == OBJTYPE_SLIME) {
//						printf ("test %d, %d, %d\n", test_x, test_y, test_z);
//					}

//					if (physicsEntity->boundingBox.clipDisplacement (test_x,
//													test_y, test_z, displacement, bb2)) {


          if ((test_x || test_y || test_z) &&
            physicsEntity->boundingBox.sweepTest (bb2, physicsEntity->displacement, time0))
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


//						if (physicsEntity->type == OBJTYPE_SLIME) {
//							printf ("test %d, %d, %d\n", test_x, test_y, test_z);
//							printf ("time %f, %f, %f\n", time0.x, time0.y, time0.z);
//							printf ("axis: %d\n", axis);
    //				}


            // no sliding
            if (physicsEntity->type == OBJTYPE_PLASMA_BOMB ||
              physicsEntity->type == OBJTYPE_NAPALM)
            {
              physicsEntity->displacement = v3d_scale (t0, physicsEntity->displacement);
            }
            // sliding
            else {
              if (axis == 0) {
                physicsEntity->displacement.x = t0 * physicsEntity->displacement.x;
              }
              else if (axis == 1) {
                physicsEntity->displacement.y = t0 * physicsEntity->displacement.y;
              }
              else if (axis == 2) {
                physicsEntity->displacement.z = t0 * physicsEntity->displacement.z;
              }
            }


            switch (physicsEntity->type) {
              case OBJTYPE_SPARK:
                if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
                  createEntity (OBJTYPE_STEAM, physicsEntity->pos, mLastUpdateTime, false);
                  physicsEntity->expirationTime = mLastUpdateTime;
                }
                break;

              case OBJTYPE_NAPALM:
                if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
                  createEntity (OBJTYPE_STEAM, physicsEntity->pos, mLastUpdateTime, false);
                  physicsEntity->expirationTime = mLastUpdateTime;
                }
                else {
                  physicsEntity->vel = v3d_zero ();
                  physicsEntity->applyPhysics = false;
                }
                break;

              case OBJTYPE_LIVE_BULLET:
              case OBJTYPE_DEAD_BULLET:
                if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
                  createEntity (OBJTYPE_STEAM, physicsEntity->pos, mLastUpdateTime, false);
                  physicsEntity->expirationTime = 0.1;
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
                  createEntity (OBJTYPE_STEAM, physicsEntity->pos, mLastUpdateTime, false);
                  physicsEntity->expirationTime = 0.1;
//									physicsEntity->vel = v3d_zero ();
//									physicsEntity->applyPhysics = false;
                }
                break;

              case OBJTYPE_ROCKET:
              case OBJTYPE_PLASMA:
              case OBJTYPE_PLASMA_BOMB:
                physicsEntity->expirationTime = 0.1;
                break;

              case OBJTYPE_SLIME:
              case OBJTYPE_BLOOD_SPRAY:
//								physicsEntity->vel = v3d_zero ();
//								physicsEntity->applyPhysics = false;
                break;

              case OBJTYPE_CREATE:
//								if (i.y > 0) {
//									v3di_t bleh = v3di_v (physicsEntity->pos);
//									block_t b;
//									b.type = 1;
//								}
                break;

              case OBJTYPE_DESTROY:
                worldMap.clearBlock(i);
                physicsEntity->expirationTime = mLastUpdateTime;
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


bool Physics::isIntersectingPlant(size_t index, const WorldMap& worldMap) const {
  block_t *block = worldMap.getBlock(obj[index]->boundingBox.getCenterPosition());
  if (block == NULL) return false;
  if (gBlockData.get(block->type)->solidityType == BLOCK_SOLIDITY_TYPE_PLANT) {
    return true;
  }
  return false;
}


// return the on_ground flag for an object
bool Physics::isIndexOnGround(size_t index) const {
  return obj[index]->on_ground;
}



// return the on_ground flag for an object
bool Physics::isHandleOnGround(size_t handle) const {
  size_t index = getIndexFromHandle(handle);
  if (index != -1) {
    return obj[index]->on_ground;
  }
  return false;
}


void Physics::grenadeExplosion( size_t index, double time, WorldMap& worldMap ) {
  PhysicsEntity *physicsEntity = obj[index];
  v3d_t centerPosition = physicsEntity->boundingBox.getCenterPosition ();

  addSoundEvent (SOUND_GRENADE_EXPLOSION, centerPosition);
  
  for (int i = 0; i < physicsEntity->numParticles; i++) {
    v3d_t randomPosition = v3d_add (centerPosition, v3d_random (0.5));
    size_t shrapnelHandle = createEntity (physicsEntity->type2, randomPosition, time, true);
    int shrapnelIndex = getIndexFromHandle (shrapnelHandle);
    if (shrapnelIndex > 0) {
      obj[shrapnelIndex]->owner = physicsEntity->owner;
      obj[shrapnelIndex]->impactDamage = physicsEntity->impactDamage;
    }
  }


  if (physicsEntity->explosionForce > 2500.0) {
    double clearRadius = (physicsEntity->explosionForce - 2500.0) * 0.001;

    worldMap.clearSphere (centerPosition, clearRadius);
  }

  size_t explosionHandle = createEntity (OBJTYPE_EXPLOSION, centerPosition, time, true);

  // FIXME: hack!
  if (explosionHandle > 0) {
    int explosionIndex = getIndexFromHandle (explosionHandle);
    obj[explosionIndex]->owner = physicsEntity->owner;
    obj[explosionIndex]->applyPhysics = false;
    obj[explosionIndex]->explosionForce = physicsEntity->explosionForce;
  }
}




void Physics::spawnExplosion(const v3d_t& pos, double time, size_t numParticles, WorldMap& worldMap) {
  addSoundEvent (SOUND_GRENADE_EXPLOSION, pos);

  for (size_t i = 0; i < numParticles - 10; i++) {
    createEntity (OBJTYPE_SPARK, v3d_add(pos, v3d_random(r_num(0.5, 0.6))), time, true);
  }

  int num_napalms = r_numi(5, 10);
  for (size_t i = 0; i < static_cast<size_t>(num_napalms); i++) {
    createEntity(OBJTYPE_NAPALM, v3d_add(pos, v3d_random(r_num(0.5, 0.6))), time, true);
  }

  worldMap.clearSphere(pos, 2.5);

  size_t handle = createEntity(OBJTYPE_EXPLOSION, pos, time, true);
  // FIXME: hack!
  if (handle != 0) {
    int index = getIndexFromHandle(handle);
    PhysicsEntity *physicsEntity = obj[index];
    physicsEntity->applyPhysics = false;
    physicsEntity->explosionForce = 5000.0;
  }
}

void Physics::spawnMeatExplosion(const v3d_t& pos, double time, size_t numParticles, WorldMap& worldMap) {
  addSoundEvent(SOUND_SPLAT, pos);

  for (size_t i = 0; i < numParticles - 10; i++) {
    createEntity(OBJTYPE_BLOOD_SPRAY, v3d_add(pos, v3d_random(r_num(0.5, 0.6))), time, true);
  }

  size_t handle = createEntity(OBJTYPE_EXPLOSION, pos, time, true);

  // FIXME: hack!
  if (handle != 0) {
    int index = getIndexFromHandle(handle);
    PhysicsEntity *physicsEntity = obj[index];
    physicsEntity->applyPhysics = false;
    physicsEntity->explosionForce = 250.0;
  }
}

void Physics::plasmaBombExplode(const v3d_t& pos, double time, size_t numParticles, WorldMap& worldMap) {
  for (size_t i = 0; i < numParticles; i++) {
    size_t handle = createEntity (OBJTYPE_PLASMA_SPARK, pos, time, true);

    if (handle != 0) {
      int index = getIndexFromHandle(handle);
      PhysicsEntity *physicsEntity = obj[index];
      if (!worldMap.isBoundingBoxEmpty(physicsEntity->boundingBox)) {
        physicsEntity->active = false;
      }
      else {
        physicsEntity->boundingBox.setDimensions(v3d_scale(r_num(2.0, 5.0), physicsEntity->boundingBox.getDimensions()));

        // FIXME: is this safe?
        // (is that the sort of question to ask in a method
        // named: plasmaBombExplode()?
        physicsEntity->expirationTime += r_num(0.2, 0.4);
        v3d_t randomVel = v3d_random(r_num(0.2, 1.0));
        physicsEntity->vel = randomVel;
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

v3d_t Physics::getRadialForce(const v3d_t& pos, const v3d_t& center, double force, double radius) const {
  v3d_t diff = v3d_sub(pos, center);
  double dist = v3d_mag(diff);
  if (dist > radius || dist == 0.0) {
    return v3d_zero();
  }
  else if (dist < 0.1) {
    dist = 0.1;
  }

  double magnitude = force / (dist * dist);
  return v3d_scale (magnitude, v3d_normalize(diff));
}


int Physics::addSoundEvent(int type, const v3d_t& position) {
  double distanceToPlayer = v3d_dist(getCenter(mPlayerHandle), position);

  if (mNumSoundEvents < MAX_SOUND_EVENTS) {
    int volume;
    if (distanceToPlayer >= 8.0) {
      volume = static_cast<int>(floor(96.0 * (1.0 / (distanceToPlayer - 7.0))));
    }
    else {
      volume = 96;
    }

    if (volume > 0) {
      mSoundEvents[mNumSoundEvents] = type;
      mSoundVolumes[mNumSoundEvents] = volume;
      mNumSoundEvents++;
    }

    return 0;
  }

  return 1;
}


void Physics::playSoundEvents(AssetManager& assetManager) {
  for (int i = 0; i < mNumSoundEvents; i++) {
    assetManager.mSoundSystem.playSoundByHandle(mSoundEvents[i], mSoundVolumes[i]);
  }
  mNumSoundEvents = 0;
}


size_t Physics::getPlayerHandle() const {
  return mPlayerHandle;
}


void Physics::readMail() {
  phys_message_t message;
  int index;

  while (getNextMessage(MAILBOX_PHYSICS, &message)) {
    switch (message.type) {
      case PHYS_MESSAGE_PLAYERPICKUPITEMS:
        mGotPickupMessage = true;
        break;

      case PHYS_MESSAGE_ITEMGRABBED:
        removeEntity(message.iValue2);
        break;

      case PHYS_MESSAGE_ADJUSTHEALTH:
        index = getIndexFromHandle(message.iValue);

        if (index >= 0) {
          obj[index]->health += message.dValue;
        }
        break;

      default:
        break;
    }
  }
}


void Physics::sendMessage(const phys_message_t& message) {
  mMessages.push_back(message);
}


int Physics::getNextMessage(int recipient, phys_message_t* message) {
  for (size_t i = 0; i < mMessages.size(); i++) {
    if (mMessages[i].recipient == recipient) {
      *message = mMessages[i];
      size_t lastMessage = mMessages.size() - 1;

      // get rid of this message
      if (i == lastMessage) {
        mMessages.pop_back();
      }
      else {
        swap(mMessages[i], mMessages[lastMessage]);
        mMessages.pop_back();
        i--;
      }
      return 1;
    }
  }

  return 0;
}


void Physics::clearMailBox(int recipient) {
  for (size_t i = 0; i < mMessages.size(); i++) {
    if (mMessages[i].recipient == recipient) {
      size_t lastMessage = mMessages.size() - 1;

      // get rid of this message
      if (i == lastMessage) {
        mMessages.pop_back();
      }
      else {
        swap(mMessages[i], mMessages[lastMessage]);
        mMessages.pop_back();
        i--;
      }
    }
  }
}


vector<size_t> Physics::getAllItemHandles() const {
  vector<size_t> itemList;
  PhysicsEntity *physicsEntity;

  for (size_t i = 0; i < obj.size(); i++) {
    physicsEntity = obj[i];
    if (physicsEntity->type == OBJTYPE_ITEM) {
      itemList.push_back(physicsEntity->itemHandle);
    }
  }

  return itemList;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Physics
// *
// * this provides the AABB physics engine
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/v3d.h"
#include "../game/GameModel.h"
#include "../world/WorldMap.h"
#include "../math/BoundingBox.h"
#include "../assets/AssetManager.h"
#include "../physics/Inactivelist.h"

//#define PHYSICS_TIME_GRANULARITY (0.03333333)  // 30 Hz
//#define PHYSICS_TIME_GRANULARITY (0.01666667)  // 60 Hz
#define PHYSICS_TIME_GRANULARITY (0.01111111)  // 90 Hz
//#define PHYSICS_TIME_GRANULARITY (0.00833333)  // 120 Hz

// physics defaults
//#define DEFAULT_GRAVITY (-9.80665)
#define DEFAULT_GRAVITY (-15.0)
#define DEFAULT_FRICTION (1.0)
#define DEFAULT_AIR_RESISTANCE (0.01)
#define DEFAULT_AIR_WALK (2.0)
#define DEFAULT_WALK_ACC (50.0)
#define DEFAULT_JUMP_ACC (250.0)
#define DEFAULT_MASS (80.0)

#define EPSILON (0.0001)



// typedefs * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


struct entity_type_info_t {
  double massLow;
  double massHigh;

  double lifespanLow;
  double lifespanHigh;

  double dimensionXLow;
  double dimensionXHigh;

  double dimensionYLow;
  double dimensionYHigh;

  double dimensionZLow;
  double dimensionZHigh;

  GLfloat color[4];
};


struct PhysicsEntity {
  size_t handle;

  GLfloat baseColor[4];
  GLfloat color[4];

  bool queued;
  bool active;

  int type;
  int type2;

  int numParticles;
  double impactDamage;

  BoundingBox boundingBox;

  double lifespan;
  double expirationTime;

  bool applyPhysics;

  double mass;
  double one_over_mass;
  double density;

  v3d_t force;
  v3d_t acc;
  v3d_t vel;
  v3d_t pos;
  v3d_t displacement;

  int aiType;
  size_t owner;
  double health;

  size_t itemHandle;

  bool on_ground;
  double worldViscosity;

  double explosionForce;
};

class GameModel;

// Physics * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class Physics {
public:
  Physics(GameModel* gameModel);
  ~Physics();

  vector<PhysicsEntity*>* getEntityVector();

  void loadInactiveList();

  int getIndexFromHandle(size_t handle) const;

  void togglePause();
  bool isPaused() const;
  void advanceOneFrame();

  void addQueuedEntities();
  void manageEntitiesList();

  PhysicsEntity* createEntity(int type, const v3d_t& position, bool center);
  PhysicsEntity* createEntity(int type, const v3d_t& position, const v3d_t& initialForce, bool center);
  PhysicsEntity* createAiEntity(int aiType, const v3d_t& position);

  void removeEntity(size_t handle);

  void setMass(size_t handle, double mass);
  void setHealth(size_t handle, double health);
  void setDimensions(size_t handle, const v3d_t& dimensions);
  double setDensity(PhysicsEntity* entity); // should this be here? or should it be a method on PhysicsEntity?

  PhysicsEntity* getEntityByHandle(size_t handle);
  void setEntity(PhysicsEntity* entity);

  v3d_t getCenter(size_t handle) const;
  v3d_t getNearCorner(size_t handle) const;
  v3d_t getFarCorner(size_t handle) const;

  v3d_t getVelocity(size_t handle) const;
  void setVelocity(size_t handle, const v3d_t& velocity);

  void set_pos(size_t handle, const v3d_t& pos);

  void setOwner(size_t handle, size_t owner);
  void setItemHandleAndType(size_t handle, size_t itemHandle, int itemType);

  void readMail();

  // * * * * * * * * UPDATE
  int update(double time, AssetManager* assetManager);
  void updateEntity(size_t index);
  void expireEntity(size_t index);

  void integrate_euler(size_t index);
  void displaceObject(size_t index);
  v3d_t calculateAcceleration(size_t handle);

  bool updateOnGroundStatus(size_t index);

  double getLastUpdateTime() const;

  bool sweepObjects(size_t indexA, size_t indexB, double& t0, double& t1, int& axis) const;
  int clipDisplacementAgainstOtherObjects(size_t index);

  void add_force(size_t handle, const v3d_t& force);

  void clip_displacement_against_world(size_t index);
  bool isIntersectingPlant(size_t index) const;

  bool isIndexOnGround(size_t index) const;
  bool isHandleOnGround(size_t handle) const;

  void grenadeExplosion(size_t index);
  void spawnExplosion(const v3d_t& pos, size_t numParticles);
  void spawnMeatExplosion(const v3d_t& pos, size_t numParticles);
  void plasmaBombExplode(const v3d_t& pos, size_t numParticles);
  v3d_t getRadialForce(const v3d_t& pos, const v3d_t& center, double force, double radius) const;

  int addSoundEvent(int type, const v3d_t& position);
  void playSoundEvents(AssetManager* assetManager);

  size_t getPlayerHandle() const;

  vector<size_t> getAllItemHandles() const;

private:
  // copy constructor guard
  Physics(const Physics& physics) { }
  // assignment operator guard
  Physics& operator=(const Physics& physics) { return *this; }

  GameModel* mGameModel;

  entity_type_info_t mEntityTypeInfo[NUM_OBJTYPES];
  vector<PhysicsEntity*> obj;
  size_t mLastEntityHandle;

  size_t mPlayerHandle;

  double mLastUpdateTime;
  bool mEntityAdded;

  bool mPaused;
  bool mAdvanceOneFrame;

  double mGravity;
  double mFriction;

  // FIXME: hacks

  // sound stuff should be somewhere else
  static const int MAX_SOUND_EVENTS = 10;
  int mNumSoundEvents;
  int mSoundEvents[MAX_SOUND_EVENTS];
  int mSoundVolumes[MAX_SOUND_EVENTS];

  InactiveList mInactiveList;

  bool mGotPickupMessage;
};

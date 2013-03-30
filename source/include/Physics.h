// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * physics_c
// *
// * this provides the AABB physics engine
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Physics_h_
#define Physics_h_

#include "v3d.h"

#include "WorldMap.h"
#include "BoundingBox.h"
#include "AssetManager.h"
#include "inactivelist.h"



//#define PHYSICS_TIME_GRANULARITY	(0.03333333)	// 30 Hz
//#define PHYSICS_TIME_GRANULARITY	(0.01666667)	// 60 Hz
#define PHYSICS_TIME_GRANULARITY	(0.01111111)	// 90 Hz
//#define PHYSICS_TIME_GRANULARITY	(0.00833333)	// 120 Hz


// physics defaults
//#define DEFAULT_GRAVITY			(-9.80665)
#define DEFAULT_GRAVITY			(-15.0)
#define DEFAULT_FRICTION		(1.0)
#define DEFAULT_AIR_RESISTANCE	(0.01)
#define DEFAULT_AIR_WALK		(2.0)
#define DEFAULT_WALK_ACC		(50.0)
#define DEFAULT_JUMP_ACC		(250.0)
#define DEFAULT_MASS			(80.0)

#define EPSILON					(0.0001)



// typedefs * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


typedef struct {
	v3d_t vector;

	double time_start;
	double time_end;
} phys_force_t;


typedef struct {
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
} entity_type_info_t;


typedef struct {
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

} phys_entity_t;



enum {
	PHYS_MESSAGE_DAMAGE,
	PHYS_MESSAGE_ITEMGRAB,
	PHYS_MESSAGE_ITEM_DESTROYED,
	PHYS_MESSAGE_SPAWN_CREATURE,
	PHYS_MESSAGE_PLAYERPICKUPITEMS,
	PHYS_MESSAGE_ITEMGRABBED,
	PHYS_MESSAGE_ADJUSTHEALTH,

	NUM_PHYS_MESSSAGES
};


#define MAILBOX_PHYSICS			(-1)
#define MAILBOX_ITEMMANAGER		(-2)
#define MAILBOX_AIMANAGER		(-3)


typedef struct {
	int sender;
	int recipient;

	int type;

	int iValue;
	int iValue2;

	double dValue;

	v3d_t vec3;
} phys_message_t;



// physics_c * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class physics_c {
public:
	physics_c ();
	~physics_c ();

	void reset (void);
	void clear (void);

	vector <phys_entity_t> *getEntityVector(void);

	void loadInactiveList (void);

	int getIndexFromHandle (size_t handle);

	void togglePause (void);
	bool isPaused (void);
	void advanceOneFrame (void);

	void addQueuedEntities (void);
	void manageEntitiesList (void);

	size_t createEntity (int type, v3d_t position, double time, bool center);
	size_t createEntity (int type, v3d_t position, v3d_t initialForce, double time, bool center);

	size_t createAiEntity (int aiType, v3d_t position, double time);

	void removeEntity (size_t handle);

	void setMass (size_t handle, double mass);
	void setHealth (size_t handle, double health);
	void setDimensions (size_t handle, v3d_t dimensions);

	phys_entity_t *getEntityByHandle(size_t handle);
	void setEntity (phys_entity_t entity);

	v3d_t getCenter (size_t handle);
	v3d_t getNearCorner (size_t handle);
	v3d_t getFarCorner (size_t handle);

	v3d_t getVelocity (size_t handle);
	void setVelocity (size_t handle, v3d_t velocity);

	void set_pos (size_t handle, v3d_t pos);

	void setOwner (size_t handle, size_t owner);
	void setItemHandleAndType (size_t handle, size_t itemHandle, int itemType);

	// * * * * * * * * UPDATE
	int update (double time, WorldMap &worldMap, AssetManager &assetManager);
	void updateEntity (size_t index, double time, WorldMap &worldMap);
	void expireEntity (size_t index, double time, WorldMap &worldMap);

	void integrate_euler (size_t index, double time, WorldMap &worldMap);
	void displaceObject (size_t index, WorldMap &worldMap);
	v3d_t calculateAcceleration (size_t handle);

	bool updateOnGroundStatus (size_t index, WorldMap &worldMap);

	double getLastUpdateTime (void);

	bool sweepObjects (size_t indexA, size_t indexB, double &t0, double &t1, int &axis);
	int clipDisplacementAgainstOtherObjects (size_t index, WorldMap &worldMap);
//	int testForEntityCollisions (World &world);

	void add_force (size_t handle, v3d_t force);

	void clip_displacement_against_world (WorldMap &worldMap, size_t index);
	bool isIntersectingPlant(size_t index, WorldMap &worldMap);

	bool isIndexOnGround (size_t index);
	bool isHandleOnGround (size_t handle);

	void grenadeExplosion (size_t index, double time, WorldMap &worldMap);
	void spawnExplosion (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap);
	void spawnMeatExplosion (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap);
	void plasmaBombExplode (v3d_t pos, double time, size_t numParticles, WorldMap &worldMap);
	v3d_t getRadialForce (v3d_t pos, v3d_t center, double force, double radius);

	int addSoundEvent (int type, v3d_t position);
	void playSoundEvents (AssetManager &assetManager);

	size_t getPlayerHandle (void);

	void sendMessage (phys_message_t message);
	int getNextMessage (int recipient, phys_message_t *message);
	void clearMailBox (int recipient);

	vector<size_t> getAllItemHandles(void);

private:
	// copy constructor guard
	physics_c (const physics_c &physics) { }
	// assignment operator guard
	physics_c & operator=(const physics_c &physics) { return *this; }


	entity_type_info_t mEntityTypeInfo[NUM_OBJTYPES];
	vector <phys_entity_t> obj;
	size_t mLastEntityHandle;

	size_t mPlayerHandle;

	double mLastUpdateTime;
	bool mEntityAdded;

	bool mPaused;
	bool mAdvanceOneFrame;

	double mGravity;
	double mFriction;

	vector <phys_message_t> mMessages;

	// FIXME: hacks

	// sound stuff should be somewhere else
	static const int MAX_SOUND_EVENTS = 10;
	int mNumSoundEvents;
	int mSoundEvents[MAX_SOUND_EVENTS];
	int mSoundVolumes[MAX_SOUND_EVENTS];

	InactiveList mInactiveList;

	bool mGotPickupMessage;
};



#endif // Physics_h_
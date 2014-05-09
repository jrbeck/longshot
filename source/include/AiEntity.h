// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiEntity
// *
// * this is where all the individual AI stuff happens.
// * the plan is to use a bunch of state table to control the behaviors of the
// * various critter types
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#pragma once

#include <vector>

#include "GL/glut.h"

#include "AiSpecies.h"
#include "WorldMap.h"
#include "Physics.h"
#include "ItemManager.h"
#include "MathUtil.h"


#define DONT_ATTACK_PLAYER		(0)
#define PLAYER_SPECIES			(AITYPE_HUMAN)


class AiEntity {
public:
	AiEntity(void);

	void update(
		double time,
		WorldMap& worldMap,
		Physics& physics,
		vector<AiEntity*>& aiEntities,
		ItemManager& itemManager);
	// this is called only if the entity is a AITYPE_PLAYUH
	void updatePlayer(Physics &physics);

	void updateState(
		double time,
		WorldMap& worldMap,
		Physics& physics,
		vector<AiEntity*>& aiEntities,
		ItemManager& itemManager);

	bool testCondition(
		int condition,
		double time,
		WorldMap& worldMap,
		Physics& physics,
		vector<AiEntity*>& aiEntities,
		ItemManager& itemManager);

	void readMail(Physics &physics);


	// this is mostly slated to be paramaterized and moved out of here.
	// good riddance!
	void updateBalloon(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities, ItemManager& itemManager);
	void updateHopper(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities, ItemManager& itemManager);
	void updateDummy(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities);

	void updateTarget(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities);
	void aquireTarget(double time, WorldMap& worldMap, Physics& physics, vector<AiEntity*>& aiEntities);

	bool useWeapon(double time, Physics& physics, ItemManager& itemManager);
	bool isTargetInRange(int gunType, double distanceToTarget);

// members * * * * * * * * * * * * * * * * *
	int mActive;

	int mType;

	v3d_t mWorldPosition;

	size_t mHandle;
	size_t mPhysicsHandle;
	size_t mTargetPhysicsHandle;

	PhysicsEntity *mPhysicsEntity;
	PhysicsEntity *mTargetPhysicsEntity;

	float mMaxHealth;
	float mCurrentHealth;

	int mCurrentState;

#define mMinDistanceToPlayer  370.0

	double mNextShotTime;
	double mNextUpdateTime;

	static const int AI_INVENTORY_SIZE = 5;
	size_t mInventory[AI_INVENTORY_SIZE];

	int mCurrentWeapon;

	// behavior stuff
	bool mWasAttacked;
	int mAttackerHandle;
	bool mWillAttackSameSpecies;


	double mFacingAngle;
	double mLookIncline;
	v3d_t mHeadOrientation;
};

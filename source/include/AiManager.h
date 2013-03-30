// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiManager
// *
// * this manages the AiEntity(s). it spawns, manages the list, etc...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *



#ifndef AiManager_h_
#define AiManager_h_

#include <vector>

#include <Windows.h>

#include "GL/glut.h"

#include "v3d.h"

#include "World.h"
#include "Physics.h"
#include "ItemManager.h"
#include "AiEntity.h"


class AiManager {
public:
	AiManager (void);
	~AiManager (void);
	
	void clear(void);

	size_t setPlayerPhysicsHandle (size_t playerPhysicsHandle, physics_c &physics, double time);
	void setPlayerFacingAndIncline (v2d_t facingAndElevation);

	AiEntity *addEntity(int type, const v3d_t &position, physics_c &physics, double time);
	void removeEntity(size_t handle);
	AiEntity *getEntityByHandle(size_t handle);
	vector<AiEntity *> *getEntities(void);

	size_t spawnEntity (int type, v3d_t position, double time,
		physics_c &physics, ItemManager &itemManager);

	int update (double time, WorldMap &worldMap, physics_c &physics, ItemManager &itemManager);

	void releaseItems(int index, physics_c &physics);
	void trimEntitiesList (void);

	void readPhysicsMessages (double time, physics_c &physics, ItemManager &itemManager);

	vector<size_t> getAllItemHandles(void);

	void setMaxCritters(int num);

private:
	// copy constructor guard
	AiManager (const AiManager &aiManager) { }
	// assignment operator guard
	AiManager & operator=(const AiManager &aiManager) { return *this; }

	size_t mNextHandle;

	size_t mPlayerAiHandle;
	size_t mPlayerPhysicsHandle;

	vector<AiEntity *> mAiEntities;

	int mMaxCrittersHACK;

};





#endif // AiManager_h_

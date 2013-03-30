// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PhysicsView
// *
// * displays the contents of the Physics class
// * part of the Model View Controller pattern
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef PhysicsView_h_
#define PhysicsView_h_

#include "Physics.h"


enum {
	OBJSKIN_ITEM,
	OBJSKIN_AMMO_BULLET,
	OBJSKIN_AMMO_SLIME,
	OBJSKIN_AMMO_PLASMA,
	OBJSKIN_AMMO_NAPALM,
	OBJSKIN_HEALTHPACK,
	OBJSKIN_GRENADE,

	NUM_OBJSKINS
};



class PhysicsView {

public:
	PhysicsView();
	~PhysicsView();

	void loadAssets (void);
	void freeAssets (void);

	void setViewPosition(v3d_t pos);

	// some things are time based, this allows for that
	void update(vector<phys_entity_t> *physicsEntities, double time);
	void setEntityColor (phys_entity_t &entity, double time);

	void drawSolidEntities (vector<phys_entity_t> *physicsEntities, WorldMap &worldMap, AssetManager &assetManager);
	void drawTransparentEntities (vector<phys_entity_t> *physicsEntities, AssetManager &assetManager, bool inWater);

	void drawEntity (const phys_entity_t &entity);
	void drawTextured (const phys_entity_t &entity, WorldMap &worldMap, AssetManager &assetManager);
	void drawLitBox (v3d_t nearCorner, v3d_t farCorner, WorldMap &worldMap, AssetManager &assetManager);
	void drawBillBoard(v3d_t pos);

private:
	GLuint mTextureHandles[NUM_OBJSKINS];
	int mBlankBlockCallListHandle;

	v3d_t mViewPosition;
};





#endif // PhysicsView_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * player_c
// *
// * this contains all (way too much) player related stuff:
// * inventory
// * camera info
// * movement
// * 
// * etc...
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Player_h_
#define Player_h_

#include <cmath>

#include "MathUtil.h"
#include "v2d.h"
#include "v3d.h"
#include "GlCamera.h"

#include "GameInput.h"
#include "Physics.h"
#include "Menu.h"
#include "ItemManager.h"
#include "Inventory.h"
#include "HeadBobble.h"

#include "LightManager.h"


// orientation defaults
#define DEFAULT_POS_X			(30.0)
#define DEFAULT_POS_Y			(30.0)
#define DEFAULT_POS_Z			(30.0)

#define DEFAULT_FACING			(45.0)
#define DEFAULT_INCLINE			(0.0)
#define DEFAULT_INCLINE_MIN		(-88.0)
#define DEFAULT_INCLINE_MAX		(88.0)


enum {
	EQUIP_PRIMARY,
	EQUIP_SECONDARY
};


#define DEFAULT_BACKPACK_SIZE	(15)



class player_c {
public:
	player_c ();
	~player_c ();

	int reset(size_t physicsHandle, size_t aiHandle, ItemManager &itemManager);
	int soft_reset(v3d_t &startPosition, Physics &phys);
	void setStartPosition(v3d_t &startPosition);
	void setPhysicsHandle(size_t handle);

	void godMode(ItemManager &itemManager);

	// update the camera target
	void update_target(void);
	int constrain_view_angles(void);

	v2d_t getFacingAndIncline(void);
	bool isHeadInWater(void);

	int set_target(v3d_t a);

	void set_draw_distance(double distance);
	void adjust_draw_distance(double amount);

	// use the gluLookAt () to set view at render time
	gl_camera_c gl_cam_setup(void);

	v3d_t get_pos(void);
	int set_pos(v3d_t a);

	bool pickUpItem(item_t item, AssetManager &assetManager);

	void useEquipped(int whichEquip, double time, Physics &phys,
		AssetManager &assetManager, ItemManager &itemManager);
	double fireGun(item_t item, double handedness, double time, Physics &phys, AssetManager &assetManager, ItemManager &itemManager);
	double useMeleeWeapon(item_t item, double time, Physics &phys, ItemManager &itemManager);

	void useBackpackItem(double time, Physics &phys, AssetManager &assetManager, ItemManager &itemManager);

	v2d_t obtainWalkVector(v2d_t walkInput);

	void updateTargetBlock(WorldMap &worldMap);
	v3di_t *getTargetBlock(int &targetBlockFace);

	void updateHud(ItemManager &itemManager);
	void drawHud(void);
	void drawWaterOverlay(void);
	void drawEquipped(ItemManager &itemManager, BitmapModel &model);
	void drawEquippedGun(double handedness, BitmapModel &model);

	void updateCharacterSheet(ItemManager &itemManager);

	bool update(double time, WorldMap &worldMap, Physics &phys, GameInput &gi, AssetManager &assetManager, ItemManager &itemManager);

	void readPhysicsMessages(Physics &physics, ItemManager &itemManager, AssetManager &assetManager);

	// HACK
	void placeLight(LightManager &lightManager, WorldMap &worldMap, GameInput &gi);
	Inventory *getInventory(void);

	gl_camera_c cam;

private:
	menu_c mHud;

	menu_c mCharacterSheet;
	bool mShowCharacterSheet;

	v3d_t mPos;			// where the 'feet' are

	v3d_t mHeadOffset;			// where's the noggin relative to the phys position
	v3d_t mFinalHeadOffset;		// where is it after all is considered
	HeadBobble mHeadBobble;		// something to consider

	v3d_t mTarget;			// where the camera is pointed
	v3d_t up;				// the 'up' vector for the camera
	v3d_t mLookVector;

	bool mIsBlockTargeted;
	v3di_t mTargetBlock;
	int mTargetBlockFace;

	size_t mPhysicsHandle;
	size_t mAiHandle;

	bool mStartLocationFound;
	v3d_t mStartPosition;

	double mMaxHealth;
	double mCurrentHealth;

	bool deathScreamUttered;

	double mLastFootStep;

	double mLastUpdateTime;

	double mNextShotTimePrimary;
	double mNextShotTimeSecondary;

	Inventory mInventory;

	melee_weapon_state_t mMeleeStatePrimary;
	melee_weapon_state_t mMeleeStateSecondary;

	v2d_t mWalkInput;

	bool mInWater;
	bool mHeadInWater;

	GLfloat mVisionTint[4];

	double mFacing;			// the angle on the x-z plane off the positive x-axis
	double mIncline;		// the angle on the vertical plane off the x-z plane intersection...
	double mInclineMin;		// upper constraint for incline angle
	double mInclineMax;		// lower constraint for incline angle

	bool mShowWorldChunkBoxes;

	bool mPlacedBlock;

	static const int LEFT_HANDED = -1;
	static const int RIGHT_HANDED = 1;
};




#endif // Player_h_

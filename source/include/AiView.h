// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiView
// *
// * displays the AiManager/AiEntities
// * part of the Model View Controller pattern
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef AiView_h_
#define AiView_h_


#include "AiEntity.h"
#include "AssetManager.h"
#include "MathUtil.h"
#include "LightManager.h"


enum {
	AISKIN_DUMMY,
	AISKIN_BALLOON,
	AISKIN_BALLOON_DEAD,
	AISKIN_SHOOTER,
	AISKIN_SHOOTER_DEAD,
	AISKIN_HOPPER,
	AISKIN_HOPPER_DEAD,
	AISKIN_HUMAN,
	AISKIN_HUMAN_DEAD,

	NUM_AISKINS
};



class AiView {
public:
	AiView();
	~AiView();

	void setAiEntities(vector<AiEntity *> *aiEntities);

	void loadAssets(void);
	void freeAssets(void);

	void draw(
		WorldMap &worldMap,
		ItemManager &itemManager,
		const LightManager &lightManager);

	void drawEntity(
		WorldMap &worldMap,
		ItemManager &itemManager,
		AiEntity &aiEntity,
		const LightManager &lightManager);

	void updateHeadOrientation(AiEntity &aiEntity) const;
	void updateFacingAngle(AiEntity &aiEntity, double desiredLookIncline) const;
	void updateLookIncline(AiEntity &aiEntity, double desiredLookIncline) const;

	void drawBody(AiEntity &aiEntity, WorldMap &worldMap, const LightManager &lightManager);
	void drawHead(AiEntity &aiEntity, WorldMap &worldMap, const LightManager &lightManager);
	void drawWeapon(AiEntity &aiEntity, WorldMap &worldMap, ItemManager &itemManager);


	void drawBodyBlock(IntColor color) const;
	void drawBlockWithFace (IntColor color) const;

	IntColor getLightValue(v3d_t position, WorldMap &worldMap, const LightManager &lightManager) const;

private:
// members * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// this points to the critter we'll be drawing
	vector<AiEntity *> *mAiEntities;

	// these hold the texture handles for the skins
	// FIXME: how about atlas textures? already basically doing it
	// * * * heads
	GLuint mHeadTextureHandles[NUM_AITYPES];
	// * * * bodies
	GLuint mTextureHandles[NUM_AISKINS];

};





#endif // AiView_h_

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * PhysicsView
// *
// * displays the contents of the Physics class
// * part of the Model View Controller pattern
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../physics/Physics.h"

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

  void loadAssets();
  void freeAssets();

  void setViewPosition(v3d_t pos);

  // some things are time based, this allows for that
  void update(vector<PhysicsEntity*>* physicsEntities, double time);
  void setEntityColor(PhysicsEntity& entity, double time);

  void drawSolidEntities(vector<PhysicsEntity*>* physicsEntities, WorldMap& worldMap, AssetManager* assetManager);
  void drawTransparentEntities(vector<PhysicsEntity*>* physicsEntities, AssetManager* assetManager, bool inWater);

  void drawEntity(const PhysicsEntity& entity);
  void drawTextured(const PhysicsEntity& entity, WorldMap& worldMap, AssetManager* assetManager);
  void drawLitBox(v3d_t nearCorner, v3d_t farCorner, WorldMap& worldMap, AssetManager* assetManager);
  void drawBillBoard(v3d_t pos);

private:
  GLuint mTextureHandles[NUM_OBJSKINS];
  int mBlankBlockCallListHandle;

  v3d_t mViewPosition;
};

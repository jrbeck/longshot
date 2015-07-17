// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * ItemManager
// *
// * this manages the items in the game. it is an awkward interface between the users
// * and the used items and the physics engine, etc...
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "GameModel.h"

#include "Physics.h"
#include "MathUtil.h"
#include "ObjectLoader.h"

#include "Constants.h"

#define ITEM_NAME_LENGTH  64


typedef struct {
  float healthMax;
  float healthCurrent;
  float healthRegen;

  float energyMax;
  float energyCurrent;
  float energyRegen;

  float damageRanged;
  float damageMelee;

  float forceWalk;
  float forceSwim;

  float armor;
} playerstats_t;


typedef struct {
  size_t ownerPhysicsHandle;
  v3d_t position;
  v3d_t angle;
  double time;

  double facing;
  double incline;
} shot_info_t;


//enum {
//  MODELTYPE_MACE,
//  MODELTYPE_AXE,
//
//  NUM_MODELS
//};


struct item_t {
  bool active;
  size_t handle;
  int type;

  char name[ITEM_NAME_LENGTH];

  double value1;

  // gun stuff
  int gunType;
  int ammoType;
  int bulletType;
  double shotDelay;
  int bulletsPerShot;
  double bulletDamage;
  double bulletSpread;
  double bulletForce;
  double explosionForce;
  int numParticles;
  int shotSound;

  int quantity;

  double mass;
};



class GameModel;
class AssetManager;


class ItemManager {
public:
  ItemManager(GameModel* gameModel);
  ~ItemManager();

  int update();

  void readPhysicsMessages();

  void clear();

  void destroyItemList(vector<size_t> itemList);
  void destroyItem(size_t handle);
  void trimItemsList();

  //  int addItem (item_t item);
  item_t getItem(size_t itemHandle);
  int getIndexFromHandle(size_t itemHandle);

  int getItemType(size_t itemHandle);

  size_t createItem(item_t item);
  size_t generateRandomItem();

  size_t generateRandomMeleeWeapon(double dps);


  void nameGun(item_t& gun);
  size_t generateRandomGun(double dps);

  void generateRandomPistol(item_t &newGun, double dps);
  void generateRandomRifle(item_t &newGun, double dps);
  void generateRandomShotgun(item_t &newGun, double dps);
  void generateRandomMachineGun(item_t &newGun, double dps);
  void generateRandomGrenadeLauncher(item_t &newGun, double dps);
  void generateRandomRocketLauncher(item_t &newGun, double dps);

  size_t spawnPhysicsEntityGun(double value, v3d_t position);

  bool useItem(v2d_t walkVector, size_t itemHandle, size_t physicsHandle);
  void useRocketPack(v2d_t walkVector, size_t itemHandle, size_t physicsHandle);

  double useGun(size_t itemHandle, const shot_info_t& shotInfo, size_t* ammoCounter);
  double useGun(size_t itemHandle, const shot_info_t& shotInfo);

  double useMeleeWeapon(size_t itemHandle, const shot_info_t& shotInfo);

  // REMINDER: this should really see how many items are no longer
  // around, perhaps on load?
  void save(FILE* file);
  void load(FILE* file);

private:
  // copy constructor guard
  ItemManager(const ItemManager& itemManager) { }
  // assignment operator guard
  ItemManager& operator=(const ItemManager& itemManager) { return *this; }

  GameModel* mGameModel;

  size_t mLastHandle;
  vector<item_t> mItems;

  int mAmmoToPhysicsTable[NUM_AMMO_TYPES];
};

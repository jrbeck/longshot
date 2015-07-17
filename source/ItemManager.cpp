#include "ItemManager.h"

// WARNING: maybe i should pay attention to this
// has to do with sprintf() not being safe....
#pragma warning (disable : 4996)


ItemManager::ItemManager(GameModel *gameModel) :
  mLastHandle(1)
{
  mGameModel = gameModel;

  // initialize the conversion table
  mAmmoToPhysicsTable[AMMO_BULLET] = OBJTYPE_LIVE_BULLET;
  mAmmoToPhysicsTable[AMMO_SLIME] = OBJTYPE_SLIME;
  mAmmoToPhysicsTable[AMMO_PLASMA] = OBJTYPE_PLASMA;
  mAmmoToPhysicsTable[AMMO_NAPALM] = OBJTYPE_NAPALM;
}

ItemManager::~ItemManager() {
  mItems.clear();
}

int ItemManager::update() {
  readPhysicsMessages();
  trimItemsList();
  return static_cast<int>(mItems.size());
}

void ItemManager::readPhysicsMessages() {
  phys_message_t message;

  int itemsDestroyed = 0;

  size_t itemHandle;
  int itemIndex;

  while (mGameModel->physics->getNextMessage(MAILBOX_ITEMMANAGER, &message)) {
    switch (message.type) {
      case PHYS_MESSAGE_ITEM_DESTROYED:
        itemHandle = message.iValue;
        itemIndex = getIndexFromHandle(itemHandle);
        if (itemIndex >= 0) {
          mItems[itemIndex].active = false;
          itemsDestroyed++;
        }

        break;

      default:
        break;
    }
  }
}

void ItemManager::clear() {
  mItems.clear();
  mLastHandle = 1;
}

void ItemManager::destroyItemList(vector<size_t> itemList) {
  int numItemsDestroyed = 0;
  for (size_t i = 0; i < itemList.size(); i++) {
    int index = getIndexFromHandle(itemList[i]);
    if (index >= 0) {
      mItems[index].active = false;
//      printf("d: %d\n", mItems[index].handle);
      numItemsDestroyed++;
    }
  }
  printf("ItemManager::destroyItemList(): %d items destroyed\n", numItemsDestroyed);
}

void ItemManager::destroyItem(size_t handle) {
  int index = getIndexFromHandle(handle);
  if (index >= 0) {
    mItems[index].active = false;
  }
  else {
    printf("ItemManager::destroyItem(): warning: could not find item\n");
  }
}

void ItemManager::trimItemsList() {
  if (mItems.size() == 0) {
    return;
  }

  for (int i = 0; i <= ((int)mItems.size() - 1); i++) {
    if (!mItems[i].active) {
      swap(mItems[i], mItems[mItems.size () - 1]);
      mItems.pop_back();
    }
  }
}

item_t ItemManager::getItem(size_t itemHandle) {
//  assert(itemHandle > 0);
  if (itemHandle == 0) {
    item_t ret;
    ret.type = ITEMTYPE_UNDEFINED;
    return ret;
  }

  int itemIndex = getIndexFromHandle(itemHandle);
  if (itemIndex < 0) {
    item_t ret;
    ret.type = ITEMTYPE_UNDEFINED;
    return ret;
  }

  return mItems[itemIndex];
}

int ItemManager::getIndexFromHandle(size_t itemHandle) {
  for (int index = 0; index < static_cast<int>(mItems.size()); index++) {
    if (mItems[index].handle == itemHandle) {
      return index;
    }
  }

  // can't find it yo
  return -1;
}

int ItemManager::getItemType(size_t itemHandle) {
  for (size_t index = 0; index < mItems.size(); index++) {
    if (mItems[index].handle == itemHandle) {
      return mItems[index].type;
    }
  }

  // can't find it yo
  return ITEMTYPE_UNDEFINED;
}

size_t ItemManager::createItem(item_t item) {
  item.active = true;
  item.handle = ++mLastHandle;
  mItems.push_back(item);
  return item.handle;
}

size_t ItemManager::generateRandomItem() {
  item_t newItem;
  memset(&newItem, 0, sizeof (item_t));

  newItem.type = r_numi (0, NUM_ITEMTYPES);

  switch (newItem.type) {
    case ITEMTYPE_HEALTHPACK:
      newItem.value1 = 10.0;
      sprintf(newItem.name, "health pack 10.0");
      break;

    case ITEMTYPE_ROCKET_PACK:
      if (r_numi(0, 30) > 27) {
        newItem.value1 = r_num(800.0, 1300.0);
        sprintf(newItem.name, "rocket pack");
      }
      else {
        newItem.type = ITEMTYPE_AMMO_BULLET;
        newItem.quantity = r_numi(6, 96);
        newItem.ammoType = r_numi(0, NUM_AMMO_TYPES);
        sprintf(newItem.name, "ammo");
      }
      break;

    case ITEMTYPE_AMMO_BULLET:
      newItem.quantity = r_numi(6, 96);
      newItem.ammoType = AMMO_BULLET;
      sprintf(newItem.name, "bullet ammo");
      break;

    case ITEMTYPE_AMMO_SLIME:
      newItem.quantity = r_numi(6, 96);
      newItem.ammoType = AMMO_SLIME;
      sprintf(newItem.name, "slime ammo");
      break;

    case ITEMTYPE_AMMO_PLASMA:
      newItem.quantity = r_numi(6, 96);
      newItem.ammoType = AMMO_PLASMA;
      sprintf(newItem.name, "plasma ammo");
      break;

    case ITEMTYPE_AMMO_NAPALM:
      newItem.quantity = r_numi(6, 96);
      newItem.ammoType = AMMO_NAPALM;
      sprintf(newItem.name, "napalm ammo");
      break;

    case ITEMTYPE_MELEE_ONE_HANDED:
      if (r_numi(0, 30) > 15) {
        return generateRandomMeleeWeapon(4.0);
      }
      else {
        newItem.type = ITEMTYPE_AMMO_BULLET;
        newItem.quantity = r_numi(6, 96);
        newItem.ammoType = r_numi(0, NUM_AMMO_TYPES);
        sprintf(newItem.name, "ammo");
      }
      break;

    default:
      return 0;
  }

  return createItem(newItem);
}

size_t ItemManager::generateRandomMeleeWeapon(double dps) {
  item_t newItem;
  memset (&newItem, 0, sizeof (item_t));

  newItem.type = ITEMTYPE_MELEE_ONE_HANDED;
  newItem.gunType = r_numi(0, NUM_MELEETYPES);

  newItem.shotDelay = r_num(0.5, 1.0);
  newItem.bulletDamage = dps * newItem.shotDelay;

  switch (newItem.gunType) {
    case MELEETYPE_MACE:
      newItem.explosionForce = 50000.0;
      sprintf(newItem.name, "mace");
      break;
    case MELEETYPE_AXE:
      newItem.explosionForce = 10000.0;
      sprintf(newItem.name, "axe");
      break;

    default:
      return 0;
  }

  return createItem(newItem);
}

void ItemManager::nameGun(item_t &gun) {
  char gunName[15];

  switch (gun.gunType) {
    case GUNTYPE_PISTOL:
      sprintf(gunName, "pistol");
      break;

    case GUNTYPE_RIFLE:
      sprintf(gunName, "rifle");
      break;

    case GUNTYPE_SHOTGUN:
      sprintf(gunName, "shotgun");
      break;

    case GUNTYPE_MACHINE_GUN:
      sprintf(gunName, "machine gun");
      break;

    case GUNTYPE_GRENADE_LAUNCHER:
      sprintf(gunName, "grenade");
      break;

    case GUNTYPE_ROCKET_LAUNCHER:
      sprintf(gunName, "rocket");
      break;

    default:
      // FIXME: default?
      printf("ItemManager::nameGun(): default gun?\n");
      sprintf(gunName, "error");
      break;
  }

  char projectileName[15];

  switch (gun.ammoType) {
    case AMMO_BULLET:
      sprintf(projectileName, "bullet");
      break;

    case AMMO_SLIME:
      sprintf(projectileName, "slime");
      break;

    case AMMO_PLASMA:
      sprintf(projectileName, "plasma");
      break;

    case AMMO_NAPALM:
      sprintf(projectileName, "napalm");
      break;

    default:
      // FIXME: default?
      printf("ItemManager::nameGun(): how did you get here?\n");
      sprintf(projectileName, "error");
      break;
  }

  // FIXME: now this is a finicky thing... hrm?
  double dps = gun.bulletDamage * static_cast<double>(gun.bulletsPerShot) / gun.shotDelay;

  sprintf(gun.name, "%s %s dps: %2.1f\0", projectileName, gunName, dps);
}

size_t ItemManager::generateRandomGun(double dps) {
  item_t newGun;

  newGun.type = ITEMTYPE_GUN_ONE_HANDED;

  int projectileType = r_numi(0, NUM_AMMO_TYPES);
  int deliveryMethod = r_numi(0, NUM_GUNTYPES);

  newGun.ammoType = projectileType;
  // this is the final bulletType
  newGun.bulletType = mAmmoToPhysicsTable[projectileType];

  switch (deliveryMethod) {
    case GUNTYPE_PISTOL:
      generateRandomPistol(newGun, dps);
      break;

    case GUNTYPE_RIFLE:
      generateRandomRifle(newGun, dps);
      break;

    case GUNTYPE_SHOTGUN:
      generateRandomShotgun(newGun, dps);
      break;

    case GUNTYPE_MACHINE_GUN:
      generateRandomMachineGun(newGun, dps);
      break;

    case GUNTYPE_GRENADE_LAUNCHER:
      // not so fast buddy - these need to be a bit less common
      if (r_numi(0, 6) == 3) {
        generateRandomGrenadeLauncher(newGun, dps);
      }
      else {
        generateRandomMachineGun(newGun, dps);
      }
      break;

    case GUNTYPE_ROCKET_LAUNCHER:
      // not so fast buddy - these need to be a bit less common
      if (r_numi(0, 8) == 2) {
        generateRandomRocketLauncher(newGun, dps);
      }
      else {
        generateRandomShotgun(newGun, dps);
      }
      break;

    default:
      // FIXME: default?
      printf("error: ItemManager::generateRandomGun(): default gun?\n");
      generateRandomPistol(newGun, dps);
      break;
  }

  // now lets adjust a bit...
  switch (projectileType) {
    case AMMO_BULLET:
      // ahh... the gold standard!
      break;

    case AMMO_SLIME:
      if (deliveryMethod == GUNTYPE_GRENADE_LAUNCHER ||
        deliveryMethod == GUNTYPE_ROCKET_LAUNCHER) {
        newGun.explosionForce *= 0.2;
      }

      break;

    case AMMO_PLASMA:
      if (deliveryMethod == GUNTYPE_PISTOL) {
        newGun.shotSound = SOUND_PLASMA_PISTOL;
      }
      else if (deliveryMethod == GUNTYPE_RIFLE) {
        newGun.bulletType = OBJTYPE_PLASMA_BOMB;
        newGun.shotSound = SOUND_PLASMA_GUN;
      }
      else if (deliveryMethod == GUNTYPE_GRENADE_LAUNCHER ||
        deliveryMethod == GUNTYPE_ROCKET_LAUNCHER) {
        newGun.explosionForce *= 0.3;
      }

      break;

    case AMMO_NAPALM:
      if (deliveryMethod == GUNTYPE_GRENADE_LAUNCHER ||
        deliveryMethod == GUNTYPE_ROCKET_LAUNCHER) {
        newGun.explosionForce *= 0.1;
      }

      break;

    default:
      // FIXME: default?
      printf("ItemManager::generateRandomGun(): how did you get here?\n");
      projectileType = AMMO_BULLET;

      break;
  }

  nameGun(newGun);
  size_t itemHandle = createItem(newGun);
  return itemHandle;
}

void ItemManager::generateRandomPistol(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_PISTOL;
  newGun.bulletForce = r_num(10000.0, 12000.0);
  newGun.explosionForce = r_num(500.0, 600.0);
  newGun.bulletSpread = r_num(0.0001, 0.015);
  newGun.bulletsPerShot = 1;
  newGun.shotDelay = r_num(0.4, 0.8);
  newGun.shotSound = SOUND_PISTOL;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

void ItemManager::generateRandomRifle(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_RIFLE;
  newGun.bulletForce = r_num(15000.0, 17500.0);
  newGun.explosionForce = r_num(500.0, 600.0);
  newGun.bulletSpread = r_num(0.0001, 0.001);
  newGun.bulletsPerShot = 1;
  newGun.shotDelay = r_num(0.6, 1.8);
  newGun.shotSound = SOUND_SHOTGUN;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

void ItemManager::generateRandomShotgun(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_SHOTGUN;
  newGun.bulletForce = r_num(7500.0, 9500.0);
  newGun.explosionForce = r_num(500.0, 600.0);
  newGun.bulletSpread = r_num(0.02, 0.06);
  newGun.bulletsPerShot = r_numi(5, 15);
  newGun.shotDelay = r_num(0.5, 1.0);
  newGun.shotSound = SOUND_SHOTGUN;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

void ItemManager::generateRandomMachineGun(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_MACHINE_GUN;
  newGun.bulletForce = r_num(12000.0, 14000.0);
  newGun.explosionForce = r_num(500.0, 600.0);
  newGun.bulletSpread = r_num(0.005, 0.10);
  newGun.bulletsPerShot = 1;
  newGun.shotDelay = r_num(0.05, 0.1);
  newGun.shotSound = SOUND_MACHINE_GUN;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

void ItemManager::generateRandomGrenadeLauncher(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_GRENADE_LAUNCHER;
  newGun.bulletForce = r_num(4500.0, 5800.0);
  newGun.explosionForce = r_num(2600.0, 5000.0);
  newGun.bulletSpread = r_num(0.000, 0.005);
  newGun.bulletsPerShot = 1;
  newGun.shotDelay = r_num(1.2, 1.7);
  newGun.numParticles = r_numi(30, 50);
  newGun.shotSound = SOUND_GRENADE_LAUNCH;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

void ItemManager::generateRandomRocketLauncher(item_t& newGun, double dps) {
  newGun.gunType = GUNTYPE_ROCKET_LAUNCHER;
  newGun.bulletForce = r_num(4500.0, 4800.0);
  newGun.explosionForce = r_num(2600.0, 5000.0);
  newGun.bulletSpread = r_num(0.000, 0.005);
  newGun.bulletsPerShot = 1;
  newGun.shotDelay = r_num(1.2, 1.7);
  newGun.numParticles = r_numi(30, 50);
  newGun.shotSound = SOUND_GRENADE_LAUNCH;

  // set damage according to dps
  newGun.bulletDamage = (dps * newGun.shotDelay) / static_cast<double>(newGun.bulletsPerShot);
}

size_t ItemManager::spawnPhysicsEntityGun(double value, v3d_t position) {
  size_t itemHandle = generateRandomGun(value);
  PhysicsEntity* physicsEntity = mGameModel->physics->createEntity(OBJTYPE_ITEM, position, false);
  mGameModel->physics->setItemHandleAndType(physicsEntity->handle, itemHandle, getItemType(itemHandle));
  return itemHandle;
}

// returns whether or not the item was destroyed
bool ItemManager::useItem(v2d_t walkVector, size_t itemHandle, size_t physicsHandle) {
  item_t item = getItem(itemHandle);

  if (item.type == ITEMTYPE_UNDEFINED) {
    printf("ItemManager::useItem() - error: tried to use a bad item handle\n");
    return false;
  }

  phys_message_t message;
  switch (item.type) {
    case ITEMTYPE_HEALTHPACK:
      message.sender = MAILBOX_ITEMMANAGER;
      message.recipient = MAILBOX_PHYSICS;
      message.type = PHYS_MESSAGE_ADJUSTHEALTH;
      message.iValue = physicsHandle;
      message.dValue = item.value1;

      // tell the physics about this
      mGameModel->physics->sendMessage(message);
      destroyItem(itemHandle);
      return true;

    case ITEMTYPE_ROCKET_PACK:
      useRocketPack(walkVector, itemHandle, physicsHandle);
      return false;

    default:
      break;
  }

  return false;
}

void ItemManager::useRocketPack(
  v2d_t walkVector,
  size_t itemHandle,
  size_t physicsHandle)
{
  item_t item = getItem(itemHandle);

  // check if in water
  // FIXME: check to see if head is in water??
  PhysicsEntity *physicsEntity = mGameModel->physics->getEntityByHandle(physicsHandle);
  if (physicsEntity->worldViscosity > 0.0) {
    return;
  }

  // handle 'air walking'
  v2d_t walk_force_2d = v2d_scale(walkVector, 300.0);
  v3d_t force = v3d_v(walk_force_2d.x, 0.0, walk_force_2d.y);
  mGameModel->physics->add_force(physicsHandle, force);

  // now for the real deal
  double rocketForce = item.value1;
  v3d_t movementForce = v3d_v(1.0 * walkVector.x, 1.0, 1.0 * walkVector.y);
  double topSpeed = 6.0;
  double taperLength = 5.0;
  if (v3d_mag(physicsEntity->vel) > topSpeed) {
    double diff = v3d_mag(physicsEntity->vel) - topSpeed;
    double thrustModifier = -rocketForce * diff / taperLength;
    v3d_t drag = v3d_scale(v3d_normalize(physicsEntity->vel), thrustModifier);
    mGameModel->physics->add_force(physicsHandle, drag);
  }

  v3d_t normalizedForce = v3d_normalize(movementForce);
  movementForce = v3d_scale(rocketForce, normalizedForce);
  mGameModel->physics->add_force(physicsHandle, movementForce);

  // how about some smoke...
  if (r_numi(0, 10) > 5) {
    v3d_t smokePosition;
    smokePosition = mGameModel->physics->getCenter(physicsEntity->handle);
    smokePosition.y -= 0.8;
    v3d_t force = v3d_v(r_num(-10.0, 10.0), r_num(-100.0, -50.0), r_num(-10.0, 10.0));
    mGameModel->physics->createEntity(OBJTYPE_SMOKE, smokePosition, force, true);
  }
}

double ItemManager::useGun(
  size_t itemHandle,
  const shot_info_t& shotInfo,
  size_t* ammoCounter)
{
  int itemIndex = getIndexFromHandle(itemHandle);

  if (mItems[itemIndex].type != ITEMTYPE_GUN_ONE_HANDED && mItems[itemIndex].type != ITEMTYPE_GUN_TWO_HANDED) {
    printf ("ItemManager::useGun() - warning! received non-gun item index\n");
    return -1.0;
  }

  int ammoType = mItems[itemIndex].ammoType;
  if (ammoType >= NUM_AMMO_TYPES) {
    printf("error: useGun() - invalid ammo type: %d\n", ammoType);
    return shotInfo.time;
  }

  int bullets = mItems[itemIndex].bulletsPerShot;
  if (mItems[itemIndex].gunType == GUNTYPE_GRENADE_LAUNCHER || mItems[itemIndex].gunType == GUNTYPE_ROCKET_LAUNCHER) {
    bullets = 10;
  }
  else if (mItems[itemIndex].gunType == GUNTYPE_SHOTGUN) {
    bullets = 3;
  }

  if (ammoCounter[ammoType] > bullets) {
    ammoCounter[ammoType] -= bullets;
    return useGun(itemHandle, shotInfo);
  }
  else {
    return shotInfo.time;
  }
}

// expects shotInfo angle to be normalized
double ItemManager::useGun(size_t itemHandle, const shot_info_t& shotInfo) {
  int itemIndex = getIndexFromHandle(itemHandle);
  if (itemIndex < 0) {
    return shotInfo.time;
  }

  for (int i = 0; i < mItems[itemIndex].bulletsPerShot; i++) {
    v3d_t finalAngle = shotInfo.angle;
    finalAngle.x += r_num(-mItems[itemIndex].bulletSpread, mItems[itemIndex].bulletSpread);
    finalAngle.y += r_num(-mItems[itemIndex].bulletSpread, mItems[itemIndex].bulletSpread);
    finalAngle.z += r_num(-mItems[itemIndex].bulletSpread, mItems[itemIndex].bulletSpread);

    v3d_t shotForce = v3d_scale(mItems[itemIndex].bulletForce, finalAngle);
    int bulletType;

    if (mItems[itemIndex].gunType == GUNTYPE_GRENADE_LAUNCHER) {
      bulletType = OBJTYPE_GRENADE;
    }
    else if (mItems[itemIndex].gunType == GUNTYPE_ROCKET_LAUNCHER) {
      bulletType = OBJTYPE_ROCKET;
    }
    else {
      bulletType = mItems[itemIndex].bulletType;
    }

    PhysicsEntity* projectileEntity = mGameModel->physics->createEntity(bulletType, shotInfo.position, shotForce, true);
    projectileEntity->owner = shotInfo.ownerPhysicsHandle;
    projectileEntity->impactDamage = mItems[itemIndex].bulletDamage;
    projectileEntity->explosionForce = mItems[itemIndex].explosionForce;

    if (mItems[itemIndex].gunType == GUNTYPE_GRENADE_LAUNCHER || mItems[itemIndex].gunType == GUNTYPE_ROCKET_LAUNCHER) {
      projectileEntity->type2 = mItems[itemIndex].bulletType;
      projectileEntity->numParticles = mItems[itemIndex].numParticles;
    }
  }

  if (mItems[itemIndex].shotSound >= 0) {
    mGameModel->physics->addSoundEvent(mItems[itemIndex].shotSound, shotInfo.position);
  }

  return shotInfo.time + mItems[itemIndex].shotDelay;
}

double ItemManager::useMeleeWeapon(size_t itemHandle, const shot_info_t& shotInfo) {
  int itemIndex = getIndexFromHandle(itemHandle);

  if (itemIndex < 0) {
    return shotInfo.time;
  }

  for (int i = 0; i < 1; i++) {
    v3d_t finalAngle = shotInfo.angle;
    int bulletType = OBJTYPE_MELEE_ATTACK;
    PhysicsEntity* projectileEntity = mGameModel->physics->createEntity(bulletType, shotInfo.position, true);
    projectileEntity->owner = shotInfo.ownerPhysicsHandle;
    projectileEntity->impactDamage = mItems[itemIndex].bulletDamage;
    projectileEntity->acc = v3d_scale(shotInfo.angle, mItems[itemIndex].explosionForce);
  }

//  if (mItems[itemIndex].shotSound >= 0) {
  mGameModel->physics->addSoundEvent(SOUND_SWOOSH, shotInfo.position);
//  }

  return shotInfo.time + mItems[itemIndex].shotDelay;
}

void ItemManager::save(FILE* file) {
  // keep track of the last handle
  // WARNING: this could be intelligently reset between loads,
  // but it's range makes me hesitate as a non-issue
  fwrite(&mLastHandle, sizeof (size_t), 1, file);

  // get rid of the inactive items
  trimItemsList();

  // write the items themselves
  size_t numItems = mItems.size();

  printf("ItemManager::save() -----\n");
  printf(" saving: %lu items\n", numItems);

  fwrite(&numItems, sizeof (size_t), 1, file);
  for(size_t i = 0; i < numItems; i++) {
    printf("%lu: %s\n", i, mItems[i].name);
    fwrite(&mItems[i], sizeof (item_t), 1, file);
  }
}

void ItemManager::load(FILE* file) {
  fread(&mLastHandle, sizeof (size_t), 1, file);

  mItems.clear();
  size_t numItems;
  fread(&numItems, sizeof (size_t), 1, file);
  for(size_t i = 0; i < numItems; i++) {
    item_t item;
    fread(&item, sizeof (item_t), 1, file);
    mItems.push_back(item);
  }
}

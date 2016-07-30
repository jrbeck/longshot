#include "../game/GameModel.h"

GameModel::GameModel() :
  mPlayer(0),
  mGalaxy(0),
  mPhysics(0),
  mLocation(0),
  mAiManager(0),
  mItemManager(0),
  mMessageBus(0)
{
  mPlayer = new Player(this);
  mGalaxy = new Galaxy();
  mPhysics = new Physics(this);
  mAiManager = new AiManager(this);
  mItemManager = new ItemManager(this);
  mMessageBus = new MessageBus();
}

GameModel::~GameModel() {
  printf("GameModel::~GameModel()");
  printf("world seed: %d\n", mWorldSeed);

  if (mMessageBus != NULL) {
    delete mMessageBus;
  }
  if (mItemManager != NULL) {
    delete mItemManager;
  }
  if (mAiManager != NULL) {
    delete mAiManager;
  }
  if (mLocation != NULL) {
    delete mLocation;
  }
  if (mPhysics != NULL) {
    delete mPhysics;
  }
  if (mGalaxy != NULL) {
    delete mGalaxy;
  }
  if (mPlayer != NULL) {
    delete mPlayer;
  }
}

void GameModel::save() {
  //  Player player;  // save
  // PLAYER * * * * * * *
  saveGameData();

  //  Galaxy *galaxy;  // save
  // GALAXY * * * * * *
  FILE* file;
  file = fopen("save/galaxy.dat", "wb");
  if (file != NULL) {
    mGalaxy->save(file);
    fclose(file);
  }

  //  Location *location;  // save
  // LOCATION * * * * * * *
  saveLocation();

  //  AiManager aiManager;  // save
  // AI STUFF * * * * * * *
  // TODO

  //  physics_c physics;  // save
  // PHYSICS * * * * * * *
  // TODO

  //  ItemManager itemManager;  // save
  // ITEMS * * * * * * *
  file = fopen("save/items.dat", "wb");
  if (file != NULL) {
    mItemManager->save(file);
    // WARNING: this pertains to the player...
    mPlayer->getInventory()->save(file);
    fclose(file);
  }
}

int GameModel::load(GameWindow* gameWindow) {
  //  Player player;  // save
  // PLAYER * * * * * * *
  GameSaveData gameSaveData = loadGameData();
  if (!gameSaveData.loadSucceeded) {
    printf("game_c::load(): UNSUCCESSFUL 1\n");
    return LOAD_UNSUCCESSFUL;
  }

  //  Galaxy *galaxy;  // save
  // GALAXY * * * * * *
  FILE* file;
  file = fopen("save/galaxy.dat", "rb");
  if (file != NULL) {
    mGalaxy->load(file);
    fclose(file);
  }
  else {
    printf(" game_c::load(): UNSUCCESSFUL 2\n");
    return LOAD_UNSUCCESSFUL;
  }

  mCurrentPlanet = mGalaxy->getPlanetByHandle(gameSaveData.planetHandle);

  switch (gameSaveData.locationType) {
  case LOCATION_SHIP:
    printf("game_c::load(): loading ship\n");
    initializeStarShip(true);
    break;
  case LOCATION_WORLD:
    printf("game_c::load(): loading planet\n");
    mCurrentPlanet = mGalaxy->getPlanetByHandle(gameSaveData.planetHandle);
    initializePlanet(true, &gameSaveData.physicsPos, false, gameWindow);
    break;
  default:
    printf("game_c::load(): error, default location\n");
    return LOAD_UNSUCCESSFUL;
  }

  //  AiManager aiManager;  // save
  // AI STUFF * * * * * * *

  //  physics_c physics;  // save
  // PHYSICS * * * * * * *
  mPhysics->set_pos(mPhysics->getPlayerHandle(), gameSaveData.physicsPos);

  //  ItemManager itemManager;  // save
  // ITEMS * * * * * * *
  file = fopen("save/items.dat", "rb");
  if (file != NULL) {
    mItemManager->load(file);
    // WARNING: this pertains to the player...
    mPlayer->getInventory()->load(file);
    fclose(file);
  }

  return LOAD_SUCCESSFUL;
}

int GameModel::saveGameData(void) {
  GameSaveData gameSaveData;
  gameSaveData.loadSucceeded = true;

  gameSaveData.physicsPos = mPhysics->getNearCorner(mPhysics->getPlayerHandle());
  v3d_print("game_c::saveGameData(): saving playerPos, ", gameSaveData.physicsPos);

  gameSaveData.locationType = mLocation->getType();
  gameSaveData.planetHandle = mCurrentPlanet->mHandle;

  // now for the file stuff
  FILE* file = fopen("save/game.dat", "wb");
  if (file == NULL) {
    return -1;
  }
  fwrite(&gameSaveData, sizeof (GameSaveData), 1, file);  // physics position
  fclose(file);
  return 0;
}

GameSaveData GameModel::loadGameData() {
  GameSaveData gameSaveData;
  FILE* file = fopen("save/game.dat", "rb");
  if (file == NULL) {
    gameSaveData.loadSucceeded = false;
    return gameSaveData;
  }
  fread(&gameSaveData, sizeof (GameSaveData), 1, file);
  fclose(file);
  return gameSaveData;
}

void GameModel::saveLocation() {
  // see if we need to save the current Location
  if (mLocation->getType() == LOCATION_SHIP) {
    FILE* file = fopen("save/playership.dat", "wb");
    mLocation->save(file);
    fclose(file);
  }
  else if (mCurrentPlanet != NULL) {
    char fileName[128];
    sprintf(fileName, "save/planet%d.dat", mCurrentPlanet->mHandle);
    FILE* file = fopen(fileName, "wb");
    mLocation->save(file);
    fclose(file);
  }
}

void GameModel::initializePlanet(bool resetPlayer, v3d_t* startPos, bool createFeatures, GameWindow* gameWindow) {
  printf("game_c::initializePlanet()\n");
  if (mLocation != 0) {
    delete mLocation;
  }
  mLocation = new World();

  FILE* file = NULL;
  v3d_t playerStartPosition;

  // gotta do some kinda loading thingie
  LoadScreen* loadScreen = new LoadScreen(gameWindow);
  loadScreen->setCompletedColor(LOAD_SCREEN_LIGHT_PURPLE);
  loadScreen->setIncompletedColor(LOAD_SCREEN_DARK_PURPLE);
  loadScreen->show();

  // this is just a generic planet
  if (mCurrentPlanet == NULL) {
    mWorldSeed = SDL_GetTicks();
    printf("world seed: %d\n", mWorldSeed);
  }
  else {
    // try to open the file for this planet
    printf("game_c::initializePlanet(): loading planet\n");
    char fileName[48];
    sprintf(fileName, "save/planet%d.dat", mCurrentPlanet->mHandle);
    printf("game_c::initializePlanet(): trying to load file: %s\n", fileName);
    file = fopen(fileName, "rb");
    mWorldSeed = mCurrentPlanet->mSeed;
  }

  if (startPos != NULL) {
    v3d_print("game_c::initializePlanet(): setting startPos, ", *startPos);
    static_cast<World*>(mLocation)->setStartPosition(*startPos);
    playerStartPosition = *startPos;
  }

  printf("game_c::initializePlanet(): initializing location\n");
  mLocation->initialize(file, mGalaxy, mCurrentPlanet->mHandle);
  if (file != NULL) {
    fclose(file);
  }

  if (startPos == NULL) {
    playerStartPosition = mLocation->getStartPosition();
  }
  else if (createFeatures) {
    printf("game_c::initializePlanet(): creating features\n");
    v3di_t worldIndex = WorldUtil::getRegionIndex(*startPos);
    FeatureGenerator::createSetPieces(worldIndex.x, worldIndex.z, *static_cast<World*>(mLocation), loadScreen);
  }

  loadScreen->setCompletedColor(LOAD_SCREEN_LIGHT_GREEN);
  loadScreen->setIncompletedColor(LOAD_SCREEN_DARK_GREEN);
  // this needs to be done after the lighting info has been set
  static_cast<World*>(mLocation)->preloadColumns(NUM_PRELOADED_COLUMNS, playerStartPosition, loadScreen);

  loadScreen->hide();
  delete loadScreen;

  resetForNewLocation(playerStartPosition, resetPlayer);
}

void GameModel::initializeStarShip(bool resetPlayer) {
  if (mLocation != NULL) {
    delete mLocation;
  }
  mLocation = new StarShip();

  FILE* file = fopen("save/playership.dat", "rb");
  mLocation->initialize(file, mGalaxy, mCurrentPlanet->mHandle);
  if (file != NULL) {
    fclose(file);
  }

  resetForNewLocation(mLocation->getStartPosition(), resetPlayer);
}

void GameModel::resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer) {
  // let's handle the items first...
  // FIXME: this is temporary
  destroyItemsOwnedByPhysicsAndAi();

  delete mPhysics;
  mPhysics = new Physics(this);

  // make a physical entity to represent the player
  // FIXME: make sure this succeeds you knucklehead!
  //  _assert(mPhysics->createEntity(OBJTYPE_PLAYER, playerStartPosition, 0.0, false) != 0);
  mPhysics->createEntity(OBJTYPE_PLAYER, playerStartPosition, false);
  printf("player physics handle: %lu\n", mPhysics->getPlayerHandle());

  // load the pre-generated physics items
  mPhysics->loadInactiveList();

  // AI
  mAiManager->clear();
  size_t playerAiHandle = mAiManager->setPlayerPhysicsHandle();

  // reset the player
  if (resetPlayer) {
    mPlayer->reset(mPhysics->getPlayerHandle(), playerAiHandle);
  }
  mPlayer->setStartPosition(playerStartPosition);
  mPlayer->soft_reset(playerStartPosition);

  // FIXME: this is really a hack...
  if (mLocation->getType() == LOCATION_SHIP) {
    mPlayer->set_draw_distance(500.0);
    mAiManager->setMaxCritters(0);
  }
  else {
    mPlayer->set_draw_distance(r_num(150.0, 500.0));
    mAiManager->setMaxCritters(20);
  }

  // TODO: handle the ItemManager:
  // some items are persistent (i.e. what the player and his posse
  // are holding onto), the rest should probably be destroyed

}

void GameModel::destroyItemsOwnedByPhysicsAndAi() {
  vector<size_t> itemList = mAiManager->getAllItemHandles();
  mItemManager->destroyItemList(itemList);
  itemList = mPhysics->getAllItemHandles();
  mItemManager->destroyItemList(itemList);
  mItemManager->trimItemsList();
}

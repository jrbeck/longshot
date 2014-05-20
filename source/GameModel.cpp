#include "GameModel.h"


GameModel::GameModel() : 
  player(0),
  galaxy(0),
  physics(0),
  location(0),
  aiManager(0),
  itemManager(0),
  gameInput(0)
{
}


GameModel::~GameModel() {

}


void GameModel::save() {
  //	player_c player;	// save
  // PLAYER * * * * * * *
  saveGameData();


  //	Galaxy *galaxy;	// save
  // GALAXY * * * * * *
  FILE *file;
  file = fopen("save/galaxy.dat", "wb");
  if (file != NULL) {
    galaxy->save(file);
    fclose(file);
  }


  //	Location *location;	// save
  // LOCATION * * * * * * *
  saveLocation();


  //	AiManager aiManager;	// save
  // AI STUFF * * * * * * *
  // TODO

  //	physics_c physics;	// save
  // PHYSICS * * * * * * *
  // TODO

  //	ItemManager itemManager;	// save
  // ITEMS * * * * * * *
  file = fopen("save/items.dat", "wb");
  if (file != NULL) {
    itemManager->save(file);
    // WARNING: this pertains to the player...
    player->getInventory()->save(file);
    fclose(file);
  }

}


int GameModel::load() {

  //	player_c player;	// save
  // PLAYER * * * * * * *
  GameSaveData gameSaveData = loadGameData();
  if (!gameSaveData.loadSucceeded) {
    printf("game_c::load(): UNSUCCESSFUL 1\n");
    return LOAD_UNSUCCESSFUL;
  }

  //	Galaxy *galaxy;	// save
  // GALAXY * * * * * *
  FILE *file;
  file = fopen("save/galaxy.dat", "rb");
  if (file != NULL) {
    galaxy->load(file);
    fclose(file);
  }
  else {
    printf(" game_c::load(): UNSUCCESSFUL 2\n");
    return LOAD_UNSUCCESSFUL;
  }


  currentPlanet = galaxy->getPlanetByHandle(gameSaveData.planetHandle);



  //	Location *location;	// save
  // LOCATION * * * * * * *
  if (location != NULL) {
    delete location;
    location = NULL;
  }

  switch (gameSaveData.locationType) {
  case LOCATION_SHIP:
    printf("game_c::load(): loading ship\n");
    location = new StarShip();
    initSpaceShip(true);
    break;
  case LOCATION_WORLD:
    printf("game_c::load(): loading planet\n");
    location = new World();
    initializePlanet(true, galaxy->getPlanetByHandle(gameSaveData.planetHandle), &gameSaveData.physicsPos, false);
    break;
  default:
    printf("game_c::load(): error, default location\n");
    return LOAD_UNSUCCESSFUL;
  }



  //	AiManager aiManager;	// save
  // AI STUFF * * * * * * *

  //	physics_c physics;	// save
  // PHYSICS * * * * * * *
  physics->set_pos(physics->getPlayerHandle(), gameSaveData.physicsPos);

  //	ItemManager itemManager;	// save
  // ITEMS * * * * * * *
  file = fopen("save/items.dat", "rb");
  if (file != NULL) {
    itemManager->load(file);
    // WARNING: this pertains to the player...
    player->getInventory()->load(file);
    fclose(file);
  }

  return LOAD_SUCCESSFUL;
}


int GameModel::saveGameData(void) {
  GameSaveData gameSaveData;
  gameSaveData.loadSucceeded = true;

  gameSaveData.physicsPos = physics->getNearCorner(physics->getPlayerHandle());
  v3d_print("game_c::saveGameData(): saving playerPos, ", gameSaveData.physicsPos);

  gameSaveData.locationType = location->getType();
  gameSaveData.planetHandle = currentPlanet->mHandle;

  // now for the file stuff
  FILE* file = fopen("save/game.dat", "wb");
  if (file == NULL) {
    return -1;
  }
  fwrite(&gameSaveData, sizeof GameSaveData, 1, file);	// physics position
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
  fread(&gameSaveData, sizeof GameSaveData, 1, file);
  fclose(file);
  return gameSaveData;
}


void GameModel::saveLocation() {
  // see if we need to save the current Location
  if (location->getType() == LOCATION_SHIP) {
    FILE* file = fopen("save/playership.dat", "wb");
    location->save(file);
    fclose(file);
  }
  else if (currentPlanet != NULL) {
    char fileName[128];
    sprintf(fileName, "save/planet%d.dat", currentPlanet->mHandle);
    FILE* file = fopen(fileName, "wb");
    location->save(file);
    fclose(file);
  }
}




void GameModel::initializePlanet(bool resetPlayer, Planet* planet, v3d_t* startPos, bool createSetPieces) {
  printf("game_c::initializePlanet()\n");

  FILE *file = NULL;
  currentPlanet = planet;
  v3d_t playerStartPosition;

  // gotta do some kinda loading thingie
  LoadScreen* loadScreen = new LoadScreen(mGameWindow);
  loadScreen->setCompletedColor(LOAD_SCREEN_LIGHT_PURPLE);
  loadScreen->setIncompletedColor(LOAD_SCREEN_DARK_PURPLE);
  loadScreen->show();

  // this is just a generic planet
  if (planet == NULL) {
    worldSeed = SDL_GetTicks();
    printf("world seed: %d\n", worldSeed);
  }
  else {
    // try to open the file for this planet
    printf("game_c::initializePlanet(): loading planet\n");
    char fileName[48];
    sprintf(fileName, "save/planet%d.dat", planet->mHandle);
    printf("game_c::initializePlanet(): trying to load file: %s\n", fileName);
    file = fopen(fileName, "rb");
    worldSeed = planet->mSeed;
  }

  if (startPos != NULL) {
    v3d_print("game_c::initializePlanet(): setting startPos, ", *startPos);
    static_cast<World*>(location)->setStartPosition(*startPos);
    playerStartPosition.x = startPos->x;
    playerStartPosition.y = startPos->y;
    playerStartPosition.z = startPos->z;
  }

  printf("game_c::initializePlanet(): initializing location\n");
  location->initialize(file, galaxy, planet->mHandle);
  if (file != NULL) {
    fclose(file);
  }

  if (startPos == NULL) {
    playerStartPosition = location->getStartPosition();
  }
  else if (createSetPieces) {
    printf("game_c::initializePlanet(): creating features\n");
    v3di_t worldIndex = WorldUtil::getRegionIndex(*startPos);
    FeatureGenerator::createSetPieces(
      worldIndex.x,
      worldIndex.z,
      *static_cast<World*>(location),
      loadScreen);
  }

  loadScreen->setCompletedColor(LOAD_SCREEN_LIGHT_GREEN);
  loadScreen->setIncompletedColor(LOAD_SCREEN_DARK_GREEN);
  // this needs to be done after the lighting info has been set
  static_cast<World*>(location)->preloadColumns(NUM_PRELOADED_COLUMNS, playerStartPosition, loadScreen);

  loadScreen->hide();
  delete loadScreen;

  // load the appropriate menu
  loadPlanetMenu();

  mCycleLighting = true;

  resetForNewLocation(playerStartPosition, resetPlayer);
}

void GameModel::initSpaceShip(bool resetPlayer) {
  FILE *file = fopen("save/playership.dat", "rb");
  // this decides whose map we're gonna render/interact with

  location->initialize(file, galaxy, currentPlanet->mHandle);
  if (file != NULL) {
    fclose(file);
  }

  // make a physical entity to represent the player
  v3d_t playerStartPosition = { 12.5, 1.1, 50.0 };

  // load the appropriate menu
  loadShipMenu();

  mCycleLighting = false;

  resetForNewLocation(playerStartPosition, resetPlayer);
}

void GameModel::resetForNewLocation(v3d_t playerStartPosition, bool resetPlayer) {
  // we need to give it an ambient light color
  IntColor sunColor;
  if (location->getType() == LOCATION_SHIP) {
    sunColor.r = LIGHT_LEVEL_MAX;
    sunColor.g = LIGHT_LEVEL_MAX;
    sunColor.b = LIGHT_LEVEL_MAX;
  }
  else {
    GLfloat* starColor = galaxy->getStarSystemByHandle(currentPlanet->mHandle)->mStarColor;
    sunColor.r = (int)((starColor[0] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.g = (int)((starColor[1] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.b = (int)((starColor[2] + 0.5f) * (GLfloat)LIGHT_LEVEL_MAX);
    sunColor.constrain(LIGHT_LEVEL_MIN, LIGHT_LEVEL_MAX);
  }
  mWorldMapView.setWorldMap(location->getWorldMap(), sunColor);


  // let's handle the items first...
  // FIXME: this is temporary
  destroyItemsOwnedByPhysicsAndAi();

  // reset this to be safe
  mPhysics->reset();

  // create a new PhysicsView
  if (mPhysicsView != NULL) {
    delete mPhysicsView;
  }
  mPhysicsView = new PhysicsView();

  // make a physical entity to represent the player
  // FIXME: make sure this succeeds you knucklehead!
  //  _assert(mPhysics->createEntity(OBJTYPE_PLAYER, playerStartPosition, 0.0, false) != 0);
  mPhysics->createEntity(OBJTYPE_PLAYER, playerStartPosition, false);
  printf("player physics handle: %d\n", mPhysics->getPlayerHandle());

  // load the pre-generated physics items
  mPhysics->loadInactiveList();

  // AI
  aiManager->clear();
  if (mAiView != NULL) {
    delete mAiView;
  }
  mAiView = new AiView();
  mAiView->setAiEntities(aiManager->getEntities());
  playerAiHandle = aiManager->setPlayerPhysicsHandle();


  // reset the player
  if (resetPlayer) {
    player->reset(mGameModel->physics->getPlayerHandle(), playerAiHandle, *mItemManager);
  }
  player->setStartPosition(playerStartPosition);
  player->soft_reset(playerStartPosition);


  // FIXME: this is really a hack...
  if (location->getType() == LOCATION_SHIP) {
    player->set_draw_distance(500.0);
    aiManager->setMaxCritters(0);
  }
  else {
    player->set_draw_distance(r_num(150.0, 500.0));
    aiManager->setMaxCritters(10);
  }

  // TODO: handle the ItemManager:
  // some items are persistent (i.e. what the player and his posse
  // are holding onto), the rest should probably be destroyed

}


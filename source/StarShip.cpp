#include "StarShip.h"

StarShip::StarShip() :
	mOrbitSky(NULL)
{
	mType = LOCATION_SHIP;
	mWorldMap = NULL;
	mLightManager = NULL;
}

StarShip::~StarShip() {
	if (mWorldMap != NULL) {
		delete mWorldMap;
	}
	if (mLightManager != NULL) {
		delete mLightManager;
	}
	if (mOrbitSky != NULL) {
		delete mOrbitSky;
	}
}



int StarShip::initialize(FILE *file, Galaxy *galaxy, int planetHandle) {
	printf("StarShip::initialize(): entry\n");
	if (mWorldMap != NULL) {
		delete mWorldMap;
	}
	mWorldMap = new WorldMap();
	mWorldMap->resize (WORLD_MAP_SIDE, WORLD_MAP_SIDE);

	if (mLightManager != NULL) {
		delete mLightManager;
	}
	mLightManager = new LightManager();


	// FIXME: setup the 'sun' and 'moon' positions
	if (mOrbitSky != NULL) {
		delete mOrbitSky;
	}
	mOrbitSky = new OrbitSky();
//	assetManager.setDirectionalLightPositions(v3d_v(0, 100, 0), v3d_v(0, -100, 0));
//	mSkySim->initialize(mPlayerStartPos);
	mOrbitSky->setOrbit(*galaxy, planetHandle);


	// make sure the WorldMap is looking at the same area that we
	// are, and that the columns are clear
	if (file == NULL) {
		initWorldColumns(true);
		generateShip();
	}
	else {
		load(file);
		initWorldColumns(false);
	}

	return 0;
}


void StarShip::save(FILE *file) {
	mWorldMap->save(file);
	mLightManager->save(file);
}


void StarShip::load(FILE *file) {
	mWorldMap->load(file);
	mLightManager->load(file);
}


int StarShip::update(v3d_t playerPosition) {
	mLightManager->update(*mWorldMap);
	return 0;
}


void StarShip::draw (gl_camera_c &cam) {
	mOrbitSky->draw(cam, v3d_v(0.0, 0.0, 0.0));
}


void StarShip::initWorldColumns(bool clearColumns) {
	int halfWorld = WORLD_MAP_SIDE / 2;

	for (int z = 0; z < WORLD_MAP_SIDE; z++) {
		for (int x = 0; x < WORLD_MAP_SIDE; x++) {
			int xIndex = x - halfWorld;
			int zIndex = z - halfWorld;

			int columnIndex = mWorldMap->getColumnIndexByRegionCoords (xIndex, zIndex);

			if (clearColumns) {
				mWorldMap->clearColumn (columnIndex);

				mWorldMap->mColumns[columnIndex].mWorldIndex.x = xIndex;
				mWorldMap->mColumns[columnIndex].mWorldIndex.z = zIndex;

				mWorldMap->mColumns[columnIndex].mWorldPosition.x = xIndex * WORLD_CHUNK_SIDE;
				mWorldMap->mColumns[columnIndex].mWorldPosition.z = zIndex * WORLD_CHUNK_SIDE;

				mWorldMap->mColumns[columnIndex].mNeedShadowVolume = true;
				mWorldMap->mColumns[columnIndex].mNeedLightingApplied = false;
				mWorldMap->mColumns[columnIndex].mNeedVisibility = false;
				mWorldMap->mColumns[columnIndex].mNeedDisplayList = false;
			}
			else {
				// try to load from the inactive columns
				if (mWorldMap->mInactiveColumnManager.loadFromInactiveColumns(xIndex, zIndex, mWorldMap->mColumns[columnIndex]) == 0) {
//					printf ("loaded column from InactiveColumnManager\n");
				}
				else {
//					printf ("did not load column from InactiveColumnManager\n");
					mWorldMap->clearColumn (columnIndex);

					mWorldMap->mColumns[columnIndex].mWorldIndex.x = xIndex;
					mWorldMap->mColumns[columnIndex].mWorldIndex.z = zIndex;

					mWorldMap->mColumns[columnIndex].mWorldPosition.x = xIndex * WORLD_CHUNK_SIDE;
					mWorldMap->mColumns[columnIndex].mWorldPosition.z = zIndex * WORLD_CHUNK_SIDE;

					mWorldMap->mColumns[columnIndex].mNeedShadowVolume = true;
					mWorldMap->mColumns[columnIndex].mNeedLightingApplied = false;
					mWorldMap->mColumns[columnIndex].mNeedVisibility = false;
					mWorldMap->mColumns[columnIndex].mNeedDisplayList = false;
				}
			}

			mWorldMap->removeFromChangedList(columnIndex);
		}
	}
}



void StarShip::generateShip() {
	printf("generating starship\n");

	// fill it
	v3di_t nc = v3di_v(0, 0, 0);
	v3di_t fc = v3di_v(10, 5, 25);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_METAL_TILE_GREEN);

	nc = v3di_v(15, 0, 0);
	fc = v3di_v(25, 5, 25);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_METAL_TILE_GREEN);

	nc = v3di_v(10, 0, 15); // corridor
	fc = v3di_v(15, 4, 45);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_METAL_TILE_GREEN);

	nc = v3di_v(7, 0, 45); // bridge
	fc = v3di_v(18, 5, 55);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_METAL_TILE_GREEN);

	// carve it out
	nc = v3di_v(1, 1, 1);
	fc = v3di_v(9, 4, 24);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	nc = v3di_v(16, 1, 1);
	fc = v3di_v(24, 4, 24);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	nc = v3di_v(1, 1, 16);
	fc = v3di_v(24, 3, 24);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	nc = v3di_v(11, 1, 16); // corridor
	fc = v3di_v(14, 3, 46);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	nc = v3di_v(8, 1, 46); // bridge
	fc = v3di_v(17, 4, 54);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	nc = v3di_v(7, 2, 47); // windshield
	fc = v3di_v(18, 3, 55);
	mWorldMap->fillVolume(nc, fc, BLOCK_TYPE_AIR);

	mWorldMap->setBlockType(v3di_v(11, 1, 52), BLOCK_TYPE_CONSOLE);
	mWorldMap->setBlockType(v3di_v(12, 1, 52), BLOCK_TYPE_CONSOLE);
	mWorldMap->setBlockType(v3di_v(13, 1, 52), BLOCK_TYPE_CONSOLE);
	mWorldMap->setBlockType(v3di_v(14, 1, 52), BLOCK_TYPE_CONSOLE);

	// clear out some viewports
	for (int z = 26; z < 45; z+= 2) {
		mWorldMap->setBlockType(v3di_v(10, 2, z), BLOCK_TYPE_GLASS);
		mWorldMap->setBlockType(v3di_v(15, 2, z), BLOCK_TYPE_GLASS);
	}
	

	// how about some lights...
	IntColor color = { LIGHT_LEVEL_MAX * 0.85, LIGHT_LEVEL_MAX * 0.85, LIGHT_LEVEL_MAX * 0.85 };
	mLightManager->addLight(v3d_v(12.5, 4.0, 50.0), 16.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v(12.5, 4.0, 40.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v(12.5, 4.0, 30.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v(12.5, 4.0, 20.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v( 5.0, 4.0, 20.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v(20.0, 4.0, 20.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v( 5.0, 4.0,  7.0), 10.0, color, *mWorldMap);
	mLightManager->addLight(v3d_v(20.0, 4.0,  7.0), 10.0, color, *mWorldMap);


}


v3d_t StarShip::getStartPosition(void) {
	return v3d_v(12.5, 1.1, 50.0);
}



#include "LightManager.h"


LightManager::LightManager() :
	mNextHandle(1)
{
}


LightManager::~LightManager() {
	clear();
}


size_t LightManager::addLight( const v3d_t& position, double radius, const IntColor& color, WorldMap& worldMap ) {
	WorldLight *light = new WorldLight();
	if (light == NULL) {
		printf("LightManager::addLight(): error: out of memory\n");
		return 0;
	}
	light->mHandle = mNextHandle++;
	light->set( position, radius, color );
	mLights.push_back( light );

	updateWorldColumns( *light, worldMap );

	return light->mHandle;
}


void LightManager::updateWorldColumns( const WorldLight& light, WorldMap& worldMap) const {
	v3di_t pos;
	int columnIndex;
	int endCheckColumnIndex;

	// FIXME: this is super-ugly...can this be made a little
	// less egregious?

	for (pos.z = light.mBufferNear.z; pos.z <= light.mBufferFar.z; pos.z += WORLD_CHUNK_SIDE) {
		for (pos.x = light.mBufferNear.x; pos.x <= light.mBufferFar.x; pos.x += WORLD_CHUNK_SIDE) {
			// pickColumn() only returns >= 0 if column is loaded
			columnIndex = worldMap.pickColumn(pos);
			if (columnIndex >= 0) {
				worldMap.mColumns[columnIndex].mNeedShadowVolume = true;
			}
		}

		// check the last x-coord explictly
		pos.x = light.mBufferFar.x;
		endCheckColumnIndex = worldMap.getColumnIndex(pos);
		if (columnIndex != endCheckColumnIndex) {
			// this time, we still have to make sure it's loaded
			columnIndex = worldMap.pickColumn(pos);
			if (columnIndex >= 0) {
				worldMap.mColumns[columnIndex].mNeedShadowVolume = true;
			}
		}
	}

	// check the last z-coord explictly
	pos.z = light.mBufferFar.z;
	endCheckColumnIndex = worldMap.getColumnIndex(pos);
	if (columnIndex != endCheckColumnIndex) {
		// must repeat this loop to be sure...
		for (pos.x = light.mBufferNear.x; pos.x <= light.mBufferFar.x; pos.x += WORLD_CHUNK_SIDE) {
			// pickColumn() only returns >= 0 if column is loaded
			columnIndex = worldMap.pickColumn(pos);
			if (columnIndex >= 0) {
				worldMap.mColumns[columnIndex].mNeedShadowVolume = true;
			}
		}

		// check the last x-coord explictly
		pos.x = light.mBufferFar.x;
		endCheckColumnIndex = worldMap.getColumnIndex(pos);
		if (columnIndex != endCheckColumnIndex) {
			// this time, we still have to make sure it's loaded
			columnIndex = worldMap.pickColumn(pos);
			if (columnIndex >= 0) {
				worldMap.mColumns[columnIndex].mNeedShadowVolume = true;
			}
		}
	}
}



void LightManager::removeLight( size_t handle ) {
	// first check the active lights
	size_t numLights = mLights.size();
	for (size_t i = 0; i < numLights; i++) {
		if (mLights[i]->mHandle == handle) {
			delete mLights[i];
			if (i != (numLights - 1)) {
				swap(mLights[i], mLights[numLights - 1]);
			}
			mLights.pop_back();
			return;
		}
	}

	// now for the inactive lights list
	numLights = mInactiveLights.size();
	for (size_t i = 0; i < numLights; i++) {
		if (mLights[i]->mHandle == handle) {
			delete mInactiveLights[i];
			if (i != (numLights - 1)) {
				swap(mInactiveLights[i], mInactiveLights[numLights - 1]);
			}
			mInactiveLights.pop_back();
			return;
		}
	}
}


IntColor LightManager::getLightLevel( const v3di_t& position) const {
	IntColor level = { 0, 0, 0 };
	IntColor temp;
	size_t numLights = mLights.size();
	for (size_t i = 0; i < numLights; i++) {
		temp = mLights[i]->getLevel( position );
		level.r += temp.r;
		level.g += temp.g;
		level.b += temp.b;
	}
	return level;
}


void LightManager::update( WorldMap& worldMap ) {
	int count = 0;

	// deactivate lights that are in columns that aren't loaded
	size_t numActiveLights = mLights.size();
	for (size_t i = 0; i < numActiveLights; i++) {
		if (worldMap.pickColumn(v3di_v(mLights[i]->mWorldPosition)) < 0) {
			mLights[i]->turnOff();
			mInactiveLights.push_back(mLights[i]);

			// tell the worldMap to update accordingly
			updateWorldColumns(*mLights[i], worldMap);

			if (i != (numActiveLights - 1)) {
				swap(mLights[i], mLights[numActiveLights - 1]);
			}
			mLights.pop_back();
			numActiveLights--;
			i--;
		}
	}
	// activate lights in the mInactiveLights that are now in range
	size_t numInactiveLights = mInactiveLights.size();
	for (size_t i = 0; i < numInactiveLights; i++) {
		if (worldMap.pickColumn(v3di_v(mInactiveLights[i]->mWorldPosition)) >= 0) {
			count++;
			mInactiveLights[i]->turnOn();
			mLights.push_back(mInactiveLights[i]);

			// tell the worldMap to update accordingly
			updateWorldColumns(*mInactiveLights[i], worldMap);

			if (i != (numInactiveLights - 1)) {
				swap(mInactiveLights[i], mInactiveLights[numInactiveLights - 1]);
			}
			mInactiveLights.pop_back();
			numInactiveLights--;
			i--;
		}
	}
//	if (count != 0) {
//		printf("turned on %d lights\n", count);
//	}
}



void LightManager::clear() {
	// clear the active lights
	if (mLights.size() > 0) {
		for (size_t i = 0; i < mLights.size(); i++) {
			if (mLights[i] != NULL) {
				delete mLights[i];
			}
		}
		mLights.clear();
	}

	// now for the inactive lights
	if (mInactiveLights.size() > 0) {
		for (size_t i = 0; i < mInactiveLights.size(); i++) {
			if (mInactiveLights[i] != NULL) {
				delete mInactiveLights[i];
			}
		}
		mInactiveLights.clear();
	}

	// reset this
	mNextHandle = 1;
}


void LightManager::save( FILE* file ) {
	// we don't really care if they're active or not, just
	// need to write all the WorldLight info to disk
	size_t numLights = mLights.size();
	size_t numInactiveLights = mInactiveLights.size();
	size_t totalLights = numLights + numInactiveLights;
	fwrite(&totalLights, sizeof(size_t), 1, file);

	// active lights
	for (size_t i = 0; i < numLights; i++) {
		mLights[i]->save(file);
	}
	// inactive lights
	for (size_t i = 0; i < numInactiveLights; i++) {
		mInactiveLights[i]->save(file);
	}
}


void LightManager::load( FILE* file ) {
	clear();

	// active lights
	size_t numLights = mLights.size();
	fread(&numLights, sizeof(size_t), 1, file);
//	printf("LightManager::load(): loading %d lights\n", numLights);
	for (size_t i = 0; i < numLights; i++) {
		WorldLight *light = new WorldLight;
		light->load(file);
		light->mHandle = mNextHandle++;
		mInactiveLights.push_back(light);
	}
	printf("LightManager::load(): loaded %d lights\n", mInactiveLights.size());
}





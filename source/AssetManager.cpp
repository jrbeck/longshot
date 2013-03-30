#include <AssetManager.h>




// METHODS * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AssetManager::AssetManager() :
	mGunTexture(0),
	mGunTextureRegion(0),
	mGunBitmapModel(0)
{
	// this needs to start at zero/false
	mNumTexturesLoaded = 0;

	mTerrainTextureHandle = 0;

	mBlankBlockCallListHandle = 0;
	mShadedBlockCallListHandle = 0;

	// set up the sun and moon lights
	LightSource sun;
	sun.setColor (v3d_v (1.0, 0.95, 0.8));
	sun.setDirection (v3d_v (0.0, -1.0, 0.0));

	LightSource moon;
	moon.setColor (v3d_v (0.6, 0.2, 0.2));
	moon.setDirection (v3d_v (0.0, 1.0, 0.0));

	mDirectionalLights.push_back (sun);
	mDirectionalLights.push_back (moon);

	// this is default...
	setDirectionalLightPositions (v3d_v(0.0, 100.0, 0.0), v3d_v(0.0, -100.0, 0.0));
}



AssetManager::~AssetManager (void) {
	freeAssets ();
}



int AssetManager::loadAssets (void) {
/*	if (load_texture ("art/32_block_dirt.bmp", &mBlockTextureHandles[BLOCK_TYPE_DIRT]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_dirt_grass.bmp", &mBlockTextureHandles[BLOCK_TYPE_DIRT_GRASS]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_dirt_snow.bmp", &mBlockTextureHandles[BLOCK_TYPE_DIRT_SNOW]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_grass.bmp", &mBlockTextureHandles[BLOCK_TYPE_GRASS]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_snow.bmp", &mBlockTextureHandles[BLOCK_TYPE_SNOW]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_sand.bmp", &mBlockTextureHandles[BLOCK_TYPE_SAND]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_water.bmp", &mBlockTextureHandles[BLOCK_TYPE_WATER]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_stone.bmp", &mBlockTextureHandles[BLOCK_TYPE_STONE]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_stone_grass.bmp", &mBlockTextureHandles[BLOCK_TYPE_STONE_GRASS]) == 0) {
		mNumTexturesLoaded++; }
	if (load_texture ("art/32_block_stone_snow.bmp", &mBlockTextureHandles[BLOCK_TYPE_STONE_SNOW]) == 0) {
		mNumTexturesLoaded++; }*/
	if (loadTexture("art/32_terrain.png", &mTerrainTextureHandle) == 0) {
		mNumTexturesLoaded++;
	}

	// TESTING: BitmapModel
	mGunTexture = new Texture("art/item_sheet.png");
	mGunTextureRegion = new TextureRegion(0, 0, 45, 21, mGunTexture);
//	mGunTextureRegion = new TextureRegion(0, 32, 65, 21, mGunTexture);
	mGunBitmapModel = new BitmapModel(mGunTextureRegion);
	// END TESTING

	// make a call list for the block
	mBlankBlockCallListHandle = glGenLists(1);
	glNewList (mBlankBlockCallListHandle, GL_COMPILE);
		drawBlankBlock();
	glEndList();

//	mBlankBlockCallListHandle = loadObjectFile("art\\models\\3dplus.xml");

	// generate the display list for the shaded block
	mShadedBlockCallListHandle = glGenLists(1);
	glNewList(mShadedBlockCallListHandle, GL_COMPILE);
		drawBlock(mGlobalLightIntensities);
	glEndList();


	// didn't bring down the house hopefully
	return 0;
}


void AssetManager::freeAssets (void) {
	if (mTerrainTextureHandle != 0) {
		glDeleteTextures(1, &mTerrainTextureHandle);
		mTerrainTextureHandle = 0;
	}
	
	if (mBlankBlockCallListHandle != 0) {
		glDeleteLists (mBlankBlockCallListHandle, 1);
		mBlankBlockCallListHandle = 0;
	}

	if (mShadedBlockCallListHandle != 0) {
		glDeleteLists (mShadedBlockCallListHandle, 1); 
		mShadedBlockCallListHandle = 0;
	}

	// TESTING
	if(mGunBitmapModel != 0) {
		delete mGunBitmapModel;
		mGunBitmapModel = 0;
	}
	if(mGunTextureRegion != 0) {
		delete mGunTextureRegion;
		mGunTextureRegion = 0;
	}
	if(mGunTexture != 0) {
		delete mGunTexture;
		mGunTexture = 0;
	}
	// END TESTING

}



GLuint AssetManager::loadImg(const char *fileName) {
	GLuint textureHandle;

	if (loadTexture(fileName, &textureHandle) == 0) {
		return textureHandle;
	}

	return 0;
}


void AssetManager::setDirectionalLightPositions(v3d_t sunPosition, v3d_t moonPosition) {
	mDirectionalLights[0].setDirection (sunPosition);
	mDirectionalLights[1].setDirection (moonPosition);

	printf("AssetManager::setDirectionalLightPositions(): here\n");

	for (size_t i = 0; i < 6; i++) {
		mGlobalLightIntensities[i] = v3d_zero ();
	}

	for (size_t light = 0; light < mDirectionalLights.size (); light++) {
		bool isTheSun = false;

		if (light == 0) {
			isTheSun = true;
		}

		mGlobalLightIntensities[BLOCK_SIDE_LEF] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_LEF],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (-1.0, 0.0, 0.0), isTheSun));

		mGlobalLightIntensities[BLOCK_SIDE_RIG] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_RIG],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (1.0, 0.0, 0.0), isTheSun));

		mGlobalLightIntensities[BLOCK_SIDE_TOP] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_TOP],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (0.0, 1.0, 0.0), isTheSun));

		mGlobalLightIntensities[BLOCK_SIDE_BOT] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_BOT],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (0.0, -1.0, 0.0), isTheSun));

		mGlobalLightIntensities[BLOCK_SIDE_BAC] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_BAC],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (0.0, 0.0, -1.0), isTheSun));

		mGlobalLightIntensities[BLOCK_SIDE_FRO] =
			v3d_add (mGlobalLightIntensities[BLOCK_SIDE_FRO],
			mDirectionalLights[light].
			computeDirectionalLightIntensity (v3d_v (0.0, 0.0, 1.0), isTheSun));

	}
//	mGlobalLightIntensities[BLOCK_SIDE_LEF] = v3d_v (1, 1, 1);
//	mGlobalLightIntensities[BLOCK_SIDE_RIG] = v3d_zero ();
//	mGlobalLightIntensities[BLOCK_SIDE_BOT] = v3d_zero ();
//	mGlobalLightIntensities[BLOCK_SIDE_TOP] = v3d_zero ();
//	mGlobalLightIntensities[BLOCK_SIDE_BAC] = v3d_zero ();
//	mGlobalLightIntensities[BLOCK_SIDE_FRO] = v3d_zero ();

	// create the display list
	// WARNING: assumes the display list handle was obtained (loadAssets) previously
	
	glNewList (mShadedBlockCallListHandle, GL_COMPILE);
		drawBlock (mGlobalLightIntensities);
	glEndList();
}



GLuint AssetManager::getBlankBlockCallListHandle (void) const {
	return mBlankBlockCallListHandle;
}



GLuint AssetManager::getShadedBlockCallListHandle (void) const {
	return mShadedBlockCallListHandle;
}


// draw a block
void AssetManager::drawBlock (const v3d_t *intensities) const {
	glBegin (GL_QUADS);
		// left
		glNormal3d (-1.0, 0.0, 0.0);

		glColor4d (intensities[BLOCK_SIDE_LEF].x,
			intensities[BLOCK_SIDE_LEF].y,
			intensities[BLOCK_SIDE_LEF].z, 1.0);

		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
		glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
		glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
		glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB

		// right
		glNormal3d (1.0, 0.0, 0.0);

		glColor4d (intensities[BLOCK_SIDE_RIG].x,
			intensities[BLOCK_SIDE_RIG].y,
			intensities[BLOCK_SIDE_RIG].z, 1.0);

		glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
		glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
		glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF

		// top
		glNormal3d (0.0, 1.0, 0.0);

		glColor4d (intensities[BLOCK_SIDE_TOP].x,
			intensities[BLOCK_SIDE_TOP].y,
			intensities[BLOCK_SIDE_TOP].z, 1.0);
		
		glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
		glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
		glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB

		// bottom
		glNormal3d (0.0, -1.0, 0.0);

		glColor4d (intensities[BLOCK_SIDE_BOT].x,
			intensities[BLOCK_SIDE_BOT].y,
			intensities[BLOCK_SIDE_BOT].z, 1.0);

		glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
		glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
		glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF

		// front
		glNormal3d (0.0, 0.0, 1.0);

		glColor4d (intensities[BLOCK_SIDE_FRO].x,
			intensities[BLOCK_SIDE_FRO].y,
			intensities[BLOCK_SIDE_FRO].z, 1.0);

		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
		glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
		glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
		glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF

		// back
		glNormal3d (0.0, 0.0, -1.0);

		glColor4d (intensities[BLOCK_SIDE_BAC].x,
			intensities[BLOCK_SIDE_BAC].y,
			intensities[BLOCK_SIDE_BAC].z, 1.0);

		glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
		glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
		glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
		glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
	glEnd ();
}



// expects v3d_t intensities[8] - one for each corner
void AssetManager::drawBlock2 (const v3di_t intensities[8]) const {
	v3d_t v3dIntensities[NUM_BOX_CORNERS];

	for (int i = 0; i < NUM_BOX_CORNERS; i++) {
		v3dIntensities[i].x = (double)intensities[i].x * ONE_OVER_LIGHT_LEVEL_MAX;
		v3dIntensities[i].y = (double)intensities[i].y * ONE_OVER_LIGHT_LEVEL_MAX;
		v3dIntensities[i].z = (double)intensities[i].z * ONE_OVER_LIGHT_LEVEL_MAX;
	}


	// left
	glNormal3d (-1.0, 0.0, 0.0);

	glColor4d (v3dIntensities[BOX_CORNER_LBB].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities[BOX_CORNER_LBB].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities[BOX_CORNER_LBB].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB

	glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF

	glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF

	glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB

	// right
	glNormal3d (1.0, 0.0, 0.0);

	glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB

	glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB

	glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF

	// top
	glNormal3d (0.0, 1.0, 0.0);

	glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB

	glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF

	glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB

	// bottom
	glNormal3d (0.0, -1.0, 0.0);

	glColor4d (v3dIntensities[BOX_CORNER_LBB].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities[BOX_CORNER_LBB].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities[BOX_CORNER_LBB].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB

	glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB

	glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF

	glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF

	// front
	glNormal3d (0.0, 0.0, 1.0);


	glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF

	glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF

	glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF

	// back
	glNormal3d (0.0, 0.0, -1.0);

	glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB
	glColor4d (v3dIntensities[BOX_CORNER_LBB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities[BOX_CORNER_LBB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities[BOX_CORNER_LBB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB
	glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB
	glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB	
}




void AssetManager::drawBlock3 (int r, int g, int b) const {
	GLfloat color[4] = {
		(GLfloat)r * ONE_OVER_LIGHT_LEVEL_MAX,
		(GLfloat)g * ONE_OVER_LIGHT_LEVEL_MAX,
		(GLfloat)b * ONE_OVER_LIGHT_LEVEL_MAX,
		1.0f
	};

	// set the color once
	glColor4fv(color);

		// left
	glNormal3d (-1.0, 0.0, 0.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB

	// right
	glNormal3d (1.0, 0.0, 0.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF

	// top
	glNormal3d (0.0, 1.0, 0.0);
	glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB

	// bottom
	glNormal3d (0.0, -1.0, 0.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF

	// front
	glNormal3d (0.0, 0.0, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);	// LTF

	// back
	glNormal3d (0.0, 0.0, -1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);	// RTB	
}






void AssetManager::drawBlockWithFace (v3di_t intensities) const {
	v3d_t v3dIntensities;

	v3dIntensities.x = (double)intensities.x * ONE_OVER_LIGHT_LEVEL_MAX;
	v3dIntensities.y = (double)intensities.y * ONE_OVER_LIGHT_LEVEL_MAX;
	v3dIntensities.z = (double)intensities.z * ONE_OVER_LIGHT_LEVEL_MAX;

	// left - BACK
	glNormal3d (-1.0, 0.0, 0.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB

	// right - FACE
	glNormal3d (1.0, 0.0, 0.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
	glTexCoord2f (0.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF

	// top
	glNormal3d (0.0, 1.0, 0.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB

	// bottom
	glNormal3d (0.0, -1.0, 0.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF

	// front - right
	glNormal3d (0.0, 0.0, 1.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF

	// back - left
	glNormal3d (0.0, 0.0, -1.0);

	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
	glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
		v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
		v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB	
}



void AssetManager::drawBlankBlock (void) {
	// left
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB

	// right
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF

	// top
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);
	glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB

	// bottom
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF

	// front
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF

	// back
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
}



void AssetManager::drawBlockFace (int whichFace, const GLfloat intensities[4]) const {
//	glBegin (GL_QUADS);

	switch (whichFace) {
		case BLOCK_SIDE_LEF:
			drawBlockFaceLeft (intensities);
			break;
		case BLOCK_SIDE_RIG:
			drawBlockFaceRight (intensities);
			break;
		case BLOCK_SIDE_TOP:
			drawBlockFaceTop (intensities);
			break;
		case BLOCK_SIDE_BOT:
			drawBlockFaceBottom (intensities);
			break;
		case BLOCK_SIDE_FRO:
			drawBlockFaceFront (intensities);
			break;
		case BLOCK_SIDE_BAC:
			drawBlockFaceBack (intensities);
			break;
	}

//	glEnd ();
}



void AssetManager::drawBlockFaceLeft (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);
	glColor4fv (intensities);

	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
}



void AssetManager::drawBlockFaceRight (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);
	glColor4fv (intensities);

	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
}



void AssetManager::drawBlockFaceTop (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);
	glColor4fv (intensities);

	glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
}



void AssetManager::drawBlockFaceBottom (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);
	glColor4fv (intensities);

	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
}



void AssetManager::drawBlockFaceFront (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);
	glColor4fv (intensities);

	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);	// LBF
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);	// RBF
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);	// RTF
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);	// LTF
}



void AssetManager::drawBlockFaceBack (const GLfloat intensities[4]) const {
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);
	glColor4fv (intensities);

	glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);	// RBB
	glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);	// LBB
	glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);	// LTB
	glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);	// RTB
}


GLuint AssetManager::getTerrainTextureHandle (void) const {
	return mTerrainTextureHandle;
}


void AssetManager::drawBlock(GLfloat height, v3di_t pos, const block_t &block) const {
	GLfloat blockAlpha = gBlockData.get(block.type)->alpha;

	if (blockAlpha <= 0.0) return;
	if (block.faceVisibility == 0) return;

	static GLfloat intensities[4];
	intensities[3] = blockAlpha;

	static GLfloat faceLighting[6][3];

	for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
		faceLighting[i][0] = (GLfloat)block.faceLighting[i][0] * ONE_OVER_LIGHT_LEVEL_MAX;
		faceLighting[i][1] = (GLfloat)block.faceLighting[i][1] * ONE_OVER_LIGHT_LEVEL_MAX;
		faceLighting[i][2] = (GLfloat)block.faceLighting[i][2] * ONE_OVER_LIGHT_LEVEL_MAX;
	}


	static GLfloat npos[3];
	npos[0] = (GLfloat)pos.x;
	npos[1] = (GLfloat)pos.y;
	npos[2] = (GLfloat)pos.z;

	static GLfloat fpos[3];
	fpos[0] = npos[0] + 1.0f;
	fpos[1] = npos[1] + height;
	fpos[2] = npos[2] + 1.0f;



	// FIXME: this is hard coded for 32x32x4 textures (64x64 pixels per side) in
	// a 1024x1024 (i.e. 16 textures per side)
	// TODO: try a half-pixel hack to fix the bleeding issues
//	#define HALF_PIXEL		(1.0f / 2048.0f);
	#define TEX_MULT		(0.0625f)
	#define TEX_MULT_HALF	(0.03125f)

	GLfloat textureX = (static_cast<GLfloat>(block.type % 16) * TEX_MULT);
	GLfloat textureY = (static_cast<GLfloat>(block.type / 16) * TEX_MULT);

	// left
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_LEF],
//			mGlobalLightIntensities[BLOCK_SIDE_LEF]);
		intensities[0] = faceLighting[BLOCK_SIDE_LEF][0];
		intensities[1] = faceLighting[BLOCK_SIDE_LEF][1];
		intensities[2] = faceLighting[BLOCK_SIDE_LEF][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3fv(npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f(npos[0], npos[1], fpos[2]);	// LBF

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f(npos[0], fpos[1], fpos[2]);	// LTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f(npos[0], fpos[1], npos[2]);	// LTB
	}

	// right
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_RIG],
//			mGlobalLightIntensities[BLOCK_SIDE_RIG]);
		intensities[0] = faceLighting[BLOCK_SIDE_RIG][0];
		intensities[1] = faceLighting[BLOCK_SIDE_RIG][1];
		intensities[2] = faceLighting[BLOCK_SIDE_RIG][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f(fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f(fpos[0], fpos[1], npos[2]);	// RTB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3fv(fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f(fpos[0], npos[1], fpos[2]);	// RBF
	}

	// top
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_TOP],
//			mGlobalLightIntensities[BLOCK_SIDE_TOP]);
		intensities[0] = faceLighting[BLOCK_SIDE_TOP][0];
		intensities[1] = faceLighting[BLOCK_SIDE_TOP][1];
		intensities[2] = faceLighting[BLOCK_SIDE_TOP][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);

		glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
		glVertex3f(npos[0], fpos[1], npos[2]);	// LTB

		glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
		glVertex3f(npos[0], fpos[1], fpos[2]);	// LTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3fv(fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f(fpos[0], fpos[1], npos[2]);	// RTB

		// draw the underside of the water surface
		if (gBlockData.get(block.type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
			glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

			glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
			glVertex3f(npos[0], fpos[1], npos[2]);	// LTB

			glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
			glVertex3f(fpos[0], fpos[1], npos[2]);	// RTB

			glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
			glVertex3fv(fpos);	// RTF

			glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
			glVertex3f(npos[0], fpos[1], fpos[2]);	// LTF
		}
	}

	// bottom
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_BOT],
//			mGlobalLightIntensities[BLOCK_SIDE_BOT]);
		intensities[0] = faceLighting[BLOCK_SIDE_BOT][0];
		intensities[1] = faceLighting[BLOCK_SIDE_BOT][1];
		intensities[2] = faceLighting[BLOCK_SIDE_BOT][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3fv(npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f(fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT);
		glVertex3f(fpos[0], npos[1], fpos[2]);	// RBF

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT);
		glVertex3f(npos[0], npos[1], fpos[2]);	// LBF
	}

	// front
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_FRO],
//			mGlobalLightIntensities[BLOCK_SIDE_FRO]);

		intensities[0] = faceLighting[BLOCK_SIDE_FRO][0];
		intensities[1] = faceLighting[BLOCK_SIDE_FRO][1];
		intensities[2] = faceLighting[BLOCK_SIDE_FRO][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f(npos[0], npos[1], fpos[2]);	// LBF

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f(fpos[0], npos[1], fpos[2]);	// RBF

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3fv(fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f(npos[0], fpos[1], fpos[2]);	// LTF
	}

	// back
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_BAC],
//			mGlobalLightIntensities[BLOCK_SIDE_BAC]);

		intensities[0] = faceLighting[BLOCK_SIDE_BAC][0];
		intensities[1] = faceLighting[BLOCK_SIDE_BAC][1];
		intensities[2] = faceLighting[BLOCK_SIDE_BAC][2];

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f(fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3fv(npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f(npos[0], fpos[1], npos[2]);	// LTB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f(fpos[0], fpos[1], npos[2]);	// RTB
	}
}


/*

void AssetManager::drawBlock (GLfloat height, v3di_t pos, const block_t &block) const {
	GLfloat blockAlpha = gBlockData.get (block.type).alpha;

	if (blockAlpha <= 0.0) return;
	if (block.faceVisibility == 0) return;

	static GLfloat intensities[4];
	intensities[3] = blockAlpha;

	static GLfloat faceLighting[6][3];

	for (int i = 0; i < NUM_BLOCK_SIDES; i++) {
		faceLighting[i][0] = (GLfloat)block.faceLighting[i][0] * ONE_OVER_LIGHT_LEVEL_MAX;
		faceLighting[i][1] = (GLfloat)block.faceLighting[i][1] * ONE_OVER_LIGHT_LEVEL_MAX;
		faceLighting[i][2] = (GLfloat)block.faceLighting[i][2] * ONE_OVER_LIGHT_LEVEL_MAX;
	}

	static GLfloat npos[3];
	npos[0] = (GLfloat)pos.x;
	npos[1] = (GLfloat)pos.y;
	npos[2] = (GLfloat)pos.z;

	static GLfloat fpos[3];
	fpos[0] = npos[0] + 1.0f;
	fpos[1] = npos[1] + 1.0f;
	fpos[2] = npos[2] + 1.0f;

	// FIXME: this is hard coded for 32x32x4 textures (64x64 pixels per side) in
	// a 1024x1024 (i.e. 16 textures per side)
	#define TEX_MULT		(0.0625f)
	#define TEX_MULT_HALF	(0.03125f)

	GLfloat textureX = static_cast<GLfloat>(block.type % 16) * TEX_MULT;
	GLfloat textureY = static_cast<GLfloat>(block.type / 16) * TEX_MULT;

	// left
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_LEF],
//			mGlobalLightIntensities[BLOCK_SIDE_LEF]);
		intensities[0] = faceLighting[BLOCK_SIDE_LEF][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].x);
		intensities[1] = faceLighting[BLOCK_SIDE_LEF][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].y);
		intensities[2] = faceLighting[BLOCK_SIDE_LEF][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3fv (npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f (npos[0], npos[1], fpos[2]);	// LBF

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f (npos[0], fpos[1], npos[2]);	// LTB
	}

	// right
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_RIG],
//			mGlobalLightIntensities[BLOCK_SIDE_RIG]);
		intensities[0] = faceLighting[BLOCK_SIDE_RIG][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].x);
		intensities[1] = faceLighting[BLOCK_SIDE_RIG][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].y);
		intensities[2] = faceLighting[BLOCK_SIDE_RIG][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f (fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3fv (fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF
	}

	// top
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_TOP],
//			mGlobalLightIntensities[BLOCK_SIDE_TOP]);
		intensities[0] = faceLighting[BLOCK_SIDE_TOP][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].x);
		intensities[1] = faceLighting[BLOCK_SIDE_TOP][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].y);
		intensities[2] = faceLighting[BLOCK_SIDE_TOP][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);

		glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
		glVertex3f (npos[0], fpos[1], npos[2]);	// LTB

		glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
		glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3fv (fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB

		// draw the underside of the water surface
		if (block.type == globalWaterBlockType) {
			glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

			glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
			glVertex3f (npos[0], fpos[1], npos[2]);	// LTB

			glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
			glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB

			glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
			glVertex3fv (fpos);	// RTF

			glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
			glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF
		}
	}

	// bottom
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_BOT],
//			mGlobalLightIntensities[BLOCK_SIDE_BOT]);
		intensities[0] = faceLighting[BLOCK_SIDE_BOT][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].x);
		intensities[1] = faceLighting[BLOCK_SIDE_BOT][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].y);
		intensities[2] = faceLighting[BLOCK_SIDE_BOT][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3fv (npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f (fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT);
		glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT);
		glVertex3f (npos[0], npos[1], fpos[2]);	// LBF
	}

	// front
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_FRO],
//			mGlobalLightIntensities[BLOCK_SIDE_FRO]);

		intensities[0] = faceLighting[BLOCK_SIDE_FRO][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].x);
		intensities[1] = faceLighting[BLOCK_SIDE_FRO][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].y);
		intensities[2] = faceLighting[BLOCK_SIDE_FRO][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f (npos[0], npos[1], fpos[2]);	// LBF

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3fv (fpos);	// RTF

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF
	}

	// back
	if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
//		intensities = v3d_add (faceLighting[BLOCK_SIDE_BAC],
//			mGlobalLightIntensities[BLOCK_SIDE_BAC]);

		intensities[0] = faceLighting[BLOCK_SIDE_BAC][0] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].x);
		intensities[1] = faceLighting[BLOCK_SIDE_BAC][1] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].y);
		intensities[2] = faceLighting[BLOCK_SIDE_BAC][2] *
			static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].z);

		glColor4fv (intensities);

		glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
		glVertex3f (fpos[0], npos[1], npos[2]);	// RBB

		glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
		glVertex3fv (npos);	// LBB

		glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
		glVertex3f (npos[0], fpos[1], npos[2]);	// LTB

		glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
		glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB
	}
}


*/


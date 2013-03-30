// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AssetManager
// *
// * this is a big mess of quasi-related stuff.
// * it needs a lot of work...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef _assetmanager_h
#define _assetmanager_h

#include <cmath>
#include <vector>


#include "SdlInterface.h"
#include "SoundSystem.h"
#include "LightSource.h"
#include "ObjectLoader.h"
#include "Constants.h"
#include "BlockTypeData.h"
#include "Texture.h"
#include "TextureRegion.h"
#include "BitmapModel.h"




class AssetManager {
public:
	AssetManager (void);
	~AssetManager ();

	int loadAssets (void);
	void freeAssets (void);

	// this is basically a wrapper for loadTexture() from SdlInterface.h
	// should not exist, or if it does, let's take care of destroying
	// the textures/maintaining the textures/models for the context.
	// This would prevent the dependent subsystems (e.g. the 'Views')
	// from having to worry about them. This is an *AssetManager*
	// isn't it?
	static GLuint loadImg(const char *fileName);

	void setDirectionalLightPositions (v3d_t sunPosition, v3d_t moonPosition);

	GLuint getBlankBlockCallListHandle (void) const;
	GLuint getShadedBlockCallListHandle (void) const;

	GLuint getTerrainTextureHandle (void) const;

	void drawBlock (GLfloat height, v3di_t pos, const block_t &block) const;
/*	void drawWorldBlockFace (
		BYTE face,
		const GLfloat *texCoords,
		const GLint *npos,
		const GLint *fpos,
		GLfloat *intensities,
		const GLfloat *faceLighting) const;
*/

	static void drawBlankBlock (void);

	void drawBlock (const v3d_t *intensities) const;
	void drawBlock2 (const v3di_t intensities[8]) const;
	void drawBlock3 (int r, int g, int b) const;
	void drawBlockWithFace (v3di_t intensities) const;

	void drawBlockFace (int whichFace, const GLfloat intensities[4]) const;

	SoundSystem mSoundSystem;

	// TESTING:
	// this stuff needs to be done right...
	Texture *mGunTexture;
	TextureRegion *mGunTextureRegion;
	BitmapModel *mGunBitmapModel;
	// END TESTING


private:
	// copy constructor guard
	AssetManager (const AssetManager &assetManager) { }
	// assignment operator guard
	AssetManager & operator=(const AssetManager &assetManager) { return *this; }

	void drawBlockFaceLeft (const GLfloat intensities[4]) const;
	void drawBlockFaceRight (const GLfloat intensities[4]) const;
	void drawBlockFaceTop (const GLfloat intensities[4]) const;
	void drawBlockFaceBottom (const GLfloat intensities[4]) const;
	void drawBlockFaceFront (const GLfloat intensities[4]) const;
	void drawBlockFaceBack (const GLfloat intensities[4]) const;

// members * * * * * * * * * * * * * * * * * * * * * * *
	int mNumTexturesLoaded;
//	GLuint mBlockTextureHandles[NUM_BLOCK_TYPES];

	GLuint mTerrainTextureHandle;

	GLuint mBlankBlockCallListHandle;
	GLuint mShadedBlockCallListHandle;

	vector <LightSource> mDirectionalLights;

	v3d_t mGlobalLightIntensities[6];

	// HACK * * * * * *
//	GLfloat ONE_OVER_LIGHT_LEVEL_MAX;
};


#endif // _assetmanager_h

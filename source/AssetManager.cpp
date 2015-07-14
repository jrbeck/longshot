#include <AssetManager.h>




// METHODS * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AssetManager::AssetManager() :
  mGunTexture(0),
  mGunTextureRegion(0),
  mGunBitmapModel(0),
  mNumTexturesLoaded(0),
  mTerrainTextureHandle(0),
  mBlankBlockCallListHandle(0),
  mShadedBlockCallListHandle(0)
{
  // set up the sun and moon lights
  LightSource sun;
  sun.setColor(v3d_v(1.0, 0.95, 0.8));
  sun.setDirection(v3d_v(0.0, -1.0, 0.0));

  LightSource moon;
  moon.setColor(v3d_v(0.6, 0.2, 0.2));
  moon.setDirection(v3d_v(0.0, 1.0, 0.0));

  mDirectionalLights.push_back(sun);
  mDirectionalLights.push_back(moon);

  // this is default...
  setDirectionalLightPositions(v3d_v(0.0, 100.0, 0.0), v3d_v(0.0, -100.0, 0.0));

  for (int i = 0; i < NUM_MELEETYPES; i++) {
    mModelDisplayListHandles[i] = 0;
  }
}



AssetManager::~AssetManager() {
  printf("AssetManager going down!!\n");
  freeAssets();
}



int AssetManager::loadAssets() {
  if (loadTexture("art/32_terrain.png", &mTerrainTextureHandle) == 0) {
    mNumTexturesLoaded++;
  }

  // TESTING: BitmapModel
  mGunTexture = new Texture("art/item_sheet.png");
//  mGunTextureRegion = new TextureRegion(0, 0, 45, 21, mGunTexture);
  mGunTextureRegion = new TextureRegion(0, 32, 65, 21, mGunTexture);
  mGunBitmapModel = new BitmapModel(mGunTextureRegion);
  // END TESTING

  // make a call list for the block
  mBlankBlockCallListHandle = glGenLists(1);
  glNewList(mBlankBlockCallListHandle, GL_COMPILE);
    drawBlankBlock();
  glEndList();

//  mBlankBlockCallListHandle = loadObjectFile("art\\models\\3dplus.xml");

  // generate the display list for the shaded block
  mShadedBlockCallListHandle = glGenLists(1);
  glNewList(mShadedBlockCallListHandle, GL_COMPILE);
    drawBlock(mGlobalLightIntensities);
  glEndList();


  // the mace and the axe ...
  // this model type is deprecated
  mModelDisplayListHandles[MELEETYPE_MACE] = ObjectLoader::loadObjectToDisplayList("art\\models\\mace.xml");
  mModelDisplayListHandles[MELEETYPE_AXE] = ObjectLoader::loadObjectToDisplayList("art\\models\\axe.xml");
  // mModelDisplayListHandles[MELEETYPE_MACE] = 0; //ObjectLoader::loadObjectToDisplayList("art\\models\\mace.xml");
  // mModelDisplayListHandles[MELEETYPE_AXE] = 0; //ObjectLoader::loadObjectToDisplayList("art\\models\\axe.xml");

  return 0;
}


void AssetManager::freeAssets() {
  if (mTerrainTextureHandle != 0) {
    glDeleteTextures(1, &mTerrainTextureHandle);
    mTerrainTextureHandle = 0;
  }

  if (mBlankBlockCallListHandle != 0) {
    glDeleteLists(mBlankBlockCallListHandle, 1);
    mBlankBlockCallListHandle = 0;
  }

  if (mShadedBlockCallListHandle != 0) {
    glDeleteLists(mShadedBlockCallListHandle, 1);
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

  for (int i = 0; i < NUM_MELEETYPES; i++) {
    if (mModelDisplayListHandles[i] != 0) {
      glDeleteLists(mModelDisplayListHandles[i], 1);
      mModelDisplayListHandles[i] = 0;
    }
  }

}

int AssetManager::loadTexture(const char* filename, GLuint* texture_handle) {
  SDL_Surface* surface = IMG_Load(filename);

  if (surface == NULL) {
    printf ("AssetManager::loadTexture: IMG_Load failed on %s: %s\n", filename, SDL_GetError());
    // SDL_Quit ();
    return -1;
  }

  // Check that the image's width is a power of 2
  if ((surface->w & (surface->w - 1)) != 0) {
    printf("warning: %s's width is not a power of 2\n", filename);
  }

  // Also check if the height is a power of 2
  if ((surface->h & (surface->h - 1)) != 0) {
    printf("warning: %s's height is not a power of 2\n", filename);
  }

  // get the number of channels in the SDL surface
  GLint numColors = surface->format->BytesPerPixel;
  GLenum textureFormat;
  if (numColors == 4) { // contains an alpha channel
    if (surface->format->Rmask == 0x000000ff) {
      textureFormat = GL_RGBA;
    }
    else {
      textureFormat = GL_BGRA;
    }
  }
  else if (numColors == 3) { // no alpha channel
    if (surface->format->Rmask == 0x000000ff) {
      textureFormat = GL_RGB;
    }
    else {
      textureFormat = GL_BGR;
    }
  }
  else {
    printf("AssetManager::loadTexture: %s is not in a recognized format, not loading\n", filename);
    *texture_handle = 0;
    SDL_FreeSurface(surface);
    return 1;
  }


//    surfaceAlpha = SDL_DisplayFormat(surface);
//    SDL_SetAlpha(surfaceAlpha, 0, SDL_ALPHA_TRANSPARENT);

/*    tempSurfaceFixed = SDL_CreateRGBSurface(
    SDL_SWSURFACE,
    tempSurface->w, tempSurface->h,
    32, R_Mask, G_Mask, B_Mask, A_Mask);
*/

  // Have OpenGL generate a texture object handle for us
  glGenTextures(1, texture_handle);

  // Bind the texture object
  glBindTexture(GL_TEXTURE_2D, *texture_handle);

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//    surfaceAlpha = SDL_DisplayFormatAlpha(surface);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    numColors,
    surface->w, surface->h,
    0,
    textureFormat,
    GL_UNSIGNED_BYTE,
    surface->pixels);

  SDL_FreeSurface(surface);
  return 0;
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
//  mGlobalLightIntensities[BLOCK_SIDE_LEF] = v3d_v (1, 1, 1);
//  mGlobalLightIntensities[BLOCK_SIDE_RIG] = v3d_zero ();
//  mGlobalLightIntensities[BLOCK_SIDE_BOT] = v3d_zero ();
//  mGlobalLightIntensities[BLOCK_SIDE_TOP] = v3d_zero ();
//  mGlobalLightIntensities[BLOCK_SIDE_BAC] = v3d_zero ();
//  mGlobalLightIntensities[BLOCK_SIDE_FRO] = v3d_zero ();

  // create the display list
  // WARNING: assumes the display list handle was obtained (loadAssets) previously

  glNewList (mShadedBlockCallListHandle, GL_COMPILE);
    drawBlock (mGlobalLightIntensities);
  glEndList();
}



GLuint AssetManager::getBlankBlockCallListHandle() const {
  return mBlankBlockCallListHandle;
}



GLuint AssetManager::getShadedBlockCallListHandle() const {
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

    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
    glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
    glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
    glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB

    // right
    glNormal3d (1.0, 0.0, 0.0);

    glColor4d (intensities[BLOCK_SIDE_RIG].x,
      intensities[BLOCK_SIDE_RIG].y,
      intensities[BLOCK_SIDE_RIG].z, 1.0);

    glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
    glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
    glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF

    // top
    glNormal3d (0.0, 1.0, 0.0);

    glColor4d (intensities[BLOCK_SIDE_TOP].x,
      intensities[BLOCK_SIDE_TOP].y,
      intensities[BLOCK_SIDE_TOP].z, 1.0);

    glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
    glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
    glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB

    // bottom
    glNormal3d (0.0, -1.0, 0.0);

    glColor4d (intensities[BLOCK_SIDE_BOT].x,
      intensities[BLOCK_SIDE_BOT].y,
      intensities[BLOCK_SIDE_BOT].z, 1.0);

    glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
    glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
    glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF

    // front
    glNormal3d (0.0, 0.0, 1.0);

    glColor4d (intensities[BLOCK_SIDE_FRO].x,
      intensities[BLOCK_SIDE_FRO].y,
      intensities[BLOCK_SIDE_FRO].z, 1.0);

    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
    glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
    glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
    glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF

    // back
    glNormal3d (0.0, 0.0, -1.0);

    glColor4d (intensities[BLOCK_SIDE_BAC].x,
      intensities[BLOCK_SIDE_BAC].y,
      intensities[BLOCK_SIDE_BAC].z, 1.0);

    glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
    glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
    glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
    glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
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
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB

  glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF

  glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF

  glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB

  // right
  glNormal3d (1.0, 0.0, 0.0);

  glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB

  glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB

  glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF

  // top
  glNormal3d (0.0, 1.0, 0.0);

  glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB

  glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF

  glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB

  // bottom
  glNormal3d (0.0, -1.0, 0.0);

  glColor4d (v3dIntensities[BOX_CORNER_LBB].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities[BOX_CORNER_LBB].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities[BOX_CORNER_LBB].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB

  glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB

  glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF

  glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF

  // front
  glNormal3d (0.0, 0.0, 1.0);


  glColor4d (v3dIntensities[BOX_CORNER_LBF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities[BOX_CORNER_LBF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities[BOX_CORNER_LBF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF

  glColor4d (v3dIntensities[BOX_CORNER_RBF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities[BOX_CORNER_RBF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities[BOX_CORNER_RBF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF

  glColor4d (v3dIntensities[BOX_CORNER_RTF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities[BOX_CORNER_RTF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities[BOX_CORNER_RTF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities[BOX_CORNER_LTF].x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities[BOX_CORNER_LTF].y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities[BOX_CORNER_LTF].z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF

  // back
  glNormal3d (0.0, 0.0, -1.0);

  glColor4d (v3dIntensities[BOX_CORNER_RBB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities[BOX_CORNER_RBB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities[BOX_CORNER_RBB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB
  glColor4d (v3dIntensities[BOX_CORNER_LBB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities[BOX_CORNER_LBB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities[BOX_CORNER_LBB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB
  glColor4d (v3dIntensities[BOX_CORNER_LTB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities[BOX_CORNER_LTB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities[BOX_CORNER_LTB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB
  glColor4d (v3dIntensities[BOX_CORNER_RTB].x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities[BOX_CORNER_RTB].y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities[BOX_CORNER_RTB].z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB
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
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB

  // right
  glNormal3d (1.0, 0.0, 0.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF

  // top
  glNormal3d (0.0, 1.0, 0.0);
  glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB

  // bottom
  glNormal3d (0.0, -1.0, 0.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF

  // front
  glNormal3d (0.0, 0.0, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTF]);  // LTF

  // back
  glNormal3d (0.0, 0.0, -1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner_centered[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner_centered[BOX_CORNER_RTB]);  // RTB
}






void AssetManager::drawBlockWithFace(v3di_t intensities) const {
  v3d_t v3dIntensities;

  v3dIntensities.x = (double)intensities.x * ONE_OVER_LIGHT_LEVEL_MAX;
  v3dIntensities.y = (double)intensities.y * ONE_OVER_LIGHT_LEVEL_MAX;
  v3dIntensities.z = (double)intensities.z * ONE_OVER_LIGHT_LEVEL_MAX;

  // left - BACK
  glNormal3d (-1.0, 0.0, 0.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_LEF].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_LEF].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_LEF].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB

  // right - FACE
  glNormal3d (1.0, 0.0, 0.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_RIG].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_RIG].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_RIG].z, 1.0);
  glTexCoord2f (0.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF

  // top
  glNormal3d (0.0, 1.0, 0.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_TOP].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_TOP].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_TOP].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB

  // bottom
  glNormal3d (0.0, -1.0, 0.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BOT].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BOT].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BOT].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF

  // front - right
  glNormal3d (0.0, 0.0, 1.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_FRO].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_FRO].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_FRO].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF

  // back - left
  glNormal3d (0.0, 0.0, -1.0);

  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
  glColor4d (v3dIntensities.x * mGlobalLightIntensities[BLOCK_SIDE_BAC].x,
    v3dIntensities.y * mGlobalLightIntensities[BLOCK_SIDE_BAC].y,
    v3dIntensities.z * mGlobalLightIntensities[BLOCK_SIDE_BAC].z, 1.0);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
}



void AssetManager::drawBlankBlock (void) {
  // left
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB

  // right
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF

  // top
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);
  glTexCoord2f (0.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f (0.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB

  // bottom
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF

  // front
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF

  // back
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
}



void AssetManager::drawBlockFace(int whichFace, const GLfloat intensities[4]) const {
//  glBegin (GL_QUADS);

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

//  glEnd ();
}



void AssetManager::drawBlockFaceLeft(const GLfloat intensities[4]) const {
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);
  glColor4fv (intensities);

  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
}



void AssetManager::drawBlockFaceRight (const GLfloat intensities[4]) const {
  glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);
  glColor4fv(intensities);

  glTexCoord2f(1.0f, 0.5f); glVertex3fv(cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f(1.0f, 0.0f); glVertex3fv(cube_corner[BOX_CORNER_RTB]);  // RTB
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner[BOX_CORNER_RBF]);  // RBF
}



void AssetManager::drawBlockFaceTop (const GLfloat intensities[4]) const {
  glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);
  glColor4fv(intensities);

  glTexCoord2f(0.0f, 0.0f); glVertex3fv(cube_corner[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f(0.0f, 0.5f); glVertex3fv(cube_corner[BOX_CORNER_LTF]);  // LTF
  glTexCoord2f(0.5f, 0.5f); glVertex3fv(cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f(0.5f, 0.0f); glVertex3fv(cube_corner[BOX_CORNER_RTB]);  // RTB
}



void AssetManager::drawBlockFaceBottom (const GLfloat intensities[4]) const {
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);
  glColor4fv (intensities);

  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (0.5f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 1.0f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
}



void AssetManager::drawBlockFaceFront (const GLfloat intensities[4]) const {
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);
  glColor4fv (intensities);

  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBF]);  // LBF
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBF]);  // RBF
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTF]);  // RTF
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTF]);  // LTF
}



void AssetManager::drawBlockFaceBack (const GLfloat intensities[4]) const {
  glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);
  glColor4fv (intensities);

  glTexCoord2f (0.5f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_RBB]);  // RBB
  glTexCoord2f (1.0f, 0.5f); glVertex3fv (cube_corner[BOX_CORNER_LBB]);  // LBB
  glTexCoord2f (1.0f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_LTB]);  // LTB
  glTexCoord2f (0.5f, 0.0f); glVertex3fv (cube_corner[BOX_CORNER_RTB]);  // RTB
}


GLuint AssetManager::getTerrainTextureHandle() const {
  return mTerrainTextureHandle;
}


void AssetManager::getBlockTextureCoords(int blockType, GLfloat& textureX, GLfloat& textureY) const {
  textureX = ((GLfloat)(blockType % 16) * TEX_MULT);
  textureY = ((GLfloat)(blockType / 16) * TEX_MULT);
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

  GLfloat textureX, textureY;
  getBlockTextureCoords(block.type, textureX, textureY);


  // left
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_LEF],
//      mGlobalLightIntensities[BLOCK_SIDE_LEF]);
    intensities[0] = faceLighting[BLOCK_SIDE_LEF][0];
    intensities[1] = faceLighting[BLOCK_SIDE_LEF][1];
    intensities[2] = faceLighting[BLOCK_SIDE_LEF][2];

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3fv(npos);  // LBB

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f(npos[0], npos[1], fpos[2]);  // LBF

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f(npos[0], fpos[1], fpos[2]);  // LTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f(npos[0], fpos[1], npos[2]);  // LTB
  }

  // right
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_RIG],
//      mGlobalLightIntensities[BLOCK_SIDE_RIG]);
    intensities[0] = faceLighting[BLOCK_SIDE_RIG][0];
    intensities[1] = faceLighting[BLOCK_SIDE_RIG][1];
    intensities[2] = faceLighting[BLOCK_SIDE_RIG][2];

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f(fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f(fpos[0], fpos[1], npos[2]);  // RTB

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3fv(fpos);  // RTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f(fpos[0], npos[1], fpos[2]);  // RBF
  }

  // top
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_TOP],
//      mGlobalLightIntensities[BLOCK_SIDE_TOP]);
    intensities[0] = faceLighting[BLOCK_SIDE_TOP][0];
    intensities[1] = faceLighting[BLOCK_SIDE_TOP][1];
    intensities[2] = faceLighting[BLOCK_SIDE_TOP][2];

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);

    glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
    glVertex3f(npos[0], fpos[1], npos[2]);  // LTB

    glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
    glVertex3f(npos[0], fpos[1], fpos[2]);  // LTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3fv(fpos);  // RTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f(fpos[0], fpos[1], npos[2]);  // RTB

    // draw the underside of the water surface
    if (gBlockData.get(block.type)->solidityType == BLOCK_SOLIDITY_TYPE_LIQUID) {
      glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

      glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
      glVertex3f(npos[0], fpos[1], npos[2]);  // LTB

      glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
      glVertex3f(fpos[0], fpos[1], npos[2]);  // RTB

      glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
      glVertex3fv(fpos);  // RTF

      glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
      glVertex3f(npos[0], fpos[1], fpos[2]);  // LTF
    }
  }

  // bottom
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_BOT],
//      mGlobalLightIntensities[BLOCK_SIDE_BOT]);
    intensities[0] = faceLighting[BLOCK_SIDE_BOT][0];
    intensities[1] = faceLighting[BLOCK_SIDE_BOT][1];
    intensities[2] = faceLighting[BLOCK_SIDE_BOT][2];

    glColor4fv(intensities);

    glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

    glTexCoord2f(textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3fv(npos);  // LBB

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f(fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + TEX_MULT);
    glVertex3f(fpos[0], npos[1], fpos[2]);  // RBF

    glTexCoord2f(textureX + TEX_MULT, textureY + TEX_MULT);
    glVertex3f(npos[0], npos[1], fpos[2]);  // LBF
  }

  // front
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_FRO],
//      mGlobalLightIntensities[BLOCK_SIDE_FRO]);

    intensities[0] = faceLighting[BLOCK_SIDE_FRO][0];
    intensities[1] = faceLighting[BLOCK_SIDE_FRO][1];
    intensities[2] = faceLighting[BLOCK_SIDE_FRO][2];

    glColor4fv(intensities);

    glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f(npos[0], npos[1], fpos[2]);  // LBF

    glTexCoord2f(textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f(fpos[0], npos[1], fpos[2]);  // RBF

    glTexCoord2f(textureX + TEX_MULT, textureY + 0.0f);
    glVertex3fv(fpos);  // RTF

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f(npos[0], fpos[1], fpos[2]);  // LTF
  }

  // back
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_BAC],
//      mGlobalLightIntensities[BLOCK_SIDE_BAC]);

    intensities[0] = faceLighting[BLOCK_SIDE_BAC][0];
    intensities[1] = faceLighting[BLOCK_SIDE_BAC][1];
    intensities[2] = faceLighting[BLOCK_SIDE_BAC][2];

    glColor4fv(intensities);

    glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f(fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f(textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3fv(npos);  // LBB

    glTexCoord2f(textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f(npos[0], fpos[1], npos[2]);  // LTB

    glTexCoord2f(textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f(fpos[0], fpos[1], npos[2]);  // RTB
  }
}




void AssetManager::drawPlantBlock(v3di_t worldPosition, const block_t& block) const {

  glColor4f(
    (GLfloat)block.faceLighting[0][0] * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)block.faceLighting[0][1] * ONE_OVER_LIGHT_LEVEL_MAX,
    (GLfloat)block.faceLighting[0][2] * ONE_OVER_LIGHT_LEVEL_MAX,
    1.0f);

  GLfloat texTop;
  GLfloat texLeft;
  getBlockTextureCoords(block.type, texLeft, texTop);

  GLfloat texBottom = texTop + TEX_MULT_HALF;
  GLfloat texRight = texLeft + TEX_MULT_HALF;

  GLfloat vertXLow = (GLfloat)worldPosition.x;
  GLfloat vertXHigh = (GLfloat)worldPosition.x + 1.0f;
  GLfloat vertYLow = (GLfloat)worldPosition.y;
  GLfloat vertYHigh = (GLfloat)worldPosition.y + 1.0f;
  GLfloat vertZLow = (GLfloat)worldPosition.z;
  GLfloat vertZHigh = (GLfloat)worldPosition.z + 1.0f;

  // front and back of first quad
  glTexCoord2f(texLeft, texTop); glVertex3f(vertXLow, vertYHigh, vertZLow);
  glTexCoord2f(texRight, texTop); glVertex3f(vertXHigh, vertYHigh, vertZHigh);
  glTexCoord2f(texRight, texBottom); glVertex3f(vertXHigh, vertYLow, vertZHigh);
  glTexCoord2f(texLeft, texBottom); glVertex3f(vertXLow, vertYLow, vertZLow);

  glTexCoord2f(texLeft, texTop); glVertex3f(vertXLow, vertYHigh, vertZLow);
  glTexCoord2f(texLeft, texBottom); glVertex3f(vertXLow, vertYLow, vertZLow);
  glTexCoord2f(texRight, texBottom); glVertex3f(vertXHigh, vertYLow, vertZHigh);
  glTexCoord2f(texRight, texTop); glVertex3f(vertXHigh, vertYHigh, vertZHigh);

  // front and back of the second quad
  glTexCoord2f(texRight, texTop); glVertex3f(vertXLow, vertYHigh, vertZHigh);
  glTexCoord2f(texLeft, texTop); glVertex3f(vertXHigh, vertYHigh, vertZLow);
  glTexCoord2f(texLeft, texBottom); glVertex3f(vertXHigh, vertYLow, vertZLow);
  glTexCoord2f(texRight, texBottom); glVertex3f(vertXLow, vertYLow, vertZHigh);

  glTexCoord2f(texRight, texTop); glVertex3f(vertXLow, vertYHigh, vertZHigh);
  glTexCoord2f(texRight, texBottom); glVertex3f(vertXLow, vertYLow, vertZHigh);
  glTexCoord2f(texLeft, texBottom); glVertex3f(vertXHigh, vertYLow, vertZLow);
  glTexCoord2f(texLeft, texTop); glVertex3f(vertXHigh, vertYHigh, vertZLow);
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
  #define TEX_MULT    (0.0625f)
  #define TEX_MULT_HALF  (0.03125f)

  GLfloat textureX = static_cast<GLfloat>(block.type % 16) * TEX_MULT;
  GLfloat textureY = static_cast<GLfloat>(block.type / 16) * TEX_MULT;

  // left
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_LEF],
//      mGlobalLightIntensities[BLOCK_SIDE_LEF]);
    intensities[0] = faceLighting[BLOCK_SIDE_LEF][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].x);
    intensities[1] = faceLighting[BLOCK_SIDE_LEF][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].y);
    intensities[2] = faceLighting[BLOCK_SIDE_LEF][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_LEF].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3fv (npos);  // LBB

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f (npos[0], npos[1], fpos[2]);  // LBF

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f (npos[0], fpos[1], fpos[2]);  // LTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f (npos[0], fpos[1], npos[2]);  // LTB
  }

  // right
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_RIG],
//      mGlobalLightIntensities[BLOCK_SIDE_RIG]);
    intensities[0] = faceLighting[BLOCK_SIDE_RIG][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].x);
    intensities[1] = faceLighting[BLOCK_SIDE_RIG][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].y);
    intensities[2] = faceLighting[BLOCK_SIDE_RIG][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_RIG].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f (fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f (fpos[0], fpos[1], npos[2]);  // RTB

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3fv (fpos);  // RTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f (fpos[0], npos[1], fpos[2]);  // RBF
  }

  // top
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_TOP],
//      mGlobalLightIntensities[BLOCK_SIDE_TOP]);
    intensities[0] = faceLighting[BLOCK_SIDE_TOP][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].x);
    intensities[1] = faceLighting[BLOCK_SIDE_TOP][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].y);
    intensities[2] = faceLighting[BLOCK_SIDE_TOP][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_TOP].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);

    glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
    glVertex3f (npos[0], fpos[1], npos[2]);  // LTB

    glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
    glVertex3f (npos[0], fpos[1], fpos[2]);  // LTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3fv (fpos);  // RTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f (fpos[0], fpos[1], npos[2]);  // RTB

    // draw the underside of the water surface
    if (block.type == globalWaterBlockType) {
      glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

      glTexCoord2f (textureX + 0.0f, textureY + 0.0f);
      glVertex3f (npos[0], fpos[1], npos[2]);  // LTB

      glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
      glVertex3f (fpos[0], fpos[1], npos[2]);  // RTB

      glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
      glVertex3fv (fpos);  // RTF

      glTexCoord2f (textureX + 0.0f, textureY + TEX_MULT_HALF);
      glVertex3f (npos[0], fpos[1], fpos[2]);  // LTF
    }
  }

  // bottom
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_BOT],
//      mGlobalLightIntensities[BLOCK_SIDE_BOT]);
    intensities[0] = faceLighting[BLOCK_SIDE_BOT][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].x);
    intensities[1] = faceLighting[BLOCK_SIDE_BOT][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].y);
    intensities[2] = faceLighting[BLOCK_SIDE_BOT][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BOT].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3fv (npos);  // LBB

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f (fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT);
    glVertex3f (fpos[0], npos[1], fpos[2]);  // RBF

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT);
    glVertex3f (npos[0], npos[1], fpos[2]);  // LBF
  }

  // front
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_FRO],
//      mGlobalLightIntensities[BLOCK_SIDE_FRO]);

    intensities[0] = faceLighting[BLOCK_SIDE_FRO][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].x);
    intensities[1] = faceLighting[BLOCK_SIDE_FRO][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].y);
    intensities[2] = faceLighting[BLOCK_SIDE_FRO][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_FRO].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f (npos[0], npos[1], fpos[2]);  // LBF

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3f (fpos[0], npos[1], fpos[2]);  // RBF

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3fv (fpos);  // RTF

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f (npos[0], fpos[1], fpos[2]);  // LTF
  }

  // back
  if (block.faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
//    intensities = v3d_add (faceLighting[BLOCK_SIDE_BAC],
//      mGlobalLightIntensities[BLOCK_SIDE_BAC]);

    intensities[0] = faceLighting[BLOCK_SIDE_BAC][0] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].x);
    intensities[1] = faceLighting[BLOCK_SIDE_BAC][1] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].y);
    intensities[2] = faceLighting[BLOCK_SIDE_BAC][2] *
      static_cast<GLfloat>(mGlobalLightIntensities[BLOCK_SIDE_BAC].z);

    glColor4fv (intensities);

    glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + TEX_MULT_HALF);
    glVertex3f (fpos[0], npos[1], npos[2]);  // RBB

    glTexCoord2f (textureX + TEX_MULT, textureY + TEX_MULT_HALF);
    glVertex3fv (npos);  // LBB

    glTexCoord2f (textureX + TEX_MULT, textureY + 0.0f);
    glVertex3f (npos[0], fpos[1], npos[2]);  // LTB

    glTexCoord2f (textureX + TEX_MULT_HALF, textureY + 0.0f);
    glVertex3f (fpos[0], fpos[1], npos[2]);  // RTB
  }
}


*/


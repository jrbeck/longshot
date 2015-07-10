#include "SkySim.h"

SkySim::SkySim() :
  mCloudSim(NULL)
{
  mInitialTimeOfDay = 0.0;
  mTimeFactor = 500000.0;

  mSunPosition = v3d_v (10000.0, 5000.0, 5000.0);
  mSunColor[0] = 1.0f;
  mSunColor[1] = 1.0f;
  mSunColor[2] = 0.5f;

  mStarAlpha = 0.0;

  mSkyColor[0] = 0.3f;
  mSkyColor[1] = 0.4f;
  mSkyColor[2] = 0.6f;
}


SkySim::~SkySim() {
  if (mCloudSim != NULL) {
    delete mCloudSim;
  }
}


void SkySim::initialize(v3d_t playerStartPosition) {

  // set up stars
  for (size_t i = 0; i < NUM_STARS; i++) {
    mStars[i].x = r_num (-10.0, 10.0);
    mStars[i].y = r_num (-10.0, 10.0);
    mStars[i].z = r_num (-10.0, 10.0);

    mStars[i] = v3d_scale (10000.0, v3d_normalize (mStars[i]));
  }

  // set up sun
  mCurrentSunPosition.x = (mSunPosition.x * cos (0.0)) - (mSunPosition.y * sin (0.0));
  mCurrentSunPosition.y = (mSunPosition.x * sin (0.0)) + (mSunPosition.y * cos (0.0));
  mCurrentSunPosition.z = mSunPosition.z;

//  assetManager.setDirectionalLightPositions (mCurrentSunPosition,
//    v3d_neg (mCurrentSunPosition));

  // now figure out the sky color and the world lighting
  setSkyColorAndWorldLighting ();

  // these can be combined eh?
  if (mCloudSim != NULL) {
    delete mCloudSim;
  }
  mCloudSim = new CloudSim();
  mCloudSim->newClouds (playerStartPosition);
}


void SkySim::setSkyColorAndWorldLighting(void) {
  mStarAlpha = static_cast<GLfloat>(v3d_dot (v3d_normalize (mCurrentSunPosition), v3d_v (0.0, -1.0, 0.0)));
  mStarAlpha *= 1.5;
  if (mStarAlpha > 1.0) {
    mStarAlpha = 1.0;
  }
  else if (mStarAlpha < -1.0) {
    mStarAlpha = -1.0;
  }

//  mWorldLightingFloor = 0.4; //0.40 - (mStarAlpha * 0.20);
//  defaultLightingCeiling = 0.9; //0.65 - (mStarAlpha * 0.20);

//  printf ("starAlpha: %3.5f\n", starAlpha);
//  printf ("f: %3.5f   c: %3.5f\n", mWorldLightingFloor, defaultLightingCeiling);

  GLfloat currentSkyColor[4] = {
    mSkyColor[0] * (1.0f - max(0.0f, mStarAlpha)),
    mSkyColor[1] * (1.0f - max(0.0f, mStarAlpha)),
    mSkyColor[2] * (1.0f - max(0.0f, mStarAlpha)),
    1.0f };

  glClearColor(
    currentSkyColor[0],
    currentSkyColor[1],
    currentSkyColor[2],
    currentSkyColor[3]);

  glFogfv(GL_FOG_COLOR, currentSkyColor);
}


void SkySim::update() {
  static Uint32 lastCloudTick = -1000;
  Uint32 thisTick = SDL_GetTicks ();

  // now for the cloud updates
  if (thisTick - lastCloudTick > 2000) {
    mCloudSim->update (mStarAlpha);
    lastCloudTick = thisTick;
  }
}



void SkySim::draw(gl_camera_c &cam, v3d_t playerPosition) {
  double secs = (static_cast<double>(SDL_GetTicks ()) / mTimeFactor) + mInitialTimeOfDay;

//  secs = 0.0;
//  secs = 3.14;

  mCurrentSunPosition.x = (mSunPosition.x * cos (secs)) - (mSunPosition.y * sin (secs));
  mCurrentSunPosition.y = (mSunPosition.x * sin (secs)) + (mSunPosition.y * cos (secs));
  mCurrentSunPosition.z = mSunPosition.z;

//  assetManager.setDirectionalLightPositions (mCurrentSunPosition, v3d_neg (mCurrentSunPosition));

  // 0.075, 0.35, 0.45 is a great color!
  setSkyColorAndWorldLighting ();

  // we'll use these to restore them afterwards
  v2d_t oldNearAndFar = cam.getNearAndFar ();

  v2d_t sunClippingDistances = v2d_v (10.0, 100000.0);
  cam.setNearAndFar (sunClippingDistances);

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_2D);
  glEnable (GL_COLOR_MATERIAL);
  glDisable (GL_FOG);
  glEnable (GL_BLEND);

  // draw the sun
  glPushMatrix ();
    glTranslated (mCurrentSunPosition.x + playerPosition.x,
      mCurrentSunPosition.y + playerPosition.y,
      mCurrentSunPosition.z + playerPosition.z);

    glScaled (500.0, 500.0, 500.0);
    glColor4f (mSunColor[0], mSunColor[1], mSunColor[2], -(mStarAlpha - 0.25f));

    glBegin (GL_QUADS);
      AssetManager::drawBlankBlock ();
    glEnd ();
  glPopMatrix ();

  // draw the moon
  glPushMatrix ();
    glTranslated (-mCurrentSunPosition.x + playerPosition.x,
      -mCurrentSunPosition.y + playerPosition.y,
      -mCurrentSunPosition.z + playerPosition.z);

    glScaled (900.0, 900.0, 900.0);
    // nice moon blue color
//    glColor4f (0.75, 1.0, 1.0, mStarAlpha + 0.25);

    // red
    glColor4f (0.8f, 0.0f, 0.1f, mStarAlpha + 0.25f);

    glBegin (GL_QUADS);
      AssetManager::drawBlankBlock ();
    glEnd ();
  glPopMatrix ();


  // draw the clouds
  mCloudSim->draw (cam);

  // draw the stars
  for (size_t i = 0; i < NUM_STARS; i++) {
    glPushMatrix ();
      glRotated (secs, 0.0, 0.0, 1.0);

      glTranslated (mStars[i].x + playerPosition.x,
        mStars[i].y + playerPosition.y,
        mStars[i].z + playerPosition.z);

      glScalef (100.0f, 100.0f, 100.0f);
      glColor4f (1.0f, 1.0f, 0.5f, mStarAlpha);

      glBegin (GL_QUADS);
        AssetManager::drawBlankBlock ();
      glEnd ();
    glPopMatrix ();
  }

  glDisable (GL_BLEND);
  glEnable (GL_FOG);
  glDisable (GL_COLOR_MATERIAL);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_DEPTH_TEST);

  // restore the view volume
  cam.setNearAndFar (oldNearAndFar);
}


int SkySim::getSkyLightLevel(void) {
  double lightRange = (LIGHT_LEVEL_MAX - 8) - (LIGHT_LEVEL_MIN);
//  return LIGHT_LEVEL_MIN + (BYTE)(lightRange * (1.0 - max(mStarAlpha, 0.0f)));
  static BYTE MOON_LEVEL_HACK = 4;
  return MOON_LEVEL_HACK + (BYTE)(lightRange * (1.0 - max(mStarAlpha, 0.0f)));
}




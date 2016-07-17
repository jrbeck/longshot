#include "OrbitSky.h"

OrbitSky::OrbitSky() {
}

OrbitSky::~OrbitSky() {
}

void OrbitSky::setOrbit(Galaxy& galaxy, size_t planetHandle) {
  mBodies.clear();

  // space is so black...
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  celestial_body body;
  body.color[3] = 1.0f;

  // set up the stars
  for (int i = 0; i < 150; ++i) {
    body.color[0] = r_num(0.8, 1.0);
    body.color[1] = r_num(0.8, 1.0);
    body.color[2] = r_num(0.8, 1.0);
    body.pos = v3d_random(1000.0);
    body.size = 1.0f;
    mBodies.push_back(body);
  }

  // set up the sun
  GLfloat* sunColor = galaxy.getStarSystemByHandle(planetHandle)->mStarColor;
  body.color[0] = sunColor[0];
  body.color[1] = sunColor[1];
  body.color[2] = sunColor[2];

  body.pos.x = 100.0;
  body.pos.y = r_num(-25.0, 25.0);
  body.pos.z = 100.0;

  body.size = 10.0f;

  mBodies.push_back(body);

  // set up the planet
  body.color[0] = 0.0f;
  body.color[1] = 0.8f;
  body.color[2] = 0.3f;

  body.pos.x = -100.0;
  body.pos.y = r_num(-25.0, 25.0);
  body.pos.z = 100.0;

  body.size = 40.0f;

  mBodies.push_back(body);
}

void OrbitSky::draw(GlCamera& cam, v3d_t playerPosition) {
  // we'll use these to restore them afterwards
  v2d_t oldNearAndFar = cam.getNearAndFar ();

  v2d_t sunClippingDistances = v2d_v (10.0, 100000.0);
  cam.setNearAndFar (sunClippingDistances);

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_2D);
  glEnable (GL_COLOR_MATERIAL);
  glDisable (GL_FOG);
  glEnable (GL_BLEND);

  size_t numBodies = mBodies.size();
  for (size_t i = 0; i < numBodies; ++i) {
    glPushMatrix ();
      glTranslated(mBodies[i].pos.x + playerPosition.x,
        mBodies[i].pos.y + playerPosition.y,
        mBodies[i].pos.z + playerPosition.z);

      glScalef(mBodies[i].size, mBodies[i].size, mBodies[i].size);
      glColor4fv(mBodies[i].color);

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

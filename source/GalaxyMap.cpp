#include "GalaxyMap.h"

GalaxyMap::GalaxyMap(GameWindow* gameWindow) :
  mZoomLevel(ZOOM_GALAXY),
  mSelectedSystem(-1),
  mLeftMouseButtonClicked(false)
{
  mGameWindow = gameWindow;
  mMenu = new menu_c();

  mResult.action = ACTION_NONE;
  mResult.starSystem = NULL;
  mResult.planet = NULL;
}


GalaxyMap::~GalaxyMap() {
  if (mMenu != NULL) {
    delete mMenu;
  }
}


GalaxyMapResult GalaxyMap::enterViewMode(Galaxy* galaxy, Planet* selectedPlanet) {
  mGalaxy = galaxy;

  setUpOpenGl();

  int quit = 0;
  while (!quit && mResult.action == ACTION_NONE) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    setUpOpenGl();

    if (mZoomLevel == ZOOM_GALAXY) {
      drawGalaxy(selectedPlanet);
    }
    else {
      if (mSelectedSystem < 0 || mSelectedSystem >= galaxy->mStarSystems.size()) {
        mZoomLevel = ZOOM_GALAXY;
        mSelectedSystem = -1;
      }
      else {
        drawStarSystem(*galaxy->mStarSystems[mSelectedSystem], selectedPlanet);
      }
    }


    // draw the menu
    mMenu->draw();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    mGameWindow->swapBuffers();

    quit = handleInput();

    // allow 'esc' to take us from the system map back to
    // the galaxy map
    if (quit == 1 && mZoomLevel != ZOOM_GALAXY) {
      mZoomLevel = ZOOM_GALAXY;
      quit = 0;
      mLeftMouseButtonClicked = false;
    }
  }

  return mResult;
}



void GalaxyMap::drawGalaxy(Planet *selectedPlanet) {

  static GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat selectedColor[4] = { 0.0f, 0.83f, 0.0f, 1.0f };

  v2d_t mouse;



  mouse.x = GALACTIC_WIDTH * (mMousePos.x / SCREEN_W);
  mouse.y = GALACTIC_HEIGHT * (1.0 - (mMousePos.y / SCREEN_H));

  int nearest = 0;
  double nearestDist = v2d_dist(mouse, mGalaxy->mStarSystems[nearest]->mPosition);

  size_t numStarSystems = mGalaxy->mStarSystems.size();
  for (size_t i = 0; i < numStarSystems; i++) {
    double dist = v2d_dist(mouse, mGalaxy->mStarSystems[i]->mPosition);
    if (dist < nearestDist) {
      nearestDist = dist;
      nearest = i;
    }

    drawHex(0.5f, mGalaxy->mStarSystems[i]->mPosition, mGalaxy->mStarSystems[i]->mStarColor);
  }

  StarSystem *inRangeSystem = NULL;

  // show the nearest if in range
  if (nearestDist < 5.0) {
    inRangeSystem = mGalaxy->mStarSystems[nearest];
    drawRing(1.5f, mGalaxy->mStarSystems[nearest]->mPosition, color);

    if (mLeftMouseButtonClicked) {
      mZoomLevel = ZOOM_SYSTEM;
      mSelectedSystem = nearest;
      mLeftMouseButtonClicked = false;
    }
  }

  // show the currently orbited system if any
  if (selectedPlanet != NULL) {
    StarSystem *selectedSystem;
    selectedSystem = mGalaxy->getStarSystemByHandle(selectedPlanet->mHandle);
    if (selectedSystem != NULL) {
      drawRing(1.5f, selectedSystem->mPosition, selectedColor);
    }

    // now draw the line from the nearest StarSystem (if in range)
    // to the selected StarSystem (if they're not the same)
    if (inRangeSystem != NULL &&
      inRangeSystem->mHandle != selectedSystem->mHandle)
    {
      glBegin(GL_LINES);
        glVertex3d(inRangeSystem->mPosition.x, inRangeSystem->mPosition.y, 0.0);
        glVertex3d(selectedSystem->mPosition.x, selectedSystem->mPosition.y, 0.0);
      glEnd();
    }

  }

  // this might need to be updated
  updateGalaxyMenu(mGalaxy->getStarSystemByHandle(selectedPlanet->mHandle), inRangeSystem);
}



void GalaxyMap::drawStarSystem(StarSystem &starSystem, Planet *selectedPlanet) {
  float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float selectedColor[4] = { 0.0f, 0.83f, 0.0f, 1.0f };




  v2d_t starPosition = { GALACTIC_WIDTH * 0.5, GALACTIC_HEIGHT * 0.5 };

  v2d_t mouse;
  mouse.x = (GALACTIC_WIDTH * (mMousePos.x / SCREEN_W)) - starPosition.x;
  mouse.y = (GALACTIC_HEIGHT * (1.0 - (mMousePos.y / SCREEN_H))) - starPosition.y;

  int hoverPlanet = -1;
  int chosenPlanet = -1;

  int nearest = -1;
  double nearestDist = 1000000.0;

  // draw the star
  drawHex(starSystem.mStarRadius, starPosition, starSystem.mStarColor);

  // draw the planets
  size_t numPlanets = starSystem.mPlanets.size();
  for (size_t i = 0; i < numPlanets; i++) {
    double orbitRadius = starSystem.mPlanets[i]->mOrbitRadius;
    double orbitAngle = starSystem.mPlanets[i]->mAngle;

    starSystem.mPlanets[i]->mPosition.x = orbitRadius * cos(orbitAngle);
    starSystem.mPlanets[i]->mPosition.y = orbitRadius * sin(orbitAngle);

    drawHex(starSystem.mPlanets[i]->mRadius, v2d_add(starSystem.mPlanets[i]->mPosition, starPosition), color);
    drawRing(orbitRadius, starPosition, color);

    // find the nearest planet to the mouse cursor
    if (i == 0) {
      nearest = 0;
      nearestDist = v2d_dist(mouse, starSystem.mPlanets[i]->mPosition);
    }
    else {
      double dist = v2d_dist(mouse, starSystem.mPlanets[i]->mPosition);
      if (dist < nearestDist) {
        nearestDist = dist;
        nearest = i;
      }
    }
  }

  // circle the nearest if in range
  if (nearestDist < 5.0) {
    drawRing(
      starSystem.mPlanets[nearest]->mRadius * 1.25f,
      v2d_add(starSystem.mPlanets[nearest]->mPosition, starPosition),
      color);

    // check if we have a selection
    if (mLeftMouseButtonClicked) {
      mResult.action = ACTION_WARP;
      mResult.starSystem = &starSystem;
      mResult.planet = starSystem.mPlanets[nearest];
      mLeftMouseButtonClicked = false;
      return;
    }
  }

  // draw a ring around the selected planet if non-NULL and
  // in this system
  if (selectedPlanet != NULL) {
    Planet *selPlanet; // this should be the same as selectedPlanet in the end
    selPlanet = starSystem.getPlanetByHandle(selectedPlanet->mHandle);
    if (selPlanet != NULL) {
      drawRing(
        selPlanet->mRadius * 1.25f,
        v2d_add(selPlanet->mPosition, starPosition),
        selectedColor);
    }

  }


  updateStarSystemMenu();

}


void GalaxyMap::updateGalaxyMenu(StarSystem *currentSystem, StarSystem *hoverSystem) {
  mMenu->clear();

  static GLfloat colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat colorGrey[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

  static v2d_t fontSize = { 0.01, 0.02 };

  mMenu->addText(
    v2d_v(0.1, 0.1),
    v2d_v(0.3, 0.05),
    fontSize,
    "blah",
    TEXT_JUSTIFICATION_CENTER,
    colorWhite,
    colorGrey);

}



void GalaxyMap::updateStarSystemMenu() {
  mMenu->clear();


}




void GalaxyMap::setUpOpenGl() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

//	glViewport(0, 0, SCREEN_W, SCREEN_H);
//	glOrtho (0, SCREEN_W, SCREEN_H, 0, -1, 1);
  glOrtho(0.0, GALACTIC_WIDTH, GALACTIC_HEIGHT, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glDisable(GL_TEXTURE_2D);

//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	SDL_GL_SwapBuffers();
}




void GalaxyMap::drawHex(float radius, v2d_t center, const GLfloat color[4]) {
  if (radius <= 0.0f) return;
  double degInRad;
  glColor4fv(color);
  glBegin(GL_TRIANGLE_FAN);
    for (int i = 450; i > 90; i -= 60) {
      degInRad = DEG2RAD ((double)i);
      glVertex2f (
        (GLfloat)((cos(degInRad) * radius) + center.x),
        (GLfloat)((sin(degInRad) * radius) + center.y));
    }
  glEnd();
}



void GalaxyMap::drawRing(float radius, v2d_t center, const GLfloat color[4]) {
  if (radius <= 0.0f) return;
  double degInRad;
  glColor4fv (color);
  glBegin (GL_LINE_STRIP);
    // initial vertex
    glVertex2d (center.x + radius, center.y);
    
    // other vertices
    for (int i = 5; i < 360; i += 5) {
      degInRad = DEG2RAD((double)i);
      glVertex2d (
        center.x + (cos(degInRad) * radius),
        center.y + (sin(degInRad) * radius));
    }

    // final vertex
    glVertex2d (center.x + radius, center.y);
  glEnd ();
}



// handle an SDL_Event
int GalaxyMap::handleInput (void) {
  int quit = 0;
  mMouseMoved = 0;
  mLeftMouseButtonClicked = false;

  // goes through all the queued events and deals with each one
  while (SDL_PollEvent (&sdlevent) && !quit) {
    switch (sdlevent.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_KEYDOWN:
        quit = handleKeystroke ();
        break;

      case SDL_KEYUP:
        handleKeyup();
        break;

      case SDL_MOUSEMOTION:
        mMouseDelta.x = -sdlevent.motion.xrel;
        mMouseDelta.y = sdlevent.motion.yrel;

        mMousePos.x = sdlevent.motion.x;
        mMousePos.y = SCREEN_H - sdlevent.motion.y;

        mMouseMoved = 1;

        break;

      // handle the mousebuttondown event
      case SDL_MOUSEBUTTONDOWN:
        handleMouseButtonDown(sdlevent.button.button, v2d_v(sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      case SDL_MOUSEBUTTONUP:
        handleMouseButtonUp(sdlevent.button.button, v2d_v(sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      case SDL_MOUSEWHEEL:
        // this doens't exist ... hasn't been needed yet
 //       handleMouseWeelEvent(sdlevent.wheel);

      default:
        break;
    }
  }


  // RTS MODE
  // handle mouse drag
  // FIXME: not here please!
  Uint8 ms;
  ms = SDL_GetMouseState(NULL, NULL);

  if (mMouseMoved && (ms & SDL_BUTTON (SDL_BUTTON_LEFT))) {
    v2d_t md = v2d_scale (mMouseDelta, 0.1);

    md.x = -md.x;

//		mRtsCam.translate (md);
  }
  if (mMouseMoved && (ms & SDL_BUTTON (SDL_BUTTON_RIGHT))) {
    v2d_t md = v2d_scale (mMouseDelta, 0.002);

    md.x = -md.x;

//		mRtsCam.pan (md);
  }

  return quit;
}



int GalaxyMap::handleKeystroke (void) {
  switch (sdlevent.key.keysym.sym) {
    case SDLK_ESCAPE:	// quit
      return 1;

    case SDLK_a:
      break;

    case SDLK_z:
      break;

  }

  // don't quit!
  return 0;
}


int GalaxyMap::handleKeyup (void) {
/*	if (*mode == MODE_PLAYER) {
    switch (sdlevent.key.keysym.sym) {
      case SDLK_w:
        break;


      case SDLK_SPACE:
        break;
    }
  }
*/

  return 0;
}



void GalaxyMap::handleMouseButtonDown (int button, v2d_t pos) {
  switch (button) {
 
    case SDL_BUTTON_RIGHT:
      break;
  }
}



void GalaxyMap::handleMouseButtonUp (int button, v2d_t pos) {
  switch (button) {
  case SDL_BUTTON_LEFT:
    mLeftMouseButtonClicked = true;
    break;
  case SDL_BUTTON_RIGHT:
    break;
  }
}









#include "Edit.h"

edit_c::edit_c(void) {
  // initialize the terrain
//  printf ("%6d: terrain\n", SDL_GetTicks ());
//  ter.resize (DEFAULT_SIDE_UNIV);
}


edit_c::~edit_c(void) {
}


void edit_c::setup_opengl(void) {
  // set the clear color
  glClearColor(0.0f, 0.35f, .6f, 0.00f); // nice deep sky blue color
//  glClearColor (0.2f, 0.2f, 0.2f, 0.0f);
//  glClearColor (0.5f, 0.2f, 0.0f, 0.0f);  // orangey

  // set up the viewport
  glViewport(0, 0, SCREEN_W, SCREEN_H);

  // enable various things
  glEnable(GL_TEXTURE_2D);            // Enable 2D Texture Mapping
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE);        // Select The Type Of Blending
  glShadeModel(GL_SMOOTH);            // Enables Smooth Color Shading
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glEnable(GL_CULL_FACE);

  glEnable(GL_LIGHTING);

//  glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
//  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  // LIGHTING
  GLfloat LightAmbient[] =  { 0.5f, 0.2f, 0.0f, 1.0f };
  GLfloat LightDiffuse[] =  { 0.7f, 0.5f, 0.0f, 1.0f };
  GLfloat LightPosition[] =  { 0.0f, 100.0f, 0.0f, 1.0f };

  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);    // Setup The Ambient Light
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);    // Setup The Diffuse Light
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);  // Position The Light
  glEnable(GL_LIGHT1);                // Enable Light One
//  glDisable (GL_LIGHT1);

  glDisable(GL_BLEND);

  // FOG
  GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };  // Storage For Three Types Of Fog
  GLuint fogfilter = 2;                // Which Fog Mode To Use
//  GLfloat  fogColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};    // Fog Color
  GLfloat  fogColor[4] = {0.5f, 0.2f, 0.0f, 1.0f};    // Fog Color

  glFogi(GL_FOG_MODE, fogMode[fogfilter]);      // Fog Mode
  glFogfv(GL_FOG_COLOR, fogColor);          // Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.15f);            // How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
  glFogf(GL_FOG_START, 250.0f);            // Fog Start Depth
  glFogf(GL_FOG_END, 300.0f);            // Fog End Depth
  glDisable(GL_FOG);                  // Enables GL_FOG

  // go ahead and clear the buffer
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapBuffers();
}


int edit_c::enter_edit_mode (void) {
  printf ("%6d: entered EDIT mode ----------------\n", SDL_GetTicks ());

  // change the mouse mode
  SDL_WM_GrabInput (SDL_GRAB_OFF);
  SDL_ShowCursor (1);

  int quit = 0;

  unsigned int ticks = SDL_GetTicks ();
  int frame = 0;

  int blocks_drawn = 0;

  setup_opengl ();

  while (!quit) {
    unsigned int delta_t = SDL_GetTicks () - ticks;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // get the camera info
    gl_camera_c *cam = rts.gl_cam_setup ();

    // draw the octree
//    blocks_drawn += oct->draw (*cam);

    // draw the terrain as mesh
    ter.draw ();

    // draw the trees
//    forest.draw_block ();

    // swap the back buffer to the front
    SDL_GL_SwapBuffers ();

    // make sure everything goes through
    glFlush ();

    // do some frames per second calculating
    frame++;

    if (delta_t >= 5000) {
      printf ("FPS: %f, total frames: %d\n", (double)frame / ((double)delta_t / 1000.0), frame);
      printf ("average blocks per frame: %10.2f\n", (double)blocks_drawn / (double)frame);
      frame = 0;
      blocks_drawn = 0;
      ticks = SDL_GetTicks ();
    }

    // deal with the input
    quit = handle_input ();
  }

  printf ("%6d: exiting EDIT mode ----------------\n", SDL_GetTicks ());
  return 0;
}




// handle an SDL_Event
int edit_c::handle_input (void) {
  int quit = 0;
  mouse_moved = 0;

  // goes through all the queued events and deals with each one
  while (SDL_PollEvent (&sdlevent) && !quit) {
    switch (sdlevent.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      case SDL_KEYDOWN:
        quit = handle_keystroke ();
        break;

      case SDL_KEYUP:
        handle_keyup ();
        break;

      case SDL_MOUSEMOTION:
        mouse_delta.x = -sdlevent.motion.xrel;
        mouse_delta.y = sdlevent.motion.yrel;

        mouse_position.x = sdlevent.motion.x;
        mouse_position.y = SCREEN_H - sdlevent.motion.y;

        mouse_moved = 1;

        break;

      // handle the mousebuttondown event
      case SDL_MOUSEBUTTONDOWN:
        handle_mouse_button_down (sdlevent.button.button, v2d_v (sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      case SDL_MOUSEBUTTONUP:
        handle_mouse_button_up (sdlevent.button.button, v2d_v (sdlevent.button.x, SCREEN_H - sdlevent.button.y));
        break;

      default:
        break;
    }
  }


  // RTS MODE
  // handle mouse drag
  // FIXME: not here please!
  Uint8 ms;
  ms = SDL_GetMouseState (NULL, NULL);

  if (mouse_moved && (ms & SDL_BUTTON (SDL_BUTTON_LEFT))) {
    v2d_t md = v2d_scale (mouse_delta, 0.1);

    md.x = -md.x;

    rts.translate (md); }
  if (mouse_moved && (ms & SDL_BUTTON (SDL_BUTTON_RIGHT))) {
    v2d_t md = v2d_scale (mouse_delta, 0.002);

    md.x = -md.x;

    rts.pan (md); }

  return quit;
}


// deal with a keystroke
int edit_c::handle_keystroke (void) {
  Terrain t1 (8);
  t1.generateTilable (1.0, mPrng);
  t1.normalize (0.0, 10.0);

  Terrain t2 (64);
  t2.generateTilable (2.0, mPrng);
  t2.normalize (0.0, 4.0);

  switch (sdlevent.key.keysym.sym) {
    case SDLK_ESCAPE:  // quit
      return 1;

    case SDLK_t: // seed a new terrain
/*      for (int j = 0; j < DEFAULT_SIDE_UNIV; j++) {
        for (int i = 0; i < DEFAULT_SIDE_UNIV; i++) {
          double u, v;

          u = 2.0 * static_cast<double>(i) * (8.0 / DEFAULT_SIDE_UNIV);
          v = 2.0 * static_cast<double>(j) * (8.0 / DEFAULT_SIDE_UNIV);

          double val1 = t1.getValueBilerp (u, v);

          u = 1.0 * static_cast<double>(i) * (64.0 / DEFAULT_SIDE_UNIV);
          v = 1.0 * static_cast<double>(j) * (64.0 / DEFAULT_SIDE_UNIV);

          double val2 = t2.getValueBilerp (u, v);

          ter.set_value (i, j, val1 + val2);

        }
      }

      ter.normalize (-20.0, 50.0);

      // convert the the terrain to an octree
      oct->from_terrain (ter);
*/
      break;

    case SDLK_o: // smooth the terrain
      ter.smooth ();

      // convert the the terrain to an octree
//      oct->from_terrain (ter);

      break;

    case SDLK_i: //alt_smooth the terrain
      ter.alt_smooth ();

      // convert the the terrain to an octree
//      oct->from_terrain (ter);

      break;

    case SDLK_p:    // pause
//      paused = (paused + 1) % 2;
      break;

    case SDLK_r:    // generate a random number
      rand ();
      break;

    default:
      break;
  }

  // don't quit!
  return 0;
}


// deal with a key up event
int edit_c::handle_keyup (void) {
/*  if (*mode == MODE_PLAYER) {
    switch (sdlevent.key.keysym.sym) {
      case SDLK_w:
        player->add_input_event (INPUT_TYPE_FORE_UP, 1.0);
        break;

      case SDLK_s:
        player->add_input_event (INPUT_TYPE_BACK_UP, 1.0);
        break;

      case SDLK_a:
        player->add_input_event (INPUT_TYPE_LEFT_UP, 1.0);
        break;

      case SDLK_d:
        player->add_input_event (INPUT_TYPE_RIGHT_UP, 1.0);
        break;

      case SDLK_SPACE:  // jump
        player->add_input_event (INPUT_TYPE_JUMP_UP, 1.0);
        break;

      default:
        break; } }*/

  return 0;
}


// deal with a mouse button down event
void edit_c::handle_mouse_button_down (int button, v2d_t pos) {
  switch (button) {
    case SDL_BUTTON_WHEELUP:
      // zoom in
      rts.zoom (-20.0);
      break;

    case SDL_BUTTON_WHEELDOWN:
      // zoom out
      rts.zoom (20.0);
      break;

    case SDL_BUTTON_RIGHT:
      break;

    default:
      break; }
}


// deal with a mouse button up event
void edit_c::handle_mouse_button_up (int button, v2d_t pos) {
/*  if (*mode == MODE_PLAYER) {
    switch (button) {
      case SDL_BUTTON_RIGHT:
        player->add_input_event (INPUT_TYPE_ROCKET_UP, 1.0);
        break;

      default:
        break;
    }
  }*/
}


// create a random world for a game session
void edit_c::create_world_for_game (void) {
  // generate the terrain
  ter.generateTilable (DEFAULT_TERRAIN_OFFSET, mPrng);

  // smooth the terrain
  ter.smooth ();

  ter.normalize (-25.0, 75.0);

  // create the octree
//  oct->from_terrain (ter);
}



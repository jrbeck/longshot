// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * edit_c
// *
// * deprecated - please remove
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef _edit_h
#define _edit_h

#include "v2d.h"

#include "RtsCam.h"
#include "Terrain.h"
#include "World.h"



class edit_c {
  rts_cam_c rts;
  Terrain ter;
  PseudoRandom mPrng;

  SDL_Event sdlevent;

  v2d_t mouse_position;
  v2d_t mouse_delta;
  int mouse_moved;

//  int draw_mode;

public:
  edit_c (void);
  ~edit_c (void);

  void setupOpenGl (void);

  int enter_edit_mode (void);

  int handle_input (void);
  int handle_keystroke (void);
  int handle_keyup (void);
  void handle_mouse_button_down (int button, v2d_t pos);
  void handle_mouse_button_up (int button, v2d_t pos);

  void create_world_for_game (void);
};



#endif // _edit_h

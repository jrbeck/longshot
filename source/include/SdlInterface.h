// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Sdl Interface
// *
// * SDL utility library
// * used to create window, set GL state, etc...
// *
// * needs major work - should be eliminated in favor of a more abstract interface to
// * openGL/directX/Win32
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SdlInterface_h_
#define SdlInterface_h_


//#include "GL/glaux.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
//#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "MathUtil.h"
#include "v3d.h"
#include "v2d.h"



// TYPEDEFS * * * * * * * * * * * * * * * * * * * * * * * *
typedef struct {
	GLfloat r;
	GLfloat g;
	GLfloat b;
} rgb_float_t;


typedef struct {
	int initialized;
	int screen_w;
	int screen_h;
	bool fullscreen;

} sdl_mode_info_t;


// GLOBALS * * * * * * * * * * * * * * * * * * * * * * * *
extern sdl_mode_info_t desktopMode;
extern sdl_mode_info_t currentMode;
//TTF_Font *sdlu_font;




class SdlInterface {
};


int initSdl (void);
int deinitSdl (void);
int setVideoMode (sdl_mode_info_t mode);
void drawProgressBar (v2d_t lower, v2d_t higher, int numerator, int denominator, rgb_float_t c1, rgb_float_t c2);
void drawProgressBar (v2d_t lower, v2d_t higher, double percent, rgb_float_t c1, rgb_float_t c2);
int loadTexture (const char *filename, GLuint *texture_handle);
//AUX_RGBImageRec *loadBmp (const char *fileName);
//int loadGLTexture (const char *fileName, GLuint *textureHandle);


#endif // SdlInterface_h_

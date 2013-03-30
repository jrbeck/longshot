#include "SdlInterface.h"


sdl_mode_info_t desktopMode;
sdl_mode_info_t currentMode;



int initSdl (void) {
	if (SDL_Init (SDL_INIT_EVERYTHING) != 0) {
		printf ("Unable to initialize SDL: %s\n", SDL_GetError ());
		return 1;
	}

	const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo ();

	if (videoInfo != NULL) {
		printf ("desktop resolution: %d, %d\n", videoInfo->current_w, videoInfo->current_h);
		desktopMode.screen_w = videoInfo->current_w;
		desktopMode.screen_h = videoInfo->current_h;
		desktopMode.fullscreen = true;
	}

	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

	return 0;
}



int setVideoMode (sdl_mode_info_t mode) {
	SDL_Surface *screen;

	if (mode.fullscreen) {
		screen = SDL_SetVideoMode (mode.screen_w, mode.screen_h, 32, SDL_OPENGL | SDL_FULLSCREEN);
	}
	else {
		screen = SDL_SetVideoMode (mode.screen_w, mode.screen_h, 32, SDL_OPENGL);
	}
	
	if (screen == NULL) {
		printf ("Unable to set video mode: %s\n", SDL_GetError ());
		return 1;
	}

	currentMode = mode;

	// no need for this anymore
	SDL_FreeSurface (screen);

	return 0;
}



int deinitSdl (void) {
	SDL_Quit ();

	return 0;
}



void drawProgressBar (
	v2d_t lower,
	v2d_t higher,
	int numerator,
	int denominator,
	rgb_float_t c1,
	rgb_float_t c2)
{
	double percent = static_cast<double>(numerator) / static_cast<double>(denominator);
	drawProgressBar (lower, higher, percent, c1, c2);
}



void drawProgressBar (v2d_t lower, v2d_t higher, double percent, rgb_float_t c1, rgb_float_t c2) {
	double midx; // middle top and bottom

	midx = lower.x + ((higher.x - lower.x) * percent);

	glColor3f (c1.r, c1.g, c1.b);

	glBegin (GL_QUADS);
		glVertex2d (lower.x, lower.y);
		glVertex2d (lower.x, higher.y);
		glVertex2d (midx, higher.y);
		glVertex2d (midx, lower.y);

		glColor3f (c2.r, c2.g, c2.b);

		glVertex2d (midx, lower.y);
		glVertex2d (midx, higher.y);
		glVertex2d (higher.x, higher.y);
		glVertex2d (higher.x, lower.y);
	glEnd ();
}

/*

{

	SDL_Surface *surface;
	GLenum texture_format;
	GLint  nOfColors;

	if ((surface = IMG_Load("bag.png"))) { 

		// Check that the image's width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) {
			printf("warning: bag.png's width is not a power of 2\n");
		}

		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) {
			printf("warning: bag.png's height is not a power of 2\n");
		}

		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4) { // contains an alpha channel
			if (surface->format->Rmask == 0x000000ff) {
				texture_format = GL_RGBA;
			}
			//else
				//texture_format = GL_BGRA;
		}
		else if (nOfColors == 3) { // no alpha channel
			if (surface->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
			}
			//else
				//texture_format = GL_BGR;
		}
		else {
			printf("warning: the image is not truecolor..  this will probably break\n");
			// this error should not go unhandled
		}

		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );

		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );

		// Set the texture's stretching properties
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
						  texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	} 
	else {
		printf("SDL could not load bag.png: %s\n", SDL_GetError());
		SDL_Quit();
		//return 1;
	}    

	// Free the SDL_Surface only if it was successfully created
	if ( surface ) { 
		SDL_FreeSurface( surface );
	}



}



*/




int loadTexture(const char *filename, GLuint *texture_handle) {
	SDL_Surface *surface; // Gives us the information to make the texture

	if ((surface = IMG_Load(filename))) { 
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
			printf("warning: the image is not truecolor..  this will probably break\n");
			// this error should not go unhandled
		}



//		surfaceAlpha = SDL_DisplayFormat(surface);
//		SDL_SetAlpha(surfaceAlpha, 0, SDL_ALPHA_TRANSPARENT);

/*		tempSurfaceFixed = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			tempSurface->w, tempSurface->h,
			32, R_Mask, G_Mask, B_Mask, A_Mask);
*/

		// Have OpenGL generate a texture object handle for us
		glGenTextures (1, texture_handle);

		// Bind the texture object
		glBindTexture (GL_TEXTURE_2D, *texture_handle);

		// Set the texture's stretching properties
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//		surfaceAlpha = SDL_DisplayFormatAlpha(surface);

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			numColors,
			surface->w, surface->h,
			0,
			textureFormat,
			GL_UNSIGNED_BYTE,
			surface->pixels);
	}
	else {
//        printf ("SDL could not load image.bmp: %s\n", SDL_GetError());
//        SDL_Quit ();
		return -1;
	}

	// Free the SDL_Surface only if it was successfully created
	if (surface) { 
		SDL_FreeSurface (surface);
	}

	// return success
	return 0;
}


/*
// THESE ARE TO LOAD WITH GLAUX - if you're into that...
AUX_RGBImageRec *loadBmp (const char *fileName) {
	if (fileName == NULL) {
		return NULL;
	}

	// we'll use this trick to make sure file exists
	FILE *file = fopen (fileName,"r");
	if (file != NULL) {
		fclose (file);
		return auxDIBImageLoadA (fileName);
	}

	return NULL;
}



int loadGLTexture (const char *fileName, GLuint *textureHandle) {
	AUX_RGBImageRec *imageRec = loadBmp (fileName);
	if (imageRec == NULL) {
		// couldn't load the texture
		return -1;
	}

	glGenTextures (1, textureHandle);
	if (*textureHandle == 0) {
		// failed to get a texture handle
		return -2;
	}

	glBindTexture (GL_TEXTURE_2D, *textureHandle);
	glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, 3,
		imageRec->sizeX, imageRec->sizeY,
		0, GL_RGB, GL_UNSIGNED_BYTE, imageRec->data);

	if (imageRec->data != NULL) {
		// free the data first
		free (imageRec->data);
	}

	// now for the AUX_RGBImageRec
	free (imageRec);

	return 0;
}
*/


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * SpriteBatcher
// *
// * desc: collects a bunch of sprite draw requests and...draws them!
// *
// * this is the openGl 1.0 version. as such, it uses nasty things like immediate mode
// * rendering of GL_QUADS. this is very old-skool and needs to be brought into this
// * millenium.
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SpriteBatcher_h_
#define SpriteBatcher_h_

#include "TextureRegion.h"

class Sprite {
public:
	Sprite() { }
	~Sprite() { }

	GLfloat mVert1[2];
	GLfloat mVert2[2];
	GLfloat mVert3[2];
	GLfloat mVert4[2];

	TextureRegion *mTextureRegion;
};



class SpriteBatcher {
public:
	Sprite *mSprites;
	size_t mMaxSprites;
	size_t mNumSprites;

	SpriteBatcher(size_t maxSprites);
	~SpriteBatcher();

	void beginBatch(Texture *texture);
	void endBatch();

	void drawSprite(GLfloat x, GLfloat y, GLfloat width, GLfloat height, TextureRegion *textureRegion);
	// this expects an angle in degrees
	void drawSprite(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat angle, TextureRegion *textureRegion);
};


#endif // SpriteBatcher_h_

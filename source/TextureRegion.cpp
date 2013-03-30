#include "TextureRegion.h"


TextureRegion::TextureRegion() {
}


TextureRegion::TextureRegion(GLuint x, GLuint y, GLuint width, GLuint height, Texture *texture) {
	setupRegion(x, y, width, height, texture);
}


TextureRegion::~TextureRegion() {
	printf("TextureRegion::~TextureRegion()\n");
}


void TextureRegion::setupRegion(GLuint x, GLuint y, GLuint width, GLuint height, Texture *texture) {
	mX = x;
	mY = y;
	mW = width;
	mH = height;

	u1 = (GLfloat)mX / (GLfloat)texture->mWidth;
	v1 = (GLfloat)mY / (GLfloat)texture->mHeight;
	u2 = u1 + ((GLfloat)mW / (GLfloat)texture->mWidth);
	v2 = v1 + ((GLfloat)mH / (GLfloat)texture->mHeight);

	mTexture = texture;
}


void TextureRegion::getFlippedCoords(bool flipX, bool flipY, GLfloat coords[4]) {
	if (flipX) {
		coords[0] = u2;
		coords[2] = u1;
	}
	else {
		coords[0] = u1;
		coords[2] = u2;
	}

	if (flipY) {
		coords[1] = v2;
		coords[3] = v1;
	}
	else {
		coords[1] = v1;
		coords[3] = v2;
	}
}

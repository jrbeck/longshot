#include "BitmapModel.h"



BitmapModel::BitmapModel(TextureRegion* region) {
	mTexture = region->mTexture;

	mWidth = region->mW;
	mHeight = region->mH;

	GLuint textureHandle = region->mTexture->mGlHandle;
	GLint textureWidth, textureHeight;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight);

	Pixel32 *pixelData = new Pixel32[textureWidth * textureHeight];

	glGetTexImage(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		GL_UNSIGNED_INT_8_8_8_8,
		pixelData);


	// let's generate a display list
	mDisplayListHandle = glGenLists(1);
	glNewList(mDisplayListHandle, GL_COMPILE);

	// these are the normalized pixel width/height
	GLfloat pixelW = 1.0f / (GLfloat)textureWidth;
	GLfloat pixelH = 1.0f / (GLfloat)textureHeight;

	// draw the top, bottom, right, and left sides of the voxels
	for (int t = region->mY; t < (region->mY + region->mH); t++) {
		for (int s = region->mX; s < (region->mX + region->mW); s++) {

			int faceVis = getFaceVisibility(s, t, pixelData, region);

			if (faceVis != 0) {

				GLfloat u = region->u1 + ((s - region->mX) * pixelW);
				GLfloat v = region->v1 + ((t - region->mY) * pixelH);

				drawVoxel(faceVis, s - region->mX, t - region->mY, u, v, pixelW, pixelH);
			}
		}
	}

	// draw a single textured quad for the front and back
	drawFront(0, 0, region->mW, region->mH, region->u1, region->v1, region->u2, region->v2);
	drawBack(0, 0, region->mW, region->mH, region->u1, region->v1, region->u2, region->v2);

	// all done with the display list
	glEndList();

	// free those pixels...
	delete [] pixelData;
}

BitmapModel::~BitmapModel() {
	glDeleteLists(mDisplayListHandle, 1);
}

void BitmapModel::bindTexture() {
//	if (mTexture) {
		mTexture->bind();
//	}
}

void BitmapModel::draw() {
	glBegin(GL_QUADS);
		glCallList(mDisplayListHandle);
	glEnd();
}

BYTE BitmapModel::getFaceVisibility(
	GLuint s, GLuint t,
	Pixel32* pixelData,
	TextureRegion* region) const
{

	// check the current pixel
	if (pixelData[s + (t * mTexture->mWidth)].a == 0) {
		// alpha == 0 -> don't draw a voxel
		return 0;
	}

	// front and back will always be drawn
	BYTE visibility = 0;
	visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_FRO];
	visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_BAC];

	// check the neighbors
	if (t == region->mY || pixelData[s + ((t - 1) * mTexture->mWidth)].a == 0) {
		visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_TOP];
	}

	if (t == (region->mY + region->mH - 1) ||	pixelData[s + ((t + 1) * mTexture->mWidth)].a == 0)	{
		visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_BOT];
	}

	if (s == region->mX || pixelData[(s - 1) + (t * mTexture->mWidth)].a == 0) {
		visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_LEF];
	}

	if (s == (region->mX + region->mW - 1) || pixelData[(s + 1) + (t * mTexture->mWidth)].a == 0)	{
		visibility |= gBlockSideBitmaskLookup[BLOCK_SIDE_RIG];
	}

	return visibility;
}

void BitmapModel::drawVoxel(
	BYTE faceVisibility,
	GLuint s, GLuint t,
	GLfloat u, GLfloat v,
	GLfloat pixelW, GLfloat pixelH) const
{
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_FRO]) {
//		drawBlockFaceFront(s, t, u, v, u + pixelW, v + pixelH);
	}
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BAC]) {
//		drawBlockFaceBack(s, t, u, v, u + pixelW, v + pixelH);
	}
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_LEF]) {
		drawBlockFaceLeft(s, t, u, v, u + pixelW, v + pixelH);
	}
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_RIG]) {
		drawBlockFaceRight(s, t, u, v, u + pixelW, v + pixelH);
	}
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_TOP]) {
		drawBlockFaceTop(s, t, u, v, u + pixelW, v + pixelH);
	}
	if (faceVisibility & gBlockSideBitmaskLookup[BLOCK_SIDE_BOT]) {
		drawBlockFaceBottom(s, t, u, v, u + pixelW, v + pixelH);
	}
}

void BitmapModel::drawFront(
	GLfloat s1, GLfloat t1,
	GLfloat s2, GLfloat t2,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
//	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		s2 * VOXEL_SCALE_X,
		t2 * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v2);
	glVertex3f(
		s1 * VOXEL_SCALE_X,
		t2 * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s1 * VOXEL_SCALE_X,
		t1 * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
	
	glTexCoord2f(u2, v1);
	glVertex3f(
		s2 * VOXEL_SCALE_X,
		t1 * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
}

void BitmapModel::drawBack(
	GLfloat s1, GLfloat t1,
	GLfloat s2, GLfloat t2,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
//	glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		s2 * VOXEL_SCALE_X,
		t2 * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		s2 * VOXEL_SCALE_X,
		t1 * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s1 * VOXEL_SCALE_X,
		t1 * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
	
	glTexCoord2f(u1, v2);
	glVertex3f(
		s1 * VOXEL_SCALE_X,
		t2 * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceFront (
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
//	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_FRO]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
	
	glTexCoord2f(u2, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceBack(
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
//	glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_BAC]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
	
	glTexCoord2f(u1, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceLeft(
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
	glNormal3iv(gCubeFaceNormalLookup[BLOCK_SIDE_LEF]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceRight(
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_RIG]);

	glTexCoord2f(u2, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceTop(
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_TOP]);

	glTexCoord2f(u1, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		t * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);
}


void BitmapModel::drawBlockFaceBottom(
	GLfloat s, GLfloat t,
	GLfloat u1, GLfloat v1,
	GLfloat u2, GLfloat v2) const
{
	glNormal3iv (gCubeFaceNormalLookup[BLOCK_SIDE_BOT]);

	glTexCoord2f(u1, v2);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v2);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		-0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u2, v1);
	glVertex3f(
		(s + 1) * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);

	glTexCoord2f(u1, v1);
	glVertex3f(
		s * VOXEL_SCALE_X,
		(t + 1) * VOXEL_SCALE_Y,
		0.5f * VOXEL_SCALE_Z);
}

#include "CloudSim.h"



CloudSim::CloudSim (void) {
	mCloudDisplayListHandle = 0;

	//	GLfloat intensities[4] = {0.6f, 0.65f, 0.7f, 1.0};
	mLightIntensities[0] = 0.2f;
	mLightIntensities[1] = 0.3f;
	mLightIntensities[2] = 0.3f;
	mLightIntensities[3] = 1.0f;

	mPosition = v3d_zero ();
}



CloudSim::~CloudSim (void) {
	if (mCloudDisplayListHandle != 0) {
		glDeleteLists (mCloudDisplayListHandle, 1);
	}
}


void CloudSim::clear(void) {
	mTerrain.set_all(0.0);
	redrawDisplayLists(v3d_v(0.0, 0.0, 0.0));
}



void CloudSim::newClouds (v3d_t position) {
	mTerrain.resize(SIDE_WIDTH);
	mTerrain.generateTilable(5.0, mPrng);
	mTerrain.normalize(0.0, 1.0);

	mPosition = position;

	redrawDisplayLists(position);
}



void CloudSim::redrawDisplayLists (v3d_t position) {
	if (mCloudDisplayListHandle != 0) {
		glDeleteLists (mCloudDisplayListHandle, 1);
	}

	// generate the display list for the shaded block
	mCloudDisplayListHandle = glGenLists (1);
	glNewList (mCloudDisplayListHandle, GL_COMPILE);
		drawForDisplayList (position);
	glEndList();
}



void CloudSim::update (GLfloat starAlpha) {
	GLfloat dayIntensities[4] = {0.6f, 0.65f, 0.7f, 1.0f};
	GLfloat nightIntensities[4] = {0.09f, 0.16f, 0.24f, 0.2f};

	// get starAlpha between 0.0 and 1.0
	starAlpha += 1.0;
	starAlpha *= 0.5;

	mLightIntensities[0] = static_cast<GLfloat>(lerp (dayIntensities[0], nightIntensities[0], starAlpha));
	mLightIntensities[1] = static_cast<GLfloat>(lerp (dayIntensities[1], nightIntensities[1], starAlpha));
	mLightIntensities[2] = static_cast<GLfloat>(lerp (dayIntensities[2], nightIntensities[2], starAlpha));
	mLightIntensities[3] = static_cast<GLfloat>(lerp (dayIntensities[3], nightIntensities[3], starAlpha));

	redrawDisplayLists (mPosition);

}


void CloudSim::drawForDisplayList (v3d_t position) {
	glPushMatrix ();

	glTranslated (position.x, 0.0, position.z);
	glScaled (BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

	glBegin (GL_QUADS);

	for (int j = 0; j < SIDE_WIDTH; j++) {
		for (int i = 0; i < SIDE_WIDTH; i++) {
			double terrainValue = mTerrain.getValueBilerp (i, j) - 0.5;
			if (terrainValue > 0.15) {
				GLfloat alpha = static_cast<GLfloat>(terrainValue);

				v3di_t iPosition = v3di_v (i, 5, j);
			
				drawCloudBlock (iPosition, alpha);
			}
		}
	}

	glEnd ();

	glPopMatrix ();


}



void CloudSim::draw (gl_camera_c &cam) {
	glEnable (GL_BLEND);
	glDisable (GL_CULL_FACE);

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			// FIXME: implement bounding box optimization
			// clip the entire region against the frustum if possible
//			if (glCamera.bounding_box_test (mBoundingBox) == FRUSTUM_OUTSIDE) {
//				return 0;
//			}

			glPushMatrix ();
				glTranslated (SIDE_WIDTH * BLOCK_SIZE * i, 0.0, SIDE_WIDTH * BLOCK_SIZE * j);
				glCallList (mCloudDisplayListHandle);
			glPopMatrix ();
		}
	}

	glEnable (GL_CULL_FACE);
}



void CloudSim::drawCloudBlock (v3di_t position, GLfloat alpha) {
	GLfloat npos[3], fpos[3];

	npos[0] = static_cast<GLfloat>(position.x);
	npos[1] = static_cast<GLfloat>(position.y);
	npos[2] = static_cast<GLfloat>(position.z);

	fpos[0] = static_cast<GLfloat>(position.x) + 1;
	fpos[1] = static_cast<GLfloat>(position.y) + 1;
	fpos[2] = static_cast<GLfloat>(position.z) + 1;
	
	GLfloat oldAlpha = mLightIntensities[3];

	mLightIntensities[3] *= alpha;

	glColor4fv (mLightIntensities);

	// left
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_LEF],
//		mGlobalLightIntensities[BLOCK_SIDE_LEF]);

//	glNormal3d (-1.0, 0.0, 0.0);

	glVertex3fv (npos);	// LBB
	glVertex3f (npos[0], npos[1], fpos[2]);	// LBF
	glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF
	glVertex3f (npos[0], fpos[1], npos[2]);	// LTB

	// right
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_RIG],
//		mGlobalLightIntensities[BLOCK_SIDE_RIG]);

//	glNormal3d (1.0, 0.0, 0.0);

	glVertex3f (fpos[0], npos[1], npos[2]);	// RBB
	glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB
	glVertex3fv (fpos);	// RTF
	glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF

	// top
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_TOP],
//		mGlobalLightIntensities[BLOCK_SIDE_TOP]);

//	glNormal3d (0.0, 1.0, 0.0);

	glVertex3f (npos[0], fpos[1], npos[2]);	// LTB
	glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF
	glVertex3fv (fpos);	// RTF
	glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB

	// bottom
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_BOT],
//		mGlobalLightIntensities[BLOCK_SIDE_BOT]);

//	glNormal3d (0.0, -1.0, 0.0);

	glVertex3fv (npos);	// LBB
	glVertex3f (fpos[0], npos[1], npos[2]);	// RBB
	glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF
	glVertex3f (npos[0], npos[1], fpos[2]);	// LBF

	// front
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_FRO],
//		mGlobalLightIntensities[BLOCK_SIDE_FRO]);

//	glNormal3d (0.0, 0.0, 1.0);

	glVertex3f (npos[0], npos[1], fpos[2]);	// LBF
	glVertex3f (fpos[0], npos[1], fpos[2]);	// RBF
	glVertex3fv (fpos);	// RTF
	glVertex3f (npos[0], fpos[1], fpos[2]);	// LTF

	// back
//	intensities = v3d_add (block.faceLighting[BLOCK_SIDE_BAC],
//		mGlobalLightIntensities[BLOCK_SIDE_BAC]);

//	glNormal3d (0.0, 0.0, -1.0);

	glVertex3f (fpos[0], npos[1], npos[2]);	// RBB
	glVertex3fv (npos);	// LBB
	glVertex3f (npos[0], fpos[1], npos[2]);	// LTB
	glVertex3f (fpos[0], fpos[1], npos[2]);	// RTB

	mLightIntensities[3] = oldAlpha;
}


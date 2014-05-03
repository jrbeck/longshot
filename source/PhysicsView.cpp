#include "PhysicsView.h"


PhysicsView::PhysicsView() {
	for (int i = 0; i < NUM_OBJSKINS; i++) {
		mTextureHandles[i] = 0;
	}

	loadAssets();
}


PhysicsView::~PhysicsView() {
	freeAssets ();
}




void PhysicsView::loadAssets (void) {
	// load up the default, untextured block
	mBlankBlockCallListHandle = glGenLists(1);
	glNewList (mBlankBlockCallListHandle, GL_COMPILE);
		AssetManager::drawBlankBlock();
	glEndList();

	// load the skins... methinks ItemManager should handle at least some of this
	mTextureHandles[OBJSKIN_ITEM] = AssetManager::loadImg("art/32_obj_item.bmp");
	mTextureHandles[OBJSKIN_AMMO_BULLET] = AssetManager::loadImg("art/32_obj_ammo_bullets.bmp");
	mTextureHandles[OBJSKIN_AMMO_SLIME] = AssetManager::loadImg("art/32_obj_ammo_slime.bmp");
	mTextureHandles[OBJSKIN_AMMO_PLASMA] = AssetManager::loadImg("art/32_obj_ammo_plasma.bmp");
	mTextureHandles[OBJSKIN_AMMO_NAPALM] = AssetManager::loadImg("art/32_obj_ammo_napalm.bmp");
	mTextureHandles[OBJSKIN_HEALTHPACK] = AssetManager::loadImg("art/32_obj_healthpack.bmp");
	mTextureHandles[OBJSKIN_GRENADE] = AssetManager::loadImg("art/32_obj_grenade.bmp");
}




void PhysicsView::freeAssets (void) {
	if (mBlankBlockCallListHandle != 0) {
		glDeleteLists (mBlankBlockCallListHandle, 1);
		mBlankBlockCallListHandle = 0;
	}

	for (int i = 0; i < NUM_OBJSKINS; i++) {
		if (mTextureHandles[i] > 0) {
			glDeleteTextures (1, &mTextureHandles[i]);
			mTextureHandles[i] = 0;
		}
	}
}


void PhysicsView::setViewPosition(v3d_t pos) {
	mViewPosition = pos;
}


void PhysicsView::update(vector<phys_entity_t> *physicsEntities, double time) {
	size_t numEntities = physicsEntities->size();
	for (size_t i = 0; i < numEntities; i++) {
		setEntityColor((*physicsEntities)[i], time);
	}
}



void PhysicsView::setEntityColor(phys_entity_t &entity, double time) {
	float age = 1.0f - static_cast<GLfloat>((entity.expirationTime - time) / entity.lifespan);
	if (age > 1.0f) {
		age = 1.0f;
	}
	else if (age < 0.0f) {
		age = 0.0f;
	}

	switch (entity.type) {
		// these ones never change color or transparency
		default:
			break;

		// only the transparency changes for these fellers
		case OBJTYPE_STEAM:
			entity.color[3] = 0.4f - (age * 0.4f);
			break;	
	
		case OBJTYPE_SLIME:
		case OBJTYPE_PLASMA:
		case OBJTYPE_PLASMA_BOMB:
		case OBJTYPE_PLASMA_SPARK:
		case OBJTYPE_BLOOD_SPRAY:
			entity.color[3] = 1.0f - age;
			break;

		case OBJTYPE_SMOKE:
			// FIXME: HACK... do this right please!
//			entity.color[3] = (1.0f - age) * mEntityTypeInfo[OBJTYPE_SMOKE].color[3];
			entity.color[3] = (1.0f - age) * 0.25f;
			break;


		case OBJTYPE_PLASMA_TRAIL:
			entity.color[0] = entity.baseColor[0] - age;
			entity.color[1] = entity.baseColor[1] * age;
			entity.color[2] = entity.baseColor[2] + age;
			entity.color[3] = (1.0f - age) + 0.3f;
			break;

		case OBJTYPE_EXPLOSION:
		case OBJTYPE_NAPALM:
			entity.color[3] = (1.0f - age) * 0.65f;
			break;

		case OBJTYPE_SPARK:
		case OBJTYPE_DEAD_BULLET:
		case OBJTYPE_LIVE_BULLET:
			entity.color[1] = 0.6f * (1.0f - age) + 0.1f;
			break;

		case OBJTYPE_FIRE:
			entity.color[1] = ((1.0f - age) * 0.50f) + 0.25f;
			break;
	}
}





void PhysicsView::drawSolidEntities (vector<phys_entity_t> *physicsEntities, WorldMap &worldMap, AssetManager &assetManager) {
//	glDisable (GL_TEXTURE_2D);
//	glEnable (GL_COLOR_MATERIAL);

	glEnable (GL_TEXTURE_2D);

	int type;

	size_t numEntities = physicsEntities->size();
	for (size_t index = 0; index < numEntities; index++) {
		type = (*physicsEntities)[index].type;
		if ((*physicsEntities)[index].active && (*physicsEntities)[index].color[3] >= 1.0) {
			// ignores players and AI
			if (type == OBJTYPE_PLAYER ||
				type == OBJTYPE_AI_ENTITY)
			{
				continue;
			}
			else if (type == OBJTYPE_ITEM ||
				type == OBJTYPE_GRENADE)
			{
				// FIXME: items should be drawn by ItemManagerView
				drawTextured ((*physicsEntities)[index], worldMap, assetManager);
			}
			else {
				glDisable (GL_TEXTURE_2D);
					drawEntity ((*physicsEntities)[index]);
				glEnable (GL_TEXTURE_2D);

				// BITMAP MODEL
				//glPushMatrix();
				//v3d_t nc = (*physicsEntities)[index].boundingBox.mNearCorner;
				//v3d_t fc = (*physicsEntities)[index].boundingBox.mFarCorner;
				//v3d_t dim = (*physicsEntities)[index].boundingBox.mDimensions;
				//glTranslated (nc.x, nc.y, nc.z);
				//glScaled (dim.x * 15.0, dim.y * 15.0, dim.z * 15.0);
				//glColor3f(1.0f, 1.0f, 1.0f);
				//assetManager.mGunBitmapModel->bindTexture();
				//assetManager.mGunBitmapModel->draw();
				//glPopMatrix();

				// BILLBOARD
//				drawBillBoard((*physicsEntities)[index].boundingBox.getCenterPosition());
			}
		}
	}

//	glDisable (GL_COLOR_MATERIAL);
//	glEnable (GL_TEXTURE_2D);
	glColor3f (1.0, 1.0, 1.0);
}



void PhysicsView::drawTransparentEntities (vector<phys_entity_t> *physicsEntities, AssetManager &assetManager, bool inWater) {
	//glBindTexture (GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	size_t numEntities = physicsEntities->size();
	for (size_t index = 0; index < numEntities; index++) {
		if ((*physicsEntities)[index].active &&
			((*physicsEntities)[index].worldViscosity > 0.0) == inWater &&
			(*physicsEntities)[index].color[3] < 1.0f)
		{
			drawEntity((*physicsEntities)[index]);
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f);
}



void PhysicsView::drawEntity (const phys_entity_t &entity) {
	// FIXME: this seems like a wasteful way of doing this
	// it used to be that the box drawn was 0,0,0 -> 1,1,1, now
	// its a unit cube centered at 0,0,0. i was lazy when i
	// fixed it, but it works...
	v3d_t nc = entity.boundingBox.mNearCorner;
	v3d_t fc = entity.boundingBox.mFarCorner;
	v3d_t dim = entity.boundingBox.mDimensions;



	glPushMatrix ();
		glTranslated (nc.x, nc.y, nc.z);
		glScaled (dim.x, dim.y, dim.z);
		glColor4fv (entity.color);

		glBegin (GL_QUADS);
			glCallList(mBlankBlockCallListHandle);
		glEnd ();


	// draw displacement vector
//		if (!obj[index].queued) {
//			glColor3f (1.0f, 1.0f, 1.0f);
//			glBegin (GL_LINES);
//				glVertex3d (0.0, 0.0, 0.0);
//				glVertex3d (-obj[index].displacement.x,
//							-obj[index].displacement.y,
//							-obj[index].displacement.z);
//			glEnd ();
//		}

	glPopMatrix ();
}




void PhysicsView::drawTextured (const phys_entity_t &entity, WorldMap &worldMap, AssetManager &assetManager) {

	if (entity.type == OBJTYPE_AI_ENTITY ||
		entity.type == OBJTYPE_PLAYER)
	{
		// we don't draw these here...
		return;
	}

	static double angle = 0.0;

	angle += 0.2;
	if (angle > 360.0) {
		angle -= 360.0;
	}


	v3d_t nearCorner = entity.boundingBox.mNearCorner;
	v3d_t farCorner = entity.boundingBox.mFarCorner;
	v3d_t center = entity.boundingBox.getCenterPosition();
	v3d_t dimensions = entity.boundingBox.mDimensions;

	glPushMatrix ();

	if (entity.type == OBJTYPE_ITEM) {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if (entity.type2 == ITEMTYPE_GUN_ONE_HANDED) {
			glTranslated(center.x, center.y, center.z);
			glScaled(5.0 * dimensions.x, 5.0 * dimensions.y, 5.0 * dimensions.z);
			glRotated(angle, 0.0, 1.0, 0.0);
			glTranslated((double)assetManager.mGunBitmapModel->mWidth * -0.5 * VOXEL_SCALE_X,
				(double)assetManager.mGunBitmapModel->mHeight * -0.5 * VOXEL_SCALE_Y,
				0.0);
			
			assetManager.mGunBitmapModel->bindTexture();
			assetManager.mGunBitmapModel->draw();
		}
		else {
			glTranslated(center.x, center.y, center.z);
			glScaled(dimensions.x, dimensions.y, dimensions.z);
		
			switch (entity.type2) {
			case ITEMTYPE_HEALTHPACK:
				glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_HEALTHPACK]);
				drawLitBox (nearCorner, farCorner, worldMap, assetManager);
				break;

			case ITEMTYPE_AMMO_BULLET:
				glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_AMMO_BULLET]);
				drawLitBox (nearCorner, farCorner, worldMap, assetManager);
				break;

			case ITEMTYPE_AMMO_SLIME:
				glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_AMMO_SLIME]);
				drawLitBox (nearCorner, farCorner, worldMap, assetManager);
				break;

			case ITEMTYPE_AMMO_PLASMA:
				glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_AMMO_PLASMA]);
				drawLitBox (nearCorner, farCorner, worldMap, assetManager);
				break;

			case ITEMTYPE_AMMO_NAPALM:
				glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_AMMO_NAPALM]);
				drawLitBox (nearCorner, farCorner, worldMap, assetManager);
				break;

			default:
				glBindTexture(GL_TEXTURE_2D, mTextureHandles[OBJSKIN_ITEM]);
				drawLitBox(nearCorner, farCorner, worldMap, assetManager);
				break;
			}
		}
	}
	else if (entity.type == OBJTYPE_GRENADE) {
		glTranslated(center.x, center.y, center.z);
		glScaled(dimensions.x, dimensions.y, dimensions.z);

		glEnable (GL_COLOR_MATERIAL);
		glBindTexture (GL_TEXTURE_2D, mTextureHandles[OBJSKIN_GRENADE]);
		//glColor4fv (obj[index].color);
		glColor4f (1.0f, 0.0f, 0.0f, 1.0f);
		drawLitBox (nearCorner, farCorner, worldMap, assetManager);
		glDisable (GL_COLOR_MATERIAL);
	}
	else {
		// this is an undefined situation
		glTranslated(center.x, center.y, center.z);
		glScaled(dimensions.x, dimensions.y, dimensions.z);

		glBindTexture(GL_TEXTURE_2D, 0);
		drawLitBox(nearCorner, farCorner, worldMap, assetManager);
	}

	glPopMatrix ();
}



void PhysicsView::drawLitBox (v3d_t nearCorner, v3d_t farCorner, WorldMap &worldMap, AssetManager &assetManager) {
	nearCorner.x += 0.001;
	nearCorner.y += 0.001;
	nearCorner.z += 0.001;

	farCorner.x -= 0.001;
	farCorner.y -= 0.001;
	farCorner.z -= 0.001;

	v3di_t worldLights[8];

	v3d_t vertexPos;

	int lightCeiling = (int)worldMap.mWorldLightingCeiling;
	int lightFloor = (int)worldMap.mWorldLightingFloor;

	// BOX_CORNER_LBB
	vertexPos = nearCorner;

	BYTE blockType;
	BYTE blockUniqueLighting;

	block_t *block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_LBB].x = lightCeiling;
		worldLights[BOX_CORNER_LBB].y = lightCeiling;
		worldLights[BOX_CORNER_LBB].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_LBB].x = blockUniqueLighting;
		worldLights[BOX_CORNER_LBB].y = blockUniqueLighting;
		worldLights[BOX_CORNER_LBB].z = blockUniqueLighting;
	}
	else {
		// solid
		worldLights[BOX_CORNER_LBB].x = lightFloor;
		worldLights[BOX_CORNER_LBB].y = lightFloor;
		worldLights[BOX_CORNER_LBB].z = lightFloor;
	}

	// BOX_CORNER_LBF
	vertexPos.x = nearCorner.x;
	vertexPos.y = nearCorner.y;
	vertexPos.z = farCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_LBF].x = lightCeiling;
		worldLights[BOX_CORNER_LBF].y = lightCeiling;
		worldLights[BOX_CORNER_LBF].z = lightCeiling;
	}
	else if (gBlockData.get(blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_LBF].x = blockUniqueLighting;
		worldLights[BOX_CORNER_LBF].y = blockUniqueLighting;
		worldLights[BOX_CORNER_LBF].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_LBF].x = lightFloor;
		worldLights[BOX_CORNER_LBF].y = lightFloor;
		worldLights[BOX_CORNER_LBF].z = lightFloor;
	}

	// BOX_CORNER_LTB
	vertexPos.x = nearCorner.x;
	vertexPos.y = farCorner.y;
	vertexPos.z = nearCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_LTB].x = lightCeiling;
		worldLights[BOX_CORNER_LTB].y = lightCeiling;
		worldLights[BOX_CORNER_LTB].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_LTB].x = blockUniqueLighting;
		worldLights[BOX_CORNER_LTB].y = blockUniqueLighting;
		worldLights[BOX_CORNER_LTB].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_LTB].x = lightFloor;
		worldLights[BOX_CORNER_LTB].y = lightFloor;
		worldLights[BOX_CORNER_LTB].z = lightFloor;
	}


	// BOX_CORNER_LTF
	vertexPos.x = nearCorner.x;
	vertexPos.y = farCorner.y;
	vertexPos.z = farCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_LTF].x = lightCeiling;
		worldLights[BOX_CORNER_LTF].y = lightCeiling;
		worldLights[BOX_CORNER_LTF].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_LTF].x = blockUniqueLighting;
		worldLights[BOX_CORNER_LTF].y = blockUniqueLighting;
		worldLights[BOX_CORNER_LTF].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_LTF].x = lightFloor;
		worldLights[BOX_CORNER_LTF].y = lightFloor;
		worldLights[BOX_CORNER_LTF].z = lightFloor;
	}


	// BOX_CORNER_RBB
	vertexPos.x = farCorner.x;
	vertexPos.y = nearCorner.y;
	vertexPos.z = nearCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_RBB].x = lightCeiling;
		worldLights[BOX_CORNER_RBB].y = lightCeiling;
		worldLights[BOX_CORNER_RBB].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_RBB].x = blockUniqueLighting;
		worldLights[BOX_CORNER_RBB].y = blockUniqueLighting;
		worldLights[BOX_CORNER_RBB].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_RBB].x = lightFloor;
		worldLights[BOX_CORNER_RBB].y = lightFloor;
		worldLights[BOX_CORNER_RBB].z = lightFloor;
	}


	// BOX_CORNER_RBF
	vertexPos.x = farCorner.x;
	vertexPos.y = nearCorner.y;
	vertexPos.z = farCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_RBF].x = lightCeiling;
		worldLights[BOX_CORNER_RBF].y = lightCeiling;
		worldLights[BOX_CORNER_RBF].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_RBF].x = blockUniqueLighting;
		worldLights[BOX_CORNER_RBF].y = blockUniqueLighting;
		worldLights[BOX_CORNER_RBF].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_RBF].x = lightFloor;
		worldLights[BOX_CORNER_RBF].y = lightFloor;
		worldLights[BOX_CORNER_RBF].z = lightFloor;
	}


	// BOX_CORNER_RTB
	vertexPos.x = farCorner.x;
	vertexPos.y = farCorner.y;
	vertexPos.z = nearCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_RTB].x = lightCeiling;
		worldLights[BOX_CORNER_RTB].y = lightCeiling;
		worldLights[BOX_CORNER_RTB].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_RTB].x = blockUniqueLighting;
		worldLights[BOX_CORNER_RTB].y = blockUniqueLighting;
		worldLights[BOX_CORNER_RTB].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_RTB].x = lightFloor;
		worldLights[BOX_CORNER_RTB].y = lightFloor;
		worldLights[BOX_CORNER_RTB].z = lightFloor;
	}


	// BOX_CORNER_RTF
	vertexPos.x = farCorner.x;
	vertexPos.y = farCorner.y;
	vertexPos.z = farCorner.z;

	block = worldMap.getBlock (vertexPos);
	if (block == NULL) {
		blockType = BLOCK_TYPE_INVALID;
		blockUniqueLighting = 0;
	}
	else {
		blockType = block->type;
		blockUniqueLighting = block->uniqueLighting;
	}

	if (blockType == BLOCK_TYPE_INVALID) {
		worldLights[BOX_CORNER_RTF].x = lightCeiling;
		worldLights[BOX_CORNER_RTF].y = lightCeiling;
		worldLights[BOX_CORNER_RTF].z = lightCeiling;
	}
	else if (gBlockData.get (blockType)->lightAttenuation < LIGHT_ATTENUATION_OPAQUE) {
		worldLights[BOX_CORNER_RTF].x = blockUniqueLighting;
		worldLights[BOX_CORNER_RTF].y = blockUniqueLighting;
		worldLights[BOX_CORNER_RTF].z = blockUniqueLighting;
	}
	else {
		worldLights[BOX_CORNER_RTF].x = lightFloor;
		worldLights[BOX_CORNER_RTF].y = lightFloor;
		worldLights[BOX_CORNER_RTF].z = lightFloor;
	}


	glBegin (GL_QUADS);
		assetManager.drawBlock2(worldLights);
	glEnd ();
}




void PhysicsView::drawBillBoard(v3d_t pos) {

	double halfWidth = 0.1;
	double halfHeight = 0.1;

	v3d_t delta = v3d_sub(pos, mViewPosition);
	v3d_t up = { 0.0, 1.0, 0.0 };
	v3d_t right = v3d_normalize(v3d_cross(delta, up));
	up = v3d_normalize(v3d_cross(delta, right));

	glColor3d(1.0, 1.0, 1.0);

	glBegin (GL_QUADS);
		v3d_t vert = v3d_add(pos, v3d_add(v3d_scale(-halfWidth, right), v3d_scale(halfHeight, up)));
		glTexCoord2f(0.0, 1.0); glVertex3d(vert.x, vert.y, vert.z);

		vert = v3d_add(pos, v3d_add(v3d_scale(halfWidth, right), v3d_scale(halfHeight, up)));
		glTexCoord2f(1.0, 1.0); glVertex3d(vert.x, vert.y, vert.z);

		vert = v3d_add(pos, v3d_add(v3d_scale(halfWidth, right), v3d_scale(-halfHeight, up)));
		glTexCoord2f(1.0, 0.0); glVertex3d(vert.x, vert.y, vert.z);

		vert = v3d_add(pos, v3d_add(v3d_scale(-halfWidth, right), v3d_scale(-halfHeight, up)));
		glTexCoord2f(0.0, 0.0); glVertex3d(vert.x, vert.y, vert.z);
	glEnd();
}


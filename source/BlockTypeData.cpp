#include "blocktypedata.h"

// this is the instantiation
// FIXME: this can't be kosher
BlockTypeDataContainer gBlockData;

int gWaterBlockType = BLOCK_TYPE_SLUDGE;

// METHODS * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


BlockTypeDataContainer::BlockTypeDataContainer () {
	loadBlockTypeData ();
}



void BlockTypeDataContainer::loadBlockTypeData (void) {

	BlockTypeData defaultBlockTypeData;

	defaultBlockTypeData.alpha = 1.0f;
	defaultBlockTypeData.lightAttenuation = LIGHT_ATTENUATION_OPAQUE;
	defaultBlockTypeData.lightEmission = 0;
	defaultBlockTypeData.visionTint[0] = 0.0f;
	defaultBlockTypeData.visionTint[1] = 0.0f;
	defaultBlockTypeData.visionTint[2] = 0.0f;
	defaultBlockTypeData.visionTint[3] = 0.0f;

	defaultBlockTypeData.height = 1.0;
	defaultBlockTypeData.healthEffect = 0.0;
	defaultBlockTypeData.health = 100.0;
	defaultBlockTypeData.flammability = 0;

	defaultBlockTypeData.friction = 0.98;
	defaultBlockTypeData.elasticity = 500.0;
	defaultBlockTypeData.viscosity = 0.0;

	defaultBlockTypeData.climbingDifficulty = 0.0;
	defaultBlockTypeData.solidityType = BLOCK_SOLIDITY_TYPE_ROCK;
	defaultBlockTypeData.blocksVisibility = 100.0;

	// set all to the default block
	for (int i = 0; i < NUM_BLOCK_TYPES; i++) {
		mData[i] = defaultBlockTypeData;
	}

	// now handle the specifics


	mData[BLOCK_TYPE_AIR].alpha = 0.0f;
	mData[BLOCK_TYPE_AIR].lightAttenuation = LIGHT_ATTENUATION_NONE;
	mData[BLOCK_TYPE_AIR].solidityType = BLOCK_SOLIDITY_TYPE_AIR;
	mData[BLOCK_TYPE_AIR].blocksVisibility = 0.0;

	
	mData[BLOCK_TYPE_WATER].alpha = 0.85f;
	mData[BLOCK_TYPE_WATER].lightAttenuation = 8;
	mData[BLOCK_TYPE_WATER].visionTint[0] = 0.1f;
	mData[BLOCK_TYPE_WATER].visionTint[1] = 0.1f;
	mData[BLOCK_TYPE_WATER].visionTint[2] = 0.5f;
	mData[BLOCK_TYPE_WATER].visionTint[3] = 0.5f;
	mData[BLOCK_TYPE_WATER].solidityType = BLOCK_SOLIDITY_TYPE_LIQUID;
	mData[BLOCK_TYPE_WATER].blocksVisibility = 50.0;
	mData[BLOCK_TYPE_WATER].viscosity = 6.5;


	mData[BLOCK_TYPE_SLUDGE].alpha = 0.85f;
	mData[BLOCK_TYPE_SLUDGE].lightAttenuation = 16;
	mData[BLOCK_TYPE_SLUDGE].solidityType = BLOCK_SOLIDITY_TYPE_LIQUID;
	mData[BLOCK_TYPE_SLUDGE].blocksVisibility = 50.0;
	mData[BLOCK_TYPE_SLUDGE].viscosity = 15.0;

	mData[BLOCK_TYPE_GREEN_STAR_TILE].lightEmission = 32;


	// FIXME: alpha < 1.0 with BLOCK_SOLIDITY_TYPE_LIQUID causes
	// no ground to the liquid
	mData[BLOCK_TYPE_LAVA].alpha = 0.85f;
	mData[BLOCK_TYPE_LAVA].lightEmission = 12;
	mData[BLOCK_TYPE_LAVA].visionTint[0] = 0.6f;
	mData[BLOCK_TYPE_LAVA].visionTint[1] = 0.4f;
	mData[BLOCK_TYPE_LAVA].visionTint[2] = 0.0f;
	mData[BLOCK_TYPE_LAVA].visionTint[3] = 0.5f;
	mData[BLOCK_TYPE_LAVA].healthEffect = -0.1;
	mData[BLOCK_TYPE_LAVA].solidityType = BLOCK_SOLIDITY_TYPE_LIQUID;
	mData[BLOCK_TYPE_LAVA].viscosity = 12.0;


	mData[BLOCK_TYPE_BLOOD].alpha = 0.85f;
	mData[BLOCK_TYPE_BLOOD].lightAttenuation = 6;
	mData[BLOCK_TYPE_BLOOD].lightEmission = 10;
	mData[BLOCK_TYPE_BLOOD].visionTint[0] = 0.6f;
	mData[BLOCK_TYPE_BLOOD].visionTint[1] = 0.0f;
	mData[BLOCK_TYPE_BLOOD].visionTint[2] = 0.0f;
	mData[BLOCK_TYPE_BLOOD].visionTint[3] = 0.5f;
	mData[BLOCK_TYPE_BLOOD].solidityType = BLOCK_SOLIDITY_TYPE_LIQUID;
	mData[BLOCK_TYPE_BLOOD].viscosity = 12.0;


//	BLOCK_TYPE_GLASS
	mData[BLOCK_TYPE_GLASS].solidityType = BLOCK_SOLIDITY_TYPE_GLASS;


	mData[BLOCK_TYPE_FLOWER].lightAttenuation = 6;
	mData[BLOCK_TYPE_FLOWER].solidityType = BLOCK_SOLIDITY_TYPE_PLANT;
	mData[BLOCK_TYPE_FLOWER].blocksVisibility = 0.0;
}



BlockTypeData *BlockTypeDataContainer::get(int type) {
	if (type < 0 || type >= NUM_BLOCK_TYPES) {
		return &mData[BLOCK_TYPE_AIR];
	}

	return &mData[type];
}



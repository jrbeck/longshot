#include "Constants.h"




int SCREEN_W;
int SCREEN_H;
bool FULLSCREEN_MODE	=	(false);

GLuint gDefaultFontTextureHandle = 0;


BYTE gBlockSideBitmaskLookup[NUM_BLOCK_SIDES] = {
	BIT_MASK_1,
	BIT_MASK_2,
	BIT_MASK_3,
	BIT_MASK_4,
	BIT_MASK_5,
	BIT_MASK_6
};



GLfloat cube_corner[NUM_BOX_CORNERS][3] = {
			{0.0, 0.0, 0.0},	// LBB
			{0.0, 0.0, 1.0},	// LBF
			{0.0, 1.0, 0.0},	// LTB
			{0.0, 1.0, 1.0},	// LTF
			{1.0, 0.0, 0.0}, 	// RBB
			{1.0, 0.0, 1.0},	// RBF
			{1.0, 1.0, 0.0},	// RTB
			{1.0, 1.0, 1.0} };	// RTF


GLfloat cube_corner_centered[NUM_BOX_CORNERS][3] = {
			{ -0.5, -0.5, -0.5 },	// LBB
			{ -0.5, -0.5, +0.5 },	// LBF
			{ -0.5, +0.5, -0.5 },	// LTB
			{ -0.5, +0.5, +0.5 },	// LTF
			{ +0.5, -0.5, -0.5 },	// RBB
			{ +0.5, -0.5, +0.5 },	// RBF
			{ +0.5, +0.5, -0.5 },	// RTB
			{ +0.5, +0.5, +0.5 } };	// RTF


GLint gCubeFaceNormalLookup[NUM_BLOCK_SIDES][3] = {
			{-1, 0, 0},		// BLOCK_SIDE_LEF
			{+1, 0, 0},		// BLOCK_SIDE_RIG
			{0, +1, 0},		// BLOCK_SIDE_TOP
			{0, -1, 0},		// BLOCK_SIDE_BOT
			{0, 0, +1},		// BLOCK_SIDE_FRO
			{0, 0, -1} };	// BLOCK_SIDE_BAC



v3di_t gBlockNeighborAddLookup[NUM_BLOCK_SIDES] = {
			{-1, 0, 0},		// BLOCK_SIDE_LEF
			{+1, 0, 0},		// BLOCK_SIDE_RIG
			{0, +1, 0},		// BLOCK_SIDE_TOP
			{0, -1, 0},		// BLOCK_SIDE_BOT
			{0, 0, +1},		// BLOCK_SIDE_FRO
			{0, 0, -1} };	// BLOCK_SIDE_BAC

#include "BiomeMap.h"


BiomeMap::BiomeMap() {
}

BiomeMap::~BiomeMap() {
}

void BiomeMap::randomize(PseudoRandom prng, int typesW, int typesH) {
	mTypesW = typesW;
	mTypesH = typesH;
	mInterval1 = 1.0 / typesW;
	mInterval2 = 1.0 / typesH;


	mTerrain1.resize(1024);
	mTerrain1.generateTilable(64.0, prng);
	mTerrain1.normalize(0.0, 1.0);

	mTerrain2.resize(1024);
	mTerrain2.generateTilable(64.0, prng);
	mTerrain2.normalize(0.0, 1.0);

}

BiomeInfo BiomeMap::getBiomeInfo(int i, int j) const {

	double iScale = 0.75;
	double jScale = 0.75;

//	double a = mTerrain1.get_value(i, j);
//	double b = mTerrain2.get_value(i, j);
	double a = mTerrain1.getValueBilerp((double)i * iScale, (double)j * jScale);
	double b = mTerrain2.getValueBilerp((double)i * iScale, (double)j * jScale);

	int aa = min(floor(a / mInterval1), mTypesW - 1);
	int bb = min(floor(b / mInterval2), mTypesH - 1);

	double cutoff = 0.05;

	double x = (a - ((double)aa * mInterval1)) / mInterval1;
	if (x > 0.5) {
		x = 1.0 - x;
	}

	x = 2.0 * x;

	if (x < cutoff) {
		x = x / cutoff;
//		x = 0.0;
	}
	else {
		x = 1.0;
	}


	double y = (b - ((double)bb * mInterval2)) / mInterval2;
	if (y > 0.5) {
		y = 1.0 - y;
	}

	y = 2.0 * y;

	if (y < cutoff) {
		y = y / cutoff;
//		y = 0.0;
	}
	else {
		y = 1.0;
	}

	BiomeInfo ret;
	ret.type = aa + (bb * mTypesW);
	ret.value = x * y;

	return ret;
}

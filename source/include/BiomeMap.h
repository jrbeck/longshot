// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * BiomeMap
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "PseudoRandom.h"
#include "Terrain.h"

struct BiomeInfo {
	int type;
	float value;
};


class BiomeMap {
public:
	BiomeMap();
	~BiomeMap();

	void randomize(PseudoRandom prng, int typesW, int typesH);

	BiomeInfo getBiomeInfo(int x, int z) const;

private:
	Terrain mTerrain1;
	Terrain mTerrain2;

	int mTypesW;
	int mTypesH;

	double mInterval1;
	double mInterval2;
};


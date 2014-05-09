// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Inventory
// *
// * this is as inventory container for a player.
// * it contains info about equipped gear as well as backpack inventory.
// * the items are just handles to items contained in an ItemManager
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

#include "AssetManager.h"


class Inventory {
public:
	Inventory();
	~Inventory();

	void clear(void);

	void resizeBackpack(int newSize);

	int getNextFreeBackpackSlot(void);

	bool swapBackPackItemIntoPrimary(void);
	bool swapBackPackItemIntoSecondary(void);

	void nextBackPackItem(void);
	void previousBackPackItem(void);

	void save(FILE *file);
	void load(FILE *file);


// MEMBERS * * * * * * * * * * * *
	size_t mCredits;

	vector<size_t> mBackpack;
	
	size_t mPrimaryItem;
	size_t mSecondaryItem;

	size_t mHeadGear;
	size_t mFootGear;
	size_t mLegGear;
	size_t mTorsoGear;

	size_t mAmmoCounter[NUM_AMMO_TYPES];

	size_t mSelectedBackpackItem;
};

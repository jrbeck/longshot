// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * RogueViewer
// *
// * this allows the user to view landscapes/features/rogue dungeons...
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef RogueViewer_h_
#define RogueViewer_h_


#include "AssetManager.h"
#include "feature/FeatureGenerator.h"
#include "World.h"
#include "WorldMapView.h"

#include "v2d.h"

#include "RtsCam.h"


struct CameraState {
	v3d_t position;
	v3d_t target;
};



enum {
	VIZ_MODE_MODEL,
	VIZ_MODE_ROGUE,

	NUM_VIZ_MODES,

	VIZ_MODE_PLANET
};


class RogueViewer {
private:
	rts_cam_c mRtsCam;

	SDL_Event sdlevent;
	v2d_t mMousePos;
	v2d_t mMouseDelta;
	int mMouseMoved;

	int mVizMode;

	CameraState mCameraStates[NUM_VIZ_MODES];

	World mWorld;
	WorldMap mWorldMap;
	WorldMapView mWorldMapView;
	AssetManager mAssetManager;
	RogueMap mRogueMap;

public:
	RogueViewer (void);
	~RogueViewer (void);

	void setupOpenGl (void);

	int start (void);

	v3d_t findStartPosition (WorldMap &worldMap);

	// TODO: get this i/o junk outta here
	int handleInput (void);
	int handleKeystroke (void);
	int handleKeyup (void);
	void handleMouseButtonDown (int button, v2d_t pos);
	void handleMouseButtonUp (int button, v2d_t pos);

	void swapVizMode (int mode);
	void generateNewMap (void);
};





#endif // RogueViewer_h_




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * SoundSystem
// *
// * this is in interface the SDL_mixer library
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef SoundSystem_h_
#define SoundSystem_h_


#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>



enum {
	SOUND_AMBIENT,
	SOUND_GUNSHOT,
	SOUND_MACHINE_GUN,
	SOUND_SHOTGUN,
	SOUND_PLASMA_GUN,
	SOUND_PLASMA_PISTOL,
	SOUND_BLOOP,
	SOUND_SPLAT,
	SOUND_GRENADE_EXPLOSION,
	SOUND_GRENADE_LAUNCH,
	SOUND_FLAME_THROWER,
	SOUND_TIGER_ROAR,
	SOUND_ALIEN_ROAR,
	SOUND_HUMAN_PAIN,
	SOUND_HUMAN_JUMP,
	SOUND_HUMAN_DEATH,
	SOUND_FOOTSTEP,
	SOUND_SPLASH_MEDIUM,
	SOUND_HEALTHPACK,
	SOUND_PISTOL,
	SOUND_PISTOL_RELOAD,
	SOUND_SWOOSH,
	SOUND_PUNCH,

	NUM_SOUNDS
};





class SoundSystem {
public:
	SoundSystem ();
	~SoundSystem ();

	int initialize (void);

	void playSoundByHandle (int handle, int volume);

	int loadSound (char *fileName);

	void stopAllSounds (void);

// members * * * * * * * * * * * * * * * * * *
	int mAudioRate;
	Uint16 mAudioFormat;
	int mAudioChannels;
	int mAudioBuffers;

	int mLastHandle;

	Mix_Chunk *mWav[NUM_SOUNDS];
};



#endif SoundSystem_h_

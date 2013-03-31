#include "SoundSystem.h"


SoundSystem::SoundSystem () {
	mAudioRate = 44100;
	mAudioFormat = AUDIO_S16; 
	mAudioChannels = 2;
	mAudioBuffers = 1024;

	for (int i = 0; i < NUM_SOUNDS; i++) {
		mWav[i] = NULL;
	}
}


SoundSystem::~SoundSystem() {
	printf("SoundSystem::~SoundSystem: unloading sounds (%d)\n", NUM_SOUNDS);
	for (int i = 0; i < NUM_SOUNDS; i++) {
		if (mWav[i] != NULL) {
			Mix_FreeChunk (mWav[i]);
		}
	}
}


int SoundSystem::initialize (void) {
	if (Mix_OpenAudio (mAudioRate, mAudioFormat, mAudioChannels, mAudioBuffers)) {
		printf("SoundSystem::initialize(): Mix_OpenAudio() failed\n");

		Mix_AllocateChannels( NUM_CHANNELS );

		return 1;
	}

	return 0;

}


void SoundSystem::playSoundByHandle (int handle, int volume) {
	if (mWav[handle] != NULL) {
		int channel;

		if (handle == SOUND_AMBIENT) {
			channel = Mix_PlayChannel (-1, mWav[handle], -1);
		}
		else {
			channel = Mix_PlayChannel (-1, mWav[handle], 0);
		}

		if (channel != -1) {
			Mix_Volume (channel, volume);
		}
		else {
		}
	}
	else {
		printf ("SoundSystem::playSoundByHandle: invalid sample!\n");
	}
}


int SoundSystem::loadSound( char *fileName ) {
	printf("SoundSystem::loadSound(): loading sounds (%d)\n", NUM_SOUNDS);
//	mWav[SOUND_AMBIENT] = Mix_LoadWAV ("sounds/not by john/ambient.ogg");
	mWav[SOUND_PISTOL] = Mix_LoadWAV ("sounds/tgc/gun_pistol.ogg");
	mWav[SOUND_MACHINE_GUN] = Mix_LoadWAV ("sounds/jb/machine_gun.ogg");
	mWav[SOUND_SHOTGUN] = Mix_LoadWAV ("sounds/jb/shotgun.ogg");
	mWav[SOUND_PLASMA_GUN] = Mix_LoadWAV ("sounds/jb/plasma_gun.ogg");
	mWav[SOUND_PLASMA_PISTOL] = Mix_LoadWAV ("sounds/jb/plasma_pistol.ogg");
	mWav[SOUND_GRENADE_EXPLOSION] = Mix_LoadWAV ("sounds/tgc/explosion_small.ogg");
	mWav[SOUND_GRENADE_LAUNCH] = Mix_LoadWAV ("sounds/tgc/gun_launcher.ogg");

	// REPLACE
	mWav[SOUND_SPLAT] = Mix_LoadWAV ("sounds/not by john/splat.ogg");
	mWav[SOUND_TIGER_ROAR] = Mix_LoadWAV ("sounds/not by john/roar.ogg");
	mWav[SOUND_HUMAN_PAIN] = Mix_LoadWAV ("sounds/not by john/human_pain.ogg");
	// END REPLACE

	mWav[SOUND_HUMAN_JUMP] = Mix_LoadWAV ("sounds/tgc/human_jump.ogg");
	mWav[SOUND_HUMAN_DEATH] = Mix_LoadWAV ("sounds/tgc/human_death.ogg");
	mWav[SOUND_FOOTSTEP] = Mix_LoadWAV ("sounds/jb/footstep.ogg");

	// REPLACE
	mWav[SOUND_SPLASH_MEDIUM] = Mix_LoadWAV ("sounds/not by john/splash_medium.ogg");
	// END REPLACE

	mWav[SOUND_HEALTHPACK] = Mix_LoadWAV ("sounds/jb/healthpack.ogg");
	mWav[SOUND_PISTOL_RELOAD] = Mix_LoadWAV ("sounds/tgc/gun_reload.ogg");
	mWav[SOUND_SWOOSH] = Mix_LoadWAV ("sounds/tgc/swoosh.ogg");
	mWav[SOUND_PUNCH] = Mix_LoadWAV ("sounds/tgc/punch.ogg");


	for (int i = 0; i < NUM_SOUNDS; i++) {
		if (mWav[i] != NULL) {
			Mix_VolumeChunk( mWav[i], 127 );
		}
	}

	return 0;
}


void SoundSystem::stopAllSounds (void) {
	Mix_HaltChannel (-1);
}



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

		Mix_AllocateChannels(32);

		return 1;
	}

	return 0;

}


void SoundSystem::playSoundByHandle (int handle, int volume) {
	if (mWav[handle] != NULL) {
		int channel;

//		if (handle == SOUND_MACHINE_GUN) {
//			printf ("machine gun!\n");
//			return;
//		}
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


int SoundSystem::loadSound (char *fileName) {
	printf("SoundSystem::loadSound(): loading sounds (%d)\n", NUM_SOUNDS);
//	mWav[SOUND_AMBIENT] = Mix_LoadWAV ("sounds/not by john/ambient.ogg");
	mWav[SOUND_AMBIENT] = Mix_LoadWAV ("sounds/not by john/plasma_gun.ogg");
	mWav[SOUND_GUNSHOT] = Mix_LoadWAV ("sounds/not by john/gunshot.ogg");
	mWav[SOUND_MACHINE_GUN] = Mix_LoadWAV ("sounds/not by john/machine_gun.ogg");
	mWav[SOUND_SHOTGUN] = Mix_LoadWAV ("sounds/not by john/shotgun.ogg");
	mWav[SOUND_PLASMA_GUN] = Mix_LoadWAV ("sounds/by john/plasma_gun.ogg");
	mWav[SOUND_PLASMA_PISTOL] = Mix_LoadWAV ("sounds/by john/plasma_pistol.ogg");
	mWav[SOUND_BLOOP] = Mix_LoadWAV ("sounds/not by john/bloop.ogg");
	mWav[SOUND_SPLAT] = Mix_LoadWAV ("sounds/not by john/splat.ogg");
	mWav[SOUND_GRENADE_EXPLOSION] = Mix_LoadWAV ("sounds/not by john/grenade.ogg");
	mWav[SOUND_GRENADE_LAUNCH] = Mix_LoadWAV ("sounds/not by john/grenade_launch.ogg");
	mWav[SOUND_FLAME_THROWER] = Mix_LoadWAV ("sounds/not by john/flame_thrower.ogg");
	mWav[SOUND_TIGER_ROAR] = Mix_LoadWAV ("sounds/not by john/roar.ogg");
	mWav[SOUND_ALIEN_ROAR] = Mix_LoadWAV ("sounds/not by john/alien_roar.ogg");
	mWav[SOUND_HUMAN_PAIN] = Mix_LoadWAV ("sounds/not by john/human_pain.ogg");
	mWav[SOUND_HUMAN_JUMP] = Mix_LoadWAV ("sounds/not by john/human_jump.ogg");
	mWav[SOUND_HUMAN_DEATH] = Mix_LoadWAV ("sounds/not by john/human_death.ogg");
	mWav[SOUND_FOOTSTEP] = Mix_LoadWAV ("sounds/not by john/footstep.ogg");
	mWav[SOUND_SPLASH_MEDIUM] = Mix_LoadWAV ("sounds/not by john/splash_medium.ogg");
	mWav[SOUND_HEALTHPACK] = Mix_LoadWAV ("sounds/not by john/healthpack.ogg");
	mWav[SOUND_PISTOL] = Mix_LoadWAV ("sounds/not by john/pistol.ogg");
	mWav[SOUND_PISTOL_RELOAD] = Mix_LoadWAV ("sounds/not by john/pistol_reload.ogg");
	mWav[SOUND_SWOOSH] = Mix_LoadWAV ("sounds/not by john/swoosh.ogg");
	mWav[SOUND_PUNCH] = Mix_LoadWAV ("sounds/not by john/punch.ogg");


	for (int i = 0; i < NUM_SOUNDS; i++) {
		if (mWav[i] != NULL) {
			Mix_VolumeChunk (mWav[i], 127);
		}
	}

	return 0;
}


void SoundSystem::stopAllSounds (void) {
	Mix_HaltChannel (-1);
}



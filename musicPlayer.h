#ifndef _PLAYER_
#define _PLAYER_
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <alloca.h> // needed for mixer
#include <string.h>
typedef float SAMPLE;
/* STRUCTS */
typedef struct
{
    int offset; /* Index into sample array. */
    long int numSamples;
    short *file;
    int sampleRate;
    int numChannels;
} call_back_data;

void musicPlayer_initialize();
void musicPlayer_terminate(void);
void musicPlayer_playSong(char *filePath);
void musicPlayer_playPause(void);
void musicPlayer_nextSong(void);
int musicPlayer_getVolume();
void musicPlayer_setVolume(int newVolume);
bool musicPlayer_isPlaying();

#endif

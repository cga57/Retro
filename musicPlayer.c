#include "portaudio.h"
#include "musicPlayer.h"
#include "wavParser.h"

#define ENGINE "beatbox-wav-files/345558__inspectorj__car-engine-exterior-a.wav"
#define I_RAN_SO_FAR_AWAY "beatbox-wav-files/i_ran_so_far_away-flock_of_seagulls.wav"
#define TOTO "beatbox-wav-files/africa-toto.wav"
#define DONT_SPEAK "beatbox-wav-files/dont_speak-no_doubt.wav"
#define TEMPLE_OF_LOVE "beatbox-wav-files/temple_of_love-sisters_of_mercy.wav"
#define MAMBO_NO_5 "beatbox-wav-files/mambo_no_5-lou_bega.wav"

static call_back_data audioData;
static PaStream *stream = NULL;
static bool isPaused = false;
static bool endPlaying = false;
static pthread_t playingAudioThread;
int song = 0;
#define AUDIOMIXER_MAX_VOLUME 100
#define DEFAULT_VOLUME 80
static int volume = 0;

// Initialize the PA library and open a stream for audio I/O
static int patestCallback(const void *input,
                          void *output,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData)
{
    call_back_data *audio = (call_back_data *)userData;
    unsigned long framesleft = audio->numSamples - audio->offset;

   // memset(output, 0, sizeof(float) * frameCount * audio->numChannels); // clear output buffer

    (void) input; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;

    short *rptr = &audio->file[audio->offset];
    short *wptr = (short*)output;

    /*  if frameselft is less than frames per buffer, last time to write to output buffer*/
    if (framesleft < framesPerBuffer)
    {
        for(int i = 0; i < framesPerBuffer; i ++ ){
            if (i < framesleft){
                *wptr++ = *rptr++;
                if(audio->numChannels >=2){
                    *wptr++ = *rptr++;
                }
            } else{
                *wptr++ = 0;
                if(audio->numChannels >=2){
                    *wptr++ = *rptr++;
                }
            }
        }
        audio->offset+= framesleft;
        return paComplete;
    }else{
        for(int i = 0; i < framesPerBuffer; i ++ ){
            *wptr++ = *rptr++;
            if(audio->numChannels >=2){
                *wptr++ = *rptr++;
            }
        }
        audio->offset+= framesPerBuffer;
        return paContinue;
    }
    
}

// Initialize the PA library.
// In your callback you can read audio data from the inputBuffer and/or write data to the outputBuffer.
void initializePortAudio()
{
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("Failue to Start PortAudio .PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

// terminate the library the PA library
void closePortAudio()
{
    PaError err = Pa_Terminate();
    if (err != paNoError)
    {
        printf("Failue to Close PortAudio. PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

// open a stream for audio I/O
void openStream()
{
    /* Open an audio I/O stream. */
    PaError err = Pa_OpenDefaultStream(&stream,
                                       0,         /* no input channels */
                                       audioData.numChannels,         /* stereo output */
                                       paInt16, /* 32 bit floating point output */
                                       audioData.sampleRate,
                                       paFramesPerBufferUnspecified, /* frames per buffer, i.e. the number
                                                           of sample frames that PortAudio will
                                                           request from the callback. Many apps
                                                           may want to use
                                                           paFramesPerBufferUnspecified, which
                                                           tells PortAudio to pick the best,
                                                           possibly changing, buffer size.*/
                                       patestCallback,               /* this is your callback function */
                                       &audioData);                  /*This is a pointer that will be passed to
                                                                          your callback*/
    if (err != paNoError)
    {
        printf("Opening DefaultStreamFailed.PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

// Start the stream. Your callback function will be now be called repeatedly by PA in the background.
void startPlaying()
{
    if(!Pa_IsStreamActive(stream)){
        PaError err = Pa_StartStream(stream);
        if (err != paNoError)
        {
            printf("Starting Stream failed. PortAudio error: %s\n", Pa_GetErrorText(err));
        }
        isPaused = false;
    }
  
}

// Stop the stream  by calling a stop function.
void stopPlaying()
{
    if(!Pa_IsStreamStopped(stream)){
        PaError err = Pa_StopStream(stream);
        if (err != paNoError)
        {
            printf("Starting Stream failed. PortAudio error: %s\n", Pa_GetErrorText(err));
        }
        isPaused = true;
    }
}

// Close the stream
void closeStream()
{
    PaError err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        printf("Failue to Close stream. PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

// thread function that will continue to play songs
void *musicPlayer_PlayingAudio(void *_arg)
{
    while (!endPlaying)
    {
        Pa_Sleep(100);
        if(!Pa_IsStreamActive(stream) && !Pa_IsStreamStopped(stream)){
            musicPlayer_nextSong();
        }
    }
    pthread_exit(NULL);
}

// initialize thread
void musicPlayer_initialize(void)
{   
    musicPlayer_setVolume(DEFAULT_VOLUME);
    initializePortAudio();
    musicPlayer_playSong(ENGINE);
    pthread_create(&playingAudioThread, NULL, musicPlayer_PlayingAudio, NULL);
}

// cleaning up thread
void musicPlayer_terminate(void)
{
    endPlaying = true;
    pthread_join(playingAudioThread, NULL);
    closeStream();
    closePortAudio();
    closeFile(&audioData);
}

// input path to wav file to be played
void musicPlayer_playSong(char *filePath)
{
    parseWaveFile(filePath, &audioData);
    openStream();
    isPaused = false;
    startPlaying();
}

void musicPlayer_nextSong()
{
    stopPlaying();
    closeStream();
    closeFile(&audioData);
    isPaused = false;
    switch (song)
    {
    case 0:
        musicPlayer_playSong(I_RAN_SO_FAR_AWAY);
        song++;
        break;
    case 1:
        musicPlayer_playSong(TOTO);
        song++;
        break;
    case 2:
        musicPlayer_playSong(DONT_SPEAK);
        song++;
        break;
    case 3:
        musicPlayer_playSong(TEMPLE_OF_LOVE);
        song++;
        break;
    
    case 4:
        musicPlayer_playSong(MAMBO_NO_5);
        song = 0;
        break;

    default:
        break;
    } 
}

void musicPlayer_playPause(void){
    if(!isPaused){
        isPaused = true;
        stopPlaying();
    } else{
        isPaused = false;
        startPlaying();
    }
}


// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void musicPlayer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *volHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}

bool musicPlayer_isPlaying(){
    return !isPaused;
}

int musicPlayer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

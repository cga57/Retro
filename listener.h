#ifndef _LISTENER__H_
#define _LISTENER_H_

//This module spawns a thread that listens to port 12345 for incoming commands through UDP packets. It then sends back response based off of those commands.

#include <pthread.h>
// Begin/end the background thread which samples light levels.
void Listener_startListening();

void Listener_stopListening(void);


void Listener_interpretResponse();

//increase volume
void Listener_increaseVolume(void);

//decrease volume
void Listener_decreaseVolume(void);

//update values
void Listener_update();

//play next song
void Listener_nextSong();

//pause
void Listener_pausePlay();



#endif
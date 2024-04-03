#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// Begin/end the background thread which samples light levels.
void Joystick_init(void);

void Joystick_stop(void);

void Joystick_play_pattern(void);


#endif
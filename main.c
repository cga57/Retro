
#include <signal.h>
#include <stdbool.h>
#include "SeeedGrayOLED.h"
#include <stdio.h>
#include <stdlib.h>
#include "udpSocketInterface.h"
#include "musicPlayer.h"
#include "joystick.h"
#include <string.h>
#include "helper.h"
#include "gps.h"
#include "screen.h"
#include "sharedMem-Linux.h"
#include "listener.h"

int main()
{
    musicPlayer_initialize();
    Listener_startListening();
    SharedMemory_init();
    Screen_init();
    GPS_init();
    Joystick_init();
    Listener_stopListening();
    Joystick_stop();
    GPS_shutdown();
    Screen_shutdown();
    SharedMemory_shutdown();
    musicPlayer_terminate();

    return 0;
}

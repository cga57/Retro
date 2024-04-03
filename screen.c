#include <signal.h>
#include <stdbool.h>
#include "SeeedGrayOLED.h"
#include <stdio.h>
#include <string.h>
#include "helper.h"
#include "gps.h"
#include "unistd.h"
#include <pthread.h>
#include "sharedMem-Linux.h"
#include "PRU/sharedDataStructure/sharedDataStruct_PRU0.h"
#include "musicPlayer.h"
#include "sharedMem-Linux.h"

static void animation();

static pthread_t screenThreadId;
int screenFlag = 1; // Set to 0 when done

static void animation()
{
    init();
    clearDisplay();
    setVerticalMode();
    setTextXY(5, 3);
    setGrayLevel(10);
    putString("Arrow");

    setTextXY(7, 3);
    setGrayLevel(10);
    putString("----->");

    Helper_sleepForMs(1000);
    clearDisplay();
}

static void setLightingMode(double speed)
{

    if (SharedMemory_isReversed())
    {
        if (SharedMemory_getDistance() < 7)
        {
            SharedMemory_setLightMode(TOO_CLOSE_REVERSED);
        }
        else
        {
            SharedMemory_setLightMode(REVERSED);
        }
    }
    else
    {
        if (speed >= 0 && speed < 20)
        {
            SharedMemory_setLightMode(ECO);
        }
        else if (speed >= 20 && speed < 40)
        {
            SharedMemory_setLightMode(SPORTS);
        }
        else if (speed >= 40 && speed < 70)
        {
            SharedMemory_setLightMode(CHANGING);
        }
    }
}
static void playRunning(){
    // > play
    // || play
    if(musicPlayer_isPlaying()){
        setTextXY(0, 2);
        setGrayLevel(10);
        putString("playing");
    } else{
        setTextXY(0, 2);
        setGrayLevel(10);
        putString("pausing");
    }
}

static void *runScreen()
{
    animation(); // runs animation on screen
    while (screenFlag)
    {
        // clearDisplay();
        double speed = GPS_getSpeedInKmph();
        setLightingMode(speed);
        char strSpeed[20];
        sprintf(strSpeed, "%.2f", speed);
        printf("%s", strSpeed);
        char result[50];
        strcpy(result, "V: ");    // copy the prefix string into the result string
        strcat(result, strSpeed); // concatenate the suffix string to the result string
        setVerticalMode();
        setTextXY(5, 0);
        setGrayLevel(5);
        putString(result);
        playRunning();

    }

    return NULL;
}
void Screen_init()
{
    initI2cBus("/dev/i2c-2", 0x3C);
    pthread_create(&screenThreadId, NULL, &runScreen, NULL);
}
void Screen_shutdown()
{
    screenFlag = 0;
    pthread_join(screenThreadId, NULL);
}
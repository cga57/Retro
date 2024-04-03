#include "joystick.h"
#define CONFIG_PINS "config-pin p8.14 gpio && config-pin p8.15 gpio && config-pin p8.16 gpio && config-pin p8.18 gpio && config-pin p8.17 gpio"
#define EXPORT "/sys/class/gpio/export"
#define UNEXPORT "/sys/class/gpio/unexport"
#define UP 26
#define RIGHT 47
#define DOWN 46
#define LEFT 65
#define PUSHED 27
#define UP_PATH "/sys/class/gpio/gpio26/value"
#define RIGHT_PATH "/sys/class/gpio/gpio47/value"
#define DOWN_PATH "/sys/class/gpio/gpio46/value"
#define LEFT_PATH "/sys/class/gpio/gpio65/value"
#define PUSHED_PATH "/sys/class/gpio/gpio27/value"
#include "musicPlayer.h"
#include "sharedMem-Linux.h"
static bool endJoystick = false;
static pthread_t joystickThread;

static void runCommand(char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// static void writePin(char *path, int pinNumber)
// {
//     // Use fopen() to open the file for write access.
//     FILE *pFile = fopen(path, "w");
//     if (pFile == NULL)
//     {
//         printf("ERROR: Unable to open export file.\n");
//         exit(1);
//     }
//     // Write to data to the file using fprintf():
//     fprintf(pFile, "%d", pinNumber);
//     // Close the file using fclose():
//     fclose(pFile);
// }

bool readFromFileToScreen(char *fileName)
{
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL)
    {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    return buff[0] == '0';
}

// make program sleep in ms
static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

void configPins(void)
{
    runCommand(CONFIG_PINS);
    // writePin(UNEXPORT, UP);
    // writePin(UNEXPORT, RIGHT);
    // writePin(UNEXPORT, LEFT);
    // writePin(UNEXPORT, DOWN);
    // writePin(UNEXPORT, PUSHED);
    // writePin(EXPORT, UP);
    // writePin(EXPORT, RIGHT);
    // writePin(EXPORT, LEFT);
    // writePin(EXPORT, DOWN);
    // writePin(EXPORT, PUSHED);
    // sleepForMs(600);
    runCommand("cd /sys/class/gpio/gpio26 && echo in > direction");
    runCommand("cd /sys/class/gpio/gpio47 && echo in > direction");
    runCommand("cd /sys/class/gpio/gpio46 && echo in > direction");
    runCommand("cd /sys/class/gpio/gpio65 && echo in > direction");
    runCommand("cd /sys/class/gpio/gpio27 && echo in > direction");
}

void *Joystick_start(void *_arg)
{
    while (!endJoystick)
    {
        if (readFromFileToScreen(UP_PATH))
        {
            musicPlayer_setVolume(musicPlayer_getVolume() + 5);
            sleepForMs(200);
        }
        if (readFromFileToScreen(RIGHT_PATH))
        {
            SharedMemory_toggleReversed();
            sleepForMs(200);
        }
        if (readFromFileToScreen(DOWN_PATH))
        {
            musicPlayer_setVolume(musicPlayer_getVolume() - 5);
            sleepForMs(200);
        }
        if (readFromFileToScreen(LEFT_PATH))
        {
            musicPlayer_nextSong();
            sleepForMs(200);
        }
        if (readFromFileToScreen(PUSHED_PATH))
        {
            musicPlayer_playPause();
            sleepForMs(200);
        }

        sleepForMs(10);
    }
    pthread_exit(NULL);
}
void Joystick_init(void)
{

    configPins();

    pthread_create(&joystickThread, NULL, Joystick_start, NULL);
}

void Joystick_stop(void)
{
    endJoystick = true;
    pthread_join(joystickThread, NULL);
}

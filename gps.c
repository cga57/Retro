#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include "helper.h"
#include <pthread.h>
#include <unistd.h>
#include "gps.h"

#define SERIAL_PORT "/dev/ttyS2"
#define BAUDRATE B9600
#define CRTSCTS 020000000000 /* flow control */

static double latitude, longitude;
static double speed;
char **result = NULL;

// these are just funciton definitions so that c knows that these functions exist
static void parse_gpgga(char *line);
static void parse_gprmc(char *line);
static void split(char *s, const char *delim);

// flag to check direction
// if isWest is true, then longitude is negative
// if isSouth is true, then latitude

static pthread_t gpsThreadId;
int gpsFlag = 1; // Set to 0 when done

static void split(char *s, const char *delim)
{
    int count = 0;
    char *copy = strdup(s); // create a copy of s to avoid modifying the original string

    // count the number of tokens
    for (char *token = strtok(copy, delim); token; token = strtok(NULL, delim))
    {
        count++;
    }

    // allocate memory for the array of strings
    result = malloc(sizeof(char *) * (count + 1));
    if (!result)
    {
        free(copy); // free the copy of the string
        return;
    }

    // split the string into tokens and store them in the array
    int i = 0;
    for (char *token = strtok(s, delim); token; token = strtok(NULL, delim))
    {
        // printf("%s\n", token);
        result[i] = strdup(token); // allocate memory for each token
        // printf("%s", result[i]);
        i++;
        // printf("i: %d\n", i);
        // printf("---------------------------------\n");
    }
    result[i] = NULL; // terminate the array with a NULL pointer

    free(copy); // free the copy of the string
    // return result;
}

double GPS_getSpeedInKmph()
{
    return speed * MILES_PER_KILOMETER;
}
double GPS_getLat()
{
    return latitude;
}
double GPS_getLong()
{
    return longitude;
}
static void *runGPS()
{
    Helper_runCommand("config-pin P9.21 uart");
    Helper_runCommand("config-pin P9.22 uart");

    int fd;
    struct termios options;
    char buf[255];

    // Open serial port
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        printf("Error: Unable to open serial port\n");
        // return -1;
        exit(-1);
    }

    // Configure serial port
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN] = 5;
    options.c_cc[VTIME] = 2;
    tcsetattr(fd, TCSANOW, &options);
    memset(buf, 0, sizeof(buf));

    while (gpsFlag)
    {
        int bytesRead = read(fd, buf, sizeof(buf));
        while (bytesRead == 0)
        {
            bytesRead = read(fd, buf, sizeof(buf));
        }

        split(buf, "$");

        if (result != NULL)
        {
            for (int i = 0; result[i] != NULL; i++)
            {
                if (strncmp(result[i], "GPGGA", 5) == 0)
                {
                    parse_gpgga(result[i]);
                    // latitude and longitude should go to the front-end (node server)
                    // printf("Latitude: %lf\nLongitude: %lf\n", latitude, longitude);
                }
                if (strncmp(result[i], "GPRMC", 5) == 0)
                {
                    // printf("%s", result[i]);
                    parse_gprmc(result[i]);
                    // we need to send this speed to the oled screen

                    // printf("Speed %lf\n", speed);
                }
                printf("---------------------------------\n");
            }

            // printf("%s", result[1]);
            // parse_gprmc(result[2]);
            // printf("Speed %lf\n", speed);

            free(result);
            result = NULL;
        }

        // printf("---------------------------------\n");
        Helper_sleepForMs(1000); // currently we sleep for 1 second every sample, we might want to decrease this to like 200ms
        memset(buf, 0, sizeof(buf));
    }

    // strcpy(line, "$GPGGA,203754.210,4911.3261,N,12250.9984,W,1,04,1.89,167.7,M,-16.7,M,,*57");
    // parse_gpgga(line);
    // printf("Latitude: %lf\nLongitude: %lf\n", latitude, longitude);

    return NULL;
}

void GPS_init()
{
    pthread_create(&gpsThreadId, NULL, &runGPS, NULL);
}
void GPS_shutdown()
{
    gpsFlag = 0;
    pthread_join(gpsThreadId, NULL);
}
// velocity
static void parse_gprmc(char *line)
{
    char *token = strtok(line, ",");
    int token_count = 0;

    while (token != NULL)
    {
        if (token_count == 7)
        {
            speed = atof(token);
            break;
        }
        token = strtok(NULL, ",");
        token_count++;
    }
}

static void parse_gpgga(char *line)
{
    char *token;
    int count = 0;
    const char *delimiter = ",";
    token = strtok(line, delimiter);
    while (token != NULL)
    {
        count++;
        // if (count == 1 && strcmp(token, "GPGGA") != 0)
        // {
        //     printf("Invalid line\n");
        //     return;
        // }
        if (count == 3)
        {
            double degrees = atof(token) / 100;
            double minutes = atof(token) - (degrees * 100);
            latitude = degrees + (minutes / 60);
        }
        if(count == 4){
            if(!strcmp(token, "S")){
                latitude *= -1;
            }
        }
        if (count == 5)
        {
            double degrees = atof(token) / 100;
            double minutes = atof(token) - (degrees * 100);
            longitude = degrees + (minutes / 60);
        }
        if(count == 6){
            if(!strcmp(token, "W")){
                longitude*=-1;
            }
        }
        token = strtok(NULL, delimiter);
    }
}
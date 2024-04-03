// *
//  * UDP Listening program
//  * By Brian Fraser, Modified from Linux Programming Unleashed (book). Modified by Param Manahs
//  *
//  * Usage:
//  *	On the target, run this program (netListenTest).
//  *	On the host:
//  *		> netcat -u netcat -u 192.168.7.2 12345
//  *		(Change the IP address to your board)
//  *

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <stdbool.h>
#include "listener.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h>
#include "musicPlayer.h"
#include "gps.h"
#include "sharedMem-Linux.h"

#define MSG_MAX_LEN 1500
#define PORT 12345
static pthread_t ListenerThread;
static bool endListening = false;
static struct sockaddr_in sinRemote;
static int socketDescriptor;
static unsigned int sin_len;
static char messageTx[MSG_MAX_LEN];
static char messageRx[MSG_MAX_LEN];
//code from this module was adapted from https://opencoursehub.cs.sfu.ca/bfraser/solutions/433/06-linuxprogramming/demo_udpListen.c and bits from https://opencoursehub.cs.sfu.ca/bfraser/solutions/433/06-linuxprogramming/demo_pipe.c



void *Listener_Listening(void* _arg)
{   
    // Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network sho

    // Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));	

    while(!endListening){
     // Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		sin_len = sizeof(sinRemote);
		// Pass buffer size - 1 for max # bytes so room for the null (string data)
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN - 1, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Check for errors (-1)

		// Make it null terminated (so string functions work)
		// - recvfrom given max size - 1, so there is always room for the null
		messageRx[bytesRx] = 0;
		// Extract the value from the message:
		// (process the message any way your app requires).
		

		// Compose the reply message:
		// (NOTE: watch for buffer overflows!).
		Listener_interpretResponse();

        // if (strncmp(messageRx, "stop", strlen("stop")) == 0) {
        //     snprintf(messageTx,MSG_MAX_LEN,"Program terminating.\n");
        //     // Transmit a reply:
        //     sin_len = sizeof(sinRemote);
        //     sendto( socketDescriptor,
        //         messageTx, strlen(messageTx),
        //         0,
        //         (struct sockaddr *) &sinRemote, sin_len);
        // break;
        // }
    }
    close(socketDescriptor);
    pthread_exit(NULL);
}

void Listener_startListening()
{

    pthread_create(&ListenerThread, NULL, Listener_Listening,NULL);
}

void Listener_stopListening(void)
{
    pthread_join(ListenerThread, NULL);
}
//Exit the program.
void Listener_stop(void)
{
    printf("stopped listening\n");
    snprintf(messageTx,MSG_MAX_LEN,"Program terminating.\n");
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
    endListening = true;
}

void Listener_increaseVolume(void)
{
    musicPlayer_setVolume(musicPlayer_getVolume() + 5);
    snprintf(messageTx,MSG_MAX_LEN,"%d-v",musicPlayer_getVolume());
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
}

void Listener_decreaseVolume(void)
{
    musicPlayer_setVolume(musicPlayer_getVolume() - 5);
    snprintf(messageTx,MSG_MAX_LEN,"%d-v",musicPlayer_getVolume());
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
}

void Listener_update()
{
    snprintf(messageTx,MSG_MAX_LEN,"update,%d,%f,%f,%d",musicPlayer_getVolume(),GPS_getLat(),GPS_getLong(),SharedMemory_getLightMode());
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
}


void Listener_nextSong()
{
    musicPlayer_nextSong();
    snprintf(messageTx,MSG_MAX_LEN,"skipping to next song");
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
}


void Listener_pausePlay()
{
    musicPlayer_playPause();
    snprintf(messageTx,MSG_MAX_LEN,"song has been paused or played");
    // Transmit a reply:
    sin_len = sizeof(sinRemote);
    sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
}



void Listener_interpretResponse(){
    if(strncmp(messageRx, "stop", strlen("stop")) == 0){
        Listener_stop();
    } else if(strncmp(messageRx, "-volume", strlen("-volume")) == 0){
        Listener_decreaseVolume();
    }else if(strncmp(messageRx, "+volume", strlen("+volume")) == 0){
        Listener_increaseVolume();
    }else if(strncmp(messageRx, "update", strlen("update")) == 0){
        Listener_update();
    }else if(strncmp(messageRx, "pause/play", strlen("pause/play")) == 0){
        Listener_pausePlay();
        Listener_nextSong();
    }else{
        snprintf(messageTx,MSG_MAX_LEN,"Unknown command\n");
        // Transmit a reply:
        sin_len = sizeof(sinRemote);
        sendto(socketDescriptor,messageTx, strlen(messageTx),0,(struct sockaddr *) &sinRemote, sin_len);
    }
}


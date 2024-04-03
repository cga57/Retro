#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "gps.h"
#include "udpSocketInterface.h"

static struct sockaddr_in sinX;
static int socketDescriptor;

struct sockaddr_in sinRemote;
static unsigned int sin_len;
static pthread_t listenThread;
int udpFlag = 1;

static void *readData()
{
	while (udpFlag)
	{
		double latitide = GPS_getLat();
		double longitude = GPS_getLong();
		char messageTx[50];
		sprintf(messageTx, "{\"lat\":%f,\"long\":%f}", latitide, longitude);
		if (socketDescriptor == -1)
		{
			printf("Uninitialized socket descriptor!\n");
		}

		// TODO: send in chunks

		sin_len = sizeof(sinRemote);
		sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote, sin_len);
	}
	return NULL;
}

void Udp_init()
{

	memset(&sinX, 0, sizeof(sinX));
	sinX.sin_family = AF_INET;
	sinX.sin_addr.s_addr = htonl(INADDR_ANY);
	sinX.sin_port = htons(PORT);

	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	if (socketDescriptor == -1)
	{
		printf("Error!\n");
	}

	if (bind(socketDescriptor, (struct sockaddr *)&sinX, sizeof(sinX)) == -1)
	{
		close(socketDescriptor);
		socketDescriptor = -1;
		printf("Error!\n");
	}
	pthread_create(&listenThread, NULL, &readData, NULL);
}

char *Udp_recv()
{
	sin_len = sizeof(sinRemote);
	char messageRx[MSG_MAX_LEN];
	if (socketDescriptor == -1)
	{
		printf("Uninitialized socket descriptor!\n");
	}
	int bytesRx = recvfrom(socketDescriptor,
						   messageRx, MSG_MAX_LEN - 1, 0,
						   (struct sockaddr *)&sinRemote, &sin_len);
	if (bytesRx == -1)
	{
		printf("Error recieving bytes!\n");
	}

	char *recievedMsg = malloc(bytesRx * sizeof(char));

	for (int i = 0; i < bytesRx; i++)
	{
		recievedMsg[i] = messageRx[i];
	}
	recievedMsg[bytesRx] = 0;

	return recievedMsg;
}

void Udp_send(char *message)
{
	if (socketDescriptor == -1)
	{
		printf("Uninitialized socket descriptor!\n");
	}

	// TODO: send in chunks

	sin_len = sizeof(sinRemote);
	sendto(socketDescriptor,
		   message, strlen(message),
		   0,
		   (struct sockaddr *)&sinRemote, sin_len);
}

void Udp_cleanup()
{
	udpFlag = 0;
	pthread_join(listenThread, NULL);
	if (socketDescriptor != -1)
	{
		close(socketDescriptor);
		socketDescriptor = -1;
	}
}

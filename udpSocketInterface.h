#ifndef _UDP_SOC_INTERFACE_
#define _UDP_SOC_INTERFACE_

#define MSG_MAX_LEN 1024
#define PORT 12345

// initializes the socket descriptor
void Udp_init();

// returns the received message
char* Udp_recv();

// sends the passed message
void Udp_send(char *message);

// cleanup
void Udp_cleanup();

#endif
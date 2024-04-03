/*
This Helper module includes some helper functions that are used across the program
*/

#ifndef _HELPER_H_
#define _HELPER_H_


// Puts the program to sleep for the specified milliseconds
void Helper_sleepForMs(long long delayInMs);


// Gets time in milli seconds
long long Helper_getTimeInMs(void);

// running command
void Helper_runCommand(char* command);

#endif
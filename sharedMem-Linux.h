#ifndef _SHARED
#define _SHARED_
void SharedMemory_init(void);
void SharedMemory_setLightMode(int num);
long double SharedMemory_getDistance();
void SharedMemory_shutdown();
int SharedMemory_getLightMode();

bool SharedMemory_isReversed();
void SharedMemory_toggleReversed();
#endif
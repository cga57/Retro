#ifndef _GPS_H_
#define _GPS_H_

#define MILES_PER_KILOMETER 1.60934

void GPS_init();
void GPS_shutdown();
double GPS_getSpeedInKmph();
double GPS_getLat();
double GPS_getLong();

#endif
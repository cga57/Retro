#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define allowInputTrig "config-pin p8.22 gpio" // GPIO 9_48
#define allowInputEcho "config-pin p8.21 gpio" // GPIO 9_49

#define A1raw_value "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define Trig_direction "/sys/class/gpio/gpio88/direction"
#define Echo_direction "/sys/class/gpio/gpio86/direction"
#define Echo_active_low "/sys/class/gpio/gpio86/active_low"
#define Trig_value "/sys/class/gpio/gpio88/value"
#define Echo_value "/sys/class/gpio/gpio86/value"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8

#endif
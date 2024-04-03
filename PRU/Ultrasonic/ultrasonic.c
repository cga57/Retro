/*
    NeoPixel RGBW demo program for 8 LED unit such as:
        https://www.adafruit.com/product/2868
    NOTE: This is RGBW, not RGB!

    Install process
    - Put the NeoPixel into a breadboard
    - Connect the NeoPixel with 3 wires:
        - Connect NeoPixel "GND" and "DIN" (data in) to the 3-pin "LEDS" header on Zen
            Zen Cape's LEDS header:
                Pin 1: DIN (Data): left most pin; beside USB-micro connection, connects to P8.11
                Pin 2: GND (Ground): middle pin
                Pin 3: Unused (it's "5V external power", which is not powered normally on the BBG)
        - Connect NeoPixel "5VDC" to P9.7 or P9.8
            Suggest using the header-extender to make it easier to make a good connection.
        - OK to wire directly to BBG: no level shifter required.
    - Software Setup
        - On Host
            make      # on parent folder to copy to NFS
        - On Target:
            config-pin P8.27 pruin
            config-pin P8.28 pruout
            make
            make install_PRU1
    - All lights should light up on the LED strip

    Based on code from the PRU Cookbook by Mark A. Yoder:
        https://beagleboard.org/static/prucookbook/#_setting_neopixels_to_different_colors
*/

#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "../sharedDataStructure/sharedDataStruct_PRU1.h"

static void fill_colors_array(int color);

#define oneCycleinNs = 140
#define oneCyclesOn 40000 // Stay on 700ns
#define COLOR_CHANGE_DELAY 200000000

// P8_28 for output (on R30), PRU1
#define DATA_PIN_MASK (1 << 10) // Bit number to output on
#define ECHO_RIGHT_MASK (1 << 8)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200          // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct1_t *pSharedMemStruct = (volatile void *)THIS_PRU_DRAM_USABLE;

void main(void)
{
    // // Initialize:
    pSharedMemStruct->distance = 1;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
    // long double distance_in_cm;
    // Initialize the shared memory
    while (1)
    {
        __R30 |= DATA_PIN_MASK; // Set the GPIO pin to 1
        // writeIntToFile(Trig_value, 1);

        __delay_cycles(3000);
        // sleepForMs(0.01);

        __R30 &= ~(DATA_PIN_MASK); // Clear the GPIO pin
                                   // writeIntToFile(Trig_value, 0);

        // long double start_time = 0;
        // long double end_time = 0;
        // long double length_of_time = 0;
        // long double distance_in_cm = 0;

        long long i = 0;

        // pSharedMemStruct->distance = 300;
        while ((__R31 & ECHO_RIGHT_MASK) == 0) // ascii 0
        {
            i = 0;
        }

        // pSharedMemStruct->distance = 400;
        while ((__R31 & ECHO_RIGHT_MASK) == ECHO_RIGHT_MASK) // ascii 1
        {
            i++;
        }

        long double time = i * 0.0005;

        // pSharedMemStruct->distance = 500;
        // length_of_time = end_time - start_time;
        // distance_in_cm = length_of_time * 0.000017150; // convert to cm
        // if (distance_in_cm <= 0)
        // {
        //     distance_in_cm = 0;
        // }

        pSharedMemStruct->distance = time;
    }

    __halt();
}

// unsigned long long getTimeInNs()
// {
//     unsigned long long result = 0;
//     struct timespec spec;
//     clock_gettime(CLOCK_MONOTONIC, &spec);
//     result = (unsigned long long)(spec.tv_sec) * 1000000000 + (unsigned long long)(spec.tv_nsec);
//     return result;
// }

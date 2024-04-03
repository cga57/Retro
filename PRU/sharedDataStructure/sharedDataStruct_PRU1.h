#ifndef _SHARED_DATA_STRUCT_PRU_1_H_
#define _SHARED_DATA_STRUCT__PRU_1_H_

#include <stdbool.h>
#include <stdint.h>
// My Shared Memory Structure
// ----------------------------------------------------------------
typedef struct
{
    // 1 byte enum (1 byte aligned)
    long double distance;

} sharedMemStruct1_t;

#endif

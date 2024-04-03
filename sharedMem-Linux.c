#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedMem-Linux.h"
#include "PRU/sharedDataStructure/sharedDataStruct_PRU0.h"
#include "PRU/sharedDataStructure/sharedDataStruct_PRU1.h"

static volatile void *getPruMmapAddr(void);
static void freePruMmapAddr(volatile void *PruBase);
static bool isReversed;

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000  // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ((base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ((base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ((base) + PRU_SHAREDMEM)

static volatile sharedMemStruct_t *pSharedPru0;
static volatile sharedMemStruct1_t *pSharedPru1;
static volatile void *pPruBase;

// Return the address of the PRU's base memory
static volatile void *getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED)
    {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

static void freePruMmapAddr(volatile void *PruBase)
{
    if (munmap((void *)PruBase, PRU_LEN))
    {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void SharedMemory_init(void)
{
    printf("Sharing memory with PRU\n");
    // Get access to shared memory for my uses
    pPruBase = getPruMmapAddr();

    pSharedPru1 = PRU1_MEM_FROM_BASE(pPruBase);
    pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    isReversed = false;
    printf("    %15s: %Lf\n", "Distance", pSharedPru1->distance);
    printf("    %15s: %d\n", "SharedMemory_LightMode", pSharedPru0->myMode);
}
void SharedMemory_setLightMode(int num)
{
    switch (num)
    {
    case SPORTS:
        pSharedPru0->myMode = SPORTS;
        break;
    case ECO:
        pSharedPru0->myMode = ECO;
        break;
    case CHANGING:
        pSharedPru0->myMode = CHANGING;
        break;
    case REVERSED:
        pSharedPru0->myMode = REVERSED;
        break;
    case TOO_CLOSE_REVERSED:
        pSharedPru0->myMode = TOO_CLOSE_REVERSED;
        break;
    default:
        break;
    }
}
int SharedMemory_getLightMode(){
    return pSharedPru0->myMode;
}

long double SharedMemory_getDistance()
{
    return pSharedPru1->distance;
}

void SharedMemory_shutdown()
{
    freePruMmapAddr(pPruBase);
}

bool SharedMemory_isReversed()
{
    return isReversed;
}

void SharedMemory_toggleReversed()
{
    isReversed = !isReversed;
}
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../utils/utils.h"

#define MD5_LENGTH 32
#define PID_LENGTH 1000
#define MAX_SIZE_BUFF 1024
#define MAX_RESULT_LENGTH (MD5_LENGTH + MAX_SIZE_BUFF + PID_LENGTH + PID_LENGTH + 140)
#define ATR_SEM_NAME_PREFIX 21
#define RTC_SEM_NAME_PREFIX 19

typedef struct sharedMemoryInfoCDT * sharedMemoryInfoADT;

sharedMemoryInfoADT openSharedMemory(pid_t pid, unsigned int amount_of_files, int mode);
void closeSharedMemory(sharedMemoryInfoADT shm);

unsigned int writeOnSharedMemory(sharedMemoryInfoADT shm, char * buff);
unsigned int readOnSharedMemory(sharedMemoryInfoADT shm, char * buff);

void postRWSem(sharedMemoryInfoADT shm);
void waitRWSem(sharedMemoryInfoADT shm);

void postCloseSHMSem(sharedMemoryInfoADT shm);
void waitCloseSHMSem(sharedMemoryInfoADT shm);

char * getName(sharedMemoryInfoADT shm);
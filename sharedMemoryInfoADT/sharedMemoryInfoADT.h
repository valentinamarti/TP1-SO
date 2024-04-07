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
#define MAX_RESULT_LENGTH (MD5_LENGTH + MAX_SIZE_BUFF + PID_LENGTH + PID_LENGTH + 40)

typedef struct sharedMemoryInfoCDT * sharedMemoryInfoADT;

sharedMemoryInfoADT openSharedMemory(pid_t pid, size_t length, int mode);
void closeSharedMemory(sharedMemoryInfoADT shm);

unsigned int writeOnSharedMemory(sharedMemoryInfoADT shm, char * buff);
unsigned int readOnSharedMemory(sharedMemoryInfoADT shm, char * buff);

char * getName(sharedMemoryInfoADT shm);
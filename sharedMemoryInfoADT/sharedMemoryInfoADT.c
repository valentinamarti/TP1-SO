#include "sharedMemoryInfoADT.h"
#include <bits/mman-map-flags-generic.h>

static void validateSemCreation(sem_t * sem);

typedef struct sharedMemoryInfoCDT {
    char name[10 + PID_LENGTH];
    int fd;
    char * mapping;
    size_t length;
    size_t index;
    sem_t * available_to_read;
    char available_to_read_sem_name[ATR_SEM_NAME_PREFIX + PID_LENGTH];
    sem_t * ready_to_close_shm;
    char ready_to_close_shm_sem_name[RTC_SEM_NAME_PREFIX + PID_LENGTH];
}sharedMemoryInfoCDT;

sharedMemoryInfoADT openSharedMemory(pid_t pid, unsigned int amount_of_files, int mode) {
    sharedMemoryInfoADT new_shm_info = malloc(sizeof(struct sharedMemoryInfoCDT));

    sprintf(new_shm_info->name, "sh_memory-%d", pid);

    new_shm_info->fd = shm_open(new_shm_info->name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    validate(new_shm_info->fd, CREATE_SH_MEMORY_ERROR_MSG);

    new_shm_info->length = amount_of_files * MAX_RESULT_LENGTH;
    ftruncate(new_shm_info->fd, (off_t) new_shm_info->length);

    new_shm_info->mapping = (char *) mmap(NULL, new_shm_info->length, mode, MAP_SHARED, new_shm_info->fd, 0);
    validate((*new_shm_info->mapping), CREATE_MAP_ERROR_MSG);

    new_shm_info->index = 0;

    sprintf(new_shm_info->available_to_read_sem_name, "available_to_read_sem-%d", pid);
    new_shm_info->available_to_read = sem_open(new_shm_info->available_to_read_sem_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    validateSemCreation(new_shm_info->available_to_read);

    sprintf(new_shm_info->ready_to_close_shm_sem_name, "ready_to_close_sem-%d", pid);
    new_shm_info->ready_to_close_shm = sem_open(new_shm_info->ready_to_close_shm_sem_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    validateSemCreation(new_shm_info->ready_to_close_shm);

    return new_shm_info;
}

void closeSharedMemory(sharedMemoryInfoADT shm) {
    shm_unlink(shm->name);
    close(shm->fd);
    sem_close(shm->available_to_read);
    sem_close(shm->ready_to_close_shm);
    free(shm);
}

unsigned int writeOnSharedMemory(sharedMemoryInfoADT shm, char * buff) {
    unsigned int written_chars = sprintf(&(shm->mapping[shm->index]), "%s", buff);
    sem_post(shm->available_to_read);
    shm->index += written_chars;
    return written_chars;
}

unsigned int readOnSharedMemory(sharedMemoryInfoADT shm, char * buff) {
     sem_wait(shm->available_to_read);
     unsigned int readBytes = sprintf(buff, "%s", &(shm->mapping[shm->index]));
     shm->index += readBytes;
     return readBytes;
}

char * getName(sharedMemoryInfoADT shm) {
    return shm->name;
}

void postRWSem(sharedMemoryInfoADT shm) {
    sem_post(shm->available_to_read);
}

void waitRWSem(sharedMemoryInfoADT shm) {
    sem_wait(shm->available_to_read);
}

static void validateSemCreation(sem_t * sem) {
    if (sem == SEM_FAILED) {
        perror(CREATE_SEM_ERROR_MSG);
        exit(errno);
    }
}

void postCloseSHMSem(sharedMemoryInfoADT shm) {
    sem_post(shm->ready_to_close_shm);
}

void waitCloseSHMSem(sharedMemoryInfoADT shm) {
    sem_wait(shm->ready_to_close_shm);
}

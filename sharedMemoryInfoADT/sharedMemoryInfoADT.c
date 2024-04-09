#include "sharedMemoryInfoADT.h"
#include <bits/mman-map-flags-generic.h>

typedef struct sharedMemoryInfoCDT {
    char name[10 + PID_LENGTH];
    int fd;
    char * mapping;
    size_t length;
    size_t index;
    sem_t * sem_mutex;
    char sem_name[4 + PID_LENGTH];
}sharedMemoryInfoCDT;

sharedMemoryInfoADT openSharedMemory(pid_t pid, size_t length, int mode) {
    sharedMemoryInfoADT new_shm_info = malloc(sizeof(struct sharedMemoryInfoCDT));

    sprintf(new_shm_info->name, "sh_memory-%d", pid);

    new_shm_info->fd = shm_open(new_shm_info->name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    validate(new_shm_info->fd, CREATE_SH_MEMORY_ERROR_MSG);

    new_shm_info->length = length;
    ftruncate(new_shm_info->fd, (off_t) new_shm_info->length);

    new_shm_info->mapping = (char *) mmap(NULL, new_shm_info->length, mode, MAP_SHARED, new_shm_info->fd, 0);
    validate((*new_shm_info->mapping), CREATE_MAP_ERROR_MSG);

    new_shm_info->index = 0;

    sprintf(new_shm_info->sem_name, "sem-%d", pid);
    new_shm_info->sem_mutex = sem_open(new_shm_info->sem_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (new_shm_info->sem_mutex == SEM_FAILED) {
        perror(CREATE_SEM_ERROR_MSG);
        exit(errno);
    }
    return new_shm_info;
}

void closeSharedMemory(sharedMemoryInfoADT shm) {
    shm_unlink(shm->name);
    close(shm->fd);
    sem_close(shm->sem_mutex);
    free(shm);
}

unsigned int writeOnSharedMemory(sharedMemoryInfoADT shm, char * buff) {
    //sem_wait(shm->sem_mutex);
    unsigned int written_chars = sprintf(&(shm->mapping[shm->index]), "%s", buff);
    sem_post(shm->sem_mutex);
    shm->index += written_chars;
    return written_chars;
}

unsigned int readOnSharedMemory(sharedMemoryInfoADT shm, char * buff) {
//    sem_wait(shm->sem_mutex);
//    int j = 0;
//    while (shm->mapping[shm->index] != '\n' && shm->mapping[shm->index] != '\0') {
//        buff[j++] = shm->mapping[shm->index];
//        shm->index++;
//    }
//    if (shm->mapping[shm->index] == '\n') {
//        buff[j++] = '\n';
//    }
//    buff[j] = '\0';
//    shm->index++;
//    return j;


     sem_wait(shm->sem_mutex);
     unsigned int readBytes = sprintf(buff, "%s", &(shm->mapping[shm->index]));
     shm->index += readBytes;
     return readBytes;
}

char * getName(sharedMemoryInfoADT shm) {
    return shm->name;
}

void postSem(sharedMemoryInfoADT shm) {
    sem_post(shm->sem_mutex);
}

void waitSem(sharedMemoryInfoADT shm) {
    sem_wait(shm->sem_mutex);
}

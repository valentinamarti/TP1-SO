#include "utils.h"

sem_t mutex;

void validate(int code, char * message) {
    if (code == -1) {
        perror(message);
        exit(errno);
    }
}

void * open_shm(int mode, int parent_pid, size_t * lenght, int * shm_fd, char *shm_name){
    char shm_new_name[10+PID_LENGTH];
    sprintf(shm_new_name, "shm_memory-%d", parent_pid);
    strcpy(shm_name, shm_new_name);

    *shm_fd = shm_open(shm_new_name, O_CREAT | O_RDWR, mode);
    validate(*shm_fd, CREATE_SH_MEMORY_ERROR_MSG);

    *lenght = (MAX_SIZE_BUFF + 1) * MAX_AMOUNT_FILES;

    ftruncate(*shm_fd, *lenght);

    void * addr = mmap(NULL, *lenght, mode, MAP_PRIVATE, *shm_fd, 0);
    validate((*(int*)addr), CREATE_MAP_ERROR_MSG);

    return addr;
}

void close_shm(int shm_fd, void * addr, size_t lenght) {
    munmap(addr, lenght);
    close(shm_fd);
}

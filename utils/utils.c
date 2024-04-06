#include "utils.h"

void validate(int code, char * message) {
    if (code == -1) {
        perror(message);
        exit(errno);
    }
}

void * open_shm(int mode, int parent_pid, int amount_of_files, size_t * lenght, int * sh_fd){
    char shm_name[10+PID_LENGTH];
    sprintf(shm_name, "sh_memory-%d", parent_pid);

    *sh_fd = shm_open(shm_name, O_CREAT | O_RDWR, mode);
    validate(*sh_fd, CREATE_SH_MEMORY_ERROR_MSG);

    *lenght = (MAX_LENGTH + 1) * amount_of_files;

    ftruncate(*sh_fd, *lenght);

    void * addr = mmap(NULL, *lenght, mode, MAP_PRIVATE, *sh_fd, 0);
    validate((*(int*)addr), CREATE_MAP_ERROR_MSG);

    return addr;
}

void close_shm(int sh_fd, void * addr, size_t lenght){
    munmap(addr, lenght);
    close(sh_fd);
}

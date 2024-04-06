#ifndef UTILS_H
#define UTILS_H

#define PIPE_ERROR_MSG "ERROR: error when creating pipe."
#define FORK_ERROR_MSG "ERROR: error when forking."
#define EXECVE_ERROR_MSG "ERROR: error when executing child."
#define WAITPID_ERROR_MSG "ERROR: error when waiting for child process."
#define SELECT_ERROR_MSG "ERROR: error when executing select."
#define READ_ERROR_MSG "ERROR: error when reading"
#define CREATE_SH_MEMORY_ERROR_MSG "ERROR: error when creating shared memory."
#define CREATE_MAP_ERROR_MSG "ERROR: error when creating map."
#define MD5_LENGTH 32
#define PID_LENGTH 1000
#define MAX_SIZE_BUFF 1024
#define MAX_RESULT_LENGTH (MD5_LENGTH + MAX_SIZE_BUFF + PID_LENGTH + PID_LENGTH + 40)
#define MAX_AMOUNT_FILES 1000


#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void validate(int code, char * message);

void * open_shm(int mode, int parent_pid, size_t * lenght, int * shm_fd, char *shm_name);
void close_shm(int shm_fd, void * addr, size_t lenght);

#endif
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
#define PATH_LENGTH 1024
#define PID_LENGTH 1000
#define MAX_LENGTH (MD5_LENGTH + PATH_LENGTH + PID_LENGTH + PID_LENGTH + 40)

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void validate(int code, char * message);

void * open_shm(int mode, int parent_pid, int amount_of_files, size_t * lenght, int * sh_fd);

#endif
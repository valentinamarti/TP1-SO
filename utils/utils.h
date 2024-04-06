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
#define CREATE_SEM_ERROR_MSG "ERROR: error when creating semaphore."

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

#endif
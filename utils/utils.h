#ifndef UTILS_H
#define UTILS_H

#define PIPE_ERROR_MSG "ERROR: error when creating pipe"
#define FORK_ERROR_MSG "ERROR: error when forking"
#define EXECVE_ERROR_MSG "ERROR: error when executing child"
#define WAITPID_ERROR_MSG "ERROR: error when waiting for child process"
#define MD5_LENGTH 32
#define PATH_LENGTH 1024
#define PID_LENGTH 1000
#define MAX_LENGTH (MD5_LENGTH + PATH_LENGTH + PID_LENGTH + PID_LENGTH + 40)

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void validate(int code, char * message);

#endif
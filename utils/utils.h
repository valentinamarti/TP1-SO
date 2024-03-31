#ifndef UTILS_H
#define UTILS_H

#define PIPE_ERROR_MSG "ERROR: error when creating pipe"
#define FORK_ERROR_MSG "ERROR: error when forking"
#define EXECVE_ERROR_MSG "ERROR: error when executing child"
#define WAITPID_ERROR_MSG "ERROR: error when waiting for child process"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void validate(int code, char * message);

#endif
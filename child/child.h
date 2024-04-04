#ifndef CHILD_H
#define CHILD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../utils/utils.h"

#define PIPE_FILEDESCRIPTORS 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define STDOUT 1
#define STDIN 0
#define MAX_SIZE_BUFF 1024

void read_and_execute();
void validate(int code, char * message);

#endif
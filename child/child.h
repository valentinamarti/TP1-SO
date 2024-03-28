#ifndef CHILD_H
#define CHILD_H

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE_FILEDESCRIPTORS 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define STDOUT 1
#define STDIN 0

void read_and_execute();
void signal_handler(int sig);


#endif
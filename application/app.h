#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MIN_CHILDREN 5
#define INFLEX_POINT 50
#define PIPE_FILEDESCRIPTORS 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define STDOUT 1
#define STDIN 0

int get_children_amount(int amount_of_files);
void validate(int code, char * message);

#endif


#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/select.h>
#include "../utils/utils.h"

#define MIN_CHILDREN 5
#define INFLEX_POINT 50
#define PIPE_FILEDESCRIPTORS 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define STDOUT 1
#define STDIN 0
#define AMOUNT_OF_FILES_DISTRIBUTION 16
#define MIN_DISTRIBUTION 1

int get_children_amount(int amount_of_files, unsigned int * amount_to_send);
void validate(int code, char * message);

#endif


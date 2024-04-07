#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/select.h>
#include "../utils/utils.h"
#include "../sharedMemoryInfoADT/sharedMemoryInfoADT.h"

#define MIN_CHILDREN 5
#define INFLEX_POINT 50
#define PIPE_FILEDESCRIPTORS 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define STDOUT 1
#define STDIN 0
#define AMOUNT_OF_FILES_DISTRIBUTION 16
#define MIN_DISTRIBUTION 1
#define SLEEP_TIME 10
#define NEWLINE "\n"

unsigned int get_children_amount(unsigned int amount_of_files, unsigned int * amount_to_send);
void validate(int code, char * message);
void closePipes(int fds[], unsigned int amount_of_children);
void writeInPipe(int fd, char * buff);

#endif


#ifndef VIEW_H
#define VIEW_H

#include "../sharedMemoryInfoADT/sharedMemoryInfoADT.h"
#include "../utils/utils.h"

#define COMMAND_LENGTH 28
#define NUMERIC_BASE 10

int isProcessRunning(char * process_name);
void saveAppInfo(char * pid_aux, char * length_aux,  pid_t *app_pid, size_t *length);

#endif
#include "view.h"

int main(int argc, char * argv[]) {

    pid_t app_pid;
    size_t length;
    char buff[MAX_RESULT_LENGTH + 1];

    if(!isProcessRunning("appx") || (argc != 1 && argc != 3)){
        perror(INICIALIZE_VIEW_ERRIR_MSG);
        exit(errno);
    }else if(argc == 3){
        app_pid = (pid_t) strtol(argv[1], NULL, NUMERIC_BASE);
        length = (size_t) strtol(argv[2], NULL, NUMERIC_BASE);
    }else{
        char pid_aux[PID_LENGTH];
        char length_aux[MAX_RESULT_LENGTH];
        if (scanf("%s %s", pid_aux, length_aux) != 2) {
            perror(INICIALIZE_VIEW_ERRIR_MSG);
            exit(errno);
        }
        app_pid = (pid_t) strtol(pid_aux, NULL, NUMERIC_BASE);
        length = (size_t) strtol(length_aux, NULL, NUMERIC_BASE);
    }

    sharedMemoryInfoADT shm = openSharedMemory(app_pid, length, PROT_READ);

    unsigned int bytes_read = 1;
    while(bytes_read != 0){
        bytes_read += readOnSharedMemory(shm, buff);
      printf("%s", buff);
    }


    closeSharedMemory(shm);

}

int isProcessRunning(char * process_name){
    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "pgrep -x %s > /dev/null", process_name);
    return system(command) == 0;
}


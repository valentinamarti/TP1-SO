#include "view.h"

int main(int argc, char * argv[]) {

    pid_t app_pid;
    size_t amount_of_files;
    char buff[MAX_SIZE_BUFF + 1];

    if (!isProcessRunning("appx")|| (argc != 1 && argc != 3)) {
        perror(INITIALIZE_VIEW_ERROR_MSG);
        exit(errno);
    } else if(argc == 3) {
        app_pid = (pid_t) strtol(argv[1], NULL, NUMERIC_BASE);
        amount_of_files = (size_t) strtol(argv[2], NULL, NUMERIC_BASE);
    } else {
        char pid_aux[PID_LENGTH];
        char length_aux[MAX_RESULT_LENGTH];
        if (scanf("%s %s", pid_aux, length_aux) != 2) {
            perror(INITIALIZE_VIEW_ERROR_MSG);
            exit(errno);
        }
        app_pid = (pid_t) strtol(pid_aux, NULL, NUMERIC_BASE);
        amount_of_files = (size_t) strtol(length_aux, NULL, NUMERIC_BASE);
    }

    
    sharedMemoryInfoADT shm = openSharedMemory(app_pid, amount_of_files, PROT_READ);
    waitCloseSHMSem(shm);

    unsigned int bytes_read = 1;
    while(bytes_read != 0){
        bytes_read = readOnSharedMemory(shm, buff);
        printf("%s", buff);
    }
    postCloseSHMSem(shm);
    closeSharedMemory(shm);
}

int isProcessRunning(char * process_name){
    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "pidof %s > /dev/null", process_name);

    int status = system(command);

    if (status == -1) {
        perror("system");
        return 0;
    }

    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}


#include "child.h"

char flag = 1;

int main(int argc, char * argv[]) {

    while (flag) {
        signal(SIGUSR1, signal_handler);
        signal(SIGUSR2, signal_handler);
    }
    return 0;
}

void signal_handler(int sig) {
    switch (sig) {
        case SIGUSR1 : read_and_execute(); break;
        case SIGUSR2 : flag = 0; break;
        default : break;
    }
}

void read_and_execute() {
    pid_t pid;
    int status = 0;
    pid_t wpid;

    signed long linelen;
    char * line = NULL;
    size_t linecap = 0;

    int fd[PIPE_FILEDESCRIPTORS];

    if (pipe(fd) == -1) {
        perror("ERROR: error when creating pipe");
        exit(EXIT_FAILURE);
    }

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        pid = fork();
        if (pid < 0) {
            perror("ERROR: error when forking");
            exit(EXIT_FAILURE);
        } else {
            if (pid == 0) {
                char * md5_argv[] = {"~//usr/bin/md5sum", line, NULL};
                char * md5_env[] = {NULL};

                /*
                No sabemos como redireccionar la salida de md5 a un pipe 
                para poder obtener el resultado sin leerlo de stdin

                No podriamos usar stdin o stdout porque se pisaria con el canal de comunicacion con el 
                proceso padre
                */

                if(execve("~//usr/bin/md5sum", md5_argv, md5_env) == -1){
                    perror("ERROR: error when executing child");
                    exit(EXIT_FAILURE);
                }
            }
        }


    }
    while ( (wpid = waitpid(-1, &status, 0)) > 0);
    //kill(getppid(), SIGUSR1); // Aca le queremos mandar una señal al padre para decirle que ya termine
    return 0;
}



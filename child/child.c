#include "child.h"

char flag = 1;

int main(int argc, char * argv[]) {
    read_and_execute();
    return 0;
}


void read_and_execute() {
    pid_t pid;
    int status = 0;
    pid_t wpid;

    signed long linelen;
    char * line = NULL;
    size_t linecap = 0;

    int fd[PIPE_FILEDESCRIPTORS];

    validate(pipe(fd), PIPE_ERROR_MSG);

    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        pid = fork();
        validate(pid, FORK_ERROR_MSG);
        if (pid == 0) { // Child process
            char absolute_path[1024 + 1];
            realpath(line, absolute_path);               
            absolute_path[strlen(absolute_path) - 1] = '\0'; // realpath() copy in "absolute path", the absolute path of "line" with a \n at the end

            char * md5_argv[] = {"/usr/bin/md5sum", absolute_path, NULL};
            char * md5_env[] = {NULL};
            
            validate(execve("/usr/bin/md5sum", md5_argv, md5_env), EXECVE_ERROR_MSG);
        }
    }
    while ( (wpid = waitpid(-1, &status, 0)) > 0);
    return;
}

